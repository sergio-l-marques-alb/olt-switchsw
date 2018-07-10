#!/bin/sh

# Configure virtual interface for packet capture
# Using eth0 interface with VLAN 2046

ip link add link dtl0 name dtl0.2046 type vlan id 2046
ip link set dtl0 up
vconfig add eth0 2046
ip link set eth0.2046 up
ip link set dtl0.2046 up

brctl addbr cap_br
brctl addif cap_br eth0.2046
brctl addif cap_br dtl0.2046
ip link set cap_br up

