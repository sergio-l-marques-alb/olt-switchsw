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

#define PTIN_FIRST_ROOT_PORT  8

int canal_buga;

static int convert_ipaddr2uint32(const char *ipaddr, uint32 *value_uint32);

void help_oltBuga(void)
{
  printf(
    "\n\n\r"
    "usar: buga [Opcoes]\n\r"
    "\n\r"
    "Opcoes:\n\r"
    "help [comando]\n\r"
    "ping - Waits until fastpath application is up\r\n"
    "m 1000 console[/dev/...]\n\r"
    "m 1001 - Show network configurations\r\n"
    "m 1002 <ipaddr> <netmask> <gateway> <managememt_vlan> - Configure network definitions\r\n"
    "m 1003 <gateway> - Configure gateway ip\r\n"
    "m 1004 - Get resources state\r\n"
    "m 1010 port[0-15] enable[0,1] speed[1G=3,2.5G=4] fullduplex[0,1] framemax(bytes) lb[0,1] macLearn[0,1] - switch port configuration\n\r"
    "m 1011 port[0-15] - get switch port configuration\n\r"
    "m 1012 port[0-15] - Get Phy states\n\r"
    "m 1020 port[0-15] - Show switch RFC2819 statistics\n\r"
    "m 1021 port[0-15] - Clear switch RFC2819 statistics\n\r"
    "m 1030 port[0-35] - Get QoS configuration\r\n"
    "m 1031 port[0-35] trustMode[1:Untrust;2:802.1P;3:IPprec;4:DSCP] shapingRate[0-100] cos_pr0[0-7] cos_pr1 ... cos_pr7 - Set general QoS configuration\r\n"
    "m 1032 port[0-35] cos[0-7] scheduler[1:Strict;2:Weighted] min_bandwidth[Mbps] max_bandwidth[Mbps] - Set specific QoS configuration\r\n"
    "m 1040 <page> - Read dynamic MAC table (start reading from page 0)\r\n"
    "m 1041 <page> - Read static MAC table (start reading from page 0)\r\n"
    "m 1042 flow_id[1-127] port[0-35] macAddr[xxxxxxxxxxxxh] - Add a static entry to the MAC table\r\n"
    "m 1043 flow_id[1-127] port[0-35] macAddr[xxxxxxxxxxxxh] - Remove an entry to the MAC table\r\n"
    "m 1044 vlan_id[1-4095] macAddr[xxxxxxxxxxxxh] - Remove an entry from MAC table\r\n"
    "m 1100 flow_id[1-127] - Read E-tree info of id <flow_id>\n\r"
    "m 1101 flow_id[1-127] type[Normal=0,TLS=10,MC=11] mc_flood[0/1] dhcp[0/1] port1[0-15] svid1[2-4095] port2 svid2 ... port16 svid16 - Create an E-tree of id <flow_id>\n\r"
    "m 1102 flow_id[1-127] - Remove an E-tree of id <flow_id>\n\r"
    "m 1200 flow_id[1-127] cvid[1-4095] - Read BWP associated to flow <flow_id> and CVlanID <cvid>\n\r"
    "m 1201 flow_id[1-127] cvid[1-4095] cir[mbps] eir[mbps] dstPort[0-7] - Associate a BWP to flow <flow_id> and CVlanID <cvid>\n\r"
    "m 1202 flow_id[1-127] cvid[1-4095] - Remove BWP associated to flow <flow_id> and CVlanID <cvid>\n\r"
    "m 1210 flow_id[1-127] cvid[1-4095] - Show Vlan statistics\n\r"
    "m 1211 flow_id[1-127] clientVlan/IPChannel - Show absolute flow statistics\n\r"
    "m 1212 flow_id[1-127] clientVlan/IPChannel - Show differential flow statistics\n\r"
    "m 1213 flow_id[1-127] clientVlan/IPChannel - Add flow statistics measurement\n\r"
    "m 1214 flow_id[1-127] clientVlan/IPChannel - Remove flow statistics measurement\n\r"
    "m 1220 flow_id[1-127] port[0-17] cvid[1-4095] - Read DHCPop82 profile\n\r"
    "m 1221 flow_id[1-127] port[0-17] cvid[1-4095] <circuitId> <remoteId> - Define a DHCPop82 profile\n\r"
    "m 1222 flow_id[1-127] port[0-17] cvid[1-4095] - Remove a DHCPop82 profile\n\r"
    "m 1223 <access_node_id> - Redefine the DHCPop82 Access Node Identifier\n\r"
    "m 1240 <page> - Read DHCP binding table (start reading from page 0)\r\n"
    "m 1242 macAddr[xxxxxxxxxxxxh] - Remove a MAC address from DHCP Binding table\r\n"
    "m 1301 flow_id1[1-127] flow_id2[1-127] ... - Add multicast EVC ID's (for IGMP management)\n\r"
    "m 1302 flow_id1[1-127] flow_id2[1-127] ... - Remove multicast EVC ID's (for IGMP management)\n\r"
    "m 1304 flow_id[1-127] cvid1[1-4095] cvid2[1-4095] ... - Add multicast vlan clients for EVC <flow_id> (for IGMP management)\n\r"
    "m 1305 flow_id[1-127] cvid1[1-4095] cvid2[1-4095] ... - Remove multicast vlan clients for EVC <flow_id> (for IGMP management)\n\r"
    "m 1310 flow_id[1-127] port[0-15]   - Show IGMP statistics for interface <port> associated to EVC <flow_id>\n\r"
    "m 1311 flow_id[1-127] cvid[1-4095] - Show IGMP statistics for client of vlan <cvid> associated to EVC <flow_id>\n\r"
    "m 1315 flow_id[1-127] port[0-15]   - Clear IGMP statistics for interface <port> associated to EVC <flow_id>\n\r"
    "m 1316 flow_id[1-127] cvid[1-4095] - Clear IGMP statistics for client of vlan <cvid> associated to EVC <flow_id>\n\r"
    "m 1320 flow_id[1-127] port[0-15]   - Show DHCP statistics for interface <port> associated to EVC <flow_id>\n\r"
    "m 1321 flow_id[1-127] cvid[1-4095] - Show DHCP statistics for client of vlan <cvid> associated to EVC <flow_id>\n\r"
    "m 1325 flow_id[1-127] port[0-15]   - Clear DHCP statistics for interface <port> associated to EVC <flow_id>\n\r"
    "m 1326 flow_id[1-127] cvid[1-4095] - Clear DHCP statistics for client of vlan <cvid> associated to EVC <flow_id>\n\r"
    "m 1400 snooping_admin[0/1] querier_admin[0/1] querier_ipaddr[ddd.ddd.ddd.ddd] querier_inerval[1-1800] cos[0-7] - IGMP snooping admin mode\r\n"
    "m 1402 port1[0-15] type1[0=client,1=router] port2 type2 ... - IGMP snooping: add client interfaces\r\n"
    "m 1403 port1[0-15] port2 ...   - IGMP snooping: remove interfaces\r\n"
    "m 1405 vlan1[2-4093] xlate1[2-4093] vlan2 xlate2 ... - IGMP snooping: add mclient vlans and their translation values\r\n"
    "m 1406 vlan1[2-4093] vlan2 ... - IGMP snooping: remove mclient vlans\r\n"
    "m 1408 port[0-15] vlan1[2-4093] vlan2 ...     - IGMP snooping: add mrouter vlans for the given port\r\n"
    "m 1409 port[0-15] vlan1[2-4093] vlan2 ...     - IGMP snooping: remove mrouter vlans for the given port\r\n"
    "m 1411 vlan1[2-4093] ipaddr1[ddd.ddd.ddd.ddd] vlan2 ipaddr2 ... - IGMP snooping querier: Add vlan and its ip address\r\n"
    "m 1412 vlan1[2-4093] vlan2 ... - IGMP snooping querier: remove vlans\r\n"
    "m 1420 flow_id[1-127] client_vlanId[1-4095/0] - List active channels for a particular EVC and client Vlan (if 0 show for all clients)\r\n"
    "m 1421 flow_id[1-127] ipchannel[ddd.ddd.ddd.ddd] - List clients watching a channel (ip) associated to this EVCid\r\n"
    "m 1430 flow_id[1-127] ipchannel[ddd.ddd.ddd.ddd] srcport_bmp[XXXXXh] dstport_bmp[XXXXXh] - Add static MC channel\r\n"
    "m 1431 flow_id[1-127] ipchannel[ddd.ddd.ddd.ddd] - Remove static MC channel\r\n"
    "m 1500 lag_index[18..35] - Get LAG configurations\r\n"
    "m 1501 lag_index[18..35] static_mode[0/1] load_balance[0..6] port_bmp[XXXXXh] - Create LAG\r\n"
    "m 1502 lag_index[18..35] - Destroy LAG\r\n"
    "m 1503 lag_index[18..35] - Get LAG status\r\n"
    "m 1504 port_index[0..15/-1] lacp_aggregation[0/1] lacp_activity[0/1] lacp_timeout[0=long,1=short] - Set LACP Admin State\r\n"
    "m 1505 port_index[0..15/-1] - Get LACP Admin State\r\n"
    "m 1510 port[0-17/-1] - Show LACP statistics for port <port>\n\r"
    "m 1511 port[0-17/-1] - Clear LACP statistics for port <port>\n\r"
    "m 1998 - Reset alarms\r\n"
    "m 1999 <param> - Reset defaults, except for lag <param>\r\n"
    "m 2000 [10-1000000] - Set MAC Learning table aging time\r\n"

    /*"m 1304 port[0-15] - Get SFP info\n\r"*/
    "\n\r"
    );
}


//******************************************************************************
//
//******************************************************************************

