/**
 * ptin_prot_oam_eth.c 
 *  
 * Adapting ptin_oam.[ch] to Fastpath
 *
 * @author
 *  
 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <logger.h>
#include <ipc.h>
#include <ptin_cnfgr.h>
#include <ptin_intf.h>
#include <ethsrv_oam.h>
#include <ptin_oam_packet.h>
#include <ptin_prot_oam_eth.h>
#include <ptin_packet.h>


#include "ptin_xlate_api.h"
#include "ptin_fpga_api.h"

#ifdef __Y1731_802_1ag_OAM_ETH__

/* *******************************************************************************/
/*                                  GLOBAL VARS                                  */
/* *******************************************************************************/

T_ETH_SRV_OAM oam;

#define PTIN_ETH_OAM_TIMER_EVENT    0

/* *******************************************************************************/
/*                                   FUNCTIONS                                   */
/* *******************************************************************************/










/* Protection alarms sent from linecard to MC control fw **********************************************************/
#define IPC_CHANNEL_UPLINKPROT            6005
#define TRAP_SW_ETH_UPLINKPROT_ALARMS  0x8008

//#define UPLINK_LC_MAX_PORTS 4

typedef enum
{
  ALARMS_CLEAR = 0x00,
  LC_FAILURE   = 0x01,
  PORT_LOS     = 0x02,
  MEP_LOC      = 0x04,
  MEP_RDI      = 0x08,
  PORT_SD      = 0x10,
  UNDEFINED3   = 0x20,
  UNDEFINED4   = 0x40,
  PON_NOT_SYNC = 0x80
} PROT_PortAlarms_t;

typedef struct {
  uint8       slotId,
              port,     //0..3
              alarm;    //PROT_PortAlarms_t
} __attribute__((packed)) msg_uplinkProtAlarms2mx_st;

typedef struct {
    unsigned char       active;
    PROT_PortAlarms_t   a;
    L7_uint64           vid;
} T_UPLINKPROT_TRAPS;
static T_UPLINKPROT_TRAPS uplinkprot_traps[PTIN_SYS_SLOTS_MAX][PTIN_SYS_INTFS_PER_SLOT_MAX];

static unsigned short int ptin_debug_oam = 0;
inline void ptin_debug_oam_set(unsigned short int enable){ptin_debug_oam = enable;};

void dump_uplinkprot_traps(void) {
L7_uint32 s, p;

 printf("DUMP_UPLINKPROT_TRAPS()\n\r");
 for (s=0; s<PTIN_SYS_SLOTS_MAX; s++)
     for (p=0; p<PTIN_SYS_INTFS_PER_SLOT_MAX; p++)
         if (uplinkprot_traps[s][p].active)
             printf("slot=%u\tport=%u\tvid=%llu\talarm=%x\n\r", s, p, uplinkprot_traps[s][p].vid, uplinkprot_traps[s][p].a);

  fflush(stdout);
}


int send_also_uplinkprot_traps(L7_uint8 set1_clr0_init2, L7_uint16 slot, L7_uint16 port, L7_uint64 vid) {
T_UPLINKPROT_TRAPS *p;

    if (slot>=PTIN_SYS_SLOTS_MAX) return 1;
    if (port>=PTIN_SYS_INTFS_PER_SLOT_MAX) return 2;

    p=&uplinkprot_traps[slot][port];

    switch (set1_clr0_init2) {
    default:
        p->active=0;
        break;
    case 1:
        p->active=1;
        break;
    case 0:
        //we may have several MEPs in this port, with different VIDs; only one to send these traps to
        if (p->active && p->vid!=vid) return 3;
        p->active=0;
        break;
    }
    p->a=ALARMS_CLEAR;
    p->vid=vid;
    return 0;
}




