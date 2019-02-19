/* $Id: cint_evpn.c,v 1.15 Broadcom SDK $
 $Copyright: Copyright 2015 Broadcom Corporation.
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
 *
 * File: cint_evpn.c
 * Purpose: Example of a EVPN application. 
 */

/* 
 *  EVPN core diagram:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                            .  .  . . .      . . .   . . .                                            |
 *  |                                          .            .  .       .       .                                           |
 *  |   Access                               .                                  .                                          |                       
 *  |                                      .                MPLS Core            . .                                       |
 *  |                                    .   /+-----+  -   -   -   -   -   -+-----+  .                                     |             
 *  |   +--------+                     .      | P1  | \                 /   | P3  | \                                      | 
 *  |  /        /|                    .   /   |     |                       |     | \  .                                   | 
 *  | +--------+ |                   .        +-----+   \             /     +-----+   \  .                                 |
 *  | |  CE1   | |\                 .  /       / |                             |      \   .                                |
 *  | |        |/  \    +--------+ .                      \         /                   \   .+--------+\                   |
 *  | +--------+\ esi  /        /| /        /    |                             |       \    /        /|                    |
 *  |             200\+--------+ |                          \     /                       \+--------+ | \                  |
 *  |             \   |  PE1   | |      /        |                             |        \  |   PE3  | |                    |
 *  |                 |        |/ \                          \ /                           |        |/   \                 |
 *  |               \ +--------+     /           |                             |         \/+--------+                      |
 *  |                 /             \                        / \                                          \                |
 *  |                             /              |                             |        / \           esi    +--------+    |
 *  |               /  \+--------+    \                    /     \                         +--------+ 100   /        /|    |
 *  |                  /        /|\              |                             |      /   /        /|    / +--------+ |    |
 *  |             /   +--------+ |      \                /         \                     +--------+ |      |  CE2   | |    |
 *  |   +--------+ esi|  PE2   | |. \            |                             |    /    |   PE4  | |  /   |        |/     |
 *  |  /        /| 300|        |/  .      \            /             \                   |        |/       +--------+      |
 *  | +--------+ |  / +--------+    . \          |                             |  /     /+--------+  /                     |
 *  | |  CE3   | | /                 .      \+-----+  /                 \   +-----+       .                                |
 *  | |        |/                      .\    | P2  |                        | P4  |   / .                                  |
 *  | +--------+ /                      . \  |     |/                     \ |     |    .                                   |
 *  |                                   .   \+-----+  -   -   -   -   -  -  +-----+ / .                                    |
 *  |                                     .                                   . .  .                                       |
 *  |                                      .               . .       .      .                                              |
 *  |                                        .  .   .  .  .   .  .  . . . .                                                |
 *  |                                                                                                                      |
 *  |                                                                                                                      |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  
 *  Configuration:
 *  
 *  Soc properties:
 *  
 *  bcm886xx_roo_enable=1
 *  evpn_enable=1
 *  roo_extension_label_encapsulation=1
 *  bcm886xx_pph_learn_extension_disable=0
 *  
 *  BCM shell + cint calls (start from BCM shell):
 *  Pass add_frr_label=1 for adding a frr label to mpls stack, otherwise pass add_frr_label=0.
 *  field_processor_evpn_example() sets pmf configurations.
 *  
 * cint;                                                                  
 * cint_reset();                                                          
 * exit;  
 * cint ../../../../src/examples/dpp/utility/cint_utils_global.c   
 * cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c                      
 * cint ../../../../src/examples/dpp/utility/cint_utils_mpls.c                     
 * cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c                
 * cint ../../../../src/examples/dpp/utility/cint_utils_port.c                     
 * cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../../../src/examples/dpp/cint_field_evpn.c                          
 * cint ../../../../src/examples/dpp/cint_evpn.c               
 * cint                                                                   
 * int unit = 0; 
 * int rv = 0; 
 * int ce1_port = 200; 
 * int ce3_port = 201;  
 * int p1_port = 202;  
 * int p2_port = 203;  
 * evpn_add_frr_label = 1;
 * evpn_nof_linker_layer_tags=1;
 * rv = evpn_pe_config(unit,ce1_port,ce3_port,p1_port,p2_port); 
 * print rv; 
 * rv = field_processor_evpn_example(unit); 
 * print rv; 
 *  
 *  
 *  EVPN scenarios configured in this cint:
 *
 *  1)  Global configurations for pe:
 *      1.1) Configure vsi for this application. will be used for the flooding domain of the application.
 *      1.2) Open a muilticast group that will serve as the flooding domain of the application. Receives
 *           as a parameter the above configured vsi.
 *      1.3) Configure port properties for this application.
 *
 *  2)  Create ingress pe known unicast (locally learned destination).                                               
 *      Packet arrives from ce1, destined for ce3. The mac address
 *      for ce3 was previously learned at pe1                                                                                                             
 *      2.1) Create in ac for ce1.                                   
 *      2.2) Create out ac for ce3.                                                             
 *                                                                                          
 *  3)  Create ingress pe known unicast traffic with remote learning.
 *      Packet arrives from ce1, destined for ce2. The mac address
 *      for ce2 was previously learned at pe3 and distributed to pe1 via BGP.
 *      Packet exits pe1 with evpn over lsp over (optional) frr label.
 *      3.1) Create in ac for ce1.
 *      3.2) Create a (outgoing) rif with pe1's my_mac facing p1.
 *      3.3) Create egress tunnel: evpn over lsp. Set the tunnel to point to the above created rif.
 *      3.4) Create a fec pointing to this tunnel.
 *      3.5) Create a MACT entry with ce2's mac, the egress tunnel's gport as destination.
 *      3.6) Set LL information (next hop for the above created tunnel).
 *      3.7) Optional: Add FRR label to be placed on top of the mpls stack.
 *
 *  4)  Create ingress pe bum traffic: receive a packet from ce1
 *      with an unknown DA. Replicate the packet with a MC group (flooding domain) comprising of:
 *      1. A copy to ce3, which belongs to esi 300, of which pe1 is the DF.
 *      2. Two copies with IML over lsp over (optional) frr label, for remote pes not attached to ce1's esi.
 *      3. One copy with ESI over IML over lsp over (optional) frr label for pe2, which is attached to ce1's esi.   
 *      4.1) Create in ac for ce1.
 *      4.2) Set additional data for ce1's ac for pmf processing.
 *      4.3) Set PMF inlif profile for ce1's ac.
 *      4.4) Create ac for ce3 (add it to flooding domain).
 *      4.5) Create a (outgoing) rif with pe's my_mac facing p1.
 *      4.6) Create egress tunnels: two with IML over lsp, one with esi over iml over lsp. Set them
 *          to point to the link layer entry.
 *      4.7) Set LL information (next hop for the above created tunnels).
 *      4.8) Set additional data for the tunnel of pe1->pe2 (for esi prepending).
 *      4.9) Optional: Add FRR label to be placed on top of the mpls stack (all three copies; FRR labels vary according to the copy).
 *
 *  5)  Create egress pe known unicast. Pe1 receives a packet destined for ce3, SA==ce2_mac.
 *      ce3's mac was learned by pe3 from pe1 via BGP. Packet is EVPN over LSP.
 *      Terminate the tunnels and forward to ce3 (pe1 is ce3' DF).
 *      5.1) Create out ac for ce3.
 *      5.2) Create in ac for p1.
 *      5.3) Create ingress tunnel (for termination) with EVPN ovel LSP.
 *
 *  6)  Create egress pe bum traffic. Handle two cases:
 *      1. Send a packet with IML over LSP from pe3, with unknown destination. Packet should
 *      be flooded to ce3 (pe1 is it's DF) and to remote pes (will be pruned due to orientation). In fact,
 *      only the packet to ce3 will exit the device.
 *      2. Send a packet with ESI over IML over LSP from pe2, with unknown destination.
 *      Packet should be flooded to ce1 (will be filtered due to same esi value as in ESI label) and to
 *      remote PEs (will be pruned due to orientation).
 *      6.1) Create out ac for ce3.
 *      6.2) Create in ac for p1 and an in ac for p2.
 *      6.3) Create a (outgoing) rif with pe's my_mac facing p1.
 *      6.4) Create egress tunnels: two with IML over lsp, one with esi over iml over lsp. Set them
 *           to point to the link layer entry (eventually they will be pruned due to orientation).
 *      6.5) Set LL information (next hop for the above created tunnels).
 *      6.6) Set additional data for the tunnel of pe1->pe2 (for esi prepending).
 *      6.7) Set IML label range.
 *      6.8) Create 4 ingress tunnels (representing 2 label stacks) :
 *           1. for pe2->pe1 create one tunnel with lsp,
 *             the other with IML (and an indication for ESI).
 *           2. for pe3->pe1 create one tunnel with lsp, the other with IML (without
 *              an indication for ESI.
 *
 *  7)  Create unicast routing into evpn core: Packet arrives from ce1 as unicast ipv4 over ethernet, destined for ce2.
 *      Packet exits pe1 with ipv4 over evpn over lsp over (optional) frr label.
 *      7.1) Create in ac for ce1.
 *      7.2) Create a rif (incoming + outgoing linker layer).
 *      7.3) Create a rif (outgoing native).
 *      7.4) Create LL information (linker layer), to pointed by the egress tunnel.
 *      7.5) Create egress tunnel: evpn over lsp. Set the tunnel to point to the above created linker layer.
 *      7.6) Create an overlay fec (cascaded) pointing to this tunnel.
 *      7.7) Create a native fec pointing to the overlay fec.
 *      7.8) Create routing entry pointing to native fec.
 *      7.9) Optional: Add FRR label to be placed on top of the mpls stack.
 *      
 *  8)  Create multicast routing into evpn core: Packet arrives from ce1 as multicast ipv4 over ethernet.
 *      Packet exits pe1 with 4 copies:
 *      1. A copy to ce3, which belongs to esi 300, of which pe1 is the DF (falls back to bridge).
 *      2. Two copies with IML over lsp over (optional) frr label, for remote pes not attached to ce1's esi.
 *      3. One copy with ESI over IML over lsp over (optional) frr label for pe2, which is attached to ce1's esi (falls back to bridge).
 *      8.1)  Create in ac for ce1.
 *      8.2)  Create out ac for ce3.
 *      8.3)  Set PMF inlif profile for ce1's ac.
 *      8.4)  Create a rif (incoming + outgoing linker layer).
 *      8.5)  Create a rif (outgoing native).
 *      8.6)  Enable in rif for multicast.
 *      8.7)  Create egress tunnels: two with IML over lsp, one with esi over iml over lsp. Set them
 *            to point to the link layer entry.
 *      8.8)  Open multicast group and add members to the group (each copy contains outrif and outlif. Packets
 *            that fall back to bridge entail in rif == out rif.
 *      8.9)  Create a routing entry with the multicast group as destination.
 *      8.10) Set additional data for the tunnel of pe1->pe2 (for esi prepending).
 *      8.11) Optional: Add FRR label to be placed on top of the mpls stack.
 * 
 * 
 *  Traffic:
 * 
 *  Send traffic according to the above scenarios. We assume evpn_add_frr_label=1 , evpn_nof_linker_layer_tags=1.
 * 
 *  ############################################## 
 *  Scenario 1: Global configurations. No traffic.
 *  ##############################################
 * 
 * 
 *  ################################################################## 
 *  Scenario 2: Ingress pe known unicast (locally learned destination)
 *  ##################################################################
 * 
 *  Sending packet from port == 200:   -----------> 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:14|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:14|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 *  ################################################################# 
 *  Scenario 3: Ingress pe known unicast traffic with remote learning
 *  #################################################################
 * 
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:13|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5200||Label:8000||01:13|01:12|0x8100|20 ||    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 *  ##################################
 *  Scenario 4: Ingress pe bum traffic
 *  ##################################
 * 
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:17|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (First copy) on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |01:16|11:00:00:01:11||0x8100|20 ||Label:9100||Label:5100||Label:6100||Label:200 ||01:17|01:12|0x8100|20 ||    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||TTL:1     ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Second copy) on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5300||Label:6300||01:17|01:12|0x8100|20 ||    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Third copy) on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5200||Label:6200||01:17|01:12|0x8100|20 ||    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Fourth copy) on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:17|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 *  ###################################
 *  Scenario 5: Egress pe known unicast
 *  ###################################
 * 
 *  Sending packet from port == 202:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:5200||Label:8000||01:14|01:13|0x8100|20 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:60    ||TTL:60    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:14|01:13||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 *  #################################
 *  Scenario 6: Egress pe bum traffic
 *  #################################
 * 
 *  Sending packet from port == 202:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:5200||Label:6200||01:17|01:13|0x8100|20 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:32    ||TTL:32    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:17|01:13||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *   Sending packet from port == 203:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:16||0x8100|20 ||Label:5100||Label:6100||Label:300 ||01:17|01:14|0x8100|20 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:60    ||TTL:20    ||TTL:1     ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *   Receiving: None (all exiting packets are filtered).
 * 
 * 
 *  #####################################
 *  Scenario 7: Unicast routing into evpn
 *  #####################################
 * 
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    | DA  | SA  ||TPID1 |VID||      SIP      |      DIP      |TTL|Data|
 *   |    |01:13|01:12||0x8100|20 ||160.161.161.163|160.161.161.162|64 |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||      SIP      |      DIP      |TTL|Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5200||Label:6200||01:13|01:18|0x8100|30 ||160.161.161.163|160.161.161.162|63 |    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||               |               |   |    |
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||     |     |      |   ||               |               |   |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 *  #######################################
 *  Scenario 8: Multicast routing into evpn
 *  #######################################
 * 
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    | DA              | SA  ||TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:00:5E:00:01:02|01:12||0x8100|20 ||224.0.1.1|224.0.1.2|64 |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (First copy) on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   ||   MPLS   || DA              | SA  |TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:16|11:00:00:01:11||0x8100|20 ||Label:9100||Label:5100||Label:6100||Label:200 ||01:00:5E:00:01:02|01:12|0x8100|20 ||224.0.1.1|224.0.1.2|64 |    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||Exp:0     ||                 |     |      |   ||         |         |   |    |
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||TTL:1     ||                 |     |      |   ||         |         |   |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Second copy) on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA              | SA              |TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5300||Label:6300||01:00:5E:00:01:02|00:11:00:00:01:18|0x8100|30 ||224.0.1.1|224.0.1.2|63 |    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||                 |                 |      |   ||         |         |   |    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||                 |                 |      |   ||         |         |   |    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Third copy) on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA              | SA              |TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5200||Label:6200||01:00:5E:00:01:02|00:11:00:00:01:18|0x8100|30 ||224.0.1.1|224.0.1.2|63 |    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||                 |                 |      |   ||         |         |   |    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||                 |                 |      |   ||         |         |   |    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Fourth copy) on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    | DA              | SA  ||TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:00:5E:00:01:02|01:12||0x8100|20 ||224.0.1.1|224.0.1.2|64 |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
   
/* Global variable that holds the bit for iml_bos_indication */   
int evpn_iml_bos_indication = 1<<25;
/* Global variable that determines whether to add a frr label on top of the encapsulation stack */ 
int evpn_add_frr_label = 0;

