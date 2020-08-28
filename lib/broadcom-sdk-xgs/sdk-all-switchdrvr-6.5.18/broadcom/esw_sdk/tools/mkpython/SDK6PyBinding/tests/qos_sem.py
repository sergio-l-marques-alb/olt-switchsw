##############################################################################
#
# Test to check API semantics for selected QoS APIs.
# NOTE : Tests are intended for API semantics. Functionality not covered.
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.bcm.qos import *
from sdk6.bcm.l3 import *

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

def qos_sem_test_001():
    Test_Coverage = ""
    port=1
    fails = 0

    try:
        ingress_qos_map_id_ptr = int_ptr()
        ingress_qos_flags=BCM_QOS_MAP_L2|BCM_QOS_MAP_INGRESS
        rv = bcm_qos_map_create(0, ingress_qos_flags, ingress_qos_map_id_ptr)
        ingress_qos_map_id = ingress_qos_map_id_ptr.value()
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_qos_map_create")
    Test_Coverage += "BCM_QOS_MAP_L2\n"
    Test_Coverage += "BCM_QOS_MAP_INGRESS\n"
    Test_Coverage += "bcm_qos_map_create\n"

    try:
        entry = bcm_qos_map_t()
        entry.color=bcmColorGreen;
        entry.pkt_pri=0;
        entry.int_pri=1;
        rv = bcm_qos_map_add(0, ingress_qos_flags, entry, ingress_qos_map_id);
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_qos_map_add")
    Test_Coverage += "bcm_qos_map_t\n"
    Test_Coverage += "bcm_qos_map_add\n"

    try:
        egress_qos_map_id = 10
        rv = bcm_qos_port_map_set(0, port, ingress_qos_map_id, egress_qos_map_id)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_qos_port_map_set")
    Test_Coverage += "bcm_qos_port_map_set\n"

    print("\n******** API Coverage ********\n" + Test_Coverage)
    return fails

