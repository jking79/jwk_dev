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
		if x < len(flist):
			temp.append( flist[x] - slist[x])
	return temp
	
def listsqrt( list ):
	temp =[]
	for x in list:
		temp.append( math.sqrt(x) )
	return temp
	
def listsum( flist, slist ):
	temp =[]
	for x in range( 0, len(slist)):
		if x < len(flist):
			##print x
			temp.append( flist[x] - slist[x])
	return temp
	
def myvar(numlist, m):
	meanlist = []
	for x in numlist:
		meanlist.append( pow( ( float(x) - m ), 2 ) )
	return sum( meanlist ) / len( numlist )

def filter( list, mean, var, title ):
	temp = []
	for x in list:
		if x > ( mean - 10 * math.sqrt(var)):
			if x < ( mean + 10 * math.sqrt( var )):
				temp.append( x )
                else:
                    print str(x) + "\n"
	return temp
	

def getrange( med, var, bin, s ):
	start = int(round( med - s*math.sqrt(var) ))
	stop = int(round( med + s*math.sqrt(var) ))
	return np.arange( start, stop + bin, bin)


titleList = [ '10', '25', '50', '100', '250', 'Gen300', 'Gen310', 'Gen312', 'Gen313', 'Gen314', 'Gen315' ]
xlist = [ 10,25,50,100,250,300,310,312,313,314,315]
rlist = []
flist = []
slist = []
dlist = []
vrlist = []
vflist = []
vslist = []
vdlist = []


for entry in titleList:
	fileName = "dataSig" + entry + "Hz" ##raw_input( 'Enter Input File Name: ')
	yRawDataFile = fileName + '.dat'
	ySavePlotName = fileName + '_Filtered.png'

	out = []
	lr =[]
	lf=[]

	print yRawDataFile

	
	infile = open( yRawDataFile, 'r')
	line = infile.readline()
	for line in infile.readlines():
		line = line.strip()
		out = line.split(',', 3)

		if 'R' in line:
			lr.append( int(out[2]) )
		if 'F' in line:
			lf.append( int(out[2]) )

	lr[0] = lr[2]
	lf[0] = lf[2]
		
	mr = mymean( lr )
	##rlist.append( mr )
	vr = myvar( lr, mr )
	##vrlist.append(vr)
	mf = mymean( lf )
	##flist.append( mf )
	vf = myvar( lf, mf )
	##vflist.append(vf)

	lr = filter( lr, mr, vr )
	lf = filter ( lf, mf, vf )
	
	mr = mymean( lr )
	rlist.append( mr )
	vr = myvar( lr, mr )
	vrlist.append(vr)
	mf = mymean( lf )
	flist.append( mf )
	vf = myvar( lf, mf )
	vflist.append(vf)
	
	lsum =  listsum( lr, lf ) 
	##[i + j for i, j in zip(lr, lf)] ##map( listadd, lr, lf )
	ldif =  listdiff( lr, lf) 
	##[i - j for i, j in zip(lr, lf)]  ##map( listsub, lr, lf )

	mls = mymean( lsum )
	slist.append( mls )
	vls = myvar( lsum, mls)
	vslist.append(vls)
	mld = mymean( ldif )
	dlist.append( mld )
	vld = myvar( ldif, mld )
	vdlist.append( vld )

	binwidth = 1
	d = 1 # number of decimal places to display

	plt.figure(1)

	plt.subplot(4,1,1)	
	plt.hist(lr, bins=getrange( mr, vr, binwidth, 5))
	plt.title("Histograms for " + yRawDataFile + " :Xscale 1.6 microsec") ##\n  Mean: " + str(mr) + "  stdev: " + str(math.sqrt(vr)))
	plt.xlabel("Mean: " + str(round( mr, d)) + "  stdev: " + str(round( math.sqrt(vr), d)))
	plt.ylabel("R data")


	plt.subplot(4,1,2)
	plt.hist(lf, bins=getrange( mf, vf, binwidth, 5))
	plt.xlabel("Mean: " + str(round(mf,d)) + "  stdev: " + str(round(math.sqrt(vf), d)))
	##plt.xlabel("1.6 micro seconds")
	plt.ylabel("F data")

	plt.subplot(4,1,3)
	plt.hist(ldif, bins=getrange( mld, vld, binwidth, 5))
	plt.xlabel("Mean: " + str(round(mld, d)) + "  stdev: " + str(round(math.sqrt(vld), d)))
	##plt.xlabel("1.6 micro seconds")
	plt.ylabel("Diffrence")

	plt.subplot(4,1,4)
	plt.hist(lsum, bins=getrange( mls, vls, binwidth, 5))
	plt.xlabel("Mean: " + str(round(mls, d)) + "  stdev: " + str(round(math.sqrt(vls),d)))
	##plt.xlabel("1.6 micro seconds")
	plt.ylabel("Sum")

	plt.subplots_adjust( hspace = 0.75 )

	##plt.show()
	plt.savefig( ySavePlotName, format = "png" )
	plt.clf()

## meanv 1/f   sd v 1/f    diff vs 1/f   expected time v measured time
iflist = []
iflist = inverse(xlist)
##
print( 'mean vs freq')
plt.figure(1)
plt.plot( xlist, rlist, '-b', label = "Rise" )
plt.plot( xlist, flist, '-r', label = "Fall" )
plt.plot( xlist, slist, '-g', label = "Rise + Fall" )
##plt.plot( xlist, dlist, '-k', label = "Rise - Fall" )
plt.legend(loc='upper right')
plt.ylabel("1.6 microseconds" )
plt.xlabel("Frequency of SigGenerator") 
plt.title("Mean Time vs SigGen Frequency")
plt.savefig( "MeanFrequencyPlotFiltered.png", format = "png")
##plt.show()
print('rise, fall, combo')
plt.figure(2)
plt.plot( iflist, rlist, '-b', label = "Rise" )
plt.plot( iflist, flist, '-r', label = "Fall" )
plt.plot( iflist, slist, '-g', label = "Rise + Fall" )
##plt.plot( iflist, dlist, '-k', label = "Rise - Fall" )
plt.legend(loc='upper right')
plt.ylabel("1.6 microseconds" )
plt.xlabel("Period of SigGenerator") 
plt.title("Mean vs Period")
plt.savefig( "MeanPeriodPlotFiltered.png", format = "png")
##
print('Diffrence v period')
plt.figure(3)
plt.plot( iflist, dlist, '-k', label = "Rise - Fall" )
##plt.legend(loc='upper right')
plt.ylabel("1.6 microseconds" )
plt.xlabel("Period of SigGenerator") 
plt.title("Difference Mean vs SigGen Period")
plt.savefig( "DiffrencePlotFiltered.png", format = "png")
##
##print listsqrt(vrlist)
print('Stdev v period')
plt.figure(4)
plt.scatter( iflist, listsqrt(vrlist), '-b', label = "Rise" )
plt.plot( iflist, listsqrt(vflist), '-r', label = "Fall" )
plt.plot( iflist, listsqrt(vslist), '-g', label = "Rise + Fall" )
plt.plot( iflist, listsqrt(vdlist), '-k', label = "Rise - Fall" )
plt.legend(loc='upper right')
plt.ylabel("Standard Deviation: 1.6 microseconds" )
plt.xlabel("Period of SigGenerator")
plt.title("Standard Deiations vs SigGen Period")
plt.savefig( "StdDevPlotFiltered.png", format = "png")
