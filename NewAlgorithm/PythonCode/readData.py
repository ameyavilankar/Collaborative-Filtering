import numpy as np

UMatrix = np.zeros((100, 6040))
VMatrix = np.zeros((100, 100))

SingularValues = np.loadtxt('singularvalues.txt')
print "Singular: ", SingularValues.shape

for i in xrange(100):
		UMatrix[i] = np.loadtxt('SVDOutput.txtU.' + str(i) + '_1_of_1')
		VMatrix[i] = np.loadtxt('SVDOutput.txtV.' + str(i) + '_1_of_1')

print "UMatrix: ", UMatrix.shape
print "VMatrix: ", VMatrix.shape

output = np.zeros((6040, 100))

for i in xrange(100):
	output = np.add(output, SingularValues[i] * np.outer(UMatrix[i], VMatrix[i]))

print "Output: ", output.shape

for i in xrange(output.shape[0]):
	for j in xrange(output.shape[1]):
		print output[i][j], " ",
	print ""

