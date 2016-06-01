 $Id: README.txt,v 1.99 Broadcom SDK $
 $Copyright: Copyright 2016 Broadcom Corporation.
 This program is the proprietary software of Broadcom Corporation
 and/or its licensors, and may only be used, duplicated, modified
 or distributed pursuant to the terms and conditions of a separate,
 written license agreement executed between you and Broadcom
 (an "Authorized License").  Except as set forth in an Authorized
 License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and
 Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE
 NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 ALL USE OF THE SOFTWARE.  
  
 Except as expressly set forth in the Authorized License,
  
 1.     This program, including its structure, sequence and organization,
 constitutes the valuable trade secrets of Broadcom, and you shall use
 all reasonable efforts to protect the confidentiality thereof,
 and to use this information only in connection with your use of
 Broadcom integrated circuit products.
  
 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 
 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
                      
+------------------------+                                                     
|     Link Monitoring    |                                                          
+------------------------+                                                        
 ~ Description:                                                                
    Example application to monitor the state of the links and to             
    react to ports/links failures.                                            
                                                                        
 ~ File Reference:                                                              
    cint_linkscan.c                                                           
                                                                              
+------------------------+                                                        
|    Snake Application   |                                                         
+------------------------+                                                       
 ~ Description:                                                                 
    Setup that enables running 80G traffic, while only injecting              
    10G from the external traffic generator.                                  
                                                                              
 ~ File Reference:                                                              
    cint_snake.c                                                             

+------------------------------------------------+                                                        
|    TM Application - Priority Over Port (Ardon) |                                                         
+------------------------------------------------+                                                       
 ~ Description:                                                                 
    Provide an example of building scheduling scheme for "Priority Over Port" with 8 priorities.
    This cint directed to system with reduced resources (Ardon).    
    Also includes examples for: adding ports, MC, ISQ, CGM and other configurations.                                   
                                                                              
 ~ File Reference:                                                              
    cint_tm_prio_o_port_reduce_resources.c    
 
+------------------------+                                                        
|    Multicast Options   |                                                         
+------------------------+                                                         
 ~ Description:  
    Broadcom devices support a variety of multicast options, with
    different schedulting options. 
                  
 ~ File Reference:
    cint_multicast_applications.c                  
                                     
Port-Level Scheduled MC 
-----------------------
 ~ Description: 
    Configuration results in a fully scheduled MC scheme. 
    Its drawback is that it consumes the most resources.

 ~ File Reference:                                                              
    cint_multicast_application_port_lvl_example.c
    cint_multicast_application_port_lvl_example_multi_device.c          
    
Device-Level Scheduled MC
-------------------------  
 ~ Description:
    Configuration results in a partially scheduled MC scheme. 
    It consumes relatively few resources. It is a viable 
    compromise between port-level scheduled MC and Fabric MC.    
    
 ~ File Reference:
    cint_multicast_application_device_lvl_example.c              
    cint_multicast_application_device_lvl_example_multi_device.c 
    
Fabric MC
---------    
 ~ Description:
    Configuration results in unscheduled MC scheme. This 
    configuration provides no end-to-end scheduling by the Credit 
    Scheduler. The Ingress Scheduling is based on the ingress queue.

 ~ File Reference:
    cint_fabric_mc_credit_elements.c          
    cint_enhance_application_fap.c        

+------------------------+       
| Arad Special Multicast |    
+------------------------+
 ~ Description:
    Example of port level multicast configuration. 
 
 ~ File Reference:
    cint_multicast_special_egress_mode.c   
    
+------------------------+     
|          CosQ          |    
+------------------------+ 
 ~ Description:                                                             
    Examples of traffic-management applications with the following
    configurations:                                               
  
Egress Transmit
---------------
 ~ Description:
    Covers simple Egress transmit scheduling setup and the setup of 
    drop thresholds.  

 ~ File Reference:
    cint_egress_transmit_scheduler.c           
    cint_drop_threshold.c      
    
E2E Scheduling
--------------
 ~ Description:
    Covers the E2E hierarchy setup. Examples are given for the hookup 
    of different element types.            

 ~ File Reference:
    General: cint_tm_fap_with_id.c        
    SP and Ingress Shapping Validation: cint_tm_fap_config2.c
    WFQ Validation: cint_e2e_wfq.c   
    Composite SE Validation: cint_e2e_composite_se.c
    Composite Connector Validation: cint_e2e_composite_connectors.c  
    Dual Shaper Validation: cint_e2e_dual_shaper.c
    cint_tm_fap.c
    cint_tm_fap_config_large_scheduling_scheme.c

Flow Control:
-------------
 ~ Description:
    Covers configurations for different FC options.
    Each function configure a specific option (such as PFC Reception).

 ~ File Reference:
    Calendar interfaces: cint_fc_arad.c 
    Reception examples: cint_fc_rx.c
    Generation examples: cint_fc_tx.c
    OOB examples: cint_arad_fc_oob_config_example.c
                  cint_jer_fc_oob_config_example.c
    Inband examples: cint_arad_fc_inbnd_config_example.c
                     cint_jer_fc_ilkn_inb_config_example.c
                     cint_jer_fc_trigger_config_example.c
    PFC Reception mapping examples: cint_arad_pfc_reception_mapping.c
    COE examples: cint_jer_fc_coe_config_example.c

Ingress Queue Manager:
---------------------
 ~ Description:
    Covers configuration including setting queue maximum size, 
    WRED application, and Incoming TC mapping. 
    
 ~ File Reference: 
    cint_itm_examples.c

VOQ Attributes:
--------------
~ Description:
    Various functions for testing Arad VOQ attributes:

~ File Reference:
    cint_queue_tests.c

SYSTEM RED:
-----------
~ Description:  
    Example of setting System RED configuration.  
                  
~ File Reference:
    cint_system_red.c


Unicast Flow-based Forwarding
------------------------------------

~Description:
    Unicast Flow-based Forwarding includes TM hierarchy scheduling example 
    and PP application that associate destination to TM-flow id.

~File Refernce:
    cint_pp_config2.c, cint_tm_fap_config2.c



+------------------------+     
|  VSQ Applications   |    
+------------------------+ 
 ~ Description:
    Examples for VSQ applications including: 
    
Global Thresholds:
------------------
 ~ Description:
    Global VSQ holds counters on available resources per resource 
    type. This is useful to trigger DP-dependent drop decisions and 
    flow control.   
    
 ~ File Reference:
    cint_vsq_example.c    
    cint_vsq_tc_examples.c
    cint_vsq_gl_example.c    

VSQ Traffic Class:
------------------
 ~ Description:
    The focus of the CINT is to show how to enable a tail 
    drop and WRED settings using this property.  
    
 ~ File Reference:
    cint_vsq_tc_examples.c

Flow control:
--------------

~ Description:
    Example of how to configure flow control for specific VSQs (LLFC and PFCl) 

~ File Reference:
    cint_vsq_fc_examples.c

VSQ Resource Allocation:
------------------------
 ~ Description:
    Example of how to configure Global and per VSQ resource allocation.

 ~ File Reference:
    cint_vsq_resource_allocation.c


+-------------------------------+ 
|  OTN/TDM Applications   |
+-------------------------------+ 
 ~ Description:
    Examples of OTN/TDM applications including TDM standard ,TDM optimize, and TDM packet mode.
    Packet mode is often used when expected TDM traffic consist of packet sizes that are higher than 256.
    
 ~ File Reference
    cint_tdm_example.c 
    cint_tdm_packet_mode_example.c
+------------------------+ 
|            Bridge         |
+------------------------+    
 ~ Description:
    Enable activating the Arad in various modes: IPv4/6 Bridge Router, 
    VPLS LER, MPLS LSR, and others. Relevant to PP users. 
    
Traditional Bridge:
-------------------
 ~ Description:
    After the boot up, the device acts as a traditional bridge. 
    By this setting, you can switch packets from one port to another by 
    means of the Ethernet header. This also enables users to define VLANs.
    
 ~ File Reference:
    Vlan bridge: cint_vlan_basic_bridge.c
    Mact Management: cint_l2_mact.c      
    L2 Cache: cint_l2_cache.c     
    Port TPID: cint_port_tpid.c    
    Spanning-Tree Groups: cint_stg.c        
    VLAN Port gport membership: cint_vlan_gport_add.c
    utility function for MACT configuration: cint_mact.c
    
    
Metro Bridge:
-------------
 ~ Description:
    This application enables you to define an L2VPN service. In VPN, 
    you can create a Virtual Switch Instance (VSI/VPN) and connect 
    logical ports to it.       
    
 ~ File Reference:
    P2P Service: cint_vswitch_metro_p2p.c                                 
        cint_vswitch_metro_p2p_multi_device.c             
    VPLS QOS P2P Service: Example of Settings QOS in VPLS application
        cint_qos_vpls.c
    Multipoint Metro Service: 
        cint_vswitch_metro_mp.c - Supports multi device. Please note that some function signatures might have changed.
    Multipoint VPLS Service: cint_vswitch_vpls.c                               
    Simple example of Metro scheme: cint_vswitch_metro_simple.c
    

Bridge Flooding VLAN setting:
-------------
 ~ Description:
    Examples for bridge(VLAN/VSI/VPN) flooding VLAN setting.
    Demonstrate how to set unknow unicast group, unknow multicast group 
    and broadcast group of a Virtual Switch Instance (VLAN/VSI/VPN) separately.

 ~ File Reference:
    cint_vlan_control_config.c

Bridge Router:
--------------
 ~ Description:
    This application enables you to define L3 routers. You can also 
    define IP Unicast Routing, IP Multicast Routing, and MPLS LSR.           
    
 ~ File Reference:
    L3 Routing Examples: cint_ipv4_fap.c                                             
        cint_ip_route_tunnel.c
    IPMC Examples: cint_ipmc_example.c                   
        cint_ipmc_example_multi_device.c      
    MPLS LSR: cint_mpls_lsr.c 
        cint_mpls_tunnel_initiator.c
        cint_mpls_mc.c
    ARP request/reply trapping
        cint_arp_traps.c

IP UC routing
---------------

~ Description:
    Example for IPv4 unicast router: demonstrate how to 
    - open router interface
    - create egress object
    - add route and host entries.
    - add host entries while saving FEC entries.
    - set VRF default destination

~ File Reference:
    cint_ip_route.c


IP UC routing explicit RIF
---------------------------------------------------------------------

~ Description:
    Example for IPv4 routing over 802.1q where the VSI/RIF is explicitly supplied by the user
    and not determined by the incoming VLAN value.
    The example includes:
    -	LIF Creation with an explicit VSI
    -	RIF Creation according to <port, vlan>
    -	Egress VLAN Edit configuration
     
