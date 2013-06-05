def ddf():
	pass


from math import sqrt


def euclidean_similarity(prefs, person1, person2):
	# Get the list of the commonly rated movies
	commonMovies = {}

	# Find the common movies and them to the dictionary
	for movie in prefs[person1]:
	for movie in prefs[person2]:
	  commonMovies[movie] = 1

	# if no common movies return no similarity
	if len(commonMovies) == 0:
	return 0

	# Calculate the sum of powers of the differences in movie ratings
	distance = sqrt(sum([pow((prefs[person1][movie] - prefs[person2][movie]), 2) for movie in commonMovies]))

	# convert the euclidean into similarity
	return 1/(1 + distance)

def pearson_similarity(prefs, person1, person2):
	# Get the list of the commonly rated movies
	commonMovies = {}

	# Find the common movies and add them to the dictionary
	for movie in prefs[person1]:
	for movie in prefs[person2]:
	  commonMovies[movie] = 1

	# if no common movies return no similarity
	if len(commonMovies) == 0:
	return 0

	# Find the sum of the movie ratings for the common movies
	sum1 = sum([prefs[person1][movie]] for movie in commonMovies)
	sum2 = sum([prefs[person2][movie]] for movie in commonMovies)  

	# Find the sum of the square of the movie ratings for the common movies
	sumsqr1 = sum([pow(prefs[person1][movie], 2) for movie in commonMovies])
	sumsqr2 = sum([pow(prefs[person2][movie], 2) for movie in commonMovies])

	# Find the sum of the products
	sumProducts = sum([prefs[person1][movie] * prefs[person2][movie] for movie in commonMovies])

	# Calculate Pearson score
	numerator = sumProducts - (sum1 * sum2/n)
	denominator = sqrt((sumsqr1 - pow(sum1, 2)/n) * (sumsqr2 - pow(sum2, 2)/n))

	if denominator == 0:
	return 0

	return numerator/denominator

def tanimoto (list1, list2):
	intersection = [common_item for common_item in list1 if common_item in list2]
	return float(len(c))/(len(a) + len(b) - len(c))

def topSimilarUsers(prefs, person, n = 10, similarity = pearson_similarity):
	# Calculate the similarity score of person with all the other persons in prefs except himself/herself
	scores = [(similarity(prefs, person, otherPerson), otherPerson) for otherPerson in prefs if otherPerson != person] 

	# sort the list of the persons according to the scores in descending order
	scores.sort()
	scores.reverse()

	# return the top n similar users
	return scores[0:n]

# returns all the users that have correlation greater than zero
def positiveCorrelationUsers(prefs, person):
	# Calculate the similarity score of person with all the other persons in prefs except himself/herself
	scores = [(similarity(prefs, person, otherPerson), otherPerson) for otherPerson in prefs if otherPerson != person] 

	# get only thos entries that have positive correlation
	indices = np.where(scores[:, 0] > 0.0)[0]
	scores = scores[indices]

	# sort the list of the persons according to the scores in descending order
	scores.sort()
	scores.reverse()

	return scores

# gets the recommendations for the person
def getRecommendations(prefs,person,similarity = sim_pearson):
	totals = {}
	simSums = {}
	
	for other in prefs:
		# don't compare me to myself
		if other == person: continue
		sim = similarity(prefs,person,other)
		
		# ignore scores of zero or lower
		if sim <= 0: continue
		for item in prefs[other]:
			# only score movies I haven't seen yet
			if item not in prefs[person] or prefs[person][item] == 0:
			# Similarity * Score
			totals.setdefault(item,0)
			totals[item] += prefs[other][item] * sim
			# Sum of similarities
			simSums.setdefault(item,0)
			simSums[item] += sim
	
	# Create the normalized list
	rankings=[(total/simSums[item],item) for item,total in totals.items()]
	
	# Return the sorted list
	rankings.sort()
	rankings.reverse()
	
	return rankings

# Convert from user to item-based clustering
def transformPrefs(prefs):
	result={}

	# Flip item and person
	for person in prefs:
		for item in prefs[person]:
			result.setdefault(item,{})
			result[item][person] = prefs[person][item]
	
	return result

def calculateSimilarItems(prefs,n = 10):
	# Create a dictionary of items showing which other items they are most similar to.
	similarItems = {}
	
	# Invert the preference matrix to be item-centric
	itemPrefs = transformPrefs(prefs)
	
	count = 0
	for item in itemPrefs:
		# Status updates for large datasets
		count = count + 1
		if count % 100 == 0: print "%d / %d" % (count,len(itemPrefs))
		
		# Find the most similar items to this one
		scores = topMatches(itemPrefs,item,n = n,similarity = sim_distance)
		similarItems[item]=scores

	return similarItems

def getRecommendedItems(prefs,itemMatch,user):
	userRatings=prefs[user]
	scores={}
	totalSim={}

	# Loop over items rated by this user
	for (item,rating) in userRatings.items():
		# Loop over items similar to this one
		for (similarity,item2) in itemMatch[item]:
			# Ignore if this user has already rated this item
			if item2 in userRatings: continue
			
			# Weighted sum of rating times similarity
			scores.setdefault(item2,0)
			scores[item2]+=similarity*rating
			
			# Sum of all the similarities
			totalSim.setdefault(item2,0)
			totalSim[item2]+=similarity

	# Divide each total score by total weighting to get an average
	rankings=[(score/totalSim[item],item) for item,score in scores.items()]
	
	# Return the rankings from highest to lowest
	rankings.sort( )
	rankings.reverse( )
	return rankings

def loadMovieLens(path='/data/movielens'):
	# Get movie titles
	movies={}
	for line in open(path+'/u.item'):
		(id,title)=line.split('|')[0:2]
		movies[id]=title
	
	# Load data
	prefs={}
	for line in open(path+'/u.data'):
		(user,movieid,rating,ts)=line.split('\t')
		prefs.setdefault(user,{})
		prefs[user][movies[movieid]]=float(rating)
	
	return prefs

