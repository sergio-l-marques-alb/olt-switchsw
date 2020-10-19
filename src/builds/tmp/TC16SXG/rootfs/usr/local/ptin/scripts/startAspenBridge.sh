#!/bin/sh

# Configure virtual interface for Aspen inband communication
# Using eth0 interface with VLAN 2045

vconfig add dtl0 2045
ip address add 172.0.1.2/16 dev dtl0.2045
ip link set dtl0.2045 up

