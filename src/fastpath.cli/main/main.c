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

void help_oltBuga(void)
{
  printf(
        "\n\n\r"
        "usar: buga [Opcoes]\n\r"
        "\n\r"
        "Opcoes:\n\r"
        "help [comando]\n\r"
        "ping [period] [N] - Waits until fastpath application is up or return error code [0-OK, 2-CRASH]\r\n"
        "m 1000 console[/dev/...]\n\r"
        "m 1001 console[/dev/...] - Logger output\n\r"
        "m 1004 - Get resources state\r\n"
        "m 1005 - Get current slot map configuration\r\n"
        "m 1006 <enable> <port1> <port2> ... - Enable PRBS TX/RX machine\r\n"
        "m 1007 <port1> <port2> ... - Read number of PRBS errors\r\n"
        "m 1008 - Validate provided slot map configuration\r\n"
        "m 1009 - Apply new slot map configuration\r\n"
        "m 1010 port[0-17] enable[0,1] speed[1G=3,2.5G=4] fullduplex[0,1] framemax(bytes) lb[0,1] macLearn[0,1] - switch port configuration\n\r"
        "m 1011 port[0-17] - get switch port configuration\n\r"
        "m 1012 port[0-17] - Get Phy states\n\r"
        "m 1013 slot[2-19] - Apply linkscan procedure\n\r"
        "m 1014 slot[2-19] port[0-3] cmd[0/1] - (Uplink) Protection command\n\r"
        "m 1015 [0-Phy,1-Lag]/[intf#] - Get port type definitions\r\n"
        "m 1016 slot=[0-17] intf=<[0-Phy;1-Lag]/intf#> defvid=[1-4095] defprio=[0-7] aftypes=[0/1] ifilter=[0/1] rvlanreg=[0/1] vlanaware=[0/1] type=[0/1/2]\r\n"
        "       dtag=[0/1] otpid=[XXXXh] itpid=[XXXXh] etype=[0/1/2] mlen=[0/1] mlsmen=[0/1] mlsmprio=[0-7] mlsmsp=[0/1] trust=[0/1] - Set port type definitions\r\n"
        "m 1017 [0-Phy,1-Lag]/[intf#] - Get MAC address of given interface\r\n"
        "m 1018 [0-Phy,1-Lag]/[intf#] macAddr[xxxxxxxxxxxxh] - Set MAC address for the provided interface\r\n"
        "m 1020 port[0-17] - Show switch RFC2819 statistics\n\r"
        "m 1021 port[0-17] - Clear switch RFC2819 statistics\n\r"
        "m 1030 [0-Phy,1-Lag]/[intf#] - Get QoS configuration\r\n"
        "m 1031 [0-Phy,1-Lag]/[intf#] trustMode[1:Untrust;2:802.1P;3:IPprec;4:DSCP] shapingRate[Mbps] cos_pr0[0-7] cos_pr1 ... cos_pr7 - Set general QoS configuration\r\n"
        "m 1032 [0-Phy,1-Lag]/[intf#] cos[0-7] scheduler[1:Strict;2:Weighted] min_bandwidth[Mbps] max_bandwidth[Mbps] - Set specific QoS configuration\r\n"
        "m 1040 <startId> <numEntries> - Read MAC table\r\n"
        "m 1041 vlan_id[1-4095] macAddr[xxxxxxxxxxxxh] [0-Phy,1-Lag]/[intf#] - Add a static entry to the MAC table\r\n"
        "m 1042 vlan_id[1-4095] macAddr[xxxxxxxxxxxxh] - Remove an entry from MAC table\r\n"
        "m 1043 - Flush all entries of MAC table\r\n"
        "m 1220 flow_id[1-127] [0-Phy,1-Lag]/[intf#] cvid[1-4095] - Read DHCPop82 profile\n\r"
        "m 1221 flow_id[1-127] [0-Phy,1-Lag]/[intf#] cvid[1-4095] op82/op37/op18 <circuitId> <remoteId> - Define a DHCPop82 profile\n\r"
        "m 1222 flow_id[1-127] [0-Phy,1-Lag]/[intf#] cvid[1-4095] - Remove a DHCPop82 profile\n\r"
        "m 1240 <page> - Read DHCP binding table (start reading from page 0)\r\n"
        "m 1242 macAddr[xxxxxxxxxxxxh] - Remove a MAC address from DHCP Binding table\r\n"
        "m 1310 flow_id[1-127] [0-Phy,1-Lag]/[intf#] (ovid[0-4095] cvid[1-4095]) - Show IGMP statistics for interface <type>/<id> and client <cvid> associated to EVC <flow_id>\n\r"
        "m 1312 flow_id[1-127] [0-Phy,1-Lag]/[intf#] (ovid[0-4095] cvid[1-4095]) - Clear IGMP statistics for interface<type>/<id> and client <cvid> associated to EVC <flow_id>\n\r"
        "m 1320 flow_id[1-127] [0-Phy,1-Lag]/[intf#] (cvid[1-4095]) - Show DHCP statistics for interface <type>/<id> and client <cvid> associated to EVC <flow_id>\n\r"
        "m 1322 flow_id[1-127] [0-Phy,1-Lag]/[intf#] (cvid[1-4095]) - Clear DHCP statistics for interface<type>/<id> and client <cvid> associated to EVC <flow_id>\n\r"
        "m 1400 [admin=<0/1>] [ipaddr=x.x.x.x] [cos=0..7] [gmi=<group_membership_interval>] [qi=<querier_interval>] - Configure igmp snooping + querier\r\n"
        "m 1401 MC_flow_id[1-127] UC_flow_id[1-127]  - Add IGMP instance with the MC+UC evc's pair\r\n"
        "m 1402 MC_flow_id[1-127] UC_flow_id[1-127]  - Remove IGMP instance with the MC+UC evc's pair\r\n"
        "m 1403 MC_evcId start_index[0-...] - Get list of IGMP channel-associations\r\n"
        "m 1404 MC_evcId[1-127] groupAddr[ddd.ddd.ddd.ddd] sourceAddr[ddd.ddd.ddd.ddd] groupMaskBits[22-32] sourceMaskBits[22-32] - Add IGMP channel-associations\r\n"
        "m 1405 MC_evcId[1-127] groupAddr[ddd.ddd.ddd.ddd] sourceAddr[ddd.ddd.ddd.ddd] groupMaskBits[22-32] sourceMaskBits[22-32] - Remove IGMP channel-associations\r\n"
        "m 1406 MC_flow_id[1-127] [0-Phy,1-Lag]/[intf#] ovid[0-4095] cvid[0-4095] - Add MC client to IGMP instance\r\n"
        "m 1407 MC_flow_id[1-127] [0-Phy,1-Lag]/[intf#] ovid[0-4095] cvid[0-4095] - Remove MC client to IGMP instance\r\n"
      /*"m 1400 snooping_admin[0/1] querier_admin[0/1] querier_ipaddr[ddd.ddd.ddd.ddd] querier_inerval[1-1800] cos[0-7] - IGMP snooping admin mode\r\n"
        "m 1402 port1[0-15] type1[0=client,1=router] port2 type2 ... - IGMP snooping: add client interfaces\r\n"
        "m 1403 port1[0-15] port2 ...   - IGMP snooping: remove interfaces\r\n"
        "m 1405 vlan1[2-4093] xlate1[2-4093] vlan2 xlate2 ... - IGMP snooping: add mclient vlans and their translation values\r\n"
        "m 1406 vlan1[2-4093] vlan2 ... - IGMP snooping: remove mclient vlans\r\n"
        "m 1408 port[0-15] vlan1[2-4093] vlan2 ...     - IGMP snooping: add mrouter vlans for the given port\r\n"
        "m 1409 port[0-15] vlan1[2-4093] vlan2 ...     - IGMP snooping: remove mrouter vlans for the given port\r\n"
        "m 1411 vlan1[2-4093] ipaddr1[ddd.ddd.ddd.ddd] vlan2 ipaddr2 ... - IGMP snooping querier: Add vlan and its ip address\r\n"
//      "m 1412 vlan1[2-4093] vlan2 ... - IGMP snooping querier: remove vlans\r\n"*/
        "m 1420 evc_id[1-127] page_idx[0..] intf[0-Phy,1-Lag]/[intf#] svid[1-4095] cvid[1-4095/0]  - List active channels for a particular EVC and client\r\n"
        "m 1421 evc_id[1-127] page_idx[0..] ipchannel[ddd.ddd.ddd.ddd] - List clients watching a channel (ip) associated to this EVCid\r\n"
        "m 1430 flow_id[1-127] ipchannel[ddd.ddd.ddd.ddd] sourceAddr[ddd.ddd.ddd.ddd] - Add static MC channel\r\n"
        "m 1431 flow_id[1-127] ipchannel[ddd.ddd.ddd.ddd] sourceAddr[ddd.ddd.ddd.ddd] - Remove static MC channel\r\n"
        "m 1500 lag_index[0-17] - Get LAG configurations\r\n"
        "m 1501 lag_index[0-17] static_mode[0/1] load_balance[0..6] port_bmp[XXXXXh] - Create LAG\r\n"
        "m 1502 lag_index[0-17] - Destroy LAG\r\n"
        "m 1503 lag_index[0-17] - Get LAG status\r\n"
        "m 1504 port_index[0..17/-1] lacp_aggregation[0/1] lacp_activity[0/1] lacp_timeout[0=long,1=short] - Set LACP Admin State\r\n"
        "m 1505 port_index[0..17/-1] - Get LACP Admin State\r\n"
        "m 1510 port[0..17/-1] - Show LACP statistics for port <port>\n\r"
        "m 1511 port[0..17/-1] - Clear LACP statistics for port <port>\n\r"
        "m 1997 - Reset Multicast machine\r\n"
        "m 1998 - Reset alarms\r\n"
        "m 1999 - Reset defaults, except for lag InBand\r\n"
        "m 2000 [10-1000000] - Set MAC Learning table aging time\r\n"
        "m 2001 - Get MAC Learning aging time\r\n"
        "--- NEW COMMANDS FP6.3 ---------------------------------------------------------------------------------------------------------------\r\n"
        "m 1600 EVC#[0-64] - Read EVC config\r\n"
        "m 1601 EVC#[0-64] Type[0:P2MP/1:P2P/2:Q] Stacked[0/1] MacLearn[0/1] Mask[0x010:CPUtrap;0x100:DHCP] MCFlood[0-All;1-Unknown;2-None]\r\n"
        "       type[0-Phy;1-Lag]/intf#/mef[0-Root;1-Leaf]/VLAN/iVlan ... - Create EVC\r\n"
        "m 1602 EVC#[0-64] - Delete EVC\r\n"
        "m 1603 EVC#[0-64] type[0-Phy;1-Lag]/intf#/mef[0-Root;1-Leaf]/VLAN/iVlan ... - Add ports to EVC\r\n"
        "m 1604 EVC#[0-64] type[0-Phy;1-Lag]/intf# ... - Remove ports from EVC\r\n"
        "m 1605 EVC#[0-64] type[0-Phy;1-Lag] intf# Out.VLAN Inn.VLAN - Add P2P bridge on Stacked EVCs between the root and a leaf intf\r\n"
        "m 1606 EVC#[0-64] type[0-Phy;1-Lag] intf# Inn.VLAN - Deletes P2P bridge on Stacked EVCs between the root and a leaf intf\r\n"
        "m 1607 EVC#[0-64] type[0-Phy;1-Lag] intf# Out.VLAN Inn.VLAN CVlan flags[01h:DHCP;02h:IGMP;04h:PPPoE] MaxMACs - Add a GEM flow to an EVC\r\n"
        "m 1608 EVC#[0-64] type[0-Phy;1-Lag] intf# Out.VLAN - Delete a GEM flow from an EVC\r\n"
        "m 1610 - Reads Network Connectivity (inBand) configuration\r\n"
        "m 1611 <intf_type[0:phy 1:lag]> <intf#> <ipaddr> <netmask> <gateway> <managememt_vlan> - Sets Network Connectivity (inBand) configuration\r\n"
        "m 1620 slot=[0-17] evc=[1-64] intf=<[0-Phy;1-Lag]/intf#> svid=[1-4095] cvid=[1-4095] - Get Profile data of a specific Bandwidth Policer\r\n"
        "m 1621 slot=[0-17] evc=[1-64] intf=<[0-Phy;1-Lag]/intf#> svid=[1-4095] cvid=[1-4095] cir=[mbps] eir=[mbps] cbs=[bytes] ebs=[bytes] - Create/reconfig bandwidth Policer\r\n"
        "m 1622 slot=[0-17] evc=[1-64] intf=<[0-Phy;1-Lag]/intf#> svid=[1-4095] cvid=[1-4095] - Delete bandwidth Policer\r\n"
        "m 1624 slot=[0-17] bc=[BC rate (bps)] mc=[MC rate (bps)] uc=[unknown UC rate (bps)] - Storm control configuration\r\n"
        "m 1625 slot=[0-17] - Storm control reset\r\n"
        "m 1626 slot=[0-17] - Storm control clear\r\n"
        "m 1630 slot=[0-17] evc=[1-64] intf=<[0-Phy;1-Lag]/intf#> svid=[1-4095] cvid=[1-4095] channel=[ipv4-xxx.xxx.xxx.xxx] - Show absolute evc statistics\n\r"
        "m 1632 slot=[0-17] evc=[1-64] intf=<[0-Phy;1-Lag]/intf#> svid=[1-4095] cvid=[1-4095] channel=[ipv4-xxx.xxx.xxx.xxx] - Add evc statistics measurement\n\r"
        "m 1633 slot=[0-17] evc=[1-64] intf=<[0-Phy;1-Lag]/intf#> svid=[1-4095] cvid=[1-4095] channel=[ipv4-xxx.xxx.xxx.xxx] - Remove evc statistics measurement\n\r"
        "--- IP Source Guard ------------------------------------------------------------------------------------------------------------------\n\r"
        "m 1700 type[0-Phy;1-Lag]/intf# enable[0/1] - Enable/Disable IP Source Guard on Ptin Port\n\r"
        "m 1701 idType[1-eEVCId; 2-rootVLAN] iD[1-131071] type[0-Phy;1-Lag]/intf# macAddr[xxxxxxxxxxxxh] ipAddr[ddd.ddd.ddd.ddd] removeOrAdd[0/1] - Add/Remove IP Source Guard Entry on Ptin Port of Extended EVC Id\n\r"
        "--- Routing --------------------------------------------------------------------------------------------------------------------------\n\r"
        "m 1810 routingIntf[2-Rtr]/[intf#] evc_id[1-127] ipaddr[ddd.ddd.ddd.ddd] subnetMask[ddd.ddd.ddd.ddd] mtu[xxx]- Create new routing interface\r\n"
        "m 1811 intf[2-Rtr]/[intf#] - Remove routing interface\r\n"
        "m 1820 intf[2-Rtr]/[intf#] - Get ARP table\r\n"
        "m 1821 intf[2-Rtr]/[intf#] ipaddr[ddd.ddd.ddd.ddd] - Purge ARP entry\r\n"
        "m 1830 intf[2-Rtr]/[intf#] - Get route table\r\n"
        "m 1831 ipaddr[ddd.ddd.ddd.ddd] subnetMask[ddd.ddd.ddd.ddd] gateway[ddd.ddd.ddd.ddd] pref[1-255] isNullRoute[0/1] - Add static route\r\n"
        "m 1832 ipaddr[ddd.ddd.ddd.ddd] subnetMask[ddd.ddd.ddd.ddd] gateway[ddd.ddd.ddd.ddd] isNullRoute[0/1] - Delete static route\r\n"
        "m 1840 index[0-15] ipaddr[ddd.ddd.ddd.ddd] count[1-15] size[0-65507] interval[1-60] - Create ping session\r\n"
        "m 1841 index[0-15] - Query ping session\r\n"
        "m 1842 index[0-15] - Free ping session\r\n"
        "m 1850 index[0-15] ipaddr[ddd.ddd.ddd.ddd] probes[1-10] size[0-65507] interval[1-60] dontFrag[0/1] port[0-65535] maxTtl[1-255] initTtl[1-255] maxFails[0-255] - Create traceroute session\r\n"
        "m 1851 index[0-15] - Query traceroute session\r\n"
        "m 1852 index[0-15] - Get traceroute session hops\r\n"
        "m 1853 index[0-15] - Free traceroute session\r\n"
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

    printf("Checking if Fastpath is alive...\n\r");
    do {
      // Send command
      valued = send_data (canal_buga, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta);
      ret = *((int *)resposta.info);
      if ( valued ) {
        printf("Without answer from fastpath... probably it is still starting\n\r");
      }
      else if (resposta.flags != (FLAG_RESPOSTA | FLAG_ACK)) {
        printf("Invalid answer... Request not Aknowledged\n\r");
        sleep(period);
      }
      else if (resposta.infoDim == sizeof(int) && ret == 1) {
        printf("Fastpath replied... Application is still loading!\n\r");
        sleep(period);
      }
      else
        break;
    } while ((--nretries) > 0);

    close_ipc(canal_buga);

    if (nretries > 0) {
      if (ret == 0) {
        printf("Fastpath replied... Application is up!\n\r");
        exit(ret);
      }
      else if (ret == 2) {
        printf("Fastpath replied... Application has CRASHED!\n\r");
        exit(ret);
      }
      else {
        printf("Fastpath replied... return code is unknown: %d\n\r", (int)ret);
        exit(ret);
      }
    }
    else if (resposta.infoDim == sizeof(int) && ret == 1)
      exit(ret);

    printf("Fastpath did not reply... Timeout!\n\r");
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

          swcfg->SlotId = (uint8)-1;
          swcfg->mask = 0x01;

          /* integer value to be sent */
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          swcfg->aging_time = (uint32) valued;

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

        swcfg->SlotId = (uint8)-1;
        swcfg->mask = 0xff;

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
          ptr->slot_id = (uint8)-1;

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
          *ptr = 0;

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
          ptr->slot_id = -1;

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

          // Validate number of arguments
          if (argc<3+0)  {
            help_oltBuga();
            exit(0);
          }

          /* If argument is provided, redirect logger to file */
          /* Otherwise, will be stdout */
          if (argc>3)
          {
            /* File direction */
            len = strlen(argv[3+0]);

            /* Validate length */
            if (len>100) {
              printf("File name too long\n");
            }

            /* $ character referes to default filename */
            if (len==1 && argv[3+0][0]=='$')
            {
              comando.info[0] = '\0';
              printf("Going to use default filename...\n");
            }
            /* Otherwise, use specified filename */
            else
            {
              strncpy((char *) &comando.info[0], argv[3+0], 101 );
              comando.info[100] = '\0';
              printf("Going to use \"%s\" filename...\n",comando.info);
            }

            /* Consider also the null character */
            comando.infoDim = len+1;
          }
          else
          {
            /* Stdout direction */
            comando.info[0] = '\0';
            comando.infoDim = 0;
          }

          comando.msgId = CCMSG_APP_LOGGER_OUTPUT;
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
          resources->SlotId = (uint8)-1;

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
            ptr[i].SlotId = (uint8)-1;

            // Port
            if (StrToLongLong(argv[3+1+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr[i].intf.intf_type = 0;
            ptr[i].intf.intf_id   = (uint8)valued;

            // enable
            ptr[i].enable = enable;
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
            ptr[i].SlotId = (uint8)-1;

            // Port
            if (StrToLongLong(argv[3+i],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr[i].intf.intf_type = 0;
            ptr[i].intf.intf_id   = (uint8)valued;
          }

          comando.msgId = CCMSG_ETH_PCS_PRBS_STATUS;
          comando.infoDim = sizeof(msg_ptin_pcs_prbs)*(argc-3);
        }
        break;

      // Slot mode configuration
      case 1005:
        {
          /* Get slot configuration */
          *((uint32 *) &comando.info[0]) = 0;

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

          ptr->SlotId = (uint8)-1;

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
            ptr->slot_list[i].slot_index  = index;
            ptr->slot_list[i].slot_mode   = mode;
            ptr->slot_list[i].slot_config = 1;
          }

          comando.msgId = (msg==1009) ? CCMSG_SLOT_MAP_MODE_APPLY : CCMSG_SLOT_MAP_MODE_VALIDATE;
          comando.infoDim = sizeof(msg_slotModeCfg_t);
        }
        break;

      // Switch Phy port configuration
      case 1010:
        {
          msg_HWEthPhyConf_t *ptr;

          // Validate number of arguments
          if (argc!=3+7)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_HWEthPhyConf_t *) &(comando.info[0]);

          // Slot id
          ptr->SlotId = (uint8)-1;

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->Port = (uint8) valued;
          // enable
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->PortEnable = (uint8) valued;
          // speed
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->Speed = (uint8) valued;
          // full_duplex
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->Duplex = (uint8) valued;
          // frame_max length
          if (StrToLongLong(argv[3+4],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->MaxFrame = (uint16) valued;
          // loopback
          if (StrToLongLong(argv[3+5],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->LoopBack = (uint8) valued;
          // MAC learning
          if (StrToLongLong(argv[3+6],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->MacLearning = (uint8) valued;

          ptr->Mask = 0xFFFF;
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
          ptr->slot_id = (uint8)-1;

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->generic_id = (uint8) valued;

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
          ptr->slot_id = (uint8)-1;

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->generic_id = (uint8) valued;

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
          ptr->slot_id = (uint8)-1;
          ptr->generic_id = 0;
          ptr->type = 3;
          ptr->param = 0;

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
          ptr->slotId = (uint8) valued;

          // Port
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->port = (uint8) valued;

          // Command
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->protCmd = (uint8) valued;
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
        ptr->SlotId = (uint8)-1;
        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = type;
        ptr->intf.intf_id   = intf;

        ptr->Mask = 0xFFFF;
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

        ptr->SlotId                       = (uint8)-1;
        ptr->intf.intf_type               = (L7_uint8)-1;
        ptr->intf.intf_id                 = (L7_uint8)-1;
        ptr->Mask                         = 0x0000;
        ptr->defVid                       = 1;
        ptr->defPrio                      = 0;
        ptr->acceptable_frame_types       = 0;
        ptr->ingress_filter               = 0;
        ptr->restricted_vlan_reg          = 0;
        ptr->vlan_aware                   = 1;
        ptr->type                         = 2;
        ptr->doubletag                    = 1;
        ptr->outer_tpid                   = 0x8100;
        ptr->inner_tpid                   = 0x8100;
        ptr->egress_type                  = 0;
        ptr->macLearn_enable              = 0;
        ptr->macLearn_stationMove_enable  = 0;
        ptr->macLearn_stationMove_prio    = 0;

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
          else if (strcmp(param,"intf")==0 || strcmp(param,"port")==0)
          {
            if (sscanf(value,"%d/%d",&type,&intf)!=2)
            {
              printf("Invalid port value\r\n");
              exit(0);
            }
            ptr->intf.intf_type = type;
            ptr->intf.intf_id   = intf;
          }
          else if (strcmp(param,"defvid")==0 || strcmp(param,"vid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid defvid value\r\n");
              exit(0);
            }
            ptr->defVid = (uint16) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_DEFVID;
          }
          else if (strcmp(param,"defprio")==0 || strcmp(param,"prio")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid defprio value\r\n");
              exit(0);
            }
            ptr->defPrio = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_DEFPRIO;
          }
          else if (strncmp(param,"aft",3)==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid aftypes value\r\n");
              exit(0);
            }
            ptr->acceptable_frame_types = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_ACCEPTABLE_FRAME_TYPES;
          }
          else if (strncmp(param,"if",2)==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid ifilter value\r\n");
              exit(0);
            }
            ptr->ingress_filter = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_INGRESS_FILTER;
          }
          else if (strncmp(param,"rvlan",5)==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid rvlanreg value\r\n");
              exit(0);
            }
            ptr->restricted_vlan_reg = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_RESTRICTED_VLAN_REG;
          }
          else if (strncmp(param,"vlana",5)==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid vlanaware value\r\n");
              exit(0);
            }
            ptr->vlan_aware = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_VLAN_AWARE;
          }
          else if (strcmp(param,"type")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid type value\r\n");
              exit(0);
            }
            ptr->type = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_TYPE;
          }
          else if (strcmp(param,"dtag")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid dtag value\r\n");
              exit(0);
            }
            ptr->doubletag = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_DOUBLETAG;
          }
          else if (strcmp(param,"otpid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid otpid value\r\n");
              exit(0);
            }
            ptr->outer_tpid = (uint16) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_OUTER_TPID;
          }
          else if (strcmp(param,"itpid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid itpid value\r\n");
              exit(0);
            }
            ptr->inner_tpid = (uint16) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_INNER_TPID;
          }
          else if (strcmp(param,"etype")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid egress_type value\r\n");
              exit(0);
            }
            ptr->egress_type = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_EGRESS_TYPE;
          }
          else if (strcmp(param,"mlen")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid mlen value\r\n");
              exit(0);
            }
            ptr->macLearn_enable = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_MACLEARN_ENABLE;
          }
          else if (strcmp(param,"mlsmen")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid mlsmen value\r\n");
              exit(0);
            }
            ptr->macLearn_stationMove_enable = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE;
          }
          else if (strcmp(param,"mlsmprio")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid mlsmprio value\r\n");
              exit(0);
            }
            ptr->macLearn_stationMove_prio = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO;
          }
          else if (strcmp(param,"mlsmsp")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid mlsmsp value\r\n");
              exit(0);
            }
            ptr->macLearn_stationMove_samePrio = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO;
          }
          else if (strcmp(param,"trust")==0 || strcmp(param,"trusted")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid trust value\r\n");
              exit(0);
            }
            ptr->protocol_trusted = (uint8) valued;
            ptr->Mask |= MSG_HWPORTEXT_MASK_PROTTRUSTED_INTF;
          }
          else
          {
            printf("Invalid param\r\n");
            exit(0);
          }
        }

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
        ptr->SlotId = (uint8)-1;
        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = type;
        ptr->intf.intf_id   = intf;

        ptr->Mask = 0x0001;

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
        ptr->SlotId = (uint8)-1;
        // port
        if (sscanf(argv[3+0],"%d/%d",&type,&intf)!=2)
        {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = type;
        ptr->intf.intf_id   = intf;

        // MAC address
        if (StrToLongLong(argv[3+1],&valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        memcpy(ptr->macAddr,&(((uint8 *) &valued)[2]),sizeof(uint8)*6);

        ptr->Mask = 0x0001;

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
          ptr->slot_id = (uint8)-1;

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->generic_id = (uint8) valued;

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

          // Slot id
          ptr->SlotId = (uint8)-1;

          // port
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->Port = (uint8) valued;

          comando.msgId = CCMSG_ETH_PHY_COUNTERS_CLEAR;
          comando.infoDim = sizeof(msg_HWEthRFC2819_PortStatistics_t);
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
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          memcpy(ptr->entry[0].addr,&(((uint8 *) &valued)[2]),sizeof(uint8)*6);

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
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          memcpy(ptr->entry[0].addr,&(((uint8 *) &valued)[2]),sizeof(uint8)*6);

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

      // DHCP Bind table reading
      case 1240:
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

          // page
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->page = valued;

          comando.msgId = CCMSG_ETH_DHCP_BIND_TABLE_GET;
          comando.infoDim = sizeof(msg_DHCPv4v6_bind_table_t);
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
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          memcpy(ptr->bind_table[0].macAddr,&(((uint8 *) &valued)[2]),sizeof(uint8)*6);

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
          msg_MCActiveChannelClients_t *ptr;

          // Validate number of arguments
          if (argc<3+1)  {
            help_oltBuga();
            exit(0);
          }

          // Pointer to data array
          ptr = (msg_MCActiveChannelClients_t *) &(comando.info[0]);
          memset(ptr,0x00,sizeof(msg_MCActiveChannelClients_t));

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

          comando.msgId = CCMSG_ETH_IGMP_CLIENT_GROUPS_GET;
          comando.infoDim = sizeof(msg_MCActiveChannelClients_t);
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
          ptr->SlotId = (uint8)-1;

          // LAG index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = (uint8) valued;

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

          // Slot id
          ptr->SlotId = (uint8)-1;

          // LAG index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = (uint8) valued;

          // Static mode
          if (StrToLongLong(argv[3+1],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->static_enable = (uint64) (valued & 1);

          // Load Balance mode
          if (StrToLongLong(argv[3+2],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->loadBalance_mode = (uint64) (valued & 0xFF);

          // Member ports bitmap
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->members_pbmp = (uint32) valued;
          ptr->members_pbmp2= (uint32) (valued>>32);

          ptr->admin = 1;
          ptr->stp_enable = 0;

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

          // Slot id
          ptr->SlotId = (uint8)-1;

          // LAG index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = (uint8) valued;

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
          ptr->SlotId = (uint8)-1;

          // LAG index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = (uint8) valued;

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

          // Slot id
          ptr->SlotId = (uint8)-1;

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
              ptr[i].id               = i;
              ptr[i].state_aggregation= state_aggregation;
              ptr[i].lacp_activity    = lacp_activity;
              ptr[i].lacp_timeout     = lacp_timeout;
            }
            comando.infoDim = sizeof(msg_LACPAdminState_t)*PTIN_SYSTEM_N_PORTS;
          }
          else if (port>=0 && port<PTIN_SYSTEM_N_PORTS) {
            ptr->id               = port;
            ptr->state_aggregation= state_aggregation;
            ptr->lacp_activity    = lacp_activity;
            ptr->lacp_timeout     = lacp_timeout;
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
          ptr->SlotId = (uint8)-1;

          // Port index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = (uint8) ((int) valued);

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
          ptr->SlotId = (uint8)-1;

          // Port index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = (uint8) ((int) valued);

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

          // Slot id
          ptr->SlotId = (uint8)-1;

          // Port index
          if (StrToLongLong(argv[3+0],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = (uint8) ((int) valued);

          ptr->LACPdus_rx = 0;
          ptr->LACPdus_tx = 0;

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

          // Slot id
          ptr->SlotId = (uint8)-1;

          // evc index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = valued;
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
          ptr->SlotId = (uint8)-1;

          // evc index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = valued;

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

          // MC Flood type
          if (StrToLongLong(argv[3+5], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->mc_flood = valued;

          ptr->n_intf   = argc - (3+6);

          // Interfaces...
          unsigned int intf, type, mef, vid, ivid;
          for (i=3+6; i<argc; i++) {
            printf("argv[%u]=%s  **  ", i, argv[i]);
            sscanf(argv[i], "%d/%d/%d/%d/%d", &type, &intf, &mef, &vid, &ivid);
            printf("%d/%d/%d/%d/%d\n", type, intf, mef, vid, ivid);
            ptr->intf[i-(3+6)].intf_type = type;
            ptr->intf[i-(3+6)].intf_id   = intf;
            ptr->intf[i-(3+6)].mef_type  = mef;
            ptr->intf[i-(3+6)].vid       = vid;
            ptr->intf[i-(3+6)].inner_vid = ivid;
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
          ptr->SlotId = (uint8)-1;

          // evc index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->id = valued;
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
          ptr[i].slotId         = slotId;
          ptr[i].evcId          = evc_idx;
          ptr[i].intf.intf_type = type;
          ptr[i].intf.intf_id   = intf;
          ptr[i].intf.mef_type  = mef;
          ptr[i].intf.vid       = vid;
          ptr[i].intf.inner_vid = ivid;

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
          ptr[i].slotId         = slotId;
          ptr[i].evcId          = evc_idx;
          ptr[i].intf.intf_type = type;
          ptr[i].intf.intf_id   = intf;

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
          ptr->SlotId = (uint8)-1;

          // EVC index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evcId = valued;

          // Intf type
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = valued;

          // Intf#
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_id = valued;

          ptr->intf.mef_type = 1; // leaf

          // Outer VLAN
          if (StrToLongLong(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.vid = valued;

          // Inner VLAN
          if (StrToLongLong(argv[3+4], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->inn_vlan = valued;
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
          ptr->SlotId = (uint8)-1;

          // EVC index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evcId = valued;

          // Intf type
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = valued;

          // Intf#
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_id = valued;

          ptr->intf.vid = 0xFFFF;

          // Inner VLAN
          if (StrToLongLong(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->inn_vlan = valued;
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
          ptr->SlotId = (uint8)-1;

          // EVC index
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->evcId = valued;

          // Intf type
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_type = valued;

          // Intf#
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.intf_id = valued;

          // Outer VLAN
          if (StrToLongLong(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.outer_vid = valued;

          // Inner VLAN
          if (StrToLongLong(argv[3+4], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf.inner_vid = valued;

          // NNI Client VLAN
          if (argc >= 3+6)
          {
            if (StrToLongLong(argv[3+5], &valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->nni_cvlan = valued;
          }

          // Flags
          if (argc >= 3+7)
          {
            if (StrToLongLong(argv[3+6], &valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->flags = (uint32) valued<<8;
          }

          // Maximum number of MAC addresses
          if (argc >= 3+8)
          {
            if (StrToLongLong(argv[3+7], &valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->macLearnMax = (uint8) valued;
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
        ptr->SlotId = (uint8)-1;

        // EVC index
        if (StrToLongLong(argv[3+0], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->evcId = valued;

        // Intf type
        if (StrToLongLong(argv[3+1], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_type = valued;

        // Intf#
        if (StrToLongLong(argv[3+2], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.intf_id = valued;

        // Outer VLAN
        if (StrToLongLong(argv[3+3], &valued)<0)  {
          help_oltBuga();
          exit(0);
        }
        ptr->intf.outer_vid = valued;
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
          ptr->SlotId = (uint8)-1;

          ptr->mask = 0xFFFFFFFF;

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

          ptr->mask = 0xFFFFFFFF;

          // Slot id
          ptr->SlotId = (uint8)-1;

          // Intf type
          if (StrToLongLong(argv[3+0], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf[0].intf_type = valued;

          // Intf #
          if (StrToLongLong(argv[3+1], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->intf[0].intf_id = valued;

          ptr->n_intf = 1;

          // IP Addr
          if (StrToLongLong(argv[3+2], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->ipaddr = valued;

          // NetMask
          if (StrToLongLong(argv[3+3], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->netmask = valued;

          // Gateway
          if (StrToLongLong(argv[3+4], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->gateway = valued;

          // VLAN
          if (StrToLongLong(argv[3+5], &valued)<0)  {
            help_oltBuga();
            exit(0);
          }
          ptr->mgmtVlanId = valued;
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

        ptr->SlotId = (uint8)-1;
        ptr->evcId = (uint32)-1;
        ptr->profile.cir = (uint64)-1;
        ptr->profile.eir = (uint64)-1;
        ptr->profile.cbs = (uint64)-1;
        ptr->profile.ebs = (uint64)-1;

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
          else if (strcmp(param,"evc")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid evc value\r\n");
              exit(0);
            }
            ptr->evcId = (uint32) valued;
          }
          else if (strcmp(param,"intf")==0 || strcmp(param,"intfsrc")==0)
          {
            if (sscanf(value,"%d/%d",&type,&intf)!=2)
            {
              printf("Invalid intf/intfsrc value\r\n");
              exit(0);
            }
            ptr->intf_src.intf_type = (uint8) type;
            ptr->intf_src.intf_id   = (uint8) intf;
            ptr->mask |= MSG_HWETH_BWPROFILE_MASK_INTF_SRC;
          }
          else if (strcmp(param,"intfdst")==0)
          {
            if (sscanf(value,"%d/%d",&type,&intf)!=2)
            {
              printf("Invalid intfdst value\r\n");
              exit(0);
            }
            ptr->intf_dst.intf_type = (uint8) type;
            ptr->intf_dst.intf_id   = (uint8) intf;
            ptr->mask |= MSG_HWETH_BWPROFILE_MASK_INTF_DST;
          }
          else if (strcmp(param,"svid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid svid value\r\n");
              exit(0);
            }
            ptr->service_vlan = (uint16) valued;
            ptr->mask |= MSG_HWETH_BWPROFILE_MASK_SVLAN;
          }
          else if (strcmp(param,"cvid")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid cvid value\r\n");
              exit(0);
            }
            ptr->client_vlan = (uint16) valued;
            ptr->mask |= MSG_HWETH_BWPROFILE_MASK_CVLAN;
          }
          else if (strcmp(param,"cir")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid cir value\r\n");
              exit(0);
            }
            ptr->profile.cir = (uint64) valued*1000000;
            ptr->mask |= MSG_HWETH_BWPROFILE_MASK_PROFILE;
          }
          else if (strcmp(param,"eir")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid eir value\r\n");
              exit(0);
            }
            ptr->profile.eir = (uint64) valued*1000000;
            ptr->mask |= MSG_HWETH_BWPROFILE_MASK_PROFILE;
          }
          else if (strcmp(param,"cbs")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid cbs value\r\n");
              exit(0);
            }
            ptr->profile.cbs = (uint64) valued;
            ptr->mask |= MSG_HWETH_BWPROFILE_MASK_PROFILE;
          }
          else if (strcmp(param,"ebs")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid ebs value\r\n");
              exit(0);
            }
            ptr->profile.ebs = (uint64) valued;
            ptr->mask |= MSG_HWETH_BWPROFILE_MASK_PROFILE;
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

        ptr->SlotId = (uint8)-1;
        ptr->id = 0;

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
          else if (strcmp(param,"bc")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid Broadcast rate value\r\n");
              exit(0);
            }
            ptr->bcast_rate = (uint32) valued;
            ptr->mask |= MSG_STORMCONTROL_MASK_BCAST;
          }
          else if (strcmp(param,"mc")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid Multicast rate value\r\n");
              exit(0);
            }
            ptr->mcast_rate = (uint32) valued;
            ptr->mask |= MSG_STORMCONTROL_MASK_MCAST;
          }
          else if (strcmp(param,"uc")==0)
          {
            if (StrToLongLong(value,&valued)<0)
            {
              printf("Invalid Unknown Unicast rate value\r\n");
              exit(0);
            }
            ptr->ucast_unknown_rate = (uint32) valued;
            ptr->mask |= MSG_STORMCONTROL_MASK_UCUNK;
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
          ptr->SlotId= (uint8)-1;
          ptr->evc_id= (uint32)-1;

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
            else if (strcmp(param,"evc")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid evc value\r\n");
                exit(0);
              }
              ptr->evc_id = (uint32) valued;
            }
            else if (strcmp(param,"intf")==0)
            {
              if (sscanf(value,"%d/%d",&type,&intf)!=2)
              {
                printf("Invalid intf/intfsrc value\r\n");
                exit(0);
              }
              ptr->intf.intf_type = (uint8) type;
              ptr->intf.intf_id   = (uint8) intf;
              ptr->mask |= MSG_EVC_COUNTERS_MASK_INTF;
            }
            else if (strcmp(param,"svid")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid svid value\r\n");
                exit(0);
              }
              ptr->service_vlan = (uint16) valued;
              ptr->mask |= MSG_EVC_COUNTERS_MASK_SVLAN;
            }
            else if (strcmp(param,"cvid")==0)
            {
              if (StrToLongLong(value,&valued)<0)
              {
                printf("Invalid cvid value\r\n");
                exit(0);
              }
              ptr->client_vlan = (uint16) valued;
              ptr->mask |= MSG_EVC_COUNTERS_MASK_CVLAN;
            }
            else if (strcmp(param,"channel")==0)
            {
              if (convert_ipaddr2uint64(value,&valued)<0)
              {
                printf("Invalid channel IP value (xxx.xxx.xxx.xxx format)\r\n");
                exit(0);
              }
              ptr->channel_ip = (uint32) valued;
              ptr->mask |= MSG_EVC_COUNTERS_MASK_CHANNEL;
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
          if (StrToLongLong(argv[3+3],&valued)<0)  {
            help_oltBuga();
            exit(0);
          }          
          memcpy(ptr->macAddr,&(((uint8 *) &valued)[2]),sizeof(uint8)*6);

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
      
      case 1998:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Alarms reset done\n\r");
        else
          printf(" Switch: Alarms reset failed - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1999:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Defaults restored\n\r");
        else
          printf(" Switch: Reset defaults failed - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 2000:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: L2 Aging time defined\n\r");
        else
          printf(" Switch: Error setting L2 aging time - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 2001:
      {
        msg_switch_config_t *po=(msg_switch_config_t *) &resposta.info[0];

        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
          printf(" Slot %u: Age time = %lu\n\r",po->SlotId,po->aging_time);
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
          printf(" Error redirecting Stdout - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1001:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Logger redirected successfully\n\r");
        else
          printf(" Error redirecting logger - error %08x\n\r", *(unsigned int*)resposta.info);
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

            printf(" FFP available rules (Slot=%u):\r\n",po->SlotId);
            for (j=0; j<PTIN_POLICY_MAX_STAGES; j++) {
              printf(" Stage=%s\r\n",((j==0) ? "Lookup" : ((j==1) ? "Ingress" : ((j==2) ? "Egress" : "Unknown") ) ) );
              for (i=0; i<PTIN_POLICY_MAX_GROUPS; i++) {
                if (po->cap[i][j].inUse)
                {
                  printf("   Idx %u: Group %d (slice_width=%u)\r\n",i,po->cap[i][j].group_id,po->cap[i][j].total.slice_width);
                  printf("     rules    = %03u / %03u (%u rules in use)\r\n", po->cap[i][j].free.rules, po->cap[i][j].total.rules, po->cap[i][j].count.rules);
                  printf("     counters = %03u / %03u\r\n", po->cap[i][j].free.counters, po->cap[i][j].total.counters);
                  printf("     meters   = %03u / %03u\r\n", po->cap[i][j].free.meters  , po->cap[i][j].total.meters  );
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

            printf(" Slot map configuration (Slot=%u):\r\n",po->SlotId);
            for (i=0; i<MSG_SLOTMODECFG_NSLOTS; i++)
            {
              if (po->slot_list[i].slot_config)
              {
                printf(" Index=%u:   Slot=%2u - Mode=%u\r\n", i, po->slot_list[i].slot_index, po->slot_list[i].slot_mode);
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
        printf(" PRBS enable not executed - error %08x\n\r", *(unsigned int*)resposta.info);
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

          printf(" PRBS status of SlotId=%u\r\n",po->SlotId);
          for (i=0; i<n; i++)
          {
            printf(" Port %u/%-2u:  Lock=%u Errors=%lu\r\n",
                   po[i].intf.intf_type,po[i].intf.intf_id,
                   po[i].rxStatus.lock,po[i].rxStatus.rxErrors);
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
          printf(" Slot map not valid - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1009:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Slot map configuration applied successfully\n\r");
        else
          printf(" Slot map configuration not applied - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1010:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch port configuration executed successfully\n\r");
        else
          printf(" Switch port configuration not executed - error %08x\n\r", *(unsigned int*)resposta.info);
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
              printf("Slot %u, Port %u configuration:\n\r",po[index].SlotId,po[index].Port);
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
                     po[index].Speed,
                     po[index].Media,
                     po[index].Duplex,
                     po[index].LoopBack,
                     po[index].FlowControl,
                     po[index].PortEnable,
                     po[index].MaxFrame,
                     po[index].VlanAwareness,
                     po[index].MacLearning,
                     po[index].AutoMDI );
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
              printf("Slot %u, Port %u eth state:\n\r",po[index].SlotId,po[index].Port);
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
                     po[index].Speed,
                     po[index].Duplex,
                     po[index].Collisions,
                     po[index].RxActivity,
                     po[index].TxActivity,
                     po[index].LinkUp,
                     po[index].AutoNegComplete,
                     po[index].TxFault,
                     po[index].RemoteFault,
                     po[index].LOS,
                     po[index].Media,
                     po[index].MTU_mismatch,
                     po[index].Supported_MaxFrame );
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
        printf(" Linkscan not executed - error %08x\n\r", *(unsigned int*)resposta.info);
      break;

    case 1014:
      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        printf(" Protection command applied successfully\n\r");
      else
        printf(" Protection command not executed - error %08x\n\r", *(unsigned int*)resposta.info);
      break;

    case 1015:
      {
        uint8 index, n_index;
        msg_HWPortExt_t *po=(msg_HWPortExt_t *) &resposta.info[0];

        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
          printf("Switch: port(s) read successfully\n\r");
          n_index = resposta.infoDim / sizeof(msg_HWPortExt_t);
          if ( n_index>PTIN_SYSTEM_N_PORTS )  n_index=PTIN_SYSTEM_N_PORTS;
          for ( index=0; index<n_index; index++ ) {
            printf("Port Type of Slot %u, Port %u/%u:\n\r",po[index].SlotId,po[index].intf.intf_type,po[index].intf.intf_id);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_DEFVID)
              printf("\tDefault Vid   = %u\n\r",po[index].defVid);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_DEFPRIO)
              printf("\tDefault Prio  = %u\n\r",po[index].defPrio);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_ACCEPTABLE_FRAME_TYPES)
              printf("\tAcceptable Frame Types = %u\n\r",po[index].acceptable_frame_types);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_INGRESS_FILTER)
              printf("\tIngress Filter         = %u\n\r",po[index].ingress_filter);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_RESTRICTED_VLAN_REG)
              printf("\tRestricted Vlan Reg    = %u\n\r",po[index].restricted_vlan_reg);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_VLAN_AWARE)
              printf("\tVlan Aware             = %u\n\r",po[index].vlan_aware);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_TYPE)
              printf("\tType                   = %u\n\r",po[index].type);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_DOUBLETAG)
              printf("\tDouble Tag             = %u\n\r",po[index].doubletag);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_OUTER_TPID)
              printf("\tOuter TPID             = %u\n\r",po[index].outer_tpid);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_INNER_TPID)
              printf("\tInner TPID             = %u\n\r",po[index].inner_tpid);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_EGRESS_TYPE)
              printf("\tEgress Type            = %u\n\r", po[index].egress_type);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_MACLEARN_ENABLE)
              printf("\tMAC Learn Enable                = %u\n\r",po[index].macLearn_enable);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE)
              printf("\tMAC Learn Station Move Enable   = %u\n\r",po[index].macLearn_stationMove_enable);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO)
              printf("\tMAC Learn Station Move Priority = %u\n\r",po[index].macLearn_stationMove_prio);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO)
              printf("\tMAC Learn Station Move within same priority intfs = %u\n\r",po[index].macLearn_stationMove_samePrio);
            if (po[index].Mask & MSG_HWPORTEXT_MASK_PROTTRUSTED_INTF)
              printf("\tTrusted interface      = %u\n\r",po[index].protocol_trusted);
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
          printf(" Switch port configuration not executed - error %08x\n\r", *(unsigned int*)resposta.info);
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
              printf("MAC of Slot %u, Port %u/%u:\n\r",po[index].SlotId,po[index].intf.intf_type,po[index].intf.intf_id);
              if (po[index].Mask & 0x0001)
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
          printf(" Switch MAC not attributed - error %08x\n\r", *(unsigned int*)resposta.info);
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
                    po->SlotId,po->Port,
                    po->Rx.etherStatsDropEvents          , po->Tx.etherStatsDropEvents          ,
                    po->Rx.etherStatsOctets              , po->Tx.etherStatsOctets              ,
                    po->Rx.etherStatsPkts                , po->Tx.etherStatsPkts                ,
                    po->Rx.etherStatsBroadcastPkts       , po->Tx.etherStatsBroadcastPkts       ,
                    po->Rx.etherStatsMulticastPkts       , po->Tx.etherStatsMulticastPkts       ,
                    po->Rx.etherStatsCRCAlignErrors      , po->Tx.etherStatsCRCAlignErrors      ,
                    po->Rx.etherStatsUndersizePkts       , po->Tx.etherStatsCollisions          ,
                    po->Rx.etherStatsOversizePkts        , po->Tx.etherStatsOversizePkts        ,
                    po->Rx.etherStatsFragments           , po->Tx.etherStatsFragments           ,
                    po->Rx.etherStatsJabbers             , po->Tx.etherStatsJabbers             ,
                    po->Rx.etherStatsPkts64Octets        , po->Tx.etherStatsPkts64Octets        ,
                    po->Rx.etherStatsPkts65to127Octets   , po->Tx.etherStatsPkts65to127Octets   ,
                    po->Rx.etherStatsPkts128to255Octets  , po->Tx.etherStatsPkts128to255Octets  ,
                    po->Rx.etherStatsPkts256to511Octets  , po->Tx.etherStatsPkts256to511Octets  ,
                    po->Rx.etherStatsPkts512to1023Octets , po->Tx.etherStatsPkts512to1023Octets ,
                    po->Rx.etherStatsPkts1024to1518Octets, po->Tx.etherStatsPkts1024to1518Octets,
                    po->Rx.etherStatsPkts1519toMaxOctets , po->Tx.etherStatsPkts1519toMaxOctets ,
                    po->Rx.Throughput                    , po->Tx.Throughput                    );
          }
          else
            printf(" Switch: Error reading port statistics\n\r");
        }
        break;

      case 1021:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Port statistics cleared\n\r");
        else
          printf(" Switch: Error clearing port statistics - error %08x\n\r", *(unsigned int*)resposta.info);
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
            printf("  Shaping rate  : %lu\r\n",ptr->shaping_rate);

            // Only proceed, if trust mode is valid
            if (ptr->trust_mode!=0 && ptr->trust_mode<=4) {

              // Priorities map
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("----------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
              printf("  Pkt. Priority ");
              for (j=0; j<8; j++) {
                printf(" | %8u",j);
              }
              printf(" |\r\n");
              printf("                 ");
              for (j=0; j<8; j++) {
                printf("|----------");
              }
              printf("|\r\n");
              printf("  ClassOfService");
              for (j=0; j<8; j++) {
                if (ptr->trust_mode==4) {
                  printf(" | 0x%08lX",ptr->pktprio.cos[j]);
                }
                else {
                  printf(" | %8lu",ptr->pktprio.cos[j]);
                }
              }
              printf(" |\r\n");
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("----------");
                if (j<7)  printf("-");
              }
              printf("\r\n");

              // CoS configurations
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("----------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
              printf("  CoS           ");
              for (j=0; j<8; j++) {
                printf(" | %8u",j);
              }
              printf(" |\r\n");
              printf("                 ");
              for (j=0; j<8; j++) {
                printf("|----------");
              }
              printf("|\r\n");
              printf("  Scheduler Type");
              for (j=0; j<8; j++) {
                printf(" | ");
                if (ptr->cos_config.cos[j].scheduler==0)       printf(" NotConf");
                else if (ptr->cos_config.cos[j].scheduler==1)  printf("  Strict");
                else if (ptr->cos_config.cos[j].scheduler==2)  printf("Weighted");
                else                                           printf(" Invalid");
              }
              printf(" |\r\n");
              printf("  Min. Bandwidth");
              for (j=0; j<8; j++) {
                printf(" | %8lu",ptr->cos_config.cos[j].min_bandwidth);
              }
              printf(" |\r\n");
              printf("  Max. Bandwidth");
              for (j=0; j<8; j++) {
                printf(" | %8lu",ptr->cos_config.cos[j].max_bandwidth);
              }
              printf(" |\r\n");
              printf("                  ");
              for (j=0; j<8; j++) {
                printf("----------");
                if (j<7)  printf("-");
              }
              printf("\r\n");
            } // Only proceed, if trust mode is valid
          } // Run all elements

          printf(" Switch: General QoS configuration read successfuly\n\r");
        }
        else
          printf(" Switch: Error reading general QoS configuration - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1031:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: General QoS configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting general QoS configuration - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1032:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Specific QoS configuration executed successfuly\n\r");
        else
          printf(" Switch: Error setting specific QoS configuration - error %08x\n\r", *(unsigned int*)resposta.info);
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
          printf(" Switch: Error reading MAC table - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1041:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MAC address added successfully\n\r");
        else
          printf(" Switch: MAC address not added - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1042:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MAC address removed successfully\n\r");
        else
          printf(" Switch: MAC address not removed - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1043:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MAC table flushed successfully\n\r");
        else
          printf(" Switch: Error flushing MAC table - error %08x\n\r", *(unsigned int*)resposta.info);
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
          printf(" Switch: DHCPop82 profile not read - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1221:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DHCPop82 profile added successfully\n\r");
        else
          printf(" Switch: DHCPop82 profile not added - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1222:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DHCPop82 profile removed successfully\n\r");
        else
          printf(" Switch: DHCPop82 profile not removed - error %08x\n\r", *(unsigned int*)resposta.info);
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
          printf(" Switch: Error reading DHCP Binding table - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1242:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DHCP bind entry removed successfully\n\r");
        else
          printf(" Switch: DHCP bind entry not removed - error %08x\n\r", *(unsigned int*)resposta.info);
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
          printf(" Switch: IGMP statistics not cleared - error %08x\n\r", *(unsigned int*)resposta.info);
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
            printf(" Switch: Error reading DHCP statistics - error %08x\n\r", *(unsigned int*)resposta.info);
          }
        }
        break;

      case 1322:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: DHCP statistics cleared successfully\n\r");
        else
          printf(" Switch: DHCP statistics not cleared - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1400:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP snooping/querier configured\n\r");
        else
          printf(" Switch: Error configuring IGMP snooping/querier - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1401:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP instance added\n\r");
        else
          printf(" Switch: Error adding IGMP instance - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1402:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP instance removed\n\r");
        else
          printf(" Switch: Error removing IGMP instance - error %08x\n\r", *(unsigned int*)resposta.info);
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
          printf(" Switch: Error adding IGMP association - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1405:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: IGMP association removed\n\r");
        else
          printf(" Switch: Error removing IGMP association - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1406:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MC Client successfully added\n\r");
        else
          printf(" Switch: Error adding MC Client - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1407:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: MC Client successfully removed\n\r");
        else
          printf(" Switch: Error removing MC Client - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

//    case 1400:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping/Querier configured\n\r");
//      else
//        printf(" Switch: IGMP Snooping/Querier NOT configured - error %08x\n\r", *(unsigned int*)resposta.info);
//      break;
//
//    case 1402:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: interfaces added\n\r");
//      else
//        printf(" Switch: IGMP Snooping: interfaces NOT added - error %08x\n\r", *(unsigned int*)resposta.info);
//      break;
//
//    case 1403:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: interfaces removed\n\r");
//      else
//        printf(" Switch: IGMP Snooping: interfaces NOT removed - error %08x\n\r", *(unsigned int*)resposta.info);
//      break;
//
//    case 1405:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: mclient vlans added\n\r");
//      else
//        printf(" Switch: IGMP Snooping: mclient vlans NOT added - error %08x\n\r", *(unsigned int*)resposta.info);
//      break;
//
//    case 1406:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: mclient vlans removed\n\r");
//      else
//        printf(" Switch: IGMP Snooping: mclient vlans NOT removed - error %08x\n\r", *(unsigned int*)resposta.info);
//      break;
//
//    case 1408:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: mrouter vlans added\n\r");
//      else
//        printf(" Switch: IGMP Snooping: mrouter vlans NOT added - error %08x\n\r", *(unsigned int*)resposta.info);
//      break;
//
//    case 1409:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping: mrouter vlans removed\n\r");
//      else
//        printf(" Switch: IGMP Snooping: mrouter vlans NOT removed - error %08x\n\r", *(unsigned int*)resposta.info);
//      break;
//
//    case 1411:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping Querier: vlans added to querier\n\r");
//      else
//        printf(" Switch: IGMP Snooping Querier: vlans NOT added to querier - error %08x\n\r", *(unsigned int*)resposta.info);
//      break;
//
//    case 1412:
//      if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
//        printf(" Switch: IGMP Snooping Querier: vlans removed to querier\n\r");
//      else
//        printf(" Switch: IGMP Snooping Querier: vlans NOT removed to querier - error %08x\n\r", *(unsigned int*)resposta.info);
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
          msg_MCActiveChannelClients_t *po=(msg_MCActiveChannelClients_t *) &resposta.info[0];
          uint16 i;

          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
            if (resposta.infoDim!=sizeof(msg_MCActiveChannelClients_t)) {
              printf(" Switch: Invalid structure size\r\n");
              break;
            }
            printf(" MC clients for Slot=%u, EVC=%lu and channel %03lu.%03lu.%03lu.%03lu (total=%u)\n\r",po->SlotId, po->evc_id,
                   (po->channelIp.s_addr>>24) & 0xFF, (po->channelIp.s_addr>>16) & 0xFF, (po->channelIp.s_addr>>8) & 0xFF, po->channelIp.s_addr & 0xFF,
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
          printf(" Switch: Static MC Channel NOT added - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1431:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Static MC Channel removed successfully\n\r");
        else
          printf(" Switch: Static MC Channel NOT removed - error %08x\n\r", *(unsigned int*)resposta.info);
        break;


      case 1810:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully created new routing interface\n\r");
        else
          printf(" Error %08x while creating new routing interface\n\r", *(unsigned int*)resposta.info);
        break;

      case 1811:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully removed routing interface\n\r");
        else
          printf(" Error %08x while removing routing interface\n\r", *(unsigned int*)resposta.info);
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
          printf(" ARP Table: NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1821:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully removed ARP entry\n\r");
        else
          printf(" Error %08x while removing ARP entry\n\r", *(unsigned int*)resposta.info);
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
          printf(" Route Table: NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1831:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully configure static route\n\r");
        else
          printf(" Error %08x while configuring static route\n\r", *(unsigned int*)resposta.info);
        break;

      case 1832:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully removed static route\n\r");
        else
          printf(" Error %08x while removing static route\n\r", *(unsigned int*)resposta.info);
        break;

      case 1840:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully created ping session\n\r");
        else
          printf(" Error %08x while creating ping session\n\r", *(unsigned int*)resposta.info);
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
          printf(" Ping session query: NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1842:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Sucessefully freed ping session\n\r");
        else
          printf(" Error %08x while freeing ping session\n\r", *(unsigned int*)resposta.info);
        break;


      case 1850:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully created Traceroute session\n\r");
        else
          printf(" Error %08x while creating Traceroute session\n\r", *(unsigned int*)resposta.info);
        break;

      case 1851:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
        {  
          msg_RoutingTracertSessionQuery *ptr;
          uint8 entries;

          entries = resposta.infoDim/sizeof(msg_RoutingTracertSessionQuery);

          if (entries==0) {
            printf(" Traceroute session query: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_RoutingTracertSessionQuery));
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
          printf(" Traceroute session query: NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
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
          printf(" Traceroute hops: NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1853:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Successfully freed traceroute session\n\r");
        else
          printf(" Error %08x while freeing traceroute session\n\r", *(unsigned int*)resposta.info);
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
            printf(" Slot %u, LAG %u\r\n",ptr->SlotId, ptr->id);
            printf("  Admin               = %s\r\n",((ptr->admin) ? "Enabled" : "Disabled"));
            printf("  STP state           = %s\r\n",((ptr->stp_enable) ? "Enabled" : "Disabled"));
            printf("  LAG type            = %s\r\n",((ptr->static_enable) ? "Static" : "Dynamic"));
            printf("  LoadBalance profile = %u\r\n",ptr->loadBalance_mode);
            printf("  Port bitmap         = 0x%08x 0x%08x\r\n",(unsigned int) ptr->members_pbmp2, (unsigned int) ptr->members_pbmp);
          }
          printf(" Switch: LAG configurations read successfully\n\r");
        }
        else
          printf(" Switch: LACP: Lag configurations NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1501:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: LACP: LAG created successfully\n\r");
        else
          printf(" Switch: LACP: Lag NOT created - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1502:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: LACP: LAG removed successfully\n\r");
        else
          printf(" Switch: LACP: Lag NOT removed - error %08x\n\r", *(unsigned int*)resposta.info);
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
            printf(" Slot %u LAG %u\r\n",ptr->SlotId, ptr->id);
            printf("  Admin               = %s\r\n",((ptr->admin) ? "Enabled" : "Disabled"));
            printf("  Link State          = %s\r\n",((ptr->link_status) ? "UP" : "DOWN"));
            printf("  Port channel type   = %s\r\n",((ptr->port_channel_type) ? "Static" : "Dynamic"));
            printf("  Member Ports bitmap = 0x%08x 0x%08x\r\n",(unsigned int) ptr->members_pbmp2,(unsigned int) ptr->members_pbmp1);
            printf("  Active Ports bitmap = 0x%08x 0x%08x\r\n",(unsigned int) ptr->active_members_pbmp2,(unsigned int) ptr->active_members_pbmp1);
          }
          printf(" Switch: LAG status read successfully\n\r");
        }
        else
          printf(" Switch: LACP: Lag status NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1504:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: LACP Admin State settled successfully\n\r");
        else
          printf(" Switch: LACP Admin State NOT settled - error %08x\n\r", *(unsigned int*)resposta.info);
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
            printf(" Slot %u Physical port %u:\r\n",ptr->SlotId, (uint16) ptr->id);
            printf("  State Aggregation = %s\r\n",((ptr->state_aggregation) ? "Enabled" : "Disabled"));
            printf("  LACP Activity     = %s\r\n",((ptr->lacp_activity    ) ? "True" : "False"));
            printf("  LACP Timeout      = %s\r\n",((ptr->lacp_timeout     ) ? "Short (3s)" : "Long (90s)"));
          }
          printf(" Switch: LACP Admin State read successfully\n\r");
        }
        else
          printf(" Switch: LACP Admin State NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
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
              printf( " LACP statistics for slot %u port %u:\n\r",po[i].SlotId,(uint16) po[i].id);
              printf( "   RX LACPdu's = %lu\n\r",po[i].LACPdus_rx);
              printf( "   TX LACPdu's = %lu\n\r",po[i].LACPdus_tx);
            }
            printf( "Done!\r\n");
          }
          else  {
            printf(" Switch: Error reading LACPdu statistics\n\r");
          }
        }
        break;

      case 1511:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: LACP Stats cleared successfully\n\r");
        else
          printf(" Switch: LACP Stats NOT cleared - error %08x\n\r", *(unsigned int*)resposta.info);
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

          printf("Slot %u EVC# %u\r\n", ptr->SlotId,(unsigned int)ptr->id);
          printf(" .flags         = 0x%04lx\r\n", ptr->flags);
          printf("   .stacked       = %s\r\n", ptr->flags & 0x0004 ? "True":"False");
          printf("   .MACLearning   = %s\r\n", ptr->flags & 0x0008 ? "Enabled":"Disabled");
          printf("   .CPU trapping  = %s\r\n", ptr->flags & 0x0010 ? "On":"Off");
          printf("   .DHCP snooping = %s\r\n", ptr->flags & 0x0100 ? "On":"Off");
          printf(" .MC Flood type = %u (%s)\r\n", ptr->mc_flood, ptr->mc_flood == 0 ? "All":ptr->mc_flood == 1 ? "Unknown":"None");
          printf(" .Interfaces #  = %u\r\n", ptr->n_intf);
          for (i=0; i<ptr->n_intf; i++) {
            printf("   %s# %02u %s VID=%u\r\n",
                   ptr->intf[i].intf_type == 0?"PHY":"LAG",
                   ptr->intf[i].intf_id,
                   ptr->intf[i].mef_type == 0?"Root":"Leaf",
                   ptr->intf[i].vid);
          }
        }
        break;

      case 1601:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC successfully created\n\r");
        else
          printf("Failed to create EVC - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1602:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC successfully deleted\n\r");
        else
          printf("Failed to delete EVC - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1603:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("Port successfully added to EVC\n\r");
        else
          printf("Failed to add port to EVC - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1604:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("Port successfully removed from EVC\n\r");
        else
          printf("Failed to remove port from EVC - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1605:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC stacked bridge successfully created\n\r");
        else
          printf("Failed to create EVC stacked bridge - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1606:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC stacked bridge successfully deleted\n\r");
        else
          printf("Failed to delete EVC stacked bridge - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1607:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC GEM flow successfully created\n\r");
        else
          printf("Failed to create EVC GEM flow - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1608:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("EVC GEM flow successfully deleted\n\r");
        else
          printf("Failed to delete EVC GEM flow - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1610:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK)) {
          msg_NtwConnectivity_t *pNtwConn = (msg_NtwConnectivity_t *) resposta.info;
          printf("Network Connectivity (mask=0x%08X)\r\n",  (unsigned int)pNtwConn->mask);
          printf("  Slot %u\r\n",                           pNtwConn->SlotId);
          printf("  IP Addr         = %lu.%lu.%lu.%lu\r\n", (pNtwConn->ipaddr  >> 24) & 0xFF, (pNtwConn->ipaddr  >> 16) & 0xFF,
                                                            (pNtwConn->ipaddr  >>  8) & 0xFF,  pNtwConn->ipaddr         & 0xFF);
          printf("  Mask            = %lu.%lu.%lu.%lu\r\n", (pNtwConn->netmask >> 24) & 0xFF, (pNtwConn->netmask >> 16) & 0xFF,
                                                            (pNtwConn->netmask >>  8) & 0xFF,  pNtwConn->netmask        & 0xFF);
          printf("  Gateway         = %lu.%lu.%lu.%lu\r\n", (pNtwConn->gateway >> 24) & 0xFF, (pNtwConn->gateway >> 16) & 0xFF,
                                                            (pNtwConn->gateway >>  8) & 0xFF,  pNtwConn->gateway        & 0xFF);
          printf("  Mgmt VLAN ID    = %u\r\n",              pNtwConn->mgmtVlanId);
          printf("  Interfaces (%d):\r\n", pNtwConn->n_intf);
          int i;
          for (i=0; i<pNtwConn->n_intf; i++) {
            printf("    Intf %u/%u\r\n", pNtwConn->intf[i].intf_type, pNtwConn->intf[i].intf_id);
          }
        }
        else
          printf("Failed to get network connectivity config - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1611:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf("Network Connectivity successfully configured\n\r");
        else
          printf("Failed to set network connectivity config - error %08X\r\n", *(unsigned int*)resposta.info);
        break;

      case 1620:
        {
          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          {
            msg_HwEthBwProfile_t *ptr = (msg_HwEthBwProfile_t *) &resposta.info[0];

            printf(" Slot=%u EVCid=%lu\r\n",ptr->SlotId,ptr->evcId);
            if (ptr->mask & MSG_HWETH_BWPROFILE_MASK_INTF_SRC)
              printf(" SrcIntf=%u/%u\r\n",ptr->intf_src.intf_type,ptr->intf_src.intf_id);
            if (ptr->mask & MSG_HWETH_BWPROFILE_MASK_INTF_DST)
              printf(" DstIntf=%u/%u\r\n",ptr->intf_dst.intf_type,ptr->intf_dst.intf_id);
            if (ptr->mask & MSG_HWETH_BWPROFILE_MASK_SVLAN)
              printf(" SVid=%u\r\n",ptr->service_vlan);
            if (ptr->mask & MSG_HWETH_BWPROFILE_MASK_CVLAN)
              printf(" CVid=%u\r\n",ptr->client_vlan);
            if (ptr->mask & MSG_HWETH_BWPROFILE_MASK_PROFILE)
            {
              printf(" CIR=%llu bps\r\n",ptr->profile.cir);
              printf(" EIR=%llu bps\r\n",ptr->profile.eir);
              printf(" CBS=%llu bytes\r\n",ptr->profile.cbs);
              printf(" EBS=%llu bytes\r\n",ptr->profile.ebs);
            }
            else
            {
              printf(" Non existent profile!\r\n");
            }
            printf("Switch: BW profile read successfully\n\r");
          }
          else
            printf(" Switch: BW profile not read - error %08x\n\r", *(unsigned int*)resposta.info);
        }
        break;

      case 1621:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: BW profile added successfully\n\r");
        else
          printf(" Switch: BW profile not added - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1622:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: BW profile removed successfully\n\r");
        else
          printf(" Switch: BW profile not removed - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1624:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Storm Control configured successfully\n\r");
        else
          printf(" Switch: Storm Control NOT configured - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1625:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: Storm Control reseted successfully\n\r");
        else
          printf(" Switch: Storm Control not reseted - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1630:
        {
          if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          {
            msg_evcStats_t *ptr;

            if (resposta.infoDim!=sizeof(msg_evcStats_t)) {
              printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(msg_evcStats_t));
              break;
            }

            ptr = (msg_evcStats_t *) &resposta.info[0];

            printf(" Flow Counters:\r\n");
            printf(" Slot=%u FlowId=%lu\r\n",ptr->SlotId,ptr->evc_id);
            if (ptr->mask & MSG_EVC_COUNTERS_MASK_INTF)
              printf(" Intf   =%u/%u\r\n",ptr->intf.intf_type,ptr->intf.intf_id);
            if (ptr->mask & MSG_EVC_COUNTERS_MASK_SVLAN)
              printf(" SVid   =%u\r\n",ptr->service_vlan);
            if (ptr->mask & MSG_EVC_COUNTERS_MASK_CVLAN)
              printf(" CVid   =%u\r\n",ptr->client_vlan);
            if (ptr->mask & MSG_EVC_COUNTERS_MASK_CHANNEL)
              printf(" Channel=%03u:%03u:%03u:%03u\r\n",(unsigned int) ((ptr->channel_ip>>24) & 0xFF),
                                                        (unsigned int) ((ptr->channel_ip>>16) & 0xFF),
                                                        (unsigned int) ((ptr->channel_ip>> 8) & 0xFF),
                                                        (unsigned int) (ptr->channel_ip & 0xFF));
            if (ptr->mask & MSG_EVC_COUNTERS_MASK_STATS)
            {
              if (ptr->stats.mask_stat & MSG_EVC_COUNTERS_MASK_STATS_RX)
              {
                printf("RX stats...\r\n");
                printf(" Total    : %10lu\r\n",ptr->stats.rx.pktTotal    );
                printf(" Unicast  : %10lu\r\n",ptr->stats.rx.pktUnicast  );
                printf(" Multicast: %10lu\r\n",ptr->stats.rx.pktMulticast);
                printf(" Broadcast: %10lu\r\n",ptr->stats.rx.pktBroadcast);
                printf(" Dropped  : %10lu\r\n",ptr->stats.rx.pktDropped  );
              }
              if (ptr->stats.mask_stat & MSG_EVC_COUNTERS_MASK_STATS_TX)
              {
                printf("TX stats...\r\n");
                printf(" Total    : %10lu\r\n",ptr->stats.tx.pktTotal    );
                printf(" Unicast  : %10lu\r\n",ptr->stats.tx.pktUnicast  );
                printf(" Multicast: %10lu\r\n",ptr->stats.tx.pktMulticast);
                printf(" Broadcast: %10lu\r\n",ptr->stats.tx.pktBroadcast);
                printf(" Dropped  : %10lu\r\n",ptr->stats.tx.pktDropped  );
              }
            }
            else
            {
              printf(" Non existent stats!\r\n");
            }
            printf(" Switch: EVC counters read successfully\n\r");
          }
          else
            printf(" Switch: Flow counters not read - error %08x\n\r", *(unsigned int*)resposta.info);
          break;
        }
        break;

      case 1632:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: EVC counters added successfully\n\r");
        else
          printf(" Switch: EVC counters not added - error %08x\n\r", *(unsigned int*)resposta.info);
        break;

      case 1633:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" Switch: EVC counters removed successfully\n\r");
        else
          printf(" Switch: EVC counters not removed - error %08x\n\r", *(unsigned int*)resposta.info);
        break;
      case 1700:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" IP Source Guard Correctly Configured\n\r");
        else
          printf(" IP Source Guard not Configured\n\r");          
        break;
      case 1701:
        if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
          printf(" IP Source Guard Static Entry Correctly Configured\n\r");
        else
          printf(" IP Source Guard Static Entry not Configured\n\r");          
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
  uint8  index;
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
  multiplier = 1;
  // Run all characters starting from the last one
  for (index=0,--ipaddr; index<4 && ipaddr>=start_ipaddr; ipaddr--)  {
    // If character is a decimal digit...
    if (isdigit(*ipaddr)) {
      address[index] += (uint8) (*ipaddr-'0')*multiplier;   // update address array
      multiplier*=10;                                       // update decimal multiplier for next digit
    }
    // Other, is a dot character
    else
    {
      index++;                                              // Increment address array index
      multiplier=1;                                         // Reinitialize decimal multiplier
    }
  }

  // Calculate uint32 value
  *value_uint64 = ((uint32) address[0]<<0) | ((uint32) address[1]<<8) | ((uint32) address[2]<<16) | ((uint32) address[3]<<24);

  return 0;
}