/* Will hold all ports of the application */
int evpn_port_array[4];

/* Number of vlan tags in linker layer entries of this application */
int evpn_nof_linker_layer_tags = 0;
                                            
struct cint_evpn_info_s {
    int ce1_port; /* ce1 port (access)*/
    int ce3_port; /* ce3 port (access) */
    int p1_port;  /* p1 port (provider) */
    int p2_port;  /* p2 port (provider) */
    int ce1_vlan; /* ce1's vlan */
    int ce2_vlan; /* ce2's vlan */
    int ce3_vlan; /* ce3's vlan */
    int p1_vlan;  /* p1's vlan */
    int p2_vlan;  /* p2's vlan */
    int native_vlan;  /* native vlan */
    int p1_inner_vlan; /* p1's inner vlan*/
    int p2_inner_vlan; /* p2's inner vlan*/
    int tpid;     /* tpid value for all vlan tags in the application*/
    int vsi; /* vsi representing the service in this application */
    int pe2_id; /* pe2's id */
    int pe1_pe2_ce3_esi_label; /* The esi of pe1, pe2, ce3 */
    int pe1_pe2_ce1_esi_label; /* The esi of pe1, pe2, ce1 */
    bcm_mac_t global_mac; /* global mac of PE1 */
    bcm_mac_t my_mac_lsb; /* PE1's my mac lsb facing p1 */
    bcm_mac_t my_mac; /* PE1's my mac facing p1 */
    bcm_mac_t my_mac2; /* PE1's my mac facing p1 */
    bcm_mac_t ce1_mac;  /* ce1's mac address*/
    bcm_mac_t ce2_mac; /* ce2's mac address*/
    bcm_mac_t ce3_mac ; /* ce3's mac address*/
    bcm_mac_t p1_mac; /* p1's mac address*/
    bcm_mac_t p2_mac; /* p2's mac address*/
    bcm_mac_t pe3_mac; /* pe3's mac address*/
    bcm_if_t pe1_rif; /* rif of flooding domain */
    bcm_if_t pe1_rif2; /* rif of flooding domain */
    bcm_gport_t p1_vport_id; /* vlan port id of p1's AC */
    bcm_gport_t p2_vport_id; /* vlan port id of p2's AC */
    bcm_gport_t ce1_vport_id; /* vlan port id of ce1's AC */
    bcm_gport_t ce3_vport_id; /* vlan port id of ce3's AC */
    bcm_mpls_label_t evpn_label; /* EVPN label for ingress and egress pe */
    bcm_mpls_label_t pe1_pe2_lsp_label; /* lsp label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_lsp_label; /* lsp label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe4_lsp_label; /* lsp label in pe1<-->pe4 */
    bcm_mpls_label_t pe1_pe2_iml_label; /* iml label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_iml_label; /* lsp label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe4_iml_label; /* lsp label in pe1<-->pe4 */
    bcm_mpls_label_t iml_range_low_label; /* low bound of iml range in egress pe */
    bcm_mpls_label_t iml_range_high_label; /* high bound of iml range in egress pe */
    bcm_mpls_label_t pe1_frr_label; /* frr (fast re-route) label for tunnel starting at pe1  */
    bcm_mpls_label_t pe2_frr_label; /* frr (fast re-route) label for tunnel starting at pe2  */
    uint32 evpn_profile; /* user defined evpn profile : 0-7 */
    bcm_ip_t ce2_ip; /* ce2's ip */ 
    bcm_ip_t mc_ip;  /* multicast ip for multicast routing into evpn */
    int pe1_linker_layer_encap_id; /* Linker layer encap id for pe1 */
    int pe2_linker_layer_encap_id; /* Linker layer encap id for pe2 */
    int pe1_pe3_iml_no_esi_tunnel_id; /* Egress Tunnel id of pe1<-->pe3 iml over lsp (no esi) */
    int pe1_pe4_iml_no_esi_tunnel_id; /* Egress Tunnel id of pe1<-->pe4 iml over lsp (no esi) */
    int pe1_pe2_iml_esi_tunnel_id;    /* Egress Tunnel id of pe1<-->pe2 iml over lsp (with esi) */
    int pe1_pe3_evpn_lsp_tunnel_id;   /* Egress Tunnel id of pe1<-->pe3 evpn over lsp */
    bcm_gport_t pe1_pe2_iml_label_term_tunnel_id; /* Tunnel id of pe1_pe2_iml terminated label */
    int pe1_pe3_evpn_lsp_fec;         /* Overlay FEC pointing to evpn over lsp tunnel */ 
    int pe1_ce2_native_fec;           /* Native FEC pointing to Overlay FEC pointing to evpn over lsp tunnel*/
    bcm_multicast_t mc_id;            /* multicast id for routing into evpn bum traffic copies*/    


};