static inline void send_trap_ETH_OAM_uplinkprot(L7_uint32 ptin_port, PROT_PortAlarms_t alarm, PROT_PortAlarms_t msk, L7_uint64 vid) {
L7_uint16 slot, port;
msg_uplinkProtAlarms2mx_st v;
T_UPLINKPROT_TRAPS *p;

 if (L7_SUCCESS!=ptin_intf_port2SlotPort(ptin_port, &slot, &port, L7_NULLPTR)) return;

 p=&uplinkprot_traps[slot][port];

 if (!p->active) return;
 if (!p->vid!=vid) return;

 switch (msk) {
 default: return;
 case MEP_LOC:
     if (alarm) p->a |= MEP_LOC; else p->a &= ~MEP_LOC;
     break;
 case MEP_RDI:
     if (alarm) p->a |= MEP_RDI; else p->a &= ~MEP_RDI;
     break;
 }

 v.slotId=  slot;
 v.port=    port;
 v.alarm=   p->a;
 send_ipc_message(IPC_CHANNEL_UPLINKPROT, IPC_LOCALHOST_IPADDR, TRAP_SW_ETH_UPLINKPROT_ALARMS, (char *)&v, NULL, sizeof(v), NULL);
}





















//ETHERNET SERVICE OAM   ALIASES**********************************************************************
#define ME_CONNECTION_LOSS  0
#define ME_RDI              1
#define ME_RDI_END          9
#define ME_CONNECTION_UP    2
//#define UNEXP_MEG_OR_MEP    4
#define UNEXP_MEP           5
#define UNEXP_MEG           6
#define UNEXP_LVL           7
#define UNEXP_T             8

#define POTENTIAL_ETH_LOOP  0xff

typedef struct {
         //unsigned int      reserved;
         unsigned char     slot;
         unsigned char     integration;   // do not use
         short             indice;        // do not use
         int               timestamp;
         unsigned short    alrm_code;
         unsigned short    port;
         unsigned short    vid;
         unsigned short    mep_id;
         unsigned short    rmep_id;
         unsigned char     meg_id_prefix[16];
} __attribute__ ((packed)) st_RegEthOAMTrap;

