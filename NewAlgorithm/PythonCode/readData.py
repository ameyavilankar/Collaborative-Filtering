import numpy as np

UMatrix = np.zeros((6040, 100))
VMatrix = np.zeros((100, 100))

SingularValues = np.loadtxt('Outputsingular_values')
SingularMatrix = np.zeros((100, 100))


for i in xrange(SingularMatrix.shape[0]):
	SingularMatrix[i][i] = SingularValues[i]


print "Singular: ", SingularValues.shape

for i in xrange(100):
	tempU = np.loadtxt('OutputU.' + str(i) + '_1_of_1')
	tempV = np.loadtxt('OutputV.' + str(i) + '_1_of_1')
	UMatrix[:,i] = tempU
	VMatrix[:,i] = tempV


print "UMatrix: ", UMatrix.shape
print "VMatrix: ", VMatrix.shape


output = np.dot(np.dot(UMatrix, SingularMatrix), VMatrix.T)

print "Output shape", output.shape

for i in xrange(output.shape[0]):
	print i, " ",
	for j in xrange(output.shape[1]):
		print output[i][j], " ",
	print ""