cint_evpn_info_s cint_evpn_info = 
/* ports :
   ce1_port | ce3_port | p1_port | p2_port  */
    {200,      201,        202 ,     203,               
/* vlans:
   ce1_vlan | ce2_vlan | ce3_vlan | p1_vlan | p2_vlan | native_vlan | p1_inner_vlan | p2_inner_vlan */
    20,          20,        20,        20,      20,          30,           40,              50,
/* tpid */
   0x8100,
/* vsi */
    20, 
/* pe2_id */
    100,
/* pe1_pe2_ce3_esi_label    |  pe1_pe2_ce1_esi_label */ 
            300,                       200,     
/*               global_mac              |            my_mac_lsb             |               my_mac                | */              
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x11}, {0x00, 0x11, 0x00, 0x00, 0x01, 0x11},
/*                 my_mac2                             ce1_mac                               ce2_mac                 */
    {0x00, 0x11, 0x00, 0x00, 0x01, 0x18}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x12}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x13},
/*                 ce3_mac                              p1_mac                                p2_mac                 */     
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x14},{0x00, 0x00, 0x00, 0x00, 0x01, 0x15}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x16}, 
/*                 pe3_mac              */
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x17},
/*  pe1_rif | pe1_rif2 */ 
       0,        0,
/* p1_vport_id | p2_vport_id | ce1_vport_id | ce3_vport_id */
        0,            0 ,           0,              0, 
