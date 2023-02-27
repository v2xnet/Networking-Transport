#!/bin/bash
#source /home/covcrav/phyCar/src/itsnet_app/config/testbed.sh
INTF1=wlp0s20u1
INTF2=wlan0
PATH_ITSNET_CONF="/home/covcrav/phyCar/src/itsnet/extra"
GREP_OCB=`grep "OCB = true" $PATH_ITSNET_CONF/itsnet.conf | cut -f3 -d' '`
echo $GREP_OCB
if [[ $GREP_OCB = "true" ]]; then

        MODE=OCB # OCB or IBSS
        echo "OCB"
else
        MODE=IBSS
        echo "IBSS"
fi

#MAC80211=`lsmod | grep mac80211`


######## Wireless Adhoc Configuration #########"

#### Loading wireless Driver ###########################

modprobe -r brcmfmac

if [[ $MODE = "IBSS" ]]; then 
	modprobe -r 8188eu;modprobe -r r8188eu;
	modprobe 8188eu ; modprobe brcmfmac
	iw reg set JP
	sleep 3

	ifconfig $INTF1 down
	iwconfig $INTF1 mode ad-hoc
	iwconfig $INTF1 essid itsnet
	iwconfig $INTF1 freq 14
	ifconfig $INTF1 192.168.0.11 netmask 255.255.255.0
	ifconfig $INTF1 up

	ifconfig $INTF2 down
	iwconfig $INTF2 mode ad-hoc
	iwconfig $INTF2 essid "itsnet"
	iwconfig $INTF2 freq 14
	ifconfig $INTF2 up	

fi

if [[ $MODE = "OCB" ]]; then 
	modprobe -r 8188eu; modprobe -r r8188eu;
	modprobe 8188eu
	sleep 3
	ifconfig $INTF1 up 
	iw reg set JP 
	iwconfig $INTF1 freq 14
	iw $INTF1 set txpower fixed 3000
	ifconfig $INTF1 down 
	iwconfig $INTF1 mode monitor 
	ifconfig $INTF1 up
	iwconfig $INTF1 freq 14
fi
