$Id: README.txt,v 1.99 2017/10/25 15:02:00 sd015526  Exp $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$

 ~ Name:
+--------------------------+
|    L3 Tunnels
+--------------------------+
 ~ Description:
    An example of IPv6 tunnel encapsulation/ termination scenarios. Demonstrates how to:
    - Encpasulation/Termination of various IP/Vxlan tunnel types depends on the additional
      header.
    - Termination of various IP/Vxlan tunnels.
    - Create ingress and egress objects needed to trigger IPv6 tunnel
      encapsulation
    - Add tunnel entries

 ~ File Reference:
    cint_dnx_ipv6_tunnel_encapsulation.c
    cint_vxlan_tunnel_termination.c
    cint_vxlan_roo_basic.c
    cint_vxlan_bud_node.c
    cint_dnx_ip_tunnel_encapsulation.c
    cint_dnx_ip_tunnel_termination.c
    cint_dnx_ipv6_tunnel_encapsulation_basic.c
    cint_dnx_ipv6_tunnel_termination_basic.c
    cint_dnx_ipv6_tunnel_termination.c
    cint_vxlan_split_horizon.c
    cint_vxlan_mega_config.c
    cint_ip_tunnel_to_tunnel_basic.c
    cint_ip_tunnel_to_tunnel.c

 ~ Name:
+------------+
|    VXLAN  |
+------------+
 ~ Description:
    VXLAN encapsulation/termination various scenarios

 ~ File Reference:
    cint_vxlan_mega_config.c
 ~ Name:
+---------------------------------------------------+
|    IP Unicast Routing Source MAC Encapsulation    |                                  .
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
    cint_ip_route_encap_src_mac.c

 ~ Name:
+---------------------------------------------------+
|    Basic Routing Test,on the bridge_router image  |                                  .
+---------------------------------------------------+
 ~ Description:
    An example of IPv4 Route test, based on bridge_router image
    Demonstrates how to:
    - Loading device with device_image=Bridge_Router
    - Add route and host entries
    - Send IPv4oETH0 packet
    - Verify the packet status.

 ~ File Reference:
    cint_bridge_router.c


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
    cint_ip_unknown_destination.c

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
    cint_multicast_pp_basic.c

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
    - cint_dnx_evpn_basic.c

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
    - cint_dnx_evpn_vxlan.c

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
    - cint_dnx_evpn_trunk.c

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
    - cint_dnx_evpn_protection.c

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
    - cint_dnx_utils_evpn.c

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
    cint_rx_trap_etpp_mtu_trap_lif.c

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
    cint_rx_trap_etpp_mtu_trap_rif.c

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
    cint_rx_trap_etpp_mtu_trap_port.c

 ~ Name:
+--------------------------+
|    ETPP Trap Actions     |
+--------------------------+
 ~ Description:
    An example of ETPP User Defined trap connected to MTU trap per RIF.
    Demonstrates how to:
    - Create an ETPP User Defined trap with all possible ETPP Trap actions.
    - Create MTU Trap for the supplied RIF.

 ~ File Reference:
    cint_rx_trap_etpp_trap_actions.c

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
    cint_rx_trap_l2cp_plus_subtype.c

 ~ Name:
+--------------------------+
|    L2CP Exhaustive       |
+--------------------------+
 ~ Description:
    Random L2CP traps allocation together with basic L2 forwarding,
    for the packets that don't match on the L2CP traps.
    PORTED FROM: cint_l2cp_traps_config.c

 ~ File Reference:
    cint_rx_trap_l2cp_exhaustive.c

 ~ Name:
+--------------------------+
|  FHEI Trap J1 + J2 mode  |
+--------------------------+
 ~ Description:
    Creating Trap and setting FHEI stamping.
    Creating PMF rule with Action TRAP-Code and TRAP-Qual.

 ~ File Reference:
    cint_rx_trap_fhei_config.c

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
    cint_rx_trap_saequalsda_trap.c

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
    cint_rx_trap_erpp_epmf_trap_action.c

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
    cint_rx_trap_erpp_traps.c

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
    cint_rx_trap_etpp_traps.c

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
    cint_rx_trap_ingress_traps.c

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
    cint_rx_trap_programmable_traps.c

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
    cint_rx_trap_protocol_traps.c

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
    cint_rx_trap_lif_traps.c

 ~ Name:
+--------------------------+
|    Trap Utilities        |
+--------------------------+
 ~ Description:
    Contains all Trap utility functions.
    Demonstrates how to:
    - Use all utility functions for basic trap configuration creation.

 ~ File Reference:
    cint_utils_rx.c

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
    cint_rx_trap_1588_traps.c

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
    cint_rx_trap_icmp_redirect.c

 ~ Name:
+-------------+
|   sFlow     |
+-------------+
 ~ Description:
     Contains the sequence required to configure an sFlow agent on the device.
     In addition, it creates an ip route flow to demonstrate.

 ~ File Reference:
    cint_sflow_basic.c


 ~ Name:
+--------------------------+
|    VOQ attributes        |
+--------------------------+
 ~ Description:
   Confuguration of VOQ attributes

 ~ File Reference:
    ingress_tm/cint_queue_attributes.c

 ~ Name:
+---------------------------+
| VOQ rate class thresholds |
+---------------------------+
 ~ Description:
   Confuguration of VOQ rate class thresholds

 ~ File Reference:
    ingress_tm/cint_rate_class_thresholds.c

 ~ Name:
+--------------------------+
|    DRAM bound            |
+--------------------------+
 ~ Description:
   Confuguration of DRAM bound thresholds

 ~ File Reference:
    ingress_tm/cint_dram_bound_thresh.c

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
   Confuguration VoQ to be in SRAM only mode

 ~ File Reference:
    ingress_tm/cint_voq_sram_only.c


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
    cint_vpls_mpls_qos.c

~ Name:
+--------------------------+
|  Qos in vlan translation |
+--------------------------+
 ~ Description:
    Example of QoS mapping in ingress and egress vlan translation.

 ~ File Reference:
    cint_qos_l2_vlan_edit.c

 ~ Name:
+--------------------------+
|    MPLS Per interface
+--------------------------+
 ~ Description:
    An example of MPLS termination and forwarding where the label
    lookup is coupled with the previously found interface (RIF/LIF)

 ~ File Reference:
    cint_mpls_per_if.c

 ~ Name:
+--------------------------+
|    MPLS FRR
+--------------------------+
 ~ Description:
    An example of MPLS FRR termination and related port configuration

 ~ File Reference:
    cint_dnx_mpls_frr_basic.c

 ~ Name:
+--------------------------+
|    VPLS - utility
+--------------------------+
 ~ Description:
    Utility cint based on cint_dnx_utility_mpls.c,
    including basic VPLS termination and encapsulation examples.

 ~ File Reference:
    cint_dnx_utility_vpls_advanced.c

 ~ Name:
+--------------------------+
|    VPLS - no MPLS
+--------------------------+
 ~ Description:
    Basic example of configuring VPLS encapsulation and termination without MPLS tunnel.

 ~ File Reference:
    cint_vpls_no_mpls.c

 ~ Name:
+--------------------------+
|    VPLS - tagged PWE
+--------------------------+
 ~ Description:
    Basic example of adding and terminating service delimiting tags (vlans)
    to a pwe lif.

 ~ File Reference:
    cint_vpls_pwe_tagged_mode_basic.c

 ~ Name:
+---------------------------------------+
|    ROO   |  VPLS - indexed PWE tagged |
+---------------------------------------+
 ~ Description:
    An example of termination and encapsulation of ROO with tagged indexed PWE,
    where native ETH is terminated and not used for forwarding.

 ~ File Reference:
    cint_indexed_pwe_tagged_roo.c

 ~ Name:
+--------------------------------+
|    VPLS - tagged PWE advanced
+--------------------------------+
 ~ Description:
    Advanced PWE tagged mode examples with different number of service tags

 ~ File Reference:
    cint_vpls_pwe_tagged_mode_advanced_scenario.c

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
    cint_dnx_qos_ingress_policer.c

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
     cint_dnx_qos_ecn.c