/* evpn_label | pe1_pe2_lsp_label | pe1_pe3_lsp_label | pe1_pe4_lsp_label */                                                                                                                 
       8000,          5100,               5200,               5300,
/* pe1_pe2_iml_label | pe1_pe3_iml_label | pe1_pe4_iml_label | iml_range_low_label */ 
         6100,               6200,               6300,                6000,
/* iml_range_high_label | pe1_frr_label | pe2_frr_label */                                                                         
       7000,                  828300,            828600, 
/* evpn_profile */
        1,
/*   ce2_ip  */
   0xA0A1A1A2 /* 160.161.161.162 */,
/*   mc_ip   */ 
   0xE0000103 /* 224.0.1.3 */,
/* pe1_linker_layer_encap_id | pe2_linker_layer_encap_id*/
              0,                          0, 
/* pe1_pe3_iml_no_esi_tunnel_id | pe1_pe4_iml_no_esi_tunnel_id | pe1_pe2_iml_esi_tunnel_id | pe1_pe3_evpn_lsp_tunnel_id | pe1_pe2_iml_label_term_tunnel_id */
              0,                              0,                            0,                            0,                              0,
/* pe1_pe3_evpn_lsp_fec */
             0,
/* pe1_ce2_native_fec */
             0,
/* mc_id */
   40960,
};



void evpn_info_init(int unit,
                    int ce1_port,
                    int ce3_port, 
                    int p1_port,
                    int p2_port
                    ) {
    evpn_port_array[0] = cint_evpn_info.ce1_port = ce1_port; 
    evpn_port_array[1] = cint_evpn_info.ce3_port = ce3_port; 
    evpn_port_array[2] = cint_evpn_info.p1_port = p1_port; 
    evpn_port_array[3] = cint_evpn_info.p2_port = p2_port;
}

/* Main function */
int evpn_pe_config(int unit,
                    int ce1_port,
                    int ce3_port, 
                    int p1_port,
                    int p2_port){

    int rv = BCM_E_NONE;

    evpn_info_init(unit,ce1_port,ce3_port, p1_port, p2_port); 

    /* Configure vsi for this application */
    rv = bcm_vswitch_create_with_id(unit, cint_evpn_info.vsi); /* 1.1 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vswitch_create_with_id with vid = %d\n", cint_evpn_info.vsi);
        return rv;
    }

    /* Create multicast group (flooding for this service).
       multicast id has to be the same one as the vswitch */
    egress_mc = 0;
    rv = multicast__open_mc_group(unit, &cint_evpn_info.vsi, 0); /* 1.2 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in multicast__open_mc_group\n");
        return rv;
    }

    /* Configure port properties for this application */
    rv = evpn_configure_port_properties(unit, evpn_port_array,4); /* 1.3 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_configure_port_properties\n");
        return rv;
    }

    /* Configure L2 information */
    rv = evpn_create_l2_interfaces(unit); /* 2.1, 2.2, 3.1, 4.1, 4.2, 4.3, 4.4, 5.1, 5.2 ,6.1, 6.2, 7.1, 8.1, 8.2, 8.3 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_l2_interfaces\n");
        return rv;
    }

    /* Configure L3information (router interfaces) */
    rv = evpn_create_l3_interfaces(unit); /* 3.2, 4.5, 7.2, 7.3, 8.4, 8.5 */ 
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_l3_interfaces\n");
        return rv;
    }

    /* Create egress tunnels for forwarding */
    rv = evpn_create_egress_tunnels(unit,1); /* 3.3, 3.4, 3.5, 4.6, 4.8, 6.4, 6.5, 6.6, 7.5, 8.7, 8.10 */
    if (rv != BCM_E_NONE) {
         print rv;
         printf("Error, in evpn_create_egress_tunnels\n");
         return rv;
     }
    
    /* Create ingress tunnels for termination*/
    rv = evpn_create_ingress_tunnels(unit); /* 5.3, 6.7, 6.8 */
    if (rv != BCM_E_NONE) {
         print rv;
         printf("Error, in evpn_create_ingress_tunnels\n");
         return rv;
     }

    rv = evpn_routing_into_evpn(unit); /* 7.7, 7.8, 8.6, 8.8, 8.9 */
    if (rv != BCM_E_NONE) {
         print rv;
         printf("Error, in evpn_create_ingress_tunnels\n");
         return rv;
     }

    return rv;
}


/* Create AC, learn it statically, add it to flooding domain */
int evpn_create_ac(int unit, int flags,  int vlan, \
                   int vsi, int port, bcm_gport_t *vport_id, \
                   bcm_mac_t mac_address, int add_to_flooding_domain){

    int rv = BCM_E_NONE;
    l2__mact_properties_s mact_properties;

    /* Create a vlan port*/
    rv = l2__port_vlan__create(unit, flags,  port, vlan, vsi, vport_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__port_vlan__create\n");
        return rv;
    }

    mact_properties.gport_id = *vport_id;
    mact_properties.mac_address = mac_address;
    mact_properties.vlan = vlan;
    /* Create a mact entry */
    rv = l2__mact_entry_create(unit, &mact_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__mact_entry_create\n");
        return rv;
    }
    
    /* add the vlan port to the flooding domain (cud)*/
    if (add_to_flooding_domain) {
        rv = multicast__vlan_encap_add(unit, cint_evpn_info.vsi, port, *vport_id);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in multicast__vlan_encap_add\n");
            return rv;
        }
    }
    return rv;

}
 

