 $Id: README.txt,v 1.99 Broadcom SDK $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$

 ~ Name:
+----------------------------------+
|    General Utility functions     |
+----------------------------------+
 ~ Description:
    General utility functions

 ~ File Reference:
    cint_sand_utils_global.c

 ~ Name:
+--------------------------------+
|    VXLAN Utility functions     |
+--------------------------------+
 ~ Description:
    General utility functions for vxlan applications

 ~ Description:
    Utility functions for VXLAN application

 ~ File Reference:
    cint_sand_utils_vxlan.c

 ~ Name:
+--------------------------------+
|    MPLS Utility functions     |
+--------------------------------+
 ~ Description:
    General utility functions for mpls applications

 ~ Description:
    Utility functions for MPLS application

 ~ File Reference:
    cint_sand_utils_mpls.c

~ Name:
+--------------------------------+
|    VPWS Utility functions     |
+--------------------------------+
 ~ Description:
    General utility functions for vpws applications

 ~ Description:
    Utility functions for VPWS application

 ~ File Reference:
    cint_sand_utils_vpws.c

 ~ Name:
+-------------------+
|    PORT utility   |
+-------------------+
 ~ Description:
    General cint_sand_utils_port utility provides port basic functionality.

 ~ File Reference:
    cint_sand_utils_port.c

~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
    Utility function for MACT configuration.

 ~ File Reference:
    cint_sand_mact.c

~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
    Example of Basic Bridging VLAN membership.
        - Bridge packet from in port to out port.
        - Includes vlan editing.

 ~ File Reference:
    cint_bridge_application.c

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
    cint_unknown_l2uc.c

~ Name:
+-------------------------+
|    Bridge application   |
+-------------------------+
 ~ Description:
     Demonstrates ingress ang egress vlan port membership
        - Bridge packet from in port to out port.
        - Includes Egress VLAN membership filtering on VID changed by EVE

 ~ File Reference:
    cint_vlan_membership.c

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
    cint_l2_basic_bridge_with_vlan_editing.c

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
    cint_sand_l2_multicast.c

 ~ Name:
+--------------------------+
|    IP Unicast Routing    |
+--------------------------+
 ~ Description:
    An example of IPv4 unicast router. Demonstrates how to:
    - Open the router interface
    - Create egress objects
    - Add route and host entries

 ~ File Reference:
    cint_ip_route_basic.c

 ~ Description:
    Utility functions for L3 applications

 ~ File Reference:
    cint_sand_utils_l3.c


 ~ Name:
+-----------------------------------+
|    IP Unicast Routing Priorities  |
+-----------------------------------+
 ~ Description:
    An example of IPv4 unicast router that verifies the prioritisation of the different IP UC tables.
    - Open the router interface
    - Create egress objects
    - Add route and host entries in different tables and with different masks

 ~ File Reference:
    cint_ip_route_fwd_priorities.c

~ Name:
+-----------------------------------+
|    IP Route over AC               |
+-----------------------------------+
 ~ Description:
    An example of IPv4 route over AC that belongs to different VSI's

 ~ File Reference:
    cint_route_over_ac.c

 ~ Name:
+--------------------------------+
|    IP Routing    |   Secenarios
+--------------------------------+
 ~ Description:
    Includes below examples for different test secenarios:
      An example of IPv6 route with different prefix length. Demonstrates how to:
      - Open the router interface
      - Create egress objects
      - Add route with different prefix length
      An example of IPv4 public/private route in KAPs. Demonstrates how to:
      - Open the router interface
      - Create egress objects
      - Add two IPv4 UC routes, one with vrf=0, another with vrf!=0

 ~ File Reference:
    cint_ip_route_scenarios.c
    cint_ip_route_tunnel.c

 ~ Name:
+--------------------+
|    IPv6 Routing    |
+--------------------+
 ~ Description:
    An example of IPv6 unicast and multicast routing.

 ~ File Reference:
    cint_ipv6_fap_basic.c

 ~ Name:
+---------------------------------------------------+
|    IP Routing    |      RPF      |   Secenarios
+---------------------------------------------------+
 ~ Description:
    Includes below examples for different test secenarios:
      An example of a router with Ipv4 MC RPF scenario.Demonstrates how to:
      - Create and set multicast group
      - Open the router interface
      - Create egress objects with MC PRF enable
      - Add route with Ipv4 MC

 ~ File Reference:
    cint_ip_route_rpf_scenarios.c
    cint_ip_route_rpf_basic.c

+--------------------------------------+
|    IP Routing    |      enablers
+--------------------------------------+
 ~ Description:
    verify that the routing enables enable/disable the traffic as expected and that the max NOF routing profiles can be reached
      - creats as many combination of routing enaablers in all the routing profiles modes (SOC properties options)
      - verify that the enabled/disabled routing type behaves as according to the routing profile setting

 ~ File Reference:
    cint_ip_routing_enablers.c

 ~ Name:
+--------------------------------+
|    IP ECMP    |   Secenarios
+--------------------------------+
 ~ Description:
    Includes below examples for different test secenarios:
      An example of ECMP route. Demonstrates how to:
      - Open the router interface
      - Create egress objects(FECs)
      - Create ECMP group and add FECs to it
      - Add route, and set ECMP as the destination.

 ~ File Reference:
    cint_ecmp_scenarios.c
    cint_ecmp_basic.c

 ~ Name:
+--------------------------------+
|    IP ECMP hashing             |
+--------------------------------+
 ~ Description:
    Includes examples for ECMP hashing secenarios.

 ~ File Reference:
    cint_sand_ecmp_hashing.c

 ~ Name:
+--------------------------------+
|    Hierarchical FEC            |
+--------------------------------+
 ~ Description:
    Includes examples for hierarchical fec scenarios.

 ~ File Reference:
    cint_ip_route_hierarchical_fec.c

 ~ Name:
+----------------------------------------------------+
|    IP Tunnel encapsulation     |   Scenarios
+-----------------------------------------------------+
 ~ Description:
    Includes below examples for different test scenarios:
      An example of Ip tunnel encapsulation. Demonstrates how to:
      - Open the router interfaces
      - Create egress objects(FECs)
      - Create Ip egress tunnel object, pointed by the FEC.
      - Add route entry pointing to the FEC.

 ~ File Reference:
    cint_ip_tunnel_encap_basic.c

 ~ Name:
+----------------------------------------------------+
|    IP Tunnel encapsulation     |   Scenarios
+-----------------------------------------------------+
 ~ Description:
    Includes below examples for different test scenarios:
      - Open the router interfaces
      - Create egress objects(FECs/ECMPs)
      - Create Ip egress tunnel object, pointed by the FEC.
      - Add route entry pointing to the FEC.
      - Add MPLS entries for forwarding lookup, pointing to the FEC.

 ~ File Reference:
    cint_ip_tunnel_encapsulation.c
    cint_sand_ipv6_tunnel.c
    cint_sand_ipv6_tunnel_encap.c

 ~ Name:
+----------------------------------------------------+
|    IP Tunnel termination     |   Scenarios
+-----------------------------------------------------+
 ~ Description:
    Includes below examples for different test scenarios:
      An example of Ip tunnel termination. Demonstrates how to:
      - Open the router interfaces
      - Create egress objects(FECs)
      - Create Ip ingress tunnel object, pointed by the FEC.
      - Add route entry pointing to the FEC.

 ~ File Reference:
    cint_ip_tunnel_term_basic.c

 ~ Name:
+----------------------------------------------------+
|    IP Tunnel termination     |   Scenarios
+-----------------------------------------------------+
 ~ Description:
    Includes below examples for different test scenarios:
      - Open the router interfaces
      - Create egress objects(FECs/ECMPs)
      - Create Ip ingress tunnel object.
      - Add route entry pointing to the FEC.
      - Add MPLS entries for forwarding lookup, pointing to the FEC.

 ~ File Reference:
    cint_ip_tunnel_termination.c
    cint_sand_ipv6_tunnel_term.c
    cint_sand_ipv6_vxlan.c

 ~ Name:
+----------------------------------------------------+
|    VXLAN Tunnel encapsulation     |   Scenarios
+-----------------------------------------------------+
 ~ Description:
    Includes below examples for different test scenarios:
      - Open the router interfaces
      - Create egress objects(FECs)
      - Create VXLAN egress tunnel object.
      - Add MACT pointing to the VXLAN gport.

 ~ File Reference:
    cint_vxlan_encapsulation.c

 ~ Name:
+----------------------------------------------------+
|    VXLAN Tunnel termination     |   Scenarios
+-----------------------------------------------------+
 ~ Description:
    Includes below examples for different test scenarios:
      - Open the router interfaces
      - Create egress objects(FECs)
      - Create VXLAN ingress tunnel object.
      - Add MACT for forwarding lookup.

 ~ File Reference:
    cint_vxlan_tunnel_termination_basic.c
    cint_vxlan_general_traffic.c

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
    cint_mpls_encapsulation_basic.c

 ~ Name:
+--------------------------+
|    MPLS   |
+--------------------------+
 ~ Description:
    MPLS multidevice and interiop examples for swap, termination, encapsulation and PHP:

 ~ File Reference:
    cint_sand_mpls_lsr.c

 ~ Name:
+--------------------------+
|    MPLS   |
+--------------------------+
 ~ Description:
    MPLS multidevice and interiop examples for segment routing

 ~ File Reference:
    cint_sand_mpls_segment_routing.c



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
    cint_mpls_termination_basic.c

