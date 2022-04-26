$Id: README.txt,v 1.99 2017/10/25 15:02:00 sd015526  Exp $
 $Copyright: (c) 2021 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$

 ~ Name:
+------------------------------------------+
|    Port Extender L2 MC Control Bridge    |
+------------------------------------------+
 ~ Description:
    An example of a port extender control Bridge configuration for L2 multicast traffic.
    - Configuring the device to be a control bridge and prepend attributes
    - Configuring a VSI and attaching cascaded ports to it
    - Configuring unicast and multicast port extender LIFs
    - Creating L2 multicast groups
    - Adding port extender CUDs (MC OutLif) to the multicast group
    - Adding unicast and multicast MAC entries

 ~ File Reference:
    utility/cint_dnx_utils_extender.c
    cint_dnx_port_extender_cb_l2_mc.c


 ~ Name:
+---------------------------------------+
|    Port Extender UC Control Bridge    |
+---------------------------------------+
 ~ Description:
    An example of a port extender control bridge configuration for UC traffic.
     - Configuring the device to be a control bridge and prepend attributes
     - Configuring a VSI
     - Attaching cascaded ports to the VSI
     - Configuring port extender LIFs
     - Attaching end stations to the VSI

 ~ File Reference:
    cint_dnx_port_extender_cb_uc.c


 ~ Name:
+------------------------------------------+
|    Port Extender L3 MC Control Bridge    |
+------------------------------------------+
 ~ Description:
    An example of a port extender control bridge configuration for L3 ,unicast traffic.
    - Configuring the device to be a control bridge and prepend attributes
    - Configuring port extender LIFs
    - Configuring VSI per each port extender and attaching the ports to it
    - Creating L3 multicast Groups
    - Creating L3 interface per each port extender
    - Adding port extender L2 (OutLif) and L3 (OuRif) CUDs to the multicast group
    - Adding routing table entry

 ~ File Reference:
    utility/cint_dnx_utils_extender.c
    cint_dnx_port_extender_cb_l3_mc.c


 ~ Name:
+-----------------------------+
|    Routing Over Extender    |
+-----------------------------+
 ~ Description:
    An example of routing over extender port.

 ~ File Reference:
    cint_dnx_route_over_extender.c
    utility/cint_dnx_utils_extender.c


 ~ Name:
+----------------------------------+
|    General utility functions     |
+----------------------------------+
 ~ Description:
    General utility functions of Extender

 ~ File Reference:
    utility/cint_dnx_utils_extender.c

~ Name:
+---------------------+
|    Split Horizon    |
+---------------------+
 ~ Description:
    Demonstrates Split Horizon trapping.

 ~ File Reference:
    cint_dnx_vswitch_split_horizon.c

  ~ Name:
+-------------------------+
|    Lawful Interception  |
+-------------------------+
 ~ Description:
    An example of Lawful Interception
    Demonstrates how to:
    - Configure Interception rule based on ingress mirror.
    - Create encapsulation for mirror copy based on IPv4/IPv6.
    - Add LL header removal according to setting.

 ~ File Reference:
    lawful_interception/cint_dnx_lawful_interception.c

 ~ Name:
+--------------------------+
|    L3 Tunnels
+--------------------------+
 ~ Description:
    An example of IPv4/IPv6 tunnel encapsulation/ termination scenarios. Demonstrates how to:
    - Encpasulation/Termination of various IP/Vxlan tunnel types depends on the additional
      header.
    - Termination of various IP/Vxlan tunnels.
    - Create ingress and egress objects needed to trigger IPv6 tunnel
      encapsulation
    - Add tunnel entries

 ~ File Reference:
    tunnel/cint_dnx_ipv6_tunnel_encapsulation.c
    vxlan/cint_vxlan_tunnel_termination.c
    vxlan/cint_vxlan_roo_basic.c
    vxlan/cint_vxlan_bud_node.c
    tunnel/cint_dnx_ip_tunnel_encapsulation.c
    tunnel/cint_dnx_ip_tunnel_termination.c
    tunnel/cint_dnx_ip_tunnel_term_basic.c
    tunnel/cint_dnx_ipv6_tunnel_encapsulation_basic.c
    tunnel/cint_dnx_ipv6_tunnel_termination_basic.c
    tunnel/cint_dnx_ipv6_tunnel_termination.c
    vxlan/cint_vxlan_split_horizon.c
    vxlan/cint_vxlan_mega_config.c
    tunnel/cint_ip_tunnel_to_tunnel_basic.c
    tunnel/cint_ip_tunnel_to_tunnel.c
    tunnel/cint_dnx_ip_tunnel.c

 ~ Name:
+------------+
|    VXLAN  |
+------------+
 ~ Description:
    VXLAN encapsulation/termination various scenarios

 ~ File Reference:
    vxlan/cint_vxlan_mega_config.c

 ~ Name:
+----------------------+
|    MPLS termination  |
+----------------------+
 ~ Description:
    An example of MPLS termination various scenarios including:
        * an example for terminating IPv4oMPLS tunnels.

 ~ File Reference:
    cint_ipv4_mpls_termination_basic.c

 ~ Name:
+--------------------------+
|    IP Unicast Routing    |
+--------------------------+
 ~ Description:
    An example of IPv4 unicast router. Demonstrates how to:
    - Open the router interface
    - Create egress objects
    - Add route entries

 ~ File Reference:
    l3/cint_dnx_ip_route_basic.c

 ~ Name:
+---------------------------------------------------+
|    IP Unicast Routing Source MAC Encapsulation    |
+---------------------------------------------------+
 ~ Description:
    An example of IPv4 unicast router to encapsulate the custom source MAC.
    Demonstrates how to:
    - Open the router interface
    - Create egress objects
    - Add route and host entries
    - Add host entries while saving FEC entries
    - Set VRF default destination

 ~ File Reference:
    l3/cint_ip_route_encap_src_mac.c

 ~ Name:
+--------------+
|    L3 VRRP   |
+--------------+
 ~ Description:
    An example of baisic VRRP for both IPv4 and IPv6:
    - Create an VRRP extended entries (using EXEM) for both IPv4 and IPv6.
 ~ File Reference:
    l3/cint_dnx_l3_vrrp.c

 ~ Name:
+---------------------------------------------------+
|    IP Multicast Routing with unknown destination  |
+---------------------------------------------------+
 ~ Description:
    An example of IPv4 multicast router to modify the default flooding destination to a given Mc group.
    Demonstrates how to:
    - Open the router interface
    - Create MC group
    - Modify the default flooding destination

 ~ File Reference:
    l3/cint_ip_unknown_destination.c

 ~ Name:
+------------------------------------+
|    IP Multicast on Bridge          |
+------------------------------------+
 ~ Description:
    An example of IPv4 multicast configuration with disabled MC routing.
    Demonstrates how to:
    - Open the router interface
    - Create MC group
    - Create bridge IPMC entries

 ~ File Reference:
    l3/cint_l3_ipmc_disabled_route.c

 ~ Name:
+------------------------------------+
|    IP routing using KAPS           |
+------------------------------------+
 ~ Description:
    An example of multicast, unicast and RPF configurations in KAPS using public entries in case of a single KAPS DB.

 ~ File Reference:
    l3/cint_dnx_l3_single_kaps.c

 ~ Name:
+------------------------------------+
|    ARP configuration               |
+------------------------------------+
 ~ Description:
    An example of configuring ARP in EEDB phase 8.

 ~ File Reference:
    l3/cint_dnx_l3_eedb_phase_arp.c

 ~ Name:
+------------------------------------+
|    IP ECMP tunnnel priority           |
+------------------------------------+
 ~ Description:
    An example of ECMP tunnel priority

 ~ File Reference:
    cint_cbts.c

 ~ Name:
+--------------------------+
|    MPLS Encapsulation    |
+--------------------------+
 ~ Description:
    An example of MPLS tunnel encapsulations after PPMC table lookup. Demonstrates how to:
    - Create ingress and egress objects
    - Initiate an MPLS tunnel with a push action and particular label.
    - Create a PPMC entry that points to the MPLS tunnel (one for each replication of the MC group).
    - Define the replications and create a MC group.
    - Add MC host entries.

 ~ File Reference:
    multicast/cint_multicast_pp_basic.c

 ~ Name:
+--------------------------+
|    MPLS   |
+--------------------------+
 ~ Description:
    3 basic scenarios for MPLS:
    - MPLS forwarding: A packet is forwarded following a matching of an MPLS label in the
                       MPLS forwarding table (ILM). The result is an EEDB entry with the swapping information.
    - MPLS encapsulation: A packet is forwarded following a matching of a destination ip in the ip table.
                          The result is an EEDB entry with the encapsulation (push) information.
    - MPLS upstream assigned forwarding: A packet is forwarded following a matching of two MPLS labels (in one ILM entry) in the
                       MPLS forwarding table (ILM). The result is an EEDB entry with the swapping information.

 ~ File Reference:
    mpls/cint_dnx_mpls_encapsulation_basic.c

~ Name:
+--------------------------+
|    MPLS   |
+--------------------------+
 ~ Description:
    Basic MPLS termination scenario: An MPLs label is terminated in the ingress, resulting in a IP forwarding
                                     according to the exposed destination ip.
    Basic MPLS upstream assigned termination scenario: Two MPLS labels are terminated (together) in the ingress, resulting in a IP forwarding
                                     according to the exposed destination ip.

 ~ File Reference:
    mpls/cint_dnx_mpls_termination_basic.c

 ~ Name:
+-------------+
|    EVPN     |
+-------------+
 ~ Description:
    An example of EVPN application setup
    - Create ingress and egress objects
    - Define IML range
    - Create ingress and egress multicast objects (IML)
    - Add ESI encapsulation and termination

 ~ File Reference:
    - evpn/cint_dnx_evpn_basic.c


 ~ Name:
+---------------------+
|    EVPN + E-Tree    |
+---------------------+
 ~ Description:
    An example of EVPN E-Tree application setup
    - Create ingress and egress objects
    - Define IML range
    - Create ingress and egress multicast objects (IML)
    - Add ESI encapsulation and termination
    - Add Leaf packet filter configuration

 ~ File Reference:
    - evpn/cint_dnx_evpn_etree.c


 ~ Name:
+---------------------+
|    EVPN + VXLAN     |
+---------------------+
 ~ Description:
    An example of EVPN application setup with CE connection through VXLAN o IP-Tunnels
    - Create ingress and egress objects
    - Define IML range
    - Create ingress and egress multicast objects (IML)
    - Add ESI encapsulation and termination

 ~ File Reference:
    - evpn/cint_dnx_evpn_vxlan.c

 ~ Name:
+---------------------+
|    EVPN + TRUNK     |
+---------------------+
 ~ Description:
    An example of EVPN application setup with CE connection through TRUNK links
    - Create ingress and egress objects
    - Define IML range
    - Create ingress and egress multicast objects (IML)
    - Add ESI encapsulation and termination

 ~ File Reference:
    - evpn/cint_dnx_evpn_trunk.c

 ~ Name:
+--------------------------+
|    EVPN + Protection     |
+--------------------------+
 ~ Description:
    An example of EVPN application setup with 1+1 protection on incoming IML lifs
    and 1:1 protection on outgoing IML encapsulations.
    - Define IML range
    - Create ingress and egress multicast objects (IML) with protection
    - Add ESI encapsulation and termination

 ~ File Reference:
    - evpn/cint_dnx_evpn_protection.c

 ~ Name:
+--------------------------+
|    EVPN ROO MC           |
+--------------------------+
 ~ Description:
    An example of mc routing setup into and out of an EVPN overlay.
    This example mainly demonstrate how to use IML objects for ROO MC.

 ~ File Reference:
    - evpn/cint_dnx_evpn_roo_mc.c

 ~ Name:
+--------------------------+
|    VPLS over GRE         |
+--------------------------+
 ~ Description:
    An example of VPLS over GRE over IP over ETH cases.
    Including Access to Core with encapsulation and Core to Access with decapsulation scenarios.

 ~ File Reference:
    - tunnel/cint_dnx_vswitch_vpls_gre.c

 ~ Name:
+---------------------+
|    EVPN Utility     |
+---------------------+
 ~ Description:
    A Concentrated example of EVPN application objects
    - EVPN label term/encap creation
    - IML label term/encap creation
    - ESI Filter initialization and entry creation.

 ~ File Reference:
    - utility/cint_dnx_utils_evpn.c

 ~ Name:
+--------------------------+
|    MTU Trap - LIF        |
+--------------------------+
 ~ Description:
    An example of ETPP MTU trap per OutLIF.
    Demonstrates how to:
    - Create an ETPP User Defined trap with recycle_append_ftmh action.
    - Create MTU Trap for the supplied OutLIF.

 ~ File Reference:
    traps/cint_rx_trap_etpp_mtu_trap_lif.c

 ~ Name:
+--------------------------+
|    MTU Trap - RIF        |
+--------------------------+
 ~ Description:
    An example of ETPP MTU trap per OutRIF.
    Demonstrates how to:
    - Create an ETPP User Defined trap with recycle_append_ftmh or DROP action.
    - Create MTU Trap for the supplied OutRIF.

 ~ File Reference:
    traps/cint_rx_trap_etpp_mtu_trap_rif.c

 ~ Name:
+--------------------------+
|    MTU Trap - PORT        |
+--------------------------+
 ~ Description:
    An example of ETPP MTU trap per PORT.
    Demonstrates how to:
    - Create an ETPP User Defined trap with recycle_append_ftmh or DROP action.
    - Create MTU Trap for the supplied PORT.

 ~ File Reference:
    traps/cint_rx_trap_etpp_mtu_trap_port.c

 ~ Name:
+--------------------------+
|    Trap header in FHEI   |
+--------------------------+
 ~ Description:
    Creating Trap and setting FHEI stamping.
    Creating PMF rule with Action TRAP-Code and TRAP-Qual.

 ~ File Reference:
    traps/cint_rx_trap_fhei.c

 ~ Name:
+--------------------------+
|  L2CP trap -Protocol     |
+--------------------------+
 ~ Description:
    Various configuration for L2CP using trap mechanism.

 ~ File Reference:
    traps/cint_rx_trap_l2cp_exhaustive.c

 ~ Name:
+--------------------------+
|    L2CP + Subtype Traps  |
+--------------------------+
 ~ Description:
    An example of a programmable and protocol traps used together.
    Demonstrates how to:
    - Create an 2x Ingress User Defined traps.
    - Create an L2CP Protocol Trap and connect it with one UD Trap.
    - Create a progorammable trap that matches on ETH forwarding packets with a certain subtype
      and connect it with one UD Trap.

 ~ File Reference:
    traps/cint_rx_trap_l2cp_plus_subtype.c

 ~ Name:
+--------------------------+
|    SA equals DA Trap     |
+--------------------------+
 ~ Description:
    An example of a predefined Ingress trap created via utility function.
    Demonstrates how to:
    - Create a predefined ingress trap 'bcmRxTrapLinkLayerSaEqualsDa'
    - Sets all Ingress trap actions supported by the SDK to date.

 ~ File Reference:
    traps/cint_rx_trap_saequalsda_trap.c

 ~ Name:
+--------------------------+
|    ERPP EMPF Trap Action |
+--------------------------+
 ~ Description:
    An example of an EPMF Configuration which configures Trap action, which will trigger a
    User Defined ERPP trap, connected to an application trap 'bcmRxTrapEgIpv4Ttl1'.
    It configures all ERPP trap actions supported by the SDK to date.
    Demonstrates how to:
    - Creates a TCAM FG in EPMF with action Trap attached to the Default Context.
    - Create single Entry for the field group.
    - Create a User Defined ERPP Trap and set all ERPP trap actions supported by the SDK(to date).
    - Attach the UD trap to an applcation trap 'bcmRxTrapEgIpv4Ttl1'.

 ~ File Reference:
    traps/cint_rx_trap_erpp_epmf_trap_action.c

 ~ Name:
+--------------------------+
|    ERPP Trap             |
+--------------------------+
 ~ Description:
    An example of a simple ERPP application trap being set-up by utilities.
    Demonstrates how to:
    - Creates User Defined ERPP trap.
    - Attaches it to an ERPP application trap with the supplied ERPP Trap Actions.
    - Get and Compare the information set for a supplied ERPP application trap.

 ~ File Reference:
    traps/cint_rx_trap_erpp_traps.c

 ~ Name:
+--------------------------+
|    ERPP Trap ACE action  |
+--------------------------+
 ~ Description:
    An example of how to configure ERPP trap action to overwrite ACE pointer.
    Demonstrates how to:
    - Create and set User Defined ERPP trap with overwrite ACE pointer action.

 ~ File Reference:
    traps/cint_rx_trap_erpp_ace.c

 ~ Name:
+--------------------------+
|    ERPP  Trap to CPU     |
+--------------------------+
 ~ Description:
    An example of how to configure ERPP trap to redirect packet to CPU.
    Demonstrates how to:
    - Create and set User Defined ERPP trap to redirect packet to CPU.

 ~ File Reference:
    traps/cint_rx_trap_erpp_trap_to_cpu.c

 ~ Name:
+--------------------------+
|    ETPP  Trap            |
+--------------------------+
 ~ Description:
    An example of a simple ETPP application trap being set-up by utilities.
    Demonstrates how to:
    - Creates User Defined ETPP trap.
    - Attaches it to an ETPP application trap with the supplied ETPP Trap Actions.

 ~ File Reference:
    traps/cint_rx_trap_etpp_traps.c

 ~ Name:
+--------------------------------------------+
|    ETPP trap recycle and 2nd pass stamp    |
+--------------------------------------------+
 ~ Description:
    An example of recycling etpp trapped packet, and stamp trap code on 2nd pass.
    Demonstrates how to:
    - Creates User Defined ETPP trap which recycles packet and set the recycle command
      to stamp desired trap code on the 2nd pass.

 ~ File Reference:
    traps/cint_rx_trap_etpp_recycle_and_2nd_pass_stamp.c

 ~ Name:
+--------------------------+
|    ETPP  Trap to CPU     |
+--------------------------+
 ~ Description:
    An example of how to configure ETPP trap to redirect packet to CPU.
    Demonstrates how to:
    - Create and set User Defined ETPP trap to recycle packet and redirect it to
    CPU on 2nd PASS.

 ~ File Reference:
    traps/cint_rx_trap_etpp_trap_to_cpu.c

 ~ Name:
+--------------------------+
|   ETPP Visibility Trap   |
+--------------------------+
 ~ Description:
    An example of how to configure ETPP visibility trap.
    Demonstrates how to:
    - Create and set ETPP visibility trap with various available actions.

 ~ File Reference:
    traps/cint_rx_trap_etpp_visibility.c

 ~ Name:
+--------------------------+
|    Ingress  Trap         |
+--------------------------+
 ~ Description:
    An example of a simple Ingress  trap being set-up by utilities.
    Demonstrates how to:
    - Creates an Ingress trap(supplied by the user).
    - Sets Ingress trap actions supplied by the user.
    - Get and compare the information for the supplied trap.

 ~ File Reference:
    traps/cint_rx_trap_ingress_traps.c

 ~ Name:
+--------------------------+
|    Programmable  Traps   |
+--------------------------+
 ~ Description:
    An example of programmable traps based on ETH forwarding packet and IPv4 forwarding packet.
    It also contains an example of configuring Ingress default drop/redirect trap, via programmable trap.
    Demonstrates how to:
    - Creates an Ingress User Defined trap with action DROP.
    - Create Programmable trap with ETH Matching/Mismatching params based on ETH Forwarded packet.
    - Create Programmable trap with IPv4 Matching/Mismatching params based on IPv4 Forwarded packet.
    - Attach each trap to its UD Trap.
    - Create Programmable trap with IPv4 Matching/Mismatching params based on IPv4 Forwarded packet and
      attach the Ingress default drop/redirect trap(based on user input).

 ~ File Reference:
    traps/cint_rx_trap_programmable_traps.c

 ~ Name:
+--------------------------+
|    Protocol  Traps       |
+--------------------------+
 ~ Description:
    An example of protocol traps configuration per port, with additional configuration function for MyARP and MyNDP.
    Demonstrates how to:
    - Create a Protocol trap based on user input.
    - Contains helper/additinal configuration functions for MyARP and MyNDP.
    - Contains an example of IgnoreDA usage for Protocol traps.

 ~ File Reference:
    traps/cint_rx_trap_protocol_traps.c

 ~ Name:
+--------------------------+
|    LIF  Traps            |
+--------------------------+
 ~ Description:
    An example of LIF and RIF traps, and their configuration and usage.
    Demonstrates how to:
    - Create a IN/OUT LIF Trap based on user input.
    - Create a IN/OUT RIF Trap based on user input.
    - Clear any LIF/RIF configuration set by the user.

 ~ File Reference:
    traps/cint_rx_trap_lif_traps.c

 ~ Name:
+--------------------------------+
|    LIF trap with ESEM entry    |
+--------------------------------+
 ~ Description:
    An example for configuration of LIF trap with ESEM entry.
    Demonstrates how to:
    - Create OUTLIF Trap with ESEM entry based on user input.
    - Clear  OUTLIF Trap configuration set by the user.

 ~ File Reference:
    traps/cint_rx_trap_lif_with_esem_entry.c

 ~ Name:
+--------------------------+
|    Trap Utilities        |
+--------------------------+
 ~ Description:
    Contains all Trap utility functions.
    Demonstrates how to:
    - Use all utility functions for basic trap configuration creation.

 ~ File Reference:
    traps/cint_utils_rx.c

 ~ Name:
+--------------------------+
|    1588 Traps            |
+--------------------------+
 ~ Description:
    An example for configuration of 1588 traps.
    Demonstrates how to:
    - Configure timesync in order to hit discard/accept 1588 traps.
    - Configure bcmRxTrap1588Accepted.

 ~ File Reference:
    traps/cint_rx_trap_1588_traps.c

 ~ Name:
+--------------------------+
|    ICMP redirect to CPU  |
+--------------------------+
 ~ Description:
    An example for configuration of ICMP redirect to CPU.
    Demonstrates how to:
    - Configure route with InLIF=OutLIF.
    - Configure trap bcmRxTrapIcmpRedirect to redirect packet to cpu.

 ~ File Reference:
    traps/cint_rx_trap_icmp_redirect.c

 ~ Name:
+-------------+
|   sFlow     |
+-------------+
 ~ Description:
     Contains the sequence required to configure an sFlow agent on the device.
     In addition, it creates an ip route flow to demonstrate.

 ~ File Reference:
    sflow/cint_sflow_utils.c
    sflow/cint_sflow_basic.c
    sflow/cint_sflow_advanced.c
    sflow/cint_sflow_extended_gateway.c
    sflow/cint_sflow_field.c
    sflow/cint_sflow_field_utils.c
    sflow/cint_sflow_field_advanced.c
    sflow/cint_sflow_field_extended_gateway.c

 ~ Name:
+--------------------------+
|    Credit Size           |
+--------------------------+
 ~ Description:
   Confuguration of credit size for remote devices

 ~ File Reference:
    ingress_tm/cint_mgmt_credit_size.c

 ~ Name:
+--------------------------+
|    SRAM Only             |
+--------------------------+
 ~ Description:
   Configuration VoQ to be in SRAM only mode

 ~ File Reference:
    ingress_tm/cint_voq_sram_only.c

 ~ Name:
+--------------------------+
|    Mirror-On-Drop        |
+--------------------------+
 ~ Description:
   Configuration of mirror-on-drop mechanism

 ~ File Reference:
    ingress_tm/cint_mirror_on_drop.c

~ Name:
+--------------------------------------+
|    QOS for MPLS TANDEM over VPLS     |
+--------------------------------------+
 ~ Description:
    An example of VPLS QOS.
    Demonstrates how to:
    - Create MPLS port
    - Create MPLS tunnel
    - update mpls tunnel with qos profile

 ~ File Reference:
    vpls/cint_vpls_mpls_qos.c

~ Name:
+--------------------------------------+
|    MPLS IMPLICIT_NULL label     |
+--------------------------------------+
 ~ Description:
    An example of MPLS IMPLICIT_NULL tunnel.
    Allows configuring EEDB tunnel with 0 labels

 ~ File Reference:
    mpls/cint_eedb_implicit_null.c

~ Name:
+--------------------------+
|  Qos in vlan translation |
+--------------------------+
 ~ Description:
    Example of QoS mapping in ingress and egress vlan translation.

 ~ File Reference:
   qos/cint_qos_l2_vlan_edit.c

~ Name:
+--------------------------+
|  dbal api in cint        |
+--------------------------+
 ~ Description:
    Example of dbal api in cint code.

 ~ File Reference:
    dbal/cint_dbal.c

 ~ Name:
+--------------------------+
|    MPLS Per interface
+--------------------------+
 ~ Description:
    An example of MPLS termination and forwarding where the label
    lookup is coupled with the previously found interface (RIF/LIF)

 ~ File Reference:
    mpls/cint_mpls_per_if.c

 ~ Name:
+--------------------------+
|    OAM TST LB
+--------------------------+
 ~ Description:
    An example of OAM TST LB configuration

 ~ File Reference:
    cint_dnx_oam_tst_lb.c

 ~ Name:
+--------------------------+
|    MPLS FRR
+--------------------------+
 ~ Description:
    An example of MPLS FRR termination and related port configuration

 ~ File Reference:
    mpls/cint_dnx_mpls_frr_basic.c

 ~ Name:
+--------------------------+
|    VPLS - utility
+--------------------------+
 ~ Description:
    Utility cint based on cint_dnx_utility_mpls.c,
    including basic VPLS termination and encapsulation examples.

 ~ File Reference:
    utility/cint_dnx_utility_vpls_advanced.c

 ~ Name:
+--------------------------+
|    VPLS - over GRE
+--------------------------+
 ~ Description:
    Basic example of configuring VPLS encapsulation over GRE tunnel.

 ~ File Reference:
    vpls/cint_dnx_vpls_gre.c

 ~ Name:
+--------------------------+
|    VPLS - no MPLS
+--------------------------+
 ~ Description:
    Basic example of configuring VPLS encapsulation and termination without MPLS tunnel.

 ~ File Reference:
    vpls/cint_vpls_no_mpls.c

 ~ Name:
+--------------------------+
|    VPLS - tagged PWE
+--------------------------+
 ~ Description:
    Basic example of adding and terminating service delimiting tags (vlans)
    to a pwe lif.

 ~ File Reference:
    vpls/cint_vpls_pwe_tagged_mode_basic.c

 ~ Name:
+---------------------------------------+
|    ROO   |  VPLS - indexed PWE tagged |
+---------------------------------------+
 ~ Description:
    An example of termination and encapsulation of ROO with tagged indexed PWE,
    where native ETH is terminated and not used for forwarding.

 ~ File Reference:
    vpls/cint_indexed_pwe_tagged_roo.c

 ~ Name:
+--------------------------------+
|    VPLS - tagged PWE advanced
+--------------------------------+
 ~ Description:
    Advanced PWE tagged mode examples with different number of service tags

 ~ File Reference:
    vpls/cint_vpls_pwe_tagged_mode_advanced_scenario.c

~ Name:
+--------------------------------------+
|    IVE mapping DP to PCP DEI     |
+--------------------------------------+
 ~ Description:
    An example of mapping DP to PCP-DEI.
    Demonstrates how to:
    - Create AC p2p service
    - Create IVE policer qos profile
    - configure IVE
    - set qos profile on AC

 ~ File Reference:
    qos/cint_dnx_qos_ingress_policer.c

~ Name:
 +--------------------------------------+
 |        PP QOS ECN                    |
 +--------------------------------------+
  ~ Description:
     An example of ingress ECN mapping and egress remark with ECN .
     Demonstrates how to:
     - Create ingress ECN  profile
     - Create ingress ECN opcode
     - Add ingress ECN mapping
     - Create QOS egress remark profile
     - Create QOS egress ECN opcode
     - Add egress remark mapping with ECN
     - Apply ingress ECN profile and egress remark profile

  ~ File Reference:
     qos/cint_dnx_qos_ecn.c

~ Name:
 +--------------------------------------+
 |        PP QOS EGRESS COS             |
 +--------------------------------------+
  ~ Description:
     An example of Egress COS mapping.
     Demonstrates how to:
     - Create egress COS profile
     - Add egress COS mapping
     - Create PMF field context
     - Set PMF field pre-select
     - Create PMF field group
     - Attach PMF group to context
     - Add PMF field entry to update egress COS profile

  ~ File Reference:
      qos/cint_dnx_qos_egress_cos.c

~ Name:
 +--------------------------------------+
 |        PP QOS EGRESS RECYCLE         |
 +--------------------------------------+
   ~ Description:
     An example of Egress Recycle QOS.
     Demonstrates how to:
     - Create egress extend encapsuation recycle entry with QOS model

  ~ File Reference:
      qos/cint_dnx_qos_ext_recycle.c

~ Name:
+--------------------------------------+
|Port QOS and L3 QOS mapping for Bridge|
+--------------------------------------+
 ~ Description:
    An example of port QOS.
    Demonstrates how to:
    - Create qos map id
    - Set default TC/DP
    - Set default QOS remark variable
    - bind map id with port
    - set port qos model

 ~ File Reference:
     qos/cint_dnx_qos_l2_port.c

~ Name:
+--------------------------------------+
|           MPLS PHP QOS mapping       |
+--------------------------------------+
 ~ Description:
    An example of MPLS PHP QOS.
    Demonstrates how to:
    - Create qos map id
    - Create qos opcode
    - Mapping map_id to opcode
    - Mapping opcode to  QOS

 ~ File Reference:
    qos/cint_dnx_qos_php.c

