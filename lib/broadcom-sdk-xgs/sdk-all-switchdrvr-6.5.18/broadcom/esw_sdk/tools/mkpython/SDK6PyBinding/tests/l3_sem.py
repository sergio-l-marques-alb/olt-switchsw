##############################################################################
#
# Test to check API semantics for selected L3 APIs.
# NOTE : Tests are intended for API semantics. Functionality not covered.
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.bcm.vlan import *
from sdk6.bcm.l3 import *
from sdk6.bcm.l2 import *
from sdk6.bcm.switch import *

# Construct mac address from mac string.
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

def ipv4_str_to_addr(ipv4_str):
    ip_a = ipv4_str.split(".")
    ip_addr = int(ip_a[0]) << 24 | int(ip_a[1]) << 16 | int(ip_a[2]) << 8 | int(ip_a[3])
    return hex(ip_addr)

def ip6_hex_str_to_addr(ip6_str):
    ip6a = bcm_ip6_t()
    i = 0
    alist = ip6_str.split(":")
    if len(alist) > 8:
        print("Invalid IPv6 address " + ip6_str)
        return None
    for a in alist:
        b = a[0:2]
        c = a[2:4]
        try:
            ip6a[i] = int(b, 16)
            ip6a[i+1] = int(c, 16)
            i += 2
        except:
            print("Invalid IPv6 Address '" + a + " ' in " + ip6_str)
            return None
    return ip6a

def l3_sem_test_001():
    Test_Coverage = ""
    unit = 0
    ing_vid = 100
    egr_vid = 200
    ing_port = 1
    egr_port = 2
    vrf = 1
    fails = 0
    src_ip = ipv4_str_to_addr("192.168.1.1")
    dst_ip = ipv4_str_to_addr("192.169.2.1")
    subnet = ipv4_str_to_addr("192.168.2.0")
    mask = ipv4_str_to_addr("255.255.255.0")
    smac = mac_hex_str_to_addr("00:00:00:00:01:01")
    dmac = mac_hex_str_to_addr("00:00:00:00:02:02")
    nhmac = mac_hex_str_to_addr("00:00:00:00:03:03")
    rtrmac = mac_hex_str_to_addr("00:00:00:00:04:04")

    if smac == None or dmac == None:
        print("Error in forming MAC address")
        fails += 1
    Test_Coverage += "bcm_mac_t\n"

    try:
        vlan_valid = BCM_VLAN_VALID(ing_vid)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in BCM_VLAN_VALID")
    Test_Coverage += "BCM_VLAN_VALID\n"

    # Create ingress VLAN
    try:
        rv = bcm_vlan_create(unit, ing_vid)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_vlan_create")
    Test_Coverage += "bcm_vlan_create\n"

    # Add ports to ingress VLAN
    try:
        rv = bcm_vlan_gport_add(unit, ing_vid, ing_port, 0)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_vlan_gport_add")
    Test_Coverage += "bcm_vlan_gport_add\n"

    # Enable L2 static move to CPU.
    try:
        rv = bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, 1)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error enabling bcmSwitchL2StaticMoveToCpu")
    Test_Coverage += "bcm_switch_control_set\n"

    # Get switch control.
    try:
        egr_mode_ptr = int_ptr()
        rv = bcm_switch_control_get(unit, bcmSwitchL3EgressMode, egr_mode_ptr)
        egr_mode = egr_mode_ptr.value()
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error enabling bcmSwitchL3EgressMode")
    Test_Coverage += "bcm_switch_control_get\n"

    # Create Egress object
    try:
        l3_egr_s = bcm_l3_egress_t()
        egr_id_p = int_ptr()
        l3_egr_s.mac_addr = nhmac.cast()
        l3_egr_s.vlan = egr_vid
        l3_egr_s.port = egr_port
        l3_egr_s.intf = 1
        l3_egr_s.flags = 0
        rv = bcm_l3_egress_create(unit, 0, l3_egr_s, egr_id_p)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_l3_egress_create")
    Test_Coverage += "bcm_l3_egress_t\n"
    Test_Coverage += "bcm_l3_egress_create\n"

    try:
        egr_id = egr_id_p.value()
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in reading scalar")

    # Create L3 interface
    try:
        l3_intf_s = bcm_l3_intf_t()
        l3_intf_s.l3a_intf_id = egr_id
        l3_intf_s.l3a_subnet = subnet
        l3_intf_s.l3a_ip_mask = mask
        l3_intf_s.l3a_vrf = vrf
        rv = bcm_l3_intf_create(unit, l3_intf_s)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_l3_intf_create")
    Test_Coverage += "bcm_l3_intf_t\n"
    Test_Coverage += "bcm_l3_intf_create\n"

    # Add L2 dest entry with L3 bit
    try:
        l2_s = bcm_l2_addr_t()
        l2_s.mac = dmac.cast()
        l2_s.vid = ing_vid
        l2_s.flags = BCM_L2_L3LOOKUP
        rv = bcm_l2_addr_add(unit, l2_s)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_l2_addr_add")
    Test_Coverage += "bcm_l2_addr_t\n"
    Test_Coverage += "BCM_L2_L3LOOKUP\n"
    Test_Coverage += "bcm_l2_addr_add\n"

    # Delete MAC
    try:
        rv = bcm_l2_addr_delete(unit, dmac.cast(), ing_vid)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_l2_addr_delete")
    Test_Coverage += "bcm_l2_addr_delete\n"

    # Delete l3 interface
    try:
        rv = bcm_l3_intf_delete(unit, l3_intf_s)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_l3_intf_delete")
    Test_Coverage += "bcm_l3_intf_delete\n"

    # Delete egress object
    try:
        rv = bcm_l3_egress_destroy(unit, egr_id)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_l3_egress_destroy")
    Test_Coverage += "bcm_l3_egress_destroy\n"

    print("\n******** API Coverage ********\n" + Test_Coverage)
    return fails