~ File Reference:
    src/examples/dpp/cint_ip_route_explicit_rif.c
    src/examples/dpp/utility/cint_utils_global.c
    src/examples/dpp/utility/cint_utils_l2.c
    src/examples/dpp/utility/cint_utils_l3.c
    src/examples/dpp/utility/cint_utils_port.c
    src/examples/dpp/utility/cint_utils_vlan.c

IP UC RPF
-----------

~ Description:
    Example for IPv4 unicast RPF check. demonstrate how to:
    - set loose and strict RPF mode.
    - configure the RPF traps to drop or trap packet.

~ File Reference:
    cint_ip_route_rpf.c

IP Multicast
------------------

~ Description:
    Example for configuring ingress and egress for IP Multicast, adding IPv4 and IPv6 MC entries and traversing IPMC table.

~ File Reference:
    cint_ipmc.c  

IP Multicast RPF
------------------

~ Description:
    Example for IPv4 Multicast RPF check. demonstrate how to set RPF mode: SIP, explicit, none per egress object

~ File Reference:
    cint_ipmc_example_rpf.c

IP Multicast BIDIR
------------------

~ Description:
    Example for IPv4 Multicast with BIDIR. demonstrate mapping IP group to BIDIR and forward packet according to <VRF,Group> with admission test

~ File Reference:
    cint_ipmc_bidir.c

IP Multicast Flows
------------------

~ Description:
    Example of the various IPMC flows. Each flow is exemplified with its unique configurations and an exemplary packet flow.

~ File Reference:
    cint_ipmc_flows.c

IPv6 
-----

~ Description:
    Example for IPv6 Unicast and Multicast Routing 

~ File Reference:
    cint_ipv6_fap.c
    
IP Routing enablers
--------------------

~ Description:
    Tests the routing enablers feature by using all the possible routing enablers resources.
    This test combine IPV4 UC (with and without RPF/VPN), IPV6 UC(with and without RPF), 
    IP V4 MC (bridge and route) and IPV6 (bridge and route).

~ File Reference:
    cint_enablers.c
  
ARP Traps
----------
~ Description:
    Example for Trapping ARP packets to CPU.

~ File Reference:
    cint_arp_traps.c

Trap setting
----------
~ Description:
    Demonestrate how to configure trap and snoop according to trap code.

~ File Reference:
    cint_trap_mgmt.c
    cint_egr_mtu.c


ECMP
------

~ Description:
    Example for Multipath routing.

~ File Reference:
    cint_l3_multipath.c

EVPN
------
~Description:
    Example of EVPN application
~File Reference:
    cint_evpn.c
    cint_field_evpn.c

MPLS LSR QOS
----------------

~Description:
    Example of MPLS QOS
~File Refernce:
    cint_qos.c, cint_qos_elsr.c

Fiber Channel
--------------

~ Decription:  
    Example of FCoE, demonstrate Forwarding FCoE packet according to Fiber header. including zoning check
~ File Reference:
    cint_fcoe_route.c


MPLS ELSP
--------------

~ Decription:  
    Example of MPLS ELSP
~ File Reference:
    cint_qos_elsr.c, cint_mpls_elsp.c


Traditional Bridge 
--------------------

~Description:
    Example of L2 QOS
~File Reference:
    cint_qos_l2.c

MPLS/EXP marking for Bridging
=============================

~Description:
    Example of marking MPLS/EXP values even when the packet is bridged (not routed).
    (BCM88660 Only)
~File Reference:
    cint_qos_marking.c

MPLS utilities
--------------------

~Description:
    Utilities functions for mpls applications.
~File Reference:
    cint_utils_mpls.c

VPLS utilities
--------------------

~Description:
    Utilities functions for vpls applications.
~File Reference:
    cint_utils_mpls_port.c

OAM utilities
--------------------

~Description:
    Utilities functions for oam applications.
~File Reference:
    cint_utils_oam.c

Multicast utilities
------------

~Description:
    Utilities functions for multicast applications.
~File Reference:
    cint_utils_multicast.c


Field processor utilities
------------

~Description:
    Utilities functions for field processor application.
~File Reference:
    cint_utils_field.c


L3 Routing
------------

~Description:
    Example of L3 QOS
~File Reference:
    cint_qos_l3.c



FCoE 
---------------

~ Description:
    Example for FCoE FCF (routing according to FC header)
    - open router interface
    - create egress object
    - add several routes and zonning rules

~ File Reference:
    cint_foce_route.c


Termination
--------------

~ Description
    Example of MyMac termination, where MyMac is set based on VRID. 
    The CINT demonstrates the different VRID-My-MAC-Modes.

~ File Reference:
    cint_l3_vrrp.c
    cint_ip_route.c

~ Description
    Example of flexiable multiple MyMac termination.
    Available in BCM88660 only.

~ File Reference:
    cint_multiple_mymac_term.c


Ring Protection Fast Flush
-----------------------------

~ Description
    Example of L2 Ring Protection setup and Fast Flush operation.
    Based on a vswitch setup.
    Includes failover group creation, gport association and
    de association to and from a group and Fast Flush execution

~ File Reference:
    cint_l2_fast_flush.c
    cint_port_tpid.c
    cint_l2_mact.c
    cint_multi_device_utils.c
    cint_vswitch_metro_mp.c


Port Match
------------------------

~ Description
    Configure several ingress / egress mappings over the same gport.
    Also demonstrate usage of asymmetric Ingress-only and Egress-only LIFs.

~ File Reference:
    cint_port_match.c

Port Filtering
------------------

~ Description
    Configure egress acceptable frames both for ERPP & ETPP discard.

~ File Reference:
    cint_egress_acceptable_frame_type.c

+------------------------+ 
|     Field Processor   |
+------------------------+ 
 ~ Description: 
    Examples related to ACL/PCL funtionality. Relevant for PP users.
    
 ~ File Reference:
    cint_fid_demo.c   

----------

~ Description:
    Add bcmFieldActionStat and bcmFieldActionRedirect
    for egress ACE table with per-core config params

~ File Reference:        
    cint_field_action_config.c    

------------

~ Description:
    For MPLS-forwarded packets, in case of MPLS pop, modify the identification of the Upper-Layer protocol

~ File Reference:
    cint_field_change_upper_layer_protocol_for_pop_mpls.c     

----------
        
~ Description:
    For IP-routed packets, map the PCP to the DSCP
    
~ File Reference:        
    cint_field_copy_pcp_to_dscp_for_ip_packets.c    

------------
        
~ Description:
    Perform static forwarding at egress and associate each port to a count.

~ File Reference:
    cint_field_egress_modify_tc_per_port.c

------------

~ Description:
    Perform exhaustive testcase field configuration.

~ File Reference:
    cint_field_exhaustive.c
----------

~ Description:
    Correct the learn information in VxLAN and NVGRE application

~ File reference: 
    cint_field_gre_learn_data_rebuild.c    
    
----------

~ Description:
    Usage of Key-A with single bank database.

~ File reference: 
    cint_field_key_a.c    
    
----------

~ Description:
    Correct the learn information in MAC-in-MAC application. 
    Used in case of Egress-PBB MAC-in-MAC termination.

~ File reference: 
    cint_field_mim_learn_info_set.c    
    
----------

~ Description:
    Mirror ARP Reply packets to local Port 0 (in general CPU)

~ File reference: 
    cint_field_mirror_arp_reply_frame.c    
    
----------

~ Description:
    Map the forwarding label EXP + RIF.CoS-Profile to TC and DP

~ File Reference:
    cint_field_mpls_lsr_copy_exp_to_tc_dp.c    

----------

~ Description:
    Modify the PCP value according to the Destination-MAC after a VLAN translation for Outer-Tag packets.
~ File Reference:
    cint_field_modify_pcp_per_da_after_vlan_translation.c    

---------

~ Description:
    Redirect traffic according to L2 / L3 header
~ File Reference:
    cint_field_policer_redirect_per_l2_l3_addresses.c    
        
-------------        

~ Description:
    Accept / Drop IP-routed packets according to Source and Destination L4 Port ranges
~ File Reference:
    cint_field_range_multi_demo.c    
        
-------------

~ Description:
    Accept / Drop IP-routed packets according to packet length ranges
~ File Reference:
    cint_field_range_pktlen_demo.c    
        
-------------

~ Description:
    Snoop IP-routed packets
~ File Reference:
    cint_field_snoop_tc_change_ip_routed_frame.c    
    
-------------

~ Description:
    Example of preselection usage with in/out-port as qualifier
~ File Reference:
    cint_field_presel_in_out_ports.c    

-------------


~ Description:
     Example of how to use presel management Advanced mode
~ File Reference:
    cint_field_presel_mgmt_advanced_mode.c    

-------------

~ Description:
    For TM packets, extract the LAG Load-Balancing Key after the ITMH
    and move the desired number of bytes (set per packet)
~ File Reference:
    cint_field_tm_copy_lag_lb_key_after_itmh.c    

-------------

~ Description:
    For TM packets, extract the Scheduler-Compensation value after the ITMH
    and add padding after ITMH according to the compensation value
~ File Reference:
    cint_field_tm_scheduler_compensation.c    

-------------

~ Description:
    Example for the use of Direct Table for cascaded lookup
~ File Reference:
    cint_field_mact_llid_limit.c    

-------------

~ Description:
    Rebuild the Learn-Data with the current Source-System-Port and current In-LIF 
~ File Reference:
    cint_field_learn_data_rebuild.c    

-------------

~ Description:
    Configure ingress field processor to add learn extension to PPH
    and verify it is being added to the packet in egress
~ File Reference:
    cint_field_learn_extension.c    

-------------

~ Description:
    If the Forwarding-Header-Offset is over 32 Bytes, remove up to 14 Bytes 
    after Ethernet header. This rule is a walk-around due to the limited 
    capacities of Egress stage to terminate large termination headers. 
~ File Reference:
    cint_field_ingress_large_termination.c    

-------------

~ Description:
    According to the packet format, if not simple bridged, set the rules to 
    extract the ECN value (2 bits), and set: 
    - if the ECN is supported (ECN-capable)
    - in this case, if the packet is congested (CNI bit)
~ File Reference:
    cint_field_ecn_cni_extract.c    
    
----------

~ Description:
    According to the packet format, if packet id ipv4 and ECN is supported, enable
    DM ECN according to a given threshold.
~ File Reference:
    cint_field_ecn_dm.c    
    
----------
        
~ Description:
    Create Direct Extraction field-group with action as counter 0
    that returns the value of the InLIF.
    
~ File Reference:        
    cint_field_dir_ext_counter_inlif.c

----------
        
~ Description:
    Create different Field groups for Fiber Channel application.
    
~ File Reference:        
    cint_field_fiber_channel.c

----------
        
~ Description:
    Create Field group for IP Multicast Bidirectional application.
    
~ File Reference:        
    cint_field_ipmc_bidir.c

----------
        
~ Description:
    Create Field group for VPLS over GRE work-around.
    