~ Name:
+--------------------------------------+
|    Instru IFA field                  |
+--------------------------------------+
 ~ Description:
    An example of how to configure field for IFA
    Demonstrates how to:
    - Set iPMF to trap IFA packets
	- Set iPMF to snoop packets and initiate IFA flow

 ~ File Reference:
    instru/cint_field_IFA_datapath.c

~ Name:
+--------------------------------------+
|    Instru Egress IFA field           |
+--------------------------------------+
 ~ Description:
    An example of how to configure egress field for IFA
    Demonstrates how to:
    - Set ePMF to trap IFA packets
    - Configure ou_port for recycle

 ~ File Reference:
    instru/cint_field_IFA_egress_datapath.c

~ Name:
+--------------------------------------+
|    Instru IFA                        |
+--------------------------------------+
 ~ Description:
    An example of how to configure IFA
    Demonstrates how to:
    - Set initiator,transit and terminator nodes of IFA 1.0 session

 ~ File Reference:
    instru/cint_ifa.c

~ Name:
+--------------------------------------+
|    Instru IFA                        |
+--------------------------------------+
 ~ Description:
    An example of how to configure IFA
    Demonstrates how to:
    - Set initiator,transit and terminator nodes of IFA 2.0 session

 ~ File Reference:
    instru/cint_ifa2.c

~ Name:
+--------------------------------------+
|    Instru IPFIX                      |
+--------------------------------------+
 ~ Description:
    An example of how to configure IPFIX
    Demonstrates how to:
    - Initialize IPFIX parameters
	- Configure field for IPFIX

 ~ File Reference:
    ipfix/cint_ipfix.c

~ Name:
+--------------------------------------+
|    Instru IPFIX                      |
+--------------------------------------+
 ~ Description:
    An example of how to configure field for IPFIX
	receive and export packets.
    Demonstrates how to:
    - Configure ingress PMF
	- Configure egress PMF

 ~ File Reference:
    ipfix/cint_ipfix_field.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - FEM handling, 'increment' feature    |
+-----------------------------------------------------------+
 ~ Description:
    Example of usage of the FEM increment feature.

 ~ File Reference:
    field/cint_field_fem_increment.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - Alternate Marking Instrumentaion     |
+-----------------------------------------------------------+
 ~ Description:
    Example of field configuration for RFC8321

 ~ File Reference:
    field/cint_field_alternate_marking.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - Alternate Marking Instrumentaion     |
+-----------------------------------------------------------+
 ~ Description:
    Example of field configuration for RFC8321 on IPv6

 ~ File Reference:
    field/cint_field_ipv6_alternate_marking.c

 ~ Name:
+-----------------------------------------------------------+
|   IFIT for RFC8321                                        |
+-----------------------------------------------------------+
 ~ Description:
    Example of for RFC8321 on IFIT

 ~ File Reference:
    mpls/cint_mpls_deep_stack_ifit_am.c

~ Name:
+----------------------------------------------------------------+
|    PMF Application to trap LSR missing packets to rcy port     |
+----------------------------------------------------------------+
 ~ Description:
    Example of trap all LSR missing packets to rcy port
    cint pseudo code as follows:
    - Qualify on:
      * Trap code is MPLS unknown label.
      * source port isn't recycle port.
    - Perform the following actions:
      * Trap packet to recycle port with rcycle extended term encap id

 ~ File Reference:
    mpls/cint_mpls_extended_term_flow_field.c

~ Name:
+-----------------------------------------------------------+
|    PMF Application - Terminate min bytes in ETPP FWD      |
+-----------------------------------------------------------+
 ~ Description:
    This cint is for ITPP termination disable.
    On some cases, ITPP not terminating any bytes from packet before sending it to Egress, including tunnels. Therefore since only part of the packet is copied to PP, when in TERM the whole packet gets terminated, FWD then tries to rebuild all packet from and because not all bits are there in PP packet buffer, part of the data goes missing. This is true for large packets which include tunnels.
    This cint qualifies on those suspected packets that might be affected, then changes the parsing context in order to parse only the minimum bytes required to be rebuilt for the FWD to not try and rebuild the whole packet.
    cint pseudo code as follows:
    - Qualify on:
      * IPv4 or IPv6
      * fwd_index = 3 or 4
    - Perform the following actions:
      * Parsing_start_offset = fwd_layer_offset (start parsing from fwd layer)
      * egress_parsing_index = fwd_index (same as above)
      * bytes_to_remove = 0 (same as above, if we remove bytes we will lose fwd layer offset)
      * parsing_start_type = IPv4_12B/IPv6_8B (a flag to the parser for minimum required bytes parsing)


 ~ File Reference:
    field/cint_field_etpp_fwd_term_min_bytes.c


~ Name:
+--------------------------------------------------------------+
|    PMF Application - FEM handling, invalidate FEM by context |
+--------------------------------------------------------------+
 ~ Description:
    Example of usage of the FEMs, where we do not enable
    all FEMs added to a field group for all contexts.

 ~ File Reference:
    field/cint_field_fem_invalidate.c

~ Name:
+--------------------------------------------------------------+
|    PMF Application - EFES add                                |
+--------------------------------------------------------------+
 ~ Description:
    Several example of using EFES add to perform multiple actions
    using the same data, different actions for different entries
    or contextsand  perform three bits conditions in direct
    extraction field groups (all with using EFES instead of FEM).

 ~ File Reference:
    field/cint_field_efes_add.c

~ Name:
+-------------------------------------------------------------------+
|    PMF Application - action disabling, invalidate EFES by context |
+-------------------------------------------------------------------+
 ~ Description:
    Example of disabling an action from being perfromed (so that
    no EFES is allocated to it, just like a void action) per
    context.

 ~ File Reference:
    field/cint_field_efes_invalidate.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - TRUNK GPORT as field input           |
+-----------------------------------------------------------+
 ~ Description:
    This cint shows examples of using TRUNK GPORT as input for OutPort, DstPort, InPort,
    SrcPort and PortClass qualifiers in different field stages.

 ~ File Reference:
    field/cint_field_trunk_port_quals.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - ACL OUT LIF/RIF GPORT as field input |
+-----------------------------------------------------------+
 ~ Description:
    This cint shows examples of using ACL OUT LIF/RIF qualifiers as gports.

 ~ File Reference:
    field/cint_field_epmf_outlif_profile.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - 256 data qualifiers create           |
+-----------------------------------------------------------+
 ~ Description:
    Create 256 Data qualifiers without ID and assign them to
    Field groups.

 ~ File Reference:
    field/cint_field_data_qualifier_create.c

~ Name:
+-----------------------------------------------------------+
|    PMF Application - Elephant Flow                        |
+-----------------------------------------------------------+
 ~ Description:
    Show how to configure Elephant flow using PMF API's

 ~ File Reference:
    elephant_flow/cint_elephant_flow.c
    elephant_flow/cint_elephant_flow_field.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - OAM bfd Echo                         |
+-----------------------------------------------------------+
 ~ Description:
    Provides an example of building setup for bfd Echo application

 ~ File Reference:
    field/cint_field_bfd_echo_j2.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Context Selection                                  |
+-----------------------------------------------------------+
 ~ Description:
    An exmaple to how create context and configure context
    Selection table to always hit that context

 ~ File Reference:
    field/cint_field_always_hit_context.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Qualify Upon VID                                   |
+-----------------------------------------------------------+
 ~ Description:
    An exmaple to how to qualify upon VID

 ~ File Reference:
    field/cint_field_vid_qual.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF ACE - Configure ACE action                         |
+-----------------------------------------------------------+
 ~ Description:
    Provides an example of performing an action using ACE,
    that is ACL extension, allowing the ePMF to perform an action
    in the ETPP block (instead of in the ERPP block, where the
    ePMF is).

 ~ File Reference:
    field/cint_field_ace.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF EXEM - Configure an exact match field group        |
+-----------------------------------------------------------+
 ~ Description:
    Provides an example of configuring an exact match
    field group.

 ~ File Reference:
    field/cint_field_exem.c

  ~ Name:
+------------------------------------------------------------+
|    PMF EXEM - Configure 5 EXEM field groups using prefix   |
+------------------------------------------------------------+
 ~ Description:
    Provides an example of configuring 5 exact match
    field groups with the same 3 entries using prefix.

 ~ File Reference:
    field/cint_field_exem_5fg_same_entries.c

  ~ Name:
+----------------------------------------------------------------+
|    PMF EXEM - Configure an EM FG to remap FEC IDs in IOP mode  |
+----------------------------------------------------------------+
 ~ Description:
    Provides an example of configuring an exact match
    field group for JR1-mode which remaps the learned FEC IDs
    to JR2 range of FECs.

 ~ File Reference:
    field/cint_field_jr1_mode_fec_remap.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Drop action creation                              |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to create Action drop for PMF to hold only 1 bit in payload

 ~ File Reference:
    field/cint_field_action_drop.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Cascading info used for polarity                  |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to use cascading info for polarity.

 ~ File Reference:
    field/cint_field_cascading_info_used_for_polarity.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Direct extraction different trap per core         |
+-----------------------------------------------------------+
 ~ Description:
    Sets different trap per core relating on polarity value.

 ~ File Reference:
    field/cint_field_direct_ext_different_trap_per_core.c

 ~ Name:
+---------------------------------------------------------------------------------------+
|    PMF Application - ACL OUT LIF/RIF profile as context select qualifier on EPMF stage  |
+---------------------------------------------------------------------------------------+
 ~ Description:
    This cint shows example of using ACL OUT LIF/RIF context select qualifier.

 ~ File Reference:
    field/cint_field_epmf_cs_outlif_profile.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Egress Bypass Filtering                           |
+-----------------------------------------------------------+
 ~ Description:
    Get BypassFilter by setting a trap in Ingress and detect its strength in Egress.

 ~ File Reference:
    field/cint_field_qual_bypass_filter.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Extended Range Qulifier                           |
+-----------------------------------------------------------+
 ~ Description:
    Set extended range qualifier (user defined, ttl and packet size).

 ~ File Reference:
    field/cint_field_range_qual_ext.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Disabling trap                                    |
+-----------------------------------------------------------+
 ~ Description:
    This cint shows how to overwrite any trap that was set.

 ~ File Reference:
    field/cint_field_disabling_trap.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - EFES Polarity basic example                       |
+-----------------------------------------------------------+
 ~ Description:
    This cint shows how to configure different polarity values per given context.

 ~ File Reference:
    field/cint_field_efes_polarity_basic_example.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - bcmFieldQualifyDstSysPortExt qualifier            |
+-----------------------------------------------------------+
 ~ Description:
    Example of using bcmFieldQualifyDstSysPortExt qualifier in J1 mode.

 ~ File Reference:
    field/cint_field_dsp_ext.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Redirect action                                   |
+-----------------------------------------------------------+
 ~ Description:
    Example of using bcmFieldActionRedirect action with value BCM_GPORT_BLACK_HOLE.

 ~ File Reference:
    field/cint_field_action_redirect.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Extension Header Type qualifier                   |
+-----------------------------------------------------------+
 ~ Description:
    Example of using bcmFieldQualifyExtensionHeaderType qualifier.

 ~ File Reference:
    field/cint_field_extension_header_type.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - FEM TCAM double key                               |
+-----------------------------------------------------------+
 ~ Description:
    Example of using FEM actions by double key TCAM FG.

 ~ File Reference:
    field/cint_field_fem_tcam_double_key.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - SNIFF creation Ingrees and Egress                 |
+-----------------------------------------------------------+
 ~ Description:
    Show an example how PMF can generate Sniff Commands in Ingress and Egress using TCAM FG,
    also allows to add entries per stage per action


 ~ File Reference:
    field/cint_field_sniff.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - PPPoE                                             |
+-----------------------------------------------------------+
 ~ Description:
      CINT script example for followed using PMF to redirect PPPoE
      protocol packets to CPU

 ~ File Reference:
    field/cint_field_pppoe.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Admit profile                                       |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to configure bcmFieldActionAdmitProfile
    to update the admit_profile value.

 ~ File Reference:
    field/cint_field_admit_profile.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Unknown Address                                     |
+-----------------------------------------------------------+
 ~ Description:
    This cint configures and shows example of bcmFieldQualifyPacketRes qualifier to
    qualify upon unknown address destination in IPMF1.

 ~ File Reference:
    field/cint_field_unknown_address.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - User qualifier on Ethertype                         |
+-----------------------------------------------------------+
 ~ Description:
    This cint configures and shows example of user qualifier on ethertyp.
    Relevant for tagged and untagged ethernet packets.

 ~ File Reference:
    field/cint_field_user_qual_ethertype.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Attach API                                       |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how bcm_field_group_context_attach() API can be used
    Configures 1 Field group that can be used for Bridged and IPv4 Routed packets

 ~ File Reference:
    field/cint_field_attach.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Simple TCAM 1 qual 1 action                      |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to create basic TCAM Field group with 1 qualifier and 1 action

 ~ File Reference:
    field/cint_field_tcam_1q_1a.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Ethernet 1/2 qualification                       |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to qualify on Ethernet 1/2 type on the ETH layer record qualifier

 ~ File Reference:
    field/cint_field_eth_1_2.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - TCAM Entry add by location                       |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to create basic TCAM entry add by location (EAL) Field group and add
    entries to it by location

 ~ File Reference:
    field/cint_field_tcam_entry_add_by_location.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - User qualifier Header                            |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to create user qualifier Header.

 ~ File Reference:
    field/cint_field_user_qual_header.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Assign BIER Properties                           |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to assign bier properties to arbitrary mpls packet

 ~ File Reference:
    field/cint_field_bier.c

  ~ Name:
+-----------------------------------------------------------+
|    BIER MPLS                                              |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to set up the mpls bier application

 ~ File Reference:
    multicast/cint_dnx_mpls_bier.c

  ~ Name:
+-----------------------------------------------------------+
|  PMF - Context compare API. 'Double' mode.                |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how bcm_field_context_compare_create() API can be used in mode:
    bcmFieldContextCompareTypeDouble.
    Configure FG in IPMF2 which will use this compare result as part of its key.
 ~ File Reference:
    field/cint_field_compare_double.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Context compare API with full 'offset_get' feature. |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to create a context in the compare mode Double using 1 pair and it is
    using bcm_field_compare_operand_offset_get() to get the full offset of
    the compare operand inside the 6-bit compare result.
 ~ File Reference:
    field/cint_field_compare_full_offset_get.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Context compare API. 'Single' mode.               |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to compare a value stored in initial
    keys to TCAM result of 1 of the lookups done in iPMF1.
 ~ File Reference:
    field/cint_field_compare_single.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Context compare API with spec 'offset_get' feature. |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to create a context in the compare mode Double using 2 pairs and it is
    using bcm_field_compare_operand_offset_get() to get the offset
    of a specific compare operand inside the 6-bit compare result.
 ~ File Reference:
    field/cint_field_compare_specific_offset_get.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Context cascading feature                           |
