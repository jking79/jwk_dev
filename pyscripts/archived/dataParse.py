

fileName = raw_input( 'Enter Input File Name: ')
yRawDataFile = fileName + '.dat'

out = []
print yRawDataFile

for line in open( yRawDataFile, 'r').readlines():
	out = line.split(',', 3)
	print out
	print line




