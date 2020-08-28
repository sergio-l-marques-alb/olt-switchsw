##############################################################################
#
# L3 Library file getting used by l3_config1/2.py files
# This library file can be sourced in any test file
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

try:
    from regress_lib import VERBOSE
except:
    def VERBOSE(string):
        return


def tunnel_initiator_setup(unit, sip, dip, l3if):
    tunnel_init = bcm_tunnel_initiator_t()
    l3_intf = bcm_l3_intf_t()
    bcm_tunnel_initiator_t_init(tunnel_init)

    l3_intf.l3a_intf_id = l3if
    tunnel_init.ttl = 10
    tunnel_init.type = bcmTunnelTypeIp4In4
    tunnel_init.dip = dip
    tunnel_init.sip = sip
    rv = bcm_tunnel_initiator_set(unit, l3_intf, tunnel_init)

    return rv

def set_up_packet_spray(unit, port):
    bcm_switch_control_set(unit, bcmSwitchECMPLevel1RandomSeed, 0xff)
    bcm_switch_control_set(unit, bcmSwitchECMPLevel2RandomSeed, 0xf011)
    bcm_port_control_set(unit, port, \
        bcmPortControlECMPLevel1LoadBalancingRandomizer, 8)
    bcm_port_control_set(unit, port, \
        bcmPortControlECMPLevel2LoadBalancingRandomizer, 5)

def rtag7_config(unit):
    flags = 0

    intp = int_ptr()
    bcm_switch_control_get(unit, bcmSwitchHashControl, intp)
    flags = intp.value()

    flags |= BCM_HASH_CONTROL_ECMP_ENHANCE | BCM_HASH_CONTROL_MULTIPATH_DIP
    bcm_switch_control_set(unit, bcmSwitchHashControl, flags)

    # Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP)
    bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, 0)
    flags = BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT | BCM_HASH_FIELD_PROTOCOL | BCM_HASH_FIELD_DSTL4 | BCM_HASH_FIELD_SRCL4 | BCM_HASH_FIELD_IP4DST_LO | BCM_HASH_FIELD_IP4DST_HI | BCM_HASH_FIELD_IP4SRC_LO |     BCM_HASH_FIELD_IP4SRC_HI

    # Block A - L3 packet field selection.
    bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags)
    bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags)

    # Block B - L3 packet field selection
    bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags)
    bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags)

    # Configure HASH A and HASH B functions
    bcm_switch_control_set(unit, bcmSwitchHashField0Config, BCM_HASH_FIELD_CONFIG_CRC32LO)
    bcm_switch_control_set(unit, bcmSwitchHashField1Config, BCM_HASH_FIELD_CONFIG_CRC32HI)

    # Enable preprocess.
    bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, enable)
    bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, enable)

    # Configure Seed.
    bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x55555555)
    bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555)

def resilient_hashing_config(unit):
    bcm_switch_control_set(unit, bcmSwitchEcmpMacroFlowHashEnable, 1)
    bcm_switch_control_set(unit, bcmSwitchMacroFlowEcmpHashConcatEnable, 1)

    bcm_switch_control_set(unit, bcmSwitchMacroFlowECMPHashSeed, 0x321)
    bcm_switch_control_set(unit, bcmSwitchECMPHashSet0Offset, 5)
    bcm_switch_control_set(unit, bcmSwitchECMPHashSet1Offset, 3)

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

def create_egr_obj(unit, l3_if, nh_mac, gport, intp):
    l3_egress = bcm_l3_egress_t()
    bcm_l3_egress_t_init(l3_egress)
    l3_egress.mac_addr = nh_mac.cast()
    l3_egress.intf  = l3_if
    l3_egress.port = gport
    return bcm_l3_egress_create(unit, 0, l3_egress, intp)

def vlan_create_add_port(unit, vid,  port):
    pbmp = bcm_pbmp_t()
    upbmp = bcm_pbmp_t()
    bcm_vlan_create(unit, vid)
    pbmp = BCM_PBMP_CLEAR(pbmp)
    upbmp = BCM_PBMP_CLEAR(upbmp)
    pbmp, port = BCM_PBMP_PORT_ADD(pbmp, port);
    bcm_vlan_port_add(unit, vid, pbmp, upbmp)

def create_l3_interface(unit, flags, local_mac, vid, intp):
    l3_intf = bcm_l3_intf_t()
    # L3 interface and egress object for access
    bcm_l3_intf_t_init(l3_intf)
    l3_intf.l3a_flags |= flags
    l3_intf.l3a_mac_addr = local_mac.cast()
    l3_intf.l3a_vid = vid
    bcm_l3_intf_create(unit, l3_intf)
    intp.assign(l3_intf.l3a_intf_id)

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


def l2_traverse_cb(unit,l2_s,userdata):
    VERBOSE("L2 callback called")
    return 1

def l3_traverse_cb(unit,intf,l3_s,userdata):
    VERBOSE("L3 callback called")
    return 1

def l3_multipath_traverse_cb(unit,mpintf,count,l3_s,userdata):
    VERBOSE("L3 multipath callback called")
    return 1