/* Create routing interface*/
int evpn_create_l3_interfaces(int unit){

    int rv = BCM_E_NONE;
    create_l3_intf_s ll_intf, native_intf;

    ll_intf.my_lsb_mac = cint_evpn_info.my_mac_lsb;
    ll_intf.my_global_mac = cint_evpn_info.global_mac;
    ll_intf.rif = cint_evpn_info.pe1_rif;
    ll_intf.vsi = cint_evpn_info.vsi;
    ll_intf.rpf_valid = 0;
    /* create rif */
    rv = l3__intf_rif__create(unit, &ll_intf); /* 3.2, 4.5, 6.3, 7.2, 8.4 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__intf_rif__create\n");
        return rv;
    }

    cint_evpn_info.pe1_rif = ll_intf.rif;

    native_intf.my_lsb_mac = cint_evpn_info.my_mac2;
    native_intf.my_global_mac = cint_evpn_info.global_mac;
    native_intf.rif = cint_evpn_info.pe1_rif2;
    native_intf.vsi = cint_evpn_info.native_vlan;
    native_intf.rpf_valid = 0;
    /* create rif */
    rv = l3__intf_rif__create(unit, &native_intf); /* 7.3 , 8.5*/
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__intf_rif__create\n");
        return rv;
    }

    cint_evpn_info.pe1_rif2 = native_intf.rif;

    return rv;

}

/* Set egress tunnel */
int evpn_set_egress_tunnel(int unit, int add_to_flooding_domain, int set_split_horizon, \
                            int flags, bcm_mpls_label_t label_in, bcm_mpls_label_t label_out, \
                            bcm_mpls_egress_action_t egress_action, int port, int *tunnel_id, int out_rif){
    
    int rv = BCM_E_NONE;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    mpls_tunnel_properties.flags = flags;
    mpls_tunnel_properties.label_in = label_in;
    mpls_tunnel_properties.label_out = label_out;
    mpls_tunnel_properties.egress_action = egress_action;
    mpls_tunnel_properties.next_pointer_intf = out_rif;

    /* Create egress tunnel */
    rv  = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    *tunnel_id =  mpls_tunnel_properties.tunnel_id;

    /* Adding tunnel_id to flooding domain (cud)*/
    if (add_to_flooding_domain) {
        rv = multicast__mpls_encap_add(unit, cint_evpn_info.vsi, port, mpls_tunnel_properties.tunnel_id);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in multicast__mpls_encap_add\n");
            return rv;
        }        
    }

    /* configure split horizon for this tunnel */
    if (set_split_horizon) {

        /* Tunnel encoding needed for bcm_port_class_set*/
        BCM_GPORT_TUNNEL_ID_SET(mpls_tunnel_properties.tunnel_id,mpls_tunnel_properties.tunnel_id);

        rv = bcm_port_class_set(unit,mpls_tunnel_properties.tunnel_id, bcmPortClassForwardEgress, 1);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_port_class_set, evpn_set_egress_tunnel\n");
            return rv;
        }
    }

    return rv;
            
}


/* Set ingress tunnel (for termination)*/
int evpn_set_ingress_tunnel(int unit, int set_split_horizon, \
                            int flags, bcm_mpls_label_t label, bcm_mpls_label_t second_label, \
                            bcm_mpls_switch_action_t action, bcm_if_t tunnel_if, bcm_gport_t *tunnel_id){
    
    int rv = BCM_E_NONE;
    mpls__ingress_tunnel_utils_s mpls_tunnel_properties;
    int tmp_tunnel_id = 0;


    mpls_tunnel_properties.flags = flags;
    mpls_tunnel_properties.tunnel_if = tunnel_if;
    mpls_tunnel_properties.vpn = cint_evpn_info.vsi;
    mpls_tunnel_properties.action = action;
    mpls_tunnel_properties.label = label;
    mpls_tunnel_properties.second_label = second_label;
    mpls_tunnel_properties.tunnel_id = 0;

    /* Create ingress tunnel */
    rv  = mpls__add_switch_entry(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    } 

    *tunnel_id = mpls_tunnel_properties.tunnel_id;

    if (set_split_horizon) {

        /* Tunnel encoding need for bcm_port_class_set*/
        tmp_tunnel_id = mpls_tunnel_properties.tunnel_id;
        /* Unset iml_bos_indication (bit 25), if it is set */
        evpn_unset_iml_bos_indication(tmp_tunnel_id);

        /* Set split horizon: orientation==1*/
        rv = bcm_port_class_set(unit,tmp_tunnel_id, bcmPortClassForwardIngress, 1);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_port_class_set\n");
            return rv;
        }
    }

    return rv;
            
}

/* Create egress tunnels with LL information */
int evpn_create_egress_tunnels(int unit,int config_wide_data){

    int rv = BCM_E_NONE;
    uint64 data;
    int encap_id = 0;
    l2__mact_properties_s mact_properties;
    create_l3_egress_s l3_egress;
    int gport_forward_fec = 0;
    int add_to_flooding_domain = 0;

    COMPILER_64_ZERO(data);

    /* Create linker layer entries for tunnels */
    rv = evpn_create_linker_layer_entries(unit); /* 3.6, 3.7, 4.7, 6.5, 7.4*/
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_linker_layer_entries\n");
        return rv;
    }

    /* Create tunnel to pe3, which is not attached to ce1's esi*/
    add_to_flooding_domain = 1;
    /* add_to_flooding_domain = 0;*/
    rv = evpn_set_egress_tunnel(unit, add_to_flooding_domain, 1, BCM_MPLS_EGRESS_LABEL_IML, cint_evpn_info.pe1_pe3_iml_label, cint_evpn_info.pe1_pe3_lsp_label, \ /* 4.6, 6.4, 8.7 */
                            BCM_MPLS_EGRESS_ACTION_PUSH, cint_evpn_info.p1_port, &cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id,cint_evpn_info.pe1_linker_layer_encap_id);

    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_egress_tunnel\n");
        return rv;
    }

    /* Create tunnel to pe4, which is not attached to ce1's esi*/
    add_to_flooding_domain = 1;
    /* add_to_flooding_domain = 0;*/
    rv = evpn_set_egress_tunnel(unit, add_to_flooding_domain, 1, \
                            BCM_MPLS_EGRESS_LABEL_IML, cint_evpn_info.pe1_pe4_iml_label, cint_evpn_info.pe1_pe4_lsp_label, \ /* 4.6, 6.4, 8.7 */
                            BCM_MPLS_EGRESS_ACTION_PUSH, cint_evpn_info.p1_port, &cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id, cint_evpn_info.pe1_linker_layer_encap_id);

    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_egress_tunnel\n");
        return rv;
    }


    /* Create tunnel to pe2, which is attached to ce1's esi*/
    /*add_to_flooding_domain = 0;*/
    add_to_flooding_domain = 1;
    rv = evpn_set_egress_tunnel(unit, add_to_flooding_domain, 1,  \
                            BCM_MPLS_EGRESS_LABEL_WIDE|BCM_MPLS_EGRESS_LABEL_IML, cint_evpn_info.pe1_pe2_iml_label, cint_evpn_info.pe1_pe2_lsp_label, \ /* 4.6, 6.4, 8.7 */
                            BCM_MPLS_EGRESS_ACTION_PUSH, cint_evpn_info.p2_port, &cint_evpn_info.pe1_pe2_iml_esi_tunnel_id, cint_evpn_info.pe2_linker_layer_encap_id);

    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_egress_tunnel\n");
        return rv;
    }


    /* Create egress tunnel with evpn and lsp labels */
    add_to_flooding_domain = 0;
    rv  = evpn_set_egress_tunnel(unit, add_to_flooding_domain, 0,  \ /* 3.3, 7.5 */
                            0,cint_evpn_info.evpn_label , cint_evpn_info.pe1_pe3_lsp_label, \
                            BCM_MPLS_EGRESS_ACTION_PUSH, 0, &cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id, cint_evpn_info.pe1_linker_layer_encap_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_egress_tunnel\n");
        return rv;
    }

    printf("cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id: %d\n",cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id);
    rv = evpn_create_overlay_fec_entry(unit); /* 3.4, 7.6 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__mact_entry_create\n");
        return rv;
    }

    /* Create forwarding entry for DA==ce2_mac. Result is LSP*/
    BCM_GPORT_FORWARD_PORT_SET(gport_forward_fec, cint_evpn_info.pe1_pe3_evpn_lsp_fec);
    mact_properties.gport_id = gport_forward_fec;
    mact_properties.mac_address = cint_evpn_info.ce2_mac;
    mact_properties.vlan = cint_evpn_info.ce2_vlan;
    rv = l2__mact_entry_create(unit, &mact_properties); /* 3.5 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__mact_entry_create\n");
        return rv;
    }

    /* data == PE id*/
    COMPILER_64_SET(data, 0, cint_evpn_info.pe2_id);
    /* Tunnel encoding is needed for bcm_port_wide_data_set */
    BCM_GPORT_TUNNEL_ID_SET(cint_evpn_info.pe1_pe2_iml_esi_tunnel_id,cint_evpn_info.pe1_pe2_iml_esi_tunnel_id);
    rv = bcm_port_wide_data_set(unit, cint_evpn_info.pe1_pe2_iml_esi_tunnel_id,BCM_PORT_WIDE_DATA_EGRESS, data); /* 4.8, 6.6, 8.10 */ 
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_port_wide_data_set with \n");
        return rv;
    }

    if (evpn_add_frr_label) {
        rv = evpn_enable_frr_labels_addition(unit); /* 3.7, 4.9, 7.9, 8.11 */
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in evpn_enable_frr_labels_addition with \n");
            return rv;
        }
    }


    return rv;
}

