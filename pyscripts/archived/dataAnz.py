import math,sys,getopt

"""filename = 'anotherDataCapture.txt'"""
filename = 'myPendData.txt' 
"""filename = 'defaultPendDataFile.txt'"""



file = open(filename,"rb")
block = file.read()

outStr = ''
shad = 0

count = 0
n = 0

print "Pendulum Shadow Time Data "

for ch in block:
	temp = ord(ch)
	n += 1
	if n == 1:
		if temp == 82:
			outStr += 'R '
		if temp == 70:
			outStr += 'F '
	if n == 2:
		shad += 16*16*temp
	if n == 3:
		shad += 16*temp
	if n == 4:
		shad += temp
	if n == 5:
		outStr += str( shad )
		shad = 0	 
	if n == 6:
		n = 0
		count += 1
		print str( count ) + ' : ' + outStr + '\n'
		outStr = ''
		
		
file.close()