int main (int argc, char *argv[])
{  
  int cnt;
  uint32 valued;
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
  else if ( !strcmp(argv[1],"ping") && ( argc == 2 ) )
  {
    //printf("A mensagem a enviar e' a %d\n\r", msg);

    // 1 - Preparar mensagem a enviar ao modulo de controlo
     comando.protocolId   = 1;
     comando.srcId        = PORTO_TX_MSG_BUGA;
     comando.dstId        = PORTO_RX_MSG_BUGA;
     comando.flags        = (FLAG_COMANDO); //(IPCLIB_FLAGS_CMD | (IPC_UID<<4));
     comando.counter      = rand ();
     comando.msgId        = CHMSG_APPLICATION_IS_ALIVE;
     comando.infoDim      = sizeof(unsigned int);
     *(int*)comando.info  = 0;

     canal_buga=open_ipc(PORTO_TX_MSG_BUGA,IP_LOCALHOST,NULL,20);
     if ( canal_buga<0 )
     {
       printf("Erro no open IPC do BUGA...\n\r");
       exit(0);
     }
     printf("Checking if Fastpath is alive...\n\r");
     cnt = 0;
     do {
       // Send command
       valued = send_data (canal_buga, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta);
       if ( valued )
       {
         printf("Without answer from fastpath... probably is still starting\n\r");
       }
       else if (resposta.flags!=(FLAG_RESPOSTA | FLAG_ACK)) {
         printf("Invalid answer... Request not Aknowledged\n\r");
       }
     } while ((cnt++)<6 && (valued || resposta.flags!=(FLAG_RESPOSTA | FLAG_ACK)));
     close_ipc(canal_buga);
     if (cnt<6) {
       printf("Fastpath responded... Application is up!\n\r");
     }
     else {
       printf("Fastpath did not respond... Timeout!\n\r");
     }
     exit(0);
  }
  else if ( !strcmp(argv[1],"m") && ( argc >= 3 ) )
  {
    if (StrToLong(argv[2],&valued)>=0) 
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
          case 2000: {
            L7_switch_config *swcfg;

            /* Validate number of arguments */
            if (argc != 3+1)  {
              help_oltBuga();
              exit(0);
            }

            comando.msgId = CCMSG_ETH_SWITCH_SET;
            comando.infoDim = sizeof(L7_switch_config);

            swcfg = (L7_switch_config *)comando.info;
            swcfg->mask = 0x01;

            /* integer value to be sent */
            char *endptr;
            swcfg->aging_time = strtol(argv[3], &endptr, 10);

            if (*endptr != '\0') {
              help_oltBuga();
              exit(0);
            }

            break;
          }

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

            comando.msgId = CHMSG_RESET_ALARMS;
            comando.infoDim = sizeof(int);
          }
          break;

          // Reset defaults
          case 1999:
          {
            int *ptr;

            // Validate number of arguments
            if (argc!=3+1)  {
              help_oltBuga();
              exit(0);
            }

            ptr = (int *) &(comando.info[0]);

            // integer value to be sent
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            *ptr = (uint8) valued;

            comando.msgId = CHMSG_RESET_DEFAULTS;
            comando.infoDim = sizeof(int);
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

            len = strlen((char *) &comando.info[0]);
            if (len>100) {
              printf("tty device name too long\n");
              exit(0);
            }

            strcpy((char *) &comando.info[0], tty_name );

            comando.msgId = CHMSH_ETH_CHANGE_STDOUT;
            comando.infoDim = strlen(tty_name)+1;
          }
          break;

          // Consult Network definitions
          case 1001:
          {
            st_NtwConnectivity *ptr;

            // Validate number of arguments
            if (argc!=3+0)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_NtwConnectivity *) &(comando.info[0]);

            // Clear structure
            memset(ptr,0x00,sizeof(st_NtwConnectivity));

            comando.msgId = CHMSG_ETH_NTW_CONNECTIVITY_SHOW;
            comando.infoDim = sizeof(st_NtwConnectivity);
          }
          break;

          // Configure network definitions
          case 1002:
          {
            st_NtwConnectivity *ptr;

            // Validate number of arguments
            if (argc!=3+4)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_NtwConnectivity *) &(comando.info[0]);
            ptr->err_code = 0;

            // Ip address
            if (convert_ipaddr2uint32(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->ipaddr = (uint32) valued;
            // IP mask
            if (convert_ipaddr2uint32(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            ptr->netmask = (uint32) valued;
            // Default gateway
            if (convert_ipaddr2uint32(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            ptr->defGateway = (uint32) valued;
            // Management vlan
            if (StrToLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            ptr->MngmVlanId = (uint16) valued;
            
            // Other parameters
            memset(ptr->localMacAddr,0x00,sizeof(uint8)*6);
            ptr->ntwConfProtocol = 0;

            comando.msgId = CHMSG_ETH_NTW_CONNECTIVITY;
            comando.infoDim = sizeof(st_NtwConnectivity);
          }
          break;

          // Configure Route IP / Default gateway
          case 1003:
          {
            st_RouteConnectivity *ptr;

            // Validate number of arguments
            if (argc!=3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_RouteConnectivity *) &(comando.info[0]);
            ptr->err_code = 0;

            // Ip address
            if (convert_ipaddr2uint32(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->ipaddr = (uint32) valued;
            
            // Other parameters
            ptr->netmask = 0xFFFFFFFF;
            ptr->routeOp = 0;

            comando.msgId = CHMSG_ETH_ROUTE_CONNECTIVITY;
            comando.infoDim = sizeof(st_RouteConnectivity);
          }
          break;

          // Get resources state
          case 1004:
          {
            int *ptr;

            // Validate number of arguments
            if (argc!=3+0)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (int *) &(comando.info[0]);
            *ptr = 0;

            comando.msgId = CHMSG_APPLICATION_RESOURCES;
            comando.infoDim = sizeof(int);
          }
          break;

          // Switch Phy port configuration
          case 1010:
          {
            st_HWEthPhyConf *ptr;
            
            // Validate number of arguments
            if (argc!=3+7)  {
              help_oltBuga();
              exit(0);
            }
            
            // Pointer to data array
            ptr = (st_HWEthPhyConf *) &(comando.info[0]);
            // port
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->Port = (uint8) valued;
            // enable
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            ptr->HWEthPhyConf_Block.PortEnable = (uint8) valued;
            // speed
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            ptr->HWEthPhyConf_Block.Speed = (uint8) valued;
            // full_duplex
            if (StrToLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            ptr->HWEthPhyConf_Block.Duplex = (uint8) valued;
            // frame_max length
            if (StrToLong(argv[3+4],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->HWEthPhyConf_Block.MaxFrame = (uint16) valued;
            // loopback
            if (StrToLong(argv[3+5],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            ptr->HWEthPhyConf_Block.LoopBack = (uint8) valued;
            // MAC learning
            if (StrToLong(argv[3+6],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }              
            ptr->HWEthPhyConf_Block.MacLearning = (uint8) valued;
            
            ptr->HWEthPhyConf_Block.Mask = 0xFFFF;
            comando.msgId = CHMSG_IF_GPON_ONU_SWITCH_PORT_CONFIG;
            comando.infoDim = sizeof(st_HWEthPhyConf);
          }
          break;

          // Switch Phy port configuration
          case 1011:
          {
            st_HWEthPhyConf *ptr;
            
            // Validate number of arguments
            if (argc!=3+1)  {
              help_oltBuga();
              exit(0);
            }
            
            // Pointer to data array
            ptr = (st_HWEthPhyConf *) &(comando.info[0]);
            // port
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->Port = (uint8) valued;
            
            ptr->HWEthPhyConf_Block.Mask = 0xFFFF;
            comando.msgId = CHMSG_OLT_GET_PORT_CONF;
            comando.infoDim = sizeof(st_HWEthPhyConf);
          }
          break;
          
          // Switch Phy states
          case 1012:
          {
            st_HWEthPhyState *ptr;

            // Validate number of arguments
            if (argc!=3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_HWEthPhyState *) &(comando.info[0]);
            // port
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->Port = (uint8) valued;

            ptr->HWEthPhyState_Block.Mask = 0xFFFF;
            comando.msgId = CHMSG_OLT_GET_PORT_STATE;
            comando.infoDim = sizeof(st_HWEthPhyState);
          }
          break;

          case 1020:
          {
            st_HWEthRFC2819Statistics *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_HWEthRFC2819Statistics *) &(comando.info[0]);

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->Port = (uint8) valued;

            ptr->Mask = 0xFF;
            ptr->PortStatistics.RxMask = 0xFFFFFFFF;
            ptr->PortStatistics.TxMask = 0xFFFFFFFF;

            comando.msgId = CHMSG_ETH_GET_COUNTERS;
            comando.infoDim = sizeof(st_HWEthRFC2819Statistics);
          }
          break;

          case 1021:
          {
            st_HWEthRFC2819Statistics *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_HWEthRFC2819Statistics *) &(comando.info[0]);

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->Port = (uint8) valued;

            comando.msgId = CHMSG_ETH_CLEAR_COUNTERS;
            comando.infoDim = sizeof(st_HWEthRFC2819Statistics);
          }
          break;

          case 1030:
          {
            st_QoSConfiguration *ptr;

            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_QoSConfiguration *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_QoSConfiguration));

            // port
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued==(uint32)-1) {
              ptr->index = 0xFF;
            }
            else if (valued>=SYSTEM_N_INTERF) {
              help_oltBuga();
              exit(0);
            }
            else if (valued>=SYSTEM_N_PORTS) {
              ptr->index = 0x80 | ((uint8) valued-SYSTEM_N_PORTS+1);
            }
            else {
              ptr->index = (uint8) valued;
            }

            comando.msgId = CHMSG_ETH_PORT_COS_CONFIG_GET;
            comando.infoDim = sizeof(st_QoSConfiguration);
          }
          break;

          case 1031:
          {
            st_QoSConfiguration *ptr;
            uint8 i;

            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+11)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_QoSConfiguration *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_QoSConfiguration));

            // port
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued==(uint32)-1) {
              ptr->index = 0xFF;
            }
            if (valued>=SYSTEM_N_INTERF) {
              help_oltBuga();
              exit(0);
            }
            else if (valued>=SYSTEM_N_PORTS) {
              ptr->index = 0x80 | ((uint8) valued-SYSTEM_N_PORTS+1);
            }
            else {
              ptr->index = (uint8) valued;
            }

            // Trust mode
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->trust_mode = (uint8) valued;
            ptr->mask |= 0x01;

            ptr->bandwidth_unit = 0;
            ptr->mask |= 0x02;

            // Shaping rate
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->shaping_rate = (uint8) valued;
            ptr->mask |= 0x04;


            // Priorities map
            for (i=0; i<8; i++) {
              if (StrToLong(argv[3+3+i],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->pktprio.cos[i] = (uint32) valued;
            }
            ptr->pktprio.mask = 0xFF;
            ptr->mask |= 0x08;

            comando.msgId = CHMSG_ETH_PORT_COS_CONFIG_SET;
            comando.infoDim = sizeof(st_QoSConfiguration);
          }
          break;

          case 1032:
          {
            st_QoSConfiguration *ptr;
            uint8 cos;

            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+5)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_QoSConfiguration *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_QoSConfiguration));

            // port
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued==(uint32)-1) {
              ptr->index = 0xFF;
            }
            if (valued>=SYSTEM_N_INTERF) {
              help_oltBuga();
              exit(0);
            }
            else if (valued>=SYSTEM_N_PORTS) {
              ptr->index = 0x80 | ((uint8) valued-SYSTEM_N_PORTS+1);
            }
            else {
              ptr->index = (uint8) valued;
            }

            // Extract CoS and validate it
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            cos = (uint8) valued;
            if (cos>=8) {
              help_oltBuga();
              exit(0);
            }

            ptr->mask = 0x10;
            ptr->cos_config.mask |= (1<<cos);

            // Scheduler
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->cos_config.cos[cos].scheduler = (uint8) valued;
            ptr->cos_config.cos[cos].mask |= 0x01;

            // Min Bandwidth
            if (StrToLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->cos_config.cos[cos].min_bandwidth = (uint32) valued*1000;
            ptr->cos_config.cos[cos].mask |= 0x02;

            // Max Bandwidth
            if (StrToLong(argv[3+4],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->cos_config.cos[cos].max_bandwidth = (uint32) valued*1000;
            ptr->cos_config.cos[cos].mask |= 0x04;

            comando.msgId = CHMSG_ETH_PORT_COS_CONFIG_SET;
            comando.infoDim = sizeof(st_QoSConfiguration);
          }
          break;

          case 1040:
          {
            st_switch_mac_table *ptr;

            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_switch_mac_table *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_switch_mac_table));

            // page
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = valued;

            comando.msgId = CHMSG_ETH_MAC_TABLE_SHOW;
            comando.infoDim = sizeof(st_switch_mac_table);
          }
          break;

          case 1041:
          {
            st_switch_mac_table *ptr;

            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_switch_mac_table *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_switch_mac_table));

            // page
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = valued;

            comando.msgId = CHMSG_ETH_MAC_STATIC_TABLE;
            comando.infoDim = sizeof(st_switch_mac_table);
          }
          break;

          case 1042:
          {
            st_switch_mac_operation *ptr;
            uint64 valued64;
  
            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+3)  {
              help_oltBuga();
              exit(0);
            }
  
            // Pointer to data array
            ptr = (st_switch_mac_operation *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_switch_mac_operation));
  
            // Flow id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->flow_id = (uint16) valued;
  
            // Port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->port = (uint8) valued;
  
            // MAC address
            if (StrToLongLong(argv[3+2],&valued64)<0)  {
              help_oltBuga();
              exit(0);
            }
            memcpy(ptr->mac_address,&(((uint8 *) &valued64)[2]),sizeof(uint8)*6);
  
            // No vlan
            ptr->vlan_id = (uint16)-1;
            ptr->static_address = 1;    // Is always a static entry
  
            comando.msgId = CHMSG_ETH_MAC_ENTRY_ADD;
            comando.infoDim = sizeof(st_switch_mac_operation);
          }
          break;

          case 1043:
          {
            st_switch_mac_table *ptr;
            uint64 valued64;
  
            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+3)  {
              help_oltBuga();
              exit(0);
            }
  
            // Pointer to data array
            ptr = (st_switch_mac_table *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_switch_mac_table));
  
            ptr->index = 0;
            ptr->mac_table_total_entries = 1;
            ptr->mac_table_msg_size      = 1;
            ptr->mac_table[0].mac_index  = 0;
            ptr->mac_table[0].vlan_id    = (uint16)-1;  // No vlan

            // Flow id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->mac_table[0].flow_id = (uint16) valued;
  
            // Port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->mac_table[0].port = (uint8) valued;
  
            // MAC address
            if (StrToLongLong(argv[3+2],&valued64)<0)  {
              help_oltBuga();
              exit(0);
            }
            memcpy(ptr->mac_table[0].mac_address,&(((uint8 *) &valued64)[2]),sizeof(uint8)*6);
    
            comando.msgId = CHMSG_ETH_MAC_ENTRY_REMOVE;
            comando.infoDim = sizeof(st_switch_mac_table);
          }
          break;

          case 1044:
          {
            st_switch_mac_table *ptr;
            uint64 valued64;
  
            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }
  
            // Pointer to data array
            ptr = (st_switch_mac_table *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_switch_mac_table));
  
            ptr->mac_table_total_entries = 1;
            ptr->mac_table_msg_size      = 1;
            ptr->mac_table[0].mac_index  = 0;
            ptr->mac_table[0].flow_id    = (uint16)-1;
  
            // vlanId
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->mac_table[0].vlan_id = (uint16) valued;
  
            // MAC address
            if (StrToLongLong(argv[3+1],&valued64)<0)  {
              help_oltBuga();
              exit(0);
            }
            memcpy(ptr->mac_table[0].mac_address,&(((uint8 *) &valued64)[2]),sizeof(uint8)*6);
  
            comando.msgId = CHMSG_ETH_MAC_ENTRY_REMOVE;
            comando.infoDim = sizeof(st_switch_mac_table);
          }
          break;

          case 1100:
          {
            MSG_HwEthernetMef10EvcBundling *ptr;

            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (MSG_HwEthernetMef10EvcBundling *) &(comando.info[0]);
            memset(ptr,0,sizeof(MSG_HwEthernetMef10EvcBundling));

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint16) valued;

            comando.msgId = CHMSG_ETH_FLOW_READ;
            comando.infoDim = sizeof(MSG_HwEthernetMef10EvcBundling);
          }
          break;

          case 1101:
          {
            MSG_HwEthernetMef10EvcBundling *ptr;
            uint8 i, p;
            uint16 svid;
            
            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+8)  {
              help_oltBuga();
              exit(0);
            }
            
            // Pointer to data array
            ptr = (MSG_HwEthernetMef10EvcBundling *) &(comando.info[0]);
            memset(ptr,0,sizeof(MSG_HwEthernetMef10EvcBundling));
            
            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint16) valued;
            
            // Type
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->type = (uint8) valued;
            
            // MC flood
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= (uint64) (valued & 1)<<57;
            
            // DHCP enable
            if (StrToLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= (uint64) (valued & 1)<<56;
            
            // initialize port list, in order that initial port_type is -1
            memset(ptr->uni,0xFF,sizeof(st_HwEthernetMef10CeVidMap)*SYSTEM_N_PORTS);
            // Text line will come in pairs port+svid
            for (i=(3+4)+1; i<argc && i<(3+4)+((SYSTEM_N_PORTS+1)*2); i+=2)  {
              // Extract port
              if (StrToLong(argv[i-1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              p = (uint8) valued;
              // Extract svid
              if (StrToLong(argv[i],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              svid = (uint16) valued;
              // Validate port and svid
              if (p>=SYSTEM_N_INTERF || svid<=1 || svid>=4095)  continue;
              // Determine port type and write to port list
              // Physical port
              if (p<SYSTEM_N_PORTS) {
                ptr->uni[p].type = (p<PTIN_FIRST_ROOT_PORT) ? MEF10_EVC_UNITYPE_LEAF : MEF10_EVC_UNITYPE_ROOT;
                ptr->uni[p].ceVid = svid;
              }
              // LAG interface
              else  {
                ptr->index |= (uint64) (p-SYSTEM_N_PORTS+1)<<32;
                ptr->outerTag = svid;
              }
            }
            
            comando.msgId = CHMSG_ETH_FLOW_ADD;
            comando.infoDim = sizeof(MSG_HwEthernetMef10EvcBundling);
          }
          break;
          
          case 1102:
          {
            MSG_HwEthernetMef10p1EvcRemove *ptr;
            
            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }
            
            // Pointer to data array
            ptr = (MSG_HwEthernetMef10p1EvcRemove *) &(comando.info[0]);
            
            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint16) valued;
            
            comando.msgId = CHMSG_ETH_FLOW_REMOVE;
            comando.infoDim = sizeof(MSG_HwEthernetMef10p1EvcRemove);            
          }
          break;

          case 1200:
          {
            MSG_HwEthernetProfile *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (MSG_HwEthernetProfile *) &(comando.info[0]);

            // Clear structure
            memset(ptr,0x00,sizeof(MSG_HwEthernetProfile));

            ptr->index=0;
            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<48;
            // cvid
            if (argc>3+1) {
              if (StrToLong(argv[3+1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->index |= ((uint64) valued & 0xFFFF)<<32;
            }
            else  {
              ptr->index |= ((uint64) 0xFFFF)<<32;
            }

            ptr->CIR=0;
            ptr->EIR=0;
            ptr->CBS=0;
            ptr->EBS=0;

            comando.msgId = CHMSG_ETH_BW_PROFILE_GET;
            comando.infoDim = sizeof(MSG_HwEthernetProfile);
          }
          break;

          case 1201:
          {
            MSG_HwEthernetProfile *ptr;

            // Validate number of arguments
            if (argc<3+5)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (MSG_HwEthernetProfile *) &(comando.info[0]);

            ptr->index=0;
            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<48;
            // cvid
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<32;
            // cir
            if (StrToLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->CIR = (uint64) valued*1000000;
            // eir
            if (StrToLong(argv[3+4],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->EIR = (uint64) valued*1000000;
            // Remaining parameters
            ptr->CBS=10000;
            ptr->EBS=10000;

            // Destination port
            if (StrToLong(argv[3+5],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFF)<<8;

            comando.msgId = CHMSG_ETH_BW_PROFILE_CREATE;
            comando.infoDim = sizeof(MSG_HwEthernetProfile);
          }
          break;

          case 1202:
          {
            MSG_HwEthernetProfile *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (MSG_HwEthernetProfile *) &(comando.info[0]);

            ptr->index=0;
            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<48;
            // cvid
            if (argc>3+1) {
              if (StrToLong(argv[3+1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->index |= ((uint64) valued & 0xFFFF)<<32;
            }
            else {
              ptr->index |= ((uint64) 0xFFFF)<<32;
            }

            // Remaining parameters
            ptr->CIR=0;
            ptr->EIR=0;
            ptr->CBS=0;
            ptr->EBS=0;

            comando.msgId = CHMSG_ETH_BW_PROFILE_REMOVE;
            comando.infoDim = sizeof(MSG_HwEthernetProfile);
          }
          break;

          case 1210:
          {
            st_HWEth_VlanStatistics *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_HWEth_VlanStatistics *) &(comando.info[0]);

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->FlowId = (uint16) valued;
            ptr->SVid   = 0;

            // CVid
            if (argc>3+1) {
              if (StrToLong(argv[3+1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->stat.CVid = (uint16) valued;
            }
            else  {
              ptr->stat.CVid = (uint16) -1;
            }

            comando.msgId = CHMSG_ETH_GET_VLAN_COUNTERS;
            comando.infoDim = sizeof(st_HWEth_VlanStatistics);
          }
          break;

          // Flow Counters
          case 1211:
          case 1212:
          case 1213:
          case 1214:
          {
            st_ptin_flow_counters *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ptin_flow_counters *) &(comando.info[0]);

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // CVid
            if (argc>3+1) {
              if (StrToLong(argv[3+1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->c_id.client_vlan = (uint32) valued;
            }
            else  {
              ptr->c_id.client_vlan = 0;
            }

            switch (msg) {
              case 1211:
                comando.msgId = CHMSG_ETH_GET_FLOW_COUNTERS_ABS;
                break;
              case 1212:
                comando.msgId = CHMSG_ETH_GET_FLOW_COUNTERS_DIFF;
                break;
              case 1213:
                comando.msgId = CHMSG_ETH_ADD_FLOW_COUNTERS;
                break;
              case 1214:
                comando.msgId = CHMSG_ETH_REMOVE_FLOW_COUNTERS;
                break;
            }
            comando.infoDim = sizeof(st_ptin_flow_counters);
          }
          break;

          case 1220:
          {
            st_HwEthernetDhcpOpt82Profile *ptr;

            // Validate number of arguments
            if (argc<3+3)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_HwEthernetDhcpOpt82Profile *) &(comando.info[0]);

            ptr->index=0;
            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<48;
            // port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0x3F);
            // cvid
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<32;
            // CircuitId
            ptr->circuitId[0] = '\0';
            // RemoteId
            ptr->remoteId[0] = '\0';

            comando.msgId = CHMSG_ETH_DHCP_PROFILE_GET;
            comando.infoDim = sizeof(st_HwEthernetDhcpOpt82Profile);
          }
          break;

          case 1221:
          {
            st_HwEthernetDhcpOpt82Profile *ptr;

            // Validate number of arguments
            if (argc<3+5)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_HwEthernetDhcpOpt82Profile *) &(comando.info[0]);

            ptr->index=0;
            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<48;
            // port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0x3F);
            // cvid
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<32;
            // CircuitId
            if (strlen(argv[3+3])>63)  argv[3+3][63]='\0';
            strcpy(ptr->circuitId,argv[3+3]);
            // RemoteId
            if (strlen(argv[3+4])>63)  argv[3+4][63]='\0';
            strcpy(ptr->remoteId,argv[3+4]);

            comando.msgId = CHMSG_ETH_DHCP_PROFILE_CONFIG;
            comando.infoDim = sizeof(st_HwEthernetDhcpOpt82Profile);
          }
          break;

          case 1222:
          {
            st_HwEthernetDhcpOpt82Profile *ptr;

            // Validate number of arguments
            if (argc<3+3)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_HwEthernetDhcpOpt82Profile *) &(comando.info[0]);

            ptr->index=0;
            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0xFFFF)<<48;
            // port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= ((uint64) valued & 0x3F);
            // cvid
            if (argc>3+2) {
              if (StrToLong(argv[3+2],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->index |= ((uint64) valued & 0xFFFF)<<32;
            }

            comando.msgId = CHMSG_ETH_DHCP_PROFILE_REMOVE;
            comando.infoDim = sizeof(st_HwEthernetDhcpOpt82Profile);
          }
          break;

          case 1223:
          {
            st_HwEthernetAccessNodeId *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_HwEthernetAccessNodeId *) &(comando.info[0]);

            // CircuitId
            if (strlen(argv[3+0])>40)  argv[3+0][40]='\0';
            strcpy(ptr->access_node_id,argv[3+0]);

            comando.msgId = CHMSG_ETH_DHCP_ACCESS_NODE_ID_SET;
            comando.infoDim = sizeof(st_HwEthernetAccessNodeId);
          }
          break;

          // DHCP Bind table reading
          case 1240:
          {
            st_DHCP_bind_table *ptr;

            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_DHCP_bind_table *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_DHCP_bind_table));

            // page
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = valued;

            comando.msgId = CHMSG_ETH_DHCP_BIND_TABLE_SHOW;
            comando.infoDim = sizeof(st_DHCP_bind_table);
          }
          break;

          case 1242:
          {
            st_DHCP_bind_table *ptr;
            uint64 valued64;

            // Validate number of arguments (flow_id + 2 pairs port+svid)
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_DHCP_bind_table *) &(comando.info[0]);
            memset(ptr,0,sizeof(st_DHCP_bind_table));

            ptr->bind_table_total_entries = 1;
            ptr->bind_table_msg_size      = 1;
            ptr->bind_table[0].entry_index= 0;

            // MAC address
            if (StrToLongLong(argv[3+0],&valued64)<0)  {
              help_oltBuga();
              exit(0);
            }
            memcpy(ptr->bind_table[0].macAddr,&(((uint8 *) &valued64)[2]),sizeof(uint8)*6);

            comando.msgId = CHMSG_ETH_DHCP_BIND_TABLE_REMOVE;
            comando.infoDim = sizeof(st_DHCP_bind_table);
          }
          break;

          // Add multicast EVC
          case 1301:
          {
            st_ClientIgmp *ptr;
            uint16 flow_index;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientIgmp *) &(comando.info[0]);
            memset(ptr,0x00,sizeof(st_ClientIgmp));

            // flow ids
            for (flow_index=0; flow_index<(argc-3) && flow_index<100; flow_index++) {
              if (StrToLong(argv[3+flow_index],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr[flow_index].index = (uint64) valued;
            }

            comando.msgId = CHMSG_ETH_MC_FLOW_ADD;
            comando.infoDim = sizeof(st_ClientIgmp)*flow_index;
          }
          break;

          // Remove multicast EVC
          case 1302:
          {
            st_ClientIgmp *ptr;
            uint16 flow_index;

            // Validate number of arguments
            if (argc<3)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientIgmp *) &(comando.info[0]);

            // flow ids
            for (flow_index=0; flow_index<(argc-3) && flow_index<100; flow_index++) {
              if (StrToLong(argv[3+flow_index],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr[flow_index].index = (uint64) valued;
            }
            // In case of inexistance of provided EVC id's
            if (flow_index==0) {
              ptr[0].index = (uint64) -1;
              flow_index   = 1;
            }

            comando.msgId = CHMSG_ETH_MC_FLOW_REMOVE;
            comando.infoDim = sizeof(st_ClientIgmp)*flow_index;
          }
          break;


          // Add multicast client
          case 1304:
          {
            st_ClientIgmp *ptr;
            uint16 evcId;
            uint16 client_index;

            // Validate number of arguments
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientIgmp *) &(comando.info[0]);

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            evcId = (uint16) valued;

            // CVid
            for (client_index=0; client_index<(argc-3-1) && client_index<100; client_index++) {
              if (StrToLong(argv[3+1+client_index],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr[client_index].index = (uint64) evcId;
              ptr[client_index].cvid  = (uint16) valued;
            }

            comando.msgId = CHMSG_ETH_MC_CLIENT_ADD;
            comando.infoDim = sizeof(st_ClientIgmp)*client_index;
          }
          break;

          // Remove Multicast client
          case 1305:
          {
            st_ClientIgmp *ptr;
            uint16 evcId;
            uint16 client_index;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientIgmp *) &(comando.info[0]);

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            evcId = (uint16) valued;

            // CVid
            for (client_index=0; client_index<(argc-3-1) && client_index<100; client_index++) {
              if (StrToLong(argv[3+1+client_index],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr[client_index].index = (evcId==(uint16)-1) ? ((uint64) -1) : ((uint64) evcId);
              ptr[client_index].cvid  = (uint16) valued;
            }
            // in case of the inexistence of provided clients
            if (client_index==0) {
              ptr[0].index = (evcId==(uint16)-1) ? ((uint64) -1) : ((uint64) evcId);
              ptr[0].cvid  = (uint16) -1;
              client_index = 1;
            }

            comando.msgId = CHMSG_ETH_MC_CLIENT_REMOVE;
            comando.infoDim = sizeof(st_ClientIgmp)*client_index;
          }
          break;

          case 1310:
          {
            st_ClientIgmpStatistics *ptr;

            // Validate number of arguments
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientIgmpStatistics *) &(comando.info[0]);

            memset(ptr,0x00,sizeof(st_ClientIgmpStatistics));

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // Port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->port = (uint16) valued;

            comando.msgId = CHMSG_ETH_MC_PORT_STATS_SHOW;
            comando.infoDim = sizeof(st_ClientIgmpStatistics);
          }
          break;

          case 1311:
          {
            st_ClientIgmpStatistics *ptr;

            // Validate number of arguments
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientIgmpStatistics *) &(comando.info[0]);

            memset(ptr,0x00,sizeof(st_ClientIgmpStatistics));

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // Port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->cvid = (uint16) valued;

            comando.msgId = CHMSG_ETH_MC_CLIENT_STATS_SHOW;
            comando.infoDim = sizeof(st_ClientIgmpStatistics);
          }
          break;

          case 1315:
          {
            st_ClientIgmpStatistics *ptr;

            // Validate number of arguments
            if (argc<3+0)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientIgmpStatistics *) &(comando.info[0]);

            memset(ptr,0x00,sizeof(st_ClientIgmpStatistics));

            // flow_id
            if (argc>=3) {
              if (StrToLong(argv[3+0],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->index = (uint64) (valued & 0xFFFF);
            }
            else  {
              ptr->index = (uint64) -1;
            }

            // Port
            if (argc>=3+1) {
              if (StrToLong(argv[3+1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->port = (uint16) valued;
            }
            else  {
              ptr->port = (uint16) -1;
            }

            comando.msgId = CHMSG_ETH_MC_PORT_STATS_CLEAR;
            comando.infoDim = sizeof(st_ClientIgmpStatistics);
          }
          break;

          case 1316:
          {
            st_ClientIgmpStatistics *ptr;

            // Validate number of arguments
            if (argc<3+0)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientIgmpStatistics *) &(comando.info[0]);

            memset(ptr,0x00,sizeof(st_ClientIgmpStatistics));

            // flow_id
            if (argc>=3) {
              if (StrToLong(argv[3+0],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->index = (uint64) (valued & 0xFFFF);
            }
            else  {
              ptr->index = (uint64) -1;
            }

            // Port
            if (argc>=3+1) {
              if (StrToLong(argv[3+1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->cvid = (uint16) valued;
            }
            else  {
              ptr->cvid = (uint16) -1;
            }

            comando.msgId = CHMSG_ETH_MC_CLIENT_STATS_CLEAR;
            comando.infoDim = sizeof(st_ClientIgmpStatistics);
          }
          break;

          case 1320:
          {
            st_ClientDhcpStatistics *ptr;

            // Validate number of arguments
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientDhcpStatistics *) &(comando.info[0]);

            memset(ptr,0x00,sizeof(st_ClientDhcpStatistics));

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // Port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->port = (uint16) valued;

            comando.msgId = CHMSG_ETH_DHCP_PORT_STATS_SHOW;
            comando.infoDim = sizeof(st_ClientDhcpStatistics);
          }
          break;

          case 1321:
          {
            st_ClientDhcpStatistics *ptr;

            // Validate number of arguments
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientDhcpStatistics *) &(comando.info[0]);

            memset(ptr,0x00,sizeof(st_ClientDhcpStatistics));

            // flow_id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // Port
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->cvid = (uint16) valued;

            comando.msgId = CHMSG_ETH_DHCP_CLIENT_STATS_SHOW;
            comando.infoDim = sizeof(st_ClientDhcpStatistics);
          }
          break;

          case 1325:
          {
            st_ClientDhcpStatistics *ptr;

            // Validate number of arguments
            if (argc<3+0)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientDhcpStatistics *) &(comando.info[0]);

            memset(ptr,0x00,sizeof(st_ClientDhcpStatistics));

            // flow_id
            if (argc>=3) {
              if (StrToLong(argv[3+0],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->index = (uint64) (valued & 0xFFFF);
            }
            else  {
              ptr->index = (uint64) -1;
            }

            // Port
            if (argc>=3+1) {
              if (StrToLong(argv[3+1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->port = (uint16) valued;
            }
            else  {
              ptr->port = (uint16) -1;
            }

            comando.msgId = CHMSG_ETH_DHCP_PORT_STATS_CLEAR;
            comando.infoDim = sizeof(st_ClientDhcpStatistics);
          }
          break;

          case 1326:
          {
            st_ClientDhcpStatistics *ptr;

            // Validate number of arguments
            if (argc<3+0)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_ClientDhcpStatistics *) &(comando.info[0]);

            memset(ptr,0x00,sizeof(st_ClientDhcpStatistics));

            // flow_id
            if (argc>=3) {
              if (StrToLong(argv[3+0],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->index = (uint64) (valued & 0xFFFF);
            }
            else  {
              ptr->index = (uint64) -1;
            }

            // Port
            if (argc>=3+1) {
              if (StrToLong(argv[3+1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->cvid = (uint16) valued;
            }
            else  {
              ptr->cvid = (uint16) -1;
            }

            comando.msgId = CHMSG_ETH_DHCP_CLIENT_STATS_CLEAR;
            comando.infoDim = sizeof(st_ClientDhcpStatistics);
          }
          break;

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
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->snooping_admin = (uint8) valued & 1;

            // Default IGMP prio
            ptr->igmp_packet_cos = 5;

            // Querier admin
            if (argc>=3+2) {
              if (StrToLong(argv[3+1],&valued)<0)  {
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
              if (convert_ipaddr2uint32(argv[3+2],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->querier_ipaddress = valued;
            }
            // Querier interval
            if (argc>=3+4) {
              if (convert_ipaddr2uint32(argv[3+3],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              ptr->querier_interval = (uint16) valued;
              ptr->group_membership_interval = (uint16) 260;
            }
            // Querier interval
            if (argc>=3+5) {
              if (convert_ipaddr2uint32(argv[3+4],&valued)<0)  {
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
              if (StrToLong(argv[3+i-1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              port = (uint8) valued;
              if (port>=SYSTEM_N_PORTS)  continue;

              if (StrToLong(argv[3+i],&valued)<0)  {
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
              if (StrToLong(argv[3+i],&valued)<0)  {
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

              if (StrToLong(argv[3+i-1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              vlan = (uint16) valued;

              if (StrToLong(argv[3+i],&valued)<0)  {
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
              if (StrToLong(argv[3+i],&valued)<0)  {
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
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            port = (uint8) valued;
            if (port>=SYSTEM_N_PORTS) {
              help_oltBuga();
              exit(0);
            }

            for (i=0; i<(argc-3-1) && i<100; i++) {
              if (StrToLong(argv[3+1+i],&valued)<0)  {
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
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            port = (uint8) valued;
            if (port>=SYSTEM_N_PORTS) {
              help_oltBuga();
              exit(0);
            }

            for (i=0; i<(argc-3-1) && i<100; i++) {
              if (StrToLong(argv[3+1+i],&valued)<0)  {
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

              if (StrToLong(argv[3+i-1],&valued)<0)  {
                help_oltBuga();
                exit(0);
              }
              vlan = (uint16) valued;

              if (convert_ipaddr2uint32(argv[3+i],&valued)<0)  {
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
              if (StrToLong(argv[3+i],&valued)<0)  {
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

          case 1420:
          {
            st_MCActiveChannels *ptr;

            // Validate number of arguments
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_MCActiveChannels *) &(comando.info[0]);
            memset(ptr,0x00,sizeof(st_MCActiveChannels));

            // EVC id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // Client vlan
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index |= (uint64) (valued & 0xFFFF)<<16;

            comando.msgId = CHMSG_ETH_MC_ACTIVE_CHANNELS_GET;
            comando.infoDim = sizeof(st_MCActiveChannels);
          }
          break;

          case 1421:
          {
            st_MCActiveChannelClients *ptr;

            // Validate number of arguments
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_MCActiveChannelClients *) &(comando.info[0]);
            memset(ptr,0x00,sizeof(st_MCActiveChannelClients));

            // EVC id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // Channel IP
            if (convert_ipaddr2uint32(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->channelIp = valued;

            comando.msgId = CHMSG_ETH_MC_CHANNEL_CLIENTS_GET;
            comando.infoDim = sizeof(st_MCActiveChannelClients);
          }
          break;

          case 1430:
          {
            st_MCStaticChannel *ptr;

            // Validate number of arguments
            if (argc<3+4)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_MCStaticChannel *) &(comando.info[0]);
            memset(ptr,0x00,sizeof(st_MCStaticChannel));

            // EVC id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // Channel IP
            if (convert_ipaddr2uint32(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->channelIp = valued;

            // Source ports bitmap
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->port_bmp = valued;

            // Destination ports bitmap
            if (StrToLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->portfwd_bmp = valued;

            comando.msgId = CHMSG_ETH_MC_STATIC_CHANNEL_ADD;
            comando.infoDim = sizeof(st_MCStaticChannel);
          }
          break;

          case 1431:
          {
            st_MCStaticChannel *ptr;

            // Validate number of arguments
            if (argc<3+2)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_MCStaticChannel *) &(comando.info[0]);
            memset(ptr,0x00,sizeof(st_MCStaticChannel));

            // EVC id
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) (valued & 0xFFFF);

            // Channel IP
            if (convert_ipaddr2uint32(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->channelIp = valued;

            ptr->port_bmp = 0;
            ptr->portfwd_bmp = 0;

            comando.msgId = CHMSG_ETH_MC_STATIC_CHANNEL_DEL;
            comando.infoDim = sizeof(st_MCStaticChannel);
          }
          break;

          case 1500:
          {
            st_LACPLagInfo *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_LACPLagInfo *) &(comando.info[0]);
            ptr->err_code = 0;

            // LAG index
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued!=(uint32)-1 && (valued<SYSTEM_N_PORTS || valued>=SYSTEM_N_INTERF)) {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) ( (valued==(uint32)-1) ? 0 : ((valued-SYSTEM_N_PORTS+1) & 0xFF) );

            // Static mode
            ptr->static_enable = 0;

            // Load Balance mode
            ptr->loadBalance_mode = 0;

            // Member ports bitmap
            ptr->members_pbmp = 0;

            ptr->admin = 0;
            ptr->stp_enable = 0;

            comando.msgId = CHMSG_ETH_LACP_LAG_READ;
            comando.infoDim = sizeof(st_LACPLagInfo);
          }
          break;

          case 1501:
          {
            st_LACPLagInfo *ptr;

            // Validate number of arguments
            if (argc<3+4)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_LACPLagInfo *) &(comando.info[0]);

            // LAG index
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued<SYSTEM_N_PORTS || valued>=SYSTEM_N_INTERF) {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) ((valued-SYSTEM_N_PORTS+1) & 0xFF);

            // Static mode
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->static_enable = (uint64) (valued & 1);

            // Load Balance mode
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->loadBalance_mode = (uint64) (valued & 0xFF);

            // Member ports bitmap
            if (StrToLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->members_pbmp = (uint64) valued;

            ptr->admin = 1;
            ptr->stp_enable = 0;

            comando.msgId = CHMSG_ETH_LACP_LAG_CREATE;
            comando.infoDim = sizeof(st_LACPLagInfo);
          }
          break;

          case 1502:
          {
            st_LACPLagInfo *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_LACPLagInfo *) &(comando.info[0]);

            // LAG index
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued<SYSTEM_N_PORTS || valued>=SYSTEM_N_INTERF) {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) ((valued-SYSTEM_N_PORTS+1) & 0xFF);

            comando.msgId = CHMSG_ETH_LACP_LAG_DESTROY;
            comando.infoDim = sizeof(st_LACPLagInfo);
          }
          break;

          case 1503:
          {
            st_LACPLagStatus *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_LACPLagStatus *) &(comando.info[0]);
            ptr->err_code = 0;

            // LAG index
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued!=(uint32)-1 && (valued<SYSTEM_N_PORTS || valued>=SYSTEM_N_INTERF)) {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) ( (valued==(uint32)-1) ? 0 : ((valued-SYSTEM_N_PORTS+1) & 0xFF) );

            // Static mode
            ptr->port_channel_type = 0;
            // Link
            ptr->link_status = 0;
            // Member ports bitmap
            ptr->members_pbmp = 0;
            // Active ports bitmap
            ptr->active_members_pbmp = 0;
            // Admin
            ptr->admin = 0;

            comando.msgId = CHMSG_ETH_LACP_LAG_STATUS;
            comando.infoDim = sizeof(st_LACPLagStatus);
          }
          break;

          case 1504:
          {
            st_LACPAdminState *ptr;
            int port;
            uint8 i, state_aggregation, lacp_activity, lacp_timeout;

            // Validate number of arguments
            if (argc<3+3)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_LACPAdminState *) &(comando.info[0]);

            // Port index
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued>=SYSTEM_N_PORTS && valued!=(uint32)-1) {
              help_oltBuga();
              exit(0);
            }
            port = (int) valued;

            // Aggregation state
            if (StrToLong(argv[3+1],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            state_aggregation = (uint8) (valued & 1);

            // LACP Activity
            if (StrToLong(argv[3+2],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            lacp_activity = (uint8) (valued & 1);

            // Timeout
            if (StrToLong(argv[3+3],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            lacp_timeout = (uint8) (valued & 1);

            if (port==-1) {
              for (i=0; i<SYSTEM_N_PORTS; i++) {
                ptr[i].err_code = 0;
                ptr[i].index            = i;
                ptr[i].state_aggregation= state_aggregation;
                ptr[i].lacp_activity    = lacp_activity;
                ptr[i].lacp_timeout     = lacp_timeout;
              }
              comando.infoDim = sizeof(st_LACPAdminState)*SYSTEM_N_PORTS;
            }
            else if (port>=0 && port<SYSTEM_N_PORTS) {
              ptr->err_code = 0;
              ptr->index            = port;
              ptr->state_aggregation= state_aggregation;
              ptr->lacp_activity    = lacp_activity;
              ptr->lacp_timeout     = lacp_timeout;
              comando.infoDim = sizeof(st_LACPAdminState);
            }
            else {
              help_oltBuga();
              exit(0);
            }
            comando.msgId = CHMSG_ETH_LACP_ADMINSTATE_SET;
          }
          break;

          case 1505:
          {
            st_LACPAdminState *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_LACPAdminState *) &(comando.info[0]);

            // Port index
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            if (valued>=SYSTEM_N_PORTS && valued!=(uint32)-1) {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) ((int) valued);

            ptr->state_aggregation = 0;
            ptr->lacp_activity     = 0;
            ptr->lacp_timeout      = 0;

            comando.msgId = CHMSG_ETH_LACP_ADMINSTATE_GET;
            comando.infoDim = sizeof(st_LACPAdminState);
          }
          break;

          case 1510:
          {
            st_LACPStats *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_LACPStats *) &(comando.info[0]);
            ptr->err_code = 0;

            // Port index
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) ((int) valued);
            
            ptr->LACPdus_rx = 0;
            ptr->LACPdus_tx = 0;

            comando.msgId = CHMSG_ETH_LACP_STATS_SHOW;
            comando.infoDim = sizeof(st_LACPStats);
          }
          break;

          case 1511:
          {
            st_LACPStats *ptr;

            // Validate number of arguments
            if (argc<3+1)  {
              help_oltBuga();
              exit(0);
            }

            // Pointer to data array
            ptr = (st_LACPStats *) &(comando.info[0]);
            ptr->err_code = 0;

            // Port index
            if (StrToLong(argv[3+0],&valued)<0)  {
              help_oltBuga();
              exit(0);
            }
            ptr->index = (uint64) ((int) valued);

            ptr->LACPdus_rx = 0;
            ptr->LACPdus_tx = 0;

            comando.msgId = CHMSG_ETH_LACP_STATS_CLEAR;
            comando.infoDim = sizeof(st_LACPStats);
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
  
  
        // 2 - enviar mensagem e receber resposta
        //printf("comando.msgId %08x\n\r", comando.msgId);

        valued = send_data (canal_buga, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta);
  
        close_ipc(canal_buga);
        if ( valued )
        {
          printf("Erro %lu  no send_data IPC do BUGA...\n\r", valued);
          exit(0);
        }
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
              printf(" Switch: Aging time configured correctly\n\r");
            else
              printf(" Switch: Aging time configuration failed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1000:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Stdout redirected successfully\n\r");
            else
              printf(" Error redirecting Stdout - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1001:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
            {  
              st_NtwConnectivity *ptr;

              if (resposta.infoDim!=sizeof(st_NtwConnectivity)) {
                printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(st_NtwConnectivity));
                break;
              }

              ptr = (st_NtwConnectivity *) &resposta.info[0];
              printf(" Network definitions of DTL0 interface\r\n");
              printf("  IP address        : %u.%u.%u.%u\r\n",(uint16) ((ptr->ipaddr>>24) & 0xFF),(uint16) ((ptr->ipaddr>>16) & 0xFF),(uint16) ((ptr->ipaddr>>8) & 0xFF),(uint16) (ptr->ipaddr & 0xFF));
              printf("  Netmask           : %u.%u.%u.%u\r\n",(uint16) ((ptr->netmask>>24) & 0xFF),(uint16) ((ptr->netmask>>16) & 0xFF),(uint16) ((ptr->netmask>>8) & 0xFF),(uint16) (ptr->netmask & 0xFF));
              printf("  Default Gateway   : %u.%u.%u.%u\r\n",(uint16) ((ptr->defGateway>>24) & 0xFF),(uint16) ((ptr->defGateway>>16) & 0xFF),(uint16) ((ptr->defGateway>>8) & 0xFF),(uint16) (ptr->defGateway & 0xFF));
              printf("  Management VlanId : %u\r\n",ptr->MngmVlanId);
              printf("  MAC address       : %02X:%02X:%02X:%02X:%02X:%02X\r\n",ptr->localMacAddr[0],ptr->localMacAddr[1],ptr->localMacAddr[2],ptr->localMacAddr[3],ptr->localMacAddr[4],ptr->localMacAddr[5]);
              printf("  Network protocol  : %s\r\n",((ptr->ntwConfProtocol==1) ? "BOOTP" : ((ptr->ntwConfProtocol==2) ? "DHCP" : "None")));
              printf(" Switch: Network definitions read successfully\n\r");
            }
            else
              printf(" Switch: LACP: Lag configurations NOT read - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1002:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: DTL0 configured successfully\n\r");
            else
              printf(" Switch: DTL0 NOT configured - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1003:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: DTL0 default gateway configured successfully\n\r");
            else
              printf(" Switch: DTL0 default gateway NOT configured - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1004:
          {
            int i;
            st_msg_ptin_hw_resources *po=(st_msg_ptin_hw_resources *) &resposta.info[0];

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              // Validate size
              if (resposta.infoDim!=sizeof(st_msg_ptin_hw_resources)) {
                printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(st_msg_ptin_hw_resources),resposta.infoDim);
                break;
              }
              
              printf(" FFP available rules:\r\n");
              for (i=0; i<8; i++) {
                printf("  Group %d: %u\r\n",i,po->resources.ffp_available_rules[i]);
              }
              printf(" XLate available entries:\r\n");
              printf("  Ingress/Single tagged: %u\r\n",po->resources.vlanXlate_available_entries.ing_stag);
              printf("  Ingress/Double tagged: %u\r\n",po->resources.vlanXlate_available_entries.ing_dtag);
              printf("  Egress /Single tagged: %u\r\n",po->resources.vlanXlate_available_entries.egr_stag);

              printf(" Vlans available:\r\n");
              printf("  Igmp           : %u\r\n",po->resources.vlans_available.igmp);
              printf("  Dhcp           : %u\r\n",po->resources.vlans_available.dhcp);
              printf("  Broadcast Limit: %u\r\n",po->resources.vlans_available.bcastLim);

              printf(" Flow Counters available entries: %u\r\n",po->resources.flowCounters_available_entries);
              printf("Switch: resources list read successfully\n\r");
            }
            else
              printf(" Switch: Error reading resources list\n\r");
          }
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
            st_HWEthPhyConf *po=(st_HWEthPhyConf *) &resposta.info[0];

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              printf("Switch: port(s) read successfully\n\r");
              n_index = resposta.infoDim / sizeof(st_HWEthPhyConf);
              if ( n_index>SYSTEM_N_PORTS )  n_index=SYSTEM_N_PORTS;
              for ( index=0; index<n_index; index++ ) {
                printf("Port %u configuration:\n\r",po[index].Port);
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
                       po[index].HWEthPhyConf_Block.Speed,
                       po[index].HWEthPhyConf_Block.Media,
                       po[index].HWEthPhyConf_Block.Duplex,
                       po[index].HWEthPhyConf_Block.LoopBack,
                       po[index].HWEthPhyConf_Block.FlowControl,
                       po[index].HWEthPhyConf_Block.PortEnable,
                       po[index].HWEthPhyConf_Block.MaxFrame,
                       po[index].HWEthPhyConf_Block.VlanAwareness,
                       po[index].HWEthPhyConf_Block.MacLearning,
                       po[index].HWEthPhyConf_Block.AutoMDI );
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
            st_HWEthPhyState *po=(st_HWEthPhyState *) &resposta.info[0];

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              printf("Switch: port(s) read successfully\n\r");
              n_index = resposta.infoDim / sizeof(st_HWEthPhyState);
              if ( n_index>SYSTEM_N_PORTS )  n_index=SYSTEM_N_PORTS;
              for ( index=0; index<n_index; index++ ) {
                printf("Port %u eth state:\n\r",po[index].Port);
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
                       po[index].HWEthPhyState_Block.Speed,
                       po[index].HWEthPhyState_Block.Duplex,
                       po[index].HWEthPhyState_Block.Collisions,
                       po[index].HWEthPhyState_Block.RxActivity,
                       po[index].HWEthPhyState_Block.TxActivity,
                       po[index].HWEthPhyState_Block.LinkUp,
                       po[index].HWEthPhyState_Block.AutoNegComplete,
                       po[index].HWEthPhyState_Block.TxFault,
                       po[index].HWEthPhyState_Block.RemoteFault,
                       po[index].HWEthPhyState_Block.LOS,
                       po[index].HWEthPhyState_Block.Media,
                       po[index].HWEthPhyState_Block.MTU_mismatch,
                       po[index].HWEthPhyState_Block.Suported_MaxFrame );
              }
              printf("DONE!!!\n\r");
            }
            else
              printf(" Switch: Error reading phy states\n\r");
          }
          break;

          case 1020:
          {
            st_HWEthRFC2819Statistics *po=(st_HWEthRFC2819Statistics *) &resposta.info[0];

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              printf( "Switch statistics of port %u:\n\r"
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
                      po->Port,
                      po->PortStatistics.Rx.etherStatsDropEvents          , po->PortStatistics.Tx.etherStatsDropEvents          ,
                      po->PortStatistics.Rx.etherStatsOctets              , po->PortStatistics.Tx.etherStatsOctets              ,
                      po->PortStatistics.Rx.etherStatsPkts                , po->PortStatistics.Tx.etherStatsPkts                ,
                      po->PortStatistics.Rx.etherStatsBroadcastPkts       , po->PortStatistics.Tx.etherStatsBroadcastPkts       ,
                      po->PortStatistics.Rx.etherStatsMulticastPkts       , po->PortStatistics.Tx.etherStatsMulticastPkts       ,
                      po->PortStatistics.Rx.etherStatsCRCAlignErrors      , po->PortStatistics.Tx.etherStatsCRCAlignErrors      ,
                      po->PortStatistics.Rx.etherStatsUndersizePkts       , po->PortStatistics.Tx.etherStatsCollisions          ,
                      po->PortStatistics.Rx.etherStatsOversizePkts        , po->PortStatistics.Tx.etherStatsOversizePkts        ,
                      po->PortStatistics.Rx.etherStatsFragments           , po->PortStatistics.Tx.etherStatsFragments           ,
                      po->PortStatistics.Rx.etherStatsJabbers             , po->PortStatistics.Tx.etherStatsJabbers             ,
                      po->PortStatistics.Rx.etherStatsPkts64Octets        , po->PortStatistics.Tx.etherStatsPkts64Octets        ,
                      po->PortStatistics.Rx.etherStatsPkts65to127Octets   , po->PortStatistics.Tx.etherStatsPkts65to127Octets   ,
                      po->PortStatistics.Rx.etherStatsPkts128to255Octets  , po->PortStatistics.Tx.etherStatsPkts128to255Octets  ,
                      po->PortStatistics.Rx.etherStatsPkts256to511Octets  , po->PortStatistics.Tx.etherStatsPkts256to511Octets  ,
                      po->PortStatistics.Rx.etherStatsPkts512to1023Octets , po->PortStatistics.Tx.etherStatsPkts512to1023Octets ,
                      po->PortStatistics.Rx.etherStatsPkts1024to1518Octets, po->PortStatistics.Tx.etherStatsPkts1024to1518Octets,
                      po->PortStatistics.Rx.etherStatsPkts1519toMaxOctets , po->PortStatistics.Tx.etherStatsPkts1519toMaxOctets ,
                      po->PortStatistics.Rx.Throughput                    , po->PortStatistics.Tx.Throughput                    );
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
              st_QoSConfiguration *ptr;
              uint8 i, j, n, port;
              // Validate size
              if (resposta.infoDim==0 || (resposta.infoDim%sizeof(st_QoSConfiguration))!=0) {
                printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(st_QoSConfiguration),resposta.infoDim);
                break;
              }
              // Number of elements
              n = resposta.infoDim/sizeof(st_QoSConfiguration);

              // Run all elements
              for (i=0; i<n; i++) {

                // Pointer to element
                ptr = &(((st_QoSConfiguration *) resposta.info)[i]);

                // Calculate port
                port = (uint8) (ptr->index & 0xFF);
                if (port & 0x80)  {
                  if ((port & 0x7F)==0) {
                    printf(" Switch: Invalid lag reference (0)\r\n");
                    break;
                  }
                  port = (port & 0x7F)-1+SYSTEM_N_PORTS;
                }

                // Print configuration
                printf(" QoS configuration for port %u\r\n",port);
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
              printf(" Switch: Error setting general QoS configuration - error %08x\n\r", *(unsigned int*)resposta.info);
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
          case 1041:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
            {
              st_switch_mac_table *ptr;
              uint16 i;
              // Validate size
              if (resposta.infoDim!=sizeof(st_switch_mac_table)) {
                printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(st_switch_mac_table),resposta.infoDim);
                break;
              }

              // Pointer to element
              ptr = &(((st_switch_mac_table *) resposta.info)[i]);

              printf(" Reading page %llu of MAC table:\r\n",ptr->index);

              for (i=0; i<ptr->mac_table_msg_size; i++) {
                printf(" Id %-5u, ",ptr->mac_table[i].mac_index);
                if (ptr->mac_table[i].flow_id!=(uint16)-1)
                  printf("EVC %-4u, ",ptr->mac_table[i].flow_id);
                else
                  printf("No EVC  , ");
                printf("VlanId %-4u, ",ptr->mac_table[i].vlan_id);
                printf("Port %-2u, ",ptr->mac_table[i].port);
                printf("MAC %02X:%02X:%02X:%02X:%02X:%02X, ",
                       ptr->mac_table[i].mac_address[0],
                       ptr->mac_table[i].mac_address[1],
                       ptr->mac_table[i].mac_address[2],
                       ptr->mac_table[i].mac_address[3],
                       ptr->mac_table[i].mac_address[4],
                       ptr->mac_table[i].mac_address[5]);
                printf("%s type\r\n",((ptr->mac_table[i].static_address) ? "Static" : "Dynamic"));
              }
              printf(" Switch: MAC table read successfuly\n\r");
            }
            else
              printf(" Switch: Error reading MAC table - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1042:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: MAC address added successfully\n\r");
            else
              printf(" Switch: MAC address not added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1043:
          case 1044:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: MAC address removed successfully\n\r");
            else
              printf(" Switch: MAC address not removed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1100:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              uint16 port;
              uint8  lag_id;
              MSG_HwEthernetMef10EvcBundling *ptr = (MSG_HwEthernetMef10EvcBundling *) &resposta.info[0];

              printf(" EVCid=%u\n",(uint16) (ptr->index & 0xFFFF));
              printf(" Type=%u, MC_flood=%u, DHCPop82=%u\r\n",ptr->type, (uint16) (ptr->index>>57 & 1), (uint16) (ptr->index>>56 & 1));
              lag_id = (ptr->index>>32) & 0xFF;
              for (port=0; port<SYSTEM_N_PORTS; port++) {
                if (ptr->uni[port].type==MEF10_EVC_UNITYPE_UNUSED) continue;
                printf("\tPort=%2u\tType=%s\tSVid=%u\n",
                       port,
                       ((ptr->uni[port].type==MEF10_EVC_UNITYPE_LEAF) ? "Leaf" : "Root"),
                       ptr->uni[port].ceVid);
              }
              if (lag_id>0) {
                printf("\tPort:%02u\tType=%s\tSVid=%u\n",SYSTEM_N_PORTS+lag_id-1,"Root",ptr->outerTag);
              }
              printf(" Switch: Flow read successfully\n\r");
            } else
              printf(" Switch: Flow not read - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1101:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: Flow added successfully\n\r");
            else
              printf(" Switch: Flow not added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1102:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              printf(" Switch: Flow removed successfully\n\r");
            } else
              printf(" Switch: Flow not removed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;
            
          case 1200:
          {
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
            {
              uint16 i, n, flow_id, port, svid, cvid;
              MSG_HwEthernetProfile *ptr = (MSG_HwEthernetProfile *) &resposta.info[0];
              n = resposta.infoDim / sizeof(MSG_HwEthernetProfile);

              flow_id = (uint16) ((ptr->index >> 48) & 0xFFFF);
              port    = (uint16) ((ptr->index >>  0) & 0x3F  );
              svid    = (uint16) ((ptr->index >> 16) & 0xFFFF);
              printf(" EVCid=%u\n Port=%u\n SVid=%u\n",flow_id,port,svid);

              for (i=0; i<n; i++) {
                cvid = (uint16) ((ptr[i].index >> 32) & 0xFFFF);
                printf(" CVid=%-4u\n",cvid);
                printf("\tCIR=%llu bps\n",ptr[i].CIR);
                printf("\tEIR=%llu bps\n",ptr[i].EIR);
                printf("\tCBS=%llu bytes\n",ptr[i].CBS);
                printf("\tEBS=%llu bytes\n",ptr[i].EBS);
              }
              printf(" Switch: BW profile read successfully\n\r");
            }
            else
              printf(" Switch: BW profile not read - error %08x\n\r", *(unsigned int*)resposta.info);
          }
          break;

          case 1201:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: BW profile added successfully\n\r");
            else
              printf(" Switch: BW profile not added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;
            
          case 1202:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: BW profile removed successfully\n\r");
            else
              printf(" Switch: BW profile not removed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1210:
          {
            uint16 index, n_index;
            st_HWEth_VlanStatistics *po=(st_HWEth_VlanStatistics *) &resposta.info[0];

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              printf("Switch: Vlan counters read successfully\n\r");
              n_index = resposta.infoDim / sizeof(st_HWEth_VlanStatistics);
              if ( n_index>CLIENTS_MAX )  n_index=CLIENTS_MAX;
              printf(" FlowID=%-4u\r\n",po[0].FlowId);
              for ( index=0; index<n_index; index++ ) {
                printf(" CVlanID=%-4u\t\tCountNOTRedPackets=%-20llu   CountRedPackets=%-20llu\n\r",
                       po[index].stat.CVid,po[index].stat.stat.etherNotRedPackets,po[index].stat.stat.etherRedPackets);
              }
              printf("DONE!!!\n\r");
            }
            else
              printf(" Switch: Error reading Vlan counters\n\r");
          }
          break;

          case 1211:
          case 1212:
          {
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
            {
              st_ptin_flow_counters *ptr;

              if (resposta.infoDim!=sizeof(st_ptin_flow_counters)) {
                printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(st_ptin_flow_counters));
                break;
              }

              ptr = (st_ptin_flow_counters *) &resposta.info[0];

              printf(" Flow Counters:\r\n");
              printf(" FlowId=%u",(uint16) (ptr->index & 0xFFFF));
              if (ptr->flow_type==0)      printf(" FlowType=Unicast   client_vlan=%lu\r\n",ptr->c_id.client_vlan);
              else if (ptr->flow_type==1) printf(" FlowType=Multicast IpChannel=%lu.%lu.%lu.%lu\r\n",(ptr->c_id.channel_ip>>24) & 0xFF,(ptr->c_id.channel_ip>>16) & 0xFF,(ptr->c_id.channel_ip>>8) & 0xFF,ptr->c_id.channel_ip & 0xFF);
              else if (ptr->flow_type==2) printf(" FlowType=Broadcast\r\n");
              else                        printf(" FlowType=Unknown\r\n");
              printf(" Valid Counters? %s\r\n",((ptr->countersExist) ? "yes" : "no"));
              printf("            %20s %20s\r\n","PON ports","Eth ports");
              printf(" Total    : %20llu %20llu\r\n",ptr->stats.pon_rx.pktTotal    , ptr->stats.eth_rx.pktTotal    );
              printf(" Unicast  : %20llu %20llu\r\n",ptr->stats.pon_rx.pktUnicast  , ptr->stats.eth_rx.pktUnicast  );
              printf(" Multicast: %20llu %20llu\r\n",ptr->stats.pon_rx.pktMulticast, ptr->stats.eth_rx.pktMulticast);
              printf(" Broadcast: %20llu %20llu\r\n",ptr->stats.pon_rx.pktBroadcast, ptr->stats.eth_rx.pktBroadcast);
              printf(" Dropped  : %20llu %20llu\r\n",ptr->stats.pon_rx.pktDropped  , ptr->stats.eth_rx.pktDropped  );
              printf(" Switch: Flow counters read successfully\n\r");
            }
            else
              printf(" Switch: Flow counters not read - error %08x\n\r", *(unsigned int*)resposta.info);
            break;
          }
          break;

          case 1213:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: Flow counters added successfully\n\r");
            else
              printf(" Switch: Flow counters not added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1214:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: Flow counters removed successfully\n\r");
            else
              printf(" Switch: Flow counters not removed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1220:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
            {
              st_HwEthernetDhcpOpt82Profile *ptr;

              if (resposta.infoDim!=sizeof(st_HwEthernetDhcpOpt82Profile)) {
                printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(st_HwEthernetDhcpOpt82Profile));
                break;
              }

              ptr = (st_HwEthernetDhcpOpt82Profile *) &resposta.info[0];

              printf(" DHCP profile:\r\n");
              printf(" EVCid=%u\r\n",(uint16) ((ptr->index>>48) & 0xFFFF));
              printf(" Port =%u\r\n",(uint16) (ptr->index & 0x3F));
              printf(" CVid =%u\r\n",(uint16) ((ptr->index>>32) & 0xFFFF));
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

          case 1223:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: DHCPop82 Access Node Id updated successfully\n\r");
            else
              printf(" Switch: DHCPop82 Access Node Id not updated - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1240:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
            {
              st_DHCP_bind_table *ptr;
              uint16 i;
              // Validate size
              if (resposta.infoDim!=sizeof(st_DHCP_bind_table)) {
                printf(" Switch: Invalid structure size (expected=%u, received=%u bytes)\n\r",sizeof(st_DHCP_bind_table),resposta.infoDim);
                break;
              }

              // Pointer to element
              ptr = &(((st_DHCP_bind_table *) resposta.info)[i]);

              printf(" Reading page %llu of DHCP Binding table:\r\n",ptr->index);

              for (i=0; i<ptr->bind_table_msg_size; i++) {
                printf(" Id %-5u, ",ptr->bind_table[i].entry_index);
                if (ptr->bind_table[i].flow_id!=(uint16)-1)
                  printf("EVC %-4u, ",ptr->bind_table[i].flow_id);
                else
                  printf("No EVC  , ");
                printf("VlanId %-4u, ",ptr->bind_table[i].service_vlan);
                printf("CVlanId %-4u, ",ptr->bind_table[i].client_vlan);
                printf("Port %-2u, ",ptr->bind_table[i].port);
                printf("MAC %02X:%02X:%02X:%02X:%02X:%02X, ",
                       ptr->bind_table[i].macAddr[0],
                       ptr->bind_table[i].macAddr[1],
                       ptr->bind_table[i].macAddr[2],
                       ptr->bind_table[i].macAddr[3],
                       ptr->bind_table[i].macAddr[4],
                       ptr->bind_table[i].macAddr[5]);
                printf("IPAddr %03lu.%03lu.%03lu.%03lu, ",(ptr->bind_table[i].ipAddr>>24) & 0xFF,(ptr->bind_table[i].ipAddr>>16) & 0xFF,(ptr->bind_table[i].ipAddr>>8) & 0xFF,ptr->bind_table[i].ipAddr & 0xFF);
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

          case 1301:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: MC flows added successfully\n\r");
            else
              printf(" Switch: MC flows not added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1302:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: MC flows removed successfully\n\r");
            else
              printf(" Switch: MC flows not removed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1304:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: MC clients added successfully\n\r");
            else
              printf(" Switch: MC clients not added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1305:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: MC client removed successfully\n\r");
            else
              printf(" Switch: MC client not removed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1310:
          case 1311:
          {
            st_ClientIgmpStatistics *po=(st_ClientIgmpStatistics *) &resposta.info[0];
            uint64 tmp;

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              if (resposta.infoDim!=sizeof(st_ClientIgmpStatistics)) {
                printf(" Switch: Invalid structure size\r\n");
                break;
              }

              if (msg==1310) {
                printf( " IGMP interface statistics for EVC=%u, Port=%u:\n\r",(uint16) (po->index & 0xFFFF),po->port);
              }
              else  {
                printf( " IGMP client statistics for EVC=%u, cVlan=%u:\n\r",(uint16) (po->index & 0xFFFF),po->cvid);
              }
              printf( "   Active Groups  = %llu\r\n",po->stats.active_groups );
              printf( "   Active Clients = %llu\r\n",po->stats.active_clients);

              printf( "  ___________________________________________________________________________________________________ \r\n");
              printf( " | IGMP packets sent        : " );
              ((tmp=po->stats.igmp_sent                )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " | IGMP packets tx failed   : " );
              ((tmp=po->stats.igmp_tx_failed           )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|_________________________________________________|\r\n");

              printf( " | IGMP packets intercepted : " );
              ((tmp=po->stats.igmp_intercepted         )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " | IGMP packets dropped     : " );
              ((tmp=po->stats.igmp_dropped             )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|_________________________________________________|\r\n");

              printf( " | IGMP packets rx valid    : " );
              ((tmp=po->stats.igmp_received_valid      )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " | IGMP packets rx invalid  : " );
              ((tmp=po->stats.igmp_received_invalid    )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|_________________________________________________|\r\n");

              printf( " | IGMP Joins tx            : " );
              ((tmp=po->stats.joins_sent               )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|_________________________________________________ \r\n");

              printf( " | IGMP Joins rx success    : " );
              ((tmp=po->stats.joins_received_success   )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " | IGMP Joins rx failed     : " );
              ((tmp=po->stats.joins_received_failed    )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|_________________________________________________|\r\n");

              printf( " | IGMP Leaves tx           : " );
              ((tmp=po->stats.leaves_sent              )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|\r\n");

              printf( " | IGMP Leaves rx           : " );
              ((tmp=po->stats.leaves_received          )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|\r\n");

              printf( " | IGMP GeneralQueries tx   : " );
              ((tmp=po->stats.general_queries_sent     )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|\r\n");

              printf( " | IGMP GeneralQueries rx   : " );
              ((tmp=po->stats.general_queries_received )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|\r\n");

              printf( " | IGMP SpecificQueries tx  : " );
              ((tmp=po->stats.specific_queries_sent    )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|\r\n");

              printf( " | IGMP SpecificQueries rx  : ");
              ((tmp=po->stats.specific_queries_received)==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|\r\n");

              printf( " | IGMP MembershipReportV3  : ");
              ((tmp=po->stats.membership_report_v3     )==0)  ? printf("%20c",'-') : printf( "%20llu",tmp );
              printf( " |\r\n" );
              printf( " |_________________________________________________|\r\n");

              printf( "Done!\r\n");
            }
            else  {
              if (msg==1310)
                printf(" Switch: Error reading IGMP interface statistics\n\r");
              else
                printf(" Switch: Error reading IGMP client statistics\n\r");
            }
          }
          break;

          case 1315:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: Port IGMP statistics cleared successfully\n\r");
            else
              printf(" Switch: Port IGMP statistics not cleared - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1316:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: Client IGMP statistics cleared successfully\n\r");
            else
              printf(" Switch: Client IGMP statistics not cleared - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1320:
          case 1321:
          {
            st_ClientDhcpStatistics *po=(st_ClientDhcpStatistics *) &resposta.info[0];

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              if (resposta.infoDim!=sizeof(st_ClientDhcpStatistics)) {
                printf(" Switch: Invalid structure size\r\n");
                break;
              }

              if (msg==1320) {
                printf( " DHCP interface statistics for EVC=%u, Port=%u:\n\r",(uint16) (po->index & 0xFFFF),po->port);
              }
              else  {
                printf( " DHCP client statistics for EVC=%u, cVlan=%u:\n\r",(uint16) (po->index & 0xFFFF),po->cvid);
              }
              printf( "   Packets Intercepted  = %llu\r\n",po->stats.dhcp_rx_intercepted );
              printf( "   Packets Received     = %llu\r\n",po->stats.dhcp_rx );
              printf( "   Packets Filtered     = %llu\r\n",po->stats.dhcp_rx_filtered );
              printf( "   Packets Forwarded    = %llu\r\n",po->stats.dhcp_tx_forwarded );
              printf( "   Transmissions Failed = %llu\r\n",po->stats.dhcp_tx_failed );
              printf( "   Received Client Requests without Option82   = %llu\r\n",po->stats.dhcp_rx_client_requests_without_option82 );
              printf( "   Transmitted Client Requests with Option82   = %llu\r\n",po->stats.dhcp_tx_client_requests_with_option82 );
              printf( "   Received Server Replies with Option82       = %llu\r\n",po->stats.dhcp_rx_server_replies_with_option82 );
              printf( "   Transmitted Server Replies without Option82 = %llu\r\n",po->stats.dhcp_tx_server_replies_without_option82 );
              printf( "   Received Client Packets without Option82 on Trusted Interface = %llu\r\n",po->stats.dhcp_rx_client_pkts_withoutOp82_onTrustedIntf );
              printf( "   Received Client Packets with Option82 on Untrusted Interface  = %llu\r\n",po->stats.dhcp_rx_client_pkts_withOp82_onUntrustedIntf );
              printf( "   Received Server Packets with Option82 on Untrusted Interface  = %llu\r\n",po->stats.dhcp_rx_server_pkts_withOp82_onUntrustedIntf );
              printf( "   Received Server Packets without Option82 on Trusted Interface = %llu\r\n",po->stats.dhcp_rx_server_pkts_withoutOp82_onTrustedIntf );
              printf( "Done!\r\n");
            }
            else  {
              if (msg==1320)
                printf(" Switch: Error reading DHCP interface statistics\n\r");
              else
                printf(" Switch: Error reading DHCP client statistics\n\r");
            }
          }
          break;

          case 1325:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: Port DHCP statistics cleared successfully\n\r");
            else
              printf(" Switch: Port DHCP statistics not cleared - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1326:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: Client DHCP statistics cleared successfully\n\r");
            else
              printf(" Switch: Client DHCP statistics not cleared - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1400:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping/Querier configured\n\r");
            else
              printf(" Switch: IGMP Snooping/Querier NOT configured - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1402:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping: interfaces added\n\r");
            else
              printf(" Switch: IGMP Snooping: interfaces NOT added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1403:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping: interfaces removed\n\r");
            else
              printf(" Switch: IGMP Snooping: interfaces NOT removed - error %08x\n\r", *(unsigned int*)resposta.info);
           break;

          case 1405:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping: mclient vlans added\n\r");
            else
              printf(" Switch: IGMP Snooping: mclient vlans NOT added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1406:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping: mclient vlans removed\n\r");
            else
              printf(" Switch: IGMP Snooping: mclient vlans NOT removed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1408:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping: mrouter vlans added\n\r");
            else
              printf(" Switch: IGMP Snooping: mrouter vlans NOT added - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1409:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping: mrouter vlans removed\n\r");
            else
              printf(" Switch: IGMP Snooping: mrouter vlans NOT removed - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1411:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping Querier: vlans added to querier\n\r");
            else
              printf(" Switch: IGMP Snooping Querier: vlans NOT added to querier - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1412:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
              printf(" Switch: IGMP Snooping Querier: vlans removed to querier\n\r");
            else
              printf(" Switch: IGMP Snooping Querier: vlans NOT removed to querier - error %08x\n\r", *(unsigned int*)resposta.info);
            break;

          case 1420:
          {
            st_MCActiveChannels *po=(st_MCActiveChannels *) &resposta.info[0];
            uint16 index;

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              if (resposta.infoDim!=sizeof(st_MCActiveChannels)) {
                printf(" Switch: Invalid structure size\r\n");
                break;
              }
              printf( " MC channels for EVC=%u and C-Vid=%u (total=%u)\n\r",(uint16) (po->index & 0xFFFF),
                                                                            (uint16) ((po->index>>16) & 0xFFFF),
                                                                            po->n_channels_total);
              for (index=0; index<po->n_channels_msg; index++) {
                printf("  Channel %03lu.%03lu.%03lu.%03lu\r\n",(po->channelsIp_list[index]>>24) & 0xFF,
                                                               (po->channelsIp_list[index]>>16) & 0xFF,
                                                               (po->channelsIp_list[index]>> 8) & 0xFF,
                                                                po->channelsIp_list[index] & 0xFF);
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
            st_MCActiveChannelClients *po=(st_MCActiveChannelClients *) &resposta.info[0];
            uint32 tmp;
            uint16 i, j;

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              if (resposta.infoDim!=sizeof(st_MCActiveChannelClients)) {
                printf(" Switch: Invalid structure size\r\n");
                break;
              }
              printf( " MC clients for EVC=%u and channel %03lu.%03lu.%03lu.%03lu (total=%u)\n\r",(uint16) (po->index & 0xFFFF),
                      (po->channelIp>>24) & 0xFF, (po->channelIp>>16) & 0xFF, (po->channelIp>>8) & 0xFF, po->channelIp & 0xFF,
                       po->n_clients);
              for (i=0; i<128; i++) {
                tmp = po->clients_list_bmp[i];
                if (tmp==0)  continue;
                for (j=0; j<sizeof(uint32)*8; j++,tmp>>=1) {
                  if (!(tmp & 1))  continue;
                  printf("  Client %u\r\n",(uint16) i*32+j);
                }
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

          case 1500:
            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
            {  
              st_LACPLagInfo *ptr;
              uint8 nLags, i;

              nLags = resposta.infoDim/sizeof(st_LACPLagInfo);

              if (nLags==0) {
                printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(st_LACPLagInfo));
                break;
              }

              for (i=0; i<nLags; i++) {
                ptr = &((st_LACPLagInfo *) &resposta.info[0])[i];
                printf(" LAG port %u (LAGid=%u)\r\n",(uint16) (ptr->index+SYSTEM_N_PORTS-1),(uint16) ptr->index);
                printf("  Admin               = %s\r\n",((ptr->admin) ? "Enabled" : "Disabled"));
                printf("  STP state           = %s\r\n",((ptr->stp_enable) ? "Enabled" : "Disabled"));
                printf("  LAG type            = %s\r\n",((ptr->static_enable) ? "Static" : "Dynamic"));
                printf("  LoadBalance profile = %u\r\n",ptr->loadBalance_mode);
                printf("  Port bitmap         = 0x%08X\r\n",(unsigned int) ptr->members_pbmp);
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
              st_LACPLagStatus *ptr;
              uint8 nLags, i;

              nLags = resposta.infoDim/sizeof(st_LACPLagStatus);

              if (nLags==0) {
                printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(st_LACPLagStatus));
                break;
              }

              for (i=0; i<nLags; i++) {
                ptr = &((st_LACPLagStatus *) &resposta.info[0])[i];
                printf(" LAG port %u (LAGid=%u)\r\n",(uint16) (ptr->index+SYSTEM_N_PORTS-1),(uint16) ptr->index);
                printf("  Admin               = %s\r\n",((ptr->admin) ? "Enabled" : "Disabled"));
                printf("  Link State          = %s\r\n",((ptr->link_status) ? "UP" : "DOWN"));
                printf("  Port channel type   = %s\r\n",((ptr->port_channel_type) ? "Static" : "Dynamic"));
                printf("  Member Ports bitmap = 0x%08X\r\n",(unsigned int) ptr->members_pbmp);
                printf("  Active Ports bitmap = 0x%08X\r\n",(unsigned int) ptr->active_members_pbmp);
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
              st_LACPAdminState *ptr;
              uint8 nStructs, i;

              nStructs = resposta.infoDim/sizeof(st_LACPAdminState);

              if (nStructs==0) {
                printf(" Switch: Invalid structure size (%u vs %u)\n\r",resposta.infoDim,sizeof(st_LACPAdminState));
                break;
              }

              for (i=0; i<nStructs; i++) {
                ptr = &((st_LACPAdminState *) &resposta.info[0])[i];
                printf(" Physical port %u:\r\n",(uint16) ptr->index);
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
            st_LACPStats *po=(st_LACPStats *) &resposta.info[0];
            uint8 nStructs, i;

            if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
              if (resposta.infoDim==0 || (resposta.infoDim%sizeof(st_LACPStats))!=0) {
                printf(" Switch: Invalid structure size\r\n");
                break;
              }
              nStructs = resposta.infoDim/sizeof(st_LACPStats);

              for (i=0; i<nStructs; i++) {
                printf( " LACP statistics for port %u:\n\r",(uint16) (po[i].index & 0xFF));
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

          default:
            printf(" Resposta a mensagem %u\n\r",msg);
            break;
        }
  
        //printf("Fim do send BUGA to Controlo...\n\r");
        exit(0);
  }//if argc == m 
  else
  {
    help_oltBuga();
    exit(0);
  }
  return 0;
}


static int convert_ipaddr2uint32(const char *ipaddr, uint32 *value_uint32)
{
  const char *start_ipaddr;
  uint8  address[4] = { 0, 0, 0, 0 };
  uint8  index;
  uint32 multiplier;

  // Validate argument
  if (ipaddr==NULL || *ipaddr=='\0' || value_uint32==NULL)
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
  *value_uint32 = ((uint32) address[0]<<0) | ((uint32) address[1]<<8) | ((uint32) address[2]<<16) | ((uint32) address[3]<<24);

  return 0;
}

