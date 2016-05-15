import serial
import os
import socket
from time import sleep, time

filename = 'kWh.rrd'

ser = serial.Serial('/dev/ttyUSB0', 57600, timeout=99999)
ser.setRTS(level=True)
ser.setDTR(level=True)
sleep(1)
ser.setRTS(level=False)
ser.setDTR(level=False)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

while True:
    line = ser.readline()
    #print "\x1b[H\x1b[J%s"%line 
    print line

#ret = rrd_update('example.rrd', 'N:%s:%s' %(metric1, metric2));
