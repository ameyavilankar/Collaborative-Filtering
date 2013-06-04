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

def topSimilarUsers(prefs, person, n = 10, similarity = pearson_similarity):
  # Calculate the similarity score of person with all the other persons in prefs except himself/herself
  scores = [(similarity(prefs, person, otherPerson), otherPerson) for otherPerson in prefs if otherPerson != person] 

  # sort the list of the persons according to the scores in descending order
  scores.sort()
  scores.reverse()

  # return the top n similar users
  return scores[0:n]

def 