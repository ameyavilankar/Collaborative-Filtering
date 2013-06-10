from math import sqrt
import json

def whatisthis(s):
    if isinstance(s, str):
	print "ordinary string"
    elif isinstance(s, unicode):
	print "unicode string"
    else:
	print "not a string"


def euclidean_similarity(prefs, person1, person2):
	# Get the list of the commonly rated movies
	commonMovies = {}

	# Find the common movies and them to the dictionary
	for movie in prefs[person1]:
		if movie in prefs[person2]:
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
		if movie in prefs[person2]:
			commonMovies[movie] = 1
	
	# if no common movies return no similarity
	if len(commonMovies) == 0:
		return 0
	
	# Find the sum of the movie ratings for the common movies
	sum1 = sum(prefs[person1][movie] for movie in commonMovies)
	sum2 = sum(prefs[person2][movie] for movie in commonMovies)  

	# Find the sum of the square of the movie ratings for the common movies
	sumsqr1 = sum([pow(prefs[person1][movie], 2) for movie in commonMovies])
	sumsqr2 = sum([pow(prefs[person2][movie], 2) for movie in commonMovies])

	# Find the sum of the products
	sumProducts = sum([prefs[person1][movie] * prefs[person2][movie] for movie in commonMovies])

	# Calculate Pearson score
	n = len(commonMovies)
	numerator = sumProducts - (sum1 * sum2/n)
	denominator = sqrt((sumsqr1 - pow(sum1, 2)/n) * (sumsqr2 - pow(sum2, 2)/n))

	if denominator == 0:
		return 0

	return numerator/denominator

def tanimoto (list1, list2):
	intersection = [common_item for common_item in list1 if common_item in list2]
	return float(len(c))/(len(a) + len(b) - len(c))

def topMatches(prefs, person, n = 10, similarity = pearson_similarity):
	# Calculate the similarity score of person/item with all the other persons/items in prefs except himself/herself/itself
	scores = [(similarity(prefs, person, otherPerson), otherPerson) for otherPerson in prefs if otherPerson != person] 

	# sort the list of the persons/items according to the scores in descending order
	scores.sort()
	scores.reverse()

	# return the top n similar users/items
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
def getRecommendations(prefs, person, similarity = pearson_similarity):
	totals = {}
	simSums = {}
	
	for otherPerson in prefs:
		# Dont compare person to self
		if otherPerson == person:
			continue

		# Calculate the similarity score
		similarityScore = similarity(prefs, person, otherPerson)
		
		# Ignore Similarity Scores of zero or lower
		if similarityScore <= 0:
			continue

		# If Similarity Score is greater than zero, use the person in the recommendation
		for movie in prefs[otherPerson]:
			# Only score movies person hasn't seen yet
			if movie not in prefs[person] or prefs[person][movie] == 0:
				# Calculate the sum of the rating given by otherPerson weighted by their similarity score
				totals.setdefault(movie, 0)
				totals[movie] += prefs[otherPerson][movie] * similarityScore

				# Calculate the sum of similarity scores to normalize the predicted rating
				simSums.setdefault(movie, 0)
				simSums[movie] += similarityScore
	
	# Create the normalized list by dividing the sum of weighted scores by sum of similarity scores
	rankings = [(total/simSums[movie], movie) for movie, total in totals.items()]
	
	# Return the sorted list
	rankings.sort()
	rankings.reverse()
	
	return rankings

# Convert from user-item dict to item-user dict
def transformPrefs(prefs):
	result = {}

	# Flip item and person
	for person in prefs:
		for item in prefs[person]:
			result.setdefault(item, {})
			result[item][person] = prefs[person][item]
	
	return result


def calculateSimilarMovies(prefs, n = 10):
	# Create a dictionary of items showing which other items they are most similar to.
	similarItems = {}
	
	# Invert the preference matrix to be item-centric
	itemPrefs = transformPrefs(prefs)
	
	count = 0
	for item in itemPrefs:
		# Status updates for large datasets
		count = count + 1
		if count % 100 == 0: print "%d / %d" % (count, len(itemPrefs))
		
		# Find the most similar items to this one
		scores = topMatches(itemPrefs, item, n = n, similarity = euclidean_similarity)
		similarItems[item] = scores

	return similarItems