~ Name:
+--------------------------------------+
|           L3 QOS PHB/Remark          |
+--------------------------------------+
 ~ Description:
    An example of L3 QOS.
    Demonstrates how to:
    - Create qos map id
    - Create qos opcode
    - Mapping map_id to opcode
    - Mapping opcode to L3 QOS

 ~ File Reference:
    cint_qos_l3_remark.c

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
     cint_dnx_qos_l2_port.c

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
    cint_dnx_qos_php.c

~ Name:
+--------------------------------------+
|    Instru IPT (Tail-Edit and INT)    |
+--------------------------------------+
 ~ Description:
    An example of how to configure IPT (Tail-Edit and INT) profiles with metadata and relevant headers
    Demonstrates how to:
    - Set IPT profile metadata
    - Set Switch ID (one of the attributes in metadata)
    - Set INT  first node header

 ~ File Reference:
    cint_instru_ipt.c


 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - FEM handling, 'increment' feature    |
+-----------------------------------------------------------+
 ~ Description:
    Provides an example of building setup for FEM: In this case,
    setting preselection, creating context, adding field group,
    adding FEM, attaching field group and context.

 ~ File Reference:
    cint_field_fem_increment.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - 256 data qualifiers create           |
+-----------------------------------------------------------+
 ~ Description:
    Create 256 Data qualifiers without ID and assign them to
    Field groups.

 ~ File Reference:
    cint_field_data_qualifier_create.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Application - OAM bfd Echo                         |
+-----------------------------------------------------------+
 ~ Description:
    Provides an example of building setup for bfd Echo application

 ~ File Reference:
    cint_field_bfd_echo_j2.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Context Selection                                  |
+-----------------------------------------------------------+
 ~ Description:
    An exmaple to how create context and configure context
    Selection table to always hit that context

 ~ File Reference:
    cint_field_always_hit_context.c

 ~ Name:
+-----------------------------------------------------------+
|    PMF Qualify Upon VID                                   |
+-----------------------------------------------------------+
 ~ Description:
    An exmaple to how to qualify upon VID

 ~ File Reference:
    cint_field_vid_qual.c

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
    cint_field_ace.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF EXEM - Configure an exact match field group        |
+-----------------------------------------------------------+
 ~ Description:
    Provides an example of configuring an exact match
    field group.

 ~ File Reference:
    cint_field_exem.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Drop action creation                              |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to create Action drop for PMF to hold only 1 bit in payload

 ~ File Reference:
    cint_field_action_drop.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Redirect action                                   |
+-----------------------------------------------------------+
 ~ Description:
    Example of using bcmFieldActionRedirect action with value BCM_GPORT_BLACK_HOLE.

 ~ File Reference:
    cint_field_action_redirect.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - SNIFF creation Ingrees and Egress                 |
+-----------------------------------------------------------+
 ~ Description:
    Show an example how PMF can generate Sniff Commands in Ingress and Egress using TCAM FG,
    also allows to add entries per stage per action


 ~ File Reference:
    cint_field_sniff.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - PPPoE                                             |
+-----------------------------------------------------------+
 ~ Description:
      CINT script example for followed using PMF to redirect PPPoE
      protocol packets to CPU

 ~ File Reference:
    cint_field_pppoe.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Admit profile                                       |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to configure bcmFieldActionAdmitProfile
    to update the admit_profile value.

 ~ File Reference:
    cint_field_admit_profile.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Attach API                                       |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how bcm_field_group_context_attach() API can be used
    Configures 1 Field group that can be used for Bridged and IPv4 Routed packets

 ~ File Reference:
    cint_field_attach.c
    
  ~ Name:
+-----------------------------------------------------------+
|    PMF - User qualifier Header                            |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to create user qualifer Haeder.

 ~ File Reference:
    cint_field_user_qual_header.c    
    

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Assign BIER Properties                           |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to assign bier properties to arbitrary mpls packet

 ~ File Reference:
    cint_field_bier.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - OAM Echo BFD acl Rule                             |
