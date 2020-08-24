###############################################################################
#
#  mkpython regression support library.
#  This script contains library functions to support mkpython regression.
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

import os, sys, argparse
import socket, telnetlib, time

from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.appl.cpudb import *
from sdk6.appl.ctsoc import *

verbose = False

###############################################################################
# Function : VERBOSE.
#
# Description : Dump logs when verbose is enabled.
#
# Arguments : string - String to display.
#             level  - (OPTIONAL) number of <tab> ahead of the string.
#
# Returns : None.
###############################################################################
def VERBOSE(string, level=1):
    prefix = "";
    for i in range(level):
        prefix += "\t"
    if verbose:
        print(string);

###############################################################################
# Function : set_verbose.
#
# Description : Enable/Disable verbose
#
# Arguments : v - Enable/Disable.
#
# Returns : None.
###############################################################################
def set_verbose(v):
    global verbose
    verbose = v

###############################################################################
# Function : telnet_conn.
#
# Description : Initiate telnet connection with device for CLI command support.
#
# Arguments : device_ip_str - Device IP address in string.
#             netserve_port - Port number for telnet connection.
#
# Returns : telnet handle. Exception otherwise.
###############################################################################
def telnet_conn(device_ip_str, netserve_port):
    VERBOSE("telnet_conn Entry.\n")
    try:
        tn = telnetlib.Telnet(device_ip_str, netserve_port)
    except Exception as e:
        print(e)
        raise e
    VERBOSE("telnet_conn Exit.\n")
    return tn


###############################################################################
# Function : tn_write_and_wait.
#
# Description : Execute telnet command on device and wait for prompt.
#
# Arguments : tn  - telnet handle.
#             cmd - command to be executed.
#
# Returns : None.
###############################################################################
def tn_write_and_wait(tn, cmd):
    tn.write(cmd.encode('ascii'))
    VERBOSE(tn.read_until("BCM.0>".encode('utf-8')).decode())


###############################################################################
# Function : dev_rpc_init.
#
# Description : Device side connection initiator.
#
# Arguments : tn       - telnet handle.
#             dev_key - Server key.
#
# Returns : None.
###############################################################################
def dev_rpc_init(tn, dev_key):
    VERBOSE("dev_rpc_init Entry.")
    tn_write_and_wait(tn, "ps\n")
    tn_write_and_wait(tn, "cpudb create\n")
    tn_write_and_wait(tn, "cpudb current 0\n")

    cmd = "dbparse object=cpudb_entry key=00:00:00:00:00:0%d mac=00:00:00:00:00:0%d\n" % (dev_key, dev_key)
    tn_write_and_wait(tn, cmd)

    cmd = "dbparse object=local_key key=00:00:00:00:00:0%d\n" % dev_key
    tn_write_and_wait(tn, cmd)

    tn_write_and_wait(tn, "cts atp cos=0 vlan=1\n")
    tn_write_and_wait(tn, "cts timeout RetryTO=300000 retries=10\n")
    time.sleep(1)

    tn_write_and_wait(tn, "rpc start\n")
    tn_write_and_wait(tn, "cts atp trans sock server start\n")
    VERBOSE("dev_rpc_init Exit.")


###############################################################################
# Function : host_rpc_init.
#
# Description : Host side connection initiator.
#
# Arguments : host_key - local key
#             host_ip   - IP address of host running python.
#             dev_key  - Server key.
#             dev_ip    - IP address of device.
#
# Returns : 0 if success. BCM error code otherwise.
###############################################################################
def host_rpc_init(host_key, host_ip, dev_key, dev_ip):
    VERBOSE("host_rpc_init Entry.")
    use_heap = 1

    bcm_ctsoc_mem_control(use_heap)

    rv = bcm_ctsoc_cpudb_create(host_key)
    if (rv < 0):
        print("bcm_ctsoc_cpudb_create FAILED")
        return rv

    rv = bcm_ctsoc_client_install(host_key, host_ip)
    if (rv < 0):
        print("bcm_ctsoc_client_install local FAILED")
        return rv

    rv = bcm_ctsoc_client_install(dev_key, dev_ip)
    if (rv < 0):
        print("bcm_ctsoc_client_install server FAILED")
        return rv

    rv = bcm_ctsoc_client_server_start()
    if (rv < 0):
        print("bcm_ctsoc_client_server_start FAILED")
        return rv

    VERBOSE("host_rpc_init Exit.")
    return 0


###############################################################################
# Function : dev_rpc_connect.
#
# Description : Establish device side connection.
#
# Arguments : tn        - telnet handle.
#             host_key  - local key.
#             host_ip   - IP address of host.
#
# Returns : None.
###############################################################################
def dev_rpc_connect(tn, host_ip, host_key):
    VERBOSE("dev_rpc_connect Entry.")

    cmd = "cts atp transport socket install destkey=00:00:00:00:00:0%d destip=%s\n" % (host_key, host_ip)
    tn_write_and_wait(tn, cmd)

    VERBOSE("dev_rpc_connect Exit.")


###############################################################################
# Function : dev_rpc_disconnect.
#
# Description : Disconnect device side connection.
#
# Arguments : tn        - telnet handle.
#             host_key  - local key.
#
# Returns : None.
###############################################################################
def dev_rpc_disconnect(tn, host_key):
    VERBOSE("dev_rpc_disconnect Entry.")

    cmd = "cts atp transport socket uninstall destkey=00:00:00:00:00:0%d\n" % host_key
    tn_write_and_wait(tn, cmd)

    VERBOSE("dev_rpc_disconnect Exit.")


###############################################################################
# Function : host_rpc_connect.
#
# Description : Establish host side connection.
#
# Arguments : host_key - local key
#             host_ip   - IP address of host.
#
# Returns : 0 if success. BCM error code otherwise.
###############################################################################
def host_rpc_connect(local_unit, remote_unit, dev_key):
    VERBOSE("host_rpc_connect Entry.")

    msecs=300
    retries=10

    rv = bcm_ctsoc_setup()
    if (rv < 0):
        print("bcm_ctsoc_setup FAILED")
        return rv

    rv = bcm_ctsoc_client_rpc_start(msecs, retries)
    if (rv < 0):
        print("bcm_ctsoc_client_rpc_start FAILED")
        return rv

    rv = bcm_ctsoc_client_attach(local_unit, remote_unit, dev_key)
    if (rv < 0):
        print("bcm_ctsoc_client_attach FAILED")
        return rv

    VERBOSE("host_rpc_connect Exit.")
    return 0




