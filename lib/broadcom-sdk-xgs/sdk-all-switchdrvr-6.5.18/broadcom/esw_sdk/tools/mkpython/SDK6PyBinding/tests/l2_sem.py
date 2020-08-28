##############################################################################
#
# Test to check API semantics for selected L2 APIs.
# NOTE : Tests are intended for API semantics. Functionality not covered.
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.bcm.vlan import *
from sdk6.bcm.l2 import *

# Construct mac address from mac string 
def mac_hex_str_to_addr(mac_str):
    mac = bcm_mac_t()

    i = 0
    alist = mac_str.split(":")
    if len(alist) > 6:
        print("Invalid Mac address " + mac_str)
        return None

    for a in alist:
        try:
            mac[i] = int(a, 16)
            i += 1
        except:
            print("Invalid Mac Address '" + a + " ' in " + mac_str)
            return None

    return mac

def mac_addr_to_hex_str(mac):
    mac_str = ""
    for i in range(6):
        if i > 0:
            mac_str += ":"
        mac_str += str(hex(mac[i]).split('x')[-1].strip("L"))
    return mac_str

def mac_compare(mac_1, mac_2):
    for i in range(6):
        if mac_1[i] != mac_2[i]:
            return 1
    return 0

# L2 traverse callback function.
def l2_trav_cb(unit, info, user_data):
    print("Unit : %d; vid : %d; port : %d" % (unit, info.vid, info.port))

def l2_sem_test_001():
    Test_Coverage = ""
    unit = 0
    vid = 100
    cos = 2
    port = 1
    l2_flags = 0
    fails = 0

    mac = mac_hex_str_to_addr("00:01:02:03:04:05")
    if mac == None:
        print("Error in forming MAC address")
        fails += 1
    Test_Coverage += "bcm_mac_t\n"

    try:
        l2_s = bcm_l2_addr_t()
        l2_s.mac = mac.cast()
        l2_s.vid = vid
        l2_s.port = port
        l2_s.flags = l2_flags
        l2_s.cos_dst = cos

        rv = bcm_l2_addr_add(unit, l2_s)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_l2_addr_add")
        
    Test_Coverage += "bcm_l2_addr_t\n"
    Test_Coverage += "bcm_l2_addr_add\n"

    try:
        l2_r = bcm_l2_addr_t()
        rv = bcm_l2_addr_get(unit, mac.cast(), vid, l2_r)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_l2_addr_get")
    Test_Coverage += "bcm_l2_addr_get\n"

    try:
        mac_r = uint8_array_frompointer(l2_r.mac)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in converting MAC pointer to array")

    # print "Callback function test"
    # rv = bcm_l2_traverse(unit, l2_trav_cb, None)

    print("\n******** API Coverage ********\n" + Test_Coverage)
    return fails