+-----------------------------------------------------------+
 ~ Description:
    How to configure PMF for echo BFD
    Discriminator taken from PDU at offset 10

 ~ File Reference:
    cint_field_bfd_echo.c

  ~ Name:
+-----------------------------------------------------------+
|  PMF - Context compare API. 'Double' mode.                |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how bcm_field_context_compare_create() API can be used in mode:
    bcmFieldContextCompareTypeDouble.
    Configure FG in IPMF2 which will use this compare result as part of its key.
 ~ File Reference:
    cint_field_compare_double.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Context compare API with full 'offset_get' feature. |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to create a context in the compare mode Double using 1 pair and it is
    using bcm_field_compare_operand_offset_get() to get the full offset of
    the compare operand inside the 6-bit compare result.
 ~ File Reference:
    cint_field_compare_full_offset_get.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Context compare API. 'Single' mode.               |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to compare a value stored in initial
    keys to TCAM result of 1 of the lookups done in iPMF1.
 ~ File Reference:
    cint_field_compare_single.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Context compare API with spec 'offset_get' feature. |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to create a context in the compare mode Double using 2 pairs and it is
    using bcm_field_compare_operand_offset_get() to get the offset
    of a specific compare operand inside the 6-bit compare result.
 ~ File Reference:
    cint_field_compare_specific_offset_get.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Context cascading feature                           |
+-----------------------------------------------------------+
 ~ Description:
    CINT example, shows how to switch the selected context in IPMF1
    to with new one in IPMF2 stage.

 ~ File Reference:
    cint_field_contexts_cascading.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Ctest meta data clear                               |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to clean the meta data in the Ctests.

 ~ File Reference:
    cint_field_ctest_clear.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Ctest meta data configure                           |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to configure some meta data to qualify upon.

 ~ File Reference:
    cint_field_ctest_config.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Egress mirror enable                                |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to configure bcmFieldActionMirrorEgress
    to update the egress mirror_cmd value.

 ~ File Reference:
    cint_field_egress_mirror.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Fes valid bit usage                                 |
+-----------------------------------------------------------+
 ~ Description:
    Cint example of usage of the FES valid bit.

 ~ File Reference:
    cint_field_fes_valid_bit.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Hashing feature                                     |
+-----------------------------------------------------------+
 ~ Description:
    CINT example about hashing feature.

 ~ File Reference:
    cint_field_hashing.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - IPMF2 to IPMF3 cascading                            |
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to configure IPMF2 to IPMF3 cascading.

 ~ File Reference:
    cint_field_ipmf2_ipmf3_cascading.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - ITMH application
+-----------------------------------------------------------+
 ~ Description:
    CINT example for how to enable the ITMH application.

 ~ File Reference:
    cint_field_itmh.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - key_gen_var feature. TCAM usage.                  |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to create constant values per context and set it as qualifier.
    For TCAM FG.
 ~ File Reference:
    cint_field_key_gen_var.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Vport/ Lif (in or out) Action                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how PMF use should use  bcmFieldActionOutVport0  bcmFieldActionIngressGportSet

 ~ File Reference:
    cint_field_lif_action.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Mirror Action                                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how PMF use should use bcmFieldActionMirrorIngress

 ~ File Reference:
    cint_field_mirror.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - MPLS X label presel                                 |
+-----------------------------------------------------------+
 ~ Description:
    Configure Field Processor Program Selection Preselector for header format, MPLS any label any L2/L3.

 ~ File Reference:
    cint_field_mpls_x_presel.c


+-----------------------------------------------------------+
| PMF - IP Snoop                                            |
+-----------------------------------------------------------+
 ~ Description:
    Snoop IP-routed packets using PMF configuration

 ~ File Reference:
    cint_field_ip_snoop.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Context Selection, Forwarding Layer               |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to set Context selection per Forwarding Layer Type.
    The cint Creates a context and map presel line to that context

 ~ File Reference:
    cint_field_presel_fwd_layer.c

  ~ Name:
+-----------------------------------------------------------+
|  PMF - Range Set. Presel qual VportCheck                  |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure a range for bcmFieldQualifyVPortRangeCheck presel qual.
 ~ File Reference:
    cint_field_range_presel.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Range Set. FG qual (L4 Src/Dest ports/Header size)|
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to set a Range in L4 Dest and Src Port/Packet Header Size.
    and use it as a qualifier.
 ~ File Reference:
    cint_field_range_qual.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Snoop Action                                     |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how PMF use should use bcmFieldActionSnoop

 ~ File Reference:
    cint_field_snoop.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - State Table Field Group                          |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Stage Table (bcmFieldGroupTypeStateTable) Field group

 ~ File Reference:
    cint_field_state_table.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Statistical Sampling Action                      |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how PMF use should use bcmFieldActionStatSampling

 ~ File Reference:
    cint_field_stat_sampling.c

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
    cint_field_sys_headers.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Direct TCAM Table Field Group                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Direct TCAM Table (bcmFieldGroupTypeDirectTcam) Field group

 ~ File Reference:
    cint_field_tcam_dt.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Entry Hit APis                                   |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows the usage of the entry_hit APis.

 ~ File Reference:
    cint_field_entry_hit.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Direct TCAM Table Multi Test                     |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure 2 Direct TCAM Table FGs on same bank.

 ~ File Reference:
    cint_field_dt_multi.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Direct TCAM Table Multi Entries Test             |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure multi entries on Direct TCAM Table FG on same bank.

 ~ File Reference:
    cint_field_dt_multi_entries.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - TCAM FG on IPMF3 stage                              |
+-----------------------------------------------------------+
 ~ Description:
    Basic example for TCAM on iPMF3 stage.

 ~ File Reference:
    cint_field_tcam_ipmf3.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Action Prefix                                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows an example of using an action prefix, that is
    adding a constant to the MSB of a user defined action.

 ~ File Reference:
    cint_field_tcam_prefix.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - No system hdr usage and adding trap code            |
+-----------------------------------------------------------+
 ~ Description:
    This CINT creates a new context and configures it to not use any system header,
    then it adds a preselector of the given trap_code/trap_mask for the new context.
    The preselector presel_id is also given.

 ~ File Reference:
    cint_field_trap_code_no_sys_hdr.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - UDH on IPMF1 and EPMF stages                        |
+-----------------------------------------------------------+
 ~ Description:
    This cint configures a FG that sets UDH1 type to type given then sets the data for
    UDH1 to the data given.

 ~ File Reference:
    cint_field_udh_ipmf1_epmf.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - UDH on IPMF1 and IPMF3 stages                       |
+-----------------------------------------------------------+
 ~ Description:
    This cint configures a FG that sets UDH1 type to type given then sets the data for
    UDH1 to the data given.

 ~ File Reference:
    cint_field_udh_ipmf1_ipmf3.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Cint Utilities                                      |
+-----------------------------------------------------------+
 ~ Description:
    CINT utilities to avoid the code repeating.

 ~ File Reference:
    cint_field_utils.c

  ~ Name:
+-----------------------------------------------------------+
|  PMF - key_gen_var feature. DE usage.                     |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to create constant values per context and set it as qualifier.
    For DE FG.
 ~ File Reference:
    cint_field_key_gen_var_de.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Constant Field Group                             |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Constant (bcmFieldGroupTypeConst) Field group

 ~ File Reference:
    cint_field_group_const.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - InLIF profile                                    |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use InLIF profile as a qualfier

 ~ File Reference:
    cint_field_inlif_profile.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - DstGport qualifier                               |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use bcmFieldQualifyDstGport as a qualfier

 ~ File Reference:
    cint_field_DstGport_ipmf3_qual.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Port profile                                     |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use configure port profile
      and use it as a qualfier.

 ~ File Reference:
    cint_field_port_profile.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Port general data                                |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use configure port general data
      and use it as a qualfier.

 ~ File Reference:
    cint_field_port_general_data.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - UDH Cascading                                    |
