import socket
from time import sleep, time
kwh = 2 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 2003))
s.send("bitlair.power.meter.watt %s %s\n"%(kwh,int(time())))
print("bitlair.power.meter.watt %s %s"%(kwh,int(time())))
s.close()