~ File Reference:        
    cint_field_vpls_gre_wa.c

----------
        
~ Description:
    Create Field entries to copy I-SID to system headers when working with 32K I-SIDs
    
~ File Reference:        
    cint_field_mim_32k_isids.c

----------
        
~ Description:
    Create an ELK ACL based on 5-tuples ACL key for all IPv4 unicast packets
    and use the External lookup result as Meter-ID if matched.
    
~ File Reference:        
    cint_field_elk_acl_5_tuples_ip4_uc.c

----------
        
~ Description:
    Create Field group for VxLAN if no hit during 
    SIP or VNI mapping lookups.
    
~ File Reference:        
    cint_field_vxlan_lookup_unfound.c

----------
        
~ Description:
    Create field group that drops all packets with static SA found, 
    but without a match for destination (transplant).
    
~ File Reference:        
    cint_field_drop_static_sa_transplant.c

----------
        
~ Description:
    Create two field groups, using a flag indicating these databases should be compared with one another. 
    Use compare result in second field group action (Direct Extraction).
        
~ File Reference:        
    cint_field_dir_ext_compare_result.c.c

----------
        
~ Description:
    Create 5-tuples field group that classify frame with 5-tuples parameters (SIP,DIP, IP protocol, L4 Src and DST ports) to InLIF.
        
~ File Reference:        
    cint_field_flexible_qinq_example.c

----------

~ Description:
    Explain the sequence to enable IPv6 extension header parsing and demonstrate the use in Field Processor.

~ File Reference:
    cint_field_ipv6_extension_header.c
	
----------

~ Description:
    Use SOC property pmf_vsi_profile_full_range for PMF to use 4 bits of VSI profile (L2CP traps disabled)

~ File Reference:
    cint_field_qual_vsi_profile_full_range.c

----------

~ Description:
	Create a FG and in_rif_profile=1. Action drop for matching pkt.   
~ File Reference:
	cint_field_qualify_IncomingIpIfClass.c
	
	
+------------------------+ 
|    VLAN Translation    |       
+------------------------+ 
 ~ Description:
    Set the default VLAN action and the VLAN action after lookup  
    ingress and at egress. Relevant for PP users.       
    
 ~ File Reference:
    cint_fap_vlan_translation.c  
    
 ~ Description:
    Examples of usage for vlan translation actions in 
    translation new mode (VLAN translation new mode is set when soc property
    bcm886xx_vlan_translate_mode is enabled). Relevant for PP users.
    1. Transparent TPID in ingress/egress vlan editing
    2. Default translation actions in ingress/egress  
    
 ~ File Reference:
    cint_advanced_vlan_translation_mode.c                   

~ Description: 
   Examples of using advanced vlan translation with trill

~ File Reference:
    cint_trill.c
    cint_trill_advanced_vlan_translation_mode.c
	
~ Description: 
   Examples of setting a  port default vlan translation to advanced mode

~ File Reference:
  src/examples/dpp/cint_vlan_edit_port_default_lif.c
	
    
+------------------------+ 
|      MACT Learning     |
+------------------------+ 
 ~ Description: 
    Example of different learning modes and message types.      
    Relevant for PP users.                                      

 ~ File Reference: 
    cint_l2_learning.c     
    cint_l2_learning_multi_device.c
        - Right now used only to support cint_vswitch_metro_mp.c. Not yet ready to be used on its own.

~ Description:
    Example for learning: demonstrate how to control the learning message per vlan, 
    send CPU or to OLP (for automatic learning)

~ File Reference:
    cint_l2_vlan_learning.c


+------------------------------+
|      Learn Event Parsing     |
+------------------------------+
 ~ Description:
    - Manually construct DSP packets according to predefined parameters
    - Transmit packet and save LEM Payload
    - Read entry from LEM, add via BCM API
    - Save LEM Payload and compare to previous entry

 ~ File Reference:
    cint_l2_learn_event_parsing.c


+------------------------+ 
| MACT Bulk Operations   |
+------------------------+ 
 ~ Description: 
    - dump/traverse MACT table according to group (user defined value)
    - perform several rules in one Hardware traverse
    - traverse rule includes masking on VLAN, gport, etc..
 ~ File Reference: 
    cint_mact_bulk_operations.c     

CPU SHADOW
----------------

~ Description:
    Example for sending MACT events to CPU

~ File Reference:
    cint_l2_cpu_learning.c

SA-drop
---------

~ Description:
    Example for dropping Packets according to SA.

~ File Reference:
    cint_l2_mact_sa_discard.c
    
+------------------------+
|        Warmboot        |   
+------------------------+
 ~ Description:
    Save and restore SW databases.
    
 ~ File Reference:
    cint_warmboot_utilities.c

+------------------------+
|        TRILL           |   
+------------------------+
 ~ Description:
    Trill UC/MC scenarios.
    
 ~ File Reference:
    PetraB trill unicast: cint_trill_uni_petrab.c
    PetraB trill ingress multicast: cint_trill_mc_ing_petrab.c
    PetraB trill transit multicast: cint_trill_mc_transit_petrab.c
    PetraB trill egress multicast: cint_trill_mc_egr_petrab.c
    ARAD (UC+MC in the same CINT): cint_trill.c
    ARAD FP example for Trill 2-pass (In transit, snoop a copy to recycle port): cint_pmf_trill_2pass_snoop.c
    ARAD+ trill multi homing: cint_trill_multi_homing.c
    ARAD (UC+MC transparent service in the same CINT): cint_trill_tts.c