/* Create ingress tunnels (for termination) for egress pe bum traffic */
int evpn_create_ingress_tunnels(int unit){

    int rv = BCM_E_NONE;
    int flags = 0 ;
    bcm_mpls_range_action_t action;
    bcm_gport_t dummy_tunnel_id;

    /* Setting IML range*/
    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    rv = bcm_mpls_range_action_add(unit,cint_evpn_info.iml_range_low_label, cint_evpn_info.iml_range_high_label,action); /* 6.7 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_mpls_range_action_add\n");
        return rv;
    }


    /* Set LSP ltermination label of pe3-->pe1 */
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.pe1_pe3_lsp_label, 0, \ /* 5.3, 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0, &dummy_tunnel_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Set IML termination label of pe3-->pe1 (no esi expected, indicated by BCM_MPLS_SWITCH_EXPECT_BOS) */
    flags = BCM_MPLS_SWITCH_EVPN_IML|BCM_MPLS_SWITCH_EXPECT_BOS|BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.pe1_pe3_iml_label, 0, \ /* 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0, &dummy_tunnel_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Set LSP termination label of pe2-->pe1 */
    flags = 0;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.pe1_pe2_lsp_label, 0, \ /* 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0, &dummy_tunnel_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Set IML termination label of pe2-->pe1 (esi is expected, indicated by the absence of BCM_MPLS_SWITCH_EXPECT_BOS) */
    flags = BCM_MPLS_SWITCH_EVPN_IML|BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.pe1_pe2_iml_label, 0, \ /* 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0, &cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Set PMF inLif profile*/
    rv = bcm_port_class_set(unit,cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id, bcmPortClassFieldIngress, cint_evpn_info.evpn_profile); 
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_port_class_set \n");
        return rv;
    }


    /* Create ingress tunnel : EVPN over LSP (to be terminated) */
    flags = BCM_MPLS_SWITCH_NEXT_HEADER_L2; /* Set for this inlif ethernet as next protocol */
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.evpn_label, \ /* 5.3 */
                           0,BCM_MPLS_SWITCH_ACTION_POP, BCM_IF_INVALID, &dummy_tunnel_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    }

    return rv;
}

/* Create attachment circuits for this application*/
int evpn_create_l2_interfaces(int unit){

    int rv = BCM_E_NONE;
    uint64 data;
    int add_to_flooding_domain = 0;

    COMPILER_64_ZERO(data);

    rv = evpn_create_ac(unit, BCM_VLAN_PORT_INGRESS_WIDE, \ /* 2.1, 3.1, 4.1, 7.1, 8.1 */
         cint_evpn_info.ce1_vlan, cint_evpn_info.vsi,cint_evpn_info.ce1_port, \
         &(cint_evpn_info.ce1_vport_id), cint_evpn_info.ce1_mac,0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_ac with ce1\n");
        return rv;
    }
    /* Set additional info: PE id + esi*/
    /* data == esi label + PE id and shift by an mpls label size */
    COMPILER_64_SET(data, 0, (cint_evpn_info.pe2_id << 20) | cint_evpn_info.pe1_pe2_ce1_esi_label);
    rv = bcm_port_wide_data_set(unit,cint_evpn_info.ce1_vport_id,BCM_PORT_WIDE_DATA_INGRESS, data); /* 4.2 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_port_wide_data_set\n");
        return rv;
    }
    /* Set PMF inLif profile*/
    rv = bcm_port_class_set(unit,cint_evpn_info.ce1_vport_id, bcmPortClassFieldIngress, cint_evpn_info.evpn_profile); /* 4.3, 8.3 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_port_class_set \n");
        return rv;
    }    

    add_to_flooding_domain = 1;
    rv = evpn_create_ac(unit,0, cint_evpn_info.ce3_vlan, \ /* 2.2, 4.4, 5.1, 6.1, 8.2 */
                       cint_evpn_info.vsi,cint_evpn_info.ce3_port,&(cint_evpn_info.ce3_vport_id), cint_evpn_info.ce3_mac, add_to_flooding_domain);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_and_statically_learn_ac with ce1\n");
        return rv;
    }

    rv = evpn_create_ac(unit,0, cint_evpn_info.p1_vlan, cint_evpn_info.vsi ,cint_evpn_info.p1_port, \ /* 5.2, 6.2 */
         &(cint_evpn_info.p1_vport_id),cint_evpn_info.p1_mac, 0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_ac with ce1\n");
        return rv;
    }

    rv = evpn_create_ac(unit,0, cint_evpn_info.p2_vlan, cint_evpn_info.vsi ,cint_evpn_info.p2_port, \ /*  6.2 */
         &(cint_evpn_info.p2_vport_id),cint_evpn_info.p2_mac, 0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_ac with ce1\n");
        return rv;
    }


    return rv;
}

