import serial
import os
import socket
from time import sleep, time, tzset

tzset()

filename = 'kWh.rrd'
gasfilename = 'gasm3.rrd'

ser = serial.Serial('/dev/ttyUSB0', 57600, timeout=99999)
ser.setRTS(level=True)
ser.setDTR(level=True)
sleep(1)
ser.setRTS(level=False)
ser.setDTR(level=False)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

while True:
    line = ser.readline()
    if "Cycle" in line:
        kwh = line.split()[4]
        if kwh == "incomplete":
            continue
        try:    
            if int(float(kwh)) > 12000:
                continue
        except:
            print line
            continue
        print line
        os.system("echo \"update %s N:%s\" | rrdtool -"%(filename,kwh))
        f = open('/var/www/stats/kwh', 'w')
        f.write(kwh)
        f.close()
        try:
	    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect(("localhost", 2003))
            s.send("bitlair.power.meter.watt %s %s\n"%(kwh,int(time())))
            s.close()
	    print "power update"
        except:
            print "unable to transmit stats"
            continue
    if "Gas" in line:
        m3 = line.split()[4]
        if m3 == "incomplete":
            continue
        try:
            if int(float(m3)) > 12000:
                continue
        except:
            print line
            continue
        print line
        print m3
        os.system("echo \"update %s N:%s\" | rrdtool -"%(gasfilename,m3))
        f = open('/var/www/stats/gasm3', 'w')
        f.write(m3)
        f.close()
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect(("localhost", 2003))
            s.send("bitlair.gas.meter.m3 %s %s\n"%(m3,int(time())))
            s.close()
        except:
            print "unable to transmit stats"
            continue
    elif "28152A3A3005C" in line:
        temp = line.split()[1]
        print "Onder: %sC" %(temp)
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect(("localhost", 2003))
            s.send("bitlair.temp.onder %s %s\n"%(temp,int(time())))
            s.close()
        except:
            print "unable to transmit stats"
            continue
    elif "2821533A300F3" in line:
        temp = line.split()[1]
        print "Boven: %sC" %(temp)
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect(("localhost", 2003))
            s.send("bitlair.temp.boven %s %s\n"%(temp,int(time())))
            s.close()
        except:
            print "unable to transmit stats"
            continue
    else:
        if "Marker" not in line:
            print line 

#ret = rrd_update('example.rrd', 'N:%s:%s' %(metric1, metric2));