+-------------------------+
| Internal ports (OLP/RCY |
+-------------------------+
 ~ Description:
    Examples for retrieving and configuring internal ports (OLP / Recycling).
    
 ~ File Reference:
    cint_internal_ports.c

+-------------------------+
|           MIRROR          |
+-------------------------+
~ Description:
    Examples of Mirror tests and usage

~ File Reference:
    cint_mirror_tests.c

~ Description:
    Example for mirror packet according to port x vlan, with inbound/outbound mirroring. where the mirror packet:
    - is SPAN mirrored
    - with RSPAN encapsulation
    - with ERPSAN encapsulation

~ File Reference:
    cint_mirror_erspan.c

~ Description:
    Example for mirror packet according to port x vlan, with inbound/outbound mirroring. 
    The example is only used in Petra-B.

~ File Reference:
    cint_petra_mirror_tests.c

~ Description:
    General mirror utils

~ File Refernce:
    cint_utils_mirror.c 

+-------------------------+
|     TRAP \ SNOOP      |
+-------------------------+

~ Description:
    Example of setting a trap type code and a snoop command.

~ File Refernce:
    cint_rx_trap_fap.c

~ Description:
    Example of setting an egress trap type Unknown DA / Unknown MC / BC

~ File Refernce:
    cint_port_eg_filter_example.c 

~ Description:
    General trap utils

~ File Refernce:
    cint_utils_rx.c 

~ Description:
	Example of configuring MTU filter per LIF in egress.

~ File Refernce:
	cint_etpp_lif_mtu.c
	
	
~ Description:
	Configure destination per core for ingress traps

~ File Refernce:
	cint_config_dest_per_core.c

+---------------+                                                        
|    Switch  |                                                         
+---------------+  

~ Description:
    General switch utils

~ File Refernce:
    cint_utils_switch.c 

+---------------+                                                        
|    Stacking  |                                                         
+---------------+                                                         
 ~ Description:  
    setup that configure a stacking state.
    Example: in cint_stk_mc_dyn_3tmd.c - the application should run on 3 connected Arad. 
    It's creating a stacking system when each Arad is TM domain, 
    with a multicast group that passes through this system.   
                  
 ~ File Reference:
    cint_stk_uc.c 
    cint_stk_uc_dyn.c
    cint_stk_uc_dyn_3tmd.c  
    cint_stk_mc.c


+---------------+                                                        
|    Tunneling  |                                                         
+---------------+                                                         

IPV4 tunneling
----------------

~ Description:
    Example for ipv4  packet forwarding into IPv4 tunnel with/without GRE encapsulation.
    
~ File Reference:
    cint_ip_tunnel.c

IPV4 tunnel termination:
----------------------------

~ Description:
    Example for ipv4  tunnel termination and forward packet according to internal IPv4 header. demonstrate how to
    - create tunnel terminator
    - associate packet to VRF according to terminated tunnel.
    - route according to VRF

~ File Reference:
    cint_ip_tunnel_term.c

IPv6 tunneling
----------------

~ Description:
    Example for IPv4 packets forwarding into IPv6 tunnel.

~ File Reference:
    cint_ipv6_tunnel.c

IPV6 tunnel termination
---------------------------

~ Description:
    Example for IPv6 tunnel termination and forwarding according to internal IPv4 header.

~ File Reference:
    cint_ipv6_tunnel_term.c

+---------------+                                                        
|    L2Gre        |                                                         
+---------------+                                                         

~ Description:
    Example for L2Gre application, 

~ File Reference:
    cint_l2gre.c

VPLSoGRE
--------

~ Description
    Example of VPLSoGRE using recycle.

~ File Reference:
    cint_vswitch_vpls_gre.c

+---------------+                                                        
|    Vxlan        |                                                         
+---------------+                                                         

~ Description:
    Example for VXLAN application, 

~ File Reference:
    cint_vxlan.c
	src/examples/dpp/utility/cint_utils_vxlan.c



+---------------------+                                                        
| SA Authentication |                                                         
+---------------------+

~Description:
                Example of SA Authentication application

~File Refernce:
               cint_l2_auth.c


+---------------+                                                        
| Mac-in-mac  |                                                         
+---------------+                                                         

MP
------

~ Description:
    Example for Mac-in-Mac MP. Demonstrates the following:
    - creating B-VID and VSI.
    - creating a Mac-in-Mac (backbone) port.
    - setting B-tag TPID.
    - creating a customer (access) port.
    - differential flooding for packets arriving from Mac-in-Mac (backbone) ports and from customer (access) ports.
    - bridging between 2 Mac-in-Mac (backbone) ports.
    - setting MyB-Mac for termination and encapsulation.
    - setting drop-and-continue 2 pass functionality for I-SID broadcast.
    - setting IPv4/6 UC ROO MIM
    - setting IPv4 MC ROO MIM

~ File Reference:
    cint_mim_mp.c
    cint_pmf_2pass_snoop.c
    src/examples/dpp/cint_mim_mp_roo.c
    

P2P
-------

~ Description:
    Example for Mac-in-Mac P2P. Demonstrates the following:
    - creating B-VID.
    - creating a Mac-in-Mac (backbone) port.
    - setting B-tag TPID.
    - creating a customer (access) port.
    - Cross-connecting a Mac-in-Mac (backbone) port and a customer (access) port.

~ File Reference:
    src/examples/dpp/cint_mim_p2p.c
    src/examples/dpp/cint_mim_mp.c



+---------------+                                                        
|       P2P       |                                                         
+---------------+                                                         

~ Description:
    Example for P2P. Demonstrates cross-connecting between:
    - 2 VLAN ports.
    - 2 Mac-in-Mac (backbone) ports.
    - MPLS port and VLAN port.
    - MPLS port and Mac-in-Mac (backbone) port.
    - VLAN port and Mac-in-Mac (backbone) port.
    Cross-connections can be done also with facility/path protection.

~ File Reference:
    src/examples/dpp/cint_mpls_lsr.c
    src/examples/dpp/cint_vswitch_cross_connect_p2p.c
    src/examples/dpp/cint_vswitch_cross_connect_p2p_multi_device.c 
        - can be used to cross connect two or more devices. 
    src/examples/dpp/cint_vswitch_cross_connect_p2p_directional.c
        - used for directional cross connect PWE to AC


+---------------+                                                        
|       QOS      |                                                         
+---------------+                                                         

L3 QOS
---------

~ Description:
    Example for L3 quality of service. Demonstrate the following:
    - creating ingress router interface, associated with in-RIF QOS-profile-id.
    - ingress packet DSCP mapping to in-DSCP. 
    - in-DSCP mapping to TC+DP.
    - creating egress router interface, associated with out-RIF QOS-profile-id.
    - egress in-DSCP+DP mapping to out-DSCP.

~ File Reference:
    src/examples/dpp/cint_qos_l3_rif_cos.c
    src/examples/dpp/cint_ip_route.c


+---------------+                                                        
|    Policer    |                                                         
+---------------+                                                         

Metering
----------

~ Description:
    Example for the different modes of Metering. Demonstrate the following:
    - Create several meter entries, in different modes. 
    - Getting each entry, to see the rates and bursts that were actually set in HW.
    - Mapping packets to meter and testing the configured rate with traffic.
    - Testing packets arriving with Green or Yellow color.
    - Mapping two meters per-packet.
    - Destroying the meters.

~ File Reference:
    src/examples/dpp/cint_policer_metering_example.c

Qos policer mapping
-------------------

~ Description:
    Example for mapping Drop procedure to PCP-DEI. Demonstrate the following:
    - Create Qos map mapping Green and Yellow color to PCP-DEI of outer tag or inner tag
    - Mapping packets to meters and test packets color
    - Testing outer tag or inner tag PCP-DEI of packetes with Green and Yellow color
    - Destroying Qos map

~ File Reference:
    src/examples/dpp/cint_qos_policer.c

+---------------+                                                        
|    Rate       |                                                         
+---------------+                                                         

Ethernet Policer
-----------------

~ Description:
    Example for configuring Ethernet Policer. Demonstrate the following:
    - Configuring different rates and burst sizes for each traffic type (UC, unknown UC, MC, unknown MC and BC).
    - Getting each entry, to see the rates and bursts that were actually set in HW.
    - Testing the configured rates with traffic.
    - Destroying the Ethernet Policer configuration.

~ File Reference:
    src/examples/dpp/cint_rate_policer_example.c
    

+----------------------+                                                        
|    Counter Engine    |                                                         
+----------------------+                                                        

Counter Engine
----------

~ Description:
    Example for counting all the Counters of Ingress VOQs / Egress VOQs. Demonstrate the following:
    - Set the Counter Engines to count in high resolution per VOQ. 
    - Get per VOQ all the possible counters (according to the Color and Drop / Forward)
    - Mapping a LIF stack range.

~ File Reference:
    src/examples/dpp/cint_voq_count.c
    src/examples/dpp/cint_ctrp_egress_received_tm.c
    src/examples/dpp/cint_counter_processor.c


+----------------------------+                                                        
|    Flexible hashing        |
+----------------------------+ 
Flexible Hashing
----------------

~ Description:
   Example of flexible hashing. 
   Flexible hashing utilizes the key construction of stateful-load-balancing 
   without learning or looking-up the keys. 
   Once a key has been constructed in the SLB, it may be copied to the 80 msbs 
   of one (or more) of the PMF keys in the 2nd cycle. 
   
~ File Reference:
   src/examples/dpp/cint_flexible_hashing.c
+---------------+                                                        
|    ECMP       |                                                         
+---------------+                                                         

ECMP Hashing
------------

~ Description:
    Example for creating an ECMP, mapping IPV4 host and MPLS LSR to this ECMP and showing how ECMP hashing can be done 
    according to different parts of the header. Demonstrate the following:
    - Creating an ECMP, containing several FEC entries, each one pointing to the same encapsulation but a different out-port.
    - Adding an IPV4 host and mapping it to this ECMP.
    - Addeing an MPLS LSR entry and mapping the label to this ECMP.
    - Configuring the ECMP hashing: disabling the hshing, setting the hashing according to a certain part of the header.
    - Testing with traffic and seeing how the hashing configuration affects the traffic distribution.

~ File Reference:
    src/examples/dpp/cint_ecmp_hashing.c
    
+---------------+                                                        
|    LAG        |                                                         
+---------------+                                                         

LAG Hashing
-----------

~ Description:
    Example for creating a Trunk (LAG), mapping IPV4 host and MPLS LSR to this LAG and showing how LAG hashing can be done 
    according to different parts of the header. Demonstrate the following:
    - Creating a LAG and add several ports to it.
    - Adding an IPV4 host and mapping it to this LAG.
    - Addeing an MPLS LSR entry and mapping the label to this LAG.
    - Configuring the LAG hashing: disabling the hshing, setting the hashing according to a certain part of the header and also
      setting the number of hreaders to look at and the starting header.
    - Testing with traffic and seeing how the hashing configuration affects the traffic distribution.

~ File Reference:
    src/examples/dpp/cint_lag_hashing.c    

+---------------+                                                        
|    PON        |                                                         
+---------------+                                                         

PON Application
---------------

~ Description:
    Example for PON Application to bring up three main PON services categories: 1:1 Model , N:1 Model and TLS.
    Demonstrate the following:
    - Setting ports as NNI ports and PON ports.
    - Creating PON LIFs to match Tunnel_ID and VLAN tags.
    - Setting PON LIF ingress VLAN editor to do upstream VLAN translation.
    - Setting PON LIF egress VLAN editor to do downstream VLAN translation.
    - Creating NNI lifs to match port VLAN domain and VLAN tags.
    - Adding PON LIFs and NNI LIFs to vswitch or cross connecting two of them as 1:1 service.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_application.c

PON TUNNEL_ID to CHANNEL PROFILE MAPPING
---------------------------------------------

~ Description:
    An example of how to map tunnel_ID to PP port of PON application and 
    set different properties (VTT lookup mode and VLAN range) per LLID profile.
    Demonstrate the following:
    - Setting ports as NNI ports and PON ports.
    - Create PON PP ports with different PON tunnel profile ids.
    - Map different tunnel ids to PON PP ports.
    - Enable different port tunnel lookup in PON PP ports.
    - Add different VLAN range info to PON PP ports.
    - Creating PON LIFs to match Tunnel_ID and VLAN tags.
    - Setting PON LIF ingress VLAN editor to do upstream VLAN translation.
    - Setting PON LIF egress VLAN editor to do downstream VLAN translation.
    - Creating NNI lifs to match port VLAN domain and VLAN tags.
    - Setting NNI LIF ingress VLAN editor to replace ctag or both stag and ctag.
    - Adding PON LIFs and NNI LIFs to vswitch or cross connecting two of them as 1:1 service.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c   
    src/examples/dpp/pon/cint_pon_tunnel_mapping.c
    
PON VLAN RANGE
---------------------------------------------

~ Description:
    An example of how to match incoming VLAN using VLAN range of PON application.
    Demonstrate the following:
    - Setting ports as NNI ports and PON ports.
    - Add different VLAN range info to PON ports.
    - Creating PON LIFs to match Tunnel_ID and VLAN tags.
    - Setting PON LIF ingress VLAN editor to do upstream VLAN translation.
    - Setting PON LIF egress VLAN editor to do downstream VLAN translation.
    - Creating NNI lifs to match port VLAN domain and VLAN tags.
    - Setting NNI LIF ingress VLAN editor to replace ctag or both stag and ctag.
    - Adding PON LIFs and NNI LIFs to vswitch or cross connecting two of them as 1:1 service.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c   
    src/examples/dpp/pon/cint_pon_vlan_range.c

PON TUNNEL PROFILE PROPETIES
---------------------------------------------

~ Description:
    An example of how to map tunnel_ID to PP port of PON application and 
    set different properties (VTT lookup mode, VLAN range and frame acceptable mode) per LLID profile.
    Demonstrate the following:
    - Setting ports as NNI ports and PON ports.
    - Create PON PP ports with different PON tunnel profile ids.
    - Map different tunnel ids to PON PP ports.
    - Enable different port tunnel lookup in PON PP ports.
    - Add different VLAN range info to PON PP ports.
    - Set different frame acceptable mode to PON PP ports.
    - Creating PON LIFs to match Tunnel_ID and VLAN tags.
    - Setting PON LIF ingress VLAN editor to do upstream VLAN translation.
    - Setting PON LIF egress VLAN editor to do downstream VLAN translation.
    - Creating NNI lifs to match port VLAN domain and VLAN tags.
    - Setting NNI LIF ingress VLAN editor to replace ctag or both stag and ctag.
    - Adding PON LIFs and NNI LIFs to vswitch or cross connecting two of them as 1:1 service.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c   
    src/examples/dpp/pon/cint_pon_tunnel_profile.c

PON VMAC
---------------------------------------------

~ Description:
    An example of how to use BCM APIs to setup VMAC function 
    Demonstrate the following:
    - Setting ports as NNI ports and PON ports.
    - Enable VMAC and setting VMAC encoding value and mask.
    - Setting field_class_id_size.
    - Disable ARP (next hop mac extension) feature.
    - Setup a PP model for PON application.
    - Enable VMAC per AC.
    - Update user-headers by PMF.
~ File Reference:
    src/examples/dpp/pon/cint_pon_vmac.c   
    src/examples/dpp/pon/cint_pon_field_vmac.c

PON UP RATELIMIT
---------------------------------------------

~ Description:
    An example of two level ratelmits based on upstream.
    First level ratelimit is done per outer VLAN within LLID.
    Second level ratelimit is done per LLID.
    Demonstrate the following:
    - Create the second level policer in group = 1.
    - Create PMF entries to qualify LLID and Outer VLAN.
    - Create the first level policer in group = 0.
    - Attach the first level policer to the PMF entry.
    - Attach the second level policer to the PMF entry.
    - Install PMF entries.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c   
    src/examples/dpp/pon/cint_pon_up_ratelimit.c

PON IPV4 ACL
---------------------------------------------

~ Description:
    examples of IPv4 classificaiton and ACLs.
    IPv4 classification based on IPv4, DA, IP and IPv4 DA IP + L4 Dst Port. Classification means map traffic to new queue and new cos.
    ACLs drop packets based on DA, SA, VLAN, IPPROTOCOL, SIP, L4SrcPort, L4DstPort.
    Demonstrate the following:
    - Create IPv4 classification PMF entries to qualify LLID, DA, SIP and other properties of IPv4.
    - Set PMF entries action to modify outer priority and internal priority.
    - Install PMF entries.
    - Create ACL PMF entries to qualify LLID, DA, SIP and other properties of IPv4.
    - Set PMF entries action to drop.
    - Install PMF entries.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_up_scheduler.c
    src/examples/dpp/pon/cint_pon_ipv4_acl.c

PON IPV6 ACL
---------------------------------------------

~ Description:
    examples of IPv6 classificaiton and ACLs.
    IPv6 classification based on IPv6, SIP and DIP. Classification means map traffic to new queue and new cos.
    ACLs drop packets based on DA, SA, VLAN, IPPROTOCOL, SIP, L4SrcPort, L4DstPort.
    Demonstrate the following:
    - Create IPv4 classification PMF entries to qualify LLID, SIP and DIP of IPv6.
    - Set PMF entries action to modify outer priority and internal priority.
    - Install PMF entries.
    - Create ACL PMF entries to qualify LLID, DA, SIP and other properties of IPv6.
    - Set PMF entries action to drop.
    - Install PMF entries.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_up_scheduler.c
    src/examples/dpp/pon/cint_pon_ipv6_acl.c
    
PON CHANNELIZED PORTS
---------------------------------------------

~ Description:
    An example of the PON channlized port application and 
    how to set different properties (VTT lookup mode and VLAN range) per LLID profile based on channelized port. 
    Demonstrate the following:
    - Setting ports as NNI ports and channelized PON ports.
    - Create PON PP ports with different PON tunnel profile ids.
    - Map different tunnel ids to PON PP ports.
    - Enable different port tunnel lookup in PON PP ports.
    - Add different VLAN range info to PON PP ports.
    - Set different frame acceptable mode to PON PP ports.
    - Creating PON LIFs to match Tunnel_ID and VLAN tags.
    - Setting PON LIF ingress VLAN editor to do upstream VLAN translation.
    - Setting PON LIF egress VLAN editor to do downstream VLAN translation.
    - Creating NNI lifs to match port VLAN domain and VLAN tags.
    - Setting NNI LIF ingress VLAN editor to replace ctag or both stag and ctag.
    - Adding PON LIFs and NNI LIFs to vswitch or cross connecting two of them as 1:1 service.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c   
    src/examples/dpp/pon/cint_pon_channelized_port.c

PON ADDITIONAL TPIDs
---------------------------------------------

~ Description:
    An example of how to use additional TPID for NNI port of PON application.
    Demonstrate the following:
    - Setting ports as NNI ports and PON ports.
    - Add additional TPIDs.
    - Create PON PP ports with different PON tunnel profile ids.
    - Map different tunnel ids to PON PP ports.
    - Enable different port tunnel lookup in PON PP ports.
    - Add different TPIDs to PON PP ports.
    - Creating PON LIFs to match Tunnel_ID and VLAN tags.
    - Setting PON LIF ingress VLAN editor to do upstream VLAN translation.
    - Setting PON LIF egress VLAN editor to do downstream VLAN translation.
    - Creating NNI lifs to match port VLAN domain and VLAN tags.
    - Setting NNI LIF ingress VLAN editor to replace ctag or both stag and ctag.
    - Adding PON LIFs and NNI LIFs to vswitch or cross connecting two of them as 1:1 service.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c   
    src/examples/dpp/pon/cint_pon_additional_tpids.c

PON ROUTING OVER AC
---------------------------------------------
~ Description:
    An example of how to o setup route over ac function in PON application downstream.
    Demonstrate the following:
    - Setting ports as NNI ports and PON ports.
    - Setting normal L3 application.
    - Creating PON LIF and NNI LIF.
    - Adding a route in L3 host table
    - Setting PON LIF egress VLAN editor to add tunnel ID and outer VID.
    - Setting PMF rules to extract ARP to OutLIF
~ File Reference:
    src/examples/dpp/utility/cint_utils_global.c
    src/examples/dpp/utility/cint_utils_l2.c
    src/examples/dpp/utility/cint_utils_l3.c
    src/examples/dpp/utility/cint_utils_vlan.c
    src/examples/dpp/utility/cint_utils_port.c
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_route_over_ac.c
    src/examples/dpp/pon/cint_pon_field_router_over_ac.c

+---------------+                                                        
| CMIC RCPU     |                                                         
+---------------+                                                         
- Description:
   Example for RCPU using 
     - example for system configuration
     - Packet flow
     - Packet format
 - File Reference:
    src/examples/dpp/cint_cmic_rcpu.c
    
+---------------+                                                        
| CPU RX        |                                                         
+---------------+                                                         
- Description:
   Example of CPU RX packets receive config 
     - with and without COS usage
     - start rx
     - configure different rx ports
     - configure COS to different rx ports
     - configure appropriate callbacks to different rx ports
 - File Reference:
    src/examples/dpp/cint_rx_cos.c
    src/examples/dpp/cint_rx_no_cos.c
+---------------+                                                        
| Trunk         |                                                         
+---------------+                                                         
 - Description:
   Example of trunk manipulation
     - create trunk
     - add and remove ports to or from trunk
     - remove all ports from trunk
     - remove a trunk
 - File Reference:
    src/examples/dpp/cint_trunk.c

 - Description:
    This CINT emulates the trunk(LAG) member and FEC resolution process performed by the Arad.

 - File Reference:
    cint_trunk_ecmp_lb_key_and_member_retrieve.c

+-------------------------+
| ARAD scheduling scheme  |
+-------------------------+
 ~ Description:
    Building an ARAD scheduling scheme in 8 priorities mode

 ~ File Reference:
    cint_tm_fap_multi_devices_8_priorities_example.c

+-------------------------------+
| Congestion Notifiation (CNM)  |
+-------------------------------+
 ~ Description:
         Configuration of the CNM mechanism.
             - Enable/Disable the CNM mechanism.
             - Set the Managed Queues range
             - Enalbe/Disable a Managed Queue
             - Configure the Congestion Test parameters
             - Configure the Transport Attributes of the CNM packets
 ~ File Reference:
         cint_cnm.c


+---------------------+
| Multi Device Cints  |
+---------------------+

~ Description:
        Those cints are the same as the regular cints in terms of device configurations, 
        but the configurations are done on multiple devices and not on just one. 
        There are two differences in calling multi device cints and calling regular cints:
            * Instead of accepting unit as parameter, the run functions accept an array of units as first argument, 
              and the number of units as second argument. Every unit should appear only one in the array.
            * Instead of accepting port number as argument, all multi device cints need to accept system port number. 
              To get system port number, you can use port_to_system_port in cint_multi_device_utils.c
              
~ File Reference:
        cint_multi_device_utils.c
        cint_vswitch_cross_connect_p2p_multi_device.c
        cint_vswitch_metro_mp.c
        cint_mpls_lsr.c
        cint_ip_route.c
        cint_ip_route_tunnel.c
        cint_ip_route_tunnel_remark.c

+--------------------------------+ 
|   MAC limiting per tunnel ID   |    
+--------------------------------+ 
 ~ Description:
    Example of MAC limiting base Tunnel ID  
     - Set MAC limiting mode by soc property 
     - Create pon application service
     - Set MAC limiting base Tunnel ID      

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_mact_limit.c

~ Description:
    Example for the use of Direct Table for cascaded lookup to 
    drop packets when MAC limiting reached.
~ File Reference:
    src/examples/dpp/pon/cint_pon_field_mact_llid_limit.c

+--------------------------------+ 
|   PORT VLAN RANGE              |    
+--------------------------------+ 
 ~ Description:
    An example of how to match incoming VLAN using VLAN range of Petra and Arad.
    Demonstrate the following:
    - Add different VLAN range info to ports.
    - Create LIFs.
    - Cross connect the 2 LIFs.
~ File Reference:
    src/examples/dpp/cint_port_tpid.c    
    src/examples/dpp/cint_port_vlan_range.c     

+--------------------------------+ 
|   Anti-spoofing function       |  
+--------------------------------+ 
 ~ Description:
    Example of how to use BCM APIs to implement anti-spoofing(L3 Source Binding) function in pon appplication service    
     - Set PON application configuration and L3 source bind mode by soc property 
     - Enable pon application service
     - Set L3 source binding configuration
     - Enable FP to drop un-matched traffic     

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_anti_spoofing.c
	
+----------------------------------------+ 
|   General Anti-spoofing function       |  
+----------------------------------------+ 
 ~ Description:
    Example of how to use BCM APIs to implement general anti-spoofing(host & subnet anti-spoofing) function
	in pon application service.
     - Set PON application configuration and L3 source bind mode by soc property 
     - Enable pon application service
     - Set L3 source binding configuration
     - Enable FP to drop un-matched traffic     

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_general_anti_spoofing.c

+--------------------------------+ 
|   L2 management function       |  
+--------------------------------+ 
 ~ Description:
    Example of how to use BCM APIs to implement L2 management function in pon appplication service    
     - Set PON application configuration 
     - Implement L2 management function
       Add a L2 entry
       Get a L2 entry
       Delete a L2 entry
       Delete L2 entries by port
       Delete dynamic L2 entries by port
       Delete static L2 entries by port
       Get how many L2 entries in port
       Get how many dynamic L2 entries in port
       Get how many static L2 entries in port

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_mac_management.c
    
+--------------------------------+ 
|   COS remark function          |  
+--------------------------------+ 
 ~ Description:
    Example of how to use BCM APIs to implement cos remark function in pon appplication service    
     - Set PON application configuration
     - Enable pon application service
     - Set cos remark configuration

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_cos_remark.c

+--------------------------------+ 
|   Multicast function           |  
+--------------------------------+ 
 ~ Description:
    Example of how to use BCM APIs to implement Multicast function in pon appplication service    
     - Set PON application configuration
     - Enable pon application service
     - Create a Multicast group
     - Port join a group
     - Port leave a group

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_multicast.c

+-------------------------------------------+ 
|   Downstream rate limit function          |  
+-------------------------------------------+ 
 ~ Description:
    Example of how to use BCM APIs to implement downstream rate limit function in pon appplication service    
    - Create Scheduler
    - Create VOQ Connector
    - Create VOQ
    - Set PON application configuration
    - Enable pon application service

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_dn_ratelimit.c

+------------------------------------------------------+
|   Downstream rate limit and PFC Rx function          |
+------------------------------------------------------+
 ~ Description:
    Example of how to use BCM APIs to implement downstream rate limit function and
  configure PFC Rx processing for 10G and 1G channels in pon appplication service
    - Create Scheduler
    - Create VOQ Connector
    - Create VOQ
    - Set PON application configuration
    - Enable pon application service
    - Set PON qos mapping
    - Enable PFC reception on PON port

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_dn_ratelimit_dml.c
    src/examples/dpp/pon/cint_pon_pfc_inbnd_receive.c
    src/examples/dpp/pon/cint_pon_cos_remark_llid_dml.c

+----------------------------------------+ 
|   Upstream scheduler function          |  
+----------------------------------------+ 
 ~ Description:
    Example of how to use BCM APIs to implement upstream scheduler function in pon appplication service    
    - Create SP/WRR/SP+WRR Scheduler
    - Create VOQ Connector
    - Create VOQ
    - Set PON application configuration
    - Enable pon application service

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_up_scheduler.c
    
+-----------------------------------+ 
|   CPU Rx and Tx function          |  
+-----------------------------------+ 
 ~ Description:
    Example of how to use BCM APIs to implement CPU Rx and Tx function in pon appplication service    
    - Set PON application configuration
    - Enable pon application service
    - Rx PPPOE by PMF
    - Rx DHCP/IGMP/MLD by switch port control
    - Tx packet on port
    - Tx packet on lag.

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_cpu_rx_tx.c
    

+--------------------------------+ 
|   MAC move per port            |    
+--------------------------------+ 
 ~ Description:
    Example of MAC move base PON or NNI port  
     - Set MAC limiting mode by soc property 
     - Create pon application service
     - Set MAC limiting base Tunnel ID 
     - Set MAC move base PON or NNI port     

 ~ File Reference:
    src/examples/dpp/cint_port_tpid.c
    src/examples/dpp/pon/cint_pon_utils.c
    src/examples/dpp/pon/cint_pon_mact_move.c

+--------------------------------+ 
|   Forward group AC             |    
+--------------------------------+ 
 ~ Description:
     Example of use forward group AC and PMF for DPoE IP-HSD application 
     - Set PON application configuration
     - Set PON DPoE IP-HSD application service
     - Set FG to update L2 lookup result with service flows in downstream	 

 ~ File Reference:
    src/examples/dpp/pon/cint_pon_dml_fec_app.c

+---------------------+
| OAM Eth.            |
+---------------------+

~ Description:
        Example of Ethernet OAM: 
        - Creating groups
        - Creating local and remote MEPs
        - Get and delete endpoints and groups
        - Injecting packets
        - Registrating events
              
~ File Reference:
        src/examples/dpp/cint_oam.c


+--------------------+
| OAM over LAG       |
+--------------------+

~ Description:
        Example of creating ethernet OAM over LAG:
        - Creating a lag
        - Creating Ethernet port over the lag
        - Creating OAM group
        - Creating MEP
        - Adding all lag ports to the MEP

~ File Reference:
        src/examples/dpp/cint_oam_endpoint_over_lag.c

+------------------------------------------+
| OAM Acceleration Advanced Features       |
+------------------------------------------+

~ Description:
        Example of Ethernet OAM with additional functionalities available in Arad Plus: 
        - Creating groups
        - Creating local (accelerated) and remote MEPs
        - Create additional LM/DM entries per accelerated endpoint.
        - Create Global Loopback entry and Loopback entries per accelerated endpoint
              
~ File Reference:
        src/examples/dpp/cint_oam_acceleration_advanced_features.c

+-------------------------------------+
|OAM over Eth over PWE over Eth       |
+-------------------------------------+

~ Description:
        Example configuration for OAMoEthernetoPWEoEth.
        Includes P2P and P2MP.
        - Create VPLS with default values.
        - Add vswitch cross connect (if P2P has been selected)
        - Create OAM group, OAM endpoint

~ File Reference:
       src/examples/dpp/cint_oam_cfm_o_eth_o_pwe_o_eth.c

+--------------------------------------------+
|OAM endpoint connected to VPLS tunnel       |
+--------------------------------------------+

~ Description:
        Example of creating Ethernet OAM endpoint connected to VPLS tunnel.
        - Create LSP for cross conected ports.
        - Create PW port.
        - Create OAM group, OAM endpoint

~ File Reference:
       src/examples/dpp/cint_oam_up_mep_over_vpls.c

+---------------------+ 
| BFD                 |
+---------------------+
     
~ Description:   
        Example of BFD:        
        - Creating IPV4 endpoint     
        - Creating MPLS endpoint    
        - Creating PWE endpoint       
        - Destroying endpoints   
        - Registrating events     
     
~ File Reference:
         src/examples/dpp/cint_bfd.c

+---------------------+ 
| BFD IPV6            |
+---------------------+
     
~ Description:   
        Example of BFD:        
        - Creating IPV6 endpoint     
        - Geting IPV6 endpoint    
        - Destroying endpoints   
        - Trapping BFD IPV6 PMF
        - Sending BFD IPV6 traffic     
     
~ File Reference:
         src/examples/dpp/cint_bfd_ipv6.c 
         src/examples/dpp/cint_bfd_ipv6_max_sessions.c

+---------------------+ 
| BFD Over LAG        |
+---------------------+
     
~ Description:   
        Example of BFD over LAG:        
        - Creating LAG across multiple devices.
        - Creating BFD endpoints on server and client sides
     
~ File Reference:
         src/examples/dpp/cint_bfd_over_lag.c 


+------------------------------+
| OAM Y1731 over tunnel        |
+------------------------------+

~ Description:
        Example of Y1731 OAM over MPLS-TP, PWE: 
        - Configuring MPLS-TP/PWE tunnel
        - Creating groups
        - Creating local and remote MEPs
        - Injecting packets
        - Registrating events
              
~ File Reference:
        src/examples/dpp/cint_oam_y1731_over_tunnel.c


+---------------------+
| OAM MIM             |
+---------------------+

~ Description:
        Example of OAM MIM: 
        - Creating group
        - Creating local MIM endpoint
              
~ File Reference:
        src/examples/dpp/cint_oam_mim.c

+--------------------------------------------------+
| Reflector (RFC-2544 - benchmarking methodology)   |
+--------------------------------------------------+

~ Description:
        Example of using reflector u-code functionality:
        - Setup egress field processor rule modifying out-tm-port for all traffic arriving on given port

              
~ File Reference:
        src/examples/dpp/cint_benchmarking_methodology.c



+---------------+                                                        
|    ECN        |                                                         
+---------------+                                                         

ECN PP
------

~ Description:
    Example for ECN marking during forwarding and encapsulation. Demonstrate the following:
    - Enabling ECN on the device.
    - Setting extraction of ECN (capable and congestion bits). 
    - Configuring congestion for every packet that is sent to out_port.
    - Configuring MPLS encapsulation and Egress-Remark-profile mapping of {in-DSCP,DP} to out-DSCP. 
    - Testing with traffic and seeing how ECN marking appears in forwarding and encapsulation.

~ File Reference:
    src/examples/dpp/cint_ecn_example.c 

+---------------------+
| 1588                |
+---------------------+

~ Description:
        Examples of 1588 (Precision Time Protocol): 
        - Enabling/Disabling 1588 one/two step TC (Transparent clock) per port.
        - trap/drop/snoop/fwd control over 1588 message types per port.
              
~ File Reference:
        src/examples/dpp/cint_1588.c

+---------------------+
| TIME                |
+---------------------+

~ Description:
        Examples of BroadSync API :
        - BroadSync ToD capture. 
        - Time interface traverse/delete/heartbeat_enable_get. 
              
~ File Reference:
        src/examples/dpp/cint_time.c

+---------------------+
|        EVB          |
+---------------------+

~ Description:
        Examples of EVB (Edge Virtual Bridging): 
        - Edge Virtual Bridge (defined in 802.1Qbg) describes device and protocols connecting between End Stations (VMs) 
        and the Data centers network. 

~ File Reference:
        src/examples/dpp/cint_evb_example.c

+---------------------+
| XGS MAC Extender    |
+---------------------+

~ Description:
        Examples of XGS MAC extender mappings: 
        - In this system ARAD is in device-mode PP and connected to several XGS devices usually as 1G Port extedner. Cint
        provides an example of setting for ARAD

~ File Reference:
        src/examples/dpp/cint_xgs_mac_extender_mappings.c

+---------------------+
| INITIAL VID         |
+---------------------+

~ Description:
        Examples of initial-VID:
        - By default, bcm API differs between tagged packets and untagged packets by Initial-VID database.
        - CINT introduce per port decision if to act as default or always to use Initial-VID with no difference
          between untagged packets and tagged packets. Database used in this case are match MATCH_PORT_VLAN and 
          MATCH_PORT_VLAN_VLAN (no use for MATCH_PORT_INITIAL_VLAN).
~ File Reference:
        src/examples/dpp/cint_vlan_port_initial_vid.c      
        
+---------------------+
| Local Route         |
+---------------------+

~ Description:
        Examples of local route:
        - Support the local route on pon port means pon upstream traffic can go back to other pon.
        - Support the local route based on VSI.
        - Support the local route split-horizon filter.
~ File Reference:
        src/examples/dpp/pon/cint_pon_local_route.c

+---------------------------+
| System vswitch resources  |
+---------------------------+

~ Description:
        Example of System resources in local mode.
        - All CINTs assume global resources for LIF and L2 FEC.
        - Following CINTs provide an example of how to allocate in local mode.
        - Following examples are provided: VLAN-Port, MPLS-Port.
~ File Reference:
        src/examples/dpp/cint_system_vswitch.c
        src/examples/dpp/cint_system_vswitch_vpls.c
 

+---------------------------+
| Stateful Load balancing   |
+---------------------------+

~ Description:
        Example of Stateful Load balancing configuration.
~ File Reference:
        src/examples/dpp/cint_stateful_load_balancing.c
 

+---------------+                                                        
|    IGMP       |                                                         
+---------------+                                                         

~ Description:
        Example for IGMP application, 
        - IGMP packet snooping
        - IGMP multicast 

~ File Reference:
    cint_igmp_example.c


+------------------+                                                        
|    GALoPWEoLSP   |                                                         
+------------------+                                                         

~ Description:
        Example of setup for supporting GALoPWEoLSP packets trapping.
        The packets are trapped with MplsUnexpectedNoBos trap.
        The PWE lif appears in the PPH header.

~ File Reference:
     cint_gal_o_pwe_o_mpls.c
     cint_field_pwe_gal_lif_update.c

+-----------------------------------+                                                        
|    Compensation  Configuration    |                                                         
+-----------------------------------+                                                         

~ Description:
        Configuring compensation example both for ingress and egress.

~ File Reference:
        cint_compensation_setting.c
        
+------------------------+
|       L2 Traverse      |   
+------------------------+
~ Description:
    Example for L2 traverse:
    - Traverse all entries in the l2 table by bcm_l2_traverse.
    - Traverse matching entries in the l2 table in the l2 table by bcm_l2_matched_traverse.
    - Traverse matching entries with flexible mask in the l2 table in the l2 table by bcm_l2_match_masked_traverse.

~ File Reference:
    src/examples/dpp/cint_l2_traverse.c


+--------------------------------+                                                        
|    routing over Vxlan          |                                                         
+--------------------------------+                                                         

~ Description:
    Example for routing over Vxlan UC and MC

~ File Reference:
    cint_vxlan_roo.c
    cint_vxlan_roo_mc.c
    src/examples/dpp/utility/cint_utils_roo.c


+--------------------------------+
| VSwitch VPLS with ECMP         |
+--------------------------------+

~ Description:
        Example of Virtual Switch that contains VPLS with ECMP.
~ File Reference:
        src/examples/dpp/cint_vswitch_vpls_ecmp.c

+--------------------------------+
| VSwitch VPLS Routing Over PWE  |
+--------------------------------+

~ Description:
        Example of Virtual Switch that with Routing Over PWE example.
~ File Reference:
        src/examples/dpp/cint_vswitch_vpls_roo.c
        src/examples/dpp/utility/cint_utils_roo.c

+--------------------------------+
| Dynamic port                   |
+--------------------------------+
        
~ Description:
        1. Example how to properly add dynamic ports.
        2. Example of adding default ports.
~ File Reference:
        1. src/examples/dpp/cint_dynamic_port_add_remove.c
        2. src/examples/dpp/cint_dynamic_ports_init_example.c

+--------------------------------+                                                        
|    Routing over AC             |                                                         
+--------------------------------+                                                         

~ Description:
    Example for Routing over AC   
     
~ File Reference:
    src/examples/dpp/cint_route_over_ac.c 
    src/examples/dpp/utility/cint_utils_global.c
    src/examples/dpp/utility/cint_utils_port.c
    src/examples/dpp/utility/cint_utils_l2.c
    src/examples/dpp/utility/cint_utils_l3.c
    src/examples/dpp/utility/cint_utils_vlan.c
    src/examples/dpp/cint_port_tpid.c

+--------------------------------+
|    Routing over Extender       |
+--------------------------------+

~ Description:
    Example for Routing over extender port

~ File Reference:
    src/examples/dpp/cint_route_over_extender.c
    src/examples/dpp/utility/cint_utils_global.c
    src/examples/dpp/utility/cint_utils_port.c
    src/examples/dpp/utility/cint_utils_l2.c
    src/examples/dpp/utility/cint_utils_l3.c
    src/examples/dpp/utility/cint_utils_extender.c
    src/examples/dpp/utility/cint_utils_vlan.c
    src/examples/dpp/cint_port_tpid.c

+--------------------------------+                                                        
|    Mesh system                 |                                                         
+--------------------------------+                                                         

~ Description:
    Example of configuration in Mesh system:
     - setting modid group
     - configure link topology
     - configure multicast replication (in Mesh MC) 
     
~ File Reference:
    src/examples/dpp/cint_fabric_mesh.c 

+---------------------------------+
| Port extender UC Control Bridge |
+---------------------------------+

~ Description:
    Example of a Port Extender Control Bridge configuration for UC traffic:
     - Configure the device to be a Control Bridge and prepend attributes.
     - Configure a VSI.
     - Attach Cascaded ports to the VSI.
     - Configure Port Extender LIFs.
     - Attach end stations to the VSI.

~ File Reference:
    src/examples/dpp/cint_port_extender_cb_uc.c 

+------------------------------------+
| Port extender L2 MC Control Bridge |
+------------------------------------+

~ Description:
    Example of a Port Extender Control Bridge configuration for L2 Multicast traffic:
    - Configure the device to be a Control Bridge and prepend attributes
    - Configure a VSI and attach Cascaded Ports to it
    - Configure Unicast and Multicast Port Extender LIFs
    - Create L2 Multicast Group
    - Add Port Extender CUDs (MC OutLif) to the Multicast Group
    - Add Unicast and Multicast MAC entries

~ File Reference:
    src/examples/dpp/utility/cint_utils_global.c
    src/examples/dpp/utility/cint_utils_port.c
    src/examples/dpp/utility/cint_utils_l2.c
    src/examples/dpp/utility/cint_utils_vlan.c
    src/examples/dpp/utility/cint_utils_extender.c
    src/examples/dpp/cint_port_extender_cb_l2_mc.c

+------------------------------------+
| Port extender L3 MC Control Bridge |
+------------------------------------+

~ Description:
    Example of a Port Extender Control Bridge configuration for L3 Multicast traffic:
    - Configure the device to be a Control Bridge and prepend attributes
    - Configure Port Extender LIFs
    - Configure VSI per each Port Extender and attach the Ports to it
    - Create L3 Multicast Group
    - Create L3 Interface per each Port Extender
    - Add Port Extender L2 (OutLif) and L3 (OuRif) CUDs to the Multicast Group
    - Add Routing Table Entry

~ File Reference:
    src/examples/dpp/utility/cint_utils_global.c
    src/examples/dpp/utility/cint_utils_port.c
    src/examples/dpp/utility/cint_utils_l2.c
    src/examples/dpp/utility/cint_utils_vlan.c
    src/examples/dpp/utility/cint_utils_extender.c
    src/examples/dpp/cint_port_extender_cb_l3_mc.c

+--------------------------------+
| Port extender mapping          |
+--------------------------------+

~ Description:
        Example of setting ingress PP port mapping via user define value.
~ File Reference:
        src/examples/dpp/cint_port_extender_mapping.c

+--------------------------------+
| Initial QoS mapping            |
+--------------------------------+

~ Description:
       Example of the initial qos application based on port
~ File Reference:
       src/examples/dpp/cint_qos_initial.c

+--------------------------------+
|        QOS  MSPW               |
+--------------------------------+

~ Description:
       Example of cos mapping for MS PWE Transit Node
~ File Reference:
       src/examples/dpp/cint_qos_mspw.c
 
+--------------------------------+
| Enable TLV workaround for Arad+|
+--------------------------------+

~ Description:
        Example of programming the PMF to enable a workaround for port/interface status TLV
        in OAM CCM packets received from Remote MEPs
~ File Reference:
        src/examples/dpp/cint_field_oam_bfd_advanced.c

+---------------------------------+
| Port extender dynamic switching |
+---------------------------------+

~ Description:
        Example of the port extender dynamic switching for COE ports.
~ File Reference:
        src/examples/dpp/cint_port_extender_dynamic_switching.c

+--------------------------------+
|        L3 VPN                  |
+--------------------------------+

~ Description:
       Example of L3VPN application:
       In CINT following traffic scenarios: 
       1. Route Unicast traffic 
       2. mVPN application (multicast IPMC traffic) 
       3. For 88670_A0: we demonstrate how to configure hierarchical fec for L3VPN. 
~ File Reference:
       1. src/examples/dpp/cint_l3vpn.c
       2. src/examples/dpp/cint_mvpn.c
       3. src/examples/dpp/cint_l3vpn.c

---------------------------------+
| BFD ipv4 single hop extended |
+---------------------------------+

~ Description:
        PMF stage in BFD ipv4 single hop extended solution 
~ File Reference:
        src/examples/dpp/cint_field_bfd_ipv4_single_hop.c

-----------------------------------------------+
| BFD ipv4 single hop remote detect multiplier |
+----------------------------------------------+

~ Description:
        PMF stage in BFD ipv4 single hop extended solution with remote detect multiplier 
~ File Reference:
        src/examples/dpp/cint_field_bfd_ipv4_single_hop_remote_detect_compare.c
        
----------------------------------+
|       BFD ipv6 single hop       |
+---------------------------------+

~ Description:
        PMF stage in BFD ipv6 single hop  solution 
~ File Reference:
        src/examples/dpp/cint_field_bfd_ipv6_single_hop.c

---------------------------------+
| Oam per mep statistics         |
+--------------------------------+

~ Description:
        PMF stage in OAM per mep statistics solution 
~ File Reference:
        src/examples/dpp/cint_field_oam_statistics.c

---------------------------------+
| MTU Fileting                   |
+--------------------------------+

~ Description:
        Example of MTU filtering configuration
~ File Reference:
        src/examples/dpp/cint_field_mtu_filtering.c
	src/examples/dpp/cint_egr_mtu.c

---------------------------------+
| mLDP                           |
+--------------------------------+
~ Description:
        Example of IP-GRE L3VPN which show BCM886XX being PE-based mldp.
~ File Reference:
        src/examples/dpp/cint_mldp.c
	
+---------------------------------+
|       Additional TPIDs          |
+---------------------------------+

~ Description:
        Example of set Jericho additional TPIDs to enable additional TPID 
        lookup for port.
~ File Reference:
        src/examples/dpp/cint_additional_tpids.c 


+--------------------------------+                                                        
|    routing over Trill          |                                                         
+--------------------------------+                                                         

~ Description:
    Example for routing over Trill. 

~ File Reference:
    cint_trill_roo.c



+--------------------------------+                                                        
|  Hierarchical protection VPLS  |                                                         
+--------------------------------+                                                         

~ Description:
    Example for VPLS hierarchical protection. 

~ File Reference:
    cint_vswitch_vpls_hvpls.c

+--------------------------------+
|  3level protection VPLS  |
+--------------------------------+

~ Description:
    Example for VPLS 3level protection.

~ File Reference:
    cint_vswitch_vpls_3level_protection.c

+------------------+                                                        
|  MPLS protection |                                                        
+------------------+                                                         

~Description:
    Example of MPLS protection 
~File Reference:
    cint_mpls_protection.c

+--------+                                                        
|  MPLS  |                                                        
+--------+                                                         

~Description:
    Examples of varios MPLS scenarios 
~File Reference:
    cint_mpls_various_scenarios.c

+--------------------------------+                                                        
|  E2E symmetic connection       |                                                         
+--------------------------------+                                                         

~ Description:
    Examaple for building a symmetric scheduling scheme

~ File Reference:
    cint_e2e_symmetric_connection.c

+---------------------------------------------------+
|  Two-Way Active Measurement Protocol (TWAMP)      |
+---------------------------------------------------+

~ Description:
    Examaple for handling TWAMP

~ File Reference:
    cint_twamp.c

+---------------------+
|  Auto-Negotiation   |
+---------------------+

~ Description:
    Examaple for auto-negotiation on jericho

~ File Reference:
    cint_autoneg.c
    
+----------------------------------------------------------+
| L2 encapsulation of a packet punted to external CPU      |
+----------------------------------------------------------+

~ Description:
    Example for configuration of L2 encapsulated punt packet 

~ File Reference:
    src/examples/dpp/cint_l2_encap_external_cpu.c

+-----------------------------+
|  Two level ECMP with MPLS   |
+-----------------------------+

~ Description:
    Examaple for hirarchical ECMPs each to select MPLS tunnel

~ File Reference:
    cint_mpls_ecmp.c

+-------------------------------------------------+
|  RFC2544 Reflector - 1-Pass programmable usage  |
+-------------------------------------------------+

~ Description:
    Example for enabling and programming a reflector, using a user defined
    criteria for reflection, that does not use a 2nd cycle through the device.

~ File Reference:
    cint_field_reflector_1pass.c

+--------------------------------+
|    AC 1:1 Protection           |
+--------------------------------+

~ Description:
    Example of configuration in AC 1:1 Protection:
     - Packets can be sent from the In-AC towards the Out-ACs.
     - UC - Unicast traffic is achieved by using the FEC Protection and sending a packet with a known DA.
     - MC - Multicast Protection is achieved by defining a MC group an using Egress Protection for unknown DA packets.

~ File Reference:
    src/examples/dpp/cint_ac_1to1_coupled_protection.c
    
+--------------------------------+
|           ELK ACL              |
+--------------------------------+

~ Description:
    Example of configuration in ELK ACL:
     - Indicate via SOC property that the KBP device is connected and can be used for external lookups.
     - Indicate if the IPv4 routing tables (including IPv4 Unicast with and without RPF lookups, and IPv4 Multicast lookups) are located either in BCM88650 or in KBP via SOC properties.
     - Initialize the BCM88650 device.
     - Define the Field Groups:
        - In the Forwarding stage, define a 5-tuples of IPv4 for the IPv4 Unicast packets (both bridged and routed). This lookup is the third lookup in the KBP device. The lookup hit indication and the lookup result are passed to the ingress field processor (FP) stage.
        - In the ingress FP stage, define a Field Group preselecting only on the same IPv4 unicast packets and on the hit indication. If matched, define the lookup result as meter value (direct extraction field group).

~ File Reference:
    src/examples/dpp/cint_field_elk_acl.c

+--------------------------------+
|         ELK IPV4 DC            |
+--------------------------------+

~ Description:
    Example of configuration in ELK IPV4 DC:
     - Indicate via SOC property that the KBP device is connected and can be used for external lookups.
     - Indicate if the IPv4 DC routing table are located either in BCM88650 or in KBP via SOC properties.
     - Initialize the BCM88650 device.
     - Add entries to DC routing table

~ File Reference:
    src/examples/dpp/cint_ipv4_dc.c.c

+------------------------------------------------------------------+
|       MAC extender application in Higig TM and PP mode           |
+------------------------------------------------------------------+

~ Description:
    Example of MAC extender application in Higig TM and PP mode:
     - IXIA port3-7 send PP traffic and mapped to xe22(pp port) on QMX side.
     - IXIA port3-5 send TM traffic and mapped to xe21(tm port) on QMX side.
     - IXIA port3-6 send normal traffic to xe13 and out of chip by Higig port xe21.
     - PP traffic is swithed to xe13 through MC while TM traffic forwarded to xe13 according to ITMH(0x01000d00)

~ File Reference:
    src/examples/dpp/cint_xgs_mac_extender_mappings_tm.c

+------------------------------------------------+
|       pmf stage in bfd echo solution           |
+------------------------------------------------+

~ Description:
    Example of configuration for pmf stage in bfd echo solution:
     - Enable soc property bfd_echo_enabled=1 custom_feature_bfd_echo_with_lem=1 (together with all other oam soc properties).
     - Add PMF rule using cint: cint_field_bfd_echo.c function:bfd_echo_with_lem_preselector.
     - Add bfd endpoint by calling bcm_bfd_endpoint_create with: flags |= BCM_BFD_ECHO & BCM_BFD_ENDPOINT_MULTIHOP.

~ File Reference:
    src/examples/dpp/cint_field_bfd_echo.c

+--------------------------------------------------------------+
|                              FIELD SNOOP                     |
+--------------------------------------------------------------+

~ Description:
    Example of filtering all packets that are IPv4/6 over MPLS:
     -  Set a Snoop profile to send to some destination local port. (Alternatively, a system port can be the destination of the mirror port).
     -  We receive as an output the snoop gport. This gport will be the parameter of snoop action

~ File Reference:
    src/examples/dpp/cint_field_snoop_example.c

+------------------------------------------------------------------+
|                       MPLS LIF Counter                           |
+------------------------------------------------------------------+

~ Description:
    Example of MPLS LIF Counter:
     - set the counting source and lif range of lif counting profile
     - set up the mpls service and get the inlif_gport or outlif_gport, and also do the shaping or qos according to the stat_mode
     - associate the inlif_gport or outlif_gport to the counting profile
     - send the packet
     - get 32-bit counter value for specific statistic type

~ File Reference:
    src/examples/dpp/cint_mpls_lif_counter.c

+------------------------------------------------------------------+
|                ECMP meter resolution to FEC member               |
+------------------------------------------------------------------+

~ Description:
    Example of ECMP meter resolution to FEC member:
     - simulate the ECMP meter resolution to FEC member performaed by Arad+ and Jericho and runs independently(!) from the SDK
     - based on cint_trunk_ecmp_lb_key_and_member_retrieve.c which is depented on the SDK

~ File Reference:
    src/examples/dpp/cint_ecmp_hash_resolve.c


+------------------------------------------------------------------+
|                       VLAN VSI Membership                        |
+------------------------------------------------------------------+

~ Description:
    Example of VLAN VSI Membership:
     - Ingress vlan membership filter with drop packet trap
     - Ingress vlan membership filter with send to cpu packet trap
     - Egress vsi membership filter - enable
     - Egress vsi membership filter - disable

~ File Reference:
    src/examples/dpp/cint_vlan_vsi_membership.c

+------------------------------------------------------------------+
|            BFD Endpoints Time Measurement                        |
+------------------------------------------------------------------+

~ Description:
    Example of BFD Endpoints Time Measurement:
     - Time measurement for create and remove BFD endpoints

~ File Reference:
    src/examples/dpp/cint_bfd_performance_test.c

+------------------------------------------------------------------+
|            MPLS NOP action in EEDB                               |
+------------------------------------------------------------------+

~ Description:
    Example of using MPLS NOP action:
     - Add EEDB SWAP Lif
     - Add EEDB NOP Lif
     - Add PMF rule to edit OutLif=NOP and EEI.Outlif=swap
     - Packet sent is label swapped

~ File Reference:
    src/examples/dpp/cint_mpls_nop_action_encapsulation.c

+------------------------------------------------------------------+
|            OAM OAMP Server                                       |
+------------------------------------------------------------------+

~ Description:
    Example of using OAMP server:
     - Using two units, one for the server and one for the client 
     - Create VLAN-ports and connect it
     - Adding Down and Up MEPs on each unit 

~ File Reference:
    src/examples/dpp/cint_oam_server.c

+------------------------------------------------------------------+
|            Push MPLS GRE                                         |
+------------------------------------------------------------------+

~ Description:
    Example of IP routing to MPLS + IP-GRE tunnel encapsulation 

~ File Reference:
    src/examples/dpp/cint_push_mpls_gre.c

+------------------------------------------------------------------+
|            VLAN Tag Acceptance                                   |
+------------------------------------------------------------------+

~ Description:
    Example of egress tag acceptance:
     - Enable egress tag acceptance
     - Add vlan to discard list
     - Only in advanced mode: set packet untagged after EVE

~ File Reference:
    src/examples/dpp/cint_vlan_egress_tag_acceptance.c

+------------------------------------------------------------------+
|            Egress VLAN Membership                                |
+------------------------------------------------------------------+

~ Description:
    Example of VLAN membership:
     - Enable egress vlan memebership filter
     - Add port to vlan membership list
     - Remove port from vlan membership list

~ File Reference:
    src/examples/dpp/cint_vlan_egress_membership.c

+------------------------------------------------------------------+
|            VLAN Tag Acceptance                                   |
+------------------------------------------------------------------+

~ Description:
    Example of egress tag acceptance:
     - Enable egress tag acceptance
     - Add vlan to discard list
     - Only in advanced mode: set packet untagged after EVE

~ File Reference:
    src/examples/dpp/cint_vlan_egress_tag_acceptance.c

+------------------------------------------------------------------+
|            Egress VLAN Membership                                |
+------------------------------------------------------------------+

~ Description:
    Example of VLAN membership:
     - Enable egress vlan memebership filter
     - Add port to vlan membership list
     - Remove port from vlan membership list

~ File Reference:
    src/examples/dpp/cint_vlan_egress_membership.c

+------------------------------------------------------------------+
|            Outbound Mirroring                                    |
+------------------------------------------------------------------+

~ Description:
    Example of outbound mirroring and BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED flag:
     - Enable Outbound Mirroring
     - set BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED flag

~ File Reference:
    src/examples/dpp/cint_vlan_mirror_outbound_untagged.c

+------------------------------------------------------------------+
|            Speculative parsing of headers over MPLS              |
+------------------------------------------------------------------+

~ Description:
    Example for utilizing the field APIs to filter all packets that
    are IPv4/6 over MPLS

~ File Reference:
    src/examples/dpp/cint_field_mpls_speculative_parse.c

+------------------------------------------------------------------+
|            MPLS Additional Labels                                |
+------------------------------------------------------------------+

~ Description:
    Example for encapsulation deep MPLS stack

~ File Reference:
    src/examples/dpp/cint_ip_route_tunnel_mpls_additional_labels.c

+-------------------------------------------------------------+
|            VPLS Tagged Mode                                 |
+-------------------------------------------------------------+

~ Description:
    Example for configuring ingress forwarding of VPLS tagged mode 
       - Use cross connect to connect between 2 port and create P2P service
       - Use flexible connect to connect existing mpls port p2p service
         with user defined vlas to newly created destination port

~ File Reference:
    src/examples/dpp/cint_vswitch_vpws_tagged.c


+-------------------------------------------+
|  Customer specific  tunnel encapsulation  |
+-------------------------------------------+

~ Description:
    Example for customer specific tunnel encapsulation. In this scenario, 
  DUNE will work as a pure PP, it will add customer format TM header and 
  PP header to original packet when working as ingress module, and it will
  process customer format TM header and PP header when working as egress
  module.

~ File Reference:
    cint_connect_to_np_vpws.c
    cint_nph_egress.c


+--------------------------------+
|  1+1 protection VPLS  |
+--------------------------------+

~ Description:
    Example for VPLS 1+1  protection. In this scenario, 2 PWE works as a 
  1+1 protection group(in a multicast group).In ingress, a protection path
  will decide which PW should receive packets, In Egress, traffic will be
  replicated to 2 PWE.

~ File Reference:
    cint_vswitch_vpls_1plus1_protection.c


+------------------+
|  SER             |
+------------------+

~ Description:
    Example for customer trap SER packets. Configure trap in Ingress if 
  SER occurs and send packet to CPU.

~ File Reference:
    cint_ser.c

