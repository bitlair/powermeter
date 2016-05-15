#!/bin/sh
stty -F /dev/ttyUSB0 57600

cd /home/bitlair/projects/power
#while true; do
# head -n1 /dev/ttyUSB0 | gawk '{ print $0 > "/dev/stderr"; if (/^Cycle/) { print "update kWh.rrd N:" $5 } }' | rrdtool -
# sleep 1
#done

while true; do
 head /dev/ttyUSB0 | php power.php
 sleep 1
done

