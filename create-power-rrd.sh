#!/bin/sh
rrdtool create kWh.new.rrd -s 1 \
DS:kWh:GAUGE:600:0:6000 \
RRA:AVERAGE:.5:60:4320 \
RRA:AVERAGE:.5:3600:8880 \
RRA:AVERAGE:.5:86400:3650
