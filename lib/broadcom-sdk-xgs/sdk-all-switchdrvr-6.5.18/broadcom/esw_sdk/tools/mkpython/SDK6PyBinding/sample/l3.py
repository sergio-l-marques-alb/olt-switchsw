##############################################################################
#
# Test to check selected L2,L3,FIELD APIs.
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################
from sdk6.sal.sal import *
from sdk6.bcm.types import *
from sdk6.bcm.l3 import *
from sdk6.bcm.l2 import *
from sdk6.bcm.port import *
from sdk6.bcm.vlan import *
from sdk6.bcm.switch import *
from sdk6.bcm.tunnel import *
from sdk6.bcm.field import *
from sdk6.bcm.policer import *
from sample.l3_lib import *

try:
    from regress_lib import VERBOSE
except:
    def VERBOSE(string):
        return

def l3_config_sample():
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


def test_l3_config_1():
    VERBOSE("Running Test test_l3_config_1.")
    unit=0
    vlan=2
    group=1
    port1=1
    port2=2
    gport = int_ptr()
    bcm_port_gport_get(unit, port1, gport)
    gport1 = gport.value()
    bcm_port_gport_get(unit, port2, gport)
    gport2 = gport.value()

    #l2 station add mac=0x001111111111 macm=0xffffffffffff ipv4=tru
    l2_station=bcm_l2_station_t()
    station_id=int_ptr()
    dst_mac=bcm_mac_t()
    dst_mac_mask=bcm_mac_t()
    dst_mac = mac_hex_str_to_addr("00:11:11:11:11:11")
    dst_mac_mask = mac_hex_str_to_addr("ff:ff:ff:ff:ff:ff")
    bcm_l2_station_t_init(l2_station)
    l2_station.dst_mac = dst_mac.cast()
    l2_station.dst_mac_mask = dst_mac_mask.cast()
    l2_station.flags=BCM_L2_STATION_IPV4
    rv = bcm_l2_station_add(unit, station_id, l2_station)
    if (rv < 0):
        print("Error : bcm_l2_station_add failed")
        return rv

    rv = bcm_l2_traverse(unit, l2_traverse_cb, None)
    if (rv < 0):
        print("Error : bcm_l2_traverse failed")
        return rv

    #switchcontrol l3egressmode=1
    type=bcmSwitchL3EgressMode
    rv = bcm_switch_control_set(unit, type, 1)
    if (rv < 0):
        print("Error : bcm_switch_control_set failed")
        return rv

    #vlan create 2 pbm=ce26,ce27 ubm=ce26,ce27
    pbmp = bcm_pbmp_t()
    upbmp = bcm_pbmp_t()
    rv = bcm_vlan_create(unit, vlan)
    if (rv < 0):
        print("Error : bcm_vlan_create failed")
        return rv

    pbmp = BCM_PBMP_CLEAR(pbmp)
    upbmp = BCM_PBMP_CLEAR(upbmp)
    pbmp, port1 = BCM_PBMP_PORT_ADD(pbmp, port1)
    pbmp, port2 = BCM_PBMP_PORT_ADD(pbmp, port2)
    upbmp, port1 = BCM_PBMP_PORT_ADD(upbmp, port1)
    upbmp, port2 = BCM_PBMP_PORT_ADD(upbmp, port2)
    rv = bcm_vlan_port_add(unit, vlan, pbmp, upbmp)
    if (rv < 0):
        print("Error : bcm_vlan_port_add failed")
        return rv

    #l3 intf add vlan=2 mac=0x11 intf=6
    l3_intf=bcm_l3_intf_t()
    bcm_l3_intf_t_init(l3_intf)
    l3_intf.l3a_flags = BCM_L3_WITH_ID
    l3_intf.l3a_vid = vlan
    mac=mac_hex_str_to_addr("00:00:00:00:00:11")
    l3_intf.l3a_mac_addr = mac.cast()
    l3_intf.l3a_intf_id = 6
    rv = bcm_l3_intf_create(unit, l3_intf)
    if (rv < 0):
        print("Error : bcm_l3_intf_create failed")
        return rv

    l3_intf_get=bcm_l3_intf_t()
    bcm_l3_intf_t_init(l3_intf_get)
    l3_intf_get.l3a_intf_id = 6
    rv = bcm_l3_intf_get(unit,l3_intf_get)
    if (rv < 0):
        print("Error : bcm_l3_intf_get failed")
        return rv

    #l3 egress add mac=00:11:11:11:11:11 intf=6 port=ce27
    l3_egr=bcm_l3_egress_t()
    bcm_l3_egress_t_init(l3_egr)
    l3_egr.intf = 6
    l3_egr.port = gport1
    mac=mac_hex_str_to_addr("00:11:11:11:11:11")
    l3_egr.mac_addr = mac.cast()
    egr_if=int_ptr()
    rv = bcm_l3_egress_create(unit, 0, l3_egr, egr_if)
    if (rv < 0):
        print("Error : bcm_l3_egress_create failed")
        return rv

    rv = bcm_l3_egress_traverse(unit,l3_traverse_cb, None)
    if (rv < 0):
        print("Error : bcm_l3_egress_traverse failed")
        return rv

    #l3 defip add ip=1.1.1.0 mask=255.255.255.0 intf=100002
    route=bcm_l3_route_t()
    bcm_l3_route_t_init(route)
    route.l3a_intf = 100002
    subnet="1.1.1.0"
    ip_a = subnet.split(".")
    ip_addr = int(ip_a[0]) << 24 | int(ip_a[1]) << 16 | int(ip_a[2]) << 8 | int(ip_a[3])
    route.l3a_subnet=ip_addr
    subnet="255.255.255.0"
    ip_a = subnet.split(".")
    ip_addr = int(ip_a[0]) << 24 | int(ip_a[1]) << 16 | int(ip_a[2]) << 8 | int(ip_a[3])
    route.l3a_ip_mask=ip_addr
    rv = bcm_l3_route_add(unit, route)
    if (rv < 0):
        print("Error : bcm_l3_route_add failed")
        return rv

    count=int_ptr()
    rv = bcm_l3_route_max_ecmp_get(unit,count)
    if (rv < 0):
        print("Error : bcm_l3_route_max_ecmp_get failed")
        return rv

    #fp init
    rv = bcm_field_init(unit)
    if (rv < 0):
        print("Error : bcm_l3_route_max_ecmp_get failed")
        return rv

    #fp qset clear
    #fp qset add inports
    qset = bcm_field_qset_t()
    qset = BCM_FIELD_QSET_INIT(qset)
    qset, temp = BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress)
    qset, temp = BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts)

    #fp group create 0 0
    rv = bcm_field_group_create_id(unit,qset,0,group)
    if (rv < 0):
        print("Error : bcm_field_group_create_id failed")
        return rv

    # bcm_field_show not supported via RPC
    # rv = bcm_field_show(unit,group)
    # if (rv < 0):
    #     print("Error : bcm_field_show failed")
    #     return rv

    #fp entry create 0 0
    rv = bcm_field_entry_create_id(unit,group,1)
    if (rv < 0):
        print("Error : bcm_field_entry_create_id failed")
        return rv

    #fp qual inports 
    rv = bcm_field_qualify_InPorts(unit,1,pbmp,pbmp)
    if (rv < 0):
        print("Error : bcm_field_qualify_InPorts failed")
        return rv

    rv = bcm_field_entry_install(unit,1)
    if (rv < 0):
        print("Error : bcm_field_entry_install failed")
        return rv

    #fp policer create mode=committed cir=1000 cbs=500 colorblind=1
    policer = bcm_policer_config_t()
    policer_id = int_ptr()
    bcm_policer_config_t_init(policer)
    policer.mode = bcmPolicerModeCommitted
    policer.flags = BCM_POLICER_COLOR_BLIND
    policer.ckbits_sec = 1000
    policer.ckbits_burst = 500
    rv = bcm_policer_create(unit, policer, policer_id)
    if (rv < 0):
        print("Error : bcm_policer_create failed")
        return rv

    #fp policer attach entry=0 level=0 polid=1
    rv = bcm_field_entry_policer_attach(unit, 1, 0, policer_id.value())
    if (rv < 0):
        print("Error : bcm_field_entry_policer_attach failed")
        return rv

    #fp action add 0 rpdrop
    rv = bcm_field_action_add(unit, 1, bcmFieldActionRpDrop, 0, 0)
    if (rv < 0):
        print("Error : bcm_field_action_add failed")
        return rv

    #cleanup
    rv = bcm_field_action_delete(unit, 1, bcmFieldActionRpDrop, 0, 0)
    if (rv < 0):
        print("Error : bcm_field_action_delete failed")
        return rv

    rv = bcm_field_entry_policer_detach(unit,1,0)
    if (rv < 0):
        print("Error : bcm_field_entry_policer_detach failed")
        return rv

    rv = bcm_field_entry_destroy(unit,1)
    if (rv < 0):
        print("Error : bcm_field_entry_destroy failed")
        return rv

    rv = bcm_field_group_destroy(unit,1)
    if (rv < 0):
        print("Error : bcm_field_group_destroy failed")
        return rv

    route=bcm_l3_route_t()
    bcm_l3_route_t_init(route)
    route.l3a_intf = 100002
    subnet="1.1.1.0"
    ip_a = subnet.split(".")
    ip_addr = int(ip_a[0]) << 24 | int(ip_a[1]) << 16 | int(ip_a[2]) << 8 | int(ip_a[3])
    route.l3a_subnet=ip_addr
    subnet="255.255.255.0"
    ip_a = subnet.split(".")
    ip_addr = int(ip_a[0]) << 24 | int(ip_a[1]) << 16 | int(ip_a[2]) << 8 | int(ip_a[3])
    route.l3a_ip_mask=ip_addr
    rv = bcm_l3_route_delete(unit, route)
    if (rv < 0):
        print("Error : bcm_l3_route_delete failed")
        return rv

    rv = bcm_l3_egress_destroy(unit,100002)
    if (rv < 0):
        print("Error : bcm_l3_egress_destroy failed")
        return rv

    l3_intf=bcm_l3_intf_t()
    bcm_l3_intf_t_init(l3_intf)
    l3_intf.l3a_flags = BCM_L3_WITH_ID
    l3_intf.l3a_intf_id = 6
    rv = bcm_l3_intf_delete(unit, l3_intf)
    if (rv < 0):
        print("Error : bcm_l3_intf_delete failed")
        return rv

    rv = bcm_vlan_destroy(unit,2)
    if (rv < 0):
        print("Error : bcm_vlan_destroy failed")
        return rv

    rv = bcm_l2_station_delete_all(unit)
    if (rv < 0):
        print("Error : bcm_l2_station_delete_all failed")
        return rv

    return 0


def test_l3_config_2():
    VERBOSE("Running Test test_l3_config_2.")
    unit=0
    vlan=2
    port1=1
    port2=2
    port3=3
    gport = int_ptr()
    rv = bcm_port_gport_get(unit, port1, gport)
    if (rv < 0):
        print("Error : bcm_port_gport_get failed")
        return rv
    gport1 = gport.value()

    rv = bcm_port_gport_get(unit, port2, gport)
    if (rv < 0):
        print("Error : bcm_port_gport_get failed")
        return rv
    gport2 = gport.value()

    rv = bcm_port_gport_get(unit, port3, gport)
    if (rv < 0):
        print("Error : bcm_port_gport_get failed")
        return rv
    gport3 = gport.value()

    #l2 station add mac=0x001111111111 macm=0xffffffffffff ipv6=true
    l2_station=bcm_l2_station_t()
    station_id=int_ptr()
    dst_mac=bcm_mac_t()
    dst_mac_mask=bcm_mac_t()
    dst_mac = mac_hex_str_to_addr("00:11:11:11:11:11")
    dst_mac_mask = mac_hex_str_to_addr("ff:ff:ff:ff:ff:ff")
    bcm_l2_station_t_init(l2_station)
    l2_station.dst_mac = dst_mac.cast()
    l2_station.dst_mac_mask = dst_mac_mask.cast()
    l2_station.flags=BCM_L2_STATION_IPV6
    rv = bcm_l2_station_add(unit, station_id, l2_station)
    if (rv < 0):
        print("Error : bcm_l2_station_add failed")
        return rv

    #switchcontrol l3egressmode=1
    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1)
    if (rv < 0):
        print("Error : bcm_switch_control_set failed")
        return rv

    #vlan create 2 pbm=ce1,ce2,ce3 ubm=ce1,ce2,ce3
    pbmp = bcm_pbmp_t()
    upbmp = bcm_pbmp_t()
    rv = bcm_vlan_create(unit, vlan)
    if (rv < 0):
        print("Error : bcm_vlan_create failed")
        return rv

    pbmp = BCM_PBMP_CLEAR(pbmp)
    upbmp = BCM_PBMP_CLEAR(upbmp)
    pbmp, port1 = BCM_PBMP_PORT_ADD(pbmp, port1)
    pbmp, port2 = BCM_PBMP_PORT_ADD(pbmp, port2)
    pbmp, port3 = BCM_PBMP_PORT_ADD(pbmp, port3)
    upbmp, port1 = BCM_PBMP_PORT_ADD(upbmp, port1)
    upbmp, port2 = BCM_PBMP_PORT_ADD(upbmp, port2)
    upbmp, port3 = BCM_PBMP_PORT_ADD(upbmp, port3)
    rv = bcm_vlan_port_add(unit, vlan, pbmp, upbmp)
    if (rv < 0):
        print("Error : bcm_vlan_port_add failed")
        return rv

    #l3 intf add vlan=2 mac=0x11 intf=6
    l3_intf=bcm_l3_intf_t()
    bcm_l3_intf_t_init(l3_intf)
    l3_intf.l3a_flags = BCM_L3_WITH_ID
    l3_intf.l3a_vid = vlan
    mac=mac_hex_str_to_addr("00:00:00:00:00:11")
    l3_intf.l3a_mac_addr = mac.cast()
    l3_intf.l3a_intf_id = 6
    rv = bcm_l3_intf_create(unit, l3_intf)
    if (rv < 0):
        print("Error : bcm_l3_intf_create failed")
        return rv

    #l3 egress add mac=00:11:11:11:11:11 intf=6 port=ce1
    l3_egr=bcm_l3_egress_t()
    bcm_l3_egress_t_init(l3_egr)
    l3_egr.intf = 6
    l3_egr.port = gport1
    mac=mac_hex_str_to_addr("00:11:11:11:11:11")
    l3_egr.mac_addr = mac.cast()
    egr_if=int_ptr()
    rv = bcm_l3_egress_create(unit, 0, l3_egr, egr_if)
    if (rv < 0):
        print("Error : bcm_l3_egress_create failed")
        return rv

    #l3 egress add mac=00:11:11:11:11:11 intf=6 port=ce2
    l3_egr=bcm_l3_egress_t()
    bcm_l3_egress_t_init(l3_egr)
    l3_egr.intf = 6
    l3_egr.port = gport2
    mac=mac_hex_str_to_addr("00:11:11:11:11:11")
    l3_egr.mac_addr = mac.cast()
    egr_if=int_ptr()
    rv = bcm_l3_egress_create(unit, 0, l3_egr, egr_if)
    if (rv < 0):
        print("Error : bcm_l3_egress_create failed")
        return rv

    #l3 egress add mac=00:11:11:11:11:11 intf=6 port=ce3
    l3_egr=bcm_l3_egress_t()
    bcm_l3_egress_t_init(l3_egr)
    l3_egr.intf = 6
    l3_egr.port = gport3
    mac=mac_hex_str_to_addr("00:11:11:11:11:11")
    l3_egr.mac_addr = mac.cast()
    egr_if=int_ptr()
    rv = bcm_l3_egress_create(unit, 0, l3_egr, egr_if)
    if (rv < 0):
        print("Error : bcm_l3_egress_create failed")
        return rv

    #l3 multipath add size=3 intf0=100002 intf1=100003 intf2=100004
    mpintf=int_ptr()
    intf_array=int_array(3)
    intf_array[0]=100002
    intf_array[1]=100003
    intf_array[2]=100004
    rv = bcm_l3_egress_multipath_create(unit, 0, 3, intf_array, mpintf)
    if (rv < 0):
        print("Error : bcm_l3_egress_multipath_create failed")
        return rv

    #l3 ip6route add ip=2001:0001:0001:0000:0000:0000:0000:0000 masklen=120 intf=200256 ecmp=1
    route=bcm_l3_route_t()
    bcm_l3_route_t_init(route)
    route.l3a_intf = 100002
    subnet=ip6_hex_str_to_addr("2001:0001:0001:0000:0000:0000:0000:0000")
    route.l3a_ip6_net=subnet.cast()
    mask=ip6_hex_str_to_addr("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00")
    route.l3a_ip6_mask=mask.cast()
    route.flags=BCM_L3_IP6
    rv = bcm_l3_route_add(unit, route)
    if (rv < 0):
        print("Error : bcm_l3_route_add failed")
        return rv

    rv = bcm_l3_egress_multipath_traverse(unit,l3_multipath_traverse_cb, None)
    if (rv < 0):
        print("Error : bcm_l3_egress_multipath_traverse failed")
        return rv

    #cleanup
    route=bcm_l3_route_t()
    bcm_l3_route_t_init(route)
    route.l3a_intf = 100002
    subnet=ip6_hex_str_to_addr("2001:0001:0001:0000:0000:0000:0000:0000")
    route.l3a_ip6_net=subnet.cast()
    mask=ip6_hex_str_to_addr("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00")
    route.l3a_ip6_mask=mask.cast()
    route.flags=BCM_L3_IP6
    rv = bcm_l3_route_delete(unit, route)
    if (rv < 0):
        print("Error : bcm_l3_route_delete failed")
        return rv

    rv = bcm_l3_egress_multipath_destroy(unit,mpintf.value())
    if (rv < 0):
        print("Error : bcm_l3_egress_multipath_destroy failed")
        return rv

    rv = bcm_l3_egress_destroy(unit,100002)
    if (rv < 0):
        print("Error : bcm_l3_egress_destroy failed")
        return rv

    rv = bcm_l3_egress_destroy(unit,100003)
    if (rv < 0):
        print("Error : bcm_l3_egress_destroy failed")
        return rv

    rv = bcm_l3_egress_destroy(unit,100004)
    if (rv < 0):
        print("Error : bcm_l3_egress_destroy failed")
        return rv


    l3_intf=bcm_l3_intf_t()
    bcm_l3_intf_t_init(l3_intf)
    l3_intf.l3a_flags = BCM_L3_WITH_ID
    l3_intf.l3a_intf_id = 6
    rv = bcm_l3_intf_delete(unit, l3_intf)
    if (rv < 0):
        print("Error : bcm_l3_intf_delete failed")
        return rv

    rv = bcm_vlan_destroy(unit,2)
    if (rv < 0):
        print("Error : bcm_vlan_destroy failed")
        return rv

    rv = bcm_l2_station_delete_all(unit)
    if (rv < 0):
        print("Error : bcm_l2_station_delete_all failed")
        return rv

    return 0