~ Name:
+----------------------------------+
|  MPLS EVPN                       |
+----------------------------------+
 ~ Description:
    Example of MPLS EVPN.
    Example of JERICHO1 compatible mode EVPN on JERICHO2.

 ~ File Reference:
    cint_sand_multi_device_evpn.c cint_field_evpn.c


~ Name:
+--------------------------+
|    VXLAN   |
+--------------------------+
 ~ Description:
    1 basic scenario for :
    - VXLAN termination
    - VXLAN encapsulation

 ~ File Reference:
    cint_vxlan_basic.c

~ Name:
+--------------------------+
|    VXLAN   |
+--------------------------+
 ~ Description:
    VXLAN utility file

 ~ File Reference:
    cint_sand_utils_vxlan

~ Name:
+-----------------------------------+
|    VPLS   |      PWE tagged mode
+-----------------------------------+
 ~ Description:
    Basic PWE tagged mode scenario: ETH is forwarded to ETH2oPWEoETH and ETH2oPWEoETH
    is terminated and forwarded with ETH.

 ~ File Reference:
    cint_vpls_pwe_tagged_mode_basic.c

~ Name:
+----------------------+
|    VSWITCH   |  VPLS
+----------------------+

 ~ Description:
    Basic vswitch (mc) examples in VPLS network. Unknown flooding, split horizon,
    learning, same-interface filter.

 ~ File Reference:
    cint_sand_vswitch_vpls.c

~ Name:
+----------------------+
|    VSWITCH   |  VPWS
+----------------------+

~Description:
Interiop and multi-devices P2P example of VPWS application

 ~ File Reference:
    cint_sand_vswitch_vpws.c

~ Name:
+----------------------+
|    ROO   |      VPLS
+----------------------+

~Description:
Basic ROO scenario with VPLS: IP is forwarded to IPoETHoPWEoMPLSoETH and IPoETHoPWEoMPLSoETH i terminated
and forwarded with IPoETH.

 ~ File Reference:
    cint_vpls_roo_basic.c

~ Name:
+----------------------+---------+
|    ROO   |      VPLS | Advanced scenarios
+----------------------+---------+

~Description:
    To add advanced scenarios of VPLS ROO
    Presently following scenarios are present :
        1.Tunnel to Native URPF check
        2.VPLS ROO tunnel termination and tunnel encapsulation for IPv6 payload

 ~ File Reference:
    cint_vpls_roo_advanced.c

~ Name:
+----------------------+---------+
|    ROO   |      VPLS | MC
+----------------------+---------+

~Description :
    Cover v4 IPMC for native to tunnel scenario
    covers different types of access and network copies for the ipmc group

~File Reference:
    cint_vpls_roo_mc.c
+--------------------------+
|    IP MC scenarios       |
+--------------------------+

~ Name:
+--------------------------+
|    IP Compatible MC
+--------------------------+

~Description :
    Cover varios IP Compatible MC scenarios

~File Reference:
    cint_sand_ip_compatible_mc.c

~ Name:
+--------------------------+
|    IP MC scenarios       |
+--------------------------+

~Description :
    Cover the basic IP MC scenarios for both IPv4 and IPv6

~File Reference:
    cint_ipmc_route_basic.c

~ Name:
+---------------------------------+
|    IPMC routing scenarios       |
+---------------------------------+

~Description :
    Cover the IP MC routing scenarios for both IPv4 and IPv6

~File Reference:
    cint_ipmc_route_examples.c
    cint_sand_ipmc_flows.c

 ~ Name:
+--------------+
|    L3 VPN    |
+--------------+
 ~ Description:
    An example of L3VPN application.
    In CINT following traffic scenarios:
     1. Traffic from RouterCE1 to RouterP
        purpose: check mpls & vc encapsulations, check new ethernet header
     2. Traffic from RouterP2 to RouterCE1
        purpose: check mpls and vc label termination, check new ethernet header
     3. Traffic from RouterCE2 to RouterCE1
        purpose: check ip routing
     4. Traffic from RouterP2 to RouterP3
        purpose: check swap mpls, check new ethernet header

 ~ File Reference:
     src/examples/sand/cint_sand_l3vpn.c

 ~ Name:
+--------------+
|    L3 MVPN   |
+--------------+
 ~ Description:
    An example of IP-GRE L3VPN which show BCM886XX being PE-based MVPN (multicast IPMC traffic).
    In CINT following traffic scenarios:
     1. Traffic from IP-GRE network P1, to P2, P3, CE1, CE2 (Bud node)
     2. Traffic from IP-GRE network, to PE2 (Continue)
     3. Traffic from IP-GRE network, to CE1, CE2 (Leaf node)
     4. Traffic from access side, to CE2, P1

 ~ File Reference:
     src/examples/sand/cint_sand_mvpn.c

 ~ Name:
+-----------------------------+
|  Source Mac Encapsulation   |
+-----------------------------+
 ~ Description:
        An example of source mac encapsulation.

 ~ File Reference:
     cint_sand_src_mac_encap.c

 ~ Name:
+----------------------------+
|    TCAM and FEC DMA stress |
+----------------------------+
 ~ Description:
    An example of tcam dnand fec dma stress test:

~File Reference:
    cint_field_dma_access.c

~ Name:
+--------------------------+
|    MPLS   |
+--------------------------+
 ~ Description:
    Cover 3 level fec hierarchy, each with protection, 3 labels are encapsulated at the egress.
    Each FEC (6 total - 3 levels, each with protection) points to a different label at the EEDB.
    FECs are cascaded, each level active FEC is determined based on the FECs protection state (which one is active - primary or backup).
    Last mpls pointed to at the EEDB sets next pointer to the ARP.
    Forwarding is done on IP header according to IP header table

 ~ File Reference:
    cint_mpls_3_level_protection.c

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
    src/examples/sand/cint_l2_traverse.c

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
    src/examples/sand/cint_l2_bridge_trunk.c

~ Name:
+--------------------------+
|       FLOW CONTROL       |
+--------------------------+
 ~ Description:
    Covers flow control generation and receive examples

 ~ File Reference:
    cint_flow_control_examples.c

 ~ Name:
+----------------+
|    Mirror 2    |
+----------------+
 ~ Description:
    An example of mirror packet according to port x VLAN, with inbound/outbound mirroring. where the mirror packet:
    - Is SPAN mirrored
    - Includes RSPAN encapsulation
    - Includes ERPSAN encapsulation

 ~ File Reference:
    cint_mirror_erspan.c

+----------------------------+
|     MPLS qos mapping       |
+----------------------------+
~ Description:
  An example of mpls exp mapping
  - create ingress remark qos profile and add exp mapping
  - create egress remark qos profile and add exp mapping
  - set qos profile to mpls tunnel gport

 ~ File Reference:
    cint_sand_mpls_qos.c
    cint_mpls_qos.c

+----------------------------+
|     QoS mapping       |
+----------------------------+
~ Description:
  An example of QOS L2 mapping
  - create ingress remark qos profile and add PHB mapping
  - create egress remark qos profile and EVE for packet remarking

 ~ File Reference:
    cint_qos_l2_phb.c

~ Name:
+----------------------------+
|     MSPW qos mapping       |
+----------------------------+
~ Description:
  An example of cos mapping for MS PWE Transit Node
  - Ingress
    - If Tunnel is uniform
        - map LSP.EXP to TC/DP
    - If Tunnel is pipe
        - map PWE.EXP to TC/DP(ignore PWE mode)
  - Egress
    - PWE header
        - EXP: If uniform, map TC/DP to EXP; if pipe, use user configuration
        - TTL: If uniform, it's 255; if pipe, from MPLS CMD profile
    - LSP header
        - EXP: If uniform, copy from PWE.EXP; if pipe, use user configuration
        - TTL: If uniform, copy form PWE label; if pipe, from MPLS CMD profile

 ~ File Reference:
    cint_qos_mspw.c

 ~ Name:
+----------------+
|    Topology    |
+----------------+
 ~ Description:
	Examples of topology configuration.
	 - Topology for local module
	 - Topology for FAP groups

 ~ File Reference:
	fe/cint_topology.c


 ~ Name:
+---------------------+
|    Termination 1    |
+---------------------+
 ~ Description:
    Example of MyMac termination, where MyMac is set based on VRID.
    The cint demonstrates the different VRID-My-MAC-Modes.

 ~ File Reference:
    cint_l3_vrrp.c
    cint_ip_route.c


 ~ Name:
+---------------------+
|    Termination 2    |
+---------------------+
 ~ Description:
    Example of flexiable multiple MyMac termination.
    Available in BCM88660 only.

 ~ File Reference:
    cint_multiple_mymac_term.c


 ~ Name:
+-----------------+
|    Multicast    |
+-----------------+
 ~ Description:
        The cint demonstrate two examples of multicast applications, direct and indirect.
         - Direct: Set multicast table
         - Indirect: Set multicast table and create a static topology

 ~ File Reference:
        fe/cint_multicast_test.c

 ~ Name:
+-----------------------------------+
|  IP Multicast Routing Priorities  |
+-----------------------------------+
 ~ Description:
        An example of IPv4 multicast router that verifies the prioritisation of the different IPMC tables.
         - Open the router interface
         - Create MC groups
         - Add route and host entries in different tables and with different masks

 ~ File Reference:
        cint_ipmc_route_priorities.c


 ~ Name:
