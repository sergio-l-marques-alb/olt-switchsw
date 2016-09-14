// Project olt7_8ch 
// This software will run on the stand-alone olt boxes of PT Inovacao
// Development started on 20/Aug/2008 
// nuno-f-monteiro@ptinovacao.pt

// This software is provided "AS IS," without a warranty of any kind. 
// This software is not designed or intended for use in on-line control of
// aircraft, air traffic, aircraft navigation or aircraft communications; or in
// the design, construction, operation or maintenance of any nuclear
// facility. Licensee represents and warrants that it will not use or
// redistribute the Software for such purposes
// 
// Copyright (c) 2008 PT Inova��o, SA, All Rights Reserved.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "main.h"
#include "utils.h"
#include "ipc_lib.h"
#include "globals.h"

int canal_buga;

static int convert_ipaddr2uint64(const char *ipaddr, uint64 *value_uint64);
//static int convert_macaddr2uint64(const char *macaddr, uint64 *value_uint64);
static int convert_macaddr2array(const char *macaddr, uint8 *array);

void help_oltBuga(void)
{
  printf(
        "\n\n\r"
        "usar: buga [Opcoes]\n\r"
        "\n\r"
        "Opcoes:\n\r"
        "help <comando>\n\r"
        "ping <period> <N> - Waits until OLTSWITCH application is up or return error code (0-READY, 1-BUSY, 2-LOADING, 3-CRASHED)\r\n"
        "m 1000 console(/dev/...)\n\r"
        "m 1001 file_index(0=main;1=sdk) filename - Logger output\n\r"
        "m 1003 <state> - Set new fw state for switchdrvr (0-READY, 1-BUSY, 2-LOADING, 3-CRASHED)\r\n"
        "m 1004 - Get resources state\r\n"
        "--- Slot/Ports Configurations --------------------------------------------------------------------------------------------------------\n\r"
        "m 1005 - Get current slot map configuration\r\n"
        "m 1006 enable(0/1) port1 port2 ... - Enable PRBS TX/RX machine\r\n"
        "m 1007 port1 port2 ... - Read number of PRBS errors\r\n"
        "m 1008 - Validate provided slot map configuration\r\n"
        "m 1009 - Apply new slot map configuration\r\n"
        "m 1010 p=<port(0-MAX> en=(0/1) speed=(100M-2/1G-3/2.5G-4/10G-5/AN-6) fd=(0/1) fm=<framemax(bytes)> lb=(0/1) macLearn=(0/1) - switch port configuration\n\r"
        "m 1011 port(0-MAX) - get switch port configuration\n\r"
        "m 1012 port(0-MAX) - Get Phy states\n\r"
        "m 1013 slot(2-19) - Apply linkscan procedure\n\r"
        "m 1014 slot(2-19) port(0-3) cmd(0/1) - (Uplink) Protection command\n\r"
        "m 1015 intfType/intf# - Get port type definitions\r\n"
        "m 1016 intf=<type>/<intf#> [defvid=1-4095] [defprio=0-7] [aftypes=0/1] [ifilter=0/1] [rvlanreg=0/1] [vlanaware=0/1] [type=0/1/2] [dtag=0/1]\r\n"
        "       [otpid=xxxxh] [itpid=xxxxh] [etype=0/1/2] [mlen=0/1] [mlsmen=0/1] [mlsmprio=0-7] [mlsmsp=0/1] [trust=0/1] - Set port type definitions\r\n"
        "m 1017 intfType/intf# - Get MAC address of given interface\r\n"
        "m 1018 intfType/intf# macAddr(xx:xx:xx:xx:xx:xx) - Set MAC address for the provided interface\r\n"
        "m 1020 port(0-MAX) - Show switch RFC2819 statistics\n\r"
        "m 1021 port(0-MAX) - Clear switch RFC2819 statistics\n\r"
        "m 1022 port(0-MAX) enable(0/1) - RFC2819 probe configuration\n\r"
        "m 1023 bufferType(0-15min, 1-24hours) - Clear RFC2819 Monitoring buffer\n\r"
        "m 1024 bufferType(0-15min, 1-24hours) startId(0-MAX, -1: Read from first buffer (most recent)) - Read RFC2819 buffers (inverted mode)\n\r"
        "m 1025 port(0-MAX) - Read RFC2819 probe configuration\n\r"
        "m 1026 bufferType(0-15min, 1-24hours) - RFC2819 buffers status\n\r"
        "m 1027 MAC Limiting per interface - [slot] [system] [intf] [limit] [action] [send_trap]\n\r"
        "--- QOS and L2 commandsl --------------------------------------------------------------------------------------------------------------\n\r"
        "m 1030 intfType/intf# - Get QoS configuration\r\n"
        "m 1031 intfType/intf# trustMode(1-Untrust;2-802.1P;3-IPprec;4-DSCP) shapingRate(Mbps) cos_pr0(0-7) cos_pr1 ... cos_pr7 - Set general QoS configuration\r\n"
        "m 1032 intfType/intf# cos(0-7) scheduler(1:Strict;2:Weighted) min_bandwidth(Mbps) max_bandwidth(Mbps) - Set specific QoS configuration\r\n"
        "m 1033 intfType/intf# - Get QoS3 configuration\r\n"
        "m 1034 intfType/intf# trustMode(1-Untrust;2-802.1P;3-IPprec;4-DSCP) [cos_pr0(0-7)] [cos_pr1] ... [cos_pr7] - Set QoS3-interface configuration\r\n"
        "m 1035 intfType/intf# cos(0-7) scheduler(1:Strict;2:Weighted) [weight(1-128)] [shaper(Kbps)] [min_bandwidth(Kbps)] [max_bandwidth(Kbps)] - Set QoS3-cos configuration\r\n"
        "m 1036 intfType/intf# cos(0-7) mgmtType(0/1) [decayExp(0-15)] - Set QoS3-drop configuration\r\n"
        "m 1037 intfType/intf# cos(0-7) dplevel(1-4) [tdThresh(0-100)] [wredMinThresh(0-100)] [wredMaxThresh(0-100)] [wredDropProb(0-100)] - Set QoS3-drop configuration\r\n"
        "m 1038 intfType/intf# cos(0-7) cir(Kbps) eir(Kbps) cbs(bytes) ebs(bytes) - Set QoS3-CoS policer configuration\r\n"
        "m 1040 startId(0..) numEntries - Read MAC table\r\n"
        "m 1041 vlan(1-4095) macAddr(xx:xx:xx:xx:xx:xx) intfType/intf# - Add a static entry to the MAC table\r\n"
        "m 1042 vlan(1-4095) macAddr(xx:xx:xx:xx:xx:xx) - Remove an entry from MAC table\r\n"
        "m 1043 - Flush all entries of MAC table\r\n"
        "m 1044 EVC# dir(uplink:0/downlink:1) remark(0/1) trustMode(0-None;1-Untrust;2-802.1P;3-IPprec;4-DSCP) cos_pr0(0-7) cos_pr1 ... cos_pr7 - Set QoS classification for an EVC\r\n"
        "--- Protocols ------------------------------------------------------------------------------------------------------------------------\n\r"
        "m 1220 EVC# intfType/intf# cvid(1-4095) - Read DHCPop82 profile\n\r"
        "m 1221 EVC# intfType/intf# cvid(1-4095) op82/op37/op18 <circuitId> <remoteId> - Define a DHCPop82 profile\n\r"
        "m 1222 EVC# intfType/intf# cvid(1-4095) - Remove a DHCPop82 profile\n\r"
        "m 1230 EVC# intfType/intf#                                       - DAI statistics\r\n"
        "m 1231 smac_validate(0/1) dmac_validate(0/1) ip_validate(0/1)    - DAI Global configurations\r\n"
        "m 1232 intfType/intf# trust(0/1) rateLimit(pps) burstInterval(s) - DAI Interface configurations\r\n"
        "m 1233 EVC# enable(0/1) static(0/1)                              - DAI EVC configurations\r\n"
        "m 1234 NNIVlanId(1-4094) enable(0/1) static(0/1)                 - DAI (NNI) VLAN configurations\r\n"
        "m 1235 aclName ipAddr(d.d.d.d) macAddr(xx:xx:xx:xx:xx:xx)        - Add an ARP-ACL rule entry\r\n"
        "m 1236 aclName ipAddr(d.d.d.d) macAddr(xx:xx:xx:xx:xx:xx)        - Remove an ARP-ACL rule entry\r\n"
        "m 1237 EVC# [aclName]                                            - Link aclName to EVC id (no string to unlink)\r\n"
        "m 1238 NNIVlanId(1-4094) [aclName]                               - Link aclName to NNI VLAN (no string to unlink)\r\n"
        "m 1240 page(0..)                  - Read DHCP binding table (start reading from page 0)\r\n"
        "m 1242 macAddr(xx:xx:xx:xx:xx:xx) - Remove a MAC address from DHCP Binding table\r\n"
        "m 1310 EVC# intfType/intf# [ovid(0-4095)] [cvid(1-4095)] - Show IGMP statistics for interface <type>/<id> and client <cvid> associated to EVC <flow_id>\n\r"
        "m 1312 EVC# intfType/intf# [ovid(0-4095)] [cvid(1-4095)] - Clear IGMP statistics for interface<type>/<id> and client <cvid> associated to EVC <flow_id>\n\r"
        "m 1320 EVC# intfType/intf# [cvid(1-4095)] - Show DHCP statistics for interface <type>/<id> and client <cvid> associated to EVC <flow_id>\n\r"
        "m 1322 EVC# intfType/intf# [cvid(1-4095)] - Clear DHCP statistics for interface<type>/<id> and client <cvid> associated to EVC <flow_id>\n\r"
        "m 1400 [admin=0/1] [ipaddr=d.d.d.d] [cos=0..7] [gmi=<group_membership_interval>] [qi=<querier_interval>] - Configure igmp snooping + querier\r\n"
        "m 1401 MC_EVC# UC_EVC# - Add IGMP instance with the MC+UC evc's pair\r\n"
        "m 1402 MC_EVC# UC_EVC# - Remove IGMP instance with the MC+UC evc's pair\r\n"
        "m 1403 MC_EVC# start_index(0..) - Get list of IGMP channel-associations\r\n"
        "m 1404 MC_EVC# groupAddr(d.d.d.d) sourceAddr(d.d.d.d) groupMaskBits(22-32) sourceMaskBits(22-32) - Add IGMP channel-associations\r\n"
        "m 1405 MC_EVC# groupAddr(d.d.d.d) sourceAddr(d.d.d.d) groupMaskBits(22-32) sourceMaskBits(22-32) - Remove IGMP channel-associations\r\n"
        "m 1406 MC_EVC# intfType/intf# ovid(0-4095) cvid(0-4095) - Add MC client to IGMP instance\r\n"
        "m 1407 MC_EVC# intfType/intf# ovid(0-4095) cvid(0-4095) - Remove MC client to IGMP instance\r\n"
      /*"m 1400 snooping_admin[0/1] querier_admin[0/1] querier_ipaddr[ddd.ddd.ddd.ddd] querier_inerval[1-1800] cos[0-7] - IGMP snooping admin mode\r\n"
        "m 1402 port1[0-15] type1[0=client,1=router] port2 type2 ... - IGMP snooping: add client interfaces\r\n"
        "m 1403 port1[0-15] port2 ...   - IGMP snooping: remove interfaces\r\n"
        "m 1405 vlan1[2-4093] xlate1[2-4093] vlan2 xlate2 ... - IGMP snooping: add mclient vlans and their translation values\r\n"
        "m 1406 vlan1[2-4093] vlan2 ... - IGMP snooping: remove mclient vlans\r\n"
        "m 1408 port[0-15] vlan1[2-4093] vlan2 ...     - IGMP snooping: add mrouter vlans for the given port\r\n"
        "m 1409 port[0-15] vlan1[2-4093] vlan2 ...     - IGMP snooping: remove mrouter vlans for the given port\r\n"
        "m 1411 vlan1[2-4093] ipaddr1[ddd.ddd.ddd.ddd] vlan2 ipaddr2 ... - IGMP snooping querier: Add vlan and its ip address\r\n"
//      "m 1412 vlan1[2-4093] vlan2 ... - IGMP snooping querier: remove vlans\r\n"*/
        "m 1420 EVC# page_idx(0..) intfType/intf# [svid(1-4095)] [cvid(1-4095/0)] - List active channels for a particular EVC and client\r\n"
        "m 1421 EVC# page_idx(0..) ipchannel(d.d.d.d) ipSource(d.d.d.d) - List clients watching a channel (ip) associated to this EVCid\r\n"
        "m 1430 EVC# ipchannel(d.d.d.d) sourceAddr(d.d.d.d) - Add static MC channel\r\n"
        "m 1431 EVC# ipchannel(d.d.d.d) sourceAddr(d.d.d.d) - Remove static MC channel\r\n"
        "--- LAGs -----------------------------------------------------------------------------------------------------------------------------\n\r"
        "m 1500 lag(0-MAX) - Get LAG configurations\r\n"
        "m 1501 lag(0-MAX) static_mode(0/1) load_balance(0-6) port_bmp(xxxxxh) - Create LAG\r\n"
        "m 1502 lag(0-MAX) - Destroy LAG\r\n"
        "m 1503 lag(0-MAX) - Get LAG status\r\n"
        "m 1504 port[0..MAX/-1) lacp_aggregation(0/1) lacp_activity(0/1) lacp_timeout(0=long,1=short) - Set LACP Admin State\r\n"
        "m 1505 port[0..MAX/-1) - Get LACP Admin State\r\n"
        "m 1510 port[0..MAC/-1) - Show LACP statistics for port <port>\n\r"
        "m 1511 port[0..MAX/-1) - Clear LACP statistics for port <port>\n\r"
        "m 1997 - Reset Multicast machine\r\n"
        "m 1998 - Reset alarms\r\n"
        "m 1999 - Reset defaults, except for lag InBand\r\n"
        "m 2000 age(30-1000000) - Set MAC Learning table aging time\r\n"
        "m 2001 - Get MAC Learning aging time\r\n"
        "--- EVCs -----------------------------------------------------------------------------------------------------------------------------\r\n"
        "m 1600 EVC# - Read EVC config\r\n"
        "m 1601 EVC# Type(0-P2MP;1-P2P;2-Q) Stacked(0/1) MacLearn(0/1) Mask(010h-CPUtrap;100h-DHCP;200h-IGMP) MCFlood(0-All;1-Unknown;2-None)\r\n"
        "       intfType/intf#/mef(0-Root;1-Leaf)/VLAN/iVlan ... - Create EVC\r\n"
        "m 1602 EVC# - Delete EVC\r\n"
        "m 1603 EVC# intfType/intf#/mef(0-Root;1-Leaf)/VLAN/iVlan ... - Add ports to EVC\r\n"
        "m 1604 EVC# intfType/intf# ... - Remove ports from EVC\r\n"
        "m 1605 EVC# intfType intf# Out.VLAN Inn.VLAN - Add P2P bridge on Stacked EVCs between the root and a leaf intf\r\n"
        "m 1606 EVC# intfType intf# Inn.VLAN - Deletes P2P bridge on Stacked EVCs between the root and a leaf intf\r\n"
        "m 1607 EVC# intfType intf# Out.VLAN Inn.VLAN CVlan flags(01h:DHCP;02h:IGMP;04h:PPPoE) MaxMACs - Add a GEM flow to an EVC\r\n"
        "m 1608 EVC# intfType intf# Out.VLAN - Delete a GEM flow from an EVC\r\n"
        "m 1609 EVC# [type] [mc_flood] [flags_value] [flags_mask] - Change EVC options\r\n"
        "m 1610 - Reads Network Connectivity (inBand) configuration\r\n"
        "m 1611 intfType intf# ipaddr(d.d.d.d) netmask(d.d.d.d) gateway(d.d.d.d) managememt_vlan - Sets Network Connectivity (inBand) configuration\r\n"
        "m 1620 evc=<evc#> intf=<type>/<intf#> [svid=1-4095] [cvid=1-4095] - Get Profile data of a specific Bandwidth Policer\r\n"
        "m 1621 evc=<evc#> intf=<type>/<intf#> [svid=1-4095] [cvid=1-4095] cir=<mbps> eir=<mbps> cbs=<bytes> ebs=<bytes> - Create/reconfig bandwidth Policer\r\n"
        "m 1622 evc=<evc#> intf=<type>/<intf#> [svid=1-4095] [cvid=1-4095] - Delete bandwidth Policer\r\n"
        "m 1624 [bc=<bps>] [mc=<bps>] [uc=<bps>] - Storm control configuration\r\n"
        "m 1625 [bc=0] [mc=0] [uc=0]             - Storm control reset\r\n"
        "m 1626 [bc=0] [mc=0] [uc=0]             - Storm control clear\r\n"
        "m 1627 intfType/intf# en(0/1) lim(0-?) units(0:PPS/1:%%/2:KBPS) - Broadcast stormcontrol\r\n"
        "m 1628 intfType/intf# en(0/1) lim(0-?) units(0:PPS/1:%%/2:KBPS) - Multicast stormcontrol\r\n"
        "m 1629 intfType/intf# en(0/1) lim(0-?) units(0:PPS/1:%%/2:KBPS) - Unknown Unicast stormcontrol\r\n"
        "m 1630 evc=<evc#> intf=<type>/<intf#> [svid=1-4095] [cvid=1-4095] [channel=d.d.d.d] - Show absolute evc statistics\n\r"
        "m 1632 evc=<evc#> intf=<type>/<intf#> [svid=1-4095] [cvid=1-4095] [channel=d.d.d.d] - Add evc statistics measurement\n\r"
        "m 1633 evc=<evc#> intf=<type>/<intf#> [svid=1-4095] [cvid=1-4095] [channel=d.d.d.d] - Remove evc statistics measurement\n\r"
        "--- IP Source Guard ------------------------------------------------------------------------------------------------------------------\n\r"
        "m 1700 intfType/intf# enable(0/1) - Enable/Disable IP Source Guard on Ptin Port\n\r"
        "m 1701 idType(1-eEVCId;2-rootVLAN) iD(1-131071) intfType/intf# mac(xx:xx:xx:xx:xx:xx) ip(d.d.d.d) removeOrAdd(0/1) - Add/Remove IP Source Guard Entry on Ptin Port of Extended EVC Id\n\r"
        "--- IGMP Multicast Channel Packages---------------------------------------------------------------------------------------------------\n\r"
        "m 1710 packageId(0-255) - Add Multicast Package\n\r"
        "m 1711 packageId(0-255) - Remove Multicast Package\n\r"        
        "m 1712 packageId(0-255) evcId# groupAddr(d.d.d.d) groupMask# sourceAddr(d.d.d.d) sourceMask# - Add Multicast Channels to Package\n\r"
        "m 1713 packageId(0-255) evcId# groupAddr(d.d.d.d) groupMask# sourceAddr(d.d.d.d) sourceMask# - Add Multicast Channels to Package\n\r"        
        "m 1714 evcId# intfType/intf# onuId# - Add Multicast Service\n\r"
        "m 1715 evcId# intfType/intf# onuId# - Remove Multicast Service\n\r"     
        "m 1716 evcId# intfType/intf# ovid(0-4095) cvid(0-4095) onuId(0-127) packageId(0-255) - Igmp Unicast Client Add \r\n"
        "m 1717 evcId# intfType/intf# ovid(0-4095) cvid(0-4095) onuId(0-127) packageId(0-255) - Igmp Unicast Client Remove\r\n"   
        "m 1718 evcId# intfType/intf# ovid(0-4095) ivid(0-4095) cvid(0-4095)  onuId(0-127) packageId(0-255) - Igmp MacBridge Client Add \r\n"
        "m 1719 evcId# intfType/intf# ovid(0-4095) ivid(0-4095) cvid(0-4095)  onuId(0-127) packageId(0-255) - Igmp MacBridge Client Remove \r\n"
        "--- Routing --------------------------------------------------------------------------------------------------------------------------\n\r"
        "m 1810 intfType(2-Rtr)/intf# evc_id ipaddr(d.d.d.d) subnetMask(d.d.d.d) mtu(xxx)- Create new routing interface\r\n"
        "m 1811 intfType(2-Rtr)/intf# - Remove routing interface\r\n"
        "m 1820 intfType(2-Rtr)/intf# - Get ARP table\r\n"
        "m 1821 intfType(2-Rtr)/intf# ipaddr(d.d.d.d) - Purge ARP entry\r\n"
        "m 1830 intfType(2-Rtr)/intf# - Get route table\r\n"
        "m 1831 ipaddr(d.d.d.d) subnetMask(d.d.d.d) gateway(d.d.d.d) pref(1-255) isNullRoute(0/1) - Add static route\r\n"
        "m 1832 ipaddr(d.d.d.d) subnetMask(d.d.d.d) gateway(d.d.d.d) isNullRoute(0/1) - Delete static route\r\n"
        "m 1840 index(0-15) ipaddr(d.d.d.d) count(1-15) size(0-65507) interval(1-60) - Create ping session\r\n"
        "m 1841 index(0-15) - Query ping session\r\n"
        "m 1842 index(0-15) - Free ping session\r\n"
        "m 1850 index(0-15) ipaddr(d.d.d.d) probes(1-10) size(0-65507) interval(1-60) dontFrag(0/1) port(0-65535) maxTtl(1-255) initTtl(1-255) maxFails(0-255) - Create traceroute session\r\n"
        "m 1851 index(0-15) - Query traceroute session\r\n"
        "m 1852 index(0-15) - Get traceroute session hops\r\n"
        "m 1853 index(0-15) - Free traceroute session\r\n"
        "--- Port Mirroring  --------------------------------------------------------------------------------------------------------------\n\r"
        "m 1885 mode=en(0/1) dstintf=intfType/intf# [srcintf=intfType/intf# dir=None/Both/Rx/Tx(0/1/2/3) ...]  - Configure Port Mirroring session. Multiple Src may be configured. \r\n"
        "--- OAM --------------------------------------------------------------------------------------------------------------------------\n\r"
        "m 1890 port# idx# packet_number(1-65535) packet_size(64-1500) period(0-127) dmmCosColor(0-7) - MEP Frame Delay Write \r\n"
        "m 1891 port# idx# packet_number(1-65535) packet_size(64-1500) period(0-127) dmmCosColor(0-7) - MEP Frame Delay Remove \r\n"
        "m 1892 port# idx# status(0-2) - MEP Frame Delay get results \r\n"
        "m 1893 port# idx# type(0-1) lmmPeriod(1-7) lmmCosColor(0-1) flrCosColor(0-2) - MEP Frame Loss Write \r\n"
        "m 1894 port# idx# type(0-1) lmmPeriod(1-7) lmmCosColor(0-1) flrCosColor(0-2) - MEP Frame Loss Remove \r\n"
        "m 1895 port# idx# mask(0-15) - MEP Frame Loss get results \r\n"
        "--- Reset -----------------------------------------------------------------------------------------------------------------------------------\n\r"
        "m 1997 - Reset Multicast machine\r\n"
        "m 1998 - Reset alarms\r\n"
        "m 1999 - Reset defaults, except for lag InBand\r\n"
        "m 2000 age(30-1000000) - Set MAC Learning table aging time\r\n"
        "m 2001 - Get MAC Learning aging time\r\n"
        "--------------------------------------------------------------------------------------------------------------------------------------\n\r"
        /*"m 1304 port[0-15] - Get SFP info\n\r"*/

        "testit msg[????h] byte1[??[h]] byte2[??[h]] ... - Build your own message!!!\n\r"
        "\n\r"
        );
        fflush(stdout);
}


//******************************************************************************
//
//******************************************************************************