/* Configure routing into evpn for unicast and multicast traffic */
int evpn_routing_into_evpn(int unit){

    int rv = BCM_E_NONE;
    l3_ipv4_route_entry_utils_s route_entry;
    int cuds[4];
    int ports[4];
    int nof_ports = 0;
    int is_egress = 0;
    multicast_forwarding_entry_dip_sip_s forwarding_entry;
    bcm_l3_intf_t intf;

    /* Configure unicast routing into EVPN :
       1. Create Native FEC, set it to point to overlay FEC pointing to EVPN over lsp.
       2. Create Routing entry pointing to the native FEC.
    */

    /* 1) Create Native FEC */
    rv = evpn_create_native_fec_entry(unit); /* 7.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, in evpn_create_native_fec_entry\n");
        return rv;
    }

    /* 2) Create Routing entry: Add LPM entry for access to EVPN core traffic */
    sal_memset(&route_entry, 0, sizeof(route_entry));
    route_entry.address =  cint_evpn_info.ce2_ip;
    route_entry.mask = 0xffffffff;
    rv = l3__ipv4_route__add(unit, 
                             route_entry,
                             0,     /* router instance */
                             cint_evpn_info.pe1_ce2_native_fec/* data of LPM entry:  fec */        
                             );  /* 7.8 */
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__ipv4_route__add\n");
        return rv;
    }

    /* Configure multicast routing into EVPN :
       1) Enable multicast on inRif.
       2) Open multicast group and add members to the group (Same as flooding members in ingress bum traffic).
       3) Add forwarding entry for the multicast destination ip. Entry wil point to the created multicast group.
    */

    bcm_l3_intf_t_init(&intf);
    intf.l3a_vid = cint_evpn_info.vsi;
    intf.l3a_mac_addr = cint_evpn_info.my_mac;

    /* 1) Enable multicast on inRif. */
    rv = multicast__enable_rif_ipmc(unit, &intf, 1); /* 8.6 */
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__enable_rif_ipmc\n");
        return rv;
    }
    
    /* 2) Open multicast group and add members to the group (each copy contains outrif and outlif) */
    rv = evpn_add_routing_into_evpn_multicast_entry(unit); /* 8.8 */
    if (rv != BCM_E_NONE) {
        printf("Error, in evpn_add_routing_into_evpn_bum_traffic\n");
        return rv;
    }

    /* 3) Add forwarding entry for the multicast destination ip. */
    forwarding_entry.src_ip = -1;
    forwarding_entry.mc_ip = cint_evpn_info.mc_ip;
    rv = multicast__create_forwarding_entry_dip_sip(unit, &forwarding_entry, cint_evpn_info.mc_id, /* inRif*/cint_evpn_info.pe1_rif,0); /* 8.9 */
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__create_forwarding_entry_dip_sip\n");
        return rv;
    }

    return rv;
}
    

/* Unset iml_bos_indication (bit 25), if it is set */
void evpn_unset_iml_bos_indication(int *tunnel_id){

    /* unset bit 25 */
    int iml_bos_indication_unset_mask = 0xfdffffff;
    if (*tunnel_id & evpn_iml_bos_indication) {
        *tunnel_id &= 0xfdffffff;
    }

    return;
}

/* Configure properties for ports of the application */
int evpn_configure_port_properties(int unit, int *port_array, int nof_ports){

    int rv = BCM_E_NONE;
    int i;
    bcm_pbmp_t pbmp;

    for (i=0; i< nof_ports; i++) {
        /* set VLAN domain to each port */
        rv = port__vlan_domain__set(unit, port_array[i], port_array[i]); /* 1.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, in port__vlan_domain__set, port=%d, \n", port_array[i]);
            return rv;
        }

        /* Set outer and inner tpid */
        rv = port__tpid__set(unit, port_array[i], cint_evpn_info.tpid, cint_evpn_info.tpid); /* 1.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, in port__tpid__set, port=%d, \n",  port_array[i]);
            return rv;
        }

        rv = bcm_vlan_gport_add(unit, cint_evpn_info.vsi, port_array[i], BCM_VLAN_GPORT_ADD_INGRESS_ONLY); /* 1.3 */
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan);
          return rv;
        }
    }

    return rv;
}

/* Configure linker layer entries for application: these entries are used
   for both bridging and routing into evpn */
int evpn_create_linker_layer_entries(int unit){

    int rv = BCM_E_NONE;
    l2__arp_entry_properties_s l2_egress_overlay;


    /* Create linker layer for tunnels starting at pe1 */

    l2_egress_overlay.dest_mac   = cint_evpn_info.p1_mac;  /* next hop */
    l2_egress_overlay.src_mac    = cint_evpn_info.my_mac;  /* my-mac */
    l2_egress_overlay.ethertype  = 0x8847;       /* ethertype for MPLS */
    if (evpn_nof_linker_layer_tags > 0) {
        l2_egress_overlay.outer_vlan = cint_evpn_info.p1_vlan; /* egress vlan  */
        l2_egress_overlay.outer_tpid = cint_evpn_info.tpid;
        if (evpn_nof_linker_layer_tags == 2) {
            l2_egress_overlay.inner_vlan = cint_evpn_info.p1_inner_vlan; /* egress inner vlan  */
            l2_egress_overlay.inner_tpid = cint_evpn_info.tpid;
        }
    }

    if (evpn_add_frr_label) {
        l2_egress_overlay.mpls_extended_label_value = cint_evpn_info.pe1_frr_label; /* 3.7, 4.9, 7.9, 8.11 */
    }

    rv = l2__egress_create(unit, &l2_egress_overlay); /* 3.6, 3.7, 4.7, 4.9, 6.5, 7.4, 7.9, 8.11 */
    if (rv != BCM_E_NONE) {
        printf("Error, in l2__egress_create\n");
        return rv;
    }

    cint_evpn_info.pe1_linker_layer_encap_id = l2_egress_overlay.encap_id;

    /* Create linker layer for the tunnel starting at pe2 */

    l2_egress_overlay.dest_mac   = cint_evpn_info.p2_mac;  /* next hop */
    l2_egress_overlay.src_mac    = cint_evpn_info.my_mac;  /* my-mac */
    l2_egress_overlay.ethertype  = 0x8847;       /* ethertype for MPLS */
    if (evpn_nof_linker_layer_tags > 0) {
        l2_egress_overlay.outer_vlan = cint_evpn_info.p2_vlan; /* egress vlan  */
        l2_egress_overlay.outer_tpid = cint_evpn_info.tpid;
        if (evpn_nof_linker_layer_tags == 2) {
            l2_egress_overlay.inner_vlan = cint_evpn_info.p2_inner_vlan; /* egress inner vlan  */
            l2_egress_overlay.inner_tpid = cint_evpn_info.tpid;
        }
    }
    if (evpn_add_frr_label) {
        l2_egress_overlay.mpls_extended_label_value = cint_evpn_info.pe2_frr_label; /* 4.9, 8.11 */
    }

    rv = l2__egress_create(unit, &l2_egress_overlay); /* 4.7, 4.9, 6.5, 8.11 */
    if (rv != BCM_E_NONE) {
        printf("Error, in l2__egress_create\n");
        return rv;
    }

    cint_evpn_info.pe2_linker_layer_encap_id = l2_egress_overlay.encap_id;

    return rv;
}


