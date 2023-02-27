#####" stop itsnet daemon"######


kill -SIGINT `pgrep itsnet`


#####"stop wifi and tap0 interfaces"######

#ifconfig tap0 down
#tunctl -d tap0
#modprobe -r tun
#ifconfig eth1 down

#####"stop gps daemon"#########

#kill -SIGINT `pgrep gpsd`

#####"runing network-manager again"######

#/etc/init.d/network-manager start

