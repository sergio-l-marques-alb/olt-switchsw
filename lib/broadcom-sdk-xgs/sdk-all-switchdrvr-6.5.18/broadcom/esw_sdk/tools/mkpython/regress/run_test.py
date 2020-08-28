###############################################################################
#
#  mkpython regression.
#  This script runs mkpython regression.
#
#  Usage Example: python run_test.py -d "BCM56960" -i "10.89.23.230"
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

import os, sys, argparse, time
import socket, telnetlib, importlib

from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.bcm.port import *
from sdk6.appl.cpudb import *
from sdk6.appl.ctsoc import *
from regress_lib import *

parser = argparse.ArgumentParser(description='mkpython regression utility.')
parser.add_argument('-d', '--device', help="Device Name", required=True, type=str)
parser.add_argument('-i', '--dip', help="Device IP Address", required=True, type=str)
parser.add_argument('-f', '--tfile', help="File with test list", required=True, type=str)
parser.add_argument('-v', '--verbose', help="Enable verbose", default=False,
        action='store_true')
args = parser.parse_args(sys.argv[1:])

# The following are hardcoded.
# If needed, can be moved to command parser.
netserve_port = 1234
dev_key = 1
host_key = 2
local_unit = 0
remote_unit = 0

verbose = args.verbose
dip = args.dip
device = args.device
tfile = args.tfile

my_ip = socket.gethostbyname(socket.gethostname())
import_list = []


if sys.version_info[0] < 3:
    import netaddr
    host_ip = netaddr.IPAddress(my_ip).value
    dev_ip = netaddr.IPAddress(dip).value
else:
    import ipaddress
    host_ip = ipaddress.IPv4Address._ip_int_from_string(my_ip)
    dev_ip = ipaddress.IPv4Address._ip_int_from_string(dip)

###############################################################################
# Function : rpc_con.
#
# Description : Establish RPC connection..
#
# Arguments : None.
#
# Returns : None.
###############################################################################
def rpc_con():
    global tn

    # STEP 1 : Get connection to device command prompt.
    try:
        tn = telnet_conn(dip, netserve_port)
    except:
        print("Error : Telnet connection to device failed.")
        exit(1)

    # STEP 2 : Initiate RPC from device side.
    dev_rpc_init(tn, dev_key)
    time.sleep(1)

    # STEP 3 : Initiate RPC from host side.
    rv = host_rpc_init(host_key, host_ip, dev_key, dev_ip)
    if (rv < 0):
        print("Error : host_rpc_init failed.")
        exit(1)
    time.sleep(1)

    # STEP 4 : Establish connection from device side.
    dev_rpc_connect(tn, my_ip, host_key)
    time.sleep(1)

    # STEP 5 : Establish connection from host side.
    rv = host_rpc_connect(local_unit, remote_unit, dev_key)
    if (rv < 0):
        print("Error : host_rpc_init failed.")
        exit(1)
    time.sleep(1)

    print("Remote Client Session Established.")


###############################################################################
# Function : rpc_con.
#
# Description : Establish RPC connection..
#
# Arguments : None.
#
# Returns : None.
###############################################################################
def rpc_disconnect():
    dev_rpc_disconnect(tn, host_key)


###############################################################################
# Function : check_test_list.
#
# Description : Run the tests listed in test list.
#
# Arguments : None.
#
# Returns : None.
###############################################################################
def check_test_list():
    global tests

    try:
        with open(tfile) as f:
            lines = f.readlines()
        lines = [x.strip() for x in lines]
    except:
        # Error in opening test list
        return 1

    tests = []
    for line in lines:
        if (line.startswith("#") or line == ""):
            continue
        tests.append(line)

    if (len(tests) == 0):
        # Nothing to test
        return 1

    return 0

###############################################################################
# Function : run_tests.
#
# Description : Run the tests listed in test list.
#
# Arguments : None.
#
# Returns : None.
###############################################################################
def run_tests():
    test_id = 0
    pass_list = []
    fail_list = []

    for line in tests:
        try:
            (f, test) = line.split(".")
        except:
            # Error in reading test. Move on to next test.
            print("Error reading test %s." % line)
            fail_list.append(line)
            continue

        if f not in import_list:
            try:
                module_name = "sample." + f
                VERBOSE("Importing module " + module_name)
                mod = importlib.import_module(module_name, package=None)
                import_list.append(f)
            except:
                print("Error importing file %s" % f)
                fail_list.append(line)
                continue

        try:
            function=getattr(mod, test)
            if (function() < 0):
                print("Test %s failed." % test)
                fail_list.append(line)
            else:
                pass_list.append(line)
        except:
            fail_list.append(line)
        finally:
            tn_write_and_wait(tn, "rc\n")


    # Report result
    print("\n**************************************");
    print("Test report on device " + device)
    print("**************************************");
    if (len(fail_list) != 0):
        print("Failed tests:")
        for line in fail_list:
            print("    " + line)

    if (len(pass_list) != 0):
        print("Passed tests:")
        for line in pass_list:
            print("    " + line)


if (check_test_list() != 0):
    # Nothing to test.
    print("Error : Test list not available.")
    exit(1)

set_verbose(verbose)
rpc_con()
run_tests()
rpc_disconnect()
exit(0)