+----------+
|    RX    |
+----------+
 ~ Description:
	This example demonstrates a configuration when using the CPU2CPU packets mechanism.

 ~ File Reference:
	fe/cint_sand_cpu_packets.c

~ Name:
+---------------+
|    SR cell    |
+---------------+
 ~ Description:
    This example demonstrates a configuration when using source routed cells mechanism.

 ~ File Reference:
    fe/cint_sr_cell.c

 ~ Name:
----------------------+
|    MTU Filtering    |
+---------------------+
 ~ Description:
    An example of MTU filtering configuration.

 ~ File Reference:
    src/examples/sand/cint_mtu_filtering.c

 ~ Name:
----------------------+
|    FEC              |
+---------------------+
 ~ Description:
    Includes example of testing fec with max mc group id.

 ~ File Reference:
    src/examples/sand/cint_fec_tests.c

 ~ Name:
+--------------------------------------+
|    E2E Scheduler connection          |
+--------------------------------------+
 ~ Description:
    An example E2E symmetric connection (VoQ <--> VoQconnector)
    Demonstrates how to:
    - Create VoQs
    - Create VoQ connectors
    - Connecect VoQ to VoQ connector
    - Connecect VoQ connector to VoQ

 ~ File Reference:
    src/examples/sand/cint_e2e_symmetric_connection.c

 ~ Name:
+-----------------------+
|    MACT Learning 1    |
+-----------------------+
 ~ Description:
    Example of different learning modes and message types.
    Relevant for PP users.

 ~ File Reference:
    cint_sand_l2_learning.c

 ~ Name:
+-----------------------+
|    MACT Learning 2    |
+-----------------------+
 ~ Description:
    An example of learning. Demonstrate how to control the learning message per vlan, send CPU or to OLP (for automatic learning)

 ~ File Reference:
    cint_sand_l2_vlan_learning.c

 ~ Name:
+------------------+
|    CPU Shadow    |
+------------------+
 ~ Description:
    An example of sending MACT events to CPU.

 ~ File Reference:
    cint_sand_l2_cpu_learning.c

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
    cint_sand_l2_learn_event_parsing.c

 ~ Name:
+--------------------------+
|    Multicast Utilities   |
+--------------------------+
 ~ Description:
    Utility functions for multicast applications.

 ~ File Reference:
    cint_sand_utils_multicast.c
+----------------------+
|    E2E Scheduling    |
+----------------------+
 ~ Description:
    Covers the E2E hierarchy setup. Examples are given for the hookup of different element types.

 ~ File Reference:
    - General: cint_tm_fap_with_id.c
    - SP and Ingress Shapping Validation: cint_tm_fap_config2.c
    - WFQ Validation: cint_e2e_wfq.c
    - Composite SE Validation: cint_e2e_composite_se.c
    - Composite Connector Validation: cint_e2e_composite_connectors.c
    - Interdigitated mode Validateion: cint_e2e_interdigitated.c
    - Dual Shaper Validation: cint_e2e_dual_shaper.c
    - Example of enabling and configuring enhance multicast fabric application - start_enhance_application.c
    - Example of building a FAP scheduling scheme in 8 priorities mode - bcm_tm_fap_cosq.c
    cint_tm_fap.c
    cint_tm_fap_config_large_scheduling_scheme.c
    cint_enhance_application_fap.c
    cint_tm_fap_multi_devices_8_priorities_example.c

 ~ Name:
+--------------+
|    ECN PP    |
+--------------+
 ~ Description:
    An example of ECN marking during forwarding and encapsulation. Demonstrates the following:
    - Enabling ECN on the device
    - Setting extraction of ECN (capable and congestion bits)
    - Configuring congestion for every packet that is sent to out_port.
    - Configuring MPLS encapsulation and egress remark profile mapping of {in-DSCP,DP}, to out-DSCP
    - Testing with traffic and seeing how ECN marking appears in forwarding and encapsulation

 ~ File Reference:
    src/examples/dpp/cint_ecn_example.c

 ~ Name:
+-------------------------+
|  VSQ Global Thresholds  |
+-------------------------+
 ~ Description:
    Global VSQ holds counters on available resources per resource type. This is useful to trigger DP-dependent drop decisions and flow control.

 ~ File Reference:
    cint_vsq_tc_examples.c
    cint_vsq_gl_example.c


 ~ Name:
+-------------------------+
|    VSQ Traffic Class    |
+-------------------------+
 ~ Description:
    The focus of the cint is to show how to enable a tail drop and WRED settings using this property.

 ~ File Reference:
    cint_vsq_tc_examples.c


 ~ Name:
+-------------------------------+
|    VSQ Resource Allocation    |
+-------------------------------+
 ~ Description:
    An example of how to configure global and per VSQ resource allocation.

 ~ File Reference:
    cint_vsq_resource_allocation.c

 ~ Name:
+------------------+
|    L2            |
+------------------+
 ~ Description:
    Utility functions for L2 applications..

 ~ File Reference:
    cint_sand_utils_l2.c

 ~ Name:
+--------------------------+
|    VLAN                  |
+--------------------------+
 ~ Description:
    Utility functions for VLAN applications.

 ~ File Reference:
    cint_sand_utils_vlan.c

 ~ Name:
+--------------------------+
|    OAM basic             |
+--------------------------+
 ~ Description:
    Basic OAM examples.

 ~ File Reference:
    cint_sand_oam.c

~ Name:
+--------------------------+
|    OAM utility           |
+--------------------------+
 ~ Description:
    General
    General utility functions for OAM

 ~ File Reference:
    cint_sand_utils_oam.c

~ Name:
+--------------------------+
|    OAM                   |
+--------------------------+
 ~ Description:
    Utility functions for creating accelerated OAM endpoints, type MPLS-TP and PWE.

 ~ File Reference:
    cint_sand_oam_y1731_over_tunnel.c

 ~ Name:
+--------------------------+
|    BFD                   |
+--------------------------+
  ~ Description:
    Utility functions for creating accelerated BFD endpoints, type IPv4.

 ~ File Reference:
    cint_sand_bfd.c


+--------------------+
|    Metro Bridge    |
+--------------------+
 ~ Description:
    This application enables you to define an L2VPN service.
    In VPN, you can create a Virtual Switch Instance (VSI/VPN) and connect logical ports to it.

 ~ File Reference:
    - P2P Service: cint_vswitch_metro_p2p.c
                   cint_vswitch_metro_mp_vlan_port.c  --- Simple p2p Service with AC to AC case.
    - Multipoint Metro Service:
            cint_vswitch_metro_mp_vlan_port.c  --- Simple multipoint Service.
            cint_vswitch_metro_mp.c - Supports multi device. Please note that some function signatures might have changed.
    - Multipoint VPLS Service:
	        cint_vswitch_metro_simple.c - Simple example of Metro scheme
	- Multipoint VPLS Service:
	        cint_vswitch_vpls.c - Simple example of Metro scheme: cint_vswitch_metro_simple.c

~ Name:
+-------------------------------------+
|    IPV4 Tunnel Encapsulation        |
+-------------------------------------+
 ~ Description:
    An example of IPv4  packet forwarding into IPv4 tunnel with/without GRE encapsulation.

 ~ File Reference:
    cint_sand_ip_tunnel.c
    cint_sand_ip_tunnel_encap.c

~ Name:
+-------------------------------------+
|    IPV4 Tunnel Termination          |
+-------------------------------------+
 ~ Description:
    An example of IPv4 tunnel termination and forwarding according to internal IPv4 header.

 ~ File Reference:
    cint_sand_ip_tunnel.c
    cint_sand_ip_tunnel_term.c

 ~ Name:
+-------------+
|    VPLS     |
+-------------+
 ~ Description:
    Basic VPLS incapsulation and termination scenario.

 ~ File Reference:
    cint_vpls_mp_basic.c

 ~ Name:
+-------------+
|    VxLAN    |
+-------------+
 ~ Description:
    An example of VXLAN application.

 ~ File Reference:
    cint_sand_ip_tunnel.c
    cint_sand_vxlan.c

+-----------------+
|    VxLAN ROO    |
+-----------------+
 ~ Description:
    An example of VXLAN ROO application.

 ~ File Reference:
    cint_sand_ip_tunnel.c
    cint_sand_vxlan.c
    cint_sand_vxlan_roo.c


+-------------------+
|    VxLAN ROO MC   |
+-------------------+
 ~ Description:
    An example of VXLAN ROO MC application.

 ~ File Reference:
    cint_sand_ip_tunnel.c
    cint_sand_vxlan.c
    cint_sand_vxlan_roo_mc.c

 ~ Name:
+-------------------------------------------+
|    Egress Acceptable Frame Type Filter    |
+-------------------------------------------+
 ~ Description:
    An example of egress acceptable frame type filter.

 ~ File Reference:
    cint_sand_egress_acc_frame_type.c

 ~ Name:
+-------------------------------------------+
|   Various qos and ttl mode of mpls tunnel |
+-------------------------------------------+
 ~ Description:
    An example of qos and ttl model of mpls tunnel.

 ~ File Reference:
    cint_sand_mpls_various_scenarios.c

 ~ Name:
+-------------------------------------------+
|    port mirror application    |
+-------------------------------------------+
 ~ Description:
    An example of port mirror application.

 ~ File Reference:
    cint_mirror_tests.c

 ~ Name:
+--------------------+
|    Mesh System     |
+--------------------+
 ~ Description:
    An example of configuration in a mesh system.
     - Setting modid groups
     - Configuring link topology
     - Configuring multicast replication (in Mesh MC)

 ~ File Reference:
    src/examples/sand/cint_fabric_mesh.c

~ Name:
+--------------------------+
|    MPLS  Tunnel  |
+--------------------------+
 ~ Description:
    Example of different paths using the mpls tunnel initator.

 ~ File Reference:
    cint_mpls_tunnel_initiator.c

 ~ Name:
+-------------------------------------------------------+
|    IP Unicast Routing with Egress Vlan Translation    |
+-------------------------------------------------------+
 ~ Description:
    An example of IPv4 unicast router with Egress Vlan Translation. Demonstrates how to:
    - Open the router interface
    - Create egress objects for performing Egress Vlan translation at ENCAP stage
    - Add route and host entries

 ~ File Reference:
    cint_ip_route_basic_encap_eve.c

 ~ Name:
+------------------------+
|    Vlan Translation    |
+------------------------+
 ~ Description:
	Demonstrates IVE (Ingress VLAN Editing) action when
	outer_tpid_action/inner_tpid_action is set to bcmVlanTpidActionModify.

 ~ File Reference:
	cint_vlan_translate_tpid_modify.c

 ~ Name:
+------------------------+
|    Vlan Compression    |
+------------------------+
 ~ Description:
       Demonstrates example to add/delete Vlan Range used for Vlan Compression.

 ~ File Reference:
       cint_port_vlan_range.c

~ Name:
+---------------------+
|    Cross Connect    |
+---------------------+
 ~ Description:
	Demonstrates Point to Point (P2P) cross connect of AC/PWE to AC/PWE

 ~ File Reference:
	cint_vswitch_cross_connect_p2p_ac_pwe.c
	cint_vswitch_cross_connect_p2p_directional.c
     - used for directional cross connect PWE to AC

~ Name:
+---------------------+
|    Split Horizon    |
+---------------------+
 ~ Description:
	Demonstrates Split Horizon trapping.

 ~ File Reference:
	cint_vswitch_split_horizon.c

~ Name:
+------------------------------+
|    PWE Utility functions     |
+------------------------------+
 ~ Description:
    Utility functions for PWE application

 ~ File Reference:
	cint_sand_utils_pwe.c

~ Name:
+-------------------------------------------+
|    Switch/Port TPID Utility functions     |
+-------------------------------------------+
 ~ Description:
    Utility functions for Switch/Port TPID

 ~ File Reference:
	cint_sand_utils_tpid.c

~ Name:
+-------------------------------------------+
|    Vlan Translation Utility functions     |
+-------------------------------------------+
 ~ Description:
    Utility functions for Vlan Translation

 ~ File Reference:
	cint_sand_utils_vlan_translate.c

~ Name:
+-------------------------------------------+
|    Vlan Translation Utility functions     |
+-------------------------------------------+
 ~ Description:
    Utility functions for Egerss Native Vlan Translation

 ~ File Reference:
	cint_native_egress_vlan_editing_lif_vsi_lookup.c

~ Name:
+-------------------------------------------+
|    Packet length Adjust functions         |
+-------------------------------------------+
 ~ Description:
    Packet lenght adjust for ingress and egress

 ~ File Reference:
        cint_sand_compensation_setting.c

~ Name:
+-------------------------------------------+
|    NIF PHY Loopback functions             |
+-------------------------------------------+
 ~ Description:
    Set/Clear NIF PHY loopback

 ~ File Reference:
        cint_sand_nif_phy_loopback.c

~ Name:
+-------------------------------------------+
|    TDM configuration examples             |
+-------------------------------------------+
 ~ Description:
    Examples for configuration TDM on single and multi devices

 ~ File Reference:
        cint_sand_tdm_examples.c

~ Name:
+-------------------------------------------+
|    PP Utility functions                   |
+-------------------------------------------+
 ~ Description:
    Utility functions for PP

 ~ File Reference:
        cint_sand_pp_util.c

~ Name:
+-------------------------------------------+
|          Facility FEC Protection          |
+-------------------------------------------+
 ~ Description:
    The CINT provides two protection examples:
    -Facility FEC (The Super-FEC Protection Pointer is -2)
    -Facility FEC with simultaneous mode (Facility & The Super-FEC Protection Pointer has a vaild Path pointer)

 ~ File Reference:
        cint_ac_fec_facility_protection.c

 ~ Name:
+------------------------------------+                                                        
|    Hierarchical Protection VPLS    |                                                         
+------------------------------------+                                                         
 ~ Description:
    An example of VPLS hierarchical protection. 

 ~ File Reference:
    cint_vswitch_vpls_hvpls.c

~ Name:
+--------------------------------------------------+
|          L3 route in JR compatible mode          |
+--------------------------------------------------+
 ~ Description:
    The CINT provides L3 route servcie examples for JR compatible mode

 ~ File Reference:
        cint_sand_multi_device_ipv4_uc.c

~ Name:
+--------------------------------------------------------+
|          LSR using EEI  in JR compatible mode          |
+--------------------------------------------------------+
 ~ Description:
    The CINT provides LSR servcie using EEI examples for JR compatible mode

 ~ File Reference:
        cint_sand_multi_device_mpls_lsr.c


~ Name:
+--------------------------------------------------------+
|          VXLAN ROO UC   in JR compatible mode          |
+--------------------------------------------------------+
 ~ Description:
    The CINT provides VXLAN ROO UC  examples for JR compatible mode

 ~ File Reference:
        cint_sand_multi_device_vxlan_roo.c

~ Name:
+--------------------------------------------------------+
|          VXLAN ROO MC   in JR compatible mode          |
+--------------------------------------------------------+
 ~ Description:
    The CINT provides VXLAN ROO MC  examples for JR compatible mode

 ~ File Reference:
        cint_sand_multi_device_vxlan_roo_mc.c

~ Name:
+--------------------------------------------------------+
|          IPMC   in JR compatible mode          |
+--------------------------------------------------------+
 ~ Description:
    The CINT provides IPV4 MC examples for JR compatible mode

 ~ File Reference:
        cint_sand_multi_device_ipmc.c

~ Name:
+--------------------------------------------------------+
|         Ingress PUSH with EEI                          |
+--------------------------------------------------------+
 ~ Description:
    The CINT provides Ingress PUSH MPLS label using EEI example.

 ~ File Reference:
        cint_sand_mpls_ingress_push.c

~ Name:
+--------------------------------------------------------+
|        LSR variable scenario                           |
+--------------------------------------------------------+
 ~ Description:
    The CINT provides LSR variable scenario example

 ~ File Reference:
        cint_sand_mpls_check_ver.c

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
        cint_ac_1to1_coupled_protection.c

+-----------------------+
|    Link Monitoring    |
+-----------------------+
 ~ Description:
    Example application to monitor the state of the links and to react to ports/links failure.

 ~ File Reference:
    cint_linkscan.c

~ Name:
+--------------------------+
|       QOS                |
+--------------------------+
 ~ Description:
    QOS examples for basic route an l3 remark:

 ~ File Reference:
  	cint_ip_route_basic_qos.c
  	cint_qos_l3_remark.c

~ Name:
+----------------------------------------+
|    service using EEI                   |
+----------------------------------------+
 ~ Description:
    Provided the service example using EEI under compatible mode

 ~ File Reference:
    cint_sand_service_using_eei.c


~ Name:
+--------------------------+
|       DBAL                |
+--------------------------+
 ~ Description:
	DBAL examples for dbal cint exported APIs:

 ~ File Reference:
 	cint_dbal.c

 ~ Name: 
+-----------------------------------------------------------+
|    L2 Encapsulation of a Packet Punted to External CPU    |
+-----------------------------------------------------------+
 ~ Description:
    An example of L2 encapsulated punt packet configuration.  

 ~ File Reference:
    src/examples/dpp/cint_l2_encap_external_cpu.c

~ Name:
+--------------------------+
|       STAT_PP            |
+--------------------------+
 ~ Description:
    Examples for statistics counting on numerous pp objects (lif, forwarding objects etc):

 ~ File Reference:
    cint_stat_pp_config.c
    cint_stat_pp_ipv4_route.c
    cint_stat_pp_l2_egress_fwd.c
    cint_stat_pp_mpls_egress.c
    cint_vpls_statistics.c
    cint_mpls_ecmp_basic.c

~ Name:
+--------------------------+
|       TRUNK_HQOS         |
+--------------------------+
 ~ Description:
    An exampe of how to create a system with virtual LAGs (aka trunk hqos)

 ~ File Reference:
    cint_sand_trunk_hqos.c

~ Name:
+------------------------------------+
| Multicast application for TM users |
+------------------------------------+
 ~ Description:
    Example of creating a onfiguration enables creating two types of multicast groups:
        - Port level: In port-level scheduled MC, the configuration results in a fully scheduled MC scheme.Its drawback is that it consumes the most resources.
        - Device level: In the Device-level scheduled MC, configuration results in a partially scheduled MC scheme, but it consumes relatively few resources.

 ~ File Reference:
    cint_multicast_applications.c 
