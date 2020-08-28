###############################################################################
#
# rpc_con_with_dev_cntl.py
#
# A Python connection script that uses telnet to configure remote server
# for RPC Operatations and python bindings to bcm_ctsoc_XX() commands to
# configure the local client.
#
# Invoke bcm.user as a daemon that runs the telnet protocol on port 1234:
#     ./netserve -d 1234 bcm.user
#
# Once bcm.user is running as a telnet client, it can be configured remotely
# using the python/telnet interface.
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

import telnetlib, sys

from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.bcm.port import *
from sdk6.appl.cpudb import *
from sdk6.appl.ctsoc import *

client_ip_addr = "10.175.195.15" # Host(running python) IP address
server_ip_addr = "10.18.139.212" # DUT IP address

if sys.version_info[0] < 3:
    import netaddr
    client_ip = netaddr.IPAddress(client_ip_addr).value
    server_ip = netaddr.IPAddress(server_ip_addr).value
else:
    import ipaddress
    client_ip = ipaddress.IPv4Address._ip_int_from_string(client_ip_addr)
    server_ip = ipaddress.IPv4Address._ip_int_from_string(server_ip_addr)


netserve_port=1234

local_key=2
server_key=1

msecs=300
retries=10

local_unit=0
remote_unit=0

use_heap = 1

# Establish telnet session to the server
tn=telnetlib.Telnet(server_ip_addr, netserve_port)
print("Execute a PS command on server to verify that it is up")
tn.write("ps\n")
print("Read to the next prompt on server: ")
print(tn.read_until("BCM.0>"))

## STEP : 1
## server side setup
##
tn.write("cpudb create\n")
print("Read to the next prompt on server [cpudb create]")
print(tn.read_until("BCM.0>"))
tn.write("cpudb current 0\n")
print("Read to the next prompt on server [cpudb current 0]")
print(tn.read_until("BCM.0>"))
tn.write("dbparse object=cpudb_entry key=00:00:00:00:00:0%d mac=00:00:00:00:00:0%d\n" % (server_key, server_key))
print("Read to the next prompt on server [dbparse]")
print(tn.read_until("BCM.0>"))
tn.write("dbparse object=local_key key=00:00:00:00:00:0%d\n" % server_key)
print("Read to the next prompt on server [dbparse]")
print(tn.read_until("BCM.0>"))
tn.write("cts atp cos=0 vlan=1\n")
print("Read to the next prompt on server [cts atp 1]")
print(tn.read_until("BCM.0>"))
tn.write("cts timeout RetryTO=300000 retries=10\n")
print("Read to the next prompt on server [cts timeout]")
print(tn.read_until("BCM.0>"))
tn.write("rpc start\n")
print("Read to the next prompt on server [rpc start]\n")
print(tn.read_until("BCM.0>"))
tn.write("cts atp trans sock server start\n")
print("Read to the next prompt on server [cts atp trans]")
print(tn.read_until("BCM.0>"))

## STEP : 2
# client side setup

if (sal_core_init() < 0):
    print("SAL Core Init FAILED")
if (sal_appl_init() < 0):
    print("SAL Appl Init FAILED")

# Configure BCM to use heap.
bcm_ctsoc_mem_control(use_heap)

if (bcm_ctsoc_cpudb_create(local_key) < 0):
    print("Client ctsoc cpudb create FAILED")

if (bcm_ctsoc_client_install(local_key, client_ip) < 0):
    print("Client ctsoc client install [client] FAILED")

if (bcm_ctsoc_client_install(server_key, server_ip) < 0):
    print("Client ctsoc client install [server]")

if (bcm_ctsoc_client_server_start()):
    print("Client ctsoc client server start FAILED")

## STEP : 3
## server side
tn.write("cts atp transport socket install destkey=00:00:00:00:00:02 destip=%s\n" % client_ip_addr)
print("Read to the next prompt on server [cts atp transport socket install]")
print(tn.read_until("BCM.0>"))

## STEP : 4
# client side
if (bcm_ctsoc_setup() < 0):
    print("Clinet ctsoc setup")

if (bcm_ctsoc_client_rpc_start(msecs, retries) < 0):
    print("Clinet ctsoc rpc start")

if (bcm_ctsoc_client_attach(local_unit, remote_unit, server_key) < 0):
    print("Clinet ctsoc client attach")

# Now the session is established. 
print("Remote Client Session Established")

# Example : Get basic information for a port
remote_port=1
p = bcm_port_info_t()
bcm_port_info_get(local_unit, remote_port, p)
print("Remote port: %d" % remote_port)
print("Port Enable: %d" % p.enable)
print("STP State: %d" % p.stp_state)
en_p = int_ptr()
en_p.assign(-1)
bcm_port_enable_get(local_unit, remote_port, en_p)
print("Port Enable: %d" % en_p.value())

# SHUT DOWN REMOTE SERVER
tn.write("exit");
tn.close()

## when client exits
## cts atp transport socket uninstall destkey=00:00:00:00:00:02

