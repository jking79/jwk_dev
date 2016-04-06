import serial, re, time

def full_port_name(portname):
	""" Given a port-name (of the form COM7,
	COM12, CNCA0, etc.) returns a full
	name suitable for opening with the
	Serial class.
	"""
	m = re.match('^COM(\d+)$', portname)
	if m and int(m.group(1)) < 10:
		return portname
	return '\\\\.\\' + portname


def trans_data( block, count, rFile ):
	""" translates the captured data from the detector board """
	outStr = ''
	rawStr = ''
	shadStr = ''
	shad = 0

	n = 0

	for ch in block:
		temp = ord(ch)
		rawStr += hex(temp) + ' '
		n += 1
		if n == 1:
			if temp == 82:
				outStr += 'R'
			if temp == 70:
				outStr += 'F'
		if n == 2:
			shad += 256*256*temp
		if n == 3:
			shad += 256*temp
		if n == 4:
			shad += temp
		if n == 5:
			shadStr += str( shad )
			shad = 0
		if n == 6:
			n = 0
			rawData = str( count ) + "," + outStr + ',' + shadStr + '\n'
			rFile.write( rawData )
			outStr = str(count) + ' ' + outStr + ' '+ shadStr + ' ' + rawStr + '\n'
			print outStr

	return outStr


def getData( file, raw, time ):

	yPort = full_port_name('COM3')
	yBaud = 38400
	yDataSize = 6
	ser = serial.Serial( yPort, yBaud )
	done = False

	for x in range(0,time):
		data = ser.read( yDataSize )
		out =  trans_data( data , x, raw)
		file.write( out )
		
	ser.close()




yOutFileName = 'defaultPendDataFile.txt'

endTime = int( raw_input('Enter number of Data points to Capture: '))
fileName = raw_input( 'Enter Output File Name: ')
yOutFileName = fileName + '.txt'
yRawDataFile = fileName + '.dat'
file = open( yOutFileName, 'wb') 
raw = open( yRawDataFile, 'wb')
raw
comments = raw_input( 'Enter Comments for log file: ')
file.write( comments + '\n'  )
""" startTime = time.time() """
print('Data Capture Started for : %i data points.' %endTime)
""""while not done :"""
getData( file, raw, endTime)
file.close()
raw.close()