###############################################################################
#
#  python connection script
#  This script provides information on steps to establish the RPC
#  connection with DUT.
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################


## STEP : 1
## server side setup
##
## cpudb create
## cpudb current 0
## dbparse object=cpudb_entry key=00:00:00:00:00:01 mac=00:00:00:00:00:01
## dbparse object=local_key key=00:00:00:00:00:01
## cts atp cos=0 vlan=1
## cts timeout RetryTO=300000 retries=10
## rpc start
## cts atp trans sock server start

## STEP : 2
# client side setup
#

import ipaddress

from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.bcm.port import *
from sdk6.appl.cpudb import *
from sdk6.appl.ctsoc import *

print(sal_core_init())
print(sal_appl_init())

client_ip_addr = "10.89.23.117" # Host(running python) IP address
server_ip_addr = "10.89.23.230" # DUT IP address

use_heap = 1

client_ip = ipaddress.IPv4Address._ip_int_from_string(client_ip_addr)
server_ip = ipaddress.IPv4Address._ip_int_from_string(server_ip_addr)

print(bcm_ctsoc_mem_control(use_heap))
print(bcm_ctsoc_cpudb_create(2))
print(bcm_ctsoc_client_install(2, client_ip))
print(bcm_ctsoc_client_install(1, server_ip))
print(bcm_ctsoc_client_server_start())

## STEP : 3
## server side
## cts atp transport socket install destkey=00:00:00:00:00:02 destip=10.89.23.117

## STEP : 4
# client side
print(bcm_ctsoc_setup())
print(bcm_ctsoc_client_rpc_start(300,10))
print(bcm_ctsoc_client_attach(0,0,1))

## Now the session is established. 

# Example : Get a port
p = bcm_port_info_t()
bcm_port_info_get(0, 1, p)
print(p.enable)
print(p.stp_state)
en_p = int_ptr()
en_p.assign(-1)
bcm_port_enable_get(0, 1, en_p)
print(en_p.value())

## when client exits
## cts atp transport socket uninstall destkey=00:00:00:00:00:02
