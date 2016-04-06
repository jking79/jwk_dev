#/usr/bin/python

import serial, re, time
from Tkinter import *
import matplotlib
from numpy import arange, sin, pi
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
import matplotlib.backend_bases
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

##	------- code start--------

display = False
take = False
start = True
stop = False

sync = True
hold = "D"
dhold = 0


yOutFileName = "defaultPendDataFile.txt"
yRawDataFile = "defaultRawData.txt"
comments = " none "
portName = "COM3"
yBaud = 38400
yDataSize = 6
count = 0
endtime = 0
displaymax = 200

displaydata = [ 0 ]
endtime = '500'
name = 'defalutOutFile'
comments = 'data log file'

##	"""set up gui
top = Tk()
top.title('Pendulumn Drag Experiemnt')

## 	"""button defintions for actions
def startbt():
	global display
	display = True

def takebt():
	global take
	take = True

def inputbt():
	global endtime
	global name
	global comments
	endtime = int(txbox1.get())
	Label( txframe, text = "Data Points: " +  str(endtime) ).grid( row = 1, column=1, sticky=W )
	name = txbox2.get()
	Label( txframe, text = "Output File: " + name ).grid( row = 2, column=1, sticky=W )
	comments = txbox3.get() + "\n"
	Label( txframe, text = "Comments: " + comments ).grid( row = 3, column=1, sticky=W )
	
def stopbt():
	global start
	global take
	global display
	global start
	global stop
	if start == False:
		take = False
		display = False
		start = True
		stop = True

def exitbt():
	file.close()
	raw.close()
	ser.close()
	top.quit()
	top.destroy()
		
##	"""creat button frame
butframe = Frame( top )
butframe.pack( side = LEFT )

startdButton = Button( butframe, text="Display Data", command = startbt )
startdButton.pack( side = BOTTOM )

takedButton = Button( butframe, text="Take Data", command = takebt )
takedButton.pack( side = BOTTOM )

stopdButton = Button( butframe, text="Stop Data", command = stopbt )
stopdButton.pack( side = BOTTOM )

exitdButton = Button( butframe, text="Exit", command = exitbt )
exitdButton.pack( side = BOTTOM )

exitdButton = Button( butframe, text="Set Input", command = inputbt )
exitdButton.pack( side = BOTTOM )

##	"""creat text frame
txframe = Frame( top )
txframe.pack( side = BOTTOM )

##	"""create text entry boxes
Label( txframe, text = "Data Points: " +  endtime ).grid( row = 1, column=1, sticky=W )
txbox1 = Entry( txframe)
txbox1.insert(0, endtime )
txbox1.grid( row=1, column=2, sticky=W )

Label( txframe, text = "Output File: " + name ).grid( row = 2, column=1, sticky=W )
txbox2 = Entry( txframe)
txbox2.insert(0, name)
txbox2.grid( row=2, column=2, sticky=W )

Label( txframe, text = "Comments: " + comments ).grid( row = 3, column=1, sticky=W )
txbox3 = Entry( txframe)
txbox3.insert(0, comments )
txbox3.grid( row=3, column=2, sticky=W )

## create drawing canvas

fig = plt.figure()
canvas = FigureCanvasTkAgg(fig, master = top)
canvas.get_tk_widget().pack( side = TOP )

##	""" Given a port-name (of the form COM7,
##	COM12, CNCA0, etc.) returns a full
##	name suitable for opening with the
##	Serial class.
##	"""
def full_port_name(portname):
	m = re.match("^COM(\d+)$", portname)
	if m and int(m.group(1)) < 10:
		return portname
	return "\\\\.\\" + portname

##	""" translates the captured data from the detector board """
def trans_data( block, count ):
	global sync
	global yDataSize
	global hold
	
	logStr = ""
	dataStr = ""
	rawStr = ""
	shadStr = ""
	outStr = ""
	shad = 0
	n = 0
	
	for ch in block:
		temp = ord(ch)
		rawStr += hex(temp) + " "
		n += 1
		if n == 1:
			if sync == True:
				if temp == 82:
					outStr += "R"
				if temp == 70:
					outStr += "F"
			else:
				outStr = hold
				shad += 256*256*temp
				n = 2
		if n == 2:
			shad += 256*256*temp
		if n == 3:
			shad += 256*temp
		if n == 4:
			shad += temp
		if n == 5:
			shadStr += str( shad )
			shad = 0
			if temp == 44:
				sync = True
				yDataSize = 6
			else:
				sync = False
				yDataSize = 5
		if n == 6:
			n = 0
			dataStr = str( count ) + "," + outStr + "," + shadStr + "\n"
			logStr = str(count) + " " + outStr + " "+ shadStr + " " + rawStr + "\n"
			if sync == False:
				if temp == 255:
					if temp == 82:
						hold = "R"
					if temp == 70:
						hold = "F"
				else:
					hold = "D"
	returnlist = [ logStr, dataStr ]
	##print logStr
	Label( txframe, text = logStr ).grid( row = 1, column=0, sticky=W )
	return returnlist

##	"""display current data in gui window
def showdata( data ):
	global displaydata
	global displaymax
	displaylength = []
	displaylist = []
	##print('showdata')
	data = data.strip()
	templist = data.split(",",3)
	##print templist
	displaydata.append(templist[2])
	datalen = len(displaydata)
	if datalen > displaymax :
		displaydata = displaydata[1:]
	displaystart = len(displaydata)
	displaylength = arange(0,len(displaydata),1)
	for ele in displaylength:
		displaylist.append( displaydata[ele])
	##print displaylist
	##print displaylength
	plt.clf()
	plt.plot(displaylength,displaylist)
	plt.xlabel('Data Point Count')
	plt.ylabel('Time   1 : 1.6 micro Seconds ')
	plt.gcf().canvas.draw()

## 	collecting data       
def datacollection( ):
	global stop
	global display
	global start
	global take
	global count
	global ser
	global endtime
	global file
	global raw
	global dhold
	
	##print('colection')
	if display == True:
		##print('display')
		if start == True:
			##print('start')
			yOutFileName = name + ".txt"
			yRawDataFile = name + ".dat"
			file = open( yOutFileName, "wb")
			raw = open( yRawDataFile, "wb")
			file.write( comments )
			raw.write('Raw data log \n')
			yPort = full_port_name( portName )
			ser = serial.Serial( yPort, yBaud )
			start = False
		##print('body')
		try:
			data = ser.read( yDataSize )
			dhold = data
		except Exception:
			data = ser.read( yDataSize)
			pass
		out =  trans_data( data , count)
		showdata( out[1] )
		if take == True:
			##print('take')
			file.write( out[0] )
			raw.write( out[1] )
			count = count + 1
			if count > endtime:
				stop = True
				take = False
				display = False
	if stop == True:
		take = False
		display = False
		stop = False
		start = True
		file.close()
		raw.close()
		count = 0
		displaydata = []
	##print('end collect')
	top.after(5, datacollection)

##	"""------code end-----------
## 	"""running applicaton loop
top.after(5,datacollection )
top.mainloop()
       