+-----------------------------------------------------------+
 ~ Description:
    CINT example, shows how to switch the selected context in IPMF1
    to with new one in IPMF2 stage.

 ~ File Reference:
    field/cint_field_contexts_cascading.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Ctest meta data clear                               |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to clean the meta data in the Ctests.

 ~ File Reference:
    field/cint_field_ctest_clear.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Ctest meta data configure                           |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to configure some meta data to qualify upon.

 ~ File Reference:
    field/cint_field_ctest_config.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Egress mirror enable                                |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to configure bcmFieldActionMirrorEgress
    to update the egress mirror_cmd value.

 ~ File Reference:
    field/cint_field_egress_mirror.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Fes valid bit usage                                 |
+-----------------------------------------------------------+
 ~ Description:
    Cint example of usage of the FES valid bit.

 ~ File Reference:
    field/cint_field_fes_valid_bit.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Hashing feature                                     |
+-----------------------------------------------------------+
 ~ Description:
    CINT example about hashing feature.

 ~ File Reference:
    field/cint_field_hashing.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - IPMF2 to IPMF3 cascading                            |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to configure IPMF2 to IPMF3 cascading.

 ~ File Reference:
    field/cint_field_ipmf2_ipmf3_cascading.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Scheduled Fabric + Egress MC  application
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to enable the Scheduled Fabric + Egress MC application.

 ~ File Reference:
    field/cint_field_mc_two_pass.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - key_gen_var feature. TCAM usage.                  |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to create constant values per context and set it as qualifier.
    For TCAM FG.
 ~ File Reference:
    field/cint_field_key_gen_var.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Vport/ Lif (in or out) Action                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how PMF user should use  bcmFieldActionOutVport0  bcmFieldActionIngressGportSet

 ~ File Reference:
    field/cint_field_lif_action.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - MDB DT                                           |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how PMF user should create and use MDB DT Field groups

 ~ File Reference:
    field/cint_field_mdb_dt.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Mirror Action                                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how PMF user should use bcmFieldActionMirrorIngress

 ~ File Reference:
    field/cint_field_mirror.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - MPLS X label presel                                 |
+-----------------------------------------------------------+
 ~ Description:
    Configure Field Processor Program Selection Preselector for header format, MPLS any label any L2/L3.

 ~ File Reference:
    field/cint_field_mpls_x_presel.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - IP Snoop                                            |
+-----------------------------------------------------------+
 ~ Description:
    Snoop IP-routed packets using PMF configuration

 ~ File Reference:
    field/cint_field_ip_snoop.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Context Selection, Forwarding Layer               |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to set Context selection per Forwarding Layer Type.
    The cint Creates a context and map presel line to that context

 ~ File Reference:
    field/cint_field_presel_fwd_layer.c

  ~ Name:
+-----------------------------------------------------------+
|  PMF - Range Set. Presel qual VportCheck                  |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure a range for bcmFieldQualifyVPortRangeCheck presel qual.
 ~ File Reference:
    field/cint_field_range_presel.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Range Set. FG qual (L4 Src/Dest ports/Header size)|
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to set a Range in L4 Dest and Src Port/Packet Header Size.
    and use it as a qualifier.
 ~ File Reference:
    field/cint_field_range_qual.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Snoop Action                                     |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how PMF use should use bcmFieldActionSnoop

 ~ File Reference:
    field/cint_field_snoop.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - State Table Field Group, read and write          |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Stage Table (bcmFieldGroupTypeStateTable) Field group
    without atomic read-modify-write.

 ~ File Reference:
    field/cint_field_state_table_rw.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - State Table Field Group, read-modify-write       |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Stage Table (bcmFieldGroupTypeStateTable) Field group
    with atomic read-modify-write.

 ~ File Reference:
    field/cint_field_state_table_rmw.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Statistical Sampling Action                      |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how PMF use should use bcmFieldActionStatSampling

 ~ File Reference:
    field/cint_field_stat_sampling.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - System Headers                                      |
+-----------------------------------------------------------+
 ~ Description:
    Configure TCAM field group to test the system headers.
    - First case - Configure context and set its params
              to change the system header profile to
              bcmFieldSystemHeaderProfileFtmhTsh (HW value = 5).
    - Second case - Configure same context as in the first case,
              and override the value with action result from
              IPMF2-IPMF3.

 ~ File Reference:
    field/cint_field_sys_headers.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Direct TCAM Table Field Group                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Direct TCAM Table (bcmFieldGroupTypeDirectTcam) Field group

 ~ File Reference:
    field/cint_field_tcam_dt.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Entry Hit APis                                   |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows the usage of the entry_hit APis.

 ~ File Reference:
    field/cint_field_entry_hit.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - TCAM entry cache                                 |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to add entries to TCAM field group using cache.

 ~ File Reference:
    field/cint_field_tcam_entry_cache.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - TCAM bank allocation per core                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to allocate TCAM banks to TCAM field group per core.

 ~ File Reference:
    field/cint_field_tcam_bank_per_core.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Direct TCAM Table Multi Test                     |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure 2 Direct TCAM Table FGs on same bank.

 ~ File Reference:
    field/cint_field_dt_multi.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Direct TCAM Table Multi Entries Test             |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure multi entries on Direct TCAM Table FG on same bank.

 ~ File Reference:
    field/cint_field_dt_multi_entries.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - TCAM FG on IPMF3 stage                              |
+-----------------------------------------------------------+
 ~ Description:
    Basic example for TCAM on iPMF3 stage.

 ~ File Reference:
    field/cint_field_tcam_ipmf3.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Action Prefix                                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows an example of using an action prefix, that is
    adding a constant to the MSB of a user defined action.

 ~ File Reference:
    field/cint_field_tcam_prefix.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - No system hdr usage and adding trap code            |
+-----------------------------------------------------------+
 ~ Description:
    This CINT creates a new context and configures it to not use any system header,
    then it adds a preselector of the given trap_code/trap_mask for the new context.
    The preselector presel_id is also given.

 ~ File Reference:
    field/cint_field_trap_code_no_sys_hdr.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Cint Utilities                                      |
+-----------------------------------------------------------+
 ~ Description:
    CINT utilities to avoid the code repeating.

 ~ File Reference:
    field/cint_field_utils.c

  ~ Name:
+-----------------------------------------------------------+
|  PMF - key_gen_var feature. DE usage.                     |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to create constant values per context and set it as qualifier.
    For DE FG.
 ~ File Reference:
    field/cint_field_key_gen_var_de.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Constant Field Group                             |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Constant (bcmFieldGroupTypeConst) Field group

 ~ File Reference:
    field/cint_field_group_const.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - InLIF profile                                    |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use InLIF profile as a qualfier

 ~ File Reference:
    field/cint_field_inlif_profile.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - DstGport qualifier                               |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use bcmFieldQualifyDstGport as a qualfier

 ~ File Reference:
    field/cint_field_DstGport_ipmf3_qual.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - RxTrapProfile presel qualifier                   |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use/configure bcmFieldQualifyRxTrapProfile
      as a presel qualfier in IPMF3 stage.

 ~ File Reference:
    field/cint_field_presel_rx_trap_profile.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - AcInLifWideData presel qualifier                 |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use/configure bcmFieldQualifyAcInLifWideData
      as a presel qualfier in IPMF1 stage.

 ~ File Reference:
    field/cint_field_presel_ac_inlif_wide_data.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Port profile                                     |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use configure port profile
      and use it as a qualfier.

 ~ File Reference:
    field/cint_field_port_profile.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Port general data                                |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use configure port general data
      and use it as a qualfier.

 ~ File Reference:
    field/cint_field_port_general_data.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - UDH Cascading                                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint Example of using PMF for UDH cascading.

 ~ File Reference:
    field/cint_field_udh_cascading.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - IPv6 Fragmented non first                        |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to qualify on IPv6 fragmented non first
      in IPv6 LR Qualifier

 ~ File Reference:
    field/cint_field_ipv6_fragmented_non_first.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - IPv4 Fragmented non first                        |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to qualify on IPv4 fragmented non first
      in IPv4 LR Qualifier

 ~ File Reference:
    field/cint_field_ipv4_frag_non_first_qual.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF -   VSI Profile                                    |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use VSI profile as qualifier

 ~ File Reference:
    field/cint_field_vsi_profile.c


 ~ Name:
+-----------------------------------------------------------+
|    PMF -   APP Type                                    |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use APP type as qualifier

 ~ File Reference:
    field/cint_field_app_type.c


  ~ Name:
+-----------------------------------------------------------+
|    PMF Qualifier - PP port                                |
+-----------------------------------------------------------+
 ~ Description:
     Cint that show hot to qualify upon PP port

 ~ File Reference:
    field/cint_field_in_port_qual.c


  ~ Name:
+-----------------------------------------------------------+
|    PMF - State Table address prefix usage                 |
+-----------------------------------------------------------+
 ~ Description:
     This is an example how we can split the State Table memory between different State Table FGs
      by using constant values as prefixes to define a unique range per State Table FG.

 ~ File Reference:
    field/cint_field_st_multi.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Action prefix                                     |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to use action prefix in bcm_field_action_create() API.

 ~ File Reference:
    field/cint_field_action_prefix.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Cascading IPMF1 to IPMF2 stage                      |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure IPMF1-IPMF2 Cascading field groups,
    while using result buffer from the IPMF1 as qualifier in the IPMF2.

 ~ File Reference:
    field/cint_field_cascading.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Cascading IPMF1 to IPMF2 stage, 320b FG                      |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure IPMF1-IPMF2 320b Cascading field groups,
    while using result buffer from both keys of the FG in IPMF1 as qualifiers in
    the IPMF2, cint also shows how error is expected when configuring cascading
    qualifier upon action splitted between the two keys result buffer.

 ~ File Reference:
    field/cint_field_cascading_320b.c


  ~ Name:
+-----------------------------------------------------------+
|   ECMP unicast RPF                                        |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure unicast RPF when the routing
    points to an ECMP. The interfaces that are members of the
    ECMP must be configured in iPMF3.

 ~ File Reference:
    field/cint_field_ecmp_rpf.c

  ~ Name:
+-----------------------------------------------------------+
|   ECMP unicast and multicast RPF                          |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure unicast and multicast RPF when the routing
    points to an ECMP group. It explains how to pass and how to fail the RPF check.

 ~ File Reference:
    l3/cint_ip_ecmp_rpf_examples.c

  ~ Name:
+-----------------------------------------------------------+
|   L3 snake between ports - mainly for KBP performance     |
+-----------------------------------------------------------+
 ~ Description:
    Configure l3 snake between given ports. First port must be the port connected to
    the traffic generator. The incoming packets should be configured as followed:
    DA = a0:00:00:00:00:e6
    SA = a0:00:00:00:00:e6
    vlan = 100 + ports[0]
    DIP = 32.54.67.124
    SIP = 2.1.1.1

 ~ File Reference:
    l3/cint_snake_test.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF FEM - Create FEM action for DE and TCAM             |
+-----------------------------------------------------------+
 ~ Description:
    Cint provides an example of performing an action using a
    FEM, both for a direct extraction field group and for a
    TCAM field group. The cint only describes a single simple
    case for configuring FEM.

 ~ File Reference:
    field/cint_field_fem.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF Side of IPT instumentation Appliction              |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Ingress PMF's for IPT instrumentation Application

 ~ File Reference:
    field/cint_field_hash_ipt.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Attach API: input_type_const                      |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to to use bcmFieldInputTypeConst in bcm_field_group_context_attach()

 ~ File Reference:
    field/cint_field_input_type_const.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF layer records                                       |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure qualifiers for field group for
    different types of layer records.

 ~ File Reference:
    field/cint_field_layer_record.c

 ~ Name:
+------------------------------------------------------------+
|    SRv6 Tunnel full flow example                           |
+------------------------------------------------------------+
 ~ Description:
    An example of SRv6 tunnel encapsulation/ termination scenarios. Demonstrates how to:
    - SRv6 Ingress Tunnel Encapsulation
    - Within SRv6 Tunnel End-Points traverse
    - SRv6 Egress Tunnel Termination
    - Create ingress and egress objects needed to support the
      above applications
    - Add tunnel entries

 ~ File Reference:
    tunnel/cint_dnx_srv6_tunnel_full_flow_example.c
    srv6/cint_srv6_basic.c
    srv6/cint_dnx_srv6_test.c
    utility/cint_dnx_util_srv6.c
    srv6/cint_srv6_egress_tunnel_1_and_2_passes.c

 ~ Name:
+------------------------------------------------------------+
|    SRv6 Extended encapsulation tunnel example              |
+------------------------------------------------------------+
 ~ Description:
    An example of SRv6 Ext encap tunnel, 2 PAss flow, with encapsulating up to 10 SIDs of 128b:

 ~ File Reference:
    srv6/cint_srv6_ext_encap_tunnel.c

 ~ Name:
+------------------------------------------------------------+
|    SRv6 Tunnel uSID full flow example                       |
+------------------------------------------------------------+
 ~ Description:
    An example of SRv6 uSID tunnel encapsulation/ termination scenarios. Demonstrates how to:
    - SRv6 Ingress Tunnel Encapsulation (which is mutual to uSID and Classic SID case)
    - Within SRv6 Tunnel End-Points traverse (x6 16b uSIDs)
    - SRv6 Egress Tunnel Termination  (which is mutual to uSID and Classic SID case)
    - Create ingress and egress objects needed to support the
      above applications
    - Add tunnel entries

 ~ File Reference:
    tunnel/cint_dnx_srv6_usid_full_flow_example.c

 ~ Name:
+------------------------------------------------------------+
|    SRv6 Tunnel GSID full flow example                       |
+------------------------------------------------------------+
 ~ Description:
    An example of SRv6 GSID tunnel encapsulation/ termination scenarios. Demonstrates how to:
    - SRv6 Ingress Tunnel Encapsulation (which is mutual to GSID and Classic SID case)
    - Within SRv6 Tunnel End-Points traverse (x2 32b GSIDs - GSID1 and GSID0)
    - SRv6 Egress Tunnel Termination  (which is mutual to GSID and Classic SID case)
    - Create ingress and egress objects needed to support the
      above applications
    - Add tunnel entries

 ~ File Reference:
    tunnel/cint_dnx_srv6_gsid_full_flow_example.c

  ~ Name:
+------------------------------------------------------------+
|    SRv6 EVPN                                               |
+------------------------------------------------------------+
 ~ Description:
    An example of SRv6 USP EVPN termination scenario. Demonstrates how to:
    - Configure ESI label in SRv6 as part of SRv6 Tunnel Termination
    - SRv6 Egress Tunnel Termination
    - Create ingress and egress objects needed to support the
      above applications
    - Add tunnel entries

 ~ File Reference:
    srv6/cint_dnx_srv6_evpn_basic.c
    field/cint_field_srv6_evpn_esi.c


  ~ Name:
+------------------------------------------------------------+
|    RCY Ext-Encap Flow                                      |
+------------------------------------------------------------+
 ~ Description:
    An examples for RCY ext-encapsulation flow

 ~ File Reference:
    rcy/cint_dnx_rcy_extended_encap_default_outlif.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF vlan format qualifier                               |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to use the VLAN format qualifier, bot for
    context selection and lookup.
    Vlan format qualifier indicated the type of the ethernet
    packet, whether it is untagged, tagged, and whether or not
    is is using an S-tag.


 ~ File Reference:
    field/cint_field_vlan_format.c


  ~ Name:
+-----------------------------------------------------------+
|    PMF - Direct Extraction Simple configuration           |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure PMF with Direct Extraction rule

 ~ File Reference:
    field/cint_field_basic_de.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - Basic Functionalities                            |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure OAM endpoints and various
      functionalities:
        - Endpoint create - Hierarchical LM
        - Counter resource management
        - PCP based counting

 ~ File Reference:
    oam/cint_oam_basic.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - General utility functions                        |
+-----------------------------------------------------------+
 ~ Description:
      General OAM utility functions used to create OAM setups.

 ~ File Reference:
    utility/cint_dnx_utils_oam.c

  ~ Name:
+-----------------------------------------------------------+
|    FLOW_ID - General utility functions                    |
+-----------------------------------------------------------+
 ~ Description:
      General FLOW_ID utility functions used to forcing packet ingressing from ingress-port to flow_id.

 ~ File Reference:
    utility/cint_dnx_utils_flow_id.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - Action Profiles                                  |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure OAM endpoints and various
      functionalities:
        - Configure OAM action set
        - LM types: Single Ended, Dual Ended, SLM

 ~ File Reference:
    oam/cint_oam_action.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - Statistics                                 |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure OAM statistics
      functionalities:
        - Allocate counters
        - Confiure OAM statistics
 ~ File Reference:
    oam/cint_oam_statistics.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - CFMoEthoMplsoEth                          |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure OAM endpoint base on P-2-P/MP, AC to VPLS settings
      functionalities:
        - Create P2P/P2MP AC to VPLS service
        - Confiure OAM endpoint
 ~ File Reference:
    oam/cint_dnx_oam_eth_o_mpls_o_eth.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - CFMoEthoSRv6oIPv6oETH                            |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure OAM endpoint base on AC to Srv6 tunnel settings
      functionalities:
        - Create Srv6 service
        - Confiure OAM endpoint
 ~ File Reference:
    oam/cint_dnx_oam_eth_over_srv6.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - CFMoEthoMplsoEth                          |
+-----------------------------------------------------------+
 ~ Description:
   Purpose: basic example for oam over native ethernet.

   Examples:
     - 1. Example of testing OAM over native ethernet with PWE raw mode
     - 2. Example of testing OAM over native ethernet and PWE OAM with PWE raw mode
     - 3. Example of testing OAM over native ethernet with PWE unindexed mode
 ~ File Reference:
    oam/cint_dnx_oam_eth_o_pwe_o_eth.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - Mpls oam transit signal on P-node     |
+-----------------------------------------------------------+
 ~ Description:
      Mpls OAM transit signal on P-node
      functionalities:
        - Creating ePFM to qualify OAM packet CCMoGACHoGALoMPLSxoETH,
        - set ace_value
 ~ File Reference:
    oam/cint_dnx_oam_mpls_transit_signal.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - Performance measurement                          |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure OAM endpoint performance measurement
      functionalities:
        - Configure loss measurement.
        - Configure delay measurement.
 ~ File Reference:
    oam/cint_oam_performance_measurement.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - PWE protection                                   |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure OAM endpoint base on PWE protection
      functionalities:
        - Create P2P/P2MP AC to VPLS service
        - Confiure OAM endpoint
 ~ File Reference:
    oam/cint_dnx_oam_y1731_over_pwe_protection.c

  ~ Name:
+-----------------------------------------------------------+
|    OAM - FEC injection                                    |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure OAM endpoint base on VPLS model with FEC:
        - Create VPLS service with FEC
        - Confiure OAM endpoint
 ~ File Reference:
    oam/cint_dnx_oam_fec_injection.c

~ Name:
+--------------------------------------------------+
|   Example of using OAM APIs                      |
+--------------------------------------------------+
 ~ Description:
    Example of using OAM IPv4 APIs that tests for OAM connection to bring sessions UP.

 ~ File Reference:
    oam/cint_dnx_oam.c

~ Name:
+--------------------------------------------------+
|    Vlan Translation Utility functions            |
+--------------------------------------------------+
 ~ Description:
    Utility functions for Vlan Translation

 ~ File Reference:
    utility/cint_dnx_utils_vlan.c

  ~ Name:
+-----------------------------------------------------------+
|    Load Balancing - Offline simulator                     |
+-----------------------------------------------------------+
 ~ Description:
      Provide a way to generate load balancing keys outside the SDK without sending actual packets.
      functionalities:
        - Get a device load balancing HW configuration.
        - Test the load balancing simulation on a device and compare the results
        - Generate load balancing hash key for packets without using a device

 ~ File Reference:
    load_balancing_sim/jr2_family/cint_load_balancing_basic.c
    load_balancing_sim/jr2_family/cint_load_balancing_sim.c
    load_balancing_sim/jr2_family/cint_load_balancing_fields.c
    load_balancing_sim/jr2_family/cint_load_balancing_hw_cfg_gen.c
  ~ Name:
+-----------------------------------------------------------+
|    CRC Hashing - Offline simulator                     |
+-----------------------------------------------------------+
 ~ Description:
      Provide a way to generate CRC hashing keys outside the SDK without sending actual packets.
      functionalities:
        - Get keys I & J values.
        - Get load balancing initial key.
        - Test the hashing simulation on a device.
        - Generate hashing key for packets and compare the results.

 ~ File Reference:
    field/cint_field_crc_hash_sim.c

  ~ Name:
+-----------------------------------------------------------+
|    Context Hash - multiple action keys                    |
+-----------------------------------------------------------+
 ~ Description:
    An example of configure hash context with more then one action key
 ~ File Reference:
    field/cint_field_6_hash_keys.c

~ Name:
+-----------------------------------------------+
|    CRPS database configuration   |
+-----------------------------------------------+
 ~ Description:
    An example of configuring CRPS database of various sources(IRPP/ERPP/ITM/ETM/ETPP/OAM):
   - configure a statistic data base for selected source
   - provide one configuration for all engines that belong to the data base.
   - configure specific parameters per engine (engine id, engine counters range etc)
   - enable the database
   - add/remove counter ranges
   - different expansion and data mapping scenarios are presented for all sources

 ~ File Reference:
    crps/cint_crps_erpp_config.c
    crps/cint_crps_etm_config.c
    crps/cint_crps_etpp_config.c
    crps/cint_crps_general_config.c
    crps/cint_crps_irpp_config.c
    crps/cint_crps_itm_config.c
    crps/cint_crps_oam_config.c
    crps/cint_crps_wrapper.c

~ Name:
+-----------------------------------------------+
|    CRPS counters                        |
+-----------------------------------------------+
 ~ Description:
    An example of generating stat_id for CRPS database and reading CRPS database counters:
   - go over all engines and interface of the data base and print it's configuration
   - read a counter value - implicit/explicit
   - set expected value and compare the real value to the expected
   - irpp/erpp/etpp acl rule to generate stat id
   - ingress metadata generation using acl rule

 ~ File Reference:
    crps/cint_crps_db_utils.c

~ Name:
+-------------------------------+
|    STIF configuration   |
+-------------------------------+
 ~ Description:
    An example of configuring STIF in report modes QSIZE/BILLING/INGRESS BILLING
    in single/dual/quad mode.
    - configure soc properties - enable STIF;choose ports for STIF; select the report mode
    - select the ingress and egress report size and the packet size
    - APIs sequence to connect the sources to the selected ports
    - configure the record format for ingress and egress

 ~ File Reference:
    stif/cint_stif_billing_ingress_mode.c
    stif/cint_stif_billing_mode.c
    stif/cint_stif_qsize_mode.c

~ Name:
+---------------------------------------------------------+
|    Policer generic meter configuration    |
+---------------------------------------------------------+
 ~ Description:
    An example configuring generic meter.
   - configuration for generic policer database
   - attach/detach engines (shared with crps) for the policer database
   - enable/disable/destroy generic policer database.
   - several modes of meter configurations

 ~ File Reference:
    policer/cint_policer_generic_meter.c

~ Name:
+---------------------------------------------------------+
|    Policer global meter configuration      |
+---------------------------------------------------------+
 ~ Description:
    An example configuring global meter.
    - creating groups from L2 forwarding types (UC/MC/BC/UUC/UMC)
    - configure its coresponding meter profiles
    - reconfigure the the global meter groups and profiles

 ~ File Reference:
    policer/cint_policer_global_meter.c

~ Name:
+--------------------------------------------------------+
|    Policer resolve tables configuration   |
+--------------------------------------------------------+
 ~ Description:
    An example for resolve tables configuration.
    - primary resolution configuration in parallel/serial mode
    - modify primary/final resolution

 ~ File Reference:
    policer/cint_policer_resolution_config.c

~ Name:
+-----------------------------------------+
|    Credit request profiles   |
+-----------------------------------------+
 ~ Description:
    Used to enable credit request profiles backward compatibility.
    - Legacy devices had fixed set of profiles. This cint reference creates the same fixed set of profiles.
 ~ File Reference:
    ingress_tm/cint_credit_request_profile.c

~ Name:
+--------------------------------------------------------+
|    IN-LIF WIDE DATA configuration   |
+--------------------------------------------------------+
 ~ Description:
    An example for configuring IN-LIF wide data
 ~ File Reference:
    cint_inlif_wide_data.c

~ Name:
+--------------------------------------------------------+
|    PTP 1588 configuration   |
+--------------------------------------------------------+
 ~ Description:
    An example for ptp 1588 configuration
 ~ File Reference:
    ptp/cint_ptp_1588.c

~ Name:
+-------------------+
|    EVPN Field     |
+-------------------+
 ~ Description:
    An example of EVPN Field application setup
    - Create ingress and egress objects
    - Add ESI encapsulation and termination

 ~ File Reference:
    - field/cint_field_evpn_esi.c

~ Name:
+-------------------+
|    External TCAM  |
+-------------------+
 ~ Description:
    An example of External TCAM Field application setup

 ~ File Reference:
    - field/cint_field_external.c

~ Name:
+-------------------+
|    Dir Ext EPMF   |
+-------------------+
 ~ Description:
    An example of Direct Extraction field group in EPMF stage

 ~ File Reference:
    - field/cint_field_dir_ext_epmf.c

~ Name:
+----------------------------+
|    DE complex conditions   |
+----------------------------+
 ~ Description:
    A couple of examples of complex conditions using Direct Extraction field group

 ~ File Reference:
    - field/cint_field_de_conditions.c

~ Name:
+--------------------------------------------+
|    Save cycle in IQM for outbound mirror   |
+--------------------------------------------+
 ~ Description:
    An implementation of how to disable 2nd pass Ingress mirroring in case of outbount mirror.
    in order to save cycle in IQM.

 ~ File Reference:
    - field/cint_field_outbound_mirror_save_iqm.c

~ Name:
+----------------------------+
|    External TCAM with PMF  |
+----------------------------+
 ~ Description:
    An example of External TCAM Field application setup, including configuring the PMF
    to utilize the External TCAM's result.

 ~ File Reference:
    - field/cint_field_external_with_pmf.c

~ Name:
+---------------------------------------+
|    External TCAM payload positioning  |
+---------------------------------------+
 ~ Description:
    An example of External TCAM Field application setup, including explanations regarding
    limitations about what different payoad_offset and payload_id options can be chosen.

 ~ File Reference:
    - field/cint_field_external_payload_positioning.c

~ Name:
+----------------------------------------+
|    External TCAM User defined AppType  |
+----------------------------------------+
 ~ Description:
    An example of External TCAM Field application setup, including creating a
    user defined AppType and using preselectors with it.

 ~ File Reference:
    - field/cint_field_external_ud_apptype.c

~ Name:
+----------------------------------------+
|    External TCAM with qual_packing flag  |
+----------------------------------------+
 ~ Description:
    An example of External TCAM Field application setup, where we use the flag
    BCM_FIELD_FLAG_QUAL_PACKING, indicating that we want to combine qualifiers with a size
    that is not complete bytes into less segments.

 ~ File Reference:
    - field/cint_field_external_qual_packing.c

~ Name:
+-----------------------------------------+
|    Dynamic port add and port remove     |
+-----------------------------------------+
 ~ Description:
    Examples for dynamically adding and removing ports.
 ~ File Reference:
    port/cint_dynamic_port_add_remove.c

 ~ Name:
+-------------------------------------------+
|    VPLS and MPLS Ingress 1+1 protection   |
+-------------------------------------------+
 ~ Description:
    Example of VPN flooding domain setup with one AC port and two PWE ports with VPLS and MPLS Ingress 1+1 protectio

 ~ File Reference:
    cint_dnx_vswitch_vpls_mpls_1plus1_protection.c

~ Name:
+---------------------------------------------------------------------------------------------------------------------------+
|    MPLS over VPLS - PWE tagged mode + multicast + split horizon + same interface filter + indexed/unindexed native AC     |
+---------------------------------------------------------------------------------------------------------------------------+
 ~ Description:
    Basic vswitch (mc) examples in VPLS network. Unknown flooding, split horizon,
    indexed/unindexed native AC, same-interface filter.

 ~ File Reference:
    vpls/cint_vpls_pwe_tagged_mode_split_horizon.c

~ Name:
+---------------------------------------------------------------------------------------------------------------------------+
|    VPLS P2P - PWE cross connect using vswitch APIs     |
+---------------------------------------------------------------------------------------------------------------------------+
 ~ Description:
    Point to Point (P2P) cross connect of AC/PWE to AC/PWE.

 ~ File Reference:
    vpls/cint_dnx_vswitch_cross_connect_p2p_ac_pwe.c

 ~ Name:
+-----------------------------------------------+
|    MPLS with Hierarchical TM flow (HTM) FEC   |
+-----------------------------------------------+
 ~ Description:
    Create 3 level FEC entries with HTM used by MPLS tunnels.

 ~ File Reference:
    mpls/cint_mpls_fec_htm.c

 ~ Name:
+----------------------------------+
|    MPLS PHP and SWAP from FHEI   |
+----------------------------------+
 ~ Description:
    Configure MPLS PHP and SWAP actions from FHEI.

 ~ File Reference:
    mpls/cint_mpls_operations_without_eedb.c

 ~ Name:
+----------------------------+
|    MPLS tandem over VPLS   |
+----------------------------+
 ~ Description:
    Configure MPLS and tandem MPLS EEDB entries to be encapsulated in same encap stage.

 ~ File Reference:
    mpls/cint_mpls_tandem.c

 ~ Name:
+-------------------+
|    MPLS utility   |
+-------------------+
 ~ Description:
    General MPLS utility used to create MPLS setups.

 ~ File Reference:
    utility/cint_dnx_utility_mpls.c

 ~ Name:
+-------------------+
|    VPLS utility   |
+-------------------+
 ~ Description:
    General VPLS utility used to create VPLS setups.

 ~ File Reference:
    utility/cint_dnx_utils_vpls.c
    utility/cint_dnx_utils_vpws.c

 ~ Name:
+---------------------------+
|    latency MSR            |
+---------------------------+
 ~ Description:
    General latency measurements example.

 ~ File Reference:
    latency/cint_ingress_latency.c
    latency/cint_end_to_end_latency.c
    latency/cint_end_to_end_aqm_latency.c
    latency/cint_latency_utils.c

 ~ Name:
+---------------------------+
|    Reflector              |
+---------------------------+
 ~ Description:
    Example for the required configuration for L2 and L3 reflectors.
 ~ File Reference:
    reflector/cint_reflector_l2_external.c
    reflector/cint_reflector_l2_internal.c
    reflector/cint_reflector_l3_internal.c

 ~ Name:
+---------------------------+
|    ring protection            |
+---------------------------+
 ~ Description:
    General ring protection example.

 ~ File Reference:
    cint_ring_protection.c

 ~ Name:
+---------------------------+
|    COE            |
+---------------------------+
 ~ Description:
    Basic COE example.

 ~ File Reference:
    extender/cint_dnx_coe_basic.c

 ~ Name:
+-----------------------------------------------+
|  Example of Scheduled Fabric+Egress MC        |
+-----------------------------------------------+
 ~ Description:
    An example for Scheduled Fabric+Egress MC.

 ~ File Reference:
    cint_dnx_sch_fab_eg_mc.c

 ~ Name:
+---------------------------+
|    shared shaper          |
+---------------------------+
 ~ Description:
    Example for creating E2E scheduling scheme with a shared shaper.

 ~ File Reference:
    e2e_scheduler/cint_e2e_shared_shapers.c

 ~ Name:
+---------------------------+
|    Low rate VOQ connector |
+---------------------------+
 ~ Description:
    Example for creating E2E scheduling scheme with a low rate voq connector

 ~ File Reference:
    e2e_scheduler/cint_e2e_low_rate_connectors.c

 ~ Name:
+---------------------------+
|    LAG scheduler port     |
+---------------------------+
 ~ Description:
    Example of creating and removing LAG scheduler port

 ~ File Reference:
    e2e_scheduler/cint_e2e_lag_scheduler.c

 ~ Name:
+---------------------------+
|    Virtual flows          |
+---------------------------+
 ~ Description:
    Example of creating an E2E scheduler scheme with a virtual flow in it

 ~ File Reference:
    e2e_scheduler/cint_e2e_virtual_flows.c

~ Name:
+------------------------+
|    Vlan Translation    |
+------------------------+
 ~ Description:
    Demonstrates EVE (Egress VLAN Editing) action when
    outer_tpid_action/inner_tpid_action is set to bcmVlanTpidActionOuter/bcmVlanTpidActionInner

 ~ File Reference:
    cint_vlan_translate_tpid_from_packet.c

~ Name:
+----------------------------+
| Native Vlan Translation    |
+----------------------------+
 ~ Description:
    Demonstrates Native EVE (Egress Native VLAN Editing) action.

 ~ File Reference:
    vpls/cint_vpls_native_eve_test.c
    cint_dnx_advanced_vlan_translation_mode.c

 ~ Name:
+----------------+
|    Mirror      |
+----------------+
 ~ Description:
    An example of mirror packet according to port x VLAN, with inbound/outbound mirroring. where the mirror packet:
    - Is SPAN mirrored
    - Includes RSPAN encapsulation
    - Includes ERPSAN encapsulation

 ~ File Reference:
    cint_dnx_mirror_example.c
    cint_dnx_mirror_tests.c

 ~ Name:
+----------------------------------------+
|    Stat PP - Port Objects      |
+----------------------------------------+
 ~ Description:
    An example of statistic command generation on pp ports:
    - Configure CRPS database
    - Confgure PP stat profile
    - Configure pp port statistic command generation

 ~ File Reference:
    stat_pp/cint_stat_pp_port_ingress.c

 ~ Name:
+--------------------------------------------------------+
|    Stat PP - Port Objects - CRPS + Meter     |
+--------------------------------------------------------+
 ~ Description:
    An example of statistic command generation on pp ports:
    - Configure CRPS database
    - Configure meter database
    - Confgure PP stat profiles and map them to two ports
    - Configure pp port statistic command generation

 ~ File Reference:
    stat_pp/cint_stat_pp_port_ingress_crps_meter.c

 ~ Name:
+----------------------------------------+
|    Stat PP - stat_pp configurations    |
+----------------------------------------+
 ~ Description:
    Examples and utilities for configuring statistics on fwd, lif APIs

 ~ File Reference:
    stat_pp/cint_stat_pp_config.c

 ~ Name:
+----------------------------------------+
|    Stat PP - IPV4 route stat example   |
+----------------------------------------+
 ~ Description:
    Configure statistics on 3 hierarchies of fecs, src and dst lookups in fwd
    - Configure CRPS database
    - Configure stat_pp objects (fecs, ecmp groups etc)
    - Configure IPV4 route application

 ~ File Reference:
    stat_pp/cint_stat_pp_ipv4_route.c

 ~ Name:
+------------------------------------------+
|    Stat PP - egress vsi fwd stat example |
+------------------------------------------+
 ~ Description:
    Example of egress vsi fwd statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for egress vsi
    - Configure egress vsi application

 ~ File Reference:
    stat_pp/cint_stat_pp_l2_egress_fwd.c

 ~ Name:
+--------------------------------------------------------+
|    Stat PP - egress mpls encap, php statistics example |
+--------------------------------------------------------+
 ~ Description:
    Example of egress vsi fwd statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for mpls outrif's, php
    - Configure mpls encapsulation, php applications

 ~ File Reference:
    stat_pp/cint_stat_pp_mpls_egress.c

 ~ Name:
+--------------------------------------------------+
|    Stat PP - mpls termination statistics example |
+--------------------------------------------------+
 ~ Description:
    Example of egress vsi fwd statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for mpls tunnels
    - Configure mpls termination application

 ~ File Reference:
    stat_pp/cint_stat_pp_mpls_termination.c

 ~ Name:
+--------------------------------------+
|    Stat PP - vpls statistics example |
+--------------------------------------+
 ~ Description:
    pwe port, pwe lif, ac statistics
    - Configure CRPS database
    - Configure stat_pp objects for pwe lif, pwe port, ac
    - Configure vpls application

 ~ File Reference:
    vpls/cint_vpls_statistics.c

 ~ Name:
+--------------------------------------------------+
|    Stat PP - egress outrif statistics example    |
+--------------------------------------------------+
 ~ Description:
    Example of egress outrif statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for egress outrif
    - Configure ipv4 route application

 ~ File Reference:
    stat_pp/cint_stat_pp_egress_outrif.c

 ~ Name:
+---------------------------------------------------------------+
|    Stat PP - in/out pwe port/mpls tunnel statistics example   |
+---------------------------------------------------------------+
 ~ Description:
    Example of in/out pwe port/mpls tunnel statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for in/out pwe port/mpls tunnel
    - Configure vpls application

 ~ File Reference:
    stat_pp/cint_stat_pp_vpls.c

 ~ Name:
+---------------------------------------------------------+
|    Stat PP - in/out ipv4/6 tunnel statistics example    |
+---------------------------------------------------------+
 ~ Description:
    Example of in/out ipv4/6 tunnel statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for in/out ipv4/6 tunnel
    - Configure ipv4/6 tunnel application

 ~ File Reference:
    stat_pp/cint_stat_pp_ip_tunnel.c

 ~ Name:
+----------------------------------------------+
|    Stat PP - ipv4 ipmc statistics example    |
+----------------------------------------------+
 ~ Description:
    Example of ipv4 ipmc statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for ipv4 ipmc entry
    - Configure ipv4 ipmc application

 ~ File Reference:
    stat_pp/cint_stat_pp_ipv4_ipmc.c

 ~ Name:
+----------------------------------------------+
|    Stat PP - ipv6 ipmc statistics example    |
+----------------------------------------------+
 ~ Description:
    Example of ipv6 ipmc statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for ipv6 ipmc entry
    - Configure ipv6 ipmc application

 ~ File Reference:
    stat_pp/cint_stat_pp_ipv6_ipmc.c

 ~ Name:
+-----------------------------------------------------------------------------+
|    Stat PP - 3 hierarchies of fecs, src and dst fwd lookups statistics example  |
+-----------------------------------------------------------------------------+
 ~ Description:
    Example of 3 hierarchies of fecs, src and dst fwd lookups statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for 3 hierarchies of fecs, src and dst fwd lookups
    - Configure ipv6 route application

 ~ File Reference:
    stat_pp/cint_stat_pp_ipv6_route.c

 ~ Name:
+--------------------------------------------------------+
|    Stat PP - mpls php/nop/swap fwd statistics example  |
+--------------------------------------------------------+
 ~ Description:
    Example of mpls php/nop/swap fwd statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for mpls php/nop/swap fwd entry
    - Configure mpls php/nop/swap application

 ~ File Reference:
    stat_pp/cint_stat_pp_mpls_fwd.c

 ~ Name:
+----------------------------------------------------------+
|    Stat PP - ipv4/6 loose/strict rpf statistics example  |
+----------------------------------------------------------+
 ~ Description:
    Example of ipv4/6 loose/strict rpf statistics collection
    - Configure CRPS database
    - Configure stat_pp objects for ipv4/6 loose/strict rpf
    - Configure ipv4/6 route application

 ~ File Reference:
    stat_pp/cint_stat_pp_ip_rpf.c

 ~ Name:
+----------------------------------------+
|    Stat PP - Egress metadata           |
+----------------------------------------+
 ~ Description:
    An example of statistic command generation with metadata on ETPP:
    - Configure CRPS database
    - Confgure PP stat profile
    - Configure pp egress vsi statistic command generation and metadata on eth fwd
    - Configure pp egress mpls tunnel statistis and metadata on ipv4/6 next header type

 ~ File Reference:
    stat_pp/cint_stat_pp_egress_metadata.c

 ~ Name:
+----------------------------------------+
|    Port misc configuration             |
+----------------------------------------+
 ~ Description:
    Port configurations related to other modules:
    - Port bridge configuration
    - Port trap configuration
    - Port raw header configuration
    - Port PFC configuration

 ~ File Reference:
    port/cint_port_misc_configuration.c

 ~ Name:
+----------------------------------------+
|    MPLS special label                  |
+----------------------------------------+
 ~ Description:
    Provided the MPLS special label usage example.

 ~ File Reference:
    mpls/cint_mpls_special_label.c

 ~ Name:
+----------------------------------------+
|    VPLS split horizon filter           |
+----------------------------------------+
 ~ Description:
    Provided the VPLS split horizon filter example.

 ~ File Reference:
    vpls/cint_vpls_split_horizon.c


 ~ Name:
+----------------------------------------+
|    VPLS Upstream Assigned              |
+----------------------------------------+
 ~ Description:
    Provides examples for PWE label termination in context of Previous LIF/RIF

 ~ File Reference:
    vpls/cint_vpls_per_interface.c

 ~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
     Simulates the TM ASIC bring up snake without using an actual loopback.
        - Bridge packet from one port to the next and finally from the last port to the first
        - Packet are classified at ingress according to the port Default In-LIF to select VSI=VLAN
        - MACT configured to be forwared the next port
        - An EVE action increments the VLAN to the expected value in the next port

 ~ File Reference:
    cint_l2_basic_bridge_default_ingress_with_eve.c

~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
    Example of basic L2 bridging for C_VID packet using static MACT entry

 ~ File Reference:
    cint_dnx_basic_bridge.c

 ~ Name:
+-------------------------+
|    Static transplant    |
+-------------------------+
 ~ Description:
     Test static transplant functionality.

 ~ File Reference:
    field/cint_field_static_sa_move.c

 ~ Name:
+--------------------------------+
|    MPLS Utility functions     |
+--------------------------------+
 ~ Description:
    General utility functions for mpls applications

 ~ File Reference:
    utility/cint_dnx_utils_mpls.c

 ~ Name:
+-------------------+
|    PORT utility   |
+-------------------+
 ~ Description:
    General cint_dnx_utils_port utility provides port basic functionality.

 ~ File Reference:
    utility/cint_dnx_utils_port.c

 ~ Name:
+-----------------------------------------------------------+
|    L2 Encapsulation of a Packet Tunneled to External CPU  |
+-----------------------------------------------------------+
 ~ Description:
    An example of L2 encapsulated packet configuration that is tunneled to external CPU.

 ~ File Reference:
    tunnel/cint_dnx_tunnel_l2_encap_external_cpu.c

~ Name:
+--------------------------+
|       STAT_PP            |
+--------------------------+
 ~ Description:
    Examples for statistics counting on numerous pp objects (lif, forwarding objects etc):

 ~ File Reference:
    mpls/cint_dnx_mpls_ecmp_basic.c

 ~ Name:
+-------------------+
|    Twamp application   |
+-------------------+
 ~ Description:
    TWAMP TX/RX and Reflector scenarios.

 ~ File Reference:
    twamp/cint_dnx_twamp_field.c
    twamp/cint_dnx_twamp.c

 ~ Name:
+-------------------+
|   Load Balancing  |
+-------------------+
 ~ Description:
    Example hashing functions for load balancing configurations.

 ~ File Reference:
    lb/cint_dnx_load_balancing.c

~ Name:
+-------------------------------------------+
|   L2 LIF associated with Unttaged packet  |
+-------------------------------------------+
 ~ Description:
    Example of Logical Interfaces(LIF) associated with port X untagged-packet.

 ~ File Reference:
    cint_vlan_match_untagged_packet.c

~ Name:
+------------------------------------------------+
|   L2 LIF associated with double-tagged packet  |
+------------------------------------------------+
 ~ Description:
    Example of Logical Interfaces(LIF) associated with port X double-tagged-packet:
     - Packet tagged with S-tag + C-tag VLANs.
     - Packet tagged with C-tag + C-tag VLANs.
     - Packet tagged with S-tag + S-tag VLANs.

 ~ File Reference:
    cint_vlan_match_double_tagged_packet.c

~ Name:
+----------------+
|   mLDP         |
+----------------+
 ~ Description:
    Example of mLDP configuration:
     - MPLS mLDP configuration

 ~ File Reference:
    mpls/cint_dnx_mpls_mldp.c

~ Name:
+---------------------------+
|   MPLS deep stack         |
+---------------------------+
 ~ Description:
    Example of MPLS deep stack configuration:
     - MPLS deep stack configuration

 ~ File Reference:
    mpls/cint_mpls_deep_stack.c

~ Name:
+---------------------------+
|   MPLS Alternate Marking  |
+---------------------------+
 ~ Description:
    Example of Alternate Marking ingress node with MPLS deep stack configuration:
     - MPLS deep stack configuration
     - Alternate Marking (RFC8321) configuration

 ~ File Reference:
    mpls/cint_mpls_deep_stack_alternate_marking.c