def getRecommendedItems(prefs, itemMatch, user):
	userRatings = prefs[user]
	scores = {}
	totalSim = {}

	# Loop over items rated by this user
	for (item, rating) in userRatings.items():
		# Loop over items similar to this one
		for (similarity, item2) in itemMatch[item]:
			# Ignore if this user has already rated this item
			if item2 in userRatings:
				continue
			
			# Weighted sum of rating times similarity
			scores.setdefault(item2, 0)
			scores[item2] += similarity * rating
			
			# Sum of all the similarities
			totalSim.setdefault(item2, 0)
			totalSim[item2] += similarity

	# Divide each total score by total weighting to get an average
	rankings=[(score/totalSim[item],item) for item,score in scores.items()]
	
	# Return the rankings from highest to lowest
	rankings.sort()
	rankings.reverse()
	return rankings


def loadMovieLens():
	# Build a dictionary from the movies to their titles
	movies = {}
	for line in open('movies.dat'):
		(id,title)=line.split('::')[0:2]
		movies[id]=title
	
	# Build the dictionary from user to movie titles to their ratings
	prefs = {}
	for line in open('ratings.dat'):
		(user,movieid,rating, ts) = line.split('::')
		prefs.setdefault(user,{})
		prefs[user][movies[movieid]]=float(rating)
	
	return prefs

def saveToFile(filename, dict):
    f = open(filename, "w")
    for key, value in dict.iteritems():
	f.write(key + ": \n")
	for x in value:
	    f.write(x[1] + ": " + str(x[0]) + "\n")
	f.write("\n\n")
    f.close()

# main method
if __name__ == "__main__":
	# Load the preferences from the movieLens dataset
	print "Loading the dataset..."
	personToMovie = loadMovieLens()

	#----------------------------  USER BASED CF -----------------------------------#
	print "Starting User Based CF..."
	
	print "Getting the recommended items for each person..."
	# Get the recommended items for each person using the pearson similarity score
	recommendedItemsUserBased = {}
	for person in personToMovie:
		# print "Person ", person
		recommendedItemsUserBased.setdefault(person, [])
		recommendedItemsUserBased[person] =  getRecommendations(personToMovie, person)
    
	print "Getting the top similar users for each person..."
	# Get the top N similar users for each person using the pearson similarity score
	topSimilarUsers = {}
	for person in personToMovie:
		print "Person ", person
		topSimilarUsers.setdefault(person, [])
		topSimilarUsers[person] = topMatches(personToMovie, person)
	
	#-------------------------------------------------------------------------------#
		
	#-----------------------------  ITEM BASED CF ----------------------------------#
	print "Starting the Item Based CF..."

	print "Caclulating top similar items for each item"
	# Calculate for each item the top similar items to itself: Item-Item Matrix
	topSimilarMovies = calculateSimilarMovies(personToMovie)
	
	print "Calculating the recommendations for each user..."
	# Calculate the recommended items for each user using the item-item matrix
	recommendedItemsItemBased = {}
	for person in personToMovie:
		print "Person", person
		recommendedItemsItemBased.setdefault(person, [])
		recommendedItemsItemBased[person] = getRecommendedItems(personToMovie, topSimilarMovies, person)
	
	print "Saving to files..."
	print "Saving RecommendItemsUserBased..."
	saveToFile('RecommendedItemsUserBased.txt', recommendedItemsUserBased)

	print "Saving TopSimilarUsers..."
	saveToFile('TopSimilarUsers.txt', topSimilarUsers)
	
	print "Saving TopSimilar Items..."
	saveToFile('TopSimilarItems.txt', topSimilarMovies)
	
	print "Saving RecommendedItemsItemBased..."
	saveToFile('RecommendedItemsItemBased.txt', recommendedItemsItemBased)

	#-------------------------------------------------------------------------------#
	"""
	print "Saving to files..."
	print "Saving RecommendItemsUserBased..."
	f = open('RecommendedItemsUserBased.json', 'w')
	f.write(json.dumps(recommendedItemsUserBased, sort_keys=True, indent=4, separators=(',', ': ')))
	f.close()
	
	print "Saving TopSimilarUsers..."
	f = open('TopSimilarUsers.json', 'w')
	f.write(json.dumps(topSimilarUsers, sort_keys=True, indent=4, separators=(',', ': ')))
	f.close()
	
	print "Saving TopSimilar Items..."
	f = open('TopSimilarItems.json', 'w')
	f.write(json.dumps(topSimilarMovies, sort_keys=True, indent=4, separators=(',', ': ')))
	f.close()
	
	print "Saving RecommendedItemsItemBased..."
	f = open('RecommendedItemsItemBased.json', 'w')
	f.write(json.dumps(recommendedItemsItemBased, sort_keys=True, indent=4, separators=(',', ': ')))
	f.close()

	#-------------------------------------------------------------------------------#
	print "Done with both Collaborative Filtering Techniques..."
	print "Starting Evaluation..."
	# Evaluation Measures:??
	"""