int main (int argc, char *argv[])
{  
//  int cnt;
  uint64 valued;
  unsigned int msg=0;//, interPON;
  pc_type      comando, resposta;

  if (argc < 2) {
    help_oltBuga();
    exit(0);
  }
  if ( !strcmp(argv[1],"h") || ( argc <= 1 ) )
  {
    help_oltBuga();
    exit(0);
  }
  /* PING command */
  else if ( !strcmp(argv[1],"ping") && ( argc >= 2 ) )
  {
    // 1 - Preparar mensagem a enviar ao modulo de controlo
    comando.protocolId   = 1;
    comando.srcId        = PORTO_TX_MSG_BUGA;
    comando.dstId        = PORTO_RX_MSG_BUGA;
    comando.flags        = (FLAG_COMANDO); //(IPCLIB_FLAGS_CMD | (IPC_UID<<4));
    comando.counter      = rand ();
    comando.msgId        = CCMSG_APPLICATION_IS_ALIVE;
    comando.infoDim      = sizeof(unsigned int);
    *(int*)comando.info  = 0;

    uint32 period = 20,
           nretries = 6,
           ret = 255;

    /* Period specified ? */
    if (argc >= 3)
      StrToLong(argv[2], &period);

    /* N-retries specified ? */
    if (argc == 4)
      StrToLong(argv[3], &nretries);

    canal_buga=open_ipc(PORTO_TX_MSG_BUGA,IP_LOCALHOST, NULL, period);
    if ( canal_buga<0 )
    {
      printf("Erro no open IPC do BUGA...\n\r");
      exit(-1);
    }

    printf("Checking if OLTSWITCH is alive...\n\r");
    do {
      // Send command
      valued = send_data (canal_buga, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta);
      ret = ENDIAN_SWAP32(*((int *)resposta.info));
      if ( valued ) {
        printf("Without answer from OLTSWITCH... probably it is still starting\n\r");
      }
      else if (resposta.flags != (FLAG_RESPOSTA | FLAG_ACK)) {
        printf("Invalid answer... Request not Aknowledged\n\r");
        sleep(period);
      }
      else if (resposta.infoDim == sizeof(int) && ret == 2) {
        printf("OLTSWITCH replied... Application is still loading!\n\r");
        sleep(period);
      }
      else {
        break;
      }
    } while ((--nretries) > 0);

    close_ipc(canal_buga);

    if (nretries > 0) {
      printf("OLTSWITCH replied... ");
      if (ret == 0) {
        printf("Application is ready!\n\r");
      }
      else if (ret == 1) {
        printf("Application is busy!\n\r");
      }
      else if (ret == 2) {
        printf("Application is loading!\n\r");
      }
      else if (ret == 3) {
        printf("Application has CRASHED!\n\r");
      }
      else {
        printf("OLTSWITCH replied... return code is unknown: %d\n\r", (int)ret);
      }
      exit(ret);
    }
    else if (resposta.infoDim == sizeof(int) && ret == 2)
    {
      printf("OLTSWITCH is in loading process.\n\r");
      exit(ret);
    }

    printf("OLTSWITCH did not reply... Timeout!\n\r");
    exit(-2);
  }

  else if ( !strcmp(argv[1],"m") && ( argc >= 3 ) )
  {
    if (StrToLongLong(argv[2],&valued)>=0)
    {
      msg = (unsigned int)valued;
    }
    else
    {
      help_oltBuga();
      exit(0);
    }

    //printf("A mensagem a enviar e' a %d\n\r", msg);

    // 1 - Preparar mensagem a enviar ao modulo de controlo
    comando.protocolId   = 1;
    comando.srcId        = PORTO_TX_MSG_BUGA;
    comando.dstId        = PORTO_RX_MSG_BUGA;
    comando.flags        = (FLAG_COMANDO); //(IPCLIB_FLAGS_CMD | (IPC_UID<<4));
    comando.counter      = rand ();
    comando.msgId        = msg;
    comando.infoDim      = sizeof(unsigned int);
    *(int*)comando.info  = 0;

    switch (msg) {
      /* Set MAC aging time */
      case 2000:
        {
          msg_switch_config_t *swcfg;

          /* Validate number of arguments */
          if (argc != 3+1)  {
            help_oltBuga();
            exit(0);
          }

          swcfg = (msg_switch_config_t *)comando.info;

          swcfg->SlotId = ENDIAN_SWAP8((uint8)-1);
          swcfg->mask   = ENDIAN_SWAP32(0x01);

          /* integer value to be sent */
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          swcfg->aging_time = ENDIAN_SWAP32((uint32) valued);

          comando.msgId = CCMSG_ETH_SWITCH_CONFIG_SET;
          comando.infoDim = sizeof(msg_switch_config_t);
        }
        break;

    /* Get MAC aging time */
    case 2001:
      {
        msg_switch_config_t *swcfg;

        /* Validate number of arguments */
        if (argc != 3+0)  {
          help_oltBuga();
          exit(0);
        }

        swcfg = (msg_switch_config_t *)comando.info;

        swcfg->SlotId = ENDIAN_SWAP8((uint8)-1);
        swcfg->mask   = ENDIAN_SWAP32(0xff);

        comando.msgId = CCMSG_ETH_SWITCH_CONFIG_GET;
        comando.infoDim = sizeof(msg_switch_config_t);
      }
      break;

      // Reset Multicast machine
      case 1997:
        {
          msg_HwGenReq_t *ptr;

          // Validate number of arguments
          if (argc!=3+0)  {
            help_oltBuga();
            exit(0);
          }

          // integer value to be sent
          ptr = (msg_HwGenReq_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HwGenReq_t));
          ptr->slot_id = ENDIAN_SWAP8((uint8)-1);

          comando.msgId = CCMSG_MULTICAST_MACHINE_RESET;
          comando.infoDim = sizeof(msg_HwGenReq_t);
        }
        break;

      // Reset Alarms
      case 1998:
        {
          int *ptr;

          // Validate number of arguments
          if (argc!=3+0)  {
            help_oltBuga();
            exit(0);
          }

          // integer value to be sent
          ptr = (int *) &(comando.info[0]);
          *ptr = ENDIAN_SWAP32(0);

          comando.msgId = CCMSG_ALARMS_RESET;
          comando.infoDim = sizeof(int);
        }
        break;

      // Reset defaults
      case 1999:
        {
          msg_HwGenReq_t *ptr;

          // Validate number of arguments
          if (argc != 3+0)  {
            help_oltBuga();
            exit(0);
          }

          ptr = (msg_HwGenReq_t *) &(comando.info[0]);
          ptr->slot_id = ENDIAN_SWAP8(-1);

          comando.msgId = CCMSG_DEFAULTS_RESET;
          comando.infoDim = sizeof(msg_HwGenReq_t);
        }
        break;

      // Redirect stdout
      case 1000:
        {
          char *tty_name;
          uint16 len;

          // Validate number of arguments
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          tty_name = (argc>3) ? argv[3+0] : ttyname(1);

          len = strlen((char *) tty_name);
          if (len>100) {
            printf("tty device name too long\n");
            exit(0);
          }

          strcpy((char *) &comando.info[0], tty_name );

          comando.msgId = CCMSG_APP_CHANGE_STDOUT;
          comando.infoDim = strlen(tty_name)+1;
        }
        break;

        // Redirect Logger output
        case 1001:
        {
          uint16 len = 0;
          uint8  output = 0;

          // Validate number of arguments
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          comando.infoDim = 0;

          /* Output index */
          if (argc>=3+1)
          {
            // enable
            if (StrToLongLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            output = (uint8) valued;
          }
          comando.info[0] = ENDIAN_SWAP8(output);
          comando.infoDim += 1;

          /* File */
          if (argc>=3+2)
          {
            /* File direction */
            len = strlen(argv[3+1]);

            /* Validate length */
            if (len>100) {
              printf("File name too long\n");
            }

            strncpy((char *) &comando.info[1], argv[3+1], 101 );
            comando.info[1+100] = '\0';
            printf("Going to use \"%s\" filename...\n",&comando.info[1]);

            /* Consider also the null character */
            comando.infoDim += len+1;
          }
          else
          {
            /* Stdout direction */
            comando.info[0] = '\0';
            comando.infoDim += 1;
          }

          comando.msgId = CCMSG_APP_LOGGER_OUTPUT;
        }
        break;

      // Set fw state
      case 1003:
      {
        uint8 state = 0;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        comando.infoDim = 0;

        // New state
        if (StrToLongLong(argv[3+0],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        state = (uint8) valued;

        comando.info[0] = ENDIAN_SWAP8(state);
        comando.infoDim = sizeof(uint8);

        comando.msgId = CCMSG_APP_FW_STATE_SET;
      }
      break;

      // Get application resources
      case 1004:
        {
          msg_ptin_policy_resources *resources;

          // Validate number of arguments
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          resources  = (msg_ptin_policy_resources *) &comando.info[0];
          resources->SlotId = ENDIAN_SWAP8((uint8)-1);

          comando.msgId = CCMSG_APPLICATION_RESOURCES;
          comando.infoDim = sizeof(msg_ptin_policy_resources);
        }
        break;

      // PRBS machine enable
      case 1006:
        {
          int i;
          uint8 enable;
          msg_ptin_pcs_prbs *ptr;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_ptin_pcs_prbs *) &(comando.info[0]);

          // enable
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          enable = (uint8) valued;

          for (i=0; i<(argc-3-1); i++)
          {
            memset(&ptr[i],0x00,sizeof(msg_ptin_pcs_prbs));

            // Slot id
            ptr[i].SlotId = ENDIAN_SWAP8((uint8)-1);

            // Port
            if (StrToLongLong(argv[3+1+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr[i].intf.intf_type = ENDIAN_SWAP8(0);
            ptr[i].intf.intf_id   = ENDIAN_SWAP8((uint8)valued);

            // enable
            ptr[i].enable = ENDIAN_SWAP8(enable);
          }

          comando.msgId = CCMSG_ETH_PCS_PRBS_ENABLE;
          comando.infoDim = sizeof(msg_ptin_pcs_prbs)*(argc-3-1);
        }
        break;

      // PRBS errors
      case 1007:
        {
          int i;
          msg_ptin_pcs_prbs *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_ptin_pcs_prbs *) &(comando.info[0]);

          for (i=0; i<(argc-3); i++)
          {
            memset(&ptr[i],0x00,sizeof(msg_ptin_pcs_prbs));

            // Slot id
            ptr[i].SlotId = ENDIAN_SWAP8((uint8)-1);

            // Port
            if (StrToLongLong(argv[3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr[i].intf.intf_type = ENDIAN_SWAP8(0);
            ptr[i].intf.intf_id   = ENDIAN_SWAP8((uint8)valued);
          }

          comando.msgId = CCMSG_ETH_PCS_PRBS_STATUS;
          comando.infoDim = sizeof(msg_ptin_pcs_prbs)*(argc-3);
        }
        break;

      // Slot mode configuration
      case 1005:
        {
          /* Get slot configuration */
          *((uint32 *) &comando.info[0]) = ENDIAN_SWAP32(0);

          comando.msgId = CCMSG_SLOT_MAP_MODE_GET;
          comando.infoDim = sizeof(uint32);
        }
        break;

      case 1008:
      case 1009:
        {
          uint8 i, index, mode;
          msg_slotModeCfg_t *ptr;

          // Validate number of arguments
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_slotModeCfg_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_slotModeCfg_t));

          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          /* Run all provided slots */
          for (i=0; i<(argc-3)/2 && i<MSG_SLOTMODECFG_NSLOTS; i++)
          {
            /* Slot index */
            if (StrToLongLong(argv[3+(i*2)],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            index = (uint8) valued;
            /* Slot mode */
            if (StrToLongLong(argv[3+(i*2)+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            mode = (uint8) valued;

            /* Fill array cell */
            ptr->slot_list[i].slot_index  = ENDIAN_SWAP8(index);
            ptr->slot_list[i].slot_mode   = ENDIAN_SWAP8(mode);
            ptr->slot_list[i].slot_config = ENDIAN_SWAP8(1);
          }

          comando.msgId = (msg==1009) ? CCMSG_SLOT_MAP_MODE_APPLY : CCMSG_SLOT_MAP_MODE_VALIDATE;
          comando.infoDim = sizeof(msg_slotModeCfg_t);
        }
        break;

      // Switch Phy port configuration
      case 1010:
        {
          int ret,index;
          msg_HWEthPhyConf_t *ptr;
          char param[21], value[21];

          // Validate number of arguments
          if (argc<=3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HWEthPhyConf_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HWEthPhyConf_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);
          ptr->Port   = ENDIAN_SWAP8((uint8)-1);
          ptr->Mask   = ENDIAN_SWAP16(0);

          for (index=(3+0); index<argc; index++)
          {
            param[0]='\0';
            value[0]='\0';
            if ((ret=sscanf(argv[index],"%20[^ \t:=]=%20s",param,value))!=2)
            {
              printf("Invalid syntax: use <param1>=<value1> <param2>=<value2> ... (%d param=\"%s\" value=\"%s\")\r\n",ret,param,value);
              exit(0);
            }
            /* Port */
            if (strcmp(param,"port")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid port\r\n");
                exit(0);
              }
              ptr->Port = ENDIAN_SWAP8((uint8) valued);
            }
            /* Enable */
            else if (strcmp(param,"en")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid Enable\r\n");
                exit(0);
              }
              ptr->PortEnable = ENDIAN_SWAP8((uint8) valued);
              ptr->Mask |= 0x0020;
            }
            /* Speed */
            else if (strcmp(param,"speed")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid Speed\r\n");
                exit(0);
              }
              ptr->Speed = ENDIAN_SWAP8((uint8) valued);
              ptr->Mask |= 0x0001;
            }
            /* Duplex */
            else if (strcmp(param,"duplex")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid Duplex mode\r\n");
                exit(0);
              }
              ptr->Duplex = ENDIAN_SWAP8((uint8) valued);
              ptr->Mask |= 0x0004;
            }
            /* Max Frame */
            else if (strcmp(param,"fm")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid FrameMax\r\n");
                exit(0);
              }
              ptr->MaxFrame = ENDIAN_SWAP16((uint16) valued);
              ptr->Mask |= 0x0040;
            }
            /* Loopback mode */
            else if (strcmp(param,"lb")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid Loopback mode\r\n");
                exit(0);
              }
              ptr->LoopBack = ENDIAN_SWAP8((uint8) valued);
              ptr->Mask |= 0x0008;
            }
            /* MAC learning */
            else if (strcmp(param,"macl")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid MACLearning modr\r\n");
                exit(0);
              }
              ptr->MacLearning = ENDIAN_SWAP8((uint8) valued);
              ptr->Mask |= 0x0100;
            }
            else
            {
              printf("Invalid param\r\n");
              exit(0);
            }
          }
          ptr->Mask = ENDIAN_SWAP16(ptr->Mask);

          /* Mandatory parameters */
          if (ptr->Port == (uint8)-1)
          {
            printf("ERROR: port parameter should be given!");
            exit(0);
          }

          comando.msgId = CCMSG_ETH_PHY_CONFIG_SET;
          comando.infoDim = sizeof(msg_HWEthPhyConf_t);
        }
        break;

      // Switch Phy port configuration
      case 1011:
        {
          msg_HwGenReq_t *ptr;

          // Validate number of arguments
          if (argc!=3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HwGenReq_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_HwGenReq_t));

          // Slot id
          ptr->slot_id = ENDIAN_SWAP8((uint8)-1);

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->generic_id = ENDIAN_SWAP8((uint8) valued);

          comando.msgId = CCMSG_ETH_PHY_CONFIG_GET;
          comando.infoDim = sizeof(msg_HwGenReq_t);
        }
        break;

      // Switch Phy states
      case 1012:
        {
          msg_HwGenReq_t *ptr;

          // Validate number of arguments
          if (argc!=3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HwGenReq_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_HwGenReq_t));

          // Slot id
          ptr->slot_id = ENDIAN_SWAP8((uint8)-1);

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->generic_id = ENDIAN_SWAP8((uint8) valued);

          comando.msgId = CCMSG_ETH_PHY_STATE_GET;
          comando.infoDim = sizeof(msg_HwGenReq_t);
        }
        break;

      /* m 1013 slot[2-19] - Apply linkscan procedure */
      case 1013:
        {
          msg_HwGenReq_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_HW_BOARD_ACTION;
          comando.infoDim = sizeof(msg_HwGenReq_t);

          // Pointer to data array
          ptr = (msg_HwGenReq_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HwGenReq_t));

          // Slot id
          ptr->slot_id    = ENDIAN_SWAP8((uint8)-1);
          ptr->generic_id = ENDIAN_SWAP8(0);
          ptr->type       = ENDIAN_SWAP8(3);
          ptr->param      = ENDIAN_SWAP8(0);

          // Slot id
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->generic_id = (uint8) valued;
        }
        break;

      /* m 1014 slot[2-19] port[0-3] cmd[0/1] - (Uplink) Protection command */
      case 1014:
        {
          msg_uplinkProtCmd *ptr;

          // Validate number of arguments
          if (argc<3+3)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CHMSG_ETH_UPLINK_COMMAND;
          comando.infoDim = sizeof(msg_uplinkProtCmd);

          // Pointer to data array
          ptr = (msg_uplinkProtCmd *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_uplinkProtCmd));

          // Slot id
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->slotId = ENDIAN_SWAP8((uint8) valued);

          // Port
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->port = ENDIAN_SWAP8((uint8) valued);

          // Command
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->protCmd = ENDIAN_SWAP8((uint8) valued);
        }
        break;

    // Get port type definitions
    case 1015:
      {
        msg_HWPortExt_t *ptr;
        int type, intf;

        // Validate number of arguments
        if (argc!=3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_HWPortExt_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_HWPortExt_t));

        // Slot id
        ptr->SlotId = ENDIAN_SWAP8((uint8)-1);
        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = ENDIAN_SWAP8(type);
        ptr->intf.intf_id   = ENDIAN_SWAP8(intf);

        ptr->Mask = ENDIAN_SWAP32(0xFFFF);
        comando.msgId = CCMSG_ETH_PORT_EXT_GET;
        comando.infoDim = sizeof(msg_HWPortExt_t);
      }
      break;

      // Set port type definitions
      case 1016:
      {
        msg_HWPortExt_t *ptr;
        int index, ret;
        int type, intf;
        char param[21], value[21];

        // Validate number of arguments
        if (argc<=3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_HWPortExt_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_HWPortExt_t));

        ptr->SlotId                       = ENDIAN_SWAP8 ((uint8)-1);
        ptr->intf.intf_type               = ENDIAN_SWAP8 ((L7_uint8)-1);
        ptr->intf.intf_id                 = ENDIAN_SWAP8 ((L7_uint8)-1);
        ptr->Mask                         = ENDIAN_SWAP32(0x0000);
        ptr->defVid                       = ENDIAN_SWAP16(1);
        ptr->defPrio                      = ENDIAN_SWAP8 (0);
        ptr->acceptable_frame_types       = ENDIAN_SWAP8 (0);
        ptr->ingress_filter               = ENDIAN_SWAP8 (0);
        ptr->restricted_vlan_reg          = ENDIAN_SWAP8 (0);
        ptr->vlan_aware                   = ENDIAN_SWAP8 (1);
        ptr->type                         = ENDIAN_SWAP8 (2);
        ptr->doubletag                    = ENDIAN_SWAP8 (1);
        ptr->outer_tpid                   = ENDIAN_SWAP16(0x8100);
        ptr->inner_tpid                   = ENDIAN_SWAP16(0x8100);
        ptr->egress_type                  = ENDIAN_SWAP8 (0);
        ptr->macLearn_enable              = ENDIAN_SWAP8 (0);
        ptr->macLearn_stationMove_enable  = ENDIAN_SWAP8 (0);
        ptr->macLearn_stationMove_prio    = ENDIAN_SWAP8 (0);

        for (index=(3+0); index<argc; index++)
        {
          param[0]='\0';
          value[0]='\0';
          if ((ret=sscanf(argv[index],"%20[^ \t:=]=%20s",param,value))!=2)
          {
            printf("Invalid syntax: use <param1>=<value1> <param2>=<value2> ... (%d param=\"%s\" value=\"%s\")\r\n",ret,param,value);
            exit(0);
          }

          if (strcmp(param,"slot")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid slot id\r\n");
              exit(0);
            }
            ptr->SlotId = ENDIAN_SWAP8((uint8) valued);
          }
          else if (strcmp(param,"intf")==0 || strcmp(param,"port")==0)
          {
            if (sscanf(value,"%d/%d",&type,&intf)!=2)
            {
              printf("Invalid port value\r\n");
              exit(0);
            }
            ptr->intf.intf_type = ENDIAN_SWAP8(type);
            ptr->intf.intf_id   = ENDIAN_SWAP8(intf);
          }
          else if (strcmp(param,"defvid")==0 || strcmp(param,"vid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid defvid value\r\n");
              exit(0);
            }
            ptr->defVid = ENDIAN_SWAP16((uint16) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_DEFVID;
          }
          else if (strcmp(param,"defprio")==0 || strcmp(param,"prio")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid defprio value\r\n");
              exit(0);
            }
            ptr->defPrio = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_DEFPRIO;
          }
          else if (strncmp(param,"aft",3)==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid aftypes value\r\n");
              exit(0);
            }
            ptr->acceptable_frame_types = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_ACCEPTABLE_FRAME_TYPES;
          }
          else if (strncmp(param,"if",2)==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid ifilter value\r\n");
              exit(0);
            }
            ptr->ingress_filter = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_INGRESS_FILTER;
          }
          else if (strncmp(param,"rvlan",5)==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid rvlanreg value\r\n");
              exit(0);
            }
            ptr->restricted_vlan_reg = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_RESTRICTED_VLAN_REG;
          }
          else if (strncmp(param,"vlana",5)==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid vlanaware value\r\n");
              exit(0);
            }
            ptr->vlan_aware = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_VLAN_AWARE;
          }
          else if (strcmp(param,"type")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid type value\r\n");
              exit(0);
            }
            ptr->type = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_TYPE;
          }
          else if (strcmp(param,"dtag")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid dtag value\r\n");
              exit(0);
            }
            ptr->doubletag = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_DOUBLETAG;
          }
          else if (strcmp(param,"otpid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid otpid value\r\n");
              exit(0);
            }
            ptr->outer_tpid = ENDIAN_SWAP16((uint16) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_OUTER_TPID;
          }
          else if (strcmp(param,"itpid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid itpid value\r\n");
              exit(0);
            }
            ptr->inner_tpid = ENDIAN_SWAP16((uint16) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_INNER_TPID;
          }
          else if (strcmp(param,"etype")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid egress_type value\r\n");
              exit(0);
            }
            ptr->egress_type = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_EGRESS_TYPE;
          }
          else if (strcmp(param,"mlen")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid mlen value\r\n");
              exit(0);
            }
            ptr->macLearn_enable = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_MACLEARN_ENABLE;
          }
          else if (strcmp(param,"mlsmen")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid mlsmen value\r\n");
              exit(0);
            }
            ptr->macLearn_stationMove_enable = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE;
          }
          else if (strcmp(param,"mlsmprio")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid mlsmprio value\r\n");
              exit(0);
            }
            ptr->macLearn_stationMove_prio = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO;
          }
          else if (strcmp(param,"mlsmsp")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid mlsmsp value\r\n");
              exit(0);
            }
            ptr->macLearn_stationMove_samePrio = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO;
          }
          else if (strcmp(param,"trust")==0 || strcmp(param,"trusted")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid trust value\r\n");
              exit(0);
            }
            ptr->protocol_trusted = ENDIAN_SWAP8((uint8) valued);
            ptr->Mask |= MSG_HWPORTEXT_MASK_PROTTRUSTED_INTF;
          }
          else
          {
            printf("Invalid param\r\n");
            exit(0);
          }
        }
        ptr->Mask = ENDIAN_SWAP32(ptr->Mask);

        comando.msgId = CCMSG_ETH_PORT_EXT_SET;
        comando.infoDim = sizeof(msg_HWPortExt_t);
      }
      break;

    // Get interface MAC address
    case 1017:
      {
        msg_HWPortMac_t *ptr;
        int type, intf;

        // Validate number of arguments
        if (argc!=3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_HWPortMac_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_HWPortMac_t));

        // Slot id
        ptr->SlotId = ENDIAN_SWAP8((uint8)-1);
        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = ENDIAN_SWAP8(type);
        ptr->intf.intf_id   = ENDIAN_SWAP8(intf);

        ptr->Mask = ENDIAN_SWAP16(0x0001);

        comando.msgId = CCMSG_ETH_PORT_MAC_GET;
        comando.infoDim = sizeof(msg_HWPortMac_t);
      }
      break;

    // Set interface MAC address
    case 1018:
      {
        msg_HWPortMac_t *ptr;
        int type, intf;

        // Validate number of arguments
        if (argc!=3+2)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_HWPortMac_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_HWPortMac_t));

        // Slot id
        ptr->SlotId = ENDIAN_SWAP8((uint8)-1);
        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = ENDIAN_SWAP8(type);
        ptr->intf.intf_id   = ENDIAN_SWAP8(intf);

        // MAC address
        if (convert_macaddr2array(argv[3+1],ptr->macAddr)<0)  {
          help_oltBuga();
          exit(0);
        }

        ptr->Mask = ENDIAN_SWAP16(0x0001);

        comando.msgId = CCMSG_ETH_PORT_MAC_SET;
        comando.infoDim = sizeof(msg_HWPortMac_t);
      }
      break;

      // Get port counters
      case 1020:
        {
          msg_HwGenReq_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HwGenReq_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_HwGenReq_t));

          // Slot id
          ptr->slot_id = ENDIAN_SWAP8((uint8)-1);

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->generic_id = ENDIAN_SWAP8((uint8) valued);

          comando.msgId = CCMSG_ETH_PHY_COUNTERS_GET;
          comando.infoDim = sizeof(msg_HwGenReq_t);
        }
        break;

      // Clear port counters
      case 1021:
        {
          msg_HWEthRFC2819_PortStatistics_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HWEthRFC2819_PortStatistics_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HWEthRFC2819_PortStatistics_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->Port = ENDIAN_SWAP8((uint8) valued);

          comando.msgId = CCMSG_ETH_PHY_COUNTERS_CLEAR;
          comando.infoDim = sizeof(msg_HWEthRFC2819_PortStatistics_t);
        }
        break;

      //RFC2819 probe config
      case 1022:
        {
          msg_rfc2819_admin_t *ptr;

          // Validate number of arguments
          if (argc!=3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_rfc2819_admin_t *)comando.info;

          // Slot id
          ptr->SlotId = (uint8)-1;

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->Port = (uint8) valued;

          // Enable
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->Admin = (uint8) valued;

          comando.msgId = CHMSG_RFC2819_MONITORING_CONFIG;
          comando.infoDim = sizeof(msg_rfc2819_admin_t);
        }
        break;

      //Clear RFC2819 Monitoring buffer
      case 1023:
        {
          L7_uint32 *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (L7_uint32 *)comando.info;

          // BufferType
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          *ptr = (uint8) valued;

          if (*ptr == RFC2819_BUFFER_15MIN) {
            *ptr = 0;
          }
          else 
          if (*ptr == RFC2819_BUFFER_24HOURS) {
            *ptr = 0x80000000;
          }
          else {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CHMSG_RFC2819_MONITORING_CLEAR;
          comando.infoDim = sizeof(L7_uint32);
        }
        break;

      //Read RFC2819 buffers
      case 1024:
      {
        L7_uint32 aux;
        L7_uint32 *ptr;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to parameter
        ptr = (L7_uint32 *)comando.info;

        // BufferType
        if (StrToLongLong(argv[3+0],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        aux = (L7_uint32) valued;

        if (aux == RFC2819_BUFFER_15MIN) {
          *ptr = 0x00000000;
        }
        else 
        if (aux == RFC2819_BUFFER_24HOURS) {
          *ptr = 0x80000000;
        }
        else {
          help_oltBuga();
          exit(0);
        }

        // BufferIndex
        if (StrToLongLong(argv[3+1],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        aux = (L7_uint32) valued;

        *ptr = *ptr | (aux & 0xFFFF);

        comando.msgId = CHMSG_RFC2819_MONITORING_GET;
        comando.infoDim = sizeof(L7_uint32);
      }
      break;

      //Read RFC2819 probe configuration
      case 1025:
      {
        L7_uint32 *Port;

        // Validate number of arguments
        if (argc!=3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        Port = (L7_uint32 *)comando.info;

        // port
        if (StrToLongLong(argv[3+0],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        *Port = (L7_uint32)valued;

        comando.msgId = CHMSG_RFC2819_MONITORING_SHOW_CONF;
        comando.infoDim = sizeof(L7_uint32);
      }
      break;


      //Read RFC2819 buffers status
      case 1026:
      {
        L7_uint32 *BufferType;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to parameter
        BufferType = (L7_uint32 *)comando.info;

        // BufferType
        if (StrToLongLong(argv[3+0],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        *BufferType = (L7_uint32)valued;

        if ((*BufferType != RFC2819_BUFFER_15MIN) && (*BufferType != RFC2819_BUFFER_24HOURS)) {
          help_oltBuga();
          exit(0);
        }

        comando.msgId = CHMSG_RFC2819_MONITORING_BUFF_STATUS;
        comando.infoDim = sizeof(L7_uint32);
      }
      break;

        //MAC Limiting per interface
      case 1027:
        {
          msg_l2_maclimit_config_t *ptr;

          // Validate number of arguments
          if (argc!=3+7)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_l2_maclimit_config_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_l2_maclimit_config_t));

          //SlotID
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->slotId = (uint8) valued;

          // System
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->system = (uint8) valued;

          // Intf
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) valued;

          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_id = (uint8) valued;

          // limit
          if (StrToLongLong(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->limit = (uint32) valued;

          // action
          if (StrToLongLong(argv[3+5],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->action = (uint8) valued;

          // send_trap
          if (StrToLongLong(argv[3+6],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->send_trap = (uint8) valued;
      
          comando.msgId = CCMSG_L2_MACLIMIT_CONFIG;
          comando.infoDim = sizeof(msg_l2_maclimit_config_t);
        }
        break;

      case 1030:
        {
          msg_QoSConfiguration_t *ptr;
          int type, intf;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_QoSConfiguration_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_QoSConfiguration_t));

          ptr->SlotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          comando.msgId = CCMSG_ETH_PORT_COS_GET;
          comando.infoDim = sizeof(msg_QoSConfiguration_t);
        }
        break;

      case 1031:
        {
          msg_QoSConfiguration_t *ptr;
          int type, intf;
          uint8 i;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+11)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_QoSConfiguration_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_QoSConfiguration_t));

          ptr->SlotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          // Trust mode
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->trust_mode = (uint8) valued;
          ptr->mask |= MSG_QOS_CONFIGURATION_TRUSTMODE_MASK;

          ptr->bandwidth_unit = 0;
          ptr->mask |= MSG_QOS_CONFIGURATION_BANDWIDTHUNIT_MASK;

          // Shaping rate
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->shaping_rate = (uint32) valued*1000;
          ptr->mask |= MSG_QOS_CONFIGURATION_SHAPINGRATE_MASK;


          // Priorities map
          for (i=0; i<8; i++) {
            if (StrToLongLong(argv[3+3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->pktprio.cos[i] = (uint32) valued;
          }
          ptr->pktprio.mask = 0xFF;
          ptr->mask |= MSG_QOS_CONFIGURATION_PACKETPRIO_MASK;

          comando.msgId = CCMSG_ETH_PORT_COS_SET;
          comando.infoDim = sizeof(msg_QoSConfiguration_t);
        }
        break;

      case 1032:
        {
          msg_QoSConfiguration_t *ptr;
          int type, intf;
          uint8 cos;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+5)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_QoSConfiguration_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_QoSConfiguration_t));

          ptr->SlotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          // Extract CoS and validate it
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          cos = (uint8) valued;
          if (cos>=8) {
            help_oltBuga();
            exit(0);
          }

          ptr->mask = MSG_QOS_CONFIGURATION_QOSCONF_MASK;
          ptr->cos_config.mask |= (1<<cos);

          // Scheduler
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->cos_config.cos[cos].scheduler = (uint8) valued;
          ptr->cos_config.cos[cos].mask |= MSG_QOS_CONFIGURATION_QOSCONF_SCHEDULER_MASK;

          // Min Bandwidth
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->cos_config.cos[cos].min_bandwidth = (uint32) valued*1000;
          ptr->cos_config.cos[cos].mask |= MSG_QOS_CONFIGURATION_QOSCONF_BW_MIN_MASK;

          // Max Bandwidth
          if (StrToLongLong(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->cos_config.cos[cos].max_bandwidth = (uint32) valued*1000;
          ptr->cos_config.cos[cos].mask |= MSG_QOS_CONFIGURATION_QOSCONF_BW_MAX_MASK;

          comando.msgId = CCMSG_ETH_PORT_COS_SET;
          comando.infoDim = sizeof(msg_QoSConfiguration_t);
        }
        break;

        case 1033:
        {
          msg_QoSConfiguration3_t *ptr;
          int type, intf;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_QoSConfiguration3_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_QoSConfiguration3_t));

          ptr->SlotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          comando.msgId = CCMSG_ETH_PORT_COS3_GET;
          comando.infoDim = sizeof(msg_QoSConfiguration3_t);
        }
        break;

    case 1034:
      {
        msg_QoSConfiguration3_t *ptr;
        int type, intf;
        uint8 i, j, prio;

        // Validate number of arguments (flow_id + 2 pairs port+svid)
        if (argc<3+2)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_QoSConfiguration3_t *) &(comando.info[0]);
        memset(ptr,0,sizeof(msg_QoSConfiguration3_t));

        ptr->SlotId = (uint8)-1;

        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = (uint8) type;
        ptr->intf.intf_id   = (uint8) intf;

        /* Bandwidth units */
        ptr->bandwidth_unit = 0;
        ptr->main_mask |= MSG_QOS3_BANDWIDTH_UNITS_MASK;

        // Trust mode (mandatory option)
        if (StrToLongLong(argv[3+1],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->ingress.trust_mode = (uint8) valued;
        ptr->ingress.ingress_mask |= MSG_QOS_CONFIGURATION_TRUSTMODE_MASK;
        ptr->main_mask |= MSG_QOS3_INGRESS_MASK;

        // Priorities map
        for (i=0; i<8 && argc>=(3+3+i); i++) {
          if (StrToLongLong(argv[3+2+i],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          /* 802.1p trust mode */
          if (ptr->ingress.trust_mode == 2)
          {
            ptr->ingress.cos_classif.pcp_map.cos[i] = (uint32)valued;
            ptr->ingress.cos_classif.pcp_map.prio_mask |= (1 << i);
          }
          /* IP_PREC trust mode */
          else if (ptr->ingress.trust_mode == 3)
          {
            ptr->ingress.cos_classif.ipprec_map.cos[i] = (uint32)valued;
            ptr->ingress.cos_classif.ipprec_map.prio_mask |= (1 << i);
          }
          /* DSCP trust mode */
          else if (ptr->ingress.trust_mode == 4)
          {
            for (j = 0; j < 8; j++)
            {
              prio = (i*8)+j;

              ptr->ingress.cos_classif.dscp_map.cos[prio] = ((uint32)valued >> (4*j)) & 0xf;
              ptr->ingress.cos_classif.dscp_map.prio_mask[prio/32] |= (1 << (prio%32));

              printf("Configuring prio %u: value=%u\r\n", prio, ptr->ingress.cos_classif.dscp_map.cos[prio]);
            }
          }

          ptr->ingress.ingress_mask |= MSG_QOS3_INGRESS_COS_CLASSIF_MASK; 
        }

        comando.msgId = CCMSG_ETH_PORT_COS3_SET;
        comando.infoDim = sizeof(msg_QoSConfiguration3_t);
      }
      break;

      case 1035:
        {
          msg_QoSConfiguration3_t *ptr;
          int type, intf;
          uint8 cos;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+3)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_QoSConfiguration3_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_QoSConfiguration3_t));

          ptr->SlotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          // Extract CoS and validate it
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          cos = (uint8) valued;
          if (cos>=8) {
            help_oltBuga();
            exit(0);
          }

          // Scheduler (mandatory)
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->egress.cos_scheduler[cos].schedulerType = (uint8) valued;
          ptr->egress.cos_scheduler[cos].local_mask |= MSG_QOS3_EGRESS_COS_SCHEDULER_TYPE_MASK;
          ptr->egress.egress_mask |= MSG_QOS3_EGRESS_COS_SCHEDULER_MASK;
          ptr->main_mask |= MSG_QOS3_EGRESS_MASK;

          // WRR weight
          if (argc >= 3+4)
          {
            if (StrToLongLong(argv[3 + 3], &valued) < 0)
            {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.cos_scheduler[cos].wrrSched_weight = (uint16) valued;
            ptr->egress.cos_scheduler[cos].local_mask |= MSG_QOS3_EGRESS_COS_SCHEDULER_WRR_WEIGHT_MASK;
            ptr->egress.egress_mask |= MSG_QOS3_EGRESS_COS_SCHEDULER_MASK;
          }

          // Shaper
          if (argc >= 3+5)
          {
            if (StrToLongLong(argv[3+4],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.shaping_rate = (uint32) valued;
            ptr->egress.egress_mask |= MSG_QOS3_EGRESS_INTF_SHAPER_MASK;
          }

          // Min Bandwidth
          if (argc >= 3+6)
          {
            if (StrToLongLong(argv[3+5],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.cos_shaper[cos].min_bandwidth = (uint32) valued;
            ptr->egress.cos_shaper[cos].local_mask |= MSG_QOS3_EGRESS_COS_SHAPER_MIN_BW_MASK;
            ptr->egress.egress_mask |= MSG_QOS3_EGRESS_COS_SHAPER_MASK;
          }

          // Max Bandwidth
          if (argc >= 3+7)
          {
            if (StrToLongLong(argv[3+6],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.cos_shaper[cos].max_bandwidth = (uint32) valued;
            ptr->egress.cos_shaper[cos].local_mask |= MSG_QOS3_EGRESS_COS_SHAPER_MAX_BW_MASK;
            ptr->egress.egress_mask |= MSG_QOS3_EGRESS_COS_SHAPER_MASK;
          }

          comando.msgId = CCMSG_ETH_PORT_COS3_SET;
          comando.infoDim = sizeof(msg_QoSConfiguration3_t);
        }
        break;

      case 1036:
        {
          msg_QoSConfiguration3_t *ptr;
          int type, intf;
          uint8 cos;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+3)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_QoSConfiguration3_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_QoSConfiguration3_t));

          ptr->SlotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          // Extract CoS and validate it
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          cos = (uint8) valued;
          if (cos>=8) {
            help_oltBuga();
            exit(0);
          }

          // Drop Management type
          if (StrToLongLong(argv[3+2], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->egress.cos_dropmgmt[cos].dropMgmtType = (uint8) valued;
          ptr->egress.cos_dropmgmt[cos].local_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_TYPE_MASK;
          ptr->egress.egress_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_MASK;
          ptr->main_mask |= MSG_QOS3_EGRESS_MASK;

          // WRED Decay Exponent
          if (argc >= 3+4)
          {
            if (StrToLongLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.cos_dropmgmt[cos].wred_decayExp = (uint8) valued;
            ptr->egress.cos_dropmgmt[cos].local_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_WRED_DECAYEXP_MASK;
          }

          comando.msgId = CCMSG_ETH_PORT_COS3_SET;
          comando.infoDim = sizeof(msg_QoSConfiguration3_t);
        }
        break;

      case 1037:
        {
          msg_QoSConfiguration3_t *ptr;
          int type, intf;
          uint8 cos, dp;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+3)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_QoSConfiguration3_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_QoSConfiguration3_t));

          ptr->SlotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          // Extract CoS and validate it
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          cos = (uint8) valued;
          if (cos>=8) {
            help_oltBuga();
            exit(0);
          }

          // Extract DP and validate it
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          dp = (uint8) valued;
          if (dp==0 || dp>=5) {
            help_oltBuga();
            printf("Wrong DP level: should be bewteen 1 and 4\r\n");
            exit(0);
          }
          dp--;

          // Taildrop threshold
          if (argc >= 3+4)
          {
            if (StrToLongLong(argv[3 + 3], &valued) < 0)
            {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.cos_dropmgmt[cos].dp_thresholds[dp].tailDrop_threshold = (uint8) valued;
            ptr->egress.cos_dropmgmt[cos].dp_thresholds[dp].local2_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_TAILDROP_MAX_MASK;

            ptr->egress.cos_dropmgmt[cos].local_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLDS_MASK;
            ptr->egress.egress_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_MASK;
            ptr->main_mask |= MSG_QOS3_EGRESS_MASK;
          }
          // WRED min threshold
          if (argc >= 3+5)
          {
            if (StrToLongLong(argv[3 + 4], &valued) < 0)
            {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.cos_dropmgmt[cos].dp_thresholds[dp].wred_minThreshold = (uint8) valued;
            ptr->egress.cos_dropmgmt[cos].dp_thresholds[dp].local2_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_MIN_MASK;
          }
          // WRED max threshold
          if (argc >= 3+6)
          {
            if (StrToLongLong(argv[3 + 5], &valued) < 0)
            {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.cos_dropmgmt[cos].dp_thresholds[dp].wred_maxThreshold = (uint8) valued;
            ptr->egress.cos_dropmgmt[cos].dp_thresholds[dp].local2_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_MAX_MASK;
          }
          // WRED drop probability
          if (argc >= 3+7)
          {
            if (StrToLongLong(argv[3 + 6], &valued) < 0)
            {
              help_oltBuga();
              exit(0);
            }
            ptr->egress.cos_dropmgmt[cos].dp_thresholds[dp].wred_dropProb = (uint8) valued;
            ptr->egress.cos_dropmgmt[cos].dp_thresholds[dp].local2_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_DROPPROB_MASK;
          }

          comando.msgId = CCMSG_ETH_PORT_COS3_SET;
          comando.infoDim = sizeof(msg_QoSConfiguration3_t);
        }
        break;

    case 1038:
      {
        msg_QoSConfiguration3_t *ptr;
        int type, intf;
        uint8 cos;

        // Validate number of arguments (flow_id + 2 pairs port+svid)
        if (argc<3+2)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_QoSConfiguration3_t *) &(comando.info[0]);
        memset(ptr,0,sizeof(msg_QoSConfiguration3_t));

        ptr->SlotId = (uint8)-1;

        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = (uint8) type;
        ptr->intf.intf_id   = (uint8) intf;

        // Extract CoS and validate it
        if (StrToLongLong(argv[3+1],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        cos = (uint8) valued;
        if (cos>=8) {
          help_oltBuga();
          exit(0);
        }

        /* CIR */
        if (argc >= 3+3)
        {
          // Drop Management type
          if (StrToLongLong(argv[3+2], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->ingress.cos_policer[cos].cir = (uint32) valued;
          ptr->ingress.cos_policer[cos].cbs = 9600;
          ptr->ingress.cos_policer[cos].eir = 0;
          ptr->ingress.cos_policer[cos].ebs = 0;
          ptr->ingress.cos_policer[cos].local_mask = 0xff;

          ptr->ingress.ingress_mask |= MSG_QOS3_INGRESS_COS_POLICER_MASK;
          ptr->main_mask |= MSG_QOS3_INGRESS_MASK;
        }
        /* EIR */
        if (argc >= 3+4)
        {
          // Drop Management type
          if (StrToLongLong(argv[3+3], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->ingress.cos_policer[cos].eir = (uint32) valued;
        }
        /* CBS */
        if (argc >= 3+5)
        {
          // Drop Management type
          if (StrToLongLong(argv[3+4], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->ingress.cos_policer[cos].cbs = (uint32) valued;
        }
        /* EBS */
        if (argc >= 3+6)
        {
          // Drop Management type
          if (StrToLongLong(argv[3+5], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->ingress.cos_policer[cos].ebs = (uint32) valued;
        }

        comando.msgId = CCMSG_ETH_PORT_COS3_SET;
        comando.infoDim = sizeof(msg_QoSConfiguration3_t);
      }
      break;

      case 1040:
        {
          msg_switch_mac_intro_t *ptr;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_switch_mac_intro_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_switch_mac_intro_t));

          ptr->slotId = (uint8)-1;

          // start id
          ptr->startEntryId = 0;
          if (argc>=3+1)
          {
            if (StrToLongLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->startEntryId = valued;
          }

          // Num entries
          ptr->numEntries = MSG_CMDGET_MAC_TABLE_MAXENTRIES;
          if (argc>=3+2)
          {
            if (StrToLongLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->numEntries = valued;
          }

          comando.msgId = CCMSG_ETH_MAC_TABLE_SHOW;
          comando.infoDim = sizeof(msg_switch_mac_intro_t);
        }
        break;

      case 1041:
        {
          msg_switch_mac_table_t *ptr;
          int type, intf;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+3)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_switch_mac_table_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_switch_mac_table_t));

          ptr->intro.slotId       = (uint8)-1;
          ptr->intro.startEntryId = 0;
          ptr->intro.numEntries   = 1;

          ptr->entry[0].evcId        = (uint32)-1;
          ptr->entry[0].static_entry = 1;

          // vlanId
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->entry[0].vlanId = (uint16) valued;

          // MAC address
          if (convert_macaddr2array(argv[3+1],ptr->entry[0].addr)<0)  {
            help_oltBuga();
            exit(0);
          }

          // port
          if (sscanf(argv[3+2],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->entry[0].intf.intf_type = (uint8) type;
          ptr->entry[0].intf.intf_id   = (uint8) intf;

          comando.msgId = CCMSG_ETH_MAC_ENTRY_ADD;
          comando.infoDim = sizeof(msg_switch_mac_table_t);
        }
        break;

      case 1042:
        {
          msg_switch_mac_table_t *ptr;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_switch_mac_table_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_switch_mac_table_t));

          ptr->intro.slotId       = (uint8)-1;
          ptr->intro.startEntryId = 0;
          ptr->intro.numEntries   = 1;

          ptr->entry[0].evcId = (uint32)-1;

          // vlanId
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->entry[0].vlanId = (uint16) valued;

          // MAC address
          if (convert_macaddr2array(argv[3+1],ptr->entry[0].addr)<0)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_MAC_ENTRY_REMOVE;
          comando.infoDim = sizeof(msg_switch_mac_table_t);
        }
        break;

      case 1043:
        {
          msg_switch_mac_table_t *ptr;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_switch_mac_table_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_switch_mac_table_t));

          ptr->intro.slotId       = (uint8)-1;
          ptr->intro.startEntryId = 0;
          ptr->intro.numEntries   = (L7_uint32)-1;

          comando.msgId = CCMSG_ETH_MAC_ENTRY_REMOVE;
          comando.infoDim = sizeof(msg_switch_mac_table_t);
        }
        break;

      case 1044:
        {
          msg_evc_qos_t *ptr;
          uint8 i, j, prio, dir;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+4)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_evc_qos_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_evc_qos_t));

          ptr->slot_id = ENDIAN_SWAP8((uint8)-1);

          // EVC
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id.id_type = ENDIAN_SWAP8(1);
          ptr->id.id_val.evc_id = ENDIAN_SWAP32((uint32) valued);

          // Uplink?
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          dir = (uint8) valued & 1;

          /* Mask active */
          ptr->qos[dir].mask = ENDIAN_SWAP8(0x01);

          // Remark pbits
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->qos[dir].pbits_remark = ENDIAN_SWAP8((uint8) valued & 1);

          // Trust mode
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->qos[dir].trust_mode = ENDIAN_SWAP8((uint8) valued);

          /* CoS map */
          if (ENDIAN_SWAP8(ptr->qos[dir].trust_mode) >= 2)
          {
            // Priorities map
            for (i=0; i<8 && argc>=(3+5+i); i++)
            {
              if (StrToLongLong(argv[3+4+i],&valued)<0)
              {
                help_oltBuga();
                exit(0);
              }
              /* 802.1p trust mode */
              if (ENDIAN_SWAP8(ptr->qos[dir].trust_mode) == 2)
              {
                ptr->qos[dir].cos_classif.pcp_map.cos[i] = ENDIAN_SWAP8((uint8)valued);
                ptr->qos[dir].cos_classif.pcp_map.prio_mask |= ENDIAN_SWAP8(1 << i);
                printf("Configuring PCP prio %u: value=%u\r\n", i, ENDIAN_SWAP8(ptr->qos[dir].cos_classif.pcp_map.cos[i]));
              }
              /* IP_PREC trust mode */
              else if (ENDIAN_SWAP8(ptr->qos[dir].trust_mode) == 3)
              {
                ptr->qos[dir].cos_classif.ipprec_map.cos[i] = ENDIAN_SWAP8((uint8)valued);
                ptr->qos[dir].cos_classif.ipprec_map.prio_mask |= ENDIAN_SWAP8(1 << i);
                printf("Configuring IPPREC prio %u: value=%u\r\n", i, ENDIAN_SWAP8(ptr->qos[dir].cos_classif.ipprec_map.cos[i]));
              }
              /* DSCP trust mode */
              else if (ENDIAN_SWAP8(ptr->qos[dir].trust_mode) == 4)
              {
                for (j = 0; j < 8; j++)
                {
                  prio = (i*8)+j;

                  ptr->qos[dir].cos_classif.dscp_map.cos[prio] = ENDIAN_SWAP8(((uint8)valued >> (4*j)) & 0xf);
                  ptr->qos[dir].cos_classif.dscp_map.prio_mask[prio/32] |= ENDIAN_SWAP32(1 << (prio%32));

                  printf("Configuring DSCP prio %u: value=%u\r\n", prio, ENDIAN_SWAP8(ptr->qos[dir].cos_classif.dscp_map.cos[prio]));
                }
              }
            }
          }

          comando.msgId = CCMSG_ETH_EVC_QOS_SET;
          comando.infoDim = sizeof(msg_evc_qos_t);
        }
        break;

      case 1220:
        {
          msg_HwEthernetDhcpOpt82Profile_t *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+3)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HwEthernetDhcpOpt82Profile_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_HwEthernetDhcpOpt82Profile_t));

          ptr->SlotId = (uint8)-1;

          // flow_id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evc_id = (uint32) valued;
          // port
          if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;
          ptr->mask |= MSG_INTERFACE_MASK;

          ptr->mask |= MSG_CLIENT_MASK;
          ptr->client.intf.intf_type = (uint8) type;
          ptr->client.intf.intf_id   = (uint8) intf;
          ptr->client.mask |= MSG_CLIENT_INTF_MASK;

          // cvid
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->client.inner_vlan = (uint16) valued;
          ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;

          // CircuitId
          ptr->circuitId[0] = '\0';
          // RemoteId
          ptr->remoteId[0] = '\0';

          comando.msgId = CCMSG_ETH_DHCP_PROFILE_GET;
          comando.infoDim = sizeof(msg_HwEthernetDhcpOpt82Profile_t);
        }
        break;

      case 1221:
        {
          msg_HwEthernetDhcpOpt82Profile_t *ptr;
          int type, intf;
          int op82=0,op37=0,op18=0;

          // Validate number of arguments
          if (argc<3+5)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HwEthernetDhcpOpt82Profile_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_HwEthernetDhcpOpt82Profile_t));

          ptr->SlotId = (uint8)-1;

          // flow_id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evc_id = (uint32) valued;
          // port
          if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;
          ptr->mask |= MSG_INTERFACE_MASK;

          ptr->mask |= MSG_CLIENT_MASK;
          ptr->client.intf.intf_type = (uint8) type;
          ptr->client.intf.intf_id   = (uint8) intf;
          ptr->client.mask |= MSG_CLIENT_INTF_MASK;

          // cvid
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->client.inner_vlan = (uint16) valued;
          ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;

          if (sscanf(argv[3 + 3], "%d/%d/%d", &op82, &op37, &op18) != 3)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->options |= 0x01 & op18;
          ptr->options |= 0x02 & (op37 << 1);
          ptr->options |= 0x04 & (op82 << 2);

          // CircuitId
          if (strlen(argv[3+4])>63)  argv[3+4][63]='\0';
          strcpy(ptr->circuitId,argv[3+4]);
          // RemoteId
          if (strlen(argv[3+5])>63)  argv[3+5][63]='\0';
          strcpy(ptr->remoteId,argv[3+5]);

          comando.msgId = CCMSG_ETH_DHCP_PROFILE_ADD;
          comando.infoDim = sizeof(msg_HwEthernetDhcpOpt82Profile_t);
        }
        break;

      case 1222:
        {
          msg_HwEthernetDhcpOpt82Profile_t *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+3)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HwEthernetDhcpOpt82Profile_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_HwEthernetDhcpOpt82Profile_t));

          ptr->SlotId = (uint8)-1;

          // flow_id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evc_id = (uint32) valued;
          // port
          if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;
          ptr->mask |= MSG_INTERFACE_MASK;

          ptr->mask |= MSG_CLIENT_MASK;
          ptr->client.intf.intf_type = (uint8) type;
          ptr->client.intf.intf_id   = (uint8) intf;
          ptr->client.mask |= MSG_CLIENT_INTF_MASK;

          // cvid
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->client.inner_vlan = (uint16) valued;
          ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;

          comando.msgId = CCMSG_ETH_DHCP_PROFILE_REMOVE;
          comando.infoDim = sizeof(msg_HwEthernetDhcpOpt82Profile_t);
        }
        break;

      /* DAI Statistics */
      case 1230:
      {
        msg_dai_statistics_t *ptr;
        int type, intf;

        // Validate number of arguments
        if (argc<3+2)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_dai_statistics_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_dai_statistics_t));

        ptr->slotId = (uint8)-1;

        // EVC id
        if (StrToLongLong(argv[3+0], &valued) < 0)
        {
          help_oltBuga();
          exit(0);
        }
        //ptr->service.id_type = MSG_ID_EVC_TYPE;
        //ptr->service.id_val.evc_id = (uint32) valued;
        ptr->evc_idx = (uint32) valued;

        // port
        if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = (uint8) type;
        ptr->intf.intf_id   = (uint8) intf;

        comando.msgId = CCMSG_ETH_DAI_STATISTICS;
        comando.infoDim = sizeof(msg_dai_statistics_t);
      }
      break;

      /* DAI global configurations */
      case 1231:
      {
        msg_dai_global_settings_t *ptr;

        // Validate number of arguments
        if (argc<3+0)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_dai_global_settings_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_dai_global_settings_t));

        ptr->slotId = (uint8)-1;

        // SMAC validation
        if (argc >= 3+1)
        {
          if (StrToLongLong(argv[3+0], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->validate_smac = (uint8) valued;
          ptr->mask |= 0x02;
        }
        // DMAC validation
        if (argc >= 3+2)
        {
          if (StrToLongLong(argv[3+1], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->validate_dmac = (uint8) valued;
          ptr->mask |= 0x04;
        }
        // IP validation
        if (argc >= 3+3)
        {
          if (StrToLongLong(argv[3+2], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->validate_ipAddr = (uint8) valued;
          ptr->mask |= 0x08;
        }

        comando.msgId = CCMSG_ETH_DAI_GLOBAL_CONFIG;
        comando.infoDim = sizeof(msg_dai_global_settings_t);
      }
      break;

      /* DAI interface configurations */
      case 1232:
      {
        msg_dai_intf_settings_t *ptr;
        int type, intf;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_dai_intf_settings_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_dai_intf_settings_t));

        ptr->slotId = (uint8)-1;

        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = (uint8) type;
        ptr->intf.intf_id   = (uint8) intf;

        // Trust mode
        if (argc >= 3+2)
        {
          if (StrToLongLong(argv[3+1], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->trust = (uint8) valued;
          ptr->mask |= 0x01;
        }
        // Rate Limit
        if (argc >= 3+3)
        {
          if (StrToLongLong(argv[3+2], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->rateLimit = (uint32) valued;
          ptr->mask |= 0x02;
        }
        // Burst interval
        if (argc >= 3+4)
        {
          if (StrToLongLong(argv[3+3], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->burstInterval = (uint32) valued;
          ptr->mask |= 0x04;
        }

        comando.msgId = CCMSG_ETH_DAI_INTF_CONFIG;
        comando.infoDim = sizeof(msg_dai_intf_settings_t);
      }
      break;

      /* DAI EVC configurations */
      case 1233:
      {
        msg_dai_vlan_settings_t *ptr;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_dai_vlan_settings_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_dai_vlan_settings_t));

        ptr->slotId = (uint8)-1;

        // EVC id
        if (StrToLongLong(argv[3+0], &valued) < 0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->service.id_type = MSG_ID_EVC_TYPE;
        ptr->service.id_val.evc_id = (uint32) valued;

        // DAI enable
        if (argc >= 3+2)
        {
          if (StrToLongLong(argv[3+1], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->dai_enable = (uint8) valued;
          ptr->mask |= 0x01;
        }
        // Static flag
        if (argc >= 3+3)
        {
          if (StrToLongLong(argv[3+2], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->staticFlag = (uint8) valued;
          ptr->mask |= 0x02;
        }

        comando.msgId = CCMSG_ETH_DAI_VLAN_CONFIG;
        comando.infoDim = sizeof(msg_dai_vlan_settings_t);
      }
      break;

      /* DAI EVC configurations */
      case 1234:
      {
        msg_dai_vlan_settings_t *ptr;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_dai_vlan_settings_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_dai_vlan_settings_t));

        ptr->slotId = (uint8)-1;

        // EVC id
        if (StrToLongLong(argv[3+0], &valued) < 0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->service.id_type = MSG_ID_NNIVID_TYPE;
        ptr->service.id_val.nni_vid = (uint32) valued;

        // DAI enable
        if (argc >= 3+2)
        {
          if (StrToLongLong(argv[3+1], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->dai_enable = (uint8) valued;
          ptr->mask |= 0x01;
        }
        // Static flag
        if (argc >= 3+3)
        {
          if (StrToLongLong(argv[3+2], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->staticFlag = (uint8) valued;
          ptr->mask |= 0x02;
        }

        comando.msgId = CCMSG_ETH_DAI_VLAN_CONFIG;
        comando.infoDim = sizeof(msg_dai_vlan_settings_t);
      }
      break;

      /* ARP-ACL rules management */
      case 1235:
      case 1236:
      {
        msg_arp_acl_t *ptr;

        // Validate number of arguments
        if (argc<3+3)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_arp_acl_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_arp_acl_t));

        ptr->slotId  = (uint8)-1;
        ptr->aclType = ACL_TYPE_ARP;
        ptr->action  = ACL_ACTION_PERMIT;

        // ACL name
        strncpy((char *) ptr->name, argv[3+0], 32);

        // IP address
        if (convert_ipaddr2uint64(argv[3+1], &valued) < 0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->srcIpAddr.family    = PTIN_AF_INET;
        ptr->srcIpAddr.addr.ipv4 = (uint32) valued;

        // MAC address
        if (convert_macaddr2array(argv[3+2], ptr->srcMacAddr) < 0)
        {
          help_oltBuga();
          exit(0);
        }

        comando.msgId   = (msg==1236) ? CCMSG_ACL_RULE_DEL : CCMSG_ACL_RULE_ADD;
        comando.infoDim = sizeof(msg_arp_acl_t);
      }
      break;

      /* ARP-ACL groups assignment */
      case 1237:
      {
        msg_apply_acl_t *ptr;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_apply_acl_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_apply_acl_t));

        ptr->slotId     = (uint8)-1;
        ptr->aclType    = ACL_TYPE_ARP;
        ptr->direction  = ACL_DIRECTION_IN;

        // EVC id
        if (StrToLongLong(argv[3+0], &valued) < 0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->evcId  = (uint32) valued;
        ptr->vlanId = (uint16) -1;

        // ACL name
        if (argc >= 3+2)
        {
          strncpy((char *)ptr->name, argv[3 + 1], 32); 
          ptr->name[31] = '\0';
          comando.msgId   = CCMSG_ACL_APPLY;
        }
        else
        {
          ptr->name[0] = '\0';
          comando.msgId   = CCMSG_ACL_UNAPPLY;
        }

        comando.infoDim = sizeof(msg_apply_acl_t);
      }
      break;
      /* ARP-ACL groups assignment */
      case 1238:
      {
        msg_apply_acl_t *ptr;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_apply_acl_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_apply_acl_t));

        ptr->slotId     = (uint8)-1;
        ptr->aclType    = ACL_TYPE_ARP;
        ptr->direction  = ACL_DIRECTION_IN;

        // EVC id
        if (StrToLongLong(argv[3+0], &valued) < 0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->vlanId = (uint16) valued;
        ptr->evcId  = (uint32) -1;

        // ACL name
        if (argc >= 3+2)
        {
          strncpy((char *)ptr->name, argv[3 + 1], 32); 
          ptr->name[31] = '\0';
          comando.msgId   = CCMSG_ACL_APPLY;
        }
        else
        {
          ptr->name[0] = '\0';
          comando.msgId   = CCMSG_ACL_UNAPPLY;
        }

        comando.infoDim = sizeof(msg_apply_acl_t);
      }
      break;

      // DHCP Bind table reading
      case 1240:
        {
          msg_DHCP_bind_table_request_t *ptr;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_DHCP_bind_table_request_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_DHCP_bind_table_request_t));

          ptr->slotId = (uint8)-1;

          // page
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->page = valued;
          ptr->mask = 0xff;

          comando.msgId = CCMSG_ETH_DHCP_BIND_TABLE_GET;
          comando.infoDim = sizeof(msg_DHCP_bind_table_request_t);
        }
        break;

      case 1242:
        {
          msg_DHCPv4v6_bind_table_t *ptr;

          // Validate number of arguments (flow_id + 2 pairs port+svid)
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_DHCPv4v6_bind_table_t *) &(comando.info[0]);
          memset(ptr,0,sizeof(msg_DHCPv4v6_bind_table_t));

          ptr->SlotId = (uint8)-1;

          ptr->bind_table_total_entries = 1;
          ptr->bind_table_msg_size      = 1;
          ptr->bind_table[0].entry_index= 0;

          // MAC address
          if (convert_macaddr2array(argv[3+0],ptr->bind_table[0].macAddr)<0)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_DHCP_BIND_TABLE_REMOVE;
          comando.infoDim = sizeof(msg_DHCPv4v6_bind_table_t);
        }
        break;

      /* Get interface IGMP stats */
      case 1310:
        {
          msg_IgmpClientStatistics_t *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_IgmpClientStatistics_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_IgmpClientStatistics_t));

          ptr->SlotId = (uint8)-1;

          // flow_id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->mcEvcId = (uint32) valued;

          // Port
          if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;
          ptr->mask |= MSG_INTERFACE_MASK;

          // Client vlan
          if (argc>=3+3)
          {
            /* Also inner vlan should be provided */
            if (argc<3+4)
            {
              help_oltBuga();
              exit(0);
            }

            /* Interface */
            ptr->client.intf.intf_type = ptr->intf.intf_type;
            ptr->client.intf.intf_id   = ptr->intf.intf_id;
            ptr->client.mask |= MSG_CLIENT_INTF_MASK;

            /* Outer vlan */
            if (StrToLongLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->client.outer_vlan = (uint16) valued;
            ptr->client.mask |= MSG_CLIENT_OVLAN_MASK;

            /* Inner vlan */
            if (StrToLongLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->client.inner_vlan = (uint16) valued;
            ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;

            ptr->mask = MSG_CLIENT_MASK;
            comando.msgId = CCMSG_ETH_IGMP_CLIENT_STATS_GET;
          }
          else
          {
            comando.msgId = CCMSG_ETH_IGMP_INTF_STATS_GET;
          }

          comando.infoDim = sizeof(msg_IgmpClientStatistics_t);
        }
        break;

      /* Clear interface IGMP stats */
      case 1312:
        {
          msg_IgmpClientStatistics_t *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_IgmpClientStatistics_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_IgmpClientStatistics_t));

          ptr->SlotId = (uint8)-1;

          // flow_id
          ptr->mcEvcId = (uint32)-1;
          if (argc>=3+1)
          {
            if (StrToLongLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->mcEvcId = (uint32) valued;
          }

          // Port
          if (argc>=3+2)
          {
            if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
            {
              help_oltBuga();
              exit(0);
            }
            ptr->intf.intf_type = (uint8) type;
            ptr->intf.intf_id   = (uint8) intf;
            ptr->mask |= MSG_INTERFACE_MASK;
          }

          // Client vlan
          if (argc>=3+3)
          {
            /* Also inner vlan should be provided */
            if (argc<3+4)
            {
              help_oltBuga();
              exit(0);
            }

            /* Interface */
            ptr->client.intf.intf_type = ptr->intf.intf_type;
            ptr->client.intf.intf_id   = ptr->intf.intf_id;
            ptr->client.mask |= MSG_CLIENT_INTF_MASK;

            /* Outer vlan */
            if (StrToLongLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->client.outer_vlan = (uint16) valued;
            ptr->client.mask |= MSG_CLIENT_OVLAN_MASK;

            if (StrToLongLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->client.inner_vlan = (uint16) valued;
            ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;

            ptr->mask = MSG_CLIENT_MASK;
            comando.msgId = CCMSG_ETH_IGMP_CLIENT_STATS_CLEAR;
          }
          else
          {
            comando.msgId = CCMSG_ETH_IGMP_INTF_STATS_CLEAR;
          }

          comando.infoDim = sizeof(msg_IgmpClientStatistics_t);
        }
        break;

      case 1320:
        {
          msg_DhcpClientStatistics_t *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_DhcpClientStatistics_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_DhcpClientStatistics_t));

          ptr->SlotId = (uint8)-1;

          // flow_id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evc_id = (uint32) valued;

          // Port
          if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;
          ptr->mask |= MSG_INTERFACE_MASK;

          // Client vlan
          if (argc>=3+3)
          {
            if (StrToLongLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->client.outer_vlan = (uint16) valued;
            ptr->client.mask |= MSG_CLIENT_OVLAN_MASK;

            ptr->client.intf.intf_type = ptr->intf.intf_type;
            ptr->client.intf.intf_id   = ptr->intf.intf_id;
            ptr->client.mask |= MSG_CLIENT_INTF_MASK;

            ptr->mask = MSG_CLIENT_MASK;
            comando.msgId = CCMSG_ETH_DHCP_CLIENT_STATS_GET;
          }
          else
          {
            comando.msgId = CCMSG_ETH_DHCP_INTF_STATS_GET;
          }

          comando.infoDim = sizeof(msg_DhcpClientStatistics_t);
        }
        break;

      case 1322:
        {
          msg_DhcpClientStatistics_t *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_DhcpClientStatistics_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_DhcpClientStatistics_t));

          ptr->SlotId = (uint8)-1;

          // flow_id
          ptr->evc_id = (uint32)-1;
          if (argc>=3+1)
          {
            if (StrToLongLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->evc_id = (uint32) valued;
          }

          // Port
          if (argc>=3+2)
          {
            if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
            {
              help_oltBuga();
              exit(0);
            }
            ptr->intf.intf_type = (uint8) type;
            ptr->intf.intf_id   = (uint8) intf;
            ptr->mask |= MSG_INTERFACE_MASK;
          }

          // Client vlan
          if (argc>=3+3)
          {
            if (StrToLongLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->client.inner_vlan = (uint16) valued;
            ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;

            ptr->client.intf.intf_type = ptr->intf.intf_type;
            ptr->client.intf.intf_id   = ptr->intf.intf_id;
            ptr->client.mask |= MSG_CLIENT_INTF_MASK;

            ptr->mask = MSG_CLIENT_MASK;
            comando.msgId = CCMSG_ETH_DHCP_CLIENT_STATS_CLEAR;
          }
          else
          {
            comando.msgId = CCMSG_ETH_DHCP_INTF_STATS_CLEAR;
          }

          comando.infoDim = sizeof(msg_DhcpClientStatistics_t);
        }
        break;

      case 1400:
      {
        msg_IgmpProxyCfg_t *ptr;
        int index, ret;
        char param[21], value[21];

        // Validate number of arguments
        if (argc<3+0)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_IgmpProxyCfg_t *) &(comando.info[0]);

        // Clear structure
        memset(ptr,0x00,sizeof(msg_IgmpProxyCfg_t));

        ptr->SlotId = (uint8)-1;
        ptr->querier.flags = 0x0001;

        for (index=(3+0); index<argc; index++)
        {
          param[0]='\0';
          value[0]='\0';
          if ((ret=sscanf(argv[index],"%20[^ \t:=]=%20s",param,value))!=2)
          {
            printf("Invalid syntax: use <param1>=<value1> <param2>=<value2> ... (%d param=\"%s\" value=\"%s\")\r\n",ret,param,value);
            exit(0);
          }

          if (strcmp(param,"slot")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid slot id\r\n");
              exit(0);
            }
            ptr->SlotId = (uint8) valued;
          }
          else if (strcmp(param,"admin")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid admin value\r\n");
              exit(0);
            }
            ptr->admin = (uint8) valued;
            ptr->mask |= 0x01;
          }
          else if (strcmp(param,"ipaddr")==0)
          {
            if (convert_ipaddr2uint64(value,&valued)<0)
            {
              printf("Invalid ip address\r\n");
              exit(0);
            }
            ptr->ipv4_addr.s_addr = (uint32) valued;
            ptr->mask |= 0x04;
          }
          else if (strcmp(param,"cos")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid group membership interval\r\n");
              exit(0);
            }
            ptr->igmp_cos = (uint8) valued;
            ptr->mask |= 0x08;
          }
          else if (strcmp(param,"gmi")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid group membership interval\r\n");
              exit(0);
            }
            ptr->querier.group_membership_interval = (uint16) valued;
            ptr->mask |= 0x20;
            ptr->querier.mask |= 0x0010;
            ptr->querier.flags = 0x0000;
          }
          else if (strcmp(param,"qi")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid query interval value\r\n");
              exit(0);
            }
            ptr->querier.query_interval = (uint16) valued;
            ptr->mask |= 0x20;
            ptr->querier.mask |= 0x0004;
          }
          else
          {
            printf("Invalid param\r\n");
            exit(0);
          }
        }

        comando.msgId = CCMSG_ETH_IGMP_PROXY_SET;
        comando.infoDim = sizeof(msg_IgmpProxyCfg_t);
      }
      break;

      case 1401:
      case 1402:
        {
          msg_IgmpMultcastUnicastLink_t *ptr;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_IgmpMultcastUnicastLink_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_IgmpMultcastUnicastLink_t));

          ptr->SlotId = (uint8)-1;

          // Multicast EVC id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->multicastEvcId = (uint32) valued;

          // Unicast EVC id
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->unicastEvcId = (uint32) valued;

          if (msg==1401)
          {
            comando.msgId = CCMSG_ETH_IGMP_ENTRY_ADD;
          }
          else
          {
            comando.msgId = CCMSG_ETH_IGMP_ENTRY_REMOVE;
          }
          comando.infoDim = sizeof(msg_IgmpMultcastUnicastLink_t);
        }
        break;

    case 1403:
      {
        msg_MCAssocChannel_t *ptr;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_MCAssocChannel_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_MCAssocChannel_t));

        ptr->SlotId = (uint8)-1;

        // Multicast EVC id
        if (StrToLongLong(argv[3+0],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->evcid_mc = (uint32) valued;

        /* Start index (default is zero) */
        if (argc>=3+2)
        {
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->entry_idx = (uint16) valued;
        }

        comando.msgId = CCMSG_ETH_IGMP_CHANNEL_ASSOC_GET;
        comando.infoDim = sizeof(msg_MCAssocChannel_t);
      }
      break;

    case 1404:
    case 1405:
      {
        msg_MCAssocChannel_t *ptr;

        // Validate number of arguments
        if (argc<3+2)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_MCAssocChannel_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_MCAssocChannel_t));
        /* IPv4 */
        ptr->channel_dstIp.family = PTIN_AF_INET;
        ptr->channel_srcIp.family = PTIN_AF_INET;
        /* Default Mask */
        ptr->channel_dstmask = 32;
        ptr->channel_srcmask = 32;

        ptr->SlotId = (uint8)-1;

        // Multicast EVC id
        if (StrToLongLong(argv[3+0],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->evcid_mc = (uint32) valued;

        // Group Address
        if (convert_ipaddr2uint64(argv[3+1],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->channel_dstIp.addr.ipv4 = (uint32) valued;

        /* Source address */
        if (argc>=3+3)
        {
          if (convert_ipaddr2uint64(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->channel_srcIp.addr.ipv4 = (uint32) valued;
        }

        /* Group address mask bits */
        if (argc>=3+4)
        {
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->channel_dstmask = (uint8) valued;
        }

        /* Source address mask bits */
        if (argc>=3+5)
        {
          if (StrToLongLong(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->channel_srcmask = (uint8) valued;
        }

        if (msg==1404)
        {
          comando.msgId = CCMSG_ETH_IGMP_CHANNEL_ASSOC_ADD;
        }
        else
        {
          comando.msgId = CCMSG_ETH_IGMP_CHANNEL_ASSOC_REMOVE;
        }
        comando.infoDim = sizeof(msg_MCAssocChannel_t);
      }
      break;

    case 1406:
    case 1407:
      {
        msg_IgmpClient_t *ptr;
        int type, intf;

        // Validate number of arguments
        if (argc<3+4)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_IgmpClient_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_IgmpClient_t));

        // Multicast EVC id
        if (StrToLongLong(argv[3+0],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->mcEvcId = (uint32) valued;

        // port
        if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->client.intf.intf_type = (uint8) type;
        ptr->client.intf.intf_id   = (uint8) intf;
        ptr->client.mask |= MSG_CLIENT_INTF_MASK;

        // Outer vlan
        if (StrToLongLong(argv[3+2],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->client.outer_vlan = (uint16) valued;
        ptr->client.mask |= MSG_CLIENT_OVLAN_MASK;

        // Client vlan
        if (StrToLongLong(argv[3+3],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->client.inner_vlan = (uint16) valued;
        ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;

        if (msg==1406)
          comando.msgId = CCMSG_ETH_IGMP_CLIENT_ADD;
        else
          comando.msgId = CCMSG_ETH_IGMP_CLIENT_REMOVE;
        comando.infoDim = sizeof(msg_IgmpClient_t);
      }
      break;

/*
      case 1400:
        {
          st_snooping *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(st_snooping));

          // Snooping admin
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->snooping_admin = (uint8) valued & 1;

          // Default IGMP prio
          ptr->igmp_packet_cos = 5;

          // Querier admin
          if (argc>=3+2) {
            if (StrToLongLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->querier_admin = (uint8) valued & 1;
            ptr->querier_ipaddress = 0;
            ptr->querier_interval = 60;
            ptr->group_membership_interval = 260;
            ptr->igmp_version = 2;
            ptr->igmp_version_router = 2;
          }
          // Querier IP addr
          if (argc>=3+3) {
            if (convert_ipaddr2uint64(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->querier_ipaddress = valued;
          }
          // Querier interval
          if (argc>=3+4) {
            if (convert_ipaddr2uint64(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->querier_interval = (uint16) valued;
            ptr->group_membership_interval = (uint16) 260;
          }
          // Querier interval
          if (argc>=3+5) {
            if (convert_ipaddr2uint64(argv[3+4],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->igmp_packet_cos = (uint8) (valued & 0x07);
          }

          comando.msgId = CHMSG_ETH_SNOOP_MODE;
          comando.infoDim = sizeof(st_snooping);
        }
        break;

        // IGMP snooping: Add interfaces
      case 1402:
        {
          st_snooping_intf *ptr;
          uint8 port, type, i;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping_intf *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(st_snooping_intf));
          // Initialize all ports, as "nothing to be done"
          for (port=0; port<SYSTEM_N_PORTS; port++) {
            ptr->intf_type[port] = PTIN_SNOOP_INTF_NONE;
          }

          for (i=1; i<(argc-3) && i<SYSTEM_N_PORTS*2; i+=2) {
            if (StrToLongLong(argv[3+i-1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            port = (uint8) valued;
            if (port>=SYSTEM_N_PORTS)  continue;

            if (StrToLongLong(argv[3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            type = (uint8) valued;

            // Client interface
            if (type==0) {
              ptr->intf_type[port] = PTIN_SNOOP_INTF_CLIENT;
              ptr->intf_config[port].admin = 1;
              ptr->intf_config[port].fastleave_admin_mode = 1;
              ptr->intf_config[port].group_membership_interval = 260;
              ptr->intf_config[port].max_response_time = 10;
              ptr->intf_config[port].mrouter_present_expiration_time = 0;
            }
            // Router interface
            else  {
              ptr->intf_type[port] = PTIN_SNOOP_INTF_ROUTER;
              ptr->intf_config[port].admin = 1;
            }
          }

          comando.msgId = CHMSG_ETH_SNOOP_INTF_MANGMT;
          comando.infoDim = sizeof(st_snooping_intf);
        }
        break;

        // IGMP snooping remove interfaces
      case 1403:
        {
          st_snooping_intf *ptr;
          uint8 port, i;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping_intf *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(st_snooping_intf));
          // Initialize all ports, as "nothing to be done"
          for (port=0; port<SYSTEM_N_PORTS; port++) {
            ptr->intf_type[port] = PTIN_SNOOP_INTF_NONE;
          }

          for (i=0; i<(argc-3) && i<SYSTEM_N_PORTS; i++) {
            if (StrToLongLong(argv[3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            port = (uint8) valued;
            if (port>=SYSTEM_N_PORTS)  continue;
            ptr->intf_type[port] = PTIN_SNOOP_INTF_DISABLED;
            ptr->intf_config[port].admin = 0;
          }

          comando.msgId = CHMSG_ETH_SNOOP_INTF_MANGMT;
          comando.infoDim = sizeof(st_snooping_intf);
        }
        break;

        // IGMP snooping: Add vlans for mclient interfaces
      case 1405:
        {
          st_snooping_vlan *ptr;
          uint8 i, index=0;
          uint16 vlan, xlate;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping_vlan *) &(comando.info[0]);

          for (i=1; i<(argc-3) && i<200; i+=2) {

            if (StrToLongLong(argv[3+i-1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            vlan = (uint16) valued;

            if (StrToLongLong(argv[3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            xlate = (uint16) valued;

            index = i/2;
            ptr[index].port.type = PTIN_SNOOP_INTF_CLIENT;
            ptr[index].snoop_vlan = vlan;
            ptr[index].snoop_vlan_xlate = xlate;
            ptr[index].snoop_vlan_config.admin = 1;
            ptr[index].snoop_vlan_config.fastleave_admin_mode = 1;
            ptr[index].snoop_vlan_config.group_membership_interval = 260;
            ptr[index].snoop_vlan_config.max_response_time = 10;
            ptr[index].snoop_vlan_config.mrouter_present_expiration_time = 0;
          }

          comando.msgId = CHMSG_ETH_SNOOP_VLAN_MANGMT;
          comando.infoDim = sizeof(st_snooping_vlan)*(i/2);
        }
        break;

        // IGMP snooping: Remove vlans for mclient interfaces
      case 1406:
        {
          st_snooping_vlan *ptr;
          uint8 i;
          uint16 vlan;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping_vlan *) &(comando.info[0]);

          for (i=0; i<(argc-3) && i<100; i++) {
            if (StrToLongLong(argv[3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            vlan = (uint16) valued;
            ptr[i].port.intf  = 0;
            ptr[i].port.type  = PTIN_SNOOP_INTF_CLIENT;
            ptr[i].snoop_vlan = vlan;
            ptr[i].snoop_vlan_xlate = vlan;
            ptr[i].snoop_vlan_config.admin = 0;
          }

          comando.msgId = CHMSG_ETH_SNOOP_VLAN_MANGMT;
          comando.infoDim = sizeof(st_snooping_vlan)*i;
        }
        break;

        // IGMP snooping: Add vlans for mrouter interfaces
      case 1408:
        {
          st_snooping_vlan *ptr;
          uint8 port;
          uint8 i;
          uint16 vlan;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping_vlan *) &(comando.info[0]);

          // Extract port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          port = (uint8) valued;
          if (port>=SYSTEM_N_PORTS) {
            help_oltBuga();
            exit(0);
          }

          for (i=0; i<(argc-3-1) && i<100; i++) {
            if (StrToLongLong(argv[3+1+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            vlan = (uint16) valued;

            ptr[i].port.intf = port;
            ptr[i].port.type = PTIN_SNOOP_INTF_ROUTER;
            ptr[i].snoop_vlan = vlan;
            ptr[i].snoop_vlan_xlate = vlan;
            ptr[i].snoop_vlan_config.admin = 1;
          }

          comando.msgId = CHMSG_ETH_SNOOP_VLAN_MANGMT;
          comando.infoDim = sizeof(st_snooping_vlan)*i;
        }
        break;

        // IGMP snooping: Remove vlans for mrouter interfaces
      case 1409:
        {
          st_snooping_vlan *ptr;
          uint8 port;
          uint8 i;
          uint16 vlan;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping_vlan *) &(comando.info[0]);

          // Extract port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          port = (uint8) valued;
          if (port>=SYSTEM_N_PORTS) {
            help_oltBuga();
            exit(0);
          }

          for (i=0; i<(argc-3-1) && i<100; i++) {
            if (StrToLongLong(argv[3+1+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            vlan = (uint16) valued;

            ptr[i].port.intf = port;
            ptr[i].port.type = PTIN_SNOOP_INTF_ROUTER;
            ptr[i].snoop_vlan = vlan;
            ptr[i].snoop_vlan_xlate = vlan;
            ptr[i].snoop_vlan_config.admin = 0;
          }

          comando.msgId = CHMSG_ETH_SNOOP_VLAN_MANGMT;
          comando.infoDim = sizeof(st_snooping_vlan)*i;
        }
        break;

        // IGMP snooping: Add vlans for the querier
      case 1411:
        {
          st_snooping_querier_vlan *ptr;
          uint8 i, index=0;
          uint16 vlan;
          uint32 ipaddr;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping_querier_vlan *) &(comando.info[0]);

          for (i=1; i<(argc-3) && i<200; i+=2) {

            if (StrToLongLong(argv[3+i-1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            vlan = (uint16) valued;

            if (convert_ipaddr2uint64(argv[3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ipaddr = valued;

            index = i/2;
            ptr[index].querier_vlan = vlan;
            ptr[index].querier_vlan_admin = 1;
            ptr[index].querier_ipaddress = ipaddr;
            ptr[index].querier_election_participate_mode = 1;
          }

          comando.msgId = CHMSG_ETH_SNOOP_QUERIER_MANGMT;
          comando.infoDim = sizeof(st_snooping_querier_vlan)*(i/2);
        }
        break;

        // IGMP snooping: Remove vlans for the querier
      case 1412:
        {
          st_snooping_querier_vlan *ptr;
          uint8 i;
          uint16 vlan;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (st_snooping_querier_vlan *) &(comando.info[0]);

          for (i=0; i<(argc-3) && i<100; i++) {
            if (StrToLongLong(argv[3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            vlan = (uint16) valued;
            ptr[i].querier_vlan = vlan;
            ptr[i].querier_vlan_admin = 0;
          }

          comando.msgId = CHMSG_ETH_SNOOP_QUERIER_MANGMT;
          comando.infoDim = sizeof(st_snooping_querier_vlan)*i;
        }
        break;
*/
      case 1420:
        {
          msg_MCActiveChannelsRequest_t *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_MCActiveChannelsRequest_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_MCActiveChannelsRequest_t));

          ptr->SlotId = (uint8)-1;

          // EVC id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evc_id = (uint32) valued;

          // Page index
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->entryId = (uint16) valued;

          // port
          if (argc>=3+3)
          {
            if (sscanf(argv[3+2],"%d/%d",&type,&intf)!=2)
            {
              help_oltBuga();
              exit(0);
            }
            ptr->client.intf.intf_type = (uint8) type;
            ptr->client.intf.intf_id   = (uint8) intf;
            ptr->client.mask |= MSG_CLIENT_INTF_MASK;
          }

          // Outer vlan
          if (argc>=3+4)
          {
            if (StrToLongLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->client.outer_vlan = (uint16) valued;
            ptr->client.mask |= MSG_CLIENT_OVLAN_MASK;
          }

          // Client vlan
          if (argc>=3+5)
          {
            if (StrToLongLong(argv[3+4],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->client.inner_vlan = (uint16) valued;
            ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;
          }

          comando.msgId = CCMSG_ETH_IGMP_GROUPS_GET;
          comando.infoDim = sizeof(msg_MCActiveChannelsRequest_t);
        }
        break;

      case 1421:
        {
          msg_MCActiveChannelClientsRequest_t *ptr;

          // Validate number of arguments
          if (argc<4+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_MCActiveChannelClientsRequest_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_MCActiveChannelClientsRequest_t));

          ptr->SlotId = (uint8)-1;

          // EVC id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evc_id = (uint32) valued;

          // Page index
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->page_index = (uint16) valued;

          if (argc>=3+3)
          {
            // Channel IP
            if (convert_ipaddr2uint64(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->channelIp.s_addr = (L7_uint32) valued;
          }

          if (argc>=3+4)
          {
            // Channel IP
            if (convert_ipaddr2uint64(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->sourceIp.s_addr = (L7_uint32) valued;
          }

          comando.msgId = CCMSG_ETH_IGMP_CLIENT_GROUPS_GET;
          comando.infoDim = sizeof(msg_MCActiveChannelClientsRequest_t);
        }
        break;

      case 1430:
        {
          msg_MCStaticChannel_t *ptr;

          // Validate number of arguments
          if (argc<3+3)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_MCStaticChannel_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_MCStaticChannel_t));

          ptr->SlotId = (uint8)-1;

          // EVC id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evc_id = (uint32) valued;

          // Channel IP
          if (convert_ipaddr2uint64(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->channelIp.s_addr = valued;

          if (convert_ipaddr2uint64(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sourceIp.s_addr = valued;

          comando.msgId = CCMSG_ETH_IGMP_STATIC_GROUP_ADD;
          comando.infoDim = sizeof(msg_MCStaticChannel_t);
        }
        break;

      case 1431:
        {
          msg_MCStaticChannel_t *ptr;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_MCStaticChannel_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_MCStaticChannel_t));

          ptr->SlotId = (uint8)-1;

          // EVC id
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evc_id = (uint32) valued;

          // Channel IP
          if (convert_ipaddr2uint64(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->channelIp.s_addr = valued;

          if (convert_ipaddr2uint64(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sourceIp.s_addr = valued;

          comando.msgId = CCMSG_ETH_IGMP_STATIC_GROUP_REMOVE;
          comando.infoDim = sizeof(msg_MCStaticChannel_t);
        }
        break;

      case 1810:
        {
          msg_RoutingIpv4Intf *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+4)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingIpv4Intf *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_RoutingIpv4Intf));

          ptr->slotId = (uint8)-1;

          // Routing port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->routingIntf.intf_type = (uint8) type;
          ptr->routingIntf.intf_id   = (uint8) intf;

          // EVC ID
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evcId = (uint16) valued;

          // IP address
          if (convert_ipaddr2uint64(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->ipAddress = valued;

          // Subnet Mask
          if (convert_ipaddr2uint64(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->subnetMask = valued;

          // MTU
          if (StrToLongLong(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->mtu = valued;

          ptr->mask = CCMSG_ROUTING_INTF_MASK_INTF | CCMSG_ROUTING_INTF_MASK_EVCID | CCMSG_ROUTING_INTF_MASK_IPADDR | CCMSG_ROUTING_INTF_MASK_SUBNETMASK;

          comando.msgId = CCMSG_ROUTING_INTF_CREATE;
          comando.infoDim = sizeof(msg_RoutingIpv4Intf);
        }
        break;

      case 1811:
        {
          msg_RoutingIpv4Intf *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingIpv4Intf *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_RoutingIpv4Intf));

          ptr->slotId = (uint8)-1;

          // Routing port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->routingIntf.intf_type = (uint8) type;
          ptr->routingIntf.intf_id   = (uint8) intf;

          ptr->mask = CCMSG_ROUTING_INTF_MASK_INTF;

          comando.msgId = CCMSG_ROUTING_INTF_REMOVE;
          comando.infoDim = sizeof(msg_RoutingIpv4Intf);
        }
        break;

      case 1820:
        {
          msg_RoutingArpTableRequest *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingArpTableRequest *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingArpTableRequest));

          ptr->slotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          ptr->lastIndex  = (L7_uint32)-1;
          ptr->mask       = 0x00;

          comando.msgId = CCMSG_ROUTING_ARPTABLE_GET;
          comando.infoDim = sizeof(msg_RoutingArpTableRequest);
        }
        break;

      case 1821:
        {
          msg_RoutingArpEntryPurge *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingArpEntryPurge *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingArpEntryPurge));

          ptr->slotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          // IP address
          if (convert_ipaddr2uint64(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->ipAddr = valued;

          comando.msgId = CCMSG_ROUTING_ARPENTRY_PURGE;
          comando.infoDim = sizeof(msg_RoutingArpEntryPurge);
        }
        break;

      case 1830:
        {
          msg_RoutingRouteTableRequest *ptr;
          int type, intf;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingRouteTableRequest *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingRouteTableRequest));

          ptr->slotId = (uint8)-1;

          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          ptr->lastIndex = (L7_uint32)-1;

          comando.msgId = CCMSG_ROUTING_ROUTETABLE_GET;
          comando.infoDim = sizeof(msg_RoutingRouteTableRequest);
        }
        break;

      case 1831:
        {
          msg_RoutingStaticRoute *ptr;

          // Validate number of arguments
          if (argc<3+5)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingStaticRoute *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingStaticRoute));

          ptr->slotId = (uint8)-1;

          // IP Address
          if (convert_ipaddr2uint64(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->dstIpAddr = valued;

          // Subnet Mask
          if (convert_ipaddr2uint64(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->subnetMask = valued;

          // Gateway
          if (convert_ipaddr2uint64(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->nextHopRtr = valued;

          // Preference
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->pref = (uint16) valued;

          // Is null route
          if (StrToLongLong(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->isNullRoute = (uint16) valued;

          comando.msgId = CCMSG_ROUTING_STATICROUTE_ADD;
          comando.infoDim = sizeof(msg_RoutingStaticRoute);
        }
        break;

      case 1832:
        {
          msg_RoutingStaticRoute *ptr;

          // Validate number of arguments
          if (argc<3+4)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingStaticRoute *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingStaticRoute));

          ptr->slotId = (uint8)-1;

          // IP Address
          if (convert_ipaddr2uint64(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->dstIpAddr = valued;

          // Subnet Mask
          if (convert_ipaddr2uint64(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->subnetMask = valued;

          // Gateway
          if (convert_ipaddr2uint64(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->nextHopRtr = valued;

          // Is null route
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->isNullRoute = (uint16) valued;

          comando.msgId = CCMSG_ROUTING_STATICROUTE_DELETE;
          comando.infoDim = sizeof(msg_RoutingStaticRoute);
        }
        break;

      case 1840:
        {
          msg_RoutingPingSessionCreate *ptr;

          // Validate number of arguments
          if (argc<3+5)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingPingSessionCreate *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingPingSessionCreate));

          ptr->slotId = (uint8)-1;

          // index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sessionIdx = valued;

          // Dst IP address
          if (convert_ipaddr2uint64(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->dstIpAddr = valued;

          // Probe count
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->probeCount = valued;

          // Probe size
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->probeSize = valued;

          // Probe interval
          if (StrToLongLong(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->probeInterval = valued;

          comando.msgId   = CCMSG_ROUTING_PINGSESSION_CREATE;
          comando.infoDim = sizeof(msg_RoutingPingSessionCreate);
        }
        break;

      case 1841:
        {
          msg_RoutingPingSessionQuery *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingPingSessionQuery *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingPingSessionQuery));

          ptr->slotId = (uint8)-1;

          // index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sessionIdx = valued;

          comando.msgId   = CCMSG_ROUTING_PINGSESSION_QUERY;
          comando.infoDim = sizeof(msg_RoutingPingSessionQuery);
        }
        break;

      case 1842:
        {
          msg_RoutingPingSessionFree *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingPingSessionFree *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingPingSessionFree));

          ptr->slotId = (uint8)-1;

          // index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sessionIdx = valued;

          comando.msgId   = CCMSG_ROUTING_PINGSESSION_FREE;
          comando.infoDim = sizeof(msg_RoutingPingSessionFree);
        }
        break;

      case 1850:
        {
          msg_RoutingTracertSessionCreate *ptr;

          // Validate number of arguments
          if (argc<3+5)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingTracertSessionCreate *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingTracertSessionCreate));

          ptr->slotId = (uint8)-1;

          // index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sessionIdx = valued;

          // Dst IP address
          if (convert_ipaddr2uint64(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->dstIpAddr = valued;

          // Probe per Hop count
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->probePerHop = valued;

          // Probe size
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->probeSize = valued;

          // Probe interval
          if (StrToLongLong(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->probeInterval = valued;

          // Dont frag flag
          if (StrToLongLong(argv[3+5],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->dontFrag = valued;

          // Port
          if (StrToLongLong(argv[3+6],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->port = valued;

          // Max Ttl
          if (StrToLongLong(argv[3+7],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->maxTtl = valued;

          // Init Ttl
          if (StrToLongLong(argv[3+8],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->initTtl = valued;

          // Max fails
          if (StrToLongLong(argv[3+9],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->maxFail = valued;

          comando.msgId   = CCMSG_ROUTING_TRACERTSESSION_CREATE;
          comando.infoDim = sizeof(msg_RoutingTracertSessionCreate);
        }
        break;

      case 1851:
        {
          msg_RoutingTracertSessionQuery *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingTracertSessionQuery *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingTracertSessionQuery));

          ptr->slotId = (uint8)-1;

          // index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sessionIdx = valued;

          comando.msgId   = CCMSG_ROUTING_TRACERTSESSION_QUERY;
          comando.infoDim = sizeof(msg_RoutingTracertSessionQuery);
        }
        break;

      case 1852:
        {
          msg_RoutingTracertSessionHopsRequest *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingTracertSessionHopsRequest *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingTracertSessionHopsRequest));

          ptr->slotId = (uint8)-1;

          // index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sessionIdx = valued;

          ptr->lastIndex = (L7_uint16)-1;

          comando.msgId   = CCMSG_ROUTING_TRACERTSESSION_GETHOPS;
          comando.infoDim = sizeof(msg_RoutingTracertSessionHopsRequest);
        }
        break;

      case 1853:
        {
          msg_RoutingTracertSessionFree *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_RoutingTracertSessionFree *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_RoutingTracertSessionFree));

          ptr->slotId = (uint8)-1;

          // index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->sessionIdx = valued;

          comando.msgId   = CCMSG_ROUTING_TRACERTSESSION_FREE;
          comando.infoDim = sizeof(msg_RoutingTracertSessionFree);
        }
        break;

      case 1500:
        {
          msg_LACPLagInfo_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_LACPLagInfo_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_LACPLagInfo_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // LAG index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP8((uint8) valued);

          comando.msgId = CCMSG_ETH_LACP_LAG_GET;
          comando.infoDim = sizeof(msg_LACPLagInfo_t);
        }
        break;

      case 1501:
        {
          msg_LACPLagInfo_t *ptr;

          // Validate number of arguments
          if (argc<3+4)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_LACPLagInfo_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_LACPLagInfo_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // LAG index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP8((uint8) valued);

          // Static mode
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->static_enable = ENDIAN_SWAP8((uint8) (valued & 1));

          // Load Balance mode
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->loadBalance_mode = ENDIAN_SWAP8((uint8) (valued & 0xFF));

          // Member ports bitmap
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->members_pbmp = ENDIAN_SWAP32((uint32) valued);
          ptr->members_pbmp2= ENDIAN_SWAP32((uint32) (valued>>32));

          ptr->admin = ENDIAN_SWAP8(1);
          ptr->stp_enable = ENDIAN_SWAP8(0);

          comando.msgId = CCMSG_ETH_LACP_LAG_ADD;
          comando.infoDim = sizeof(msg_LACPLagInfo_t);
        }
        break;

      case 1502:
        {
          msg_LACPLagInfo_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_LACPLagInfo_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_LACPLagInfo_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // LAG index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP8((uint8) valued);

          comando.msgId = CCMSG_ETH_LACP_LAG_REMOVE;
          comando.infoDim = sizeof(msg_LACPLagInfo_t);
        }
        break;

      case 1503:
        {
          msg_LACPLagStatus_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_LACPLagStatus_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_LACPLagStatus_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // LAG index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP8((uint8) valued);

          comando.msgId = CCMSG_ETH_LACP_LAG_STATUS_GET;
          comando.infoDim = sizeof(msg_LACPLagStatus_t);
        }
        break;

      case 1504:
        {
          msg_LACPAdminState_t *ptr;
          int port;
          uint8 i, state_aggregation, lacp_activity, lacp_timeout;

          // Validate number of arguments
          if (argc<3+4)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_LACPAdminState_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_LACPAdminState_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // Port index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          port = (uint8) valued;

          // Aggregation state
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          state_aggregation = (uint8) (valued & 1);

          // LACP Activity
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          lacp_activity = (uint8) (valued & 1);

          // Timeout
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          lacp_timeout = (uint8) (valued & 1);

          if (port==-1) {
            for (i=0; i<PTIN_SYSTEM_N_PORTS; i++) {
              ptr[i].id               = ENDIAN_SWAP8(i);
              ptr[i].state_aggregation= ENDIAN_SWAP8(state_aggregation);
              ptr[i].lacp_activity    = ENDIAN_SWAP8(lacp_activity);
              ptr[i].lacp_timeout     = ENDIAN_SWAP8(lacp_timeout);
            }
            comando.infoDim = sizeof(msg_LACPAdminState_t)*PTIN_SYSTEM_N_PORTS;
          }
          else if (port>=0 && port<PTIN_SYSTEM_N_PORTS) {
            ptr->id               = ENDIAN_SWAP8(port);
            ptr->state_aggregation= ENDIAN_SWAP8(state_aggregation);
            ptr->lacp_activity    = ENDIAN_SWAP8(lacp_activity);
            ptr->lacp_timeout     = ENDIAN_SWAP8(lacp_timeout);
            comando.infoDim = sizeof(msg_LACPAdminState_t);
          }
          else {
            help_oltBuga();
            exit(0);
          }
          comando.msgId = CCMSG_ETH_LACP_ADMINSTATE_SET;
        }
        break;

      case 1505:
        {
          msg_LACPAdminState_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_LACPAdminState_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_LACPAdminState_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // Port index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP8((uint8) ((int) valued));

          comando.msgId = CCMSG_ETH_LACP_ADMINSTATE_GET;
          comando.infoDim = sizeof(msg_LACPAdminState_t);
        }
        break;

      case 1510:
        {
          msg_LACPStats_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_LACPStats_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_LACPStats_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // Port index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP8((uint8) ((int) valued));

          comando.msgId = CCMSG_ETH_LACP_STATS_GET;
          comando.infoDim = sizeof(msg_LACPStats_t);
        }
        break;

      case 1511:
        {
          msg_LACPStats_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_LACPStats_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_LACPStats_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // Port index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP8((uint8) ((int) valued));

          ptr->LACPdus_rx = ENDIAN_SWAP32(0);
          ptr->LACPdus_tx = ENDIAN_SWAP32(0);

          comando.msgId = CCMSG_ETH_LACP_STATS_CLEAR;
          comando.infoDim = sizeof(msg_LACPStats_t);
        }
        break;

      case 1600:
        {
          msg_HwEthMef10Evc_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_EVC_GET;
          comando.infoDim = sizeof(msg_HwEthMef10Evc_t);

          // Pointer to data array
          ptr = (msg_HwEthMef10Evc_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HwEthMef10Evc_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // evc index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP32(valued);
        }
        break;

      case 1601:
        {
          msg_HwEthMef10Evc_t *ptr;
          int i;

          // Validate number of arguments
          if (argc<3+6)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_EVC_ADD;
          comando.infoDim = sizeof(msg_HwEthMef10Evc_t);

          // Pointer to data array
          ptr = (msg_HwEthMef10Evc_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HwEthMef10Evc_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // evc index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP32((uint32) valued);

          // P2P
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->flags |= (valued != 0) ? ((valued << 16) & 0x30000) : 0;
          // Stacked
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->flags |= valued != 0 ? 0x0004 : 0;
          // MAC Learning
          if (StrToLongLong(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->flags |= valued != 0 ? 0x0008 : 0;
          // Other masks
          if (StrToLongLong(argv[3+4], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->flags |= valued;
          ptr->flags = ENDIAN_SWAP32(ptr->flags);

          // MC Flood type
          if (StrToLongLong(argv[3+5], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->mc_flood = ENDIAN_SWAP8((uint8) valued);

          ptr->n_intf   = ENDIAN_SWAP8(argc - (3+6));

          // Interfaces...
          unsigned int intf, type, mef, vid, ivid;
          for (i=3+6; i<argc; i++) {
            printf("argv[%u]=%s  **  ", i, argv[i]);
            sscanf(argv[i], "%d/%d/%d/%d/%d", &type, &intf, &mef, &vid, &ivid);
            printf("%d/%d/%d/%d/%d\n", type, intf, mef, vid, ivid);
            ptr->intf[i-(3+6)].intf_type = ENDIAN_SWAP8 (type);
            ptr->intf[i-(3+6)].intf_id   = ENDIAN_SWAP8 (intf);
            ptr->intf[i-(3+6)].mef_type  = ENDIAN_SWAP8 (mef);
            ptr->intf[i-(3+6)].vid       = ENDIAN_SWAP16(vid);
            ptr->intf[i-(3+6)].inner_vid = ENDIAN_SWAP16(ivid);
          }
        }
        break;

      case 1602:
        {
          msg_HwEthMef10EvcRemove_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_EVC_REMOVE;
          comando.infoDim = sizeof(msg_HwEthMef10EvcRemove_t);

          // Pointer to data array
          ptr = (msg_HwEthMef10EvcRemove_t *) &(comando.info[0]);

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // evc index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = ENDIAN_SWAP32((uint32) valued);
        }
        break;

      case 1603:
      {
        msg_HWevcPort_t *ptr;
        int slotId, i;
        uint32 evc_idx;

        // Validate number of arguments
        if (argc<3+2)  {
          help_oltBuga();
          exit(0);
        }

        comando.msgId = CCMSG_ETH_EVC_PORT_ADD;
        comando.infoDim = 0;

        // Pointer to data array
        ptr = (msg_HWevcPort_t *) &(comando.info[0]);
        memset(ptr, 0x00, sizeof(msg_HWevcPort_t));

        // Slot id
        slotId = (uint8)-1;

        // evc index
        if (StrToLongLong(argv[3+0], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        evc_idx = valued;

        // Interfaces...
        unsigned int intf, type, mef, vid, ivid;
        for (i=0; i<(argc-(3+1)); i++) {
          printf("argv[%u]=%s  **  ", i+(3+1), argv[i+(3+1)]);
          sscanf(argv[i+(3+1)], "%d/%d/%d/%d/%d", &type, &intf, &mef, &vid, &ivid);
          printf("%d/%d/%d/%d/%d\n", type, intf, mef, vid, ivid);

          memset(&ptr[i], 0x00, sizeof(msg_HWevcPort_t));
          ptr[i].slotId         = ENDIAN_SWAP8 (slotId);
          ptr[i].evcId          = ENDIAN_SWAP32(evc_idx);
          ptr[i].intf.intf_type = ENDIAN_SWAP8 (type);
          ptr[i].intf.intf_id   = ENDIAN_SWAP8 (intf);
          ptr[i].intf.mef_type  = ENDIAN_SWAP8 (mef);
          ptr[i].intf.vid       = ENDIAN_SWAP16(vid);
          ptr[i].intf.inner_vid = ENDIAN_SWAP16(ivid);

          comando.infoDim += sizeof(msg_HWevcPort_t);
        }
      }
      break;

      case 1604:
      {
        msg_HWevcPort_t *ptr;
        int slotId, i;
        uint32 evc_idx;

        // Validate number of arguments
        if (argc<3+2)  {
          help_oltBuga();
          exit(0);
        }

        comando.msgId = CCMSG_ETH_EVC_PORT_REMOVE;
        comando.infoDim = 0;

        // Pointer to data array
        ptr = (msg_HWevcPort_t *) &(comando.info[0]);
        memset(ptr, 0x00, sizeof(msg_HWevcPort_t));

        // Slot id
        slotId = (uint8)-1;

        // evc index
        if (StrToLongLong(argv[3+0], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        evc_idx = valued;

        // Interfaces...
        unsigned int intf, type;
        for (i=0; i<(argc-(3+1)); i++) {
          printf("argv[%u]=%s  **  ", i+(3+1), argv[i+(3+1)]);
          sscanf(argv[i+(3+1)], "%d/%d", &type, &intf);
          printf("%d/%d\n", type, intf);

          memset(&ptr[i], 0x00, sizeof(msg_HWevcPort_t));
          ptr[i].slotId         = ENDIAN_SWAP8 (slotId);
          ptr[i].evcId          = ENDIAN_SWAP32(evc_idx);
          ptr[i].intf.intf_type = ENDIAN_SWAP8 (type);
          ptr[i].intf.intf_id   = ENDIAN_SWAP8 (intf);

          comando.infoDim += sizeof(msg_HWevcPort_t);
        }
      }
      break;

      /* "m 1605 EVC#[0-64] type[0-Phy;1-Lag] intf# Out.VLAN Inn.VLAN - Add P2P bridge on Stacked EVCs between the root and a leaf intf\r\n" */
      case 1605:
        {
          msg_HwEthEvcBridge_t *ptr;

          // Validate number of arguments
          if (argc<3+5)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_EVC_BRIDGE_ADD;
          comando.infoDim = sizeof(msg_HwEthEvcBridge_t);

          // Pointer to data array
          ptr = (msg_HwEthEvcBridge_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HwEthEvcBridge_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // EVC index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evcId = ENDIAN_SWAP32(valued);

          // Intf type
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = ENDIAN_SWAP8(valued);
          // Intf#
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_id = ENDIAN_SWAP8(valued);
          ptr->intf.mef_type = ENDIAN_SWAP8(1); // leaf

          // Outer VLAN
          if (StrToLongLong(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.vid = ENDIAN_SWAP16(valued);

          // Inner VLAN
          if (StrToLongLong(argv[3+4], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->inn_vlan = ENDIAN_SWAP16(valued);
        }
        break;

      /* "m 1606 EVC#[0-64] type[0-Phy;1-Lag] intf# Inn.VLAN - Deletes P2P bridge on Stacked EVCs between the root and a leaf intf\r\n" */
      case 1606:
        {
          msg_HwEthEvcBridge_t *ptr;

          // Validate number of arguments
          if (argc<3+4)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_EVC_BRIDGE_REMOVE;
          comando.infoDim = sizeof(msg_HwEthEvcBridge_t);

          // Pointer to data array
          ptr = (msg_HwEthEvcBridge_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HwEthEvcBridge_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // EVC index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evcId = ENDIAN_SWAP32(valued);

          // Intf type
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = ENDIAN_SWAP8(valued);
          // Intf#
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_id = ENDIAN_SWAP8(valued);
          ptr->intf.vid = ENDIAN_SWAP16(0xFFFF);

          // Inner VLAN
          if (StrToLongLong(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->inn_vlan = ENDIAN_SWAP16(valued);
        }
        break;

      /* "m 1607 EVC#[0-64] type[0-Phy;1-Lag] intf# Out.VLAN Inn.VLAN - Add GEM flow to EVC\r\n" */
      case 1607:
        {
          msg_HwEthEvcFlow_t *ptr;

          // Validate number of arguments
          if (argc<3+6)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_EVC_FLOW_ADD;
          comando.infoDim = sizeof(msg_HwEthEvcFlow_t);

          // Pointer to data array
          ptr = (msg_HwEthEvcFlow_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_HwEthEvcFlow_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // EVC index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evcId = ENDIAN_SWAP32(valued);

          // Intf type
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = ENDIAN_SWAP8(valued);
          // Intf#
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_id = ENDIAN_SWAP8(valued);

          // Outer VLAN
          if (StrToLongLong(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.outer_vid = ENDIAN_SWAP16(valued);

          // Inner VLAN
          if (StrToLongLong(argv[3+4], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.inner_vid = ENDIAN_SWAP16(valued);

          // NNI Client VLAN
          if (argc >= 3+6)
          {
            if (StrToLongLong(argv[3+5], &valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->nni_cvlan = ENDIAN_SWAP16(valued);
          }

          // Flags
          if (argc >= 3+7)
          {
            if (StrToLongLong(argv[3+6], &valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->flags = ENDIAN_SWAP32((uint32) valued);
          }

          // Maximum number of MAC addresses
          if (argc >= 3+8)
          {
            if (StrToLongLong(argv[3+7], &valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->macLearnMax = ENDIAN_SWAP8((uint8) valued);
          }
        }
        break;

    /* "m 1608 EVC#[0-64] type[0-Phy;1-Lag] intf# Out.VLAN Inn.VLAN - Remove a GEM flow from EVC\r\n" */
    case 1608:
      {
        msg_HwEthEvcFlow_t *ptr;

        // Validate number of arguments
        if (argc<3+4)  {
          help_oltBuga();
          exit(0);
        }

        comando.msgId = CCMSG_ETH_EVC_FLOW_REMOVE;
        comando.infoDim = sizeof(msg_HwEthEvcFlow_t);

        // Pointer to data array
        ptr = (msg_HwEthEvcFlow_t *) &(comando.info[0]);
        memset(ptr, 0x00, sizeof(msg_HwEthEvcFlow_t));

        // Slot id
        ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

        // EVC index
        if (StrToLongLong(argv[3+0], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->evcId = ENDIAN_SWAP32(valued);

        // Intf type
        if (StrToLongLong(argv[3+1], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = ENDIAN_SWAP8(valued);
        // Intf#
        if (StrToLongLong(argv[3+2], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_id = ENDIAN_SWAP8(valued);

        // Outer VLAN
        if (StrToLongLong(argv[3+3], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.outer_vid = ENDIAN_SWAP16(valued);
      }
      break;

    /* fp.cli m 1609 EVC#[0-64] [flags_value] [flags_mask] [mc_flood] [type] -> Change EVC options */
    case 1609:
      {
        msg_HwEthMef10EvcOptions_t *ptr;

        // Validate number of arguments
        if (argc<3+1)  {
          help_oltBuga();
          exit(0);
        }

        comando.msgId = CCMSG_ETH_EVC_OPTIONS_SET;
        comando.infoDim = sizeof(msg_HwEthMef10EvcOptions_t);

        // Pointer to data array
        ptr = (msg_HwEthMef10EvcOptions_t *) &(comando.info[0]);
        memset(ptr, 0x00, sizeof(msg_HwEthMef10EvcOptions_t));

        // Slot id
        ptr->SlotId = ENDIAN_SWAP8((uint8)-1);
        // Default Mask
        ptr->mask = ENDIAN_SWAP16(0);
        /* Default flags value */
        ptr->flags.value = ENDIAN_SWAP32(0);
        ptr->flags.mask  = ENDIAN_SWAP32(0xffffffff);

        // EVC index
        if (StrToLongLong(argv[3+0], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->id = ENDIAN_SWAP32(valued);

        // Type
        if (argc >= 3+2)
        {
          if (StrToLongLong(argv[3 + 1], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->type = ENDIAN_SWAP8((L7_uint8) valued);
          ptr->mask |= ENDIAN_SWAP16(PTIN_EVC_OPTIONS_MASK_TYPE);
        }

        // MC flood
        if (argc >= 3+3)
        {
          if (StrToLongLong(argv[3 + 2], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->mc_flood = ENDIAN_SWAP8((L7_uint8) valued);
          ptr->mask |= ENDIAN_SWAP16(PTIN_EVC_OPTIONS_MASK_MCFLOOD);
        }

        // Flags value
        if (argc >= 3+4)
        {
          if (StrToLongLong(argv[3 + 3], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->flags.value = ENDIAN_SWAP32((L7_uint32) valued);
          ptr->mask |= ENDIAN_SWAP16(PTIN_EVC_OPTIONS_MASK_FLAGS);
        }

        // Flags mask
        if (argc >= 3+5)
        {
          if (StrToLongLong(argv[3 + 4], &valued) < 0)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->flags.mask = ENDIAN_SWAP32((L7_uint32) valued);
        }
      }
      break;

      /* "m 1610 - Reads Network Connectivity (inBand) configuration\r\n" */
      case 1610:
        {
          msg_NtwConnectivity_t *ptr;

          // Validate number of arguments
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_NtwConnectivity_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_NtwConnectivity_t));

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          ptr->mask = ENDIAN_SWAP32(0xFFFFFFFF);

          comando.msgId = CCMSG_ETH_NTW_CONNECTIVITY_GET;
          comando.infoDim = sizeof(msg_NtwConnectivity_t);
        }
        break;

      /* "m 1611 <intf_type[0:phy 1:lag]> <intf#> <ipaddr> <netmask> <gateway> <managememt_vlan> - Sets Network Connectivity (inBand) configuration\r\n" */
      case 1611:
        {
          msg_NtwConnectivity_t *ptr;

          // Validate number of arguments
          if (argc<3+6)  {
            help_oltBuga();
            exit(0);
          }

          comando.msgId = CCMSG_ETH_NTW_CONNECTIVITY_SET;
          comando.infoDim = sizeof(msg_NtwConnectivity_t);

          // Pointer to data array
          ptr = (msg_NtwConnectivity_t *) &(comando.info[0]);
          memset(ptr, 0x00, sizeof(msg_NtwConnectivity_t));

          ptr->mask = ENDIAN_SWAP32(0xFFFFFFFF);

          // Slot id
          ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

          // Intf type
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf[0].intf_type = ENDIAN_SWAP8(valued);
          // Intf #
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf[0].intf_id = ENDIAN_SWAP8(valued);

          ptr->n_intf = ENDIAN_SWAP8(1);

          // IP Addr
          if (convert_ipaddr2uint64(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->ipaddr = ENDIAN_SWAP32(valued);

          // NetMask
          if (convert_ipaddr2uint64(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->netmask = ENDIAN_SWAP32(valued);

          // Gateway
          if (convert_ipaddr2uint64(argv[3+4], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->gateway = ENDIAN_SWAP32(valued);

          // VLAN
          if (StrToLongLong(argv[3+5], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->mgmtVlanId = ENDIAN_SWAP16(valued);
        }
        break;

    case 1620:
    case 1621:
    case 1622:
      {
        msg_HwEthBwProfile_t *ptr;
        int index, ret;
        int type, intf;
        char param[21], value[21];

        // Validate number of arguments
        if (argc<3+2)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_HwEthBwProfile_t *) &(comando.info[0]);

        // Clear structure
        memset(ptr,0x00,sizeof(msg_HwEthBwProfile_t));

        ptr->SlotId      = ENDIAN_SWAP8((uint8)-1);
        ptr->evcId       = ENDIAN_SWAP32((uint32)-1);
        ptr->profile.cir = ENDIAN_SWAP64((uint64)-1);
        ptr->profile.eir = ENDIAN_SWAP64((uint64)-1);
        ptr->profile.cbs = ENDIAN_SWAP64((uint64)-1);
        ptr->profile.ebs = ENDIAN_SWAP64((uint64)-1);

        for (index=(3+0); index<argc; index++)
        {
          param[0]='\0';
          value[0]='\0';
          if ((ret=sscanf(argv[index],"%20[^ \t:=]=%20s",param,value))!=2)
          {
            printf("Invalid syntax: use <param1>=<value1> <param2>=<value2> ... (%d param=\"%s\" value=\"%s\")\r\n",ret,param,value);
            exit(0);
          }

          if (strcmp(param,"slot")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid slot id\r\n");
              exit(0);
            }
            ptr->SlotId = ENDIAN_SWAP8((uint8) valued);
          }
          else if (strcmp(param,"evc")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid evc value\r\n");
              exit(0);
            }
            ptr->evcId = ENDIAN_SWAP32((uint32) valued);
          }
          else if (strcmp(param,"intf")==0 || strcmp(param,"intfsrc")==0)
          {
            if (sscanf(value,"%d/%d",&type,&intf)!=2)
            {
              printf("Invalid intf/intfsrc value\r\n");
              exit(0);
            }
            ptr->intf_src.intf_type = ENDIAN_SWAP8((uint8) type);
            ptr->intf_src.intf_id   = ENDIAN_SWAP8((uint8) intf);
            ptr->mask |= ENDIAN_SWAP8(MSG_HWETH_BWPROFILE_MASK_INTF_SRC);
          }
          else if (strcmp(param,"intfdst")==0)
          {
            if (sscanf(value,"%d/%d",&type,&intf)!=2)
            {
              printf("Invalid intfdst value\r\n");
              exit(0);
            }
            ptr->intf_dst.intf_type = ENDIAN_SWAP8((uint8) type);
            ptr->intf_dst.intf_id   = ENDIAN_SWAP8((uint8) intf);
            ptr->mask |= ENDIAN_SWAP8(MSG_HWETH_BWPROFILE_MASK_INTF_DST);
          }
          else if (strcmp(param,"svid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid svid value\r\n");
              exit(0);
            }
            ptr->service_vlan = ENDIAN_SWAP16((uint16) valued);
            ptr->mask |= ENDIAN_SWAP8(MSG_HWETH_BWPROFILE_MASK_SVLAN);
          }
          else if (strcmp(param,"cvid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid cvid value\r\n");
              exit(0);
            }
            ptr->client_vlan = ENDIAN_SWAP16((uint16) valued);
            ptr->mask |= ENDIAN_SWAP8(MSG_HWETH_BWPROFILE_MASK_CVLAN);
          }
          else if (strcmp(param,"cir")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid cir value\r\n");
              exit(0);
            }
            ptr->profile.cir = ENDIAN_SWAP64((uint64) valued*1000000);
            ptr->mask |= ENDIAN_SWAP8(MSG_HWETH_BWPROFILE_MASK_PROFILE);
          }
          else if (strcmp(param,"eir")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid eir value\r\n");
              exit(0);
            }
            ptr->profile.eir = ENDIAN_SWAP64((uint64) valued*1000000);
            ptr->mask |= ENDIAN_SWAP8(MSG_HWETH_BWPROFILE_MASK_PROFILE);
          }
          else if (strcmp(param,"cbs")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid cbs value\r\n");
              exit(0);
            }
            ptr->profile.cbs = ENDIAN_SWAP64((uint64) valued);
            ptr->mask |= ENDIAN_SWAP8(MSG_HWETH_BWPROFILE_MASK_PROFILE);
          }
          else if (strcmp(param,"ebs")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid ebs value\r\n");
              exit(0);
            }
            ptr->profile.ebs = ENDIAN_SWAP64((uint64) valued);
            ptr->mask |= ENDIAN_SWAP8(MSG_HWETH_BWPROFILE_MASK_PROFILE);
          }
          else
          {
            printf("Invalid param\r\n");
            exit(0);
          }
        }

        switch (msg)
        {
        case 1620:
          comando.msgId = CCMSG_ETH_BW_PROFILE_GET;
          break;
        case 1621:
          comando.msgId = CCMSG_ETH_BW_PROFILE_SET;
          break;
        case 1622:
          comando.msgId = CCMSG_ETH_BW_PROFILE_DELETE;
          break;
        }
        comando.infoDim = sizeof(msg_HwEthBwProfile_t);
      }
      break;

    case 1624:
    case 1625:
    case 1626:
      {
        msg_HwEthStormControl_t *ptr;
        int index, ret;
        char param[21], value[21];

        // Validate number of arguments
        if (argc<3+0)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_HwEthStormControl_t *) &(comando.info[0]);

        // Clear structure
        memset(ptr,0x00,sizeof(msg_HwEthStormControl_t));

        ptr->SlotId = ENDIAN_SWAP8((uint8)-1);
        ptr->id = ENDIAN_SWAP32(0);

        for (index=(3+0); index<argc; index++)
        {
          param[0]='\0';
          value[0]='\0';
          if ((ret=sscanf(argv[index],"%20[^ \t:=]=%20s",param,value))!=2)
          {
            printf("Invalid syntax: use <param1>=<value1> <param2>=<value2> ... (%d param=\"%s\" value=\"%s\")\r\n",ret,param,value);
            exit(0);
          }

          if (strcmp(param,"slot")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid slot id\r\n");
              exit(0);
            }
            ptr->SlotId = ENDIAN_SWAP8((uint8) valued);
          }
          else if (strcmp(param,"bc")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid Broadcast rate value\r\n");
              exit(0);
            }
            ptr->bcast_rate = ENDIAN_SWAP32((uint32) valued);
            ptr->mask |= ENDIAN_SWAP16(MSG_STORMCONTROL_MASK_BCAST);
          }
          else if (strcmp(param,"mc")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid Multicast rate value\r\n");
              exit(0);
            }
            ptr->mcast_rate = ENDIAN_SWAP32((uint32) valued);
            ptr->mask |= ENDIAN_SWAP16(MSG_STORMCONTROL_MASK_MCAST);
          }
          else if (strcmp(param,"uc")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid Unknown Unicast rate value\r\n");
              exit(0);
            }
            ptr->ucast_unknown_rate = ENDIAN_SWAP32((uint32) valued);
            ptr->mask |= ENDIAN_SWAP16(MSG_STORMCONTROL_MASK_UCUNK);
          }
          else
          {
            printf("Invalid param\r\n");
            exit(0);
          }
        }

        switch (msg)
        {
        case 1624:
          comando.msgId = CCMSG_ETH_STORM_CONTROL_SET;
          break;
        case 1625:
          comando.msgId = CCMSG_ETH_STORM_CONTROL_RESET;
          break;
        case 1626:
          comando.msgId = CCMSG_ETH_STORM_CONTROL_CLEAR;
          break;
        default:
          printf("Invalid Message id (%u)\r\n",msg);
          exit(0);
        }
        comando.infoDim = sizeof(msg_HwEthStormControl_t);
      }
      break;

    case 1627:
    case 1628:
    case 1629:
      {
        msg_HwEthStormControl2_t *ptr;
        int type, intf;
        L7_uint32 *rate_value;
        L7_uint8  *rate_units;

        // Validate number of arguments
        if (argc<3+2)  {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_HwEthStormControl2_t *) &(comando.info[0]);

        // Clear structure
        memset(ptr,0x00,sizeof(msg_HwEthStormControl2_t));

        ptr->SlotId = ENDIAN_SWAP8((uint8)-1);

        /* Set correct mask */
        switch (msg)
        {
        case 1627:
          rate_value = &ptr->broadcast.rate_value;
          rate_units = &ptr->broadcast.rate_units;
          ptr->mask  = ENDIAN_SWAP8(MSG_STORMCONTROL_MASK_BCAST);
          break;
        case 1628:
          rate_value = &ptr->multicast.rate_value;
          rate_units = &ptr->multicast.rate_units;
          ptr->mask = ENDIAN_SWAP8(MSG_STORMCONTROL_MASK_MCAST);
          break;
        case 1629:
          rate_value = &ptr->unknown_uc.rate_value;
          rate_units = &ptr->unknown_uc.rate_units;
          ptr->mask = ENDIAN_SWAP8(MSG_STORMCONTROL_MASK_UCUNK);
          break;
        default:
          printf("Invalid Message id (%u)\r\n",msg);
          exit(0);
        }

        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = ENDIAN_SWAP8((uint8) type);
        ptr->intf.intf_id   = ENDIAN_SWAP8((uint8) intf);

        // Enable
        if (StrToLongLong(argv[3+1], &valued) < 0)
        {
          help_oltBuga();
          exit(0);
        }

        /* Disabled? */
        if (valued == 0)
        {
          *rate_value = ENDIAN_SWAP32((uint32) -1);
        }
        /* If to enable, check additional values */
        else
        {
          /* Minimum value */
          *rate_value = ENDIAN_SWAP32(1);

          /* Threshold limit */
          if (argc >= 3+3)
          {
            if (StrToLongLong(argv[3+2], &valued) < 0)
            {
              help_oltBuga();
              exit(0);
            }
            *rate_value = ENDIAN_SWAP32((uint32) valued);
          }
          /* Threshold units */
          if (argc >= 3+4)
          {
            if (StrToLongLong(argv[3+3], &valued) < 0)
            {
              help_oltBuga();
              exit(0);
            }
            *rate_units = ENDIAN_SWAP8((uint8) valued);
          }
        }
        
        comando.msgId = CCMSG_ETH_STORMCONTROL2_SET;
        comando.infoDim = sizeof(msg_HwEthStormControl2_t);
      }
      break;

      // EVC Counters
      case 1630:
      case 1632:
      case 1633:
        {
          msg_evcStats_t *ptr;
          int index, ret;
          int type, intf;
          char param[21], value[21];

          // Validate number of arguments (at least evcid)
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_evcStats_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_evcStats_t));
          ptr->SlotId= ENDIAN_SWAP8((uint8)-1);
          ptr->evc_id= ENDIAN_SWAP32((uint32)-1);

          for (index=(3+0); index<argc; index++)
          {
            param[0]='\0';
            value[0]='\0';
            if ((ret=sscanf(argv[index],"%20[^ \t:=]=%20s",param,value))!=2)
            {
              printf("Invalid syntax: use <param1>=<value1> <param2>=<value2> ... (%d param=\"%s\" value=\"%s\")\r\n",ret,param,value);
              exit(0);
            }

            if (strcmp(param,"slot")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid slot id\r\n");
                exit(0);
              }
              ptr->SlotId = ENDIAN_SWAP8((uint8) valued);
            }
            else if (strcmp(param,"evc")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid evc value\r\n");
                exit(0);
              }
              ptr->evc_id = ENDIAN_SWAP32((uint32) valued);
            }
            else if (strcmp(param,"intf")==0)
            {
              if (sscanf(value,"%d/%d",&type,&intf)!=2)
              {
                printf("Invalid intf/intfsrc value\r\n");
                exit(0);
              }
              ptr->intf.intf_type = ENDIAN_SWAP8((uint8) type);
              ptr->intf.intf_id   = ENDIAN_SWAP8((uint8) intf);
              ptr->mask |= ENDIAN_SWAP8(MSG_EVC_COUNTERS_MASK_INTF);
            }
            else if (strcmp(param,"svid")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid svid value\r\n");
                exit(0);
              }
              ptr->service_vlan = ENDIAN_SWAP16((uint16) valued);
              ptr->mask |= ENDIAN_SWAP8(MSG_EVC_COUNTERS_MASK_SVLAN);
            }
            else if (strcmp(param,"cvid")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid cvid value\r\n");
                exit(0);
              }
              ptr->client_vlan = ENDIAN_SWAP16((uint16) valued);
              ptr->mask |= ENDIAN_SWAP8(MSG_EVC_COUNTERS_MASK_CVLAN);
            }
            else if (strcmp(param,"channel")==0)
            {
              if (convert_ipaddr2uint64(value,&valued)<0)
              {
                printf("Invalid channel IP value (d.d.d.d format)\r\n");
                exit(0);
              }
              ptr->channel_ip = ENDIAN_SWAP32((uint32) valued);
              ptr->mask |= ENDIAN_SWAP8(MSG_EVC_COUNTERS_MASK_CHANNEL);
            }
            else
            {
              printf("Invalid param\r\n");
              exit(0);
            }
          }

          switch (msg)
          {
          case 1630:
            comando.msgId = CCMSG_ETH_EVC_COUNTERS_GET;
            break;
          case 1632:
            comando.msgId = CCMSG_ETH_EVC_COUNTERS_ADD;
            break;
          case 1633:
            comando.msgId = CCMSG_ETH_EVC_COUNTERS_REMOVE;
            break;
          }
          comando.infoDim = sizeof(msg_evcStats_t);
        }
        break;
      case 1700:      
        {
          msg_IPSG_set_t *ptr;
          int                       type;
          int                       intf;
          
          // Validate number of arguments
          if (argc<3+2)  {
            help_oltBuga();
            exit(0);
          }
          
          // Pointer to data array
          ptr = (msg_IPSG_set_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_IPSG_set_t));
    
          // port
          if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          // verifySource
          if (StrToLongLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
          }
          ptr->enable = (uint8) (IPSG_ENABLE & valued);

          comando.msgId = CCMSG_ETH_IPSG_ENABLE;
          comando.infoDim = sizeof(msg_IPSG_set_t);          
        }
        break;      
      case 1701:
        {
          msg_IPSG_static_entry_t *ptr;
          int                      type;
          int                      intf;

          // Validate number of arguments
          if (argc<3+6)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_IPSG_static_entry_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_IPSG_static_entry_t));

          // IdType: idType=1 [evcId]; idType=2 [rootVlan]
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->idType = (uint8) (IPSG_ID_ALL & valued);

         // Id
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = (uint32) valued;

          // Ptin Port
          if (sscanf(argv[3+2],"%d/%d",&type,&intf)!=2)
          {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = (uint8) type;
          ptr->intf.intf_id   = (uint8) intf;

          // MAC address
          if (convert_macaddr2array(argv[3+3],ptr->macAddr)<0)  {
            help_oltBuga();
            exit(0);
          }          

          /* IPv4 Address */          
          if (convert_ipaddr2uint64(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->ipAddr.addr.ipv4 = (uint32) valued;
          ptr->ipAddr.family = PTIN_AF_INET;
         
          // Static Entry Action: 0 -Remove | 1 - Add           
          if (StrToLongLong(argv[3+5],&valued)<0)  {
              help_oltBuga();
              exit(0);
          }
          ptr->action = (uint8) (valued & IPSG_ACTION_ADD);

          comando.msgId = CCMSG_ETH_IPSG_STATIC_ENTRY;
          comando.infoDim = sizeof(msg_IPSG_static_entry_t); 
        }
        break;
//     "m 1710 packageId(0-255) - Add Multicast Package\n\r"
//     "m 1711 packageId(0-255) - Remove Multicast Package\n\r"
    case 1710:
    case 1711:
      {
        msg_igmp_package_t *ptr;        
        int packageId;

        // Validate number of arguments
        if (argc!=3+1)
        {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_igmp_package_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_igmp_package_t));

        // Package Id
        if (StrToLongLong(argv[3+0],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        packageId = (uint32) valued;

        if (packageId != PTIN_SYSTEM_IGMP_MAXPACKAGES && packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES)
        {
          printf("Invalid PackageId:%u >Max_Packages:%u\n", packageId, PTIN_SYSTEM_IGMP_MAXPACKAGES);
          exit(0);
        }

        /*Add All Packages*/
        if (packageId == PTIN_SYSTEM_IGMP_MAXPACKAGES)
        {
          packageId = 0;
          for ( packageId = 0; packageId < PTIN_SYSTEM_IGMP_MAXPACKAGES; packageId++ )
          {
            BITMAP_BIT_SET(ptr->packageBmpList, packageId, UINT32_BITSIZE);
            ptr->noOfPackages++;
          }
        }
        else
        {
          BITMAP_BIT_SET(ptr->packageBmpList, packageId, UINT32_BITSIZE);
          ptr->noOfPackages++;
        }

        if (msg==1710)
          comando.msgId = CCMSG_IGMP_PACKAGES_ADD;
        else
          comando.msgId = CCMSG_IGMP_PACKAGES_REMOVE;
        comando.infoDim = sizeof(msg_igmp_package_t);
      }
      break;
//   "m 1712 packageId(0-255) evcId# groupAddr(d.d.d.d) groupMask# sourceAddr(d.d.d.d) sourceMask# - Add Multicast Channels to Package\n\r"
//   "m 1713 packageId(0-255) evcId# groupAddr(d.d.d.d) groupMask# sourceAddr(d.d.d.d) sourceMask# - Add Multicast Channels to Package\n\r"
    case 1712:
    case 1713:
      {
        msg_igmp_package_channels_t *ptr;        

        // Validate number of arguments
        if (argc!=3+6)
        {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_igmp_package_channels_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_igmp_package_channels_t));

        /* Package Id */
        if (StrToLongLong(argv[3+0],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->packageId = (uint32) valued;

        /* Service Id */
        if (StrToLongLong(argv[3+1],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->evcId = (uint32) valued;

        /*Group Address */          
        if (convert_ipaddr2uint64(argv[3+2],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->groupAddr.addr.ipv4 = (uint32) valued;
        ptr->groupAddr.family = PTIN_AF_INET;

        /* Group Mask */
        if (StrToLongLong(argv[3+3],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->groupMask = (uint8) valued;

        /*Source Address */          
        if (convert_ipaddr2uint64(argv[3+4],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->sourceAddr.addr.ipv4 = (uint32) valued;
        ptr->sourceAddr.family = PTIN_AF_INET;

        /* Source Mask */
        if (StrToLongLong(argv[3+5],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->sourceMask = (uint8) valued;

        if (msg==1712)
          comando.msgId = CCMSG_IGMP_PACKAGE_CHANNELS_ADD;
        else
          comando.msgId = CCMSG_IGMP_PACKAGE_CHANNELS_REMOVE;
        comando.infoDim = sizeof(msg_igmp_package_channels_t);
      }
      break;
//    "m 1714 evcId# intfType/intf# onuId# - Add Multicast Service\n\r"
//    "m 1715 evcId# intfType/intf# onuId# - Remove Multicast Service\n\r"
    case 1714:
    case 1715:
      {
        msg_multicast_service_t *ptr;    
        uint type, intf;    

        // Validate number of arguments
        if (argc!=3+3)
        {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_multicast_service_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_multicast_service_t));

        /* Service Id */
        if (StrToLongLong(argv[3+0],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->evcId = (uint32) valued;

        // Ptin Port
        if (sscanf(argv[3+1],"%u/%u",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = (uint8) type;
        ptr->intf.intf_id   = (uint8) intf;

        /* Onu Id */
        if (StrToLongLong(argv[3+2],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->onuId = (uint8) valued;

        if (msg==1714)
          comando.msgId = CCMSG_MULTICAST_SERVICE_ADD;
        else
          comando.msgId = CCMSG_MULTICAST_SERVICE_REMOVE;
        comando.infoDim = sizeof(msg_multicast_service_t);
      }
      break;
//    "m 1716 evcId# intfType/intf# ovid(0-4095) cvid(0-4095) onuId(0-127) packageId(0-255) - Igmp Unicast Client Add \r\n"
//    "m 1717 evcId# intfType/intf# ovid(0-4095) cvid(0-4095) onuId(0-127) packageId(0-255) - Igmp Unicast Client Remove\r\n"
    case 1716:
    case 1717:
      {
        msg_igmp_unicast_client_packages_t *ptr;
        int type, intf, packageId;

        // Validate number of arguments
        if (argc!=3+6)
        {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_igmp_unicast_client_packages_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_igmp_unicast_client_packages_t));

        // Multicast EVC id
        if (StrToLongLong(argv[3+0],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->evcId = (uint32) valued;

        // port
        if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->client.intf.intf_type = (uint8) type;
        ptr->client.intf.intf_id   = (uint8) intf;
        ptr->client.mask |= MSG_CLIENT_INTF_MASK;

        // Outer vlan
        if (StrToLongLong(argv[3+2],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->client.outer_vlan = (uint16) valued;
        ptr->client.mask |= MSG_CLIENT_OVLAN_MASK;

        // Client vlan
        if (StrToLongLong(argv[3+3],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->client.inner_vlan = (uint16) valued;
        ptr->client.mask |= MSG_CLIENT_IVLAN_MASK;

        // Onu Id
        if (StrToLongLong(argv[3+4], &valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->onuId = (L7_uint8) valued;

        // Package Id
        if (StrToLongLong(argv[3+5],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        packageId = (uint32) valued;

        if (packageId != PTIN_SYSTEM_IGMP_MAXPACKAGES && packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES)
        {
          printf("Invalid PackageId:%u >Max_Packages:%u\n", packageId, PTIN_SYSTEM_IGMP_MAXPACKAGES);
          exit(0);
        }

        /*Add All Packages*/
        if (packageId == PTIN_SYSTEM_IGMP_MAXPACKAGES)
        {
          packageId = 0;
          for ( packageId = 0; packageId < PTIN_SYSTEM_IGMP_MAXPACKAGES; packageId++ )
          {
            BITMAP_BIT_SET(ptr->packageBmpList, packageId, UINT32_BITSIZE);
            ptr->noOfPackages++;
          }
        }
        else
        {
          BITMAP_BIT_SET(ptr->packageBmpList, packageId, UINT32_BITSIZE);
          ptr->noOfPackages++;
        }

        if (msg==1716)
          comando.msgId = CCMSG_IGMP_UNICAST_CLIENT_PACKAGES_ADD;
        else
          comando.msgId = CCMSG_IGMP_UNICAST_CLIENT_PACKAGES_REMOVE;
        comando.infoDim = sizeof(msg_igmp_unicast_client_packages_t);
      }
      break;
//    "m 1718 evcId# intfType/intf# ovid(0-4095) ivid(0-4095) cvid(0-4095)  onuId(0-127) packageId(0-255) - Igmp MacBridge Client Add \r\n"
//    "m 1719 evcId# intfType/intf# ovid(0-4095) ivid(0-4095) cvid(0-4095)  onuId(0-127) packageId(0-255) - Igmp MacBridge Client Remove \r\n"
    case 1718:
    case 1719:
      {
        msg_igmp_macbridge_client_packages_t *ptr;
        int type, intf, packageId;

        // Validate number of arguments
        if (argc!=3+7)
        {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_igmp_macbridge_client_packages_t *) &(comando.info[0]);
        memset(ptr, 0x00, sizeof(msg_igmp_macbridge_client_packages_t));


        // EVC index
        if (StrToLongLong(argv[3+0], &valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->evcId = (L7_uint32) valued;

        // port
        if (sscanf(argv[3+1],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = (L7_uint8) type;
        ptr->intf.intf_id = (L7_uint8) intf;

        // Outer VLAN
        if (StrToLongLong(argv[3+2], &valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.outer_vid = (L7_uint16) valued;

        // Inner VLAN
        if (StrToLongLong(argv[3+3], &valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.inner_vid = (L7_uint16) valued;

        if (StrToLongLong(argv[3+4], &valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->nni_cvlan = (L7_uint16) valued;

        // Onu Id
        if (StrToLongLong(argv[3+5], &valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->onuId = (L7_uint8) valued;

        // Package Id
        if (StrToLongLong(argv[3+6],&valued)<0)
        {
          help_oltBuga();
          exit(0);
        }
        packageId = (uint32) valued;

        if (packageId != PTIN_SYSTEM_IGMP_MAXPACKAGES && packageId >= PTIN_SYSTEM_IGMP_MAXPACKAGES)
        {
          printf("Invalid PackageId:%u >Max_Packages:%u\n", packageId, PTIN_SYSTEM_IGMP_MAXPACKAGES);
          exit(0);
        }

        /*Add All Packages*/
        if (packageId == PTIN_SYSTEM_IGMP_MAXPACKAGES)
        {
          for ( packageId = 0; packageId < PTIN_SYSTEM_IGMP_MAXPACKAGES; packageId++ )
          {
            BITMAP_BIT_SET(ptr->packageBmpList, packageId, UINT32_BITSIZE);
            ptr->noOfPackages++;
          }
        }
        else
        {
          BITMAP_BIT_SET(ptr->packageBmpList, packageId, UINT32_BITSIZE);
          ptr->noOfPackages++;
        }

        if (msg==1718)
          comando.msgId = CCMSG_IGMP_MACBRIDGE_CLIENT_PACKAGES_ADD;
        else
          comando.msgId = CCMSG_IGMP_MACBRIDGE_CLIENT_PACKAGES_REMOVE;
        comando.infoDim = sizeof(msg_igmp_macbridge_client_packages_t);
      }
      break;

      /* Set Port Mirroring */
      case 1885:
        {
          msg_port_mirror_t *ptr;

          int ret,index;
          int type, intf;
          char param[21], value[21];

          /* Validate number of arguments */
          if (argc < 3+1)  {
            help_oltBuga();
            exit(0);
          }

          ptr = (msg_port_mirror_t *) &(comando.info[0]);
          
          ptr->slotId = (uint8)-1;
          ptr->sessionId = 1;
          ptr->mask = PORT_MIRROR_MASK_NONE;
          ptr->sessionMode = 0;
          ptr->dst_intf.intf_type = 0;
          ptr->dst_intf.intf_id   = 0;
          ptr->n_intf = 0;

          memset(ptr->src_intf, 0, PTIN_SYSTEM_MAX_N_PORTS);
 
          for (index=(3+0); index<argc; index++)
          {
            param[0]='\0';
            value[0]='\0';
            if ((ret=sscanf(argv[index],"%20[^ \t:=]=%20s",param,value))!=2)
            {
              printf("Invalid syntax: use <param1>=<value1> <param2>=<value2> ... (%d param=\"%s\" value=\"%s\")\r\n",ret,param,value);
              exit(0);
            }

            if (strcmp(param,"mode")==0 || strcmp(param,"sessionMode")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid sessionMode \r\n");
                exit(0);
              }
              ptr->sessionMode = (uint8) valued;

              ptr->mask |= PORT_MIRROR_MASK_sessionMode;
            }
            else if (strcmp(param,"dstintf")==0 || strcmp(param,"dst_intf")==0 || strcmp(param,"dstport")==0 || strcmp(param,"dst_port")==0)
            {
              if (sscanf(value,"%d/%d",&type,&intf)!=2)
              {
                printf("Invalid port value\r\n");
                exit(0);
              }
              ptr->dst_intf.intf_type = type;
              ptr->dst_intf.intf_id   = intf;

              ptr->mask |= PORT_MIRROR_MASK_dst_intf;
            }
            else if (strcmp(param,"srcintf")==0 || strcmp(param,"src_intf")==0 || strcmp(param,"srcport")==0 || strcmp(param,"src_port")==0)
            {
              if (sscanf(value,"%d/%d",&type,&intf)!=2)
              {
                printf("Invalid port value\r\n");
                exit(0);
              }

              if (ptr->n_intf >= PTIN_SYSTEM_MAX_N_PORTS) {
                printf("Too may interaces specified \r\n");
                exit(0);
              }

              ptr->src_intf[ptr->n_intf].intf.intf_type = type;
              ptr->src_intf[ptr->n_intf].intf.intf_id   = intf;

              index++;

              if (index >= argc) {
                printf("Too few arguments. Direction must be specified \r\n");
                exit(0);
              }

              param[0]='\0';
              value[0]='\0';

              if ((ret=sscanf(argv[index],"%20[^ \t:=]=%20s",param,value))!=2)
              {
                printf("Invalid syntax: use <param1>=<value1> <param2>=<value2> ... (%d param=\"%s\" value=\"%s\")\r\n",ret,param,value);
                exit(0);
              }

              if (strcmp(param,"dir")==0 || strcmp(param,"direction")==0)
              {

                if (StrToLongLong(value,&valued)<0)
                {
                  printf("Invalid direction value\r\n");
                  exit(0);
                }

                ptr->src_intf[ptr->n_intf].direction = valued;
              }
              else
              {
                printf("direction must be specified \r\n");
                exit(0);
              }

              ptr->n_intf += 1;
              ptr->mask |= PORT_MIRROR_MASK_src_intf;
            }
            else
            {
              printf("Invalid param\r\n");
              exit(0);
            }
          }

          comando.msgId = CCMSG_PORTMIRROR_SESSION_ADD;
          comando.infoDim = sizeof(msg_port_mirror_t);
        }
        break;

//    "m 1890 port# idx# packet_number(1-65535) packet_size(64-1500) period(0-127) dmmCosColor(0-7) - MEP Frame Delay Write \r\n"
//    "m 1891 port# idx# packet_number(1-65535) packet_size(64-1500) period(0-127) dmmCosColor(0-7) - MEP Frame Delay Remove \r\n"
    case 1890:
    case 1891:
      {
    	  msg_bd_mep_dm_t *ptr;
        uint port, mep_idx, packet_size, packet_number, period, dmmCosColor;

        // Validate number of arguments
        if (argc!=3+6)
        {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (msg_bd_mep_dm_t *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(msg_bd_mep_dm_t));

        /* Port number */
        if (sscanf(argv[3+0],"%u",&port)!=1)
        {
          help_oltBuga();
          printf("Invalid port \n");
          exit(0);
        }
        ptr->port = (uint16) port;

        /* MEP id */
        if (sscanf(argv[3+1],"%u",&mep_idx)!=1)
        {
          help_oltBuga();
          printf("Invalid mep_idx \n");
          exit(0);
        }
        ptr->idx = (uint16) mep_idx;

        /* Number of packets */
        if (sscanf(argv[3+2],"%u",&packet_number)!=1)
        {
          help_oltBuga();
          printf("Invalid packet_number\n");
          exit(0);
        }
        ptr->packet_number = (uint16) packet_number;

        /* Packet size */
        if (sscanf(argv[3+3],"%u",&packet_size)!=1)
        {
          help_oltBuga();
          printf("Invalid packet_size \n");
          exit(0);
        }
        ptr->packet_size = (uint16) packet_size;


        /* Period */
        if (sscanf(argv[3+4],"%u",&period)!=1)
        {
          help_oltBuga();
          printf("Invalid period \n");
          exit(0);
        }
        ptr->period = (uint16) period;

        /* CoS Color */
        if (sscanf(argv[3+5],"%u",&dmmCosColor)!=1)
        {
          help_oltBuga();
          printf("Invalid dmmCosColor \n");
          exit(0);
        }
        ptr->dmmCosColor = (uint8) dmmCosColor;

        if (msg==1890)
          comando.msgId = CCMSG_WR_MEP_DM;
        else
          comando.msgId = CCMSG_RM_MEP_DM;
        comando.infoDim = sizeof(msg_bd_mep_dm_t);
      }
      break;
//    "m 1892 port# idx# status(0-2) - MEP Frame Delay get results \r\n"
    case 1892:
      {
    	MSG_FRAMEDELAY_status *ptr;
        uint status, idx;
        ulong port;

        // Validate number of arguments
        if (argc!=3+3)
        {
          help_oltBuga();
          exit(0);
        }

        // Pointer to data array
        ptr = (MSG_FRAMEDELAY_status *) &(comando.info[0]);
        memset(ptr,0x00,sizeof(MSG_FRAMEDELAY_status));

        /* port */
        if (sscanf(argv[3+0],"%lu",&port)!=1)
        {
          help_oltBuga();
          printf("Invalid port \n");
          exit(0);
        }
        ptr->port = (uint32) port;

        /* index */
        if (sscanf(argv[3+1],"%u",&idx)!=1)
        {
          help_oltBuga();
          printf("Invalid idx \n");
          exit(0);
        }
        ptr->idx = (uint16) idx;

        /* status */
        if (sscanf(argv[3+2],"%u",&status)!=1)
        {
          help_oltBuga();
          printf("Invalid status\n");
          exit(0);
        }
        ptr->status = (uint16) status;

        comando.msgId = CHMSG_CCM_MEP_FRAMEDELAY;
        comando.infoDim = sizeof(MSG_FRAMEDELAY_status);
      }
      break;

//    "m 1893 port# idx# type(0-1) lmmPeriod(1-7) lmmCosColor(0-1) flrCosColor(0-2) - MEP Frame Loss Write \r\n"
//    "m 1894 port# idx# type(0-1) lmmPeriod(1-7) lmmCosColor(0-1) flrCosColor(0-2) - MEP Frame Loss Remove \r\n"
	  case 1893:
	  case 1894:
		{
		  msg_bd_mep_lm_t *ptr;
		  uint port, mep_idx, type, lmmPeriod, lmmCosColor, flrCosColor;

		  // Validate number of arguments
		  if (argc!=3+6)
		  {
			help_oltBuga();
			exit(0);
		  }

		  // Pointer to data array
		  ptr = (msg_bd_mep_lm_t *) &(comando.info[0]);
		  memset(ptr,0x00,sizeof(msg_bd_mep_lm_t));

		  /* Port number */
		  if (sscanf(argv[3+0],"%u",&port)!=1)
		  {
			help_oltBuga();
			printf("Invalid port \n");
			exit(0);
		  }
		  ptr->port = (uint16) port;

		  /* MEP id */
		  if (sscanf(argv[3+1],"%u",&mep_idx)!=1)
		  {
			help_oltBuga();
			printf("Invalid mep_idx \n");
			exit(0);
		  }
		  ptr->idx = (uint16) mep_idx;

		  /* Number of packets */
		  if (sscanf(argv[3+2],"%u",&type)!=1)
		  {
			help_oltBuga();
			printf("Invalid type\n");
			exit(0);
		  }
		  ptr->type = (uint8) type;

		  /* Packet size */
		  if (sscanf(argv[3+3],"%u",&lmmPeriod)!=1)
		  {
			help_oltBuga();
			printf("Invalid lmmPeriod \n");
			exit(0);
		  }
		  ptr->lmmPeriod = (uint8) lmmPeriod;


		  /* Period */
		  if (sscanf(argv[3+4],"%u",&lmmCosColor)!=1)
		  {
			help_oltBuga();
			printf("Invalid lmmCosColor \n");
			exit(0);
		  }
		  ptr->lmmCosColor = (uint8) lmmCosColor;

		  /* CoS Color */
		  if (sscanf(argv[3+5],"%u",&flrCosColor)!=1)
		  {
			help_oltBuga();
			printf("Invalid flrCosColor \n");
			exit(0);
		  }
		  ptr->flrCosColor = (uint8) flrCosColor;

		  if (msg==1893)
			comando.msgId = CCMSG_WR_MEP_LM;
		  else
			comando.msgId = CCMSG_RM_MEP_LM;
		  comando.infoDim = sizeof(msg_bd_mep_lm_t);
		}
		break;
//    "m 1895 port# idx#  mask(0-15) - MEP Frame Loss get results \r\n"
	  case 1895:
		{
		MSG_FRAMELOSS_status *ptr;
		  uint mask, idx;
		  ulong port;

		  // Validate number of arguments
		  if (argc!=3+3)
		  {
			help_oltBuga();
			exit(0);
		  }

		  // Pointer to data array
		  ptr = (MSG_FRAMELOSS_status *) &(comando.info[0]);
		  memset(ptr,0x00,sizeof(MSG_FRAMELOSS_status));

		  /* port */
		  if (sscanf(argv[3+0],"%lu",&port)!=1)
		  {
			help_oltBuga();
			printf("Invalid port \n");
			exit(0);
		  }
		  ptr->port = (uint32) port;

		  /* index */
		  if (sscanf(argv[3+1],"%u",&idx)!=1)
		  {
			help_oltBuga();
			printf("Invalid idx \n");
			exit(0);
		  }
		  ptr->idx = (uint16) idx;

		  /* status */
		  if (sscanf(argv[3+2],"%u",&mask)!=1)
		  {
			help_oltBuga();
			printf("Invalid mask\n");
			exit(0);
		  }
		  ptr->mask = 0x0C; //(uint16) mask;    //SET to 0x0C

		  comando.msgId = CHMSG_CCM_MEP_FRAMELOSS;
		  comando.infoDim = sizeof(MSG_FRAMELOSS_status);
		}
		break;

      default:
        printf("A mensagem %d nao esta implementada!!\n\r", msg);
        exit(0);
        break;
    }
    canal_buga=open_ipc(PORTO_TX_MSG_BUGA,IP_LOCALHOST,NULL,20);
    if ( canal_buga<0 )
    {
      printf("Erro no open IPC do BUGA...\n\r");
      exit(0);
    }

    #if 0
    unsigned int i;

    printf("Comando:\r\n");
    for (i=0; i<sizeof(pc_type)-(sizeof(unsigned char)*PC_MsgDimMax)+comando.infoDim; i++)
    {
      if (i%16 == 0)  printf("0x%04x   ", i);

      printf("%02x ",*(((char *) &comando)+i));

      if ((i+1)%16 == 0)  printf("\r\n");
    }
    #endif

    // 2 - enviar mensagem e receber resposta
    //printf("comando.msgId %08x\n\r", comando.msgId);

    valued = send_data (canal_buga, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta);

    close_ipc(canal_buga);
    if ( valued )
    {
      printf("Erro %llu  no send_data IPC do BUGA...\n\r", valued);
      exit(0);
    }

    #if 0
    printf("Resposta:\r\n");
    for (i=0; i<sizeof(pc_type)-(sizeof(unsigned char)*PC_MsgDimMax)+resposta.infoDim; i++)
    {
      if (i%16 == 0)  printf("0x%04x   ", i);

      printf("%02x ",*(((char *) &resposta)+i));

      if ((i+1)%16 == 0)  printf("\r\n");
    }
    #endif

    switch (msg) {

      case 1997:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MC machine resetted\n\r");
        else
          printf(" Switch: Error resetting MC machine - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;
      
      case 1998:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Alarms reset done\n\r");
        else
          printf(" Switch: Alarms reset failed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1999:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Defaults restored\n\r");
        else
          printf(" Switch: Reset defaults failed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 2000:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: L2 Aging time defined\n\r");
        else
          printf(" Switch: Error setting L2 aging time - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 2001:
      {
        msg_switch_config_t *po=(msg_switch_config_t *) &resposta.info[0];

        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
          printf(" Slot %u: Age time = %lu\n\r", ENDIAN_SWAP8(po->SlotId), ENDIAN_SWAP32(po->aging_time));
          printf("Switch: L2 Aging time read successfully\n\r");
        }
        else
          printf(" Switch: Error reading L2 switch age time\n\r");
      }
      break;

      case 1000:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Stdout redirected successfully\n\r");
        else
          printf(" Error redirecting Stdout - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1001:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Logger redirected successfully\n\r");
        else
          printf(" Error redirecting logger - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1003:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" New fw state applied\n\r");
        else
          printf(" Error setting new fw state - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1004:
        {
          int i, j;
          msg_ptin_policy_resources *po=(msg_ptin_policy_resources *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            // Validate size
            if (resposta.infoDim!=sizeof(msg_ptin_policy_resources)) {
              printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(msg_ptin_policy_resources),resposta.infoDim);
              break;
            }

            printf(" FFP available rules (Slot=%u):\r\n", ENDIAN_SWAP8(po->SlotId));
            for (j=0; j<PTIN_POLICY_MAX_STAGES; j++) {
              printf(" Stage=%s\r\n",((j==0) ? "Lookup" : ((j==1) ? "Ingress" : ((j==2) ? "Egress" : "Unknown") ) ) );
              for (i=0; i<PTIN_POLICY_MAX_GROUPS; i++) {
                if (ENDIAN_SWAP8(po->cap[i][j].inUse))
                {
                  printf("   Idx %u: Group %d (slice_width=%u)\r\n", i, (int) ENDIAN_SWAP32(po->cap[i][j].group_id), ENDIAN_SWAP16(po->cap[i][j].total.slice_width));
                  printf("     rules    = %03u / %03u (%u rules in use)\r\n", ENDIAN_SWAP16(po->cap[i][j].free.rules), ENDIAN_SWAP16(po->cap[i][j].total.rules), ENDIAN_SWAP16(po->cap[i][j].count.rules));
                  printf("     counters = %03u / %03u\r\n", ENDIAN_SWAP16(po->cap[i][j].free.counters), ENDIAN_SWAP16(po->cap[i][j].total.counters));
                  printf("     meters   = %03u / %03u\r\n", ENDIAN_SWAP16(po->cap[i][j].free.meters)  , ENDIAN_SWAP16(po->cap[i][j].total.meters)  );
                }
              }
            }
            #if 0
            printf(" XLate available entries:\r\n");
            printf("  Ingress/Single tagged: %u\r\n",po->resources.vlanXlate_available_entries.ing_stag);
            printf("  Ingress/Double tagged: %u\r\n",po->resources.vlanXlate_available_entries.ing_dtag);
            printf("  Egress /Single tagged: %u\r\n",po->resources.vlanXlate_available_entries.egr_stag);

            printf(" Vlans available:\r\n");
            printf("  Igmp           : %u\r\n",po->resources.vlans_available.igmp);
            printf("  Dhcp           : %u\r\n",po->resources.vlans_available.dhcp);
            printf("  Broadcast Limit: %u\r\n",po->resources.vlans_available.bcastLim);

            printf(" Flow Counters available entries: %u\r\n",po->resources.flowCounters_available_entries);
            #endif
            printf("Switch: resources list read successfully\n\r");
          }
          else
            printf(" Switch: Error reading resources list\n\r");
        }
        break;

      case 1005:
        {
          int i;
          msg_slotModeCfg_t *po=(msg_slotModeCfg_t *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          {
            // Validate size
            if (resposta.infoDim!=sizeof(msg_slotModeCfg_t))
            {
              printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(msg_slotModeCfg_t),resposta.infoDim);
              break;
            }

            printf(" Slot map configuration (Slot=%u):\r\n", ENDIAN_SWAP8(po->SlotId));
            for (i=0; i<MSG_SLOTMODECFG_NSLOTS; i++)
            {
              if (po->slot_list[i].slot_config)
              {
                printf(" Index=%u:   Slot=%2u - Mode=%u\r\n", i, ENDIAN_SWAP8(po->slot_list[i].slot_index), ENDIAN_SWAP8(po->slot_list[i].slot_mode));
              }
              else
              {
                printf(" Index=%u:   Not defined\r\n", i);
              }
            }
            printf("Switch: Slot map configuration read successfully\n\r");
          }
          else
          {
            printf(" Switch: Error reading slot map configuration\n\r");
          }
        }
        break;

    case 1006:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" PRBS enable executed successfully\n\r");
      else
        printf(" PRBS enable not executed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;

    case 1007:
      {
        msg_ptin_pcs_prbs *po=(msg_ptin_pcs_prbs *) &resposta.info[0];
        int i,n;

        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
          // Validate size
          if ((resposta.infoDim%sizeof(msg_ptin_pcs_prbs))!=0) {
            printf(" Switch: Invalid structure size (N*expected=%u, received=%u bytes)\n\r",sizeof(msg_ptin_pcs_prbs),resposta.infoDim);
            break;
          }
          n = resposta.infoDim / sizeof(msg_ptin_pcs_prbs);

          printf(" PRBS status of SlotId=%u\r\n", ENDIAN_SWAP8(po->SlotId));
          for (i=0; i<n; i++)
          {
            printf(" Port %u/%-2u:  Lock=%u Errors=%lu\r\n",
                   ENDIAN_SWAP8(po[i].intf.intf_type), ENDIAN_SWAP8(po[i].intf.intf_id),
                   ENDIAN_SWAP8(po[i].rxStatus.lock), ENDIAN_SWAP32(po[i].rxStatus.rxErrors));
          }
          printf("Switch: PRBS status read successfully\n\r");
        }
        else
          printf(" Switch: Error reading PRBS status\n\r");
      }
      break;

      case 1008:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Slot map validated successfully\n\r");
        else
          printf(" Slot map not valid - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1009:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Slot map configuration applied successfully\n\r");
        else
          printf(" Slot map configuration not applied - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1010:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch port configuration executed successfully\n\r");
        else
          printf(" Switch port configuration not executed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1011:
        {
          uint8 index, n_index;
          msg_HWEthPhyConf_t *po=(msg_HWEthPhyConf_t *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            printf("Switch: port(s) read successfully\n\r");
            n_index = resposta.infoDim / sizeof(msg_HWEthPhyConf_t);
            if ( n_index>PTIN_SYSTEM_N_PORTS )  n_index=PTIN_SYSTEM_N_PORTS;
            for ( index=0; index<n_index; index++ ) {
              printf("Slot %u, Port %u configuration:\n\r", ENDIAN_SWAP8(po[index].SlotId), ENDIAN_SWAP8(po[index].Port));
              printf("\tSpeed=%u\n\r"
                     "\tMedia=%u\n\r"
                     "\tDuplex=%u\n\r"
                     "\tLoopback=%u\n\r"
                     "\tFlowControl=%u\n\r"
                     "\tPortEnable=%u\n\r"
                     "\tMaxFrame=%u\n\r"
                     "\tVlanAwareness=%u\n\r"
                     "\tMacLearning=%u\n\r"
                     "\tAutoMDI=%u\n\r",
                     ENDIAN_SWAP8 (po[index].Speed),
                     ENDIAN_SWAP8 (po[index].Media),
                     ENDIAN_SWAP8 (po[index].Duplex),
                     ENDIAN_SWAP8 (po[index].LoopBack),
                     ENDIAN_SWAP8 (po[index].FlowControl),
                     ENDIAN_SWAP8 (po[index].PortEnable),
                     ENDIAN_SWAP16(po[index].MaxFrame),
                     ENDIAN_SWAP8 (po[index].VlanAwareness),
                     ENDIAN_SWAP8 (po[index].MacLearning),
                     ENDIAN_SWAP8 (po[index].AutoMDI));
            }
            printf("DONE!!!\n\r");
          }
          else
            printf(" Switch: Error reading phy configurations\n\r");
        }
        break;

      case 1012:
        {
          uint8 index, n_index;
          msg_HWEthPhyState_t *po=(msg_HWEthPhyState_t *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            printf("Switch: port(s) read successfully\n\r");
            n_index = resposta.infoDim / sizeof(msg_HWEthPhyState_t);
            if ( n_index>PTIN_SYSTEM_N_PORTS )  n_index=PTIN_SYSTEM_N_PORTS;
            for ( index=0; index<n_index; index++ ) {
              printf("Slot %u, Port %u eth state:\n\r", ENDIAN_SWAP8(po[index].SlotId), ENDIAN_SWAP8(po[index].Port));
              printf("\tSpeed=%u\n\r"
                     "\tDuplex=%u\n\r"
                     "\tCollisions=%u\n\r"
                     "\tRxActivity=%u\n\r"
                     "\tTxActivity=%u\n\r"
                     "\tLinkUp=%u\n\r"
                     "\tAutoNegComplete=%u\n\r"
                     "\tTxFault=%u\n\r"
                     "\tRemoteFault=%u\n\r"
                     "\tLOS=%u\n\r"
                     "\tMedia=%u\n\r"
                     "\tMTU_mismatch=%u\n\r"
                     "\tSupported_MaxFrame=%u\n\r",
                     ENDIAN_SWAP8(po[index].Speed),
                     ENDIAN_SWAP8(po[index].Duplex),
                     ENDIAN_SWAP8(po[index].Collisions),
                     ENDIAN_SWAP8(po[index].RxActivity),
                     ENDIAN_SWAP8(po[index].TxActivity),
                     ENDIAN_SWAP8(po[index].LinkUp),
                     ENDIAN_SWAP8(po[index].AutoNegComplete),
                     ENDIAN_SWAP8(po[index].TxFault),
                     ENDIAN_SWAP8(po[index].RemoteFault),
                     ENDIAN_SWAP8(po[index].LOS),
                     ENDIAN_SWAP8(po[index].Media),
                     ENDIAN_SWAP8(po[index].MTU_mismatch),
                     ENDIAN_SWAP16(po[index].Supported_MaxFrame));
            }
            printf("DONE!!!\n\r");
          }
          else
            printf(" Switch: Error reading phy states\n\r");
        }
        break;

    case 1013:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" Linkscan applied successfully\n\r");
      else
        printf(" Linkscan not executed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;

    case 1014:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" Protection command applied successfully\n\r");
      else
        printf(" Protection command not executed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;

    case 1015:
      {
        L7_uint32 mask;
        uint8 index, n_index;
        msg_HWPortExt_t *po=(msg_HWPortExt_t *) &resposta.info[0];

        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
          printf("Switch: port(s) read successfully\n\r");
          n_index = resposta.infoDim / sizeof(msg_HWPortExt_t);
          if ( n_index>PTIN_SYSTEM_N_PORTS )  n_index=PTIN_SYSTEM_N_PORTS;
          for ( index=0; index<n_index; index++ ) {
            mask = ENDIAN_SWAP32(po[index].Mask);
            printf("Port Type of Slot %u, Port %u/%u:\n\r", ENDIAN_SWAP8(po[index].SlotId), ENDIAN_SWAP8(po[index].intf.intf_type) ,ENDIAN_SWAP8(po[index].intf.intf_id));
            if (mask & MSG_HWPORTEXT_MASK_DEFVID)
              printf("\tDefault Vid   = %u\n\r", ENDIAN_SWAP16(po[index].defVid));
            if (mask & MSG_HWPORTEXT_MASK_DEFPRIO)
              printf("\tDefault Prio  = %u\n\r", ENDIAN_SWAP8(po[index].defPrio));
            if (mask & MSG_HWPORTEXT_MASK_ACCEPTABLE_FRAME_TYPES)
              printf("\tAcceptable Frame Types = %u\n\r", ENDIAN_SWAP8(po[index].acceptable_frame_types));
            if (mask & MSG_HWPORTEXT_MASK_INGRESS_FILTER)
              printf("\tIngress Filter         = %u\n\r", ENDIAN_SWAP8(po[index].ingress_filter));
            if (mask & MSG_HWPORTEXT_MASK_RESTRICTED_VLAN_REG)
              printf("\tRestricted Vlan Reg    = %u\n\r", ENDIAN_SWAP8(po[index].restricted_vlan_reg));
            if (mask & MSG_HWPORTEXT_MASK_VLAN_AWARE)
              printf("\tVlan Aware             = %u\n\r", ENDIAN_SWAP8(po[index].vlan_aware));
            if (mask & MSG_HWPORTEXT_MASK_TYPE)
              printf("\tType                   = %u\n\r", ENDIAN_SWAP8(po[index].type));
            if (mask & MSG_HWPORTEXT_MASK_DOUBLETAG)
              printf("\tDouble Tag             = %u\n\r", ENDIAN_SWAP8(po[index].doubletag));
            if (mask & MSG_HWPORTEXT_MASK_OUTER_TPID)
              printf("\tOuter TPID             = %u\n\r", ENDIAN_SWAP16(po[index].outer_tpid));
            if (mask & MSG_HWPORTEXT_MASK_INNER_TPID)
              printf("\tInner TPID             = %u\n\r", ENDIAN_SWAP16(po[index].inner_tpid));
            if (mask & MSG_HWPORTEXT_MASK_EGRESS_TYPE)
              printf("\tEgress Type            = %u\n\r", ENDIAN_SWAP8(po[index].egress_type));
            if (mask & MSG_HWPORTEXT_MASK_MACLEARN_ENABLE)
              printf("\tMAC Learn Enable                = %u\n\r", ENDIAN_SWAP8(po[index].macLearn_enable));
            if (mask & MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE)
              printf("\tMAC Learn Station Move Enable   = %u\n\r", ENDIAN_SWAP8(po[index].macLearn_stationMove_enable));
            if (mask & MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO)
              printf("\tMAC Learn Station Move Priority = %u\n\r", ENDIAN_SWAP8(po[index].macLearn_stationMove_prio));
            if (mask & MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO)
              printf("\tMAC Learn Station Move within same priority intfs = %u\n\r", ENDIAN_SWAP8(po[index].macLearn_stationMove_samePrio));
            if (mask & MSG_HWPORTEXT_MASK_PROTTRUSTED_INTF)
              printf("\tTrusted interface      = %u\n\r", ENDIAN_SWAP8(po[index].protocol_trusted));
          }
          printf("DONE!!!\n\r");
        }
        else
          printf(" Switch: Error reading phy states\n\r");
      }
      break;

      case 1016:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch port configuration executed successfully\n\r");
        else
          printf(" Switch port configuration not executed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1017:
        {
          uint8 index, n_index;
          msg_HWPortMac_t *po=(msg_HWPortMac_t *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            printf("Switch: port(s) read successfully\n\r");
            n_index = resposta.infoDim / sizeof(msg_HWPortMac_t);
            if ( n_index>PTIN_SYSTEM_N_PORTS )  n_index=PTIN_SYSTEM_N_PORTS;
            for ( index=0; index<n_index; index++ ) {
              printf("MAC of Slot %u, Port %u/%u:\n\r", ENDIAN_SWAP8(po[index].SlotId), ENDIAN_SWAP8(po[index].intf.intf_type), ENDIAN_SWAP8(po[index].intf.intf_id));
              if (ENDIAN_SWAP16(po[index].Mask) & 0x0001)
              {
                printf("\tMAC = %02x:%02x:%02x:%02x:%02x:%02x\n\r",
                       po[index].macAddr[0],po[index].macAddr[1],po[index].macAddr[2],po[index].macAddr[3],po[index].macAddr[4],po[index].macAddr[5]);
              }
            }
            printf("DONE!!!\n\r");
          }
          else
            printf(" Switch: Error reading phy states\n\r");
        }
        break;

      case 1018:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch MAC attributed successfully\n\r");
        else
          printf(" Switch MAC not attributed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1020:
        {
          msg_HWEthRFC2819_PortStatistics_t *po=(msg_HWEthRFC2819_PortStatistics_t *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            printf( "Switch statistics of slot %u, port %u:\n\r"
                    "RX.DropEvents          = %20llu\t\t\tTX.DropEvents          = %20llu\n\r"
                    "RX.Octets              = %20llu\t\t\tTX.Octets              = %20llu\n\r"
                    "RX.Pkts                = %20llu\t\t\tTX.Pkts                = %20llu\n\r"
                    "RX.BroadcastPkts       = %20llu\t\t\tTX.BroadcastPkts       = %20llu\n\r"
                    "RX.MulticastPkts       = %20llu\t\t\tTX.MulticastPkts       = %20llu\n\r"
                    "RX.CRCAlignErrors      = %20llu\t\t\tTX.CRCAlignErrors      = %20llu\n\r"
                    "RX.UndersizePkts       = %20llu\t\t\tTX.Collisions          = %20llu\n\r"
                    "RX.OversizePkts        = %20llu\t\t\tTX.OversizePkts        = %20llu\n\r"
                    "RX.Fragments           = %20llu\t\t\tTX.Fragments           = %20llu\n\r"
                    "RX.Jabbers             = %20llu\t\t\tTX.Jabbers             = %20llu\n\r"
                    "RX.Pkts64octets        = %20llu\t\t\tTX.Pkts64octets        = %20llu\n\r"
                    "RX.Pkts65to127octets   = %20llu\t\t\tTX.Pkts65to127octets   = %20llu\n\r"
                    "RX.Pkts128to255octets  = %20llu\t\t\tTX.Pkts128to255octets  = %20llu\n\r"
                    "RX.Pkts256to511octets  = %20llu\t\t\tTX.Pkts256to511octets  = %20llu\n\r"
                    "RX.Pkts512to1023octets = %20llu\t\t\tTX.Pkts512to1023octets = %20llu\n\r"
                    "RX.Pkts1024to1518octets= %20llu\t\t\tTX.Pkts1024to1518octets= %20llu\n\r"
                    "RX.Pkts1519toMaxOctets = %20llu\t\t\tTX.Pkts1519toMaxOctets = %20llu\n\r"
                    "RX.Throughput          = %20llu\t\t\tTX.Throughput          = %20llu\n\r"
                    "done!\n\r",
                    ENDIAN_SWAP8(po->SlotId), ENDIAN_SWAP8(po->Port),
                    ENDIAN_SWAP64(po->Rx.etherStatsDropEvents)          , ENDIAN_SWAP64(po->Tx.etherStatsDropEvents)          ,
                    ENDIAN_SWAP64(po->Rx.etherStatsOctets)              , ENDIAN_SWAP64(po->Tx.etherStatsOctets)              ,
                    ENDIAN_SWAP64(po->Rx.etherStatsPkts)                , ENDIAN_SWAP64(po->Tx.etherStatsPkts)                ,
                    ENDIAN_SWAP64(po->Rx.etherStatsBroadcastPkts)       , ENDIAN_SWAP64(po->Tx.etherStatsBroadcastPkts)       ,
                    ENDIAN_SWAP64(po->Rx.etherStatsMulticastPkts)       , ENDIAN_SWAP64(po->Tx.etherStatsMulticastPkts)       ,
                    ENDIAN_SWAP64(po->Rx.etherStatsCRCAlignErrors)      , ENDIAN_SWAP64(po->Tx.etherStatsCRCAlignErrors)      ,
                    ENDIAN_SWAP64(po->Rx.etherStatsUndersizePkts)       , ENDIAN_SWAP64(po->Tx.etherStatsCollisions)          ,
                    ENDIAN_SWAP64(po->Rx.etherStatsOversizePkts)        , ENDIAN_SWAP64(po->Tx.etherStatsOversizePkts)        ,
                    ENDIAN_SWAP64(po->Rx.etherStatsFragments)           , ENDIAN_SWAP64(po->Tx.etherStatsFragments)           ,
                    ENDIAN_SWAP64(po->Rx.etherStatsJabbers)             , ENDIAN_SWAP64(po->Tx.etherStatsJabbers)             ,
                    ENDIAN_SWAP64(po->Rx.etherStatsPkts64Octets)        , ENDIAN_SWAP64(po->Tx.etherStatsPkts64Octets)        ,
                    ENDIAN_SWAP64(po->Rx.etherStatsPkts65to127Octets)   , ENDIAN_SWAP64(po->Tx.etherStatsPkts65to127Octets)   ,
                    ENDIAN_SWAP64(po->Rx.etherStatsPkts128to255Octets)  , ENDIAN_SWAP64(po->Tx.etherStatsPkts128to255Octets)  ,
                    ENDIAN_SWAP64(po->Rx.etherStatsPkts256to511Octets)  , ENDIAN_SWAP64(po->Tx.etherStatsPkts256to511Octets)  ,
                    ENDIAN_SWAP64(po->Rx.etherStatsPkts512to1023Octets) , ENDIAN_SWAP64(po->Tx.etherStatsPkts512to1023Octets) ,
                    ENDIAN_SWAP64(po->Rx.etherStatsPkts1024to1518Octets), ENDIAN_SWAP64(po->Tx.etherStatsPkts1024to1518Octets),
                    ENDIAN_SWAP64(po->Rx.etherStatsPkts1519toMaxOctets) , ENDIAN_SWAP64(po->Tx.etherStatsPkts1519toMaxOctets) ,
                    ENDIAN_SWAP64(po->Rx.Throughput)                    , ENDIAN_SWAP64(po->Tx.Throughput)                    );
          }
          else
            printf(" Switch: Error reading port statistics\n\r");
        }
        break;

      case 1021:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Port statistics cleared\n\r");
        else
          printf(" Switch: Error clearing port statistics - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1022:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch port configuration executed successfully\n\r");
        else
          printf(" Switch port configuration not executed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1023:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: RFCC2819 buffer cleared successfully\n\r");
        else
          printf(" Switch: Error clearing RFCC2819 buffer - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1024:
        {
          uint8 index, n_index;

          msg_rfc2819_buffer_t *ptr=(msg_rfc2819_buffer_t *)resposta.info;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            printf("Switch: RFC2819 buffer read successfully\n\r");
            n_index = resposta.infoDim / sizeof(msg_rfc2819_buffer_t);
            if ( n_index>RFC2819_MAX_BUFFER_GET_NEXT )  n_index=RFC2819_MAX_BUFFER_GET_NEXT;
            printf("Idx  |    path   | timestamp |period |   Octets   |  Packets   | Broadcast  | Multicast  | CRCAlError | Undersize  | Oversize   | Fragments  |  Jabbers   | Collisions |Utilization | Pkts64     |Pkts65to127 |Pkts128to255|Pkts256to511|Pkts512t1023| Pkts1024t15180\n\r");
            printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\r");
            for ( index=0; index<n_index; index++ ) {
              printf("%4ld |0x%.8lx | %8ld | %5ld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld | %10lld\n\r",
                        ptr[index].index             ,
                        ptr[index].path              ,
                        ptr[index].time              ,
                        ptr[index].cTempo            ,  
                        ptr[index].Octets            ,  
                        ptr[index].Pkts              ,  
                        ptr[index].Broadcast         ,  
                        ptr[index].Multicast         ,  
                        ptr[index].CRCAlignErrors    ,  
                        ptr[index].UndersizePkts     ,  
                        ptr[index].OversizePkts      ,  
                        ptr[index].Fragments         ,  
                        ptr[index].Jabbers           ,  
                        ptr[index].Collisions        ,  
                        ptr[index].Utilization       ,  
                        ptr[index].Pkts64Octets      ,  
                        ptr[index].Pkts65to127Octets ,  
                        ptr[index].Pkts128to255Octets,  
                        ptr[index].Pkts256to511Octets,  
                        ptr[index].Pkts512to1023Octets, 
                        ptr[index].Pkts1024to1518Octets);
            }
            printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\r");
            printf("DONE!!!\n\r");
          }
          else
            printf(" Switch: Error reading RFC2819 buffers\n\r");
        }
        break;

      case 1025:
        {
          L7_uint32 *ptr=(L7_uint32 *)resposta.info;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            if ((*ptr & 0x80000000)==0) {
              printf("Switch: RFC2819 Probe (Port=%ld) disabled\n\r",*ptr & 0xFFFF);
            }
            else {
              printf("Switch: RFC2819 Probe (Port=%ld) enabled\n\r",*ptr & 0xFFFF);
            }
          }
          else
            printf(" Switch: Error reading RFC2819 Probe Configuration\n\r");
        }
        break;


      case 1026:
        {
          msg_rfc2819_buffer_status_t *ptr=(msg_rfc2819_buffer_status_t *)resposta.info;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {

            printf(" Index  wrptr  Flflag MAXReg \n\r");
            printf("+++++++++++++++++++++++++++++\n\r");
            printf("%4d     %4d      %d   %4d\n\r",ptr->BufferType, ptr->wrptr, ptr->bufferfull, ptr->max_entrys);
            printf("+++++++++++++++++++++++++++++\n\r");
          }
          else
            printf(" Switch: Error reading RFC2819 buffers status\n\r");
        }
        break;

      case 1027:

       /*msg_l2_maclimit_config_t *ptr;
        ptr = &(((msg_l2_maclimit_config_t *) resposta.info)[0]);
         */
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {
          printf(" MAC Learning limiting applied \n\r");/*
          printf(" SlotId    : %10lu\r\n",ptr->slotid    );
          printf(" Mask     : %10lu\r\n",ptr->mask  );
          printf(" System   : %10lu\r\n",ptr->system);
          printf(" Intf Type: %10lu\r\n",ptr->intf.intf_type);
          printf(" Intf ID  : %10lu\r\n",ptr->intf.intf_id );*/
        }
        else
          printf(" Switch: Error appling MAC learning limiting - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1030:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {
          msg_QoSConfiguration_t *ptr;
          uint8 i, j, n;
          // Validate size
          if (resposta.infoDim==0 || (resposta.infoDim%sizeof(msg_QoSConfiguration_t))!=0) {
            printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(msg_QoSConfiguration_t),resposta.infoDim);
            break;
          }
          // Number of elements
          n = resposta.infoDim/sizeof(msg_QoSConfiguration_t);

          // Run all elements
          for (i=0; i<n; i++) {

            // Pointer to element
            ptr = &(((msg_QoSConfiguration_t *) resposta.info)[i]);

            // Print configuration
            printf(" QoS configuration for port %u/%u (SlotId=%u)\r\n",ptr->intf.intf_type,ptr->intf.intf_id,ptr->SlotId);
            printf("  Trust mode    : ");
            if (ptr->trust_mode==0)      printf("Not configured");
            else if (ptr->trust_mode==1) printf("Untrust marks");
            else if (ptr->trust_mode==2) printf("802.1p marks");
            else if (ptr->trust_mode==3) printf("IP-precedence marks");
            else if (ptr->trust_mode==4) printf("DSCP marks");
            else                         printf("Invalid");
            printf("\r\n");
            printf("  Bandwidth unit: ");
            if (ptr->bandwidth_unit==0)      printf("Percentage (0-100%%)");
            else if (ptr->bandwidth_unit==1) printf("Kilobits per second");
            else if (ptr->bandwidth_unit==2) printf("Packets per second");
            else                         printf("Invalid");
            printf("\r\n");
            printf("  Shaping rate  : %lu %s\r\n",ptr->shaping_rate, ((ptr->bandwidth_unit==0) ? "%%" : "Kbps"));

            // Only proceed, if trust mode is valid
            if (ptr->trust_mode!=0 && ptr->trust_mode<=4) {

              // Priorities map
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("------------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
              printf("  Pkt. Priority ");
              for (j=0; j<8; j++) {
                printf(" | %10u",j);
              }
              printf(" |\r\n");
              printf("                 ");
              for (j=0; j<8; j++) {
                printf("|------------");
              }
              printf("|\r\n");
              printf("  ClassOfService");
              for (j=0; j<8; j++) {
                if (ptr->trust_mode==4) {
                  printf(" | 0x%08lX",ptr->pktprio.cos[j]);
                }
                else {
                  printf(" | %10lu",ptr->pktprio.cos[j]);
                }
              }
              printf(" |\r\n");
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("------------");
                if (j<7)  printf("-");
              }
              printf("\r\n");

              // CoS configurations
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("------------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
              printf("  CoS           ");
              for (j=0; j<8; j++) {
                printf(" | %10u",j);
              }
              printf(" |\r\n");
              printf("                 ");
              for (j=0; j<8; j++) {
                printf("|------------");
              }
              printf("|\r\n");
              printf("  Scheduler Type");
              for (j=0; j<8; j++) {
                printf(" | ");
                if (ptr->cos_config.cos[j].scheduler==0)       printf("   NotConf");
                else if (ptr->cos_config.cos[j].scheduler==1)  printf("    Strict");
                else if (ptr->cos_config.cos[j].scheduler==2)  printf("  Weighted");
                else                                           printf("   Invalid");
              }
              printf(" |\r\n");
              printf("  Min. Bandwidth");
              for (j=0; j<8; j++) {
                printf(" | %10lu",ptr->cos_config.cos[j].min_bandwidth);
              }
              printf(" |\r\n");
              printf("  Max. Bandwidth");
              for (j=0; j<8; j++) {
                printf(" | %10lu",ptr->cos_config.cos[j].max_bandwidth);
              }
              printf(" |\r\n");
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("------------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
            } // Only proceed, if trust mode is valid
          } // Run all elements

          printf(" Switch: General QoS configuration read successfuly\n\r");
        }
        else
          printf(" Switch: Error reading general QoS configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1031:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: General QoS configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting general QoS configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1032:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Specific QoS configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting specific QoS configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1033:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {
          msg_QoSConfiguration3_t *ptr;
          uint8 i, j, n, dp;
          // Validate size
          if (resposta.infoDim==0 || (resposta.infoDim%sizeof(msg_QoSConfiguration3_t))!=0) {
            printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(msg_QoSConfiguration3_t),resposta.infoDim);
            break;
          }
          // Number of elements
          n = resposta.infoDim/sizeof(msg_QoSConfiguration3_t);

          // Run all elements
          for (i=0; i<n; i++)
          {
            // Pointer to element
            ptr = &(((msg_QoSConfiguration3_t *) resposta.info)[i]);

            // Print configuration
            printf(" QoS configuration for port %u/%u (SlotId=%u)\r\n",ptr->intf.intf_type,ptr->intf.intf_id,ptr->SlotId);
            printf("  Bandwidth unit: ");
            if (ptr->main_mask & MSG_QOS3_BANDWIDTH_UNITS_MASK)
            {
              if (ptr->bandwidth_unit == 0)     printf("Percentage (0-100%%)"); 
              else if (ptr->bandwidth_unit==1)  printf("Kilobits per second");
              else if (ptr->bandwidth_unit==2)  printf("Packets per second");
              else                              printf("Invalid");
            }
            else printf("XXX");
            printf("\r\n");

            printf("  Trust mode    : ");
            if (ptr->main_mask & MSG_QOS3_INGRESS_MASK && ptr->ingress.ingress_mask & MSG_QOS3_INGRESS_TRUST_MODE_MASK)
            {
              if (ptr->ingress.trust_mode==0)       printf("Not configured");
              else if (ptr->ingress.trust_mode==1)  printf("Untrust marks");
              else if (ptr->ingress.trust_mode==2)  printf("802.1p marks");
              else if (ptr->ingress.trust_mode==3)  printf("IP-precedence marks");
              else if (ptr->ingress.trust_mode==4)  printf("DSCP marks");
              else                                  printf("Invalid");
            }
            else printf("XXX");
            printf("\r\n");

            printf("  Shaping rate  : ");
            if (ptr->main_mask & MSG_QOS3_EGRESS_MASK && ptr->ingress.ingress_mask & MSG_QOS3_EGRESS_COS_SHAPER_MASK)
              printf("%lu %s\r\n",ptr->egress.shaping_rate, ((ptr->bandwidth_unit==0) ? "%" : "Kbps"));
            else
              printf("XXX\r\n");

            // Only proceed, if trust mode is valid
            if ((ptr->main_mask & MSG_QOS3_INGRESS_MASK && ptr->ingress.ingress_mask & MSG_QOS3_INGRESS_TRUST_MODE_MASK) &&
                (ptr->ingress.trust_mode!=0 && ptr->ingress.trust_mode<=4))
            {
              // Priorities map
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("------------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
              printf("  Pkt. Priority ");
              for (j=0; j<8; j++) {
                printf(" | %10u",(ptr->ingress.trust_mode == 4) ? (j*8) : j);
              }
              printf(" |\r\n");
              printf("                 ");
              for (j=0; j<8; j++) {
                printf("|------------");
              }
              printf("|\r\n");
              printf("  ClassOfService");
              for (j=0; j<8; j++) {
                if (ptr->ingress.trust_mode==4) {
                  printf(" | 0x%08lX",  ((uint32) ptr->ingress.cos_classif.dscp_map.cos[(j*8)+0] & 0x0f) |
                                       (((uint32) ptr->ingress.cos_classif.dscp_map.cos[(j*8)+1] & 0x0f) << 4 ) |
                                       (((uint32) ptr->ingress.cos_classif.dscp_map.cos[(j*8)+2] & 0x0f) << 8 ) |
                                       (((uint32) ptr->ingress.cos_classif.dscp_map.cos[(j*8)+3] & 0x0f) << 12) |
                                       (((uint32) ptr->ingress.cos_classif.dscp_map.cos[(j*8)+4] & 0x0f) << 16) |
                                       (((uint32) ptr->ingress.cos_classif.dscp_map.cos[(j*8)+5] & 0x0f) << 20) |
                                       (((uint32) ptr->ingress.cos_classif.dscp_map.cos[(j*8)+6] & 0x0f) << 24) |
                                       (((uint32) ptr->ingress.cos_classif.dscp_map.cos[(j*8)+7] & 0x0f) << 28));
                }
                else {
                  printf(" | %10u",ptr->ingress.cos_classif.pcp_map.cos[j]);
                }
              }
              printf(" |\r\n");
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("------------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
            }
            else
            {
              printf("*Ingress CoS mapping not provided!*\r\n");
            }

            /* CoS egress configurations */
            if (ptr->main_mask & MSG_QOS3_EGRESS_MASK)
            {
              // CoS configurations
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("------------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
              printf("  CoS           ");
              for (j=0; j<8; j++) {
                printf(" | %10u",j);
              }
              printf(" |\r\n");
              printf("                 ");
              for (j=0; j<8; j++) {
                printf("|------------");
              }
              printf("|\r\n");

              if (ptr->egress.egress_mask & MSG_QOS3_EGRESS_COS_SCHEDULER_MASK)
              {
                printf("  Scheduler Type");
                for (j=0; j<8; j++) {
                  printf(" | ");
                  if (ptr->egress.cos_scheduler[j].local_mask & MSG_QOS3_EGRESS_COS_SCHEDULER_TYPE_MASK)
                  {
                    if (ptr->egress.cos_scheduler[j].schedulerType==0)       printf("   NotConf"); 
                    else if (ptr->egress.cos_scheduler[j].schedulerType==1)  printf("    Strict");
                    else if (ptr->egress.cos_scheduler[j].schedulerType==2)  printf("  Weighted");
                    else                                                     printf("   Invalid");
                  }
                  else printf("       XXX");
                }
                printf(" |\r\n");
                printf("  WRR Weights   ");
                for (j=0; j<8; j++) {
                  if (ptr->egress.cos_scheduler[j].local_mask & MSG_QOS3_EGRESS_COS_SCHEDULER_WRR_WEIGHT_MASK)
                    printf(" | %10u",ptr->egress.cos_scheduler[j].wrrSched_weight);
                  else
                    printf(" | %10s", "XXX");
                }
                printf(" |\r\n");
              }
              else
              {
                printf("**Egress CoS Scheduler properties not provided!**\r\n");
              }

              if (ptr->egress.egress_mask & MSG_QOS3_EGRESS_COS_SHAPER_MASK)
              {
                printf("  Min. Bandwidth");
                for (j=0; j<8; j++) {
                  if (ptr->egress.cos_shaper[j].local_mask & MSG_QOS3_EGRESS_COS_SHAPER_MIN_BW_MASK)
                    printf(" | %10lu",ptr->egress.cos_shaper[j].min_bandwidth);
                  else
                    printf(" | %10s", "XXX");
                }
                printf(" |\r\n");
                printf("  Max. Bandwidth");
                for (j=0; j<8; j++) {
                  if (ptr->egress.cos_shaper[j].local_mask & MSG_QOS3_EGRESS_COS_SHAPER_MAX_BW_MASK)
                    printf(" | %10lu",ptr->egress.cos_shaper[j].max_bandwidth);
                  else
                    printf(" | %10s","XXX");
                }
                printf(" |\r\n");
              }
              else
              {
                printf("**Egress CoS Shaper properties not provided!**\r\n");
              }

              if (ptr->egress.egress_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_MASK)
              {
                printf("  MGMT Type     ");
                for (j=0; j<8; j++)
                {
                  printf(" | ");
                  if (ptr->egress.cos_dropmgmt[j].local_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_TYPE_MASK)
                  {
                    if (ptr->egress.cos_dropmgmt[j].dropMgmtType==0)       printf("  TailDrop");
                    else if (ptr->egress.cos_dropmgmt[j].dropMgmtType==1)  printf("      WRED");
                    else                                                   printf("   Invalid");
                  }
                  else printf("       XXX");
                }
                printf(" |\r\n");
                printf("                 |");
                for (j=0; j<8; j++) {
                  printf("------------");
                  if (j<7)  printf("-");
                }
                printf("|\r\n");

                /* Run all DP levels */
                for (dp = 0; dp < 6; dp++)
                {
                  printf("  TDRP Thresh[%u]", dp+1);
                  for (j=0; j<8; j++) {
                    if (ptr->egress.cos_dropmgmt[j].local_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLDS_MASK &&
                        ptr->egress.cos_dropmgmt[j].dp_thresholds[dp].local2_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_TAILDROP_MAX_MASK)
                      printf(" | %10u", ptr->egress.cos_dropmgmt[j].dp_thresholds[dp].tailDrop_threshold); 
                    else
                      printf(" | %10s","XXX");
                  }
                  printf(" |\r\n");
                  printf("  WRED minThr[%u]", dp+1);
                  for (j=0; j<8; j++) {
                    if (ptr->egress.cos_dropmgmt[j].local_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLDS_MASK &&
                        ptr->egress.cos_dropmgmt[j].dp_thresholds[dp].local2_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_MIN_MASK)
                      printf(" | %10u",ptr->egress.cos_dropmgmt[j].dp_thresholds[dp].wred_minThreshold);
                    else
                      printf(" | %10s","XXX");
                  }
                  printf(" |\r\n");
                  printf("  WRED maxThr[%u]", dp+1);
                  for (j=0; j<8; j++) {
                    if (ptr->egress.cos_dropmgmt[j].local_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLDS_MASK &&
                        ptr->egress.cos_dropmgmt[j].dp_thresholds[dp].local2_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_MAX_MASK)
                      printf(" | %10u",ptr->egress.cos_dropmgmt[j].dp_thresholds[dp].wred_maxThreshold);
                    else
                      printf(" | %10s","XXX");
                  }
                  printf(" |\r\n");
                  printf("  WRED dropPr[%u]", dp+1);
                  for (j=0; j<8; j++) {
                    if (ptr->egress.cos_dropmgmt[j].local_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLDS_MASK &&
                        ptr->egress.cos_dropmgmt[j].dp_thresholds[dp].local2_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_DROPPROB_MASK)
                      printf(" | %10u",ptr->egress.cos_dropmgmt[j].dp_thresholds[dp].wred_dropProb);
                    else
                      printf(" | %10s","XXX");
                  }
                  printf(" |\r\n");

                  printf("                 |");
                  for (j=0; j<8; j++) {
                    printf("------------");
                    if (j<7)  printf("-");
                  }
                  printf("|\r\n");
                }

                printf("  WRED decayExp ");
                for (j=0; j<8; j++) {
                  if (ptr->egress.cos_dropmgmt[j].local_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_WRED_DECAYEXP_MASK)
                    printf(" | %10u",ptr->egress.cos_dropmgmt[j].wred_decayExp);
                  else
                    printf(" | %10s","XXX");
                }
                printf(" |\r\n");
                printf("                  ");
                for (j=0; j<8; j++) {
                  printf("------------");
                  if (j<7)  printf("-");
                }
                printf("\r\n");
              }
              else
              {
                printf("**Egress CoS - Drop Management properties not provided!**\r\n");
              }
            }
            else
            {
              printf("*Egress CoS configurations not provided!*\r\n");
            }
          } // Run all elements

          printf(" Switch: General QoS2 configuration read successfuly\n\r");
        }
        else
          printf(" Switch: Error reading general QoS2 configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1034:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: QoS3-interface configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting QoS3-interface configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1035:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: QoS3-cos configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting QoS3-cos configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1036:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: QoS3-mgmt configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting QoS3-mgmt configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1037:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: QoS3-dpThresholds configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting QoS3-dpThresholds configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1038:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: QoS3-policer configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting QoS3-policer configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1040:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {
          msg_switch_mac_table_t *ptr;
          uint16 i;
          // Validate minimum size
          if (resposta.infoDim<sizeof(msg_switch_mac_intro_t)) {
            printf(" Switch: Invalid structure size (min.expected=%u, received=%u bytes)\n\r",sizeof(msg_switch_mac_intro_t),resposta.infoDim);
            break;
          }

          // Pointer to element
          ptr = (msg_switch_mac_table_t *) resposta.info;

          // Validate total size
          if (resposta.infoDim<sizeof(msg_switch_mac_intro_t)+sizeof(msg_switch_mac_entry)*ptr->intro.numEntries)
          {
            printf(" Switch: Invalid structure size (expected=%lu, received=%u bytes)\n\r",sizeof(msg_switch_mac_intro_t)+sizeof(msg_switch_mac_entry)*ptr->intro.numEntries,resposta.infoDim);
            break;
          }
          
          printf(" Reading %lu MAC entries from startId %lu (slot=%u):\r\n",ptr->intro.numEntries,ptr->intro.startEntryId,ptr->intro.slotId);

          for (i=0; i<ptr->intro.numEntries; i++) {
            printf(" Id %-5lu, ",ptr->intro.startEntryId+i);
            if (ptr->entry[i].evcId!=(uint32)-1)
              printf("EVC %-4lu, ",ptr->entry[i].evcId);
            else
              printf("No EVC  , ");
            printf("VlanId %-4u, ",ptr->entry[i].vlanId);
            printf("GEMid %-4u, ",ptr->entry[i].gem_id);
            printf("Port %-2u/%-2u, ",ptr->entry[i].intf.intf_type,ptr->entry[i].intf.intf_id);
            printf("MAC %02X:%02X:%02X:%02X:%02X:%02X, ",
                   ptr->entry[i].addr[0],
                   ptr->entry[i].addr[1],
                   ptr->entry[i].addr[2],
                   ptr->entry[i].addr[3],
                   ptr->entry[i].addr[4],
                   ptr->entry[i].addr[5]);
            printf("%s type\r\n",((ptr->entry[i].static_entry) ? "Static" : "Dynamic"));
          }
          printf(" Switch: MAC table read successfuly\n\r");
        }
        else
          printf(" Switch: Error reading MAC table - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1041:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MAC address added successfully\n\r");
        else
          printf(" Switch: MAC address not added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1042:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MAC address removed successfully\n\r");
        else
          printf(" Switch: MAC address not removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1043:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MAC table flushed successfully\n\r");
        else
          printf(" Switch: Error flushing MAC table - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1044:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: QoS configuration applied successfully\n\r");
        else
          printf(" Switch: Failed applying QoS configuration - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1220:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {
          msg_HwEthernetDhcpOpt82Profile_t *ptr;

          if (resposta.infoDim!=sizeof(msg_HwEthernetDhcpOpt82Profile_t)) {
            printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_HwEthernetDhcpOpt82Profile_t));
            break;
          }

          ptr = (msg_HwEthernetDhcpOpt82Profile_t *) &resposta.info[0];

          printf(" DHCP profile (Slot=%u):\r\n",ptr->SlotId);
          printf(" EVCid=%lu\r\n",ptr->evc_id);
          printf(" Mask =0x%02x\r\n",ptr->mask);
          printf(" Intf =%u/%u\r\n",ptr->intf.intf_type,ptr->intf.intf_id);
          printf(" Client.Mask  = 0x%02x\r\n",ptr->client.mask);
          printf(" Client.Intf  = %u/%u\r\n",ptr->client.intf.intf_type,ptr->client.intf.intf_id);
          printf(" Client.OVlan = %u\r\n",ptr->client.outer_vlan);
          printf(" Client.IVlan = %u\r\n",ptr->client.inner_vlan);
          printf(" options = 0x%02x\n",ptr->options);
          printf(" CircuitId=\"%s\"\r\n",ptr->circuitId);
          printf(" RemoteId =\"%s\"\r\n",ptr->remoteId);
          printf(" Switch: DHCPop82 profile read successfully\n\r");
        }
        else
          printf(" Switch: DHCPop82 profile not read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1221:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DHCPop82 profile added successfully\n\r");
        else
          printf(" Switch: DHCPop82 profile not added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1222:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DHCPop82 profile removed successfully\n\r");
        else
          printf(" Switch: DHCPop82 profile not removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1230:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {
          msg_dai_statistics_t *ptr;

          if (resposta.infoDim!=sizeof(msg_dai_statistics_t)) {
            printf(" Switch: Invalid structure size (%u vs %u)\n\r", resposta.infoDim, sizeof(msg_dai_statistics_t));
            break;
          }

          ptr = (msg_dai_statistics_t *) &resposta.info[0];

          //printf("DAI statistics for EVC#%lu (type=%u), intf=%u/%u:\r\n", ptr->service.id_val.evc_id, ptr->service.id_type, ptr->intf.intf_type, ptr->intf.intf_id);
          printf("DAI statistics for EVC#%lu / VLAN=%u, intf=%u/%u:\r\n", ptr->evc_idx, ptr->vlan_id, ptr->intf.intf_type, ptr->intf.intf_id);
          printf(" forwarded       = %lu\r\n", ptr->stats.forwarded       );
          printf(" dropped         = %lu\r\n", ptr->stats.dropped         );
          printf(" dhcpDrops       = %lu\r\n", ptr->stats.dhcpDrops       );
          printf(" dhcpPermits     = %lu\r\n", ptr->stats.dhcpPermits     );
          printf(" aclDrops        = %lu\r\n", ptr->stats.aclDrops        );
          printf(" aclPermits      = %lu\r\n", ptr->stats.aclPermits      );
          printf(" sMacFailures    = %lu\r\n", ptr->stats.sMacFailures    );
          printf(" dMacFailures    = %lu\r\n", ptr->stats.dMacFailures    );
          printf(" ipValidFailures = %lu\r\n", ptr->stats.ipValidFailures );
          printf("Switch: DAI statistics read successfully\n\r");
        }
        else
          printf(" Switch: Error reading DAI statistics - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1231:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DAI global settings applied successfully\n\r");
        else
          printf(" Switch: DAI global settings not applied - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1232:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DAI interface settings applied successfully\n\r");
        else
          printf(" Switch: DAI interface settings not applied - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1233:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DAI EVC settings applied successfully\n\r");
        else
          printf(" Switch: DAI EVC settings not applied - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1234:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DAI VLAN settings applied successfully\n\r");
        else
          printf(" Switch: DAI VLAN settings not applied - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1235:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: ARP-ACL entry added successfully\n\r");
        else
          printf(" Switch: ARP-ACL entry not added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;
      case 1236:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: ARP-ACL entry removed successfully\n\r");
        else
          printf(" Switch: ARP-ACL entry not removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;
      case 1237:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: ARP-ACL group linked to an EVC\n\r");
        else
          printf(" Switch: ARP-ACL group not linked to an EVC - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;
      case 1238:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: ARP-ACL group linked to a VLAN\n\r");
        else
          printf(" Switch: ARP-ACL group not linked to a VLAN - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1240:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {
          msg_DHCPv4v6_bind_table_t *ptr;
          uint16 i;
          // Validate size
          if (resposta.infoDim!=sizeof(msg_DHCPv4v6_bind_table_t)) {
            printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(msg_DHCPv4v6_bind_table_t),resposta.infoDim);
            break;
          }

          // Pointer to element
          ptr = &(((msg_DHCPv4v6_bind_table_t *) resposta.info)[0]);

          printf(" Reading page %u of DHCP Binding table (slot=%u):\r\n",ptr->page,ptr->SlotId);

          for (i=0; i<ptr->bind_table_msg_size; i++) {
            printf(" Id %-5u, ",ptr->bind_table[i].entry_index);
            if (ptr->bind_table[i].evc_idx!=(uint32)-1)
              printf("EVC %-4lu, ",ptr->bind_table[i].evc_idx);
            else
              printf("No EVC  , ");
            printf("VlanId %-4u, ",ptr->bind_table[i].outer_vlan);
            printf("CVlanId %-4u, ",ptr->bind_table[i].inner_vlan);
            printf("Port %u/%-2u, ",ptr->bind_table[i].intf.intf_type,ptr->bind_table[i].intf.intf_id);
            printf("MAC %02X:%02X:%02X:%02X:%02X:%02X, ",
                   ptr->bind_table[i].macAddr[0],
                   ptr->bind_table[i].macAddr[1],
                   ptr->bind_table[i].macAddr[2],
                   ptr->bind_table[i].macAddr[3],
                   ptr->bind_table[i].macAddr[4],
                   ptr->bind_table[i].macAddr[5]);
            if (ptr->bind_table[i].ipAddr.family == 0)
            {
               printf("IPAddr %03lu.%03lu.%03lu.%03lu, ", (ptr->bind_table[i].ipAddr.addr.ipv4 >> 24) & 0xFF, (ptr->bind_table[i].ipAddr.addr.ipv4 >> 16) & 0xFF, (ptr->bind_table[i].ipAddr.addr.ipv4 >> 8) & 0xFF,
                     ptr->bind_table[i].ipAddr.addr.ipv4 & 0xFF);
            }
            else if(ptr->bind_table[i].ipAddr.family == 1)
            {
               printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x, ", (int) ptr->bind_table[i].ipAddr.addr.ipv6[0], (int) ptr->bind_table[i].ipAddr.addr.ipv6[1], (int) ptr->bind_table[i].ipAddr.addr.ipv6[2],
                     (int) ptr->bind_table[i].ipAddr.addr.ipv6[3], (int) ptr->bind_table[i].ipAddr.addr.ipv6[4], (int) ptr->bind_table[i].ipAddr.addr.ipv6[5], (int) ptr->bind_table[i].ipAddr.addr.ipv6[6],
                     (int) ptr->bind_table[i].ipAddr.addr.ipv6[7], (int) ptr->bind_table[i].ipAddr.addr.ipv6[8], (int) ptr->bind_table[i].ipAddr.addr.ipv6[9], (int) ptr->bind_table[i].ipAddr.addr.ipv6[10],
                     (int) ptr->bind_table[i].ipAddr.addr.ipv6[11], (int) ptr->bind_table[i].ipAddr.addr.ipv6[12], (int) ptr->bind_table[i].ipAddr.addr.ipv6[13], (int) ptr->bind_table[i].ipAddr.addr.ipv6[14],
                     (int) ptr->bind_table[i].ipAddr.addr.ipv6[15]);
            }
            printf("Lease time %-6lu, ",ptr->bind_table[i].remLeave);
            printf("%s type\r\n",((ptr->bind_table[i].bindingType==1) ? "Static" : ((ptr->bind_table[i].bindingType==2) ? "Dynamic" : "Tentative")));
          }
          printf(" Switch: DHCP Binding table read successfuly\n\r");
        }
        else
          printf(" Switch: Error reading DHCP Binding table - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1242:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DHCP bind entry removed successfully\n\r");
        else
          printf(" Switch: DHCP bind entry not removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1310:
        {
          msg_IgmpClientStatistics_t *po=(msg_IgmpClientStatistics_t *) &resposta.info[0];
          uint32 tmp;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            if (resposta.infoDim!=sizeof(msg_IgmpClientStatistics_t)) {
              printf(" Switch: Invalid structure size\r\n");
              break;
            }

            printf( " IGMP statistics for Slot=%u, EVC=%lu, mask=0x%02x, intf=%u/%u, client={mask=0x%02x,oVlan=%u,iVlan=%u,intf=%u/%u}:\n\r",po->SlotId,
                    po->mcEvcId, po->mask,
                    po->intf.intf_type,po->intf.intf_id,
                    po->client.mask, po->client.outer_vlan, po->client.inner_vlan, po->client.intf.intf_type,po->client.intf.intf_id);
            printf( "   Active Groups  = %lu\r\n",po->stats.active_groups );
            printf( "   Active Clients = %lu\r\n",po->stats.active_clients);

            printf( "  ___________________________________________________________________________________________________ \r\n");
            printf( " | IGMP packets sent        : "                );
            ((tmp=po->stats.igmp_tx                                 )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " | IGMP packets tx failed   : %20c |\r\n",'-'  );
            printf( " |_________________________________________________|_________________________________________________|\r\n");

            printf( " | IGMP packets intercepted : "                );
            ((tmp=po->stats.igmp_total_rx                           )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " | IGMP packets dropped     : "                );
            ((tmp=po->stats.igmp_dropped_rx                         )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|_________________________________________________|\r\n");

            printf( " | IGMP packets rx valid    : "                );
            ((tmp=po->stats.igmp_valid_rx                           )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " | IGMP packets rx invalid  : "                );
            ((tmp=po->stats.igmp_invalid_rx                         )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|_________________________________________________|\r\n");

            printf( " | IGMP Joins tx            : "                );
            ((tmp=po->stats.HWIgmpv2Statistics.join_tx              )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n"                                        );
            printf( " |_________________________________________________|_________________________________________________ \r\n");

            printf( " | IGMP Joins rx success    : "                );
            ((tmp=po->stats.HWIgmpv2Statistics.join_valid_rx        )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " | IGMP Joins rx failed     : "                );
            ((tmp=po->stats.HWIgmpv2Statistics.join_invalid_rx      )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|_________________________________________________|\r\n");

            printf( " | IGMP Leaves tx           : "                );
            ((tmp=po->stats.HWIgmpv2Statistics.leave_tx             )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|\r\n");

            printf( " | IGMP Leaves rx           : "                );
            ((tmp=po->stats.HWIgmpv2Statistics.leave_valid_rx       )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|\r\n");

            printf( " | IGMP GeneralQueries tx   : "                );
            ((tmp=po->stats.HWQueryStatistics.general_query_tx      )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|\r\n");

            printf( " | IGMP GeneralQueries rx   : "                );
            ((tmp=po->stats.HWQueryStatistics.general_query_valid_rx)==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|\r\n");

            printf( " | IGMP SpecificQueries tx  : " );
            ((tmp=po->stats.HWQueryStatistics.group_query_tx        )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|\r\n");

            printf( " | IGMP SpecificQueries rx  : ");
            ((tmp=po->stats.HWQueryStatistics.group_query_valid_rx  )==0)  ? printf("%20c",'-') : printf( "%20lu",tmp );
            printf( " |\r\n" );
            printf( " |_________________________________________________|\r\n");

            printf( " | IGMP MembershipReportV3  : %20c |\r\n",'-');
            printf( " |_________________________________________________|\r\n");

            printf( "Done!\r\n");
          }
          else  {
            printf(" Switch: Error reading IGMP statistics\n\r");
          }
        }
        break;

      case 1312:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP statistics cleared successfully\n\r");
        else
          printf(" Switch: IGMP statistics not cleared - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1320:
        {
          msg_DhcpClientStatistics_t *po=(msg_DhcpClientStatistics_t *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            if (resposta.infoDim!=sizeof(msg_DhcpClientStatistics_t)) {
              printf(" Switch: Invalid structure size\r\n");
              break;
            }

            printf( " DHCP statistics for Slot=%u, EVC=%lu, mask=0x%02x, intf=%u/%u, client={mask=0x%02x,oVlan=%u,iVlan=%u,intf=%u/%u}:\n\r",po->SlotId,
                    po->evc_id, po->mask,
                    po->intf.intf_type,po->intf.intf_id,
                    po->client.mask, po->client.outer_vlan, po->client.inner_vlan, po->client.intf.intf_type,po->client.intf.intf_id);
            printf( "   Packets Intercepted  = %lu\r\n",po->stats.dhcp_rx_intercepted );
            printf( "   Packets Received     = %lu\r\n",po->stats.dhcp_rx );
            printf( "   Packets Filtered     = %lu\r\n",po->stats.dhcp_rx_filtered );
            printf( "   Packets Forwarded    = %lu\r\n",po->stats.dhcp_tx_forwarded );
            printf( "   Transmissions Failed = %lu\r\n",po->stats.dhcp_tx_failed );
            printf( "   Received Client Requests without Options    = %lu\r\n",po->stats.dhcp_rx_client_requests_without_options );
//          printf( "   Transmitted Client Requests without Options = %lu\r\n",po->stats.dhcp_tx_client_requests_without_options );
            printf( "   Transmitted Client Requests with Option82   = %lu\r\n",po->stats.dhcp_tx_client_requests_with_option82 );
            printf( "   Transmitted Client Requests with Option37   = %lu\r\n",po->stats.dhcp_tx_client_requests_with_option37 );
            printf( "   Transmitted Client Requests with Option18   = %lu\r\n",po->stats.dhcp_tx_client_requests_with_option18 );
            printf( "   Received Server Replies with Option82       = %lu\r\n",po->stats.dhcp_rx_server_replies_with_option82 );
            printf( "   Received Server Replies with Option37       = %lu\r\n",po->stats.dhcp_rx_server_replies_with_option37 );
            printf( "   Received Server Replies with Option18       = %lu\r\n",po->stats.dhcp_rx_server_replies_with_option18 );
//          printf( "   Received Server Replies without Options     = %lu\r\n",po->stats.dhcp_rx_server_replies_without_options );
            printf( "   Transmitted Server Replies without Options  = %lu\r\n",po->stats.dhcp_tx_server_replies_without_options );
            printf( "   Received Client Packets on Trusted Interface                 = %lu\r\n",po->stats.dhcp_rx_client_pkts_onTrustedIntf );
            printf( "   Received Client Packets with Options on Untrusted Interface  = %lu\r\n",po->stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf );
            printf( "   Received Server Packets on Untrusted Interface               = %lu\r\n",po->stats.dhcp_rx_server_pkts_onUntrustedIntf );
          }
          else  {
            printf(" Switch: Error reading DHCP statistics - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
          }
        }
        break;

      case 1322:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DHCP statistics cleared successfully\n\r");
        else
          printf(" Switch: DHCP statistics not cleared - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1400:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP snooping/querier configured\n\r");
        else
          printf(" Switch: Error configuring IGMP snooping/querier - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1401:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP instance added\n\r");
        else
          printf(" Switch: Error adding IGMP instance - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1402:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP instance removed\n\r");
        else
          printf(" Switch: Error removing IGMP instance - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1403:
        {
          msg_MCAssocChannel_t *po=(msg_MCAssocChannel_t *) &resposta.info[0];
          uint16 n, index;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            if (resposta.infoDim%sizeof(msg_MCAssocChannel_t) != 0) {
              printf(" Switch: Invalid structure size\r\n");
              break;
            }
            n = resposta.infoDim / sizeof(msg_MCAssocChannel_t);

            printf("Printing list of IGMP associations for Slot=%u:\n\r",po->SlotId);
            for (index=0; index<n; index++)
            {
              printf(" Idx %-4u: MC_evc=%-3lu ->  Group:%03lu.%03lu.%03lu.%03lu/%-3u  Source:%03lu.%03lu.%03lu.%03lu/%-3u\r\n",
                      po[index].entry_idx,
                      po[index].evcid_mc,
                     (po[index].channel_dstIp.addr.ipv4>>24) & 0xFF,
                     (po[index].channel_dstIp.addr.ipv4>>16) & 0xFF,
                     (po[index].channel_dstIp.addr.ipv4>> 8) & 0xFF,
                      po[index].channel_dstIp.addr.ipv4 & 0xFF,
                      po[index].channel_dstmask,
                     (po[index].channel_srcIp.addr.ipv4>>24) & 0xFF,
                     (po[index].channel_srcIp.addr.ipv4>>16) & 0xFF,
                     (po[index].channel_srcIp.addr.ipv4>> 8) & 0xFF,
                      po[index].channel_srcIp.addr.ipv4 & 0xFF,
                      po[index].channel_srcmask);
            }
            printf( "Done!\r\n");
          }
          else  {
            printf(" Switch: Error reading list of IGMP associations\n\r");
          }
        }
        break;

      case 1404:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP association added\n\r");
        else
          printf(" Switch: Error adding IGMP association - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1405:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP association removed\n\r");
        else
          printf(" Switch: Error removing IGMP association - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1406:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MC Client successfully added\n\r");
        else
          printf(" Switch: Error adding MC Client - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1407:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MC Client successfully removed\n\r");
        else
          printf(" Switch: Error removing MC Client - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

//    case 1400:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping/Querier configured\n\r");
//      else
//        printf(" Switch: IGMP Snooping/Querier NOT configured - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;
//
//    case 1402:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: interfaces added\n\r");
//      else
//        printf(" Switch: IGMP Snooping: interfaces NOT added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;
//
//    case 1403:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: interfaces removed\n\r");
//      else
//        printf(" Switch: IGMP Snooping: interfaces NOT removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;
//
//    case 1405:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: mclient vlans added\n\r");
//      else
//        printf(" Switch: IGMP Snooping: mclient vlans NOT added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;
//
//    case 1406:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: mclient vlans removed\n\r");
//      else
//        printf(" Switch: IGMP Snooping: mclient vlans NOT removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;
//
//    case 1408:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: mrouter vlans added\n\r");
//      else
//        printf(" Switch: IGMP Snooping: mrouter vlans NOT added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;
//
//    case 1409:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: mrouter vlans removed\n\r");
//      else
//        printf(" Switch: IGMP Snooping: mrouter vlans NOT removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;
//
//    case 1411:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping Querier: vlans added to querier\n\r");
//      else
//        printf(" Switch: IGMP Snooping Querier: vlans NOT added to querier - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;
//
//    case 1412:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping Querier: vlans removed to querier\n\r");
//      else
//        printf(" Switch: IGMP Snooping Querier: vlans NOT removed to querier - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
//      break;

      case 1420:
        {
          msg_MCActiveChannelsReply_t *po=(msg_MCActiveChannelsReply_t *) &resposta.info[0];
          uint16 index, n;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            if (resposta.infoDim==0 || resposta.infoDim%sizeof(msg_MCActiveChannelsReply_t) != 0) {
              printf(" Switch: Invalid structure size\r\n");
              break;
            }
            n = resposta.infoDim/sizeof(msg_MCActiveChannelsReply_t);

            printf( " MC channels (total=%u)\n\r", n);

            for (index=0; index<n; index++) {
              printf("#%-3u: GrpAddr %03lu.%03lu.%03lu.%03lu   SrcAddr=%03lu.%03lu.%03lu.%03lu   (type=%u)\r\n",
                     po[index].entryId,
                     (po[index].chIP>>24) & 0xFF,
                      (po[index].chIP>>16) & 0xFF,
                       (po[index].chIP>> 8) & 0xFF,
                         po[index].chIP & 0xFF,
                     (po[index].srcIP>>24) & 0xFF,
                      (po[index].srcIP>>16) & 0xFF,
                       (po[index].srcIP>> 8) & 0xFF,
                         po[index].srcIP & 0xFF,
                     po[index].chType);
            }
            printf( "Done!\r\n");
          }
          else  {
            printf(" Switch: Error reading list of active channels\n\r");
          }
        }
        break;

      case 1421:
        {
          msg_MCActiveChannelClientsResponse_t *po=(msg_MCActiveChannelClientsResponse_t *) &resposta.info[0];
          uint16 i;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            if (resposta.infoDim!=sizeof(msg_MCActiveChannelClientsResponse_t)) {
              printf(" Switch: Invalid structure size\r\n");
              break;
            }
            printf(" MC clients for Slot=%u, EVC=%lu and channel:%03lu.%03lu.%03lu.%03lu sourceIp:%03lu.%03lu.%03lu.%03lu (total=%u)\n\r",po->SlotId, po->evc_id,
                   (po->channelIp.s_addr>>24) & 0xFF, (po->channelIp.s_addr>>16) & 0xFF, (po->channelIp.s_addr>>8) & 0xFF, po->channelIp.s_addr & 0xFF,
                   (po->sourceIp.s_addr>>24) & 0xFF, (po->sourceIp.s_addr>>16) & 0xFF, (po->sourceIp.s_addr>>8) & 0xFF, po->sourceIp.s_addr & 0xFF,
                   po->n_clients_total);
            printf("Page %u of %u:\r\n",po->page_index,po->n_pages_total);
            for (i=0; i<po->n_clients_msg; i++) {
              printf("Client #%u\r\n",i);
              printf("  mask      = 0x%02x\r\n",po->clients_list[i].mask);
              printf("  OuterVlan = %u\r\n",po->clients_list[i].outer_vlan);
              printf("  InnerVlan = %u\r\n",po->clients_list[i].inner_vlan);
              printf("  Intf      = %u/%u\r\n",po->clients_list[i].intf.intf_type,po->clients_list[i].intf.intf_id);
            }
            printf( "Done!\r\n");
          }
          else  {
            printf(" Switch: Error reading list of active channels\n\r");
          }
        }
        break;

      case 1430:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Static MC Channel added successfully\n\r");
        else
          printf(" Switch: Static MC Channel NOT added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1431:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Static MC Channel removed successfully\n\r");
        else
          printf(" Switch: Static MC Channel NOT removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;


      case 1810:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully created new routing interface\n\r");
        else
          printf(" Error while creating new routing interface - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1811:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully removed routing interface\n\r");
        else
          printf(" Error while removing routing interface - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1820:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_RoutingArpTableResponse *ptr;
          uint8 entries, i;

          entries = resposta.infoDim/sizeof(msg_RoutingArpTableResponse);

          if (entries==0) {
            printf(" ARP Table: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_RoutingArpTableResponse));
            break;
          }

          for (i=0; i<entries; ++i) {
            ptr = &((msg_RoutingArpTableResponse *) &resposta.info[0])[i];
            printf("------------------------\r\n");
            printf("  Index       = %u\r\n",    ptr->index);
            printf("  Intf        = %u/%u\r\n", ptr->intf.intf_type, ptr->intf.intf_id);
            printf("  Type        = %u\r\n",    ptr->type);
            printf("  Age         = %u\r\n",    (unsigned int)ptr->age);
            printf("  Ip Address  = 0x%08X\r\n",(unsigned int)ptr->ipAddr);
            printf("  MAC Address = %02X:%02X:%02X:%02X:%02X:%02X\r\n", ptr->macAddr[0], ptr->macAddr[1], ptr->macAddr[2], ptr->macAddr[3], ptr->macAddr[4], ptr->macAddr[5]);
          }
        }
        else
          printf(" ARP Table: NOT read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1821:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully removed ARP entry\n\r");
        else
          printf(" Error while removing ARP entry - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1830:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_RoutingRouteTableResponse *ptr;
          uint8 entries, i;

          entries = resposta.infoDim/sizeof(msg_RoutingRouteTableResponse);

          if (entries==0) {
            printf(" Route Table: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_RoutingRouteTableResponse));
            break;
          }

          for (i=0; i<entries; ++i) {
            ptr = &((msg_RoutingRouteTableResponse *) &resposta.info[0])[i];
            printf("------------------------\r\n");
            printf("  Index               = %u\r\n",           ptr->index);
            printf("  Intf                = %u/%u\r\n",        ptr->intf.intf_type, ptr->intf.intf_id);
            printf("  Protocol            = %u\r\n",           ptr->protocol);
            printf("  Update Time         = %udays %uh%um%us\r\n", (unsigned int)ptr->updateTime.days, (unsigned int)ptr->updateTime.hours, (unsigned int)ptr->updateTime.minutes, (unsigned int)ptr->updateTime.seconds);
            printf("  Network Ip Address  = 0x%08X\r\n",       (unsigned int)ptr->networkIpAddr);
            printf("  Subnet Mask         = %u\r\n",           (unsigned int)ptr->subnetMask);
            printf("  Gw Ip Address       = 0x%08X\r\n",       (unsigned int)ptr->gwIpAddr);
            printf("  Preference          = %u\r\n",           (unsigned int)ptr->preference);
            printf("  Metric              = %u\r\n",           (unsigned int)ptr->metric);
          }
        }
        else
          printf(" Route Table: NOT read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1831:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully configure static route\n\r");
        else
          printf(" Error while configuring static route - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1832:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully removed static route\n\r");
        else
          printf(" Error while removing static route - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1840:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully created ping session\n\r");
        else
          printf(" Error while creating ping session - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1841:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_RoutingPingSessionQuery *ptr;
          uint8 entries;

          entries = resposta.infoDim/sizeof(msg_RoutingPingSessionQuery);

          if (entries==0) {
            printf(" Ping session query: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_RoutingPingSessionQuery));
            break;
          }

          ptr = (msg_RoutingPingSessionQuery *) &(resposta.info[0]);
          printf("------------------------\r\n");
          printf("  SessionIdx        = %u\r\n",          ptr->sessionIdx);
          printf("  Is Running        = %u\r\n",          ptr->isRunning);
          printf("  Probes Sent       = %u\r\n",          ptr->probeSent);
          printf("  Probes Succ       = %u\r\n",          ptr->probeSucc);
          printf("  Probes Fail       = %u\r\n",          ptr->probeFail);
          printf("  RTT (min/avg/max) = %lu/%lu/%lu\r\n", ptr->minRtt, ptr->avgRtt, ptr->maxRtt);
        }
        else
          printf(" Ping session query: NOT read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1842:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Sucessefully freed ping session\n\r");
        else
          printf(" Error while freeing ping session - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;


      case 1850:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully created Traceroute session\n\r");
        else
          printf(" Error while creating Traceroute session - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1851:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_RoutingTracertSessionQuery *ptr;
          uint8 entries;

          entries = resposta.infoDim/sizeof(msg_RoutingTracertSessionQuery);

          if (entries==0) {
            printf(" Traceroute session query: Invalid structure size (%u vs %u)\n\r", resposta.infoDim, sizeof(msg_RoutingTracertSessionQuery));
            break;
          }

          ptr = (msg_RoutingTracertSessionQuery *) &(resposta.info[0]);
          printf("------------------------\r\n");
          printf("  sessionIdx   = %u\r\n", ptr->sessionIdx);
          printf("  Is Running   = %u\r\n", ptr->isRunning);
          printf("  TTL          = %u\r\n", ptr->currTtl);
          printf("  Hop Count    = %u\r\n", ptr->currHopCount);
          printf("  Probe Count  = %u\r\n", ptr->currProbeCount);
          printf("  Test Attempt = %u\r\n", ptr->testAttempt);
          printf("  Test Success = %u\r\n", ptr->testSuccess);
        }
        else
          printf(" Traceroute session query: NOT read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1852:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_RoutingTracertSessionHopsResponse *ptr;
          uint8 entries, i;

          entries = resposta.infoDim/sizeof(msg_RoutingTracertSessionHopsResponse);

          if (entries==0) {
            printf(" Traceroute hops: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_RoutingTracertSessionHopsResponse));
            break;
          }

          for (i=0; i<entries; ++i) {
            ptr = &((msg_RoutingTracertSessionHopsResponse *) &resposta.info[0])[i];
            printf("------------------------\r\n");
            printf("  HopIdx            = %u\r\n",          ptr->hopIdx);
            printf("  TTL               = %u\r\n",          ptr->ttl);
            printf("  Ip Address        = %08X\r\n",        (unsigned int)ptr->ipAddr);
            printf("  RTT (min/avg/max) = %lu/%lu/%lu\r\n", ptr->minRtt, ptr->avgRtt, ptr->maxRtt);
            printf("  Probes Sent       = %u\r\n",          ptr->probeSent);
            printf("  Probes Recvd      = %u\r\n",          ptr->probeRecv);
          }
        }
        else
          printf(" Traceroute hops: NOT read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1853:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully freed traceroute session\n\r");
        else
          printf(" Error while freeing traceroute session - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1500:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_LACPLagInfo_t *ptr;
          uint8 nLags, i;

          nLags = resposta.infoDim/sizeof(msg_LACPLagInfo_t);

          if (nLags==0) {
            printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_LACPLagInfo_t));
            break;
          }

          for (i=0; i<nLags; i++) {
            ptr = &((msg_LACPLagInfo_t *) &resposta.info[0])[i];
            printf(" Slot %u, LAG %u\r\n", ENDIAN_SWAP8(ptr->SlotId), ENDIAN_SWAP8(ptr->id));
            printf("  Admin               = %s\r\n",(ENDIAN_SWAP8(ptr->admin) ? "Enabled" : "Disabled"));
            printf("  STP state           = %s\r\n",(ENDIAN_SWAP8(ptr->stp_enable) ? "Enabled" : "Disabled"));
            printf("  LAG type            = %s\r\n",(ENDIAN_SWAP8(ptr->static_enable) ? "Static" : "Dynamic"));
            printf("  LoadBalance profile = %u\r\n",ENDIAN_SWAP8(ptr->loadBalance_mode));
            printf("  Port bitmap         = 0x%08lx 0x%08lx\r\n", ENDIAN_SWAP32((unsigned long) ptr->members_pbmp2), ENDIAN_SWAP32((unsigned long) ptr->members_pbmp));
          }
          printf(" Switch: LAG configurations read successfully\n\r");
        }
        else
          printf(" Switch: LACP: Lag configurations NOT read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1501:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: LACP: LAG created successfully\n\r");
        else
          printf(" Switch: LACP: Lag NOT created - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1502:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: LACP: LAG removed successfully\n\r");
        else
          printf(" Switch: LACP: Lag NOT removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1503:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_LACPLagStatus_t *ptr;
          uint8 nLags, i;

          nLags = resposta.infoDim/sizeof(msg_LACPLagStatus_t);

          if (nLags==0) {
            printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_LACPLagStatus_t));
            break;
          }

          for (i=0; i<nLags; i++) {
            ptr = &((msg_LACPLagStatus_t *) &resposta.info[0])[i];
            printf(" Slot %u LAG %u\r\n", ENDIAN_SWAP8(ptr->SlotId), ENDIAN_SWAP8(ptr->id));
            printf("  Admin               = %s\r\n",(ENDIAN_SWAP8(ptr->admin) ? "Enabled" : "Disabled"));
            printf("  Link State          = %s\r\n",(ENDIAN_SWAP8(ptr->link_status) ? "UP" : "DOWN"));
            printf("  Port channel type   = %s\r\n",(ENDIAN_SWAP8(ptr->port_channel_type) ? "Static" : "Dynamic"));
            printf("  Member Ports bitmap = 0x%08lx 0x%08lx\r\n", ENDIAN_SWAP32((unsigned long) ptr->members_pbmp2), ENDIAN_SWAP32((unsigned long) ptr->members_pbmp1));
            printf("  Active Ports bitmap = 0x%08lx 0x%08lx\r\n", ENDIAN_SWAP32((unsigned long) ptr->active_members_pbmp2), ENDIAN_SWAP32((unsigned long) ptr->active_members_pbmp1));
          }
          printf(" Switch: LAG status read successfully\n\r");
        }
        else
          printf(" Switch: LACP: Lag status NOT read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1504:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: LACP Admin State settled successfully\n\r");
        else
          printf(" Switch: LACP Admin State NOT settled - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1505:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_LACPAdminState_t *ptr;
          uint8 nStructs, i;

          nStructs = resposta.infoDim/sizeof(msg_LACPAdminState_t);

          if (nStructs==0) {
            printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_LACPAdminState_t));
            break;
          }

          for (i=0; i<nStructs; i++) {
            ptr = &((msg_LACPAdminState_t *) &resposta.info[0])[i];
            printf(" Slot %u Physical port %u:\r\n", ENDIAN_SWAP8(ptr->SlotId), ENDIAN_SWAP8(ptr->id));
            printf("  State Aggregation = %s\r\n",  (ENDIAN_SWAP8(ptr->state_aggregation) ? "Enabled" : "Disabled"));
            printf("  LACP Activity     = %s\r\n",  (ENDIAN_SWAP8(ptr->lacp_activity)     ? "True" : "False"));
            printf("  LACP Timeout      = %s\r\n",  (ENDIAN_SWAP8(ptr->lacp_timeout)      ? "Short (3s)" : "Long (90s)"));
          }
          printf(" Switch: LACP Admin State read successfully\n\r");
        }
        else
          printf(" Switch: LACP Admin State NOT read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1510:
        {
          msg_LACPStats_t *po=(msg_LACPStats_t *) &resposta.info[0];
          uint8 nStructs, i;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            if (resposta.infoDim==0 || (resposta.infoDim%sizeof(msg_LACPStats_t))!=0) {
              printf(" Switch: Invalid structure size\r\n");
              break;
            }
            nStructs = resposta.infoDim/sizeof(msg_LACPStats_t);

            for (i=0; i<nStructs; i++) {
              printf( " LACP statistics for slot %u port %u:\n\r", ENDIAN_SWAP8(po[i].SlotId), ENDIAN_SWAP8(po[i].id));
              printf( "   RX LACPdu's = %lu\n\r", ENDIAN_SWAP32(po[i].LACPdus_rx));
              printf( "   TX LACPdu's = %lu\n\r", ENDIAN_SWAP32(po[i].LACPdus_tx));
            }
            printf( "Done!\r\n");
          }
          else  {
            printf(" Switch: Error reading LACPdu statistics - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
          }
        }
        break;

      case 1511:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: LACP Stats cleared successfully\n\r");
        else
          printf(" Switch: LACP Stats NOT cleared - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1600:
        {
          msg_HwEthMef10Evc_t *ptr;
          int i;

          // Pointer to data array
          ptr = (msg_HwEthMef10Evc_t *) &(comando.info[0]);

          if (resposta.flags != (FLAG_RESPOSTA | FLAG_ACK))
          {
            printf("Error reading EVC# %u config\n\r", (unsigned int)ptr->id);
            break;
          }

          ptr = (msg_HwEthMef10Evc_t *) &(resposta.info[0]);

          printf("Slot %u EVC# %u\r\n", ENDIAN_SWAP8(ptr->SlotId), (unsigned int) ENDIAN_SWAP32(ptr->id));
          printf(" .flags         = 0x%04lx\r\n", ENDIAN_SWAP32(ptr->flags));
          printf("   .stacked       = %s\r\n", ENDIAN_SWAP32(ptr->flags) & 0x0004 ? "True":"False");
          printf("   .MACLearning   = %s\r\n", ENDIAN_SWAP32(ptr->flags) & 0x0008 ? "Enabled":"Disabled");
          printf("   .CPU trapping  = %s\r\n", ENDIAN_SWAP32(ptr->flags) & 0x0010 ? "On":"Off");
          printf("   .DHCP snooping = %s\r\n", ENDIAN_SWAP32(ptr->flags) & 0x0100 ? "On":"Off");
          printf(" .MC Flood type = %u (%s)\r\n", ENDIAN_SWAP8(ptr->mc_flood),
                 ((ENDIAN_SWAP8(ptr->mc_flood) == 0) ? "All" : ((ENDIAN_SWAP8(ptr->mc_flood) == 1) ? "Unknown":"None")));
          printf(" .Interfaces #  = %u\r\n", ptr->n_intf);
          for (i=0; i<ptr->n_intf; i++) {
            printf("   %s# %02u %s VID=%u+%u\r\n",
                   ((ENDIAN_SWAP8(ptr->intf[i].intf_type) == 0) ? "PHY" : "LAG"),
                     ENDIAN_SWAP8(ptr->intf[i].intf_id),
                   ((ENDIAN_SWAP8(ptr->intf[i].mef_type) == 0) ? "Root" : "Leaf"),
                     ENDIAN_SWAP16(ptr->intf[i].vid),
                     ENDIAN_SWAP16(ptr->intf[i].inner_vid));
          }
        }
        break;

      case 1601:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC successfully created\n\r");
        else
          printf("Failed to create EVC - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1602:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC successfully deleted\n\r");
        else
          printf("Failed to delete EVC - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1603:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("Port successfully added to EVC\n\r");
        else
          printf("Failed to add port to EVC - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1604:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("Port successfully removed from EVC\n\r");
        else
          printf("Failed to remove port from EVC - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1605:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC stacked bridge successfully created\n\r");
        else
          printf("Failed to create EVC stacked bridge - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1606:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC stacked bridge successfully deleted\n\r");
        else
          printf("Failed to delete EVC stacked bridge - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1607:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC GEM flow successfully created\n\r");
        else
          printf("Failed to create EVC GEM flow - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1608:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC GEM flow successfully deleted\n\r");
        else
          printf("Failed to delete EVC GEM flow - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1609:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC options applied succesfully\n\r");
        else
          printf("Failed to apply EVC options - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1610:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK)) {
          msg_NtwConnectivity_t *pNtwConn = (msg_NtwConnectivity_t *) resposta.info;
          printf("Network Connectivity (mask=0x%08lx)\r\n",  ENDIAN_SWAP32(pNtwConn->mask));
          printf("  Slot %u\r\n",                           ENDIAN_SWAP8 (pNtwConn->SlotId));
          printf("  IP Addr         = %lu.%lu.%lu.%lu\r\n", (ENDIAN_SWAP32(pNtwConn->ipaddr)  >> 24) & 0xFF, (ENDIAN_SWAP32(pNtwConn->ipaddr)  >> 16) & 0xFF,
                                                            (ENDIAN_SWAP32(pNtwConn->ipaddr)  >>  8) & 0xFF,  ENDIAN_SWAP32(pNtwConn->ipaddr)         & 0xFF);
          printf("  Mask            = %lu.%lu.%lu.%lu\r\n", (ENDIAN_SWAP32(pNtwConn->netmask) >> 24) & 0xFF, (ENDIAN_SWAP32(pNtwConn->netmask) >> 16) & 0xFF,
                                                            (ENDIAN_SWAP32(pNtwConn->netmask) >>  8) & 0xFF,  ENDIAN_SWAP32(pNtwConn->netmask)        & 0xFF);
          printf("  Gateway         = %lu.%lu.%lu.%lu\r\n", (ENDIAN_SWAP32(pNtwConn->gateway) >> 24) & 0xFF, (ENDIAN_SWAP32(pNtwConn->gateway) >> 16) & 0xFF,
                                                            (ENDIAN_SWAP32(pNtwConn->gateway) >>  8) & 0xFF,  ENDIAN_SWAP32(pNtwConn->gateway)        & 0xFF);
          printf("  Mgmt VLAN ID    = %u\r\n",              ENDIAN_SWAP16(pNtwConn->mgmtVlanId));
          printf("  Interfaces (%d):\r\n", ENDIAN_SWAP8(pNtwConn->n_intf));
          int i;
          for (i = 0; i < ENDIAN_SWAP8(pNtwConn->n_intf); i++) {
            printf("    Intf %u/%u\r\n", ENDIAN_SWAP8(pNtwConn->intf[i].intf_type), ENDIAN_SWAP8(pNtwConn->intf[i].intf_id));
          }
        }
        else
          printf("Failed to get network connectivity config - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1611:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("Network Connectivity successfully configured\n\r");
        else
          printf("Failed to set network connectivity config - error %08lx\r\n", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1620:
        {
          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          {
            msg_HwEthBwProfile_t *ptr = (msg_HwEthBwProfile_t *) &resposta.info[0];

            printf(" Slot=%u EVCid=%lu\r\n", ENDIAN_SWAP8(ptr->SlotId), ENDIAN_SWAP32(ptr->evcId));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_HWETH_BWPROFILE_MASK_INTF_SRC)
              printf(" SrcIntf=%u/%u\r\n", ENDIAN_SWAP8(ptr->intf_src.intf_type), ENDIAN_SWAP8(ptr->intf_src.intf_id));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_HWETH_BWPROFILE_MASK_INTF_DST)
              printf(" DstIntf=%u/%u\r\n", ENDIAN_SWAP8(ptr->intf_dst.intf_type), ENDIAN_SWAP8(ptr->intf_dst.intf_id));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_HWETH_BWPROFILE_MASK_SVLAN)
              printf(" SVid=%u\r\n", ENDIAN_SWAP16(ptr->service_vlan));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_HWETH_BWPROFILE_MASK_CVLAN)
              printf(" CVid=%u\r\n", ENDIAN_SWAP16(ptr->client_vlan));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_HWETH_BWPROFILE_MASK_PROFILE)
            {
              printf(" CIR=%llu bps\r\n",   ENDIAN_SWAP64(ptr->profile.cir));
              printf(" EIR=%llu bps\r\n",   ENDIAN_SWAP64(ptr->profile.eir));
              printf(" CBS=%llu bytes\r\n", ENDIAN_SWAP64(ptr->profile.cbs));
              printf(" EBS=%llu bytes\r\n", ENDIAN_SWAP64(ptr->profile.ebs));
            }
            else
            {
              printf(" Non existent profile!\r\n");
            }
            printf("Switch: BW profile read successfully\n\r");
          }
          else
            printf(" Switch: BW profile not read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        }
        break;

      case 1621:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: BW profile added successfully\n\r");
        else
          printf(" Switch: BW profile not added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1622:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: BW profile removed successfully\n\r");
        else
          printf(" Switch: BW profile not removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1624:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Storm Control configured successfully\n\r");
        else
          printf(" Switch: Storm Control NOT configured - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1625:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Storm Control reseted successfully\n\r");
        else
          printf(" Switch: Storm Control not reseted - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1626:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Storm Control cleared successfully\n\r");
        else
          printf(" Switch: Storm Control not cleared - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1627:
      case 1628:
      case 1629:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Storm Control configured successfully\n\r");
        else
          printf(" Switch: Storm Control not configured - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1630:
        {
          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          {
            msg_evcStats_t *ptr;

            if (resposta.infoDim != sizeof(msg_evcStats_t)) {
              printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_evcStats_t));
              break;
            }

            ptr = (msg_evcStats_t *) &resposta.info[0];

            printf(" Flow Counters:\r\n");
            printf(" Slot=%u FlowId=%lu\r\n", ENDIAN_SWAP8(ptr->SlotId), ENDIAN_SWAP32(ptr->evc_id));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_EVC_COUNTERS_MASK_INTF)
              printf(" Intf   =%u/%u\r\n", ENDIAN_SWAP8(ptr->intf.intf_type), ENDIAN_SWAP8(ptr->intf.intf_id));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_EVC_COUNTERS_MASK_SVLAN)
              printf(" SVid   =%u\r\n", ENDIAN_SWAP16(ptr->service_vlan));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_EVC_COUNTERS_MASK_CVLAN)
              printf(" CVid   =%u\r\n", ENDIAN_SWAP16(ptr->client_vlan));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_EVC_COUNTERS_MASK_CHANNEL)
              printf(" Channel=%03u:%03u:%03u:%03u\r\n",(unsigned int) ((ENDIAN_SWAP32(ptr->channel_ip)>>24) & 0xFF),
                                                        (unsigned int) ((ENDIAN_SWAP32(ptr->channel_ip)>>16) & 0xFF),
                                                        (unsigned int) ((ENDIAN_SWAP32(ptr->channel_ip)>> 8) & 0xFF),
                                                        (unsigned int) ( ENDIAN_SWAP32(ptr->channel_ip) & 0xFF ));
            if (ENDIAN_SWAP8(ptr->mask) & MSG_EVC_COUNTERS_MASK_STATS)
            {
              if (ENDIAN_SWAP8(ptr->stats.mask_stat) & MSG_EVC_COUNTERS_MASK_STATS_RX)
              {
                printf("RX stats...\r\n");
                printf(" Total    : %10lu\r\n", ENDIAN_SWAP32(ptr->stats.rx.pktTotal)    );
                printf(" Unicast  : %10lu\r\n", ENDIAN_SWAP32(ptr->stats.rx.pktUnicast)  );
                printf(" Multicast: %10lu\r\n", ENDIAN_SWAP32(ptr->stats.rx.pktMulticast));
                printf(" Broadcast: %10lu\r\n", ENDIAN_SWAP32(ptr->stats.rx.pktBroadcast));
                printf(" Dropped  : %10lu\r\n", ENDIAN_SWAP32(ptr->stats.rx.pktDropped)  );
              }
              if (ENDIAN_SWAP8(ptr->stats.mask_stat) & MSG_EVC_COUNTERS_MASK_STATS_TX)
              {
                printf("TX stats...\r\n");
                printf(" Total    : %10lu\r\n", ENDIAN_SWAP32(ptr->stats.tx.pktTotal)    );
                printf(" Unicast  : %10lu\r\n", ENDIAN_SWAP32(ptr->stats.tx.pktUnicast)  );
                printf(" Multicast: %10lu\r\n", ENDIAN_SWAP32(ptr->stats.tx.pktMulticast));
                printf(" Broadcast: %10lu\r\n", ENDIAN_SWAP32(ptr->stats.tx.pktBroadcast));
                printf(" Dropped  : %10lu\r\n", ENDIAN_SWAP32(ptr->stats.tx.pktDropped)  );
              }
            }
            else
            {
              printf(" Non existent stats!\r\n");
            }
            printf(" Switch: EVC counters read successfully\n\r");
          }
          else
            printf(" Switch: Flow counters not read - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
          break;
        }
        break;

      case 1632:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: EVC counters added successfully\n\r");
        else
          printf(" Switch: EVC counters not added - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;

      case 1633:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: EVC counters removed successfully\n\r");
        else
          printf(" Switch: EVC counters not removed - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;
      case 1700:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" IP Source Guard Correctly Configured\n\r");
        else
          printf(" IP Source Guard not Configured - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;
      case 1701:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" IP Source Guard Static Entry Correctly Configured\n\r");
        else
          printf(" IP Source Guard Static Entry not Configured - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;
      case 1710:      
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" IGMP Multicast Package Correctly Added\n\r");
        else
          printf(" IGMP Multicast  Package Failed to Add - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
        break;
      case 1711:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP Multicast Package Correctly Removed\n\r");
      else
        printf(" IGMP Multicast Package Failed to Remove - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1712:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP Multicast Channel Packages Correctly Added\n\r");
      else
        printf(" IGMP Multicast Channel Packages Failed to Add - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1713:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP Multicast Channel Packages Correctly Removed\n\r");
      else
        printf(" IGMP Multicast Channel Packages Failed to Remove - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1714:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP Multicast Service Correctly Added\n\r");
      else
        printf(" IGMP Multicast Service Failed to Add - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1715:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP Multicast Service Correctly Removed\n\r");
      else
        printf(" IGMP Multicast Service Failed to Remove - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1716:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP Unicast Client  Correctly Added\n\r");
      else
        printf(" IGMP Unicast Client Failed to Add - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1717:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP Unicast Client  Correctly Removed\n\r");
      else
        printf(" IGMP Unicast Client Failed to Remove - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1718:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP MacBridge Client  Correctly Added\n\r");
      else
        printf(" IGMP MacBridge Client Failed to Add - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1719:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" IGMP MacBridge Client  Correctly Removed\n\r");
      else
        printf(" IGMP MacBridge Client Failed to Remove - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;      

      case 1890:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" FrameDelay MEP Correctly Added\n\r");
      else
        printf(" FrameDelay MEP Failed to Add - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1891:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" FrameDelay MEP Correctly Removed\n\r");
      else
        printf(" FrameDelay MEP Failed to Remove - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1892:
        {
          MSG_FRAMEDELAY_status *po=(MSG_FRAMEDELAY_status *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            // Validate size
            if (resposta.infoDim!=sizeof(MSG_FRAMEDELAY_status)) {
              printf(" FrameDelay: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(MSG_FRAMEDELAY_status),resposta.infoDim);
              break;
            }
            printf("FrameDelay: DM_Max             %lld\n\r", po->DM_Max);
            printf("FrameDelay: DM_Min             %lld\n\r", po->DM_Min);
            printf("FrameDelay: DM_Total           %lld\n\r", po->DM_Total);
            printf("FrameDelay: DM_packet_number   %d\n\r", po->DM_packet_number);
            printf("FrameDelay: results read successfully\n\r");
          }
          else
            printf(" FrameDelay: Error reading results\n\r");
        }
        break;
      case 1893:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" FrameLoss MEP Correctly Added\n\r");
      else
        printf(" FrameLoss MEP Failed to Add - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1894:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" FrameLoss MEP Correctly Removed\n\r");
      else
        printf(" FrameLoss MEP Failed to Remove - error %08lx\n\r", ENDIAN_SWAP32(*(unsigned long*)resposta.info));
      break;
      case 1895:
        {
          MSG_FRAMELOSS_status *po=(MSG_FRAMELOSS_status *) &resposta.info[0];

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            // Validate size
            if (resposta.infoDim!=sizeof(MSG_FRAMELOSS_status)) {
              printf(" FrameDelay: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(MSG_FRAMELOSS_status),resposta.infoDim);
              break;
            }
            printf("FrameLoss: Delta_LM_tx_i      %lld\n\r", po->Delta_LM_tx_i);
            printf("FrameLoss: Delta_LM_rx_i      %lld\n\r", po->Delta_LM_rx_i);
            printf("FrameLoss: Delta_LM_tx_e      %lld\n\r", po->Delta_LM_tx_e);
            printf("FrameLoss: Delta_LM_rx_e      %lld\n\r", po->Delta_LM_rx_e);
            printf("FrameLoss: results read successfully\n\r");
          }
          else
            printf(" FrameDelay: Error reading results\n\r");
        }
        break;
      default:
        printf(" Resposta a mensagem %u\n\r",msg);
        break;
    }

    //printf("Fim do send BUGA to Controlo...\n\r");
    fflush(stdout);
    exit(0);
  }//if argc == m 
  else if ( !strcmp(argv[1],"testit") && ( argc >= 3 ) )
  {
   unsigned int i;

    if (StrToLongLong(argv[2],&valued)>=0)  msg = (unsigned int)valued;
    else
    {
      help_oltBuga();
      exit(0);
    }

    //printf("A mensagem a enviar e' a %d\n\r", msg);

    // 1 - Preparar mensagem a enviar ao modulo de controlo
    comando.protocolId   = 1;
    comando.srcId        = PORTO_TX_MSG_BUGA;
    comando.dstId        = PORTO_RX_MSG_BUGA;
    comando.flags        = (FLAG_COMANDO); //(IPCLIB_FLAGS_CMD | (IPC_UID<<4));
    comando.counter      = rand ();
    comando.msgId        = msg;
    comando.infoDim      = argc-3;
    for(i=3; i<argc; i++) {
        if (StrToLongLong(argv[i],&valued)<0) {
            help_oltBuga();
            exit(0);
        }
        comando.info[i-3]= valued;
    }

    canal_buga=open_ipc(PORTO_TX_MSG_BUGA,IP_LOCALHOST,NULL,20);
    if ( canal_buga<0 )
    {
      printf("Erro no open IPC do BUGA...\n\r");
      fflush(stdout);
      exit(0);
    }
    valued = send_data (canal_buga, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta);
    close_ipc(canal_buga);
    if ( valued )
    {
      printf("Erro %llu  no send_data IPC do BUGA...\n\r", valued);
      fflush(stdout);
      exit(0);
    }

    printf("RESPOSTA: msg=%4.4x\tflags=%2.2u\tinfoDim=%u\n\r", resposta.msgId, resposta.flags, resposta.infoDim);
    for (i=0; i<resposta.infoDim; i++) printf("\t%2.2x", resposta.info[i]);
    if (resposta.infoDim) printf("\n\r"); 
  }//else if ( !strcmp(argv[1],"testit") && ( argc >= 3 ) )
  else
  {
    help_oltBuga();
    exit(0);
  }

  fflush(stdout);
  return 0;
}


static int convert_ipaddr2uint64(const char *ipaddr, uint64 *value_uint64)
{
  const char *start_ipaddr;
  uint8  address[4] = { 0, 0, 0, 0};
  uint8  index, char_i;
  uint32 multiplier;

  // Validate argument
  if (ipaddr==NULL || *ipaddr=='\0' || value_uint64==NULL)
    return -1;

  // Search for the next non space/tab character
  for (; (*ipaddr==' ' || *ipaddr=='\t') && *ipaddr!='\0'; ipaddr++ );

  start_ipaddr = ipaddr;

  // Search for the end of the argument
  for (; *ipaddr!='\0' && (*ipaddr=='.' || isdigit(*ipaddr)) ; ipaddr++ );

  if (start_ipaddr==ipaddr)
    return -1;

  // Initialize Decimal multiplier
  char_i = 0;
  multiplier = 1;
  // Run all characters starting from the last one
  for (index=0,--ipaddr; index<4 && ipaddr>=start_ipaddr; ipaddr--)  {
    // If character is a decimal digit...
    if (isdigit(*ipaddr) && char_i<3) {
      address[index] += (uint8) (*ipaddr-'0')*multiplier;   // update address array
      multiplier*=10;                                       // update decimal multiplier for next digit
      char_i++;
    }
    // Other, is a dot character
    else
    {
      index++;                                              // Increment address array index
      multiplier=1;                                         // Reinitialize decimal multiplier
      char_i = 0;
    }
  }

  // Calculate uint32 value
  *value_uint64 = ((uint32) address[0]<<0) | ((uint32) address[1]<<8) | ((uint32) address[2]<<16) | ((uint32) address[3]<<24);

  return 0;
}

#if 0
static int convert_macaddr2uint64(const char *macaddr, uint64 *value_uint64)
{
  const char *start_macaddr;
  uint8  address[6] = { 0, 0, 0, 0, 0, 0};
  uint8  index, char_i, value;
  uint32 multiplier;

  // Validate argument
  if (macaddr==NULL || *macaddr=='\0' || value_uint64==NULL)
    return -1;

  // Search for the next non space/tab character
  for (; (*macaddr==' ' || *macaddr=='\t') && *macaddr!='\0'; macaddr++ );

  start_macaddr = macaddr;

  // Search for the end of the argument
  for (; *macaddr!='\0' && (*macaddr==':' || *macaddr=='-' || isxdigit(*macaddr)); macaddr++ );

  if (start_macaddr==macaddr)
    return -1;

  // Initialize Decimal multiplier
  char_i = 0;
  multiplier = 1;
  // Run all characters starting from the last one
  for (index=0,--macaddr; index<6 && macaddr>=start_macaddr; macaddr--)  {
    // If character is a decimal digit...
    if (isxdigit(*macaddr) && char_i<2) {
      if (*macaddr>='0' && *macaddr<='9')
        value = (uint8) (*macaddr - '0');
      else if (*macaddr>='a' && *macaddr<='f')
        value = (uint8) (*macaddr - 'a' + 10);
      else if (*macaddr>='A' && *macaddr<='F')
        value = (uint8) (*macaddr - 'A' + 10);
      else
        value = 0;

      address[index] += value * multiplier;                 // update address array
      multiplier*=10;                                       // update decimal multiplier for next digit
      char_i++;
    }
    // Other, is a dot character
    else
    {
      index++;                                              // Increment address array index
      multiplier=1;                                         // Reinitialize decimal multiplier
      char_i = 0;
    }
  }

  // Calculate uint32 value
  *value_uint64 = ((uint64) address[0]<<0) |
                  ((uint64) address[1]<<8) |
                  ((uint64) address[2]<<16) |
                  ((uint64) address[3]<<24) |
                  ((uint64) address[4]<<32) |
                  ((uint64) address[5]<<40);

  return 0;
}
#endif

static int convert_macaddr2array(const char *macaddr, uint8 *array)
{
  const char *start_macaddr;
  uint8  address[6] = { 0, 0, 0, 0, 0, 0};
  uint8  index, char_i, value;
  uint32 multiplier;

  // Validate argument
  if (macaddr==NULL || *macaddr=='\0' || array==NULL)
    return -1;

  // Search for the next non space/tab character
  for (; (*macaddr==' ' || *macaddr=='\t') && *macaddr!='\0'; macaddr++ );

  start_macaddr = macaddr;

  // Search for the end of the argument
  for (; *macaddr!='\0' && (*macaddr==':' || *macaddr=='-' || isxdigit(*macaddr)); macaddr++ );

  if (start_macaddr==macaddr)
    return -1;

  // Initialize Decimal multiplier
  char_i = 0;
  multiplier = 1;
  // Run all characters starting from the last one
  for (index=0,--macaddr; index<6 && macaddr>=start_macaddr; macaddr--)  {
    // If character is a decimal digit...
    if (isxdigit(*macaddr) && char_i<2) {
      if (*macaddr>='0' && *macaddr<='9')
        value = (uint8) (*macaddr - '0');
      else if (*macaddr>='a' && *macaddr<='f')
        value = (uint8) (*macaddr - 'a' + 10);
      else if (*macaddr>='A' && *macaddr<='F')
        value = (uint8) (*macaddr - 'A' + 10);
      else
        value = 0;

      address[index] += value * multiplier;                 // update address array
      multiplier*=10;                                       // update decimal multiplier for next digit
      char_i++;
    }
    // Other, is a dot character
    else
    {
      index++;                                              // Increment address array index
      multiplier=1;                                         // Reinitialize decimal multiplier
      char_i = 0;
    }
  }

  // Calculate uint32 value
  array[5] = address[0];
  array[4] = address[1];
  array[3] = address[2];
  array[2] = address[3];
  array[1] = address[4];
  array[0] = address[5];

  return 0;
}