+-----------------------------------------------------------+
 ~ Description:
    Cint Example of using PMF for UDH cascading.

 ~ File Reference:
    cint_field_udh_cascading.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF -   VSI Profile                                    |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use VSI profile as qualifier

 ~ File Reference:
    cint_field_vsi_profile.c


 ~ Name:
+-----------------------------------------------------------+
|    PMF -   APP Type                                    |
+-----------------------------------------------------------+
 ~ Description:
      Cint shows how to use APP type as qualifier

 ~ File Reference:
    cint_field_app_type.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF WA - Workaround for FEC to set Destination trap    |
+-----------------------------------------------------------+
 ~ Description:
    Provides an example of building setup for WA that configure
    Direct extraction to Destination Action = Destination Qualifier

 ~ File Reference:
    cint_field_fec_dest_wa.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF Qualifier - PP port                                |
+-----------------------------------------------------------+
 ~ Description:
     Cint that show hot to qualify upon PP port

 ~ File Reference:
    cint_field_in_port_qual.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF WA - Workaround OAM trap FWD Layer                  |
+-----------------------------------------------------------+
 ~ Description:
    The cint show how to implement WA for HW but which does not increment
    forwarding layers for OAM trapped packets as required by Arcg

 ~ File Reference:
    cint_field_oam_wa_layer_index.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF WA - TCP/UDP trap    |
+-----------------------------------------------------------+
 ~ Description:
     The cint show how to implement Workaround for TCP UDP traps
     that might have False Positive indication

 ~ File Reference:
    cint_field_trap_wa_tcp_udp.c


  ~ Name:
+-----------------------------------------------------------+
|    PMF - State Table address prefix usage                 |
+-----------------------------------------------------------+
 ~ Description:
     This is an example how we can split the State Table memory between different State Table FGs
      by using constant values as prefixes to define a unique range per State Table FG.

 ~ File Reference:
    cint_field_st_multi.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Action prefix                                     |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to use action prefix in bcm_field_action_create() API.

 ~ File Reference:
    cint_field_action_prefix.c

  ~ Name:
+-----------------------------------------------------------+
| PMF - Cascading IPMF1 to IPMF2 stage                      |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure IPMF1-IPMF2 Cascading field groups,
    while using result buffer from the IPMF1 as qualifier in the IPMF2.

 ~ File Reference:
    cint_field_cascading.c

  ~ Name:
+-----------------------------------------------------------+
|   ECMP unicast RPF                                        |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure unicast RPF when the routing
    points to an ECMP. The interfaces that are members of the
    ECMP must be configured in iPMF3.

 ~ File Reference:
    cint_field_ecmp_rpf.c

  ~ Name:
+-----------------------------------------------------------+
|   ECMP unicast and multicast RPF                          |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure unicast and multicast RPF when the routing
    points to an ECMP group. It explains how to pass and how to fail the RPF check.

 ~ File Reference:
    cint_ip_ecmp_rpf_examples.c

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
    cint_field_fem.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF Side of IPT instumentation Appliction              |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to configure Ingress PMF's for IPT instrumentation Application

 ~ File Reference:
    cint_field_hash_ipt.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF - Attach API: input_type_const                      |
+-----------------------------------------------------------+
 ~ Description:
    Cint show how to to use bcmFieldInputTypeConst in bcm_field_group_context_attach()

 ~ File Reference:
    cint_field_input_type_const.c

  ~ Name:
+-----------------------------------------------------------+
|   PMF layer records                                       |
+-----------------------------------------------------------+
 ~ Description:
    Cint shows how to configure qualifiers for field group for
    different types of layer records.

 ~ File Reference:
    cint_field_layer_record.c

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
    cint_dnx_srv6_tunnel_full_flow_example.c
    internal/cint_srv6_basic.c

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
    cint_field_vlan_format.c


  ~ Name:
+-----------------------------------------------------------+
|    PMF - Direct Extraction Simple configuration           |
+-----------------------------------------------------------+
 ~ Description:
      Show how to configure PMF with Direct Extraction rule

 ~ File Reference:
    cint_field_basic_de.c

  ~ Name:
+-----------------------------------------------------------+
|    PMF - Exhaustive test for hash                         |
+-----------------------------------------------------------+
 ~ Description:
      Reserves all additional keys and then try to add another FG. Expect to fail with NO_RESAURCE.

 ~ File Reference:
    cint_field_hash_exhaustive.c

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
    cint_oam_basic.c

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
    cint_oam_action.c

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
    cint_crps_erpp_config.c
    cint_crps_etm_config.c
    cint_crps_etpp_config.c
    cint_crps_irpp_config.c
    cint_crps_itm_config.c
    cint_crps_oam_config.c

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
    cint_crps_db_utils.c

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
    cint_stif_billing_ingress_mode.c
    cint_stif_billing_mode.c
    cint_stif_qsize_mode.c

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
    cint_policer_generic_meter.c

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
    cint_policer_global_meter.c

~ Name:
+--------------------------------------------------------+
|    Policer resolve tables configuration   |
+--------------------------------------------------------+
 ~ Description:
    An example for resolve tables configuration.
    - primary resolution configuration in parallel/serial mode
    - modify primary/final resolution

 ~ File Reference:
    cint_policer_resolution_config.c

~ Name:
+-----------------------------------------+
|    Credit request profiles   |
+-----------------------------------------+
 ~ Description:
    Used to enable credit request profiles backward compatibility.
    - Legacy devices had fixed set of profiles. This cint reference creates the same fixed set of profiles.
 ~ File Reference:
    cint_credit_request_profile.c

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
    cint_ptp_1588.c

~ Name:
+-------------------+
|    EVPN Field     |
+-------------------+
 ~ Description:
    An example of EVPN Field application setup
    - Create ingress and egress objects
    - Add ESI encapsulation and termination

 ~ File Reference:
    - cint_field_evpn_esi.c

~ Name:
+-------------------+
|    External TCAM  |
+-------------------+
 ~ Description:
    An example of External TCAM Field application setup

 ~ File Reference:
    - cint_field_external.c

~ Name:
+----------------------------+
|    External TCAM with PMF  |
+----------------------------+
 ~ Description:
    An example of External TCAM Field application setup, including configuring the PMF
    to utilize the External TCAM's result.

 ~ File Reference:
    - cint_field_external_with_pmf.c

~ Name:
+-----------------------------------------+
|    Dynamic port add and port remove     |
+-----------------------------------------+
 ~ Description:
    Examples for dynamically adding and removing ports.
 ~ File Reference:
    cint_dynamic_port_add_remove.c

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
    cint_vpls_pwe_tagged_mode_split_horizon.c

 ~ Name:
+-----------------------------------------------+
|    MPLS with Hierarchical TM flow (HTM) FEC   |
+-----------------------------------------------+
 ~ Description:
    Create 3 level FEC entries with HTM used by MPLS tunnels.

 ~ File Reference:
    cint_mpls_fec_htm.c

 ~ Name:
+----------------------------------+
|    MPLS PHP and SWAP from FHEI   |
+----------------------------------+
 ~ Description:
    Configure MPLS PHP and SWAP actions from FHEI.

 ~ File Reference:
    cint_mpls_operations_without_eedb.c

 ~ Name:
+----------------------------+
|    MPLS tandem over VPLS   |
+----------------------------+
 ~ Description:
    Configure MPLS and tandem MPLS EEDB entries to be encapsulated in same encap stage.

 ~ File Reference:
    cint_mpls_tandem.c

 ~ Name:
+----------------------------+
|    MPLS upstream assined   |
+----------------------------+
 ~ Description:
    Configure MPLS termination and forwarding based on 2 adjacent labels used as a single lookup key.

 ~ File Reference:
    cint_mpls_upstream_assigned.c

 ~ Name:
+-------------------+
|    MPLS utility   |
+-------------------+
 ~ Description:
    General MPLS utility used to create MPLS setups.

 ~ File Reference:
    cint_dnx_utility_mpls.c

 ~ Name:
+-------------------+
|    VPLS utility   |
+-------------------+
 ~ Description:
    General VPLS utility used to create VPLS setups.

 ~ File Reference:
    cint_dnx_utils_vpls.c

 ~ Name:
+---------------------------+
|    latency MSR            |
+---------------------------+
 ~ Description:
    General latency measurements example.

 ~ File Reference:
    cint_ingress_latency.c
    cint_end_to_end_latency.c

 ~ Name:
+---------------------------+
|    Reflector              |
+---------------------------+
 ~ Description:
    Example for the required configuration for L2 and L3 reflectors.
 ~ File Reference:
    cint_reflector_l2_external.c
    cint_reflector_l2_internal.c
    cint_reflector_l3_internal.c

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
    cint_dnx_coe_basic.c

 ~ Name:
+---------------------------+
|    shared shaper            |
+---------------------------+
 ~ Description:
    Example for creating E2E scheduling scheme with a shared shaper.

 ~ File Reference:
    cint_e2e_shared_shapers.c

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

 ~ Name:
+----------------+
|    Mirror      |
+----------------+
 ~ Description:
    An example of inbound/outbound mirroring. Used in the tests which checking inbound/outbound mirroring functionality

 ~ File Reference:
    cint_mirror_tests.c

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
    cint_stat_pp_port_ingress.c

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
    cint_stat_pp_port_ingress_crps_meter.c

 ~ Name:
+----------------------------------------+
|    Stat PP - stat_pp configurations    |
+----------------------------------------+
 ~ Description:
    Examples and utilities for configuring statistics on fwd, lif APIs

 ~ File Reference:
    cint_stat_pp_config.c

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
    cint_stat_pp_ipv4_route.c

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
    cint_stat_pp_l2_egress_fwd.c

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
    cint_stat_pp_mpls_egress.c

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
    cint_stat_pp_mpls_termination.c

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
    cint_vpls_statistics.c

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
    cint_port_misc_configuration.c

 ~ Name:
+----------------------------------------+
|    MPLS special label                  |
+----------------------------------------+
 ~ Description:
    Provided the MPLS special label usage example.

 ~ File Reference:
    cint_mpls_special_label.c

 ~ Name:
+----------------------------------------+
|    VPLS split horizon filter           |
+----------------------------------------+
 ~ Description:
    Provided the VPLS split horizon filter example.

 ~ File Reference:
    cint_vpls_split_horizon.c


 ~ Name:
+----------------------------------------+
|    VPLS Upstream Assigned              |
+----------------------------------------+
 ~ Description:
    Provides examples for PWE label termination in context of:
     - Previous LIF/RIF
     - Preceding MPLS label value

 ~ File Reference:
    cint_vpls_upstream_assigned.c

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
+-------------------+
|    PORT utility   |
+-------------------+
 ~ Description:
    General cint_dnx_utils_port utility provides port basic functionality.

 ~ File Reference:
    cint_dnx_utils_port.c

 ~ Name: 
+-----------------------------------------------------------+
|    L2 Encapsulation of a Packet Tunneled to External CPU  |
+-----------------------------------------------------------+
 ~ Description:
    An example of L2 encapsulated packet configuration that is tunneled to external CPU.

 ~ File Reference:
    src/examples/dnx/tunnel/cint_dnx_tunnel_l2_encap_external_cpu.c

 ~ Name:
+-------------------+
|    Twamp application   |
+-------------------+
 ~ Description:
    TWAMP TX/RX and Reflector scenarios.

 ~ File Reference:
    cint_dnx_twamp_field.c
    cint_dnx_twamp.c

 ~ Name:
+-------------------+
|   Load Balancing  |
+-------------------+
 ~ Description:
    Example hashing functions for load balancing configurations. 

 ~ File Reference:
    cint_dnx_load_balancing.c

~ Name:
+-------------------------------------------+
|   L2 LIF associated with Unttaged packet  |
+-------------------------------------------+
 ~ Description:
    Example of Logical Interfaces(LIF) associated with port X untagged-packet.

 ~ File Reference:
    cint_vlan_match_untagged_packet.c