def l3_sem_test_002():
    Test_Coverage = ""
    fails = 0
    unit = 0
    enable = 1
    disable = 0
    vlan_id = 2

    try:
        my_station = bcm_l2_station_t()
        my_station.flags = BCM_L2_STATION_IPV6
        mac = mac_hex_str_to_addr("00:11:11:11:11:11")
        my_station.dst_mac = mac.cast()
        mac = mac_hex_str_to_addr("FF:FF:FF:FF:FF:FF")
        my_station.dst_mac_mask = mac.cast()
        my_station_id_ptr = int_ptr()
        bcm_l2_station_add(unit, my_station_id_ptr, my_station)
    except (TypeError, NameError) as err:
        fails += 1
        print(err + " Error in bcm_l2_station_add")
    Test_Coverage += "bcm_l2_station_add\n"

    try:
        bcm_vlan_create(unit, vlan_id)
        my_pbm = bcm_pbmp_t()
        my_ubm = bcm_pbmp_t()
        my_pbm, tmp = BCM_PBMP_PORT_SET(my_pbm, 69);
        my_pbm, tmp = BCM_PBMP_PORT_ADD(my_pbm, 70);
        my_pbm, tmp = BCM_PBMP_PORT_ADD(my_pbm, 71);
        my_ubm, my_pbm = BCM_PBMP_ASSIGN(my_ubm, my_pbm);
        bcm_vlan_port_add(unit, vlan_id, my_pbm, my_ubm);
    except (TypeError, NameError) as err:
        fails += 1
        print(err + " Error in bcm_vlan_port_add")
    Test_Coverage += "bcm_vlan_create\n"
    Test_Coverage += "bcm_pbmp_t\n"
    Test_Coverage += "BCM_PBMP_PORT_SET\n"
    Test_Coverage += "BCM_PBMP_PORT_ADD\n"
    Test_Coverage += "BCM_PBMP_PORT_ASSIGN\n"
    Test_Coverage += "bcm_vlan_port_add\n"

    try:
        my_intf = bcm_l3_intf_t()
        bcm_l3_intf_t_init(my_intf)
        my_intf.l3a_flags = BCM_L3_ADD_TO_ARL
        mac = mac_hex_str_to_addr("00:00:00:00:00:11")
        my_intf.l3a_mac_addr = mac.cast()
        my_intf.l3a_vid = vlan_id
        bcm_l3_intf_create(unit, my_intf)

        my_egress = bcm_l3_egress_t()
        bcm_l3_egress_t_init(my_egress)
        my_int_ptr = int_ptr()
        mac = mac_hex_str_to_addr("00:11:11:11:11:11")
        my_egress.mac_addr = mac.cast()
        my_egress.port = 70
        my_egress.intf = my_intf.l3a_intf_id;
        bcm_l3_egress_create(unit, 0, my_egress, my_int_ptr);
        my_int = my_int_ptr.value()

        my_route = bcm_l3_route_t()
        bcm_l3_route_t_init(my_route)
        my_route.l3a_intf = my_int
        my_route.l3a_flags = BCM_L3_IP6
        ip6 = ip6_hex_str_to_addr("2001:0001:0001:0000:0000:0000:0000:0000")
        my_route.l3a_ip6_net = ip6.cast()
        ip6 = ip6_hex_str_to_addr("ffff:ffff:ffff:ffff:0000:0000:0000:0000")
        my_route.l3a_ip6_mask = ip6.cast()
        my_route.l3a_vrf = 2
        bcm_l3_route_add(unit, my_route)
    except (TypeError, NameError) as err:
        fails += 1
        print("Error in bcm_l3_route_add")
    Test_Coverage += "IPv6 address set\n"
    Test_Coverage += "bcm_l3_egress_t\n"
    Test_Coverage += "bcm_l3_egress_create\n"
    Test_Coverage += "bcm_l3_route_t\n"
    Test_Coverage += "bcm_l3_route_add\n"

    print("\n******** API Coverage ********\n" + Test_Coverage)
    return fails
