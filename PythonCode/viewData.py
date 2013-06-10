from math import sqrt

def readData(filename):
	# Open the file for reading data
    file = open(filename, "r")

    while 1:
        # Read all the lines from the file and store it in lines
        lines = file.readlines(1000000000000)

        # if Lines is empty, simply break
        if not lines:
            break

        # Create a list to hold all the data
        data = []
        userids = []
        
        print "Number of Lines: ", len(lines)
        for line in lines:
        	lineList = line.split("\n")[0].split(" ")
        	lineList.pop(len(lineList) - 1)
        	list1 = [float(j) for j in lineList]
        	userids.append(list1[0])
        	list1.pop(0)
        	# Add to the data
        	data.append(list1)

    print len(data), len(userids)
    return (userids, data)

def pearson(v1,v2):
	# Simple sums
	sum1=sum(v1)
	sum2=sum(v2)

	# Sums of the squares
	sum1Sq=sum([pow(v,2) for v in v1])
	sum2Sq=sum([pow(v,2) for v in v2])

	# Sum of the products
	pSum=sum([v1[i]*v2[i] for i in range(len(v1))])

	# Calculate r (Pearson score)
	num=pSum-(sum1*sum2/len(v1))
	den=sqrt((sum1Sq-pow(sum1,2)/len(v1))*(sum2Sq-pow(sum2,2)/len(v1)))

	if den==0:
		return 0
	
	return 1.0-num/den

def scaledown(data, distance = pearson, rate = 0.01):
	n = len(data)

	# The real distances between every pair of items
	realdist = [[distance(data[i], data[j]) for j in range(n)] for i in range(0, n)]
	outersum = 0.0
	
	# Randomly initialize the starting points of the locations in 2D
	loc = [[random.random(), random.random()] for i in range(n)]
	fakedist = [[0.0 for j in range(n)] for i in range(n)]
	
	lasterror = None
	for m in range(0, 1000):
		# Find projected distances
		for i in range(n):
			for j in range(n):
				fakedist[i][j] = sqrt(sum([pow(loc[i][x] - loc[j][x], 2) for x in range(len(loc[i]))]))

		# Move points
		grad = [[0.0, 0.0] for i in range(n)]

		totalerror = 0
		for k in range(n):
			for j in range(n):
				if j == k:
					continue

        		# The error is percent difference between the distances
				errorterm = (fakedist[j][k] - realdist[j][k])/realdist[j][k]

				# Each point needs to be moved away from or towards the other
				# point in proportion to how much error it has
				grad[k][0] += ((loc[k][0] - loc[j][0])/fakedist[j][k]) * errorterm
				grad[k][1] += ((loc[k][1] - loc[j][1])/fakedist[j][k]) * errorterm

				# Keep track of the total error
				totalerror += abs(errorterm)

		print totalerror

		# If the answer got worse by moving the points, we are done
		if lasterror and lasterror < totalerror:
			break

		lasterror = totalerror

		# Move each of the points by the learning rate times the gradient
		for k in range(n):
			loc[k][0] -= rate * grad[k][0]
			loc[k][1] -= rate * grad[k][1]
	
	return loc

def draw2d(data, labels, jpeg = 'mds2d.jpg'):
	img = Image.new('RGB', (2000, 2000), (255, 255, 255))
	draw = ImageDraw.Draw(img)
	
	for i in range(len(data)):
		x = (data[i][0] + 0.5) * 1000
		y = (data[i][1] + 0.5) * 1000
	
	draw.text((x, y),labels[i],(0, 0, 0))
	img.save(jpeg, 'JPEG')

if __name__ == "__main__":
	print "Loading Dataset..."
	(userids, data) = readData('ratings.txt')

	print "Calculating Locations..."
	location = scaledown(data)

	print "Drawing the Image..."
	draw2d(location, userids, jpeg='2DClusters.jpg')