/* Create a FEC entry pointing to  an egress evpn entry. */
int evpn_create_overlay_fec_entry(int unit){

    int rv = BCM_E_NONE;

    create_l3_egress_s l3_egress;

   /* Set LL for egress tunnel */
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY | BCM_L3_CASCADED;
    l3_egress.l3_egress_id = 0;
    l3_egress.encap_id = 0;
    l3_egress.gport = cint_evpn_info.p1_port;
    l3_egress.vlan = cint_evpn_info.p1_vlan;
    l3_egress.intf = cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id;
    rv = l3__egress__create(unit, &l3_egress);/* 3.4, 7.6 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__egress__create\n");
        return rv;
    }

    cint_evpn_info.pe1_pe3_evpn_lsp_fec = l3_egress.l3_egress_id;;

    return rv;

}

/* Create a native fec entry for unicast routing into evpn */
int evpn_create_native_fec_entry(int unit){

    int rv = BCM_E_NONE;
    bcm_failover_t failover_id_fec;
    create_l3_egress_s l3_egress_into_overlay;
    int gport_forward_fec = 0;

    /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
     * In case we are using hierarchical FEC, the first FEC must be protected.
     */

    /* create failover id*/
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create 1\n");
        return rv;
    }

    BCM_GPORT_FORWARD_PORT_SET(gport_forward_fec, cint_evpn_info.pe1_pe3_evpn_lsp_fec);

    /* Create protected FEC */
    l3_egress_into_overlay.l3_flags = 0;  /* flags  */
    l3_egress_into_overlay.gport = gport_forward_fec; /* dest: fec-id instead of port */
    l3_egress_into_overlay.vlan = cint_evpn_info.native_vlan; /* out-vlan */
    l3_egress_into_overlay.intf = cint_evpn_info.pe1_rif2; /* l3_intf  */
    l3_egress_into_overlay.next_hop_mac_addr = cint_evpn_info.ce2_mac; /* next-hop */
    l3_egress_into_overlay.failover_id = failover_id_fec;
    l3_egress_into_overlay.failover_if_id = 0;

    rv = l3__egress__create(unit, &l3_egress_into_overlay); /* 7.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    /* primary FEC */
    l3_egress_into_overlay.failover_if_id = l3_egress_into_overlay.l3_egress_id;
    l3_egress_into_overlay.encap_id = 0;

    rv = l3__egress__create(unit, &l3_egress_into_overlay); /* 7.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    cint_evpn_info.pe1_ce2_native_fec = l3_egress_into_overlay.l3_egress_id;

    return rv;
}


/* Enable addition of frr labels on top of the egress tunnels configured in this application */
int evpn_enable_frr_labels_addition(int unit){

    int rv = BCM_E_NONE;

    int tunnel_id;

    BCM_GPORT_TUNNEL_ID_SET(tunnel_id,cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id);
    /* Enabling addition of frr label on top of pe1_pe3_iml_no_esi tunnel*/
    rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1); /* 4.9, 8.11 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    BCM_GPORT_TUNNEL_ID_SET(tunnel_id,cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id);
    /* Enabling addition of frr label on top of pe1_pe4_iml_no_esi tunnel*/
    rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1); /* 4.9, 8.11 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    BCM_GPORT_TUNNEL_ID_SET(tunnel_id,cint_evpn_info.pe1_pe2_iml_esi_tunnel_id);
    /* Enabling addition of frr label on top of pe1_pe2_iml_esi tunnel*/
    rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1); /* 4.9, 8.11 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    BCM_GPORT_TUNNEL_ID_SET(tunnel_id,cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id);
    /* Enabling addition of frr label on top of pe1_pe3_evpn_lsp tunnel*/
    rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1); /* 3.7, 7.9 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    return rv;

}

/* Open multicast group and add members to the group (each copy contains outrif and outlif) */
int evpn_add_routing_into_evpn_multicast_entry(int unit){

    int rv = BCM_E_NONE;
    bcm_multicast_replication_t reps[4];
    int global_lif_mask = 0x3ffff;

    /* manually change the mc entries. 1st Mc entry with OUTRIF and link it to the 2nd mc entry with outlif */
    bcm_multicast_replication_t_init(reps);

    /* Open the multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP|BCM_MULTICAST_TYPE_L3, &cint_evpn_info.mc_id); /* 8.8 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }

    /* Create two tunnel copies with their tunnel ids (outlif) and outrif (inRif != outRif for routing) */
    BCM_GPORT_LOCAL_SET(reps[0].port, cint_evpn_info.p1_port);
    reps[0].encap1 = cint_evpn_info.pe1_rif2; /* OutRif */
    reps[0].encap2 = cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id & global_lif_mask; /* OutLif */
    reps[0].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;    
    
    BCM_GPORT_LOCAL_SET(reps[1].port, cint_evpn_info.p1_port);
    reps[1].encap1 = cint_evpn_info.pe1_rif2; /* OutRif */
    reps[1].encap2 = cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id & global_lif_mask; /* OutLif */
    reps[1].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF; 
    
    /* Create one tunnel copy with it's tunnel id (outlif) and outrif (inRif == outRif for falling back to bridge) */    
    BCM_GPORT_LOCAL_SET(reps[2].port, cint_evpn_info.p2_port);
    reps[2].encap1 = cint_evpn_info.pe1_rif; /* OutRif */
    reps[2].encap2 = cint_evpn_info.pe1_pe2_iml_esi_tunnel_id & global_lif_mask; /* OutLif */
    reps[2].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF; 
    
    /* Create AC copy with it's vlan port id (outlif) and outrif (pe1_rif)
     This copy will fack back to bridge, due to inRif == OutRif*/
    BCM_GPORT_LOCAL_SET(reps[3].port, cint_evpn_info.ce3_port);
    reps[3].encap1 = cint_evpn_info.pe1_rif; /* OutRif */
    reps[3].encap2 = cint_evpn_info.ce3_vport_id & global_lif_mask; /* OutLif */
    reps[3].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF; 
   
    /* Add the copies to the multicast table */
    rv = bcm_multicast_set(unit, cint_evpn_info.mc_id, 0, 4, reps); /* 8.8 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_set\n");
        return rv;
    }

    return rv;
}