static inline void ethsrv_oam_register(unsigned short alarm, unsigned short rmep_id, T_MEG_ID *meg_id, L7_uint16 mep_id, /*L7_uint16 mep_indx,*/ L7_uint16 porta, L7_uint64 vid) {
st_RegEthOAMTrap v;

    //v.slot=         
    v.timestamp=    time(NULL);
    v.alrm_code=    alarm;    
    v.port=         porta;
    v.vid=          vid;
    v.mep_id=       mep_id;
    v.rmep_id=      rmep_id;
    if (NULL!=meg_id) memcpy(v.meg_id_prefix, meg_id, sizeof(v.meg_id_prefix));
    else            v.meg_id_prefix[0]=0;
    send_trap_ETH_OAM(&v, sizeof(v));
    //printf("TRAP %u"NLS, alarm);
}
void ethsrv_oam_register_mismerge(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {
    ethsrv_oam_register(UNEXP_MEG, 0xffff, meg_id, mep_id, porta, vid);
    LOG_TRACE(LOG_CTX_OAM,"UNEXP_MEG imep=%u mep_id=%u port=%u vid=%llu", mep_indx, mep_id, porta, vid);
}
void ethsrv_oam_register_LVL(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid, L7_uint8 level) {
    ethsrv_oam_register(UNEXP_LVL, 0xffff, meg_id, mep_id, porta, vid);
    LOG_TRACE(LOG_CTX_OAM,"UNEXP_LVL imep=%u mep_id=%u port=%u vid=%llu", mep_indx, mep_id, porta, vid);
}
void ethsrv_oam_register_T(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid, L7_uint8 period) {
    ethsrv_oam_register(UNEXP_T, 0xffff, meg_id, mep_id, porta, vid);
    LOG_TRACE(LOG_CTX_OAM,"UNEXP_T imep=%u mep_id=%u port=%u vid=%llu", mep_indx, mep_id, porta, vid);
}
void ethsrv_oam_register_unexpected_MEP_potential_loop(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {}
void ethsrv_oam_register_unexpected_MEP_id(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {
    ethsrv_oam_register(UNEXP_MEP, 0xffff, meg_id, mep_id, porta, vid);
    LOG_TRACE(LOG_CTX_OAM,"UNEXP_MEP imep=%u mep_id=%u port=%u vid=%llu", mep_indx, mep_id, porta, vid);
}
void ethsrv_oam_register_connection_restored(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {
    //send_trap_ETH_OAM_uplinkprot(port, 0, MEP_LOC, vid);
    ethsrv_oam_register(ME_CONNECTION_UP, rmep_id, (T_MEG_ID*) meg_id, mep_id, port, vid);
    LOG_TRACE(LOG_CTX_OAM,"ME_CONNECTION_UP imep=? mep_id=%u rmep_id=%u port=%u vid=%llu", mep_id, rmep_id, port, vid);
}
void ethsrv_oam_register_receiving_RDI(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {
    //send_trap_ETH_OAM_uplinkprot(port, MEP_RDI, MEP_RDI, vid);
    ethsrv_oam_register(ME_RDI, rmep_id, (T_MEG_ID*) meg_id, mep_id, port, vid);
    LOG_TRACE(LOG_CTX_OAM,"ME_RDI imep=? mep_id=%u rmep_id=%u port=%u vid=%llu", mep_id, rmep_id, port, vid);
}
void ethsrv_oam_register_RDI_END(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {
    //send_trap_ETH_OAM_uplinkprot(port, 0, MEP_RDI, vid);
    ethsrv_oam_register(ME_RDI_END, rmep_id, (T_MEG_ID*) meg_id, mep_id, port, vid);
    LOG_TRACE(LOG_CTX_OAM,"ME_RDI_END imep=? mep_id=%u rmep_id=%u port=%u vid=%llu", mep_id, rmep_id, port, vid);
}
void ethsrv_oam_register_connection_loss(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {
    //send_trap_ETH_OAM_uplinkprot(port, MEP_LOC, MEP_LOC, vid);
    ethsrv_oam_register(ME_CONNECTION_LOSS, rmep_id, (T_MEG_ID*) meg_id, mep_id, port, vid);
    LOG_TRACE(LOG_CTX_OAM,"ME_CONNECTION_LOSS imep=? mep_id=%u rmep_id=%u port=%u vid=%llu", mep_id, rmep_id, port, vid);
}




#define RAW_MODE

#ifdef RAW_MODE
    #define DUMMY_VID       4095
    #define DUMMY_VLAN_LEN  4
#else
    #define DUMMY_VLAN_LEN  0
#endif
int send_eth_pckt(L7_uint16 port, L7_uint8 up1_down0,
                  L7_uint8 *buf, L7_ulong32 length, //ETH client buffer and length
                  L7_uint64 vid, L7_uint8 prior, L7_uint8 CoS, L7_uint8 color, L7_uint16 ETHtype, L7_uint8 *pDMAC) {
    L7_INTF_TYPES_t   sysIntfType;
    L7_uint32         intIfNum;//=port+1;
	L7_uint32         activeState;  
#ifdef RAW_MODE
    L7_netBufHandle   bufHandle;
    L7_uchar8        *dataStart;
    DTL_CMD_TX_INFO_t dtlCmd;
#else
    L7_uint16         vidInternal;
#endif
    L7_uint8 buff[1600];//={1,0,0,0,2,3,   0,1,2,3,4,5,    0x88,9,   3,  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,};
    //length=70;

#if PTIN_BOARD_IS_MATRIX  
  /* Do nothing for slave matrix */
  if (!ptin_fpga_mx_is_matrixactive_rt())
  {
    if (ptin_debug_oam)
      LOG_DEBUG(LOG_CTX_OAM,"Silently ignoring packet transmission. I'm a Slave Matrix");
    return L7_SUCCESS;
  }
#endif

    ptin_intf_port2intIfNum(port, &intIfNum);

    // If outgoing interface is CPU interface, don't send it
    if ( (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
         (sysIntfType == L7_CPU_INTF) )     return 1;

//Ignore if the port has link down
    if ( (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS) || (activeState != L7_ACTIVE) )
    {
      if (ptin_debug_oam)
        LOG_DEBUG(LOG_CTX_OAM,"Silently ignoring packet transmission. Outgoing interface [intIfNum=%u] is down!",intIfNum);    
      return L7_SUCCESS;
    }

#ifdef PTIN_ENABLE_ERPS
    if (OAM_ETH_TYPE==ETHtype    &&    dont_txrx_oam_criterion(3, port, -1, vid, NULL)) return 4;
#endif

#ifdef RAW_MODE
    SYSAPI_NET_MBUF_GET(bufHandle);
    if (bufHandle == L7_NULL) {
      LOG_WARNING(LOG_CTX_OAM,"send_eth_pckt: System out of netbuffs");
      return 2;
    }
#endif

    if (NULL==pDMAC || NULL==buf) return 3;

    memcpy(buff, pDMAC, 6);
    nimGetIntfAddress(intIfNum, L7_SYSMAC_BIA, &buff[6]);   //memcpy(buff, pSMAC, 6)
    if (vid<=4095) {
        buff[12+DUMMY_VLAN_LEN]=0x81;
        buff[13+DUMMY_VLAN_LEN]=0x00;
        buff[14+DUMMY_VLAN_LEN]=prior<<5 | vid>>8;
        buff[15+DUMMY_VLAN_LEN]=vid;
        buff[16+DUMMY_VLAN_LEN]=ETHtype>>8;
        buff[17+DUMMY_VLAN_LEN]=ETHtype;
        memcpy(&buff[18+DUMMY_VLAN_LEN], buf, length);
        length+=18;   
    }
    else {
        buff[12+DUMMY_VLAN_LEN]=ETHtype>>8;
        buff[13+DUMMY_VLAN_LEN]=ETHtype;
        memcpy(&buff[14+DUMMY_VLAN_LEN], buf, length);
        length+=14;
    }

    if (length<64) {
        memset(&buff[length], 0, 64-length);
        length=64;            
    }

#ifndef RAW_MODE
    // Convert to internal VLAN ID
    if (ptin_xlate_ingress_get( intIfNum, vid, PTIN_XLATE_NOT_DEFINED, &vidInternal, L7_NULLPTR ) != L7_SUCCESS)
    {
      vidInternal = 0;
    }

    ptin_packet_send(intIfNum, vidInternal, vid, buff, length);
#else
    buff[12]=0x81;
    buff[13]=0x00;
    buff[14]=prior<<5 | DUMMY_VID>>8;
    buff[15]=(L7_uint8)DUMMY_VID;
    length+=DUMMY_VLAN_LEN;

    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
    memcpy(dataStart, buff, length);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

    {
        //DTL_CMD_TX_INFO_t  dtlCmd;

        memset((L7_uchar8 *)&dtlCmd, 0, sizeof(DTL_CMD_TX_INFO_t));

        dtlCmd.priority            = 1;
        dtlCmd.typeToSend          = DTL_L2RAW_UNICAST;
        dtlCmd.intfNum             = intIfNum;
        dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);
    }
#endif
    //printf("send_eth_pckt() port=%u\n\r", port);
    return 0;
}//send_eth_pckt








void tst_send(void) {
    {
        L7_uint8 buf[100]={1,0,0,0,2,3,   0,1,2,3,4,5,    0x88,9,   3,  00, 32,  
                           16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,};
        //length=70;
        send_eth_pckt(10, 0, &buf[15], 70, 
                      500,
                      0,
                      0,
                      0,
                      0x8100,
                      (L7_uint8*)OAM_MC_MAC.byte);
    }
}


















int MEP_is_in_LOC(L7_ulong32 i_mep, L7_ulong32 i_rmep, T_ETH_SRV_OAM *p) {
    if (i_mep>=N_MEPs) return 0;

    if (i_rmep<N_MAX_MEs_PER_MEP) return 0L-1==p->db[i_mep].mep.ME[i_rmep].LOC_timer;

    for (i_rmep=0; i_rmep<N_MAX_MEs_PER_MEP; i_rmep++) {
        if (p->db[i_mep].mep.ME[i_rmep].mep_id > HIGHEST_MEP) continue;
        if (0L-1==p->db[i_mep].mep.ME[i_rmep].LOC_timer) return 1;
    }

    return 0;
}










int MEP_is_in_RDI(L7_ulong32 i_mep, L7_ulong32 i_rmep, T_ETH_SRV_OAM *p) {
    if (i_mep>=N_MEPs) return 0;

    if (i_rmep<N_MAX_MEs_PER_MEP) return p->db[i_mep].mep.ME[i_rmep].RDI?1:0;

    for (i_rmep=0; i_rmep<N_MAX_MEs_PER_MEP; i_rmep++) {
        if (p->db[i_mep].mep.ME[i_rmep].mep_id > HIGHEST_MEP) continue;
        if (p->db[i_mep].mep.ME[i_rmep].RDI) return 1;
    }

    return 0;
}









/****************************************************************************** 
 * Task Init
 ******************************************************************************/

L7_uint32 oam_eth_TaskId = L7_ERROR;

static osapiTimerDescr_t   *ptin_eth_oamTimer             = L7_NULLPTR;













void ptin_eth_oamTimerCallback(void)
{
  ptin_CCM_PDU_Msg_t msg;
  L7_RC_t rc;

  if (L7_NULLPTR==ptin_ccm_packetRx_queue) return;

  /* process event on our thread */
  memset((void *)&msg, 0, PTIN_CCM_PDU_MSG_SIZE);
  msg.msgId = PTIN_ETH_OAM_TIMER_EVENT;

  rc = osapiMessageSend(ptin_ccm_packetRx_queue, &msg, PTIN_CCM_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
}


















/**
 * Task with infinite loop
 */
void ptin_oam_eth_task(void)
{
  LOG_TRACE(LOG_CTX_OAM,"OAM ETH Task started");

  if (osapiTaskInitDone(L7_PTIN_OAM_ETH_TASK_SYNC)!=L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_OAM, "Error syncing task");
    PTIN_CRASH();
  }

  /* Infinite Loop */
  while (1) {
  L7_uint32 status;
  ptin_CCM_PDU_Msg_t msg;

      status = (L7_uint32) osapiMessageReceive(ptin_ccm_packetRx_queue,
                                               (void*) &msg,
                                               PTIN_CCM_PDU_MSG_SIZE,
                                               L7_WAIT_FOREVER);

      if (L7_SUCCESS!=status) LOG_ERR(LOG_CTX_OAM,"Failed packet reception from ptin_ccm_packet queue (status = %d)",status);

      switch (msg.msgId) {
      case PTIN_CCM_PACKET_MESSAGE_ID:  //CCM Rx
          //LOG_INFO(LOG_CTX_OAM,"ETH OAM packet received OK");
          {
           L7_uint32 i, ptin_port;//, vid;
           L7_uint16 newOuterVlanId;           

           if (ptin_xlate_egress_get( msg.intIfNum, msg.vlanId, PTIN_XLATE_NOT_DEFINED, &newOuterVlanId, L7_NULLPTR ) != L7_SUCCESS)
           {
             newOuterVlanId = 0;
           }

           if (L7_SUCCESS!=ptin_intf_intIfNum2port(msg.intIfNum, &ptin_port)) {LOG_INFO(LOG_CTX_OAM,"but in invalid port"); break;}
           //for (i=0; i<msg.payloadLen; i++) printf(" %2.2x", msg.payload[i]);      printf("\n\r");
           for (i=2*L7_MAC_ADDR_LEN/*, vid=-1*/; i<msg.payloadLen; i+=4) {
               switch (msg.payload[i]<<8 | msg.payload[i+1]) {//ETHtype
               case 0x8100:
               case 0x88a8:
                   //if (vid>4095) vid=(msg.payload[i+2]<<8 | msg.payload[i+3])&0xfff;
                   break;
               case OAM_ETH_TYPE:
                   {
                    int r;

                    if ((r=rx_oam_pckt(ptin_port, &msg.payload[i], msg.payloadLen-i, /*msg.vlanId*/ newOuterVlanId, &msg.payload[L7_MAC_ADDR_LEN], &oam, msg.timestamp)))
                        LOG_INFO(LOG_CTX_OAM,"rx_oam_pckt()==%d", r);
                   }
                   goto _ptin_oam_eth_task1;
               default:
                   LOG_INFO(LOG_CTX_OAM,"but unexpected ETH type");
                   goto _ptin_oam_eth_task1;
               }//switch
           }//for
_ptin_oam_eth_task1:;
          }
          break;
      case PTIN_ETH_OAM_TIMER_EVENT:    //Timer
          proc_ethsrv_oam(&oam, 10);
          osapiTimerAdd((void *)ptin_eth_oamTimerCallback, L7_NULL, L7_NULL, 10, &ptin_eth_oamTimer);
          break;
      default:
          LOG_INFO(LOG_CTX_OAM,"ETH OAM packet received NOK");
      }//switch

  }//while (1)



  //ptin_ccm_packet_vlan_trap(301, 7, 0);
  ptin_ccm_packet_deinit(0xffff);
}//ptin_oam_eth_task













L7_RC_t ptin_ccm_packet_trap(L7_uint16 prt, L7_uint16 vlanId, L7_uint16 oam_level, L7_BOOL enable) {
L7_uint32 intIfNum;
L7_uint16 vidInternal;

    if (oam_level>=N_OAM_LEVELS
        ||
        L7_SUCCESS!=ptin_intf_port2intIfNum(prt, &intIfNum)
        ||
        L7_SUCCESS!=ptin_xlate_ingress_get( intIfNum, vlanId, PTIN_XLATE_NOT_DEFINED, &vidInternal, L7_NULLPTR)) return L7_FAILURE;

    if (enable) {
#ifndef COMMON_APS_CCM_CALLBACKS__ETYPE_REG
        if (L7_SUCCESS!=ptin_ccm_packet_init(oam_level)) return L7_FAILURE;
#endif
        if (L7_SUCCESS!=ptin_ccm_packet_vlan_trap(vidInternal, oam_level, 1)) {
            ptin_ccm_packet_deinit(oam_level);
            return L7_FAILURE;
        }
    }
    else {
        ptin_ccm_packet_deinit(oam_level);
        ptin_ccm_packet_vlan_trap(vidInternal, oam_level, 0);
    }

    return L7_SUCCESS;
}




void eth_srv_oam_msg_defaults_reset(void) {
L7_uint32 i;

    for (i=0; i<N_MEPs; i++) {
        if (EMPTY_T_MEP(oam.db[i].mep)) continue;
        ptin_ccm_packet_trap(oam.db[i].mep.prt, oam.db[i].mep.vid, oam.db[i].mep.level, 0);
    }
    init_eth_srv_oam(&oam);
    {
     unsigned long j;
     for (i=0; i<PTIN_SYS_SLOTS_MAX; i++)
         for (j=0; j<PTIN_SYS_INTFS_PER_SLOT_MAX; j++) send_also_uplinkprot_traps(2,i,j,0ULL-1);
    }
}























/**
 * Initializes OAM ETH module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_oam_eth_init(void)
{

  init_eth_srv_oam(&oam);
  {
   unsigned long i, j;
   for (i=0; i<PTIN_SYS_SLOTS_MAX; i++)
       for (j=0; j<PTIN_SYS_INTFS_PER_SLOT_MAX; j++) send_also_uplinkprot_traps(2,i,j,0ULL-1);
  }

  ptin_ccm_packet_init(0xffff);

  osapiTimerAdd((void *)ptin_eth_oamTimerCallback, L7_NULL, L7_NULL, 10, &ptin_eth_oamTimer);

  LOG_TRACE(LOG_CTX_OAM,"OAM ETH task ready");


  oam_eth_TaskId = osapiTaskCreate("ptin_oam_eth_task", ptin_oam_eth_task, 0, 0,
                                L7_DEFAULT_STACK_SIZE,
                                9,
                                0);

  if (oam_eth_TaskId == L7_ERROR) {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Could not create task ptin_oam_eth_task");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Task ptin_oam_eth_task created");

  if (osapiWaitForTaskInit (L7_PTIN_OAM_ETH_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to initialize ptin_oam_eth_task()\n");
    return(L7_FAILURE);
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Task ptin_oam_eth_task initialized");

  return L7_SUCCESS;
}//ptin_oam_eth_init





















#ifndef PTIN_ENABLE_ERPS
int dont_txrx_oam_criterion(unsigned char init0_setbmp1_clrbmp2_rd3, unsigned short prt, unsigned short iMEP,
                            unsigned short vid,      //just on read
                            unsigned char *vid_bmp   //always but on read
                               ) {
 return 0;
}
#else
/************************************************************************************************************* 
  Cisco needs CCMs belonging to OLT-BNG control plane MEPs not to pass over ERPs' RPLs.
  This is the fix
*************************************************************************************************************/
#include <ptin_prot_erps.h>
typedef struct {
    unsigned char vid_bmp[(1<<12)/(sizeof(unsigned char)*8)];  //For each port, the relevant VIDs
    unsigned short iMEP;            //For each port, the MEP used in ERP (none, if >=N_MEPs)...
    unsigned short vidMEP;          //...and its VID
} dont_txrx_oam_criterion_t;




int dont_txrx_oam_criterion(unsigned char init0_setbmp1_clrbmp2_rd3, unsigned short prt, unsigned short iMEP,
                            unsigned short vid,      //just on read
                            unsigned char *vid_bmp   //always but on read
                               ) {
unsigned long i;
#if 0
//PLAN B
static
dont_txrx_oam_criterion_t db[PTIN_SYSTEM_N_PORTS];
dont_txrx_oam_criterion_t *p;


 switch (init0_setbmp1_clrbmp2_rd3) {
 case 0:    //INIT
     if (prt>=PTIN_SYSTEM_N_PORTS) {        //undefined port resets all DB
         for (i=0; i<PTIN_SYSTEM_N_PORTS; i++) {
             p= &db[i];
             p->iMEP=   -1;
             p->vidMEP= -1;
             memset(p->vid_bmp, 0, sizeof(p->vid_bmp));
         }
         return 0;
     }

     p= &db[prt];
     if (NULL==vid_bmp) memset(p->vid_bmp, 0, sizeof(p->vid_bmp));  //undefined vid_bmp resets bit map
     else
     for (i=0; i<sizeof(p->vid_bmp); i++) p->vid_bmp[i] &= ~vid_bmp[i]; //defined vid_bmp resets just VIDs that are 1

     if (iMEP>=N_MEPs) {        //undefined iMEP
         p->iMEP=     -1;
         p->vidMEP=   -1;
     }

     return 0;
 case 1:    //SET
 case 2:    //CLR
     if (NULL==vid_bmp) return 1;
     if (prt>=PTIN_SYSTEM_N_PORTS) return 2;
     p= &db[prt];

     if (1==init0_setbmp1_clrbmp2_rd3)  for (i=0; i<sizeof(p->vid_bmp); i++) p->vid_bmp[i] |= vid_bmp[i];
     else                               for (i=0; i<sizeof(p->vid_bmp); i++) p->vid_bmp[i] &= ~vid_bmp[i];

     if (iMEP>=N_MEPs);
     else {
         p= &db[prt];
         p->iMEP=iMEP;
         p->vidMEP=oam.mep_db[iMEP].vid;
     }
     return 0;
 case 3:    //RD
     if (prt>=PTIN_SYSTEM_N_PORTS) return 0;
     p= &db[prt];
     if (vid==p->vidMEP) return 0;
     return p->vid_bmp[vid/8] & 1<<(vid%8)?    1:0;
 }//switch
#else
//PLAN A
//Only prt and vid parameters are valid
 for (i=0; i<MAX_PROT_PROT_ERPS; i++) {
     if (!tbl_erps[i].admin) continue;

     if (prt==tbl_erps[i].protParam.port0.idx) {
         iMEP=  tbl_erps[i].protParam.port0CfmIdx;  //not using "iMEP" parameter
         if (iMEP<N_MEPs && !EMPTY_T_MEP(oam.db[iMEP].mep) && vid==oam.db[iMEP].mep.vid) return 0;  //sub-ERP-MEP
         if (vid==tbl_erps[i].protParam.controlVid) return 0;   //ERP APS VID
         if (vid < 1<<12    &&    tbl_erps[i].protParam.vid_bmp[vid/8] & 1<<(vid%8)) {              //ERP protected VID
             if (ERPS_PORT_BLOCKING==tbl_erps[i].portState[0]) return 1;                            //RPL position
             else return 0;
         }
     }
     else
     if (prt==tbl_erps[i].protParam.port1.idx) {
         iMEP=  tbl_erps[i].protParam.port1CfmIdx;  //not using "iMEP" parameter
         if (iMEP<N_MEPs && !EMPTY_T_MEP(oam.db[iMEP].mep) && vid==oam.db[iMEP].mep.vid) return 0;  //sub-ERP-MEP
         if (vid==tbl_erps[i].protParam.controlVid) return 0;   //ERP APS VID
         if (vid < 1<<12    &&    tbl_erps[i].protParam.vid_bmp[vid/8] & 1<<(vid%8)) {              //ERP protected VID
             if (ERPS_PORT_BLOCKING==tbl_erps[i].portState[1]) return 1;                            //RPL position
             else return 0;
         }
     }
 }//for
#endif
 return 0;
}//dont_txrx_oam_criterion_DB
#endif //ifndef PTIN_ENABLE_ERPS
#else
void ethsrv_oam_register_mismerge(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {}
void ethsrv_oam_register_LVL(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid, L7_uint8 level) {}
void ethsrv_oam_register_T(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid, L7_uint8 period) {}
void ethsrv_oam_register_unexpected_MEP_potential_loop(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {}
void ethsrv_oam_register_unexpected_MEP_id(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {}
void ethsrv_oam_register_connection_restored(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {}
void ethsrv_oam_register_receiving_RDI(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {}
void ethsrv_oam_register_RDI_END(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {}
void ethsrv_oam_register_connection_loss(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {}

int send_eth_pckt(L7_uint16 port, L7_uint8 up1_down0,
                  L7_uint8 *buf, L7_ulong32 length, //ETH client buffer and length
                  L7_uint64 vid, L7_uint8 prior, L7_uint8 CoS, L7_uint8 color, L7_uint16 ETHtype, L7_uint8 *pDMAC) {return 0;}

L7_RC_t ptin_oam_eth_init(void) {return L7_SUCCESS;}
#endif //__Y1731_802_1ag_OAM_ETH__












u64 rd_TxFCl(u16 i_mep) {return 0;}  //No need to do anything; BCM HW fills this field

u64 rd_TxTimeStampb(u16 i_mep) {return 0;}  //Need to check what to do; BCM HW should fill this field
u64 rd_TxTimeStampf(u16 i_mep) {return 0;}  //Need to check what to do; BCM HW should fill this field


