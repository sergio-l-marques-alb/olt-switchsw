from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.bcm.l2 import *
from sdk6.bcm.l3 import *
from sdk6.bcm.vlan import *
from sdk6.bcm.switch import *

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


unit = 0
enable = 1
disable = 0

my_station = bcm_l2_station_t()
my_station.flags = BCM_L2_STATION_IPV6
mac = mac_hex_str_to_addr("00:11:11:11:11:11")
my_station.dst_mac = mac.cast()
mac = mac_hex_str_to_addr("FF:FF:FF:FF:FF:FF")
my_station.dst_mac_mask = mac.cast()
my_station_id_ptr = int_ptr()
bcm_l2_station_add(unit, my_station_id_ptr, my_station)
print (my_station_id_ptr.value())

vlan_id = 2
bcm_vlan_create(unit, vlan_id)
my_pbm = bcm_pbmp_t()
my_ubm = bcm_pbmp_t()
my_pbm, tmp = BCM_PBMP_PORT_SET(my_pbm, 69);
my_pbm, tmp = BCM_PBMP_PORT_ADD(my_pbm, 70);
my_pbm, tmp = BCM_PBMP_PORT_ADD(my_pbm, 71);
my_ubm, my_pbm = BCM_PBMP_ASSIGN(my_ubm, my_pbm);
bcm_vlan_port_add(unit, vlan_id, my_pbm, my_ubm);

bcm_switch_control_set(unit, bcmSwitchL3EgressMode, enable)

my_intf = bcm_l3_intf_t()
bcm_l3_intf_t_init(my_intf)
my_intf.l3a_flags = BCM_L3_ADD_TO_ARL
mac = mac_hex_str_to_addr("00:00:00:00:00:11")
my_intf.l3a_mac_addr = mac.cast()
my_intf.l3a_vid = vlan_id
bcm_l3_intf_create(unit, my_intf)

bcm_port_control_set(unit, 69, bcmPortControlVrf, 2)
bcm_port_control_set(unit, 70, bcmPortControlVrf, 3)
bcm_port_control_set(unit, 71, bcmPortControlVrf, 4)

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

my_egress1 = bcm_l3_egress_t()
my_int1_ptr = int_ptr()
bcm_l3_egress_t_init(my_egress1)
mac = mac_hex_str_to_addr("00:11:11:11:11:11")
my_egress1.mac_addr = mac.cast()
my_egress1.port = 71
my_egress1.intf = my_intf.l3a_intf_id
bcm_l3_egress_create(unit, 0, my_egress1, my_int1_ptr);
my_int1 = my_int1_ptr.value()

my_route1 = bcm_l3_route_t()
bcm_l3_route_t_init(my_route1)
my_route1.l3a_intf = my_int1
my_route1.l3a_flags = BCM_L3_IP6
ip6 = ip6_hex_str_to_addr("2001:0001:0001:0000:0000:0000:0000:0000")
my_route1.l3a_ip6_net = ip6.cast();
ip6 = ip6_hex_str_to_addr("ffff:ffff:ffff:ffff:0000:0000:0000:0000")
my_route1.l3a_ip6_mask = ip6.cast()
my_route1.l3a_vrf = 3
bcm_l3_route_add(unit, my_route1)

my_egress2 = bcm_l3_egress_t()
my_int2_ptr = int_ptr()
bcm_l3_egress_t_init(my_egress2)
mac = mac_hex_str_to_addr("00:11:11:11:11:11")
my_egress2.mac_addr = mac.cast()
my_egress2.port = 69
my_egress2.intf = my_intf.l3a_intf_id
bcm_l3_egress_create(unit, 0, my_egress2, my_int2_ptr);
my_int2 = my_int2_ptr.value()

my_route2 = bcm_l3_route_t()
bcm_l3_route_t_init(my_route2)
my_route2.l3a_intf = my_int2
my_route2.l3a_flags = BCM_L3_IP6
ip6 = ip6_hex_str_to_addr("2001:0001:0001:0000:0000:0000:0000:0000")
my_route2.l3a_ip6_net = ip6.cast();
ip6 = ip6_hex_str_to_addr("ffff:ffff:ffff:ffff:0000:0000:0000:0000")
my_route2.l3a_ip6_mask = ip6.cast()
my_route2.l3a_vrf = 4
bcm_l3_route_add(unit, my_route2)

