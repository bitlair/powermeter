 #!/bin/bash

cd /root/projects/power

ARGS2=(
	-l 0
        -v "Watt"
        --units-exponent 0
        DEF:Watt=kWh.rrd:kWh:AVERAGE
	"CDEF:weekend=LTIME,604800,%,172800,345600,LIMIT,UN,UNKN,Watt,*,INF,IF"
	"CDEF:night=LTIME,86400,%,25200,82800,LIMIT,UN,INF,UNKN,Watt,*,IF"
	"CDEF:offpeak=weekend,ISINF,night,ISINF,+,INF,*"
	AREA:offpeak#000040:""
	"CDEF:Low=Watt,0,750,LIMIT"
        "CDEF:Mid=Watt,750,1500,LIMIT"
        "CDEF:High=Watt,1500,3000,LIMIT"
        "CDEF:VHigh=Watt,3000,12000,LIMIT"
        -c BACK#000000 -c CANVAS#202020 -c MGRID#FD5A1E -c GRID#FD5A1E -c FONT#FD5A1E
        -c AXIS#FD5A1E -c ARROW#FD5A1E -c SHADEA#000000 -c SHADEB#000000
	AREA:Low#00ff00:"0 - 750" 
	AREA:Mid#ffbf00:"750 - 1500" 
	AREA:High#FF0000:"1500 - 3000" 
        AREA:VHigh#ffffff:"> 3kW"
	"GPRINT:Watt:AVERAGE:Average usage\: %.0lf W"
)

#rrdtool graph power.png  "${ARGS[@]}" -s 18:00 -e 20:10
#exit

if [ "X$1" == "Xdetail" ]; then

nice -n 10 \
rrdtool graph temp$$.png  -w 900 -h 300 "${ARGS2[@]}" -s end-6h  -e now "GPRINT:Watt:LAST:Current usage\: %.0lf W" \
	&& mv temp$$.png /var/www/stats/power-6h.png

nice -n 10 \
rrdtool graph temp$$.png  -w 520 -h 200 "${ARGS2[@]}" -s end-4h  -e now "GPRINT:Watt:LAST:Current usage\: %.0lf W" \
        && mv temp$$.png /var/www/stats/power-4h-small.png

fi

if [ "X$1" == "Xhourly" ]; then

ARGS3=(
	-w 400 -h 200 
	"CDEF:euro=Watt,offpeak,0.23,0.23,IF,*,1000,/,24,*"
	"GPRINT:euro:AVERAGE:Average cost per day\: %.2lf EUR"
)
	
nice -n 10 \
rrdtool graph temp$$.png "${ARGS2[@]}" "${ARGS3[@]}" -s end-2d  -e now \
	&& mv temp$$.png /var/www/stats/power-2d.png

nice -n 10 \
rrdtool graph temp$$.png "${ARGS2[@]}" "${ARGS3[@]}" -s end-14d  -e now \
	&& mv temp$$.png /var/www/stats/power-14d.png

nice -n 10 \
rrdtool graph temp$$.png "${ARGS2[@]}" "${ARGS3[@]}" -s end-60d -e now \
	&& mv temp$$.png /var/www/stats/power-60d.png

nice -n 10 \
rrdtool graph temp$$.png "${ARGS2[@]}" "${ARGS3[@]}" -s end-1y -e now \
	&& mv temp$$.png /var/www/stats/power-1y.png

fi
