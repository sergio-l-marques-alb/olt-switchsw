#!/bin/sh

# Configure virtual interfaces for Aspen inband communication
# Using dtl interface with VLAN 2044+2045

vconfig add dtl0 2044
vconfig add dtl0 2045

ip address add 172.0.0.2/24 dev dtl0.2044
ip address add 172.0.1.2/24 dev dtl0.2045

ip link set dtl0.2044 up
ip link set dtl0.2045 up