~ Name:
+---------------------------+
|   SRv6 Alternate Marking  |
+---------------------------+
 ~ Description:
    Example of Alternate Marking ingress node with SRv6 configuration:
     - SRv6 configuration
     - Alternate Marking (RFC8321) configuration

 ~ File Reference:
    srv6/cint_srv6_alternate_marking.c

~ Name:
+---------------------------+
|   IPv6 Alternate Marking  |
+---------------------------+
 ~ Description:
    Example of Alternate Marking ingress node with IPv6 configuration:
     - IPv6 configuration
     - Alternate Marking (RFC8321) configuration

 ~ File Reference:
    l3/cint_ipv6_alternate_marking.c

~ Name:
+---------------------------+
|   Port Extender           |
+---------------------------+
 ~ Description:
    Example of Port extender(802.1BR) configuration:
     - Port extender configuration

 ~ File Reference:
    cint_extender.c


~ Name:
+------------------------------+
|   Recycle Interface Shaper   |
+------------------------------+
 ~ Description:
    Example of configuring Recycle Interface Shaper.
     - Configuring Mirrored traffic CIR shapers, including:
       - Mapping original destination port to recycle mirror port, to be used for sniffed traffic.
       - Adding L2 entry.
       - Creating a Mirror destination and adding a mirroring rule.
       - Setting bandwidth of the relevant Recycle Interface.
       - Disabling Recycled/Mirrored traffic EIR shapers.
     - Configuring Recycled traffic CIR shapers, including:
       - Adding L2 entry.
       - Setting bandwidth of the relevant Recycle Interface.
       - Disabling Recycled/Mirrored traffic EIR shapers.

 ~ File Reference:
    cint_dnx_egq_rcy_if_bandwidth_set.c


~ Name:
+--------------------------------------------+
|    Egress Unknown DA fileter - PORT        |
+--------------------------------------------+
 ~ Description:
    An example of how to set the Port Egress filter for Unknown UC / Unknown MC / BC.
    Demonstrates how to:
    - set egress port filter according to type.
    - Create egress unknow da Trap for the supplied PORT.

 ~ File Reference:
    port/cint_port_egress_filter_example.c


~ Name:
+------------------------+
|    Egress CEP lookup   |
+------------------------+
 ~ Description:
    The CINT demonstrates the required JR2 configuration sequence for CEP Egress lookup.
        - Packet ESEM lookup to match Outer Tag
        - Packet ESEM lookup to match Inner Tag

 ~ File Reference:
    cint_l2_egress_cep.c


~ Name:
+------------------------+
| L3 Egress Create Trap  |
+------------------------+
 ~ Description:
    An example of how to create FEC with destination trap.

 ~ File Reference:
    traps/cint_rx_trap_in_l3_egress_create.c


~ Name:
+--------------------------+
|    VLAN-Port Learning    |
+--------------------------+
 ~ Description:
    An example of VLAN-Port learning for MP LIFs.
    Demonstrate VLAN-Port modifications that change the learn-info size in the In-LIF.

 ~ File Reference:
    cint_dnx_l2_learn_mp.c

~ Name:
+------------------------------------------------------+
|    VLAN-Port Learning in transition for P2P to MP    |
+------------------------------------------------------+
 ~ Description:
    An example of VLAN-Port learning info reconstruction when modifying an AC from P2P to MP
    Supports traffic and learning between all available VLAN-Port MP result-types.
    Provides verification that the selected result-type meets all the VLAN-Port configuration
    requirements that can affect the learning information.

 ~ File Reference:
    cint_dnx_ac_learn_p2p_to_mp.c

~ Name:
+-------------------------------------+
|    VLAN-Port Types configuration    |
+-------------------------------------+
 ~ Description:
    provides an example for different AC type configurations..

 ~ File Reference:
    cint_dnx_vlan_port_types.c

~ Name:
+--------------------------+
|    Instru Eventor        |
+--------------------------+
 ~ Description:
    An example basic Eventor configuration and semantic testing.

 ~ File Reference:
    instru/cint_instru_eventor.c

~ Name:
+-------------------------------------+
|    Instru Eventor for dying gasp        |
+-------------------------------------+
 ~ Description:
    An example dying gasp feature event configuration and generation testing.

 ~ File Reference:
    instru/cint_instru_dying_gasp.c

~ Name:
+----------------------------------------+
|    Instru synchronized counters        |
+----------------------------------------+
 ~ Description:
    An example for configuring and collecting
    synchronized counters

 ~ File Reference:
    instru/cint_instru_sync_counters.c

~ Name:
+--------------------------+
|    tdm bypass mode       |
+--------------------------+
 ~ Description:
    An example of basic tdm bypass configuration

 ~ File Reference:
    tdm/cint_tdm_bypass_mode.c

~ Name:
+----------------------------------+
|         AC 1+1 Protection          |
+----------------------------------+
 ~ Description: 1+1 Protection can be configured with an ability
   to learn a failover multicast group. The CINT provides a 1+1
   protection with multicast group learning and multicast group
   modification.

 ~ File Reference: cint_ac_1plus1_protection_learning_mc.c

~ Name:
+--------------------------+
|   PPPoE                  |
+--------------------------+
 ~ Description:
    An example of PPPoE encapusuaton and termination. Demonstrates how to:
    - basic encpasulation
    - basic termination
    - P2P scenario

 ~ File Reference:
    ppp/cint_dnx_pppoe.c

~ Name:
+--------------------------+
|   L2TP                  |
+--------------------------+
 ~ Description:
    An example of L2TPv2 encapusuaton and termination. Demonstrates how to:
    - basic encpasulation
    - basic termination
    - PPPoE and L2TPv2 P2P scenario

 ~ File Reference:
    ppp/cint_dnx_l2tp.c

~ Name:
+-----------------------+
|    PON Application    |
+-----------------------+
~ Description: An example of PON application for bringing up three main PON service categories: 1:1 Model, N:1 Model and TLS. Demonstrates the following:
    - Setting ports as NNI ports and PON ports
    - Creating PON LIFs to match Tunnel_ID and VLAN tags
    - Setting PON LIF ingress VLAN editor to perform upstream VLAN translation
    - Setting PON LIF egress VLAN editor to perform downstream VLAN translation
    - Creating NNI LIFs to match port VLAN domain and VLAN tags
    - Adding PON LIFs and NNI LIFs to vswitch or cross connecting two of them as a 1:1 service
 ~ File Reference:
    pon/cint_dnx_pon_utils.c
    pon/cint_dnx_pon_application.c

~ Name:
+--------------------------+
|   GTP                    |
+--------------------------+
 ~ Description:
    An example of GTP-U termination and encapsulation.

 ~ File Reference:
    gtp/cint_dnx_gtp.c

~ Name:
+--------------------------+
|   PFCDM                  |
+--------------------------+
 ~ Description:
    An example of PFCDM flow managment. Demonstrates how to:
    - recognize a flow for drops
    - set conditions for snooping to cpu in case relevant flow have a new drop reason.

 ~ File Reference:
    pfcdm/appl_ref_pfcdm_congest_mon.c
    pfcdm/cint_pfcdm_congest_mon.c
    pfcdm/cint_pfcdm_def.c
    pfcdm/cint_pfcdm_flow_manage.c
    pfcdm/cint_pfcdm_pmf_cfg.c
    pfcdm/cint_pfcdm_stat.c
    pfcdm/cint_pfcdm_utils.c

 ~ Name:
+-------------------+
|    rch utility    |
+-------------------+
 ~ Description:
    General cint_dnx_util_rch utility provides recycle header basic functionality.

 ~ File Reference:
    utility/cint_dnx_util_rch.c

 ~ Name:
+-------------------------------------------+
|    Bare metal virtual wire qual/action    |
+-------------------------------------------+
 ~ Description:
     Provides an example for using qualifiers and actions based
     on virtual wires in a bare metal image.

 ~ File Reference:
    field/cint_field_vw.c

 ~ Name:
+---------------------+
|    TRUNK            |
+---------------------+
 ~ Description:
    Utility for creating a trunk.

 ~ File Reference:
    - utility/cint_dnx_utils_trunk.c

 ~ Name:
+---------------------+
|    VRF REDIRECT     |
+---------------------+
 ~ Description:
    Provide an example for using RCH header to provide the VRF at 2nd pass.

 ~ File Reference:
    - cint_dnx_ip_route_vrf_redirect.c

 ~ Name:
+-----------------------------------------+
|    TRAP KEEP ORIGINAL FABRIC HEADERS    |
+-----------------------------------------+
 ~ Description:
    Provide an example for using ETPP User Trap with action to keep fabric headers.

 ~ File Reference:
    - traps/cint_rx_trap_etpp_keep_orig_fabric_hdr.c

 ~ Name:
+------------------------------------------------------+
|    RECYCLE RAW PACKET WITH ORIGINAL SYSTEM HEADERS   |
+------------------------------------------------------+
 ~ Description:
    Provide an example for recycling raw packet with original system headers.

 ~ File Reference:
    - traps/cint_rx_trap_raw_with_orig_sys_hdrs.c

 ~ Name:
+------------------------------+
|    Bridge fallback for IPMC  |
+------------------------------+
 ~ Description:
   Bridge fallback.

 ~ File Reference:
    field/cint_field_bridge_fallback.c

 ~ Name:
+------------------------------+
|    SLLB basic configuration  |
+------------------------------+
 ~ Description:
   SLLB basic configuration for VIP and direct routing.

 ~ File Reference:
    cint_sllb_basic.c

 ~ Name:
+---------------------------------+
|    SLLB field processor example |
+---------------------------------+
 ~ Description:
   SLLB field processor example for snooping and UDH stamping.

 ~ File Reference:
    cint_sllb_field.c

~ Name:
+---------------------------------+
|    OAM/BFD Trap reconfiguration |
+---------------------------------+
 ~ Description:
   Configure OAM/BFD trap to correctly process CPU injected packets.

 ~ File Reference:
    field/cint_field_oam_layer_index.c

~ Name:
+-----------------------------------------+
|    PMF - Update ETH header of 1588oEth  |
+-----------------------------------------+
 ~ Description:
   An example of update ETH of PTPoE traffic.

 ~ File Reference:
    field/cint_field_ieee1588_eth_update.c

~ Name:
+---------------------------------+
|    FlexE Configuration          |
+---------------------------------+
 ~ Description:
   FlexE utilities and configurations for different scenarios.
   Demonstrates how to:
   - Create FlexE PHY, group, client, flow and L1 port
   - Create L1/L2 datapath for both centralized mode and distributed mode
   - Create FlexE failover datapath
   - Create FlexE group with multiple FlexE PHYs(includes PHY adding and removal)
   - FlexE client test(capacity, synchronous/asynchronous client speed, mixed client speeds, etc)
   - FlexE calendar test(synchronous/asynchronous calendar and calendar switching)
   - FlexE snake test
   - FlexE BYPASS mode
   - Other miscellaneous configurations

 ~ File Reference:
    port/cint_dynamic_port_add_remove.c
    cint_flexe_test.c

~ Name:
+---------------------------------+
|    FlexE 1588                   |
+---------------------------------+
 ~ Description:
    FlexE 1588 example

 ~ File Reference:
    port/cint_dynamic_port_add_remove.c
    cint_flexe_test.c
    cint_flexe_1588.c

~ Name:
+--------------------------+
|   PP properties          |
+--------------------------+
 ~ Description:
    An example for configuring removed from a trunk port with pp properties

 ~ File Reference:
    port/cint_port_packet_processing_example.c

~ Name:
+--------------------------+
|   FABRIC CONNECTIITY     |
+--------------------------+
 ~ Description:
    An exmaple that shows how to align result from bcm_fabric_link_connectivity_status_get and
    keep using link number as index. Unused or detached ports will be marked as bcmFabricDeviceTypeUnknown.
    Misalignment may occur due to:
        - links used for ILKN over fabric ports
        - not all fabric ports attached

 ~ File Reference:
    utility/cint_dnx_utils_fabric.c

~ Name:
+--------------------------+
|   Asym AC DYN Learn      |
+--------------------------+
 ~ Description:
    An exmaple that updates the fec protection information for ingress ac and encap protection for egress ac

 ~ File Reference:
    cint_failover_asym_ac_dynamic_learning.c

~ Name:
+--------------------------------------+
|   Service Optimization AC Learn      |
+--------------------------------------+
 ~ Description:
    An example for learning based on service optimization AC

 ~ File Reference:
    cint_dnx_service_optimization_ac_learn.c

~ Name:
+--------------------------------------+
|   AC Learn physical port change      |
+--------------------------------------+
 ~ Description:
    An example for modifying the learn destination of an AC by supplying a different physical port.

 ~ File Reference:
    cint_dnx_ac_learn_port_change.c

~ Name:
+-----------------------------------+
|   MACT Independent VLAN learning  |
+-----------------------------------+
 ~ Description:
    Example of independent VLAN learning for L2 bridging.

 ~ File Reference:
    cint_dnx_mact_ivl.c

~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
    Utility function for MACT configuration.

 ~ File Reference:
    cint_dnx_mact.c

~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
    Example function for traversing MACT table.

 ~ File Reference:
    cint_dnx_mact_traverse.c


~ Name:
+----------------------------------------------+
|   CRPS flow encapulated with UDPoIPV4 tunnel |
+----------------------------------------------+
 ~ Description:
    Example of field processor to encapsulate CRPS flow with UDPoIPV4 tunnel and send it to a CPU port

 ~ File Reference:
    field/cint_field_crps_records.c

~ Name:
+-----------------------------------+
|   L2 compatible MC IVL support    |
+-----------------------------------+
 ~ Description:
    Example of IPv4/IPv6 MC L2 IVL flow.

 ~ File Reference:
    multicast/cint_dnx_multiast_bridge_ivl.c

 ~ Name:
+------------------------------------+
|  Linked list of Multicast lists    |
+------------------------------------+
 ~ Description:
    An example of Linked list of multicast lists. Demonstrates how to:
    - Create ingress and egress linked list of multicast lists.
    - A main ingress multicast group with list of ingress multicast groups as the members.
    - A main egress multicast group with list of egress multicast groups as the members.

 ~ File Reference:
    multicast/cint_dnx_multicast_linked_lists.c

~ Name:
+------------------------------------+
|    IP Unicast Routing Utilities    |
+------------------------------------+
 ~ Description:
    Utility functions for L3 applications

 ~ File Reference:
    utility/cint_dnx_utils_l3.c

 ~ Name:
+--------------------------+
|    Multicast Utilities   |
+--------------------------+
 ~ Description:
    Utility functions for multicast applications.

 ~ File Reference:
    utility/cint_dnx_utils_multicast.c

~ Name:
+-------------------------------------------+
|    Vlan Translation Utility functions     |
+-------------------------------------------+
 ~ Description:
    Utility functions for Vlan Translation

 ~ File Reference:
    utility/cint_dnx_utils_vlan_translate.c

 ~ Name:
+----------------------------+
|    VPLS basic examples     |
+----------------------------+
 ~ Description:
    Provide the VPLS basic examples

 ~ File Reference:
    vpls/cint_vpls_basic.c

 ~ Name:
+----------------------------------+
|    General Utility functions     |
+----------------------------------+
 ~ Description:
    General utility functions

 ~ File Reference:
    utility/cint_dnx_utils_global.c

~ Name:
+----------------------+---------+
|    ROO   |      VPLS | MC      |
+----------------------+---------+

 ~ Description:
    Cover v4 IPMC for native to tunnel scenario
    covers different types of access and network copies for the ipmc group

 ~ File Reference:
    vpls/cint_vpls_roo_mc.c

 ~ Name:
+------------------------------------+
|    Hierarchical Protection VPLS    |
+------------------------------------+
 ~ Description:
    An example of VPLS hierarchical protection.

 ~ File Reference:
    vpls/cint_vpls_hvpls.c

~ Name:
+--------------------------+
|    Bridge application    |
+--------------------------+
 ~ Description:
     Demonstrates basic l2 bridging with IVE and EVE actions for differnt canonical formats
        - Bridge packet from in port to out port.
        - Includes IVE modification to support untagged and double tagged cannonical format
        - Includes various EVE modification examples
        - Includes In-LIF lookups for sinfle and double tagged packets
        - Includes In-LIF resolutions for various packet types according to port default
        - Includes QoS Remarking example
        - Includes Symmetric LIF example for learning

 ~ File Reference:
    cint_dnx_l2_basic_bridge_with_vlan_editing.c

~ Name:
+------------------------------------+
|    Bridge application multicast    |
+------------------------------------+
 ~ Description:
     Demonstrates L2 multicast with various flooding types
        - Perform Broadcast flooding according to a VSI based MCID destination using Ingress Multicast
        - Perform known mutlicast flooding according to an SVL lookup using Egress Multicast
        - Includes EVE modification in order to not perform any EVE change to the packet.

 ~ File Reference:
    cint_dnx_l2_multicast.c

 ~ Name:
+-------------------+
|    L2 Traverse    |
+-------------------+
 ~ Description:
    An example of L2 traverse.
    - Traverse all entries in the L2 table using bcm_l2_traverse
    - Traverse matching entries in the L2 table in the L2 table by bcm_l2_matched_traverse
    - Traverse matching entries with flexible mask in the l2 table in the l2 table by bcm_l2_match_masked_traverse.

 ~ File Reference:
    cint_dnx_l2_traverse.c

~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
     Demonstrates basic L2 bridging with IVE and EVE actions for an untagged canonical format
        - Bridge packet from in port to out port.
        - Includes IVE to an untagged cannonical format
        - Includes EVE from an untagged cannonical format

 ~ File Reference:
    cint_dnx_unknown_l2uc.c

~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
     Demonstrates ingress ang egress vlan port membership
        - Bridge packet from in port to out port.
        - Includes Egress VLAN membership filtering on VID changed by EVE

 ~ File Reference:
    cint_dnx_vlan_membership.c

~ Name:
+-------------------------------------------+
|    Switch/Port TPID Utility functions     |
+-------------------------------------------+
 ~ Description:
    Utility functions for Switch/Port TPID

 ~ File Reference:
   utility/cint_dnx_utils_tpid.c

~ Name:
+---------------------+
|    L2 Bridge Trunk  |
+---------------------+
 ~ Description:
    An example of L2 Bridge trunk.
    - Cover basic L2 bridging with IVE and EVE actions for canonical format untag.
    - Create a basic L2VPN scheme with two different Logical Interfaces (LIFs) as follows:
      - InLif: Based on Default_port_lif.                                                                                                           *
      - Outlif: Based on Default_ac_profile based on egress port.
    -  The packets are forwarded to the egress using MACT (VSI and MacDa) with Trunk_ID forwarding information.

 ~ File Reference:
    cint_dnx_l2_bridge_trunk.c

~ Name:
+--------------------------------------------------+
|          L3 route in  multiple device            |
+--------------------------------------------------+
 ~ Description:
    The CINT provides L3 route servcie examples for multiple device

 ~ File Reference:
   l3/cint_dnx_ip_route_multi_device.c

~ Name:
+--------------------------------------------------------+
|        LSR variable scenario                           |
+--------------------------------------------------------+
 ~ Description:
    The CINT provides LSR variable scenario example

 ~ File Reference:
   mpls/cint_dnx_mpls_check_ver.c

 ~ Name:
+--------------------------+
|        MPLS LSR          |
+--------------------------+
 ~ Description:
    MPLS multidevice and interiop examples for swap, termination, encapsulation and PHP:

 ~ File Reference:
   mpls/cint_dnx_mpls_lsr.c

 ~ Name:
+----------------------------+
|    Traditional Bridge 1    |
+----------------------------+
 ~ Description:
    After the boot up, the device acts as a traditional bridge.
    By this setting, you can switch packets from one port to another by means of the Ethernet header. This also enables users to define VLANs.

 ~ File Reference:
    - Spanning-Tree Groups: cint_dnx_stg.c

~ Name:
+----------------------------+
|     MPLS qos mapping       |
+----------------------------+
~ Description:
  An example of mpls exp mapping
  - create ingress remark qos profile and add exp mapping
  - create egress remark qos profile and add exp mapping
  - set qos profile to mpls tunnel gport

 ~ File Reference:
   qos/cint_dnx_mpls_qos.c

 ~ Name:
+-------------------+
|    VxLAN basic    |
+-------------------+
 ~ Description:
    An example of basic VXLAN and VXLAN6 application.

 ~ File Reference:
    vxlan/cint_dnx_vxlan_basic.c

 ~ Name:
+-------------------+
|    VxLAN ROO MC   |
+-------------------+
 ~ Description:
    An example of VXLAN ROO MC application.

 ~ File Reference:
    vxlan/cint_dnx_vxlan_roo_mc.c

~ Name:
+-----------------------------------+
|    IP Route over AC               |
+-----------------------------------+
 ~ Description:
    An example of IPv4 route over AC that belongs to different VSI's

 ~ File Reference:
    l3/cint_dnx_route_over_ac.c

 ~ Name:
+-----------------------+
|    ip route scenarios |
+-----------------------+
 ~ Description:
    ip route scenarios

 ~ File Reference:
    l3/cint_dnx_ip_route_scenarios.c

 ~ Name:
+-----------------------+
|    Service using EEI  |
+-----------------------+
 ~ Description:
    Service using EEI

 ~ File Reference:
    l3/cint_dnx_service_using_eei.c

 ~ Name:
+-------------------------+
|   ingress push with EEI |
+-------------------------+
 ~ Description:
    Verify ingress push command with EEI

 ~ File Reference:
    mpls/cint_dnx_mpls_ingress_push.c

 ~ Name:
+--------------------------+
|   ipmc functions         |
+--------------------------+
 ~ Description:
    Some cint functions to verify IPMC.

 ~ File Reference:
    ipmc/cint_dnx_ipmc_route_examples.c

 ~ Name:
+--------------------------+
|  ip tunnel functions     |
+--------------------------+
 ~ Description:
    Some cint functions to verify IP tunnel.

 ~ File Reference:
    tunnel/cint_dnx_ip_tunnel_basic.c

 ~ Name:
+--------------------------+
|  IP-GRE L3VPN functions  |
+--------------------------+
 ~ Description:
    Some cint functions to verify IP-GRE L3VPN.

 ~ File Reference:
    tunnel/cint_dnx_mvpn.c
    
 ~ Name:
+--------------------------+
|   KAPS hitbit functions  |
+--------------------------+
 ~ Description:
    Cint functions to verify KAPS hitbit.

 ~ File Reference:
    l3/cint_dnx_kaps_hitbits.c

 ~ Name:
+--------------------------+
|   MPLS-ECMP functions   |
+--------------------------+
 ~ Description:
    Two levels MPLS-ECMP configuration.

 ~ File Reference:
    l3/cint_dnx_mpls_ecmp.c

  ~ Name:
+--------------------+
|    Metro Bridge    |
+--------------------+
 ~ Description:
    This application enables you to define an L2VPN service.
    In VPN, you can create a Virtual Switch Instance (VSI/VPN) and connect logical ports to it.

 ~ File Reference:

    - Multipoint Metro Service:
            cint_dnx_vswitch_metro_mp.c - Supports multi device. Please note that some function signatures might have changed.

~ Name:
+--------------------------+
|       QOS                |
+--------------------------+
 ~ Description:
    QOS example for basic route l3 remark:

 ~ File Reference:
   qos/cint_dnx_qos_l3_remark.c

 ~ Name:
+-------------+
|    VPLS     |
+-------------+
 ~ Description:
    Basic VPLS incapsulation and termination scenario.

 ~ File Reference:
    vpls/cint_dnx_vpls_mp_basic.c

 ~ Name:
+-----------------------+
|    MACT Learning 1    |
+-----------------------+
 ~ Description:
    Example of different learning modes and message types.
    Relevant for PP users.

 ~ File Reference:
    cint_dnx_l2_learning.c

 ~ Name:
+------------------+
|    CPU Shadow    |
+------------------+
 ~ Description:
    An example of sending MACT events to CPU.

 ~ File Reference:
    cint_dnx_l2_cpu_learning.c

 ~ Name:
+---------------------------+
|    Learn Event Parsing    |
+---------------------------+
 ~ Description:
    - Manually construct DSP packets according to predefined parameters
    - Transmit packet and save LEM Payload
    - Read entry from LEM, add via BCM API
    - Save LEM Payload and compare to previous entry

 ~ File Reference:
    cint_dnx_l2_learn_event_parsing.c

 ~ Name:
+---------------------------------------+
|    MACT Management                    |
+---------------------------------------+
 ~ Description:
      An example of manipulate and manage the MACT.

 ~ File Reference:
       cint_dnx_l2_mact.c

 ~ Name:
+---------------+
|    SA Drop    |
+---------------+
 ~ Description:
    An example of dropping Packets according to SA.

 ~ File Reference:
    cint_dnx_l2_mact_sa_discard.c

~ Name:
+----------------------------------+
|          1:1 Protection          |
+----------------------------------+
 ~ Description:
    1:1 Protection is configured with an ability to select a unicast path via FEC protection and multicast
    path using Egress protection.
    The CINT provides two 1:1 protection examples:
    - 1:1 Protection for both unicast and multicast packets
    - 1:1 Protection combined with 1+1 Ingress Protection for unicast traffic

 ~ File Reference:
        cint_dnx_ac_1to1_coupled_protection.c

~ Name:
+-------------------------------------------+
|          Facility FEC Protection          |
+-------------------------------------------+
 ~ Description:
    The CINT provides two protection examples:
    -Facility FEC (The Super-FEC Protection Pointer is -2)
    -Facility FEC with simultaneous mode (Facility & The Super-FEC Protection Pointer has a vaild Path pointer)

 ~ File Reference:
        cint_dnx_ac_fec_facility_protection.c

~ Name:
+----------------------------------+
|          1+1 Protection          |
+----------------------------------+
 ~ Description:
    1+1 Protection can be configured with an ability to learn a failover multicast
    group in JR2 devices.
    The CINT provides a 1+1 protection for multi_devices system

 ~ File Reference:
        cint_dnx_ac_1plus1_protection_learning.c

 ~ Name:
+------------------------+
|    Vlan Compression    |
+------------------------+
 ~ Description:
       Demonstrates example to add/delete Vlan Range used for Vlan Compression.

 ~ File Reference:
       cint_dnx_port_vlan_range.c

 ~ Name:
+-----------------------+
|    MACT Learning 2    |
+-----------------------+
 ~ Description:
    An example of learning. Demonstrate how to control the learning message per vlan, send CPU or to OLP (for automatic learning)

 ~ File Reference:
    cint_dnx_l2_vlan_learning.c

~ Name:
+-------------------------------------------------+
|    PMF - Set Various parameters for 1588 frame  |
+-------------------------------------------------+
 ~ Description:
   An example of setting various parameters for 1588 frame via bcmFieldActionIEEE1588.

 ~ File Reference:
    field/cint_field_action_ieee1588.c

 ~ Name:
+---------------------------------------+
|          Example for RCPU using       |
+---------------------------------------+
 ~ Description:
    An example of RCPU.
     - ETH RCPU Packet inserted via in_port, assuming tm_port_header_type of in_port is ETH
     - ETH forwarding to CPU_RCPU_port
     - CPU_local_port transmit ETH RCPU replay
     - ETH RCPU replay forward to out_port

 ~ File Reference:
    cint_dnx_cmic_rcpu.c

~ Name:
+--------------------------------+
|    System Vswitch Resources    |
+--------------------------------+
 ~ Description:
   An example of system resources in local mode.
    - All CINTs assume global resources for LIF and L2 FEC
    - The CINTs provide an example of how to allocate in local mode
    - Provided examples: VLAN-Port, MPLS-Port

 ~ File Reference:
   cint_dnx_system_vswitch.c
   vpls/cint_dnx_system_vswitch_vpls.c

 ~ Name:
+-----------------------+
|    Link Monitoring    |
+-----------------------+
 ~ Description:
    Example application to monitor the state of the links and to react to ports/links failure.

 ~ File Reference:
    fabric/cint_linkscan.c


 ~ Name:
+-------------------------------------------+
|    Egress Acceptable Frame Type Filter    |
+-------------------------------------------+
 ~ Description:
    An example of egress acceptable frame type filter.

 ~ File Reference:
    cint_dnx_egress_acc_frame_type.c

 ~ Name:
+------------------+
|    Port Match    |
+------------------+
 ~ Description:
    Configure several ingress/egress mappings over the same gport.
    Also demonstrates usage of asymmetric ingress-only and egress-only LIFs.

 ~ File Reference:
    cint_dnx_port_match.c

 ~ Name:
+--------------------------+
|    ECMP user profile     |
+--------------------------+
 ~ Description:
    An example of ECMP group creation using an user profile.

 ~ File Reference:
    l3/cint_dnx_ecmp_eudp_example.c

 ~ Name:
+----------------------------------------+
|    Stat PP - VPBR stat example   |
+----------------------------------------+
 ~ Description:
    Configure statistics on 3 hierarchies of fecs, src and dst lookups in fwd
    - Configure CRPS database
    - Configure stat_pp objects (fecs, ecmp groups etc)
    - Configure routing application
    - Configure IPv4/6 host and vpbr entries

 ~ File Reference:
    stat_pp/cint_stat_pp_vpbr.c

~ Name:
+-----------------------------+
|    Unicast RPF              |
+-----------------------------+
 ~ Description:
    An example of unicast RPF/loose and strict/ configuration.

 ~ File Reference:
    l3/cint_dnx_ip_route_rpf_basic.c

 ~ Name:
+--------------------------+
|    ECMP basic            |
+--------------------------+
 ~ Description:
    An example of an ECMP group creation.

 ~ File Reference:
    l3/cint_dnx_ecmp_basic.c
