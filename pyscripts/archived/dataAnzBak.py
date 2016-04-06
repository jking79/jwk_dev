import math,sys,getopt

filename = 'myPendData.txt'
  
offset = 0
count = 0

outStr = ''

with open(filename,"rb") as f:
	block = f.read()
	for ch in block:
		count += 1
		outStr += str(ord(ch)) + " "
		if count == 6:
			print outStr + '\n'
			count = 0
			outStr = ''
f.close()
