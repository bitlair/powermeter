 #!/bin/bash

cd /root/projects/power

ARGS2=(
    -l 0
    -v "liter per hour"
    --units-exponent 0
    DEF:CC=gasm3.rrd:M3:AVERAGE
    "CDEF:weekend=LTIME,604800,%,172800,345600,LIMIT,UN,UNKN,CC,*,INF,IF"
    "CDEF:night=LTIME,86400,%,25200,82800,LIMIT,UN,INF,UNKN,CC,*,IF"
    "CDEF:offpeak=weekend,ISINF,night,ISINF,+,INF,*"
    "CDEF:Low=CC,0,750,LIMIT"
    "CDEF:Mid=CC,750,1500,LIMIT"
    "CDEF:High=CC,1500,3000,LIMIT"
    "CDEF:VHigh=CC,3000,12000,LIMIT"
    -c BACK#000000 -c CANVAS#202020 -c MGRID#FD5A1E -c GRID#FD5A1E -c FONT#FD5A1E
    -c AXIS#FD5A1E -c ARROW#FD5A1E -c SHADEA#000000 -c SHADEB#000000
    AREA:Low#00ff00:"0 - 750" 
    AREA:Mid#ffbf00:"750 - 1500" 
    AREA:High#FF0000:"1500 - 3000" 
    AREA:VHigh#ffffff:"> 3 l/h"
    "GPRINT:CC:AVERAGE:Average usage\: %.0lf l/h"
)

#rrdtool graph gas.png  "${ARGS[@]}" -s 18:00 -e 20:10
#exit

if [ "X$1" == "Xdetail" ]; then

nice -n 10 \
rrdtool graph temp$$.png  -w 900 -h 300 "${ARGS2[@]}" -s end-6h  -e now "GPRINT:CC:LAST:Current usage\: %.0lf l/h" \
    && mv temp$$.png /var/www/stats/gas-6h.png

nice -n 10 \
rrdtool graph temp$$.png  -w 520 -h 200 "${ARGS2[@]}" -s end-4h  -e now "GPRINT:CC:LAST:Current usage\: %.0lf l/h" \
    && mv temp$$.png /var/www/stats/gas-4h-small.png

fi

if [ "X$1" == "Xhourly" ]; then

ARGS3=(
    -w 400 -h 200 
#    "CDEF:euro=cc/s,offpeak,0.23,0.23,IF,*,1000,/,24,*"
#    "GPRINT:euro:AVERAGE:Average cost per day\: %.2lf EUR"
)
    
nice -n 10 \
rrdtool graph temp$$.png "${ARGS2[@]}" "${ARGS3[@]}" -s end-2d  -e now \
    && mv temp$$.png /var/www/stats/gas-2d.png

nice -n 10 \
rrdtool graph temp$$.png "${ARGS2[@]}" "${ARGS3[@]}" -s end-14d  -e now \
    && mv temp$$.png /var/www/stats/gas-14d.png

nice -n 10 \
rrdtool graph temp$$.png "${ARGS2[@]}" "${ARGS3[@]}" -s end-60d -e now \
    && mv temp$$.png /var/www/stats/gas-60d.png

nice -n 10 \
rrdtool graph temp$$.png "${ARGS2[@]}" "${ARGS3[@]}" -s end-1y -e now \
    && mv temp$$.png /var/www/stats/gas-1y.png

fi
