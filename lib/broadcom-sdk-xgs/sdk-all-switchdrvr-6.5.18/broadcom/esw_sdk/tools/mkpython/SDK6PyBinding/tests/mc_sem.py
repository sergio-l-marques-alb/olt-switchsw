##############################################################################
#
# Test to check API semantics for selected multicast APIs.
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
from sdk6.bcm.multicast import *
from sdk6.bcm.switch import *

def mc_sem_test_001():
    Test_Coverage = ""
    unit = 0
    vlan = 200
    match_vlan = 100
    phy_port_1 = 1
    gport1 = 0
    fails = 0

    # Enable L3 egress mode
    try:
        rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode,1)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_switch_control_set")
    Test_Coverage += "bcm_switch_control_set\n"

    # Create VLAN
    try:
        rv = bcm_vlan_create (unit, vlan)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_vlan_create")
    Test_Coverage += "bcm_vlan_create\n"

    # Create MC Group
    try:
        group_ptr = int_ptr()
        rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VLAN, group_ptr)
        group = group_ptr.value()
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_multicast_create")
    Test_Coverage += "BCM_MULTICAST_TYPE_VLAN\n"
    Test_Coverage += "bcm_multicast_create\n"


    # VLAN control
    try:
        vlan_control = bcm_vlan_control_vlan_t()
        rv = bcm_vlan_control_vlan_get(unit, vlan, vlan_control)
        vlan_control.broadcast_group = group;
        vlan_control.unknown_multicast_group = group;
        vlan_control.unknown_unicast_group = group;
        vlan_control.flags = (vlan_control.flags & ~(BCM_VLAN_LEARN_DISABLE))
        rv = bcm_vlan_control_vlan_set(unit, vlan, vlan_control)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_vlan_control_vlan_set/get")
    Test_Coverage += "bcm_vlan_control_vlan_t\n"
    Test_Coverage += "BCM_VLAN_LEARN_DISABLE\n"
    Test_Coverage += "bcm_vlan_control_vlan_set\n"
    Test_Coverage += "bcm_vlan_control_vlan_get\n"


    try:
        rv = bcm_vlan_control_port_set(unit, phy_port_1, bcmVlanTranslateIngressEnable, 1)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_vlan_control_port_set")
    Test_Coverage += "bcm_vlan_control_port_set\n"

    try:
        vlan_port_1 = bcm_vlan_port_t()
        vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port_1.match_vlan = match_vlan;
        vlan_port_1.egress_vlan = vlan;
        vlan_port_1.port = gport1;
        rv = bcm_vlan_port_create (unit, vlan_port_1)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_vlan_port_create")
    Test_Coverage += "bcm_vlan_port_t\n"
    Test_Coverage += "bcm_vlan_port_create\n"

    try:
        rv = bcm_vlan_gport_add (unit, vlan, vlan_port_1.vlan_port_id, 0)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_vlan_gport_add")
    Test_Coverage += "bcm_vlan_gport_add\n"

    try:
        encap_ptr = int_ptr()
        rv = bcm_multicast_vlan_encap_get (unit, group, gport1, vlan_port_1.vlan_port_id, encap_ptr);
        encap = encap_ptr.value()
        rv = bcm_multicast_egress_add (unit, group, vlan_port_1.vlan_port_id, encap)
    except (TypeError, NameError) as err:
        fails += 1
        print(err)
        print("Error in bcm_multicast_vlan_encap_get/bcm_multicast_egress_add")
    Test_Coverage += "bcm_multicast_vlan_encap_get\n"
    Test_Coverage += "bcm_multicast_egress_add\n"

    print("\n******** API Coverage ********\n" + Test_Coverage)
    return fails

