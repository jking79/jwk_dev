import matplotlib.pyplot as plt
import math
import numpy as np
##from operator import add  as listadd
##from operator import sub as listsub

def inverse( list ):
	result = []
	for x in list:
		result.append( 1/ float(x) )
	return result

def mymean(numlist):
	fnum = []
	for x in numlist:
		fnum.append( float(x) )
	return sum( fnum ) / len(numlist)

def listdiff( flist, slist ):
	temp =[]
	for x in range( 0, len(slist)):
		if x <= len(flist):
			temp.append( flist[x] - slist[x])
	return temp
	
def listsum( flist, slist ):
	temp =[]
	for x in range( 0, len(slist)):
		if x <= len(flist):
			print x
			temp.append( flist[x] - slist[x])
	return temp
	
def myvar(numlist, m):
	meanlist = []
	for x in numlist:
		meanlist.append( pow( ( float(x) - m ), 2 ) )
	return sum( meanlist ) / len( numlist )


def getrange( med, var, bin, s ):
	start = int(round( med - s*math.sqrt(var) ))
	stop = int(round( med + s*math.sqrt(var) ))
	return np.arange( start, stop + bin, bin)

fileName = 'defalutOutFile'

yRawDataFile = fileName + '.dat'
ySavePlotName = fileName + '.png'

out = []
lr =[]
lf=[]

infile = open( yRawDataFile, 'r')

line = infile.readline()
for line in infile.readlines():
	line = line.strip()
	out = line.split(',', 3)
	print out
	if out[1] == 'R':
		lr.append( int(out[2]) )
	if out[1] == 'F':
		lf.append( int(out[2]) )

d = 1 
# number of decimal places to display

print len(lr)
print len(lf)


mr = mymean( lr )
##rlist.append( mr )
vr = myvar( lr, mr )
##vrlist.append(vr)
mf = mymean( lf )
##flist.append( mf )
vf = myvar( lf, mf )
##vflist.append(vf)

lsum =  listsum( lr, lf ) 
##[i + j for i, j in zip(lr, lf)] ##map( listadd, lr, lf )
ldif =  listdiff( lr, lf) 
##[i - j for i, j in zip(lr, lf)]  ##map( listsub, lr, lf )

print len(lsum)
print len(ldif)

mls = mymean( lsum )
##slist.append( mls )
vls = myvar( lsum, mls)
##vslist.append(vls)
mld = mymean( ldif )
##dlist.append( mld )
vld = myvar( ldif, mld )
##vdlist.append( vld )

binwidth = 1
d = 1 # number of decimal places to display

plt.figure(1)

plt.subplot(4,1,1)	
plt.hist(lr, bins=getrange( mr, vr, binwidth, 1))
plt.title("Histograms for " + yRawDataFile + " :Xscale 1.6 microsec") ##\n  Mean: " + str(mr) + "  stdev: " + str(math.sqrt(vr)))
plt.xlabel("Mean: " + str(round( mr, d)) + "  stdev: " + str(round( math.sqrt(vr), d)))
plt.ylabel("R data")


plt.subplot(4,1,2)
plt.hist(lf, bins=getrange( mf, vf, binwidth, 1))
plt.xlabel("Mean: " + str(round(mf,d)) + "  stdev: " + str(round(math.sqrt(vf), d)))
##plt.xlabel("1.6 micro seconds")
plt.ylabel("F data")

plt.subplot(4,1,3)
plt.hist(ldif, bins=getrange( mld, vld, binwidth, 1))
plt.xlabel("Mean: " + str(round(mld, d)) + "  stdev: " + str(round(math.sqrt(vld), d)))
##plt.xlabel("1.6 micro seconds")
plt.ylabel("Diffrence")

plt.subplot(4,1,4)
plt.hist(lsum, bins=getrange( mls, vls, binwidth, 3))
plt.xlabel("Mean: " + str(round(mls, d)) + "  stdev: " + str(round(math.sqrt(vls),d)))
##plt.xlabel("1.6 micro seconds")
plt.ylabel("Sum")

plt.subplots_adjust( hspace = 0.75 )

##plt.show()
plt.savefig( ySavePlotName, format = "png" )
plt.clf()