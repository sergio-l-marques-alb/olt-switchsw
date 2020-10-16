#!/bin/sh


######################################################################################
# IMPORTANT
######################################################################################
# Due to an unresolved kernel issue, when adding a subinterface to eth0,
# this interface also adds a default TAG and the internal LAN (192.168.200.x) is lost
# A fix was tried changing the dtb to use eTSEC but with no success
######################################################################################
#exit

# Configure virtual interface for packet capture
# Using eth0 interface with VLAN 2045

vconfig add eth0 2045
ip link set eth0.2045 up
vconfig add dtl0 2045
ip link set dtl0.2045 up

brctl addbr cap_br
brctl addif cap_br eth0.2045
brctl addif cap_br dtl0.2045
ip link set cap_br up

