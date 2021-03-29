#!/bin/sh

# Configure virtual interfaces for Aspen inband communication

if [ $BOARD_VER -eq 2 ]; then
    # Using eth1 interface with VLAN 2044+2045
    ifconfig eth1 up
    vconfig add eth1 2044
    vconfig add eth1 2045

    ifconfig eth1.2044 172.0.0.2 netmask 255.255.255.0 up
    ifconfig eth1.2045 172.0.1.2 netmask 255.255.255.0 up
else
    # Using dtl interface with VLAN 2044+2045
    vconfig add dtl0 2044
    vconfig add dtl0 2045

    ip address add 172.0.0.2/24 dev dtl0.2044
    ip address add 172.0.1.2/24 dev dtl0.2045

    ip link set dtl0.2044 up
    ip link set dtl0.2045 up
fi

