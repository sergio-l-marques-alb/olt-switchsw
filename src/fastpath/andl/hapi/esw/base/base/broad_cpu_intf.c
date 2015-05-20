/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_cpu_intf.c
*
* @purpose   This file contains the interface to the CPU
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#include <string.h>
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l7_common.h"
#include "broad_common.h"
#include "broad_stats.h"
#include "broad_l3.h"
#include "broad_l2_std.h"
#include "broad_l2_vlan.h"
#include "broad_l2_lag.h"
#include "broad_system.h"

#include "dot1s_exports.h"

#ifdef L7_ISCSI_PACKAGE
#include "broad_iscsi.h"
#endif /* L7_ISCSI_PACKAGE */

#include "unitmgr_api.h"
#include "sysnet_api.h"

#include <bcmx/tx.h>
#include <bcmx/lport.h>
#include <bcmx/port.h>    /* PTin added: link up status */

#include "bcm_int/esw/mbcm.h"
#include "dapi_trace.h"
#include "log.h"
#include "appl/stktask/topo_brd.h"
#include "sysbrds.h"

#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#include "wireless_defaultconfig.h"
#endif
#ifdef L7_STACKING_PACKAGE
#include "l7_usl_macsync.h"
#endif
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
#include "dot1ad_l2tp_api.h"
#include "broad_dot1ad.h"
extern fNimSetIntfAdminState nimSetIntfAdminStatePtr;
#endif
#endif

/* PTin added: IGMP snooping */
#if 1
#include "ptin_hapi.h"
#include "ptin_structs.h"
#include "logger.h"

extern L7_uint64 hapiBroadReceive_packets_count;

extern ptin_debug_pktTimer_t debug_pktTimer;
#endif

#include "ipc.h"

extern DAPI_t *dapi_g;
extern L7_ushort16 hapiBroadDvlanEthertype;

L7_RC_t hapiBroadRxLegacy(BROAD_PKT_RX_MSG_t *pktRxMsg,L7_BOOL *dropFrame, bcm_chip_family_t family, DAPI_t *dapi_g);
L7_BOOL hapiBroadXgsRxCheck(BROAD_PKT_RX_MSG_t *pktRxMsg, DAPI_t *dapi_g);
L7_BOOL hapiBroadMacDaCheck(BROAD_PKT_RX_MSG_t *pktRxMsg, bcm_chip_family_t family, DAPI_t *dapi_g);
L7_RC_t hapiBroadRxXgs3(BROAD_PKT_RX_MSG_t *pktRxMsg,L7_BOOL *fwdFrame,DAPI_t *dapi_g);
L7_BOOL hapiBroadXgs3RxCheck(BROAD_PKT_RX_MSG_t *pktRxMsg, DAPI_t *dapi_g);
L7_BOOL hapiBroadRxDuplicatePkt(bcm_pkt_t *bcm_pkt);

void hapiBroadRxTtlFixup(L7_uchar8 *pkt);
static void hapiBroadSTagRemove(L7_uchar8 *pkt, L7_uint32 *frameLen);
static void hapiBroadInnerTagRemove(L7_uchar8 *pkt, L7_uint32 *frameLen);

void *hapiTxBpduQueue;
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
void *hapiTxPduQueue;
void hapiBroadTagRemove(L7_uchar8 *pkt, L7_uint32 *frameLen);
#endif
#endif
void *hapiRxQueue;
static L7_BOOL hapiBroadProbeActive(DAPI_t *dapi_g); 
static void hapiBroadRxReasonCodeSet(BROAD_PKT_RX_MSG_t *pktRxMsg);

/* 
** Table to get the egress cos for transmission of packets. 
** Based on the platform, the table is setup in hapiBroadCpuTxRxInit routine.
** XGS3 family uses two cos levels for transmission, whereas XGS2 uses only one.
*/
static L7_uint32 hapiBroadTxCosTable[BROAD_TX_PRIO_LAST] = \
       {HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS,  \
        HAPI_BROAD_EGRESS_NORMAL_PRIORITY_COS};

static mac_addr_t default_mac_dst ={ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15 };
/* Enable this flag to - process PDUs only when a matching subscription on received interface exists. 
   Disable this flag to - process PDUs when atleast one subscription exists on the received interface.
*/

#define DOT1AD_L2PT_ALLOW_EXACT_MATCH_SUBCRIPTION
/* comment out this line to remove packet debug capability */
#define HAPI_BROAD_PKT_DEBUG

#ifdef HAPI_BROAD_PKT_DEBUG
static int     pktTxDebug             = 0;
static int     pktRxDebug             = 0;
static int     pktRxCallbackDebug     = 0;
static int     pktTxDebugLevel        = 0;
static int     pktRxDebugLevel        = 0;
static int     pktRxDebugQueue        = 0;
static int     pktDebugFilter         = 0;
static L7_BOOL pktDebugFilterModeDeny = L7_TRUE;
static int     voiceVlanAuthDebug     = 0;
static int     wlanVpDebugRx          = 0;
static int     wlanVpDebugTx          = 0;
static int     wlanVpDebugSize        = 0;
static int     wlanVpDebugEncap       = 0;
static int     dot1adVlanInfoDebug;

/* This is for the DebugPktSend test cases -- see broad_debug.c */
extern unsigned int hapiBroadDebugReceiveCounter;
extern int hapiBroadDebugPktReceiveEnabled;

#if defined(FEAT_METRO_CPE_V1_0)
static L7_BOOL isMgmtTrafficTagged = 1;
#endif

static char *pkt_reason_names[bcmRxReasonCount] = _SHR_RX_REASON_NAMES_INITIALIZER;

typedef struct {
  unsigned int pktRxCosRecv;
  unsigned int pktRxCosDrops;
} PKT_RX_COS_STATS_t;

static PKT_RX_COS_STATS_t hapiRxDebugCosCounters[8];
static L7_uint32          hapiTxDebugCounters[DAPI_NUM_OF_FRAME_TYPES];
/* Number of errors in uport to lport conversion on packet receive.
 * Shown as part of rxStats
 */
static L7_uint32          hapiLportToUportConvertErrors = 0;

// PTin added: intercept
int cpu_intercept_debug = 0;
int cpu_transmit_debug  = 0;

typedef enum
{
   CPU_INTERCEPT_DEBUG_STDOUT = 0x01,
   CPU_INTERCEPT_DEBUG_LEVEL1 = 0x10,
   CPU_INTERCEPT_DEBUG_LEVEL2 = 0x20,
   CPU_INTERCEPT_DEBUG_LEVEL3 = 0x40,
   CPU_INTERCEPT_DEBUG_LEVEL4 = 0x80
} CPU_INTERCEPT_DEBUG_LEVELS;

void cpu_intercept_debug_enable(int enable)
{
  cpu_intercept_debug = enable;
}
void cpu_tx_debug_enable(int enable)
{
  cpu_transmit_debug = enable;
}

int cpu_intercepted_packets_dump = 0;
int cpu_transmited_packets_dump = 0;

void ptin_debug_trap_packets_dump(int dumpNbytes)
{
  if (dumpNbytes)
  {
    cpu_intercepted_packets_dump = (dumpNbytes < 64) ? 64 : dumpNbytes; 
  }
  else
  {
    cpu_intercepted_packets_dump = 0;
  }
}
void ptin_debug_tx_packets_dump(int enable)
{
  cpu_transmited_packets_dump = enable;
}

// PTin end

#if defined(FEAT_METRO_CPE_V1_0)
void enableTaggedMgmtTraffic (L7_BOOL taggedMgmtTraffic)
{
  if (isMgmtTrafficTagged != taggedMgmtTraffic)
  {
    isMgmtTrafficTagged = taggedMgmtTraffic;
  }
}
#endif

void hapiBroadCallbackDebugPkt (int rxEnable)
{
  pktRxCallbackDebug = rxEnable;
}

void hapiBroadVoiceVlanAuthDebug( int enable )
{
  voiceVlanAuthDebug = enable;
}
void hapiBroadDebugWlanVp(int rxEnable, int txEnable, int printSize, int txEncap)
{
  wlanVpDebugRx = rxEnable;
  wlanVpDebugTx = txEnable;
  wlanVpDebugSize = printSize;
  wlanVpDebugEncap = txEncap;
}
void hapiBroadDebugPkt(int txEnable, int txLevel, int rxEnable, int rxLevel, int rxQueue)
{
  pktTxDebug = txEnable;
  pktRxDebug = rxEnable;
  pktTxDebugLevel = txLevel;
  pktRxDebugLevel = rxLevel;
  pktRxDebugQueue = rxQueue;
}
void hapiBroadDebugDot1adVlanInfo(int enable)
{
  dot1adVlanInfoDebug = enable;
}

#define HAPI_DEBUG_PKT_FILTER_STP    0x00000001
#define HAPI_DEBUG_PKT_FILTER_LACPDU 0x00000002
#define HAPI_DEBUG_PKT_FILTER_GMRP   0x00000004
#define HAPI_DEBUG_PKT_FILTER_GVRP   0x00000008
#define HAPI_DEBUG_PKT_FILTER_BCAST  0x00000010
#define HAPI_DEBUG_PKT_FILTER_MCAST  0x00000020
#define HAPI_DEBUG_PKT_FILTER_UCAST  0x00000040
#define HAPI_DEBUG_PKT_FILTER_SRCIP  0x00000080
#define HAPI_DEBUG_PKT_FILTER_DSTIP  0x00000100
#define HAPI_DEBUG_PKT_FILTER_ARP    0x00000200
#define HAPI_DEBUG_PKT_FILTER_SRCMAC 0x00000400
#define HAPI_DEBUG_PKT_FILTER_DSTMAC 0x00000800
#define HAPI_DEBUG_PKT_FILTER_PORT   0x00001000

void hapiBroadDebugPktHelp()
{
  sysapiPrintf("void hapiBroadDebugPktFilterGetPort(void)\n"
               "void hapiBroadDebugPktFilterSetPort(int unit, int slot, int port)\n"
               "void hapiBroadDebugPktFilterSetDump(L7_uchar8 start, L7_uchar8 len)\n"
               "void hapiBroadDebugPktFilterGetIpAddr(void)\n"
               "void hapiBroadDebugPktFilterSetIpAddr(L7_uint32 type, L7_uchar8 addr_byte1, L7_uchar8 addr_byte2,\n"
               "                                      L7_uchar8 addr_byte3, L7_uchar8 addr_byte4)\n"
               "  Use type = 0 to set src IP, type = 1 to set dst IP\n"
               "void hapiBroadDebugPktFilterGetMacAddr(void)\n"
               "void hapiBroadDebugPktFilterSetMacAddr(L7_uint32 type, L7_uchar8 addr_byte1, L7_uchar8 addr_byte2,\n"
               "                                       L7_uchar8 addr_byte3, L7_uchar8 addr_byte4,\n"
               "                                       L7_uchar8 addr_byte5, L7_uchar8 addr_byte6)\n"
               "  Use type = 0 to set src MAC, type = 1 to set dst MAC\n"
               "void hapiBroadDebugPktFilterGet(void)\n"
               "void hapiBroadDebugPktFilterSet(L7_uint32 filter)\n"
               "  Filter definition is as follows:\n"
               "  HAPI_DEBUG_PKT_FILTER_STP    0x00000001\n"
               "  HAPI_DEBUG_PKT_FILTER_LACPDU 0x00000002\n"
               "  HAPI_DEBUG_PKT_FILTER_GMRP   0x00000004\n"
               "  HAPI_DEBUG_PKT_FILTER_GVRP   0x00000008\n"
               "  HAPI_DEBUG_PKT_FILTER_BCAST  0x00000010\n"
               "  HAPI_DEBUG_PKT_FILTER_MCAST  0x00000020\n"
               "  HAPI_DEBUG_PKT_FILTER_UCAST  0x00000040\n"
               "  HAPI_DEBUG_PKT_FILTER_SRCIP  0x00000080\n"
               "  HAPI_DEBUG_PKT_FILTER_DSTIP  0x00000100\n"
               "  HAPI_DEBUG_PKT_FILTER_ARP    0x00000200\n"
               "  HAPI_DEBUG_PKT_FILTER_SRCMAC 0x00000400\n"
               "  HAPI_DEBUG_PKT_FILTER_DSTMAC 0x00000800\n"
               "  HAPI_DEBUG_PKT_FILTER_PORT   0x00001000\n"
               "void hapiBroadDebugPktFilterModeSet(L7_BOOL filterModeDeny)\n"
               "void hapiBroadDebugPktFilterModeGet()\n");
}

static int dbg_unit;
static int dbg_slot;
static int dbg_port;

void hapiBroadDebugPktFilterGetPort(void)
{
  printf("USP = %d.%d.%d\n", dbg_unit, dbg_slot, dbg_port);
}

void hapiBroadDebugPktFilterSetPort(int unit, int slot, int port)
{
  dbg_unit = unit;
  dbg_slot = slot;
  dbg_port = port;
}

static L7_uchar8 src_ip_addr[4];
static L7_uchar8 dst_ip_addr[4];
static L7_uchar8 src_mac_addr[6];
static L7_uchar8 dst_mac_addr[6];

static L7_uchar8 pkt_start_offset = 0;
static L7_uchar8 pkt_dump_len     = 64;

void hapiBroadDebugPktFilterSetDump(L7_uchar8 start, L7_uchar8 len)
{
  pkt_dump_len     = len;
  pkt_start_offset = start;
}

void hapiBroadDebugPktFilterGetIpAddr(void)
{
  sysapiPrintf("SRC IP = %d.%d.%d.%d\n", src_ip_addr[0], src_ip_addr[1], src_ip_addr[2], src_ip_addr[3]); 
  sysapiPrintf("DST IP = %d.%d.%d.%d\n", dst_ip_addr[0], dst_ip_addr[1], dst_ip_addr[2], dst_ip_addr[3]); 
}

void hapiBroadDebugPktFilterSetIpAddr(L7_uint32 type, L7_uchar8 addr_byte1, L7_uchar8 addr_byte2,
                                                      L7_uchar8 addr_byte3, L7_uchar8 addr_byte4)
{
  if(type == 0)
  {
    src_ip_addr[0] = addr_byte1;
    src_ip_addr[1] = addr_byte2;
    src_ip_addr[2] = addr_byte3;
    src_ip_addr[3] = addr_byte4;
  }
  else
  {
    dst_ip_addr[0] = addr_byte1;
    dst_ip_addr[1] = addr_byte2;
    dst_ip_addr[2] = addr_byte3;
    dst_ip_addr[3] = addr_byte4;
  }
}

void hapiBroadDebugPktFilterGetMacAddr(void)
{
    sysapiPrintf("DST MAC = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                 dst_mac_addr[0], dst_mac_addr[1], dst_mac_addr[2], dst_mac_addr[3], dst_mac_addr[4], dst_mac_addr[5]);
    sysapiPrintf("SRC MAC = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                 src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4], src_mac_addr[5]);
}

void hapiBroadDebugPktFilterSetMacAddr(L7_uint32 type, L7_uchar8 addr_byte1, L7_uchar8 addr_byte2,
                                                       L7_uchar8 addr_byte3, L7_uchar8 addr_byte4,
                                                       L7_uchar8 addr_byte5, L7_uchar8 addr_byte6)
{
  if(type == 0)
  {
    src_mac_addr[0] = addr_byte1;
    src_mac_addr[1] = addr_byte2;
    src_mac_addr[2] = addr_byte3;
    src_mac_addr[3] = addr_byte4;
    src_mac_addr[4] = addr_byte5;
    src_mac_addr[5] = addr_byte6;
  }
  else
  {
    dst_mac_addr[0] = addr_byte1;
    dst_mac_addr[1] = addr_byte2;
    dst_mac_addr[2] = addr_byte3;
    dst_mac_addr[3] = addr_byte4;
    dst_mac_addr[4] = addr_byte5;
    dst_mac_addr[5] = addr_byte6;
  }
}

void hapiBroadDebugPktFilterGet(void)
{
  sysapiPrintf("Filter = 0x%x\n", pktDebugFilter); 
}

void hapiBroadDebugPktFilterSet(L7_uint32 filter)
{
  pktDebugFilter = filter;
}

void hapiBroadDebugPktFilterModeSet(L7_BOOL filterModeDeny)
{
  if (filterModeDeny)
  {
    pktDebugFilterModeDeny = L7_TRUE;
  }
  else
  {
    pktDebugFilterModeDeny = L7_FALSE;
  }
}

void hapiBroadDebugPktFilterModeGet()
{
  if (pktDebugFilterModeDeny == L7_TRUE)
  {
    sysapiPrintf("Filter matches are NOT dumped, filter non-matches ARE dumped.\n");
  }
  else
  {
    sysapiPrintf("Filter matches ARE dumped, filter non-matches are NOT dumped.\n");
  }
}

L7_BOOL hapiBroadDebugPktIsFiltered(L7_uchar8 *pkt)
{
  L7_uchar8 stp_pkt[6]    = {0x01,0x80,0xc2,0x00,0x00,0x00};
  L7_uchar8 lacpdu_pkt[6] = {0x01,0x80,0xc2,0x00,0x00,0x02};
  L7_uchar8 gmrp_pkt[6]   = {0x01,0x80,0xc2,0x00,0x00,0x20};
  L7_uchar8 gvrp_pkt[6]   = {0x01,0x80,0xc2,0x00,0x00,0x21};
  L7_uchar8 bcast_pkt[6]  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  L7_uchar8 arp_pkt[2]    = {0x08,0x06};
  L7_BOOL   filter_match  = L7_FALSE;
  L7_BOOL   isFiltered;

  /* Filter L2 header. */
  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_SRCMAC)
  {
    if (memcmp(&pkt[6], src_mac_addr, 6) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_DSTMAC)
  {
    if (memcmp(&pkt[0], dst_mac_addr, 6) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_ARP)
  {
    if (memcmp(&pkt[16], arp_pkt, 2) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  /* Filter L3 header. */
  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_SRCIP)
  {
    if (memcmp(&pkt[30], src_ip_addr, 4) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_DSTIP)
  {
    if (memcmp(&pkt[34], dst_ip_addr, 4) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_STP)
  {
    if (memcmp(pkt, stp_pkt, 6) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_LACPDU)
  {
    if (memcmp(pkt, lacpdu_pkt, 6) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_GMRP)
  {
    if (memcmp(pkt, gmrp_pkt, 6) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_GVRP)
  {
    if (memcmp(pkt, gvrp_pkt, 6) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_BCAST)
  {
    if (memcmp(pkt, bcast_pkt, 6) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_MCAST)
  {
    if ((memcmp(pkt, bcast_pkt, 6) != 0) && (pkt[0] & 0x01))
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilter & HAPI_DEBUG_PKT_FILTER_UCAST)
  {
    if ((pkt[0] & 0x01) == 0)
    {
      filter_match = L7_TRUE;
    }
  }

  if (pktDebugFilterModeDeny == L7_TRUE)
  {
    /* Deny only filter matches. */
    isFiltered = (filter_match == L7_TRUE) ? L7_TRUE : L7_FALSE;
  }
  else
  {
    /* Permit only filter matches. */
    isFiltered = (filter_match == L7_TRUE) ? L7_FALSE : L7_TRUE;
  }

  return isFiltered;
}

void hapiBroadDebugPktDump(L7_uchar8 *pkt)
{
  int row, column;
  L7_uchar8  c;
  L7_uint32  row_count;

  if (hapiBroadDebugPktIsFiltered(pkt))
  {
    return;
  }

  sysapiPrintf("===================\n");
  row_count = pkt_dump_len / 16;
  if (pkt_dump_len % 16)
  {
    row_count++;
  }

  for (row = 0; row < row_count; row++)
  {
    sysapiPrintf("%04x ", row * 16);
    for (column = 0; (column < 16) && ((row * 16 + column) < pkt_dump_len); column++)
    {
      sysapiPrintf("%2.2x ",pkt[pkt_start_offset + row * 16 + column]);
    }
    sysapiPrintf("   ");
    for (column = 0; (column < 16) && ((row * 16 + column) < pkt_dump_len); column++)
    {
      c = pkt[pkt_start_offset + row * 16 + column];
      if isprint(c) 
      {
        sysapiPrintf("%c", c);
      }
      else 
      {
        sysapiPrintf(".");
      }
    }
    sysapiPrintf("\n");
  }
  sysapiPrintf("===================\n");
}
#endif

void hapiBroadDebugPktTx(L7_uint32 slotNum, L7_uint32 portNum, L7_uchar8 *pkt)
{
#ifdef HAPI_BROAD_PKT_DEBUG
  if (pktTxDebug)
  {
    if (!hapiBroadDebugPktIsFiltered(pkt))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Sending packet to slot %d port %d\n", slotNum ,portNum);
    }
  }
  if (pktTxDebugLevel)
  {
    hapiBroadDebugPktDump(pkt);
  }
#endif
}

/*********************************************************************
*
* @purpose Create and initialize data-structures for cpu frame 
*          transmission/reception.
*
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadCpuTxRxInit(DAPI_t *dapi_g)
{

#if (L7_DOT1S_BPDUFLOOD_SET == L7_TRUE_DEF)
  /* create queue for Retransmitting BPDUs 
   */
  hapiTxBpduQueue = (void *)osapiMsgQueueCreate("hapiTxBpduQ",
                                                10,
                                                sizeof(BROAD_TX_BPDU_MSG_t));

  if (hapiTxBpduQueue == L7_NULL)
  {
    L7_LOG_ERROR(0);
  }

  /* spawn BPDU sender task */
  if (osapiTaskCreate("hapiBpduTxTask",hapiBroadBpduTxTask,1,dapi_g, L7_DEFAULT_STACK_SIZE,
                       L7_DEFAULT_TASK_PRIORITY,L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    L7_LOG_ERROR(0);
  }
#endif  /* (L7_DOT1S_BPDUFLOOD_SET == L7_TRUE) */

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE

     /* For L2 Prtocol Tunneling */
  hapiTxPduQueue = (void *)osapiMsgQueueCreate("hapiTxPduQ",
                                                10,
                                                sizeof(BROAD_TX_PDU_MSG_t));

  if (hapiTxPduQueue == L7_NULL)
  {
    L7_LOG_ERROR(0);
  }

  /* spawn L2 Protocol Tunnel task */
  if (osapiTaskCreate("hapiPduTransmitTask",hapiBroadPduTransmitTask,1,dapi_g, L7_DEFAULT_STACK_SIZE,
                       L7_DEFAULT_TASK_PRIORITY,L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    L7_LOG_ERROR(0);
  }
#endif
#endif
  /* create queue for Rx */
  hapiRxQueue = (void *)osapiMsgQueueCreate("hapiRxQueue",
                                            L7_MAX_NETWORK_BUFF_PER_BOX,
                                            sizeof(BROAD_PKT_RX_MSG_t));

  if (hapiRxQueue == L7_NULL)
  {
    L7_LOG_ERROR(0);
  }

  /* spawn RX task */
  if (osapiTaskCreate("hapiRxTask",hapiBroadReceiveTask,1,dapi_g, L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    L7_LOG_ERROR(0);
  }

  if (hapiBroadHawkeyeCheck() == L7_TRUE)
  {
    /* Has only 4 queus - use queue 2 for cpu pkt transmit */
    hapiBroadTxCosTable[BROAD_TX_PRIO_HIGH  ] = 3;
    hapiBroadTxCosTable[BROAD_TX_PRIO_NORMAL] = 2;
  }
  else
  {
    /* All the other platforms use multiple queues for transmitting frames. */
  }

  return ;
}

/*********************************************************************
*
* @purpose Determines if a packet is EAPOL.
*
* @param   L7_uchar8 *pkt
*
* @returns L7_BOOL result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadPktIsEapol(L7_uchar8 *pkt)
{
  L7_uchar8           eap_ethtype[] = {0x88, 0x8e};

  if (memcmp(&pkt[16], eap_ethtype, sizeof(eap_ethtype)) == 0)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Determines if a packet is Spanning tree BPDU.
*
* @param   L7_uchar8 *pkt
*
* @returns L7_BOOL result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadPktIsBPDU(L7_uchar8 *pkt)
{
  L7_uchar8       bpdu_da[] = {0x01,0x80,0xc2,0x00,0x00, 0x00};

  if (memcmp(pkt, bpdu_da, sizeof(bpdu_da)) == 0)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Determines if a packet is EAPOL.
*
* @param   L7_uchar8 *pkt
*
* @returns L7_BOOL result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadPktIsArp(L7_uchar8 *pkt)
{
  L7_uchar8      arp_ethtype[] = {0x08, 0x06};
  L7_uchar8      offset = 12;
  L7_8021QTag_t  *vlanTagPtr = (L7_8021QTag_t*)&pkt[L7_ENET_HDR_SIZE];

  if ((osapiNtohs(vlanTagPtr->tpid) == L7_ETYPE_8021Q))
  {
    offset = 12 + 4;
  }

  if (memcmp(&pkt[offset], arp_ethtype, sizeof(arp_ethtype)) == 0)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Determines if a packet is DHCP.
*
* @param   L7_uchar8 *pkt
*
* @returns L7_BOOL result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadPktIsDhcp(L7_uchar8 *pkt)
{
  L7_uchar8           ip_ethtype[]   = {0x08, 0x00};
  L7_uchar8           ipv6_ethtype[] = {0x86, 0xdd};
  L7_uchar8           udp_proto[]    = {0x11};
  L7_ushort16         dhcpc_dport    = osapiHtons(UDP_PORT_DHCP_CLNT);
  L7_ushort16         dhcps_dport    = osapiHtons(UDP_PORT_DHCP_SERV);
  L7_ushort16         dhcp6c_dport   = osapiHtons(UDP_PORT_DHCP6_CLNT);
  L7_ushort16         dhcp6s_dport   = osapiHtons(UDP_PORT_DHCP6_SERV);
  L7_ushort16         offset;

  if ( (pkt[18] & 0x0f) >= 5 )
  {
    offset = (L7_ushort16) (pkt[18] & 0x0f)*4 - 20;
  }
  else
  {
    offset = 0;
  }

  /* IPv4 */
  if (memcmp(&pkt[16], ip_ethtype, sizeof(ip_ethtype)) == 0)
  {
    if (memcmp(&pkt[27], udp_proto, sizeof(udp_proto)) == 0)
    {
      if (memcmp(&pkt[40+offset], &dhcpc_dport, sizeof(dhcpc_dport)) == 0)
      {
        return L7_TRUE;
      }
      if (memcmp(&pkt[40+offset], &dhcps_dport, sizeof(dhcps_dport)) == 0)
      {
        return L7_TRUE;
      }
    }
  }

  /* IPv6 */
  if (memcmp(&pkt[16], ipv6_ethtype, sizeof(ipv6_ethtype)) == 0)
  {
    if (memcmp(&pkt[24], udp_proto, sizeof(udp_proto)) == 0)
    {
      if (memcmp(&pkt[60+offset], &dhcp6c_dport, sizeof(dhcp6c_dport)) == 0)
      {
        return L7_TRUE;
      }
      if (memcmp(&pkt[60+offset], &dhcp6s_dport, sizeof(dhcp6s_dport)) == 0)
      {
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Insert CAPWAP header when sending to a wlan virtual port
*
* @param   L7_uchar8    *pkt      payload frame
* @param   bcmx_lport_t lport     gport of wlan vp
* @param   uint32       flags     
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadWlanPortSend(bcm_pkt_t *pkt, bcmx_lport_t lport, uint32 flags, DAPI_t *dapi_g)
{
#ifdef L7_WIRELESS_PACKAGE
  bcm_pkt_t bcm_pkt;
  bcm_pkt_t bcm_pkt_frag;
  bcm_pkt_blk_t     bcm_pkt_blk;
  bcm_pkt_blk_t     bcm_pkt_blk_frag;
  bcmx_uport_t         uport;
  DAPI_USP_t           usp;
  L7_BOOL tagged = L7_FALSE;
  bcm_tunnel_initiator_t initiator;
  bcm_gport_t physical_txlport;;
  static L7_ushort16  iph_ident = 0; 
  L7_uchar8 *bufPtr;
  L7_uchar8 *secondFragBufPtr;
  L7_ushort16 temp16;
  L7_uint32 temp32;
  L7_udp_header_t *udp_header;
  L7_ipHeader_t   *ip;
  L7_capwapHdr_t  *capwap;
  L7_uint32 totalLength, frameLength;
  L7_ushort16 offset;
  L7_ushort16  ip_ethtype  = L7_ETYPE_IP;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_WLAN_TUNNEL_ENTRY_t wlanTunnelInfo;
  L7_uint32 encapHdrlen = sizeof(L7_ethHeader_t) + sizeof(L7_8021QTag_t) + L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + sizeof(L7_capwapHdr_t);

  usp.unit = -1;
  usp.slot = -1;
  usp.port = -1;

  memcpy(&bcm_pkt, pkt, sizeof(bcm_pkt));
  bcm_pkt.pkt_data  = &bcm_pkt_blk;
  bcm_pkt.blk_count = 1;

  if (hapiBroadWlanInfoGet(dapi_g, lport, &wlanTunnelInfo) != L7_SUCCESS)
  {
    return;
  }

  bcm_pkt.pkt_data->data = sal_dma_alloc(pkt->pkt_data->len + encapHdrlen, "hapiBuf");
  if (bcm_pkt.pkt_data->data == NULL)
  {
    return;
  }
  bufPtr = bcm_pkt.pkt_data->data;
  memcpy(&initiator, &wlanTunnelInfo.initiator, sizeof(initiator));
  physical_txlport = wlanTunnelInfo.wlan_port.port;


  /* Is the port a LAG port */
  if (BCM_GPORT_IS_TRUNK(physical_txlport))
  {
    L7_uint32 i;
    DAPI_LAG_ENTRY_t  *lagEntryPtr;
    DAPI_USP_t    lag_member_usp;
    BROAD_PORT_t *lagMemHapiPortPtr;

    /* get usp of lag port and lport of first physical member port */
    if (hapiBroadTgidToUspConvert(BCM_GPORT_TRUNK_GET(physical_txlport), &usp, dapi_g) != L7_SUCCESS)
    {
      sal_dma_free(bcm_pkt.pkt_data->data);
      return;
    }

    physical_txlport = 0;
    lagEntryPtr = GET_DAPI_PORT(dapi_g, &usp)->modeparm.lag.memberSet;
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      lag_member_usp = lagEntryPtr[i].usp;
      if ((lagEntryPtr[i].inUse == L7_TRUE) &&
          (isValidUsp (&lag_member_usp, dapi_g) == L7_TRUE))
      {
        lagMemHapiPortPtr = HAPI_PORT_GET(&lag_member_usp, dapi_g);
        physical_txlport = lagMemHapiPortPtr->bcmx_lport;
        break;
      }
    }
  }
  else
  {
    uport = BCMX_UPORT_GET(physical_txlport);
    if (uport == BCMX_UPORT_INVALID_DEFAULT)
    {
      sal_dma_free(bcm_pkt.pkt_data->data);
      return;
    }
    HAPI_BROAD_UPORT_TO_USP(uport,&usp);
  }

  if (isValidUsp (&usp, dapi_g) == L7_FALSE || physical_txlport <= 0)
  {
    sal_dma_free(bcm_pkt.pkt_data->data);
    return;
  }

  dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

  if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) && 
      (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE))
  {
    bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_TRUE);
    tagged = L7_TRUE;
  }
  else if (BROAD_IS_VLAN_TAGGING(&usp, initiator.vlan, dapi_g))
  {
    bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_FALSE);
    tagged = L7_TRUE;
  }
  else  /* send pkt w/o a tag */
  {
    bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_TRUE);
    tagged = L7_TRUE;
  }

  memcpy(bufPtr, initiator.dmac, sizeof(initiator.dmac));
  bufPtr += sizeof(initiator.dmac);
  memcpy(bufPtr, initiator.smac, sizeof(initiator.smac));
  bufPtr += sizeof(initiator.smac);

  if (tagged == L7_TRUE)
  {
    temp16 = osapiHtons(L7_ETYPE_8021Q);
    memcpy(bufPtr, &temp16, sizeof(temp16));
    bufPtr += sizeof(temp16);
  
    temp16 = osapiHtons(initiator.vlan);
    memcpy(bufPtr, &temp16, sizeof(temp16));
    bufPtr += sizeof(temp16);
  }

  temp16 = osapiHtons(ip_ethtype);
  memcpy(bufPtr, &temp16, sizeof(temp16));
  bufPtr += sizeof(temp16);

  /* fill in ip header */
  ip = (L7_ipHeader_t *)bufPtr;
  ip->iph_dst = osapiHtonl(initiator.dip);
  ip->iph_src = osapiHtonl(initiator.sip);

  ip->iph_versLen = (L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN ;
  ip->iph_tos = 0;
  ip->iph_len = L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + sizeof(L7_capwapHdr_t) + pkt->pkt_data->len;
  if (ip->iph_len > (FD_WIRELESS_CAPWAP_MTU - sizeof(L7_ethHeader_t) - sizeof(L7_8021QTag_t)))
  {
    offset = FD_WIRELESS_CAPWAP_MTU - encapHdrlen;
    offset = (offset >> 3); /* make offset a multiple of 8 */
    offset = (offset << 3); /* multiply offset by 8 to get total bytes again */
    ip->iph_len = (offset + L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + sizeof(L7_capwapHdr_t));
  }

  ip->iph_len = osapiHtons(ip->iph_len);
  iph_ident++;
  if (iph_ident > 65534)
  {
    iph_ident = 1;
  }
  ip->iph_ident  = osapiHtons(iph_ident);
  ip->iph_flags_frag  = osapiHtons(0x4000); /* don't fragment */
  ip->iph_ttl = 64;
  ip->iph_prot= IP_PROT_UDP;
  ip->iph_csum= 0;
  ip->iph_csum = inetChecksum(ip, sizeof(L7_ipHeader_t));

  /* fill in udp header */
  bufPtr += sizeof(L7_ipHeader_t);
  udp_header = (L7_udp_header_t *)bufPtr;
  udp_header->destPort = osapiHtons(dapi_g->system->wlan_capwap_data_udp_dst_port);
  udp_header->sourcePort = osapiHtons(dapi_g->system->wlan_capwap_data_udp_src_port);
  udp_header->length = 0;
  udp_header->checksum = 0; /* no udp chksum for capwap */
  udp_header->length = (sizeof(L7_udp_header_t) + sizeof(L7_capwapHdr_t) + pkt->pkt_data->len);

  if (udp_header->length > (FD_WIRELESS_CAPWAP_MTU - sizeof(L7_ethHeader_t) - sizeof(L7_8021QTag_t) - L7_IP_HDR_LEN))
  {
    offset = FD_WIRELESS_CAPWAP_MTU - encapHdrlen;
    offset = (offset >> 3); /* make offset a multiple of 8 */
    offset = (offset << 3); /* multiply offset by 8 to get total bytes again */

    udp_header->length = (offset + sizeof(L7_udp_header_t) + sizeof(L7_capwapHdr_t));
  }
  udp_header->length = osapiHtons(udp_header->length);


  totalLength = sizeof(L7_ethHeader_t) + L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + sizeof(L7_capwapHdr_t) + pkt->pkt_data->len;

  if (tagged == L7_TRUE)
  {
    totalLength += sizeof(L7_8021QTag_t);
  }

  /* fill in capwap header */
  bufPtr += sizeof(L7_udp_header_t);
  capwap = (L7_capwapHdr_t *) bufPtr;
  memset(capwap, 0, sizeof(L7_capwapHdr_t));
  temp32 = 0;
  temp32 |= (CAPWAP_VERSION << CAPWAP_PREAMBLE_VERSION_OFFSET);
  temp32 |= (CAPWAP_DATA_PAYLOAD_TYPE << CAPWAP_PREAMBLE_TYPE_OFFSET);

  temp32 |= (CAPWAP_WBID_IEEE80211 << CAPWAP_HDR_WBID_OFFSET);
  temp32 |= ((sizeof(L7_capwapHdr_t) >> 2) << CAPWAP_HDR_HLEN_OFFSET);
  temp32 = osapiHtonl(temp32);
  memcpy(&capwap->preamble, &temp32, sizeof(L7_uint32));

  /* fragmentation support */
  if (totalLength > FD_WIRELESS_CAPWAP_MTU)
  {
    L7_uint32 preamble;

    capwap->fragment_id = osapiHtons(iph_ident);
    preamble = osapiNtohl(capwap->preamble);
    preamble |= CAPWAP_HDR_F_BIT;
    capwap->preamble = osapiHtonl(preamble);

    offset = FD_WIRELESS_CAPWAP_MTU - encapHdrlen;
    offset = (offset >> 3); /* make offset a multiple of 8 */
    offset = (offset << 3); /* multiply offset by 8 to get total bytes again */

    frameLength = totalLength - offset;
    totalLength = offset + encapHdrlen;

    memcpy(&bcm_pkt_frag, &bcm_pkt, sizeof(bcm_pkt_frag));
    bcm_pkt_frag.pkt_data  = &bcm_pkt_blk_frag;
    bcm_pkt_frag.blk_count = 1;

    if (frameLength < 68)
    {
      frameLength = 68;
    }

    bcm_pkt_frag.pkt_data->data = sal_dma_alloc(frameLength, "hapiBuf");

    if (bcm_pkt_frag.pkt_data->data == NULL)
    {
      sal_dma_free(bcm_pkt.pkt_data->data);
      return;
    }
    secondFragBufPtr = bcm_pkt_frag.pkt_data->data;

    memcpy(&bcm_pkt_frag.pkt_data->data[0], &bcm_pkt.pkt_data->data[0], encapHdrlen);
    memcpy(&bcm_pkt_frag.pkt_data->data[0] + encapHdrlen, (pkt->pkt_data->data + offset), 
           (frameLength - encapHdrlen));

    /* adjust ip and udp header on second fragment */
    secondFragBufPtr += sizeof(L7_ethHeader_t) + sizeof(L7_8021QTag_t);
    ip = (L7_ipHeader_t *)secondFragBufPtr;
    ip->iph_len = L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + sizeof(L7_capwapHdr_t) + (frameLength - encapHdrlen);
    ip->iph_len = osapiHtons(ip->iph_len);
    ip->iph_csum= 0;
    ip->iph_csum = inetChecksum(ip, sizeof(L7_ipHeader_t));

    secondFragBufPtr += sizeof(L7_ipHeader_t);
    udp_header = (L7_udp_header_t *)secondFragBufPtr;
    udp_header->length = (sizeof(L7_udp_header_t) + sizeof(L7_capwapHdr_t) + (frameLength - encapHdrlen));
    udp_header->length = osapiHtons(udp_header->length);

    /* set the capwap frag offset in second fragment */
    secondFragBufPtr += sizeof(L7_udp_header_t);
    capwap = (L7_capwapHdr_t *) secondFragBufPtr;
    preamble = osapiNtohl(capwap->preamble);
    preamble |= CAPWAP_HDR_L_BIT;
    capwap->preamble = osapiHtonl(preamble);

    offset = (offset >> 3); /* divide offset by 8 to calc units of 8 octets */
    offset = (offset << 3);  /* reserved flags should be zero */
    capwap->frag_offset = osapiHtons(offset);

    bcm_pkt_frag.pkt_data->len = frameLength;

    if (wlanVpDebugEncap)
    {
      L7_uint32 k;
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n\n %s: Sending fragment to wlanvp %d with flags %x; pktflags %x \n", __FUNCTION__, lport, flags, bcm_pkt_frag.flags);
      for (k=0; k<wlanVpDebugSize && k<bcm_pkt_frag.pkt_data->len; k++)
      {
        if (k%32 == 0 && k != 0)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
        }
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "%02x", bcm_pkt_frag.pkt_data->data[k]);
      }

    }

    bcmx_tx_uc (&bcm_pkt_frag, physical_txlport, flags);
    /* increment the CPU stats */
    hapiBroadStatsCpuIncrement(bcm_pkt_frag.pkt_data->data, 
                               bcm_pkt_frag.pkt_data->len,
                               &usp,dapi_g,L7_FALSE,L7_FALSE);
    sal_dma_free(bcm_pkt_frag.pkt_data->data);
  }

  /* copy payload */
  bufPtr += sizeof(L7_capwapHdr_t);
  memcpy(bufPtr, pkt->pkt_data->data, (totalLength - encapHdrlen));
  bcm_pkt.pkt_data->len = totalLength;

  if (wlanVpDebugEncap)
  {
    L7_uint32 k;
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n\n %s: Sending capwap to wlanvp %d with flags %x; pktflags %x \n", __FUNCTION__, lport, flags, bcm_pkt.flags);
    for (k=0; k<wlanVpDebugSize && k<bcm_pkt.pkt_data->len; k++)
    {
      if (k%32 == 0 && k != 0)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
      }
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "%02x", bcm_pkt.pkt_data->data[k]);
    }

  }

  bcmx_tx_uc (&bcm_pkt, physical_txlport, flags);

  /* increment the CPU stats */
  hapiBroadStatsCpuIncrement(bcm_pkt.pkt_data->data, 
                             bcm_pkt.pkt_data->len,
                             &usp,dapi_g,L7_FALSE,L7_FALSE);
  sal_dma_free(bcm_pkt.pkt_data->data);

#endif
}

/*********************************************************************
*
* @purpose Filter wlan virtual ports from the lplist, and send capwap frame
*
* @param   L7_uchar8    *pkt      payload frame
* @param   bcmx_lplist_t *tx_ports     gport of wlan vp
* @param   bcmx_lplist_t *untagged_ports     
* @param   uint32 flags     
*
* @returns int
*
* @notes   none
*
* @end
*
*********************************************************************/
int bcmx_tx_lplist_intercept(bcm_pkt_t *pkt, bcmx_lplist_t *tx_ports,
                             bcmx_lplist_t *untagged_ports, uint32 flags, DAPI_t *dapi_g)
{
  bcmx_lport_t lport;
  int i;
  bcmx_lplist_t         removeList;
  int                   rv;

  rv = bcmx_lplist_init(&removeList,L7_MAX_INTERFACE_COUNT,0);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOG_ERROR(rv);
  }
  BCMX_LPLIST_IDX_ITER(tx_ports, lport, i) 
  {
    if (BCM_GPORT_IS_WLAN_PORT(lport))
    {
      /* insert encapsulation and transmit on the physical port */
      if (wlanVpDebugTx)
      {
        L7_uint32 k;
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n\n %s: Sending to wlanvp %d with flags %x; pktflags %x \n", __FUNCTION__, lport, flags, pkt->flags);
        for (k=0; k<wlanVpDebugSize && k<pkt->pkt_data->len; k++)
        {
          if (k%32 == 0 && k != 0)
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
          }
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "%02x", pkt->pkt_data->data[k]);
        }
      }
      hapiBroadWlanPortSend(pkt, lport, flags, dapi_g);

      BCMX_LPLIST_ADD(&removeList, lport);
    }

  }
  if (untagged_ports != NULL)
  {
    BCMX_LPLIST_IDX_ITER(untagged_ports, lport, i) 
    {
      if (BCM_GPORT_IS_WLAN_PORT(lport))
      {
        /* insert encapsulation and transmit on the physical port */
        /*if (wlanVpDebug)*/
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                        "bcmx_tx_lplist_intercept untagged: Sending to wlanvp %d with flags %x \n %02x%02x%02x%02x%02x%02x %02x%02x%02x%02x%02x%02x %02x%02x%02x%02x%02x%02x%02x%02x \n",
                        lport, flags,
                        pkt->pkt_data->data[0],pkt->pkt_data->data[1],pkt->pkt_data->data[2],pkt->pkt_data->data[3],pkt->pkt_data->data[4],pkt->pkt_data->data[5],
                        pkt->pkt_data->data[6],pkt->pkt_data->data[7],pkt->pkt_data->data[8],pkt->pkt_data->data[9],pkt->pkt_data->data[10],pkt->pkt_data->data[11],
                        pkt->pkt_data->data[12],pkt->pkt_data->data[13],pkt->pkt_data->data[14],pkt->pkt_data->data[15],pkt->pkt_data->data[16],pkt->pkt_data->data[17],
                        pkt->pkt_data->data[18],pkt->pkt_data->data[19]
                        );
        }
        /*hapiBroadWlanPortSend(pkt, lport, flags, dapi_g);*/

      }

    }
  }

  /* Now remove the ports from the list */
  BCMX_LPLIST_IDX_ITER(&removeList, lport, i)
  {
    BCMX_LPLIST_REMOVE(tx_ports, lport);
  }

  bcmx_lplist_free(&removeList);
  return bcmx_tx_lplist (pkt, tx_ports, untagged_ports, flags);

}

/*********************************************************************
*
* @purpose Sends a packet from the CPU
*
* @param   DAPI_USP_t *usp    - Used to get the driver ports.  Needs to be a valid usp.
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_FRAME_SEND
* @param   void       *data   - DAPI_FRAME_CMD_t.cmdData.send
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSend(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result       = L7_SUCCESS;
  DAPI_FRAME_CMD_t        *cmdInfo      = (DAPI_FRAME_CMD_t*)data;
  DAPI_LAG_ENTRY_t        *lagEntryPtr;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  L7_uchar8               *frameData;
  DAPI_USP_t               destUsp;
  DAPI_FRAME_TYPE_t        frameType;

  bcm_pkt_t                bcm_pkt;
  bcm_pkt_blk_t            bcm_pkt_blk;
  L7_uint32                bcm_pkt_alloc_len;

  /* Start the packet 4 bytes into the buffer to leave room for adding a vlan tag */
  L7_ulong32               pkt_start_idx      = 4;
  L7_ulong32               pkt_idx            = 4;

  L7_uint32                buf_len;
  L7_int32                 i;
  L7_netBlockHandle        blockHandle        = L7_NULL; /* important to initialize NULL */
  L7_BOOL                  portIsForwarding;
  L7_uint32                frameLength;
  L7_BOOL                  etype_eapol = L7_FALSE;
  bcmx_lplist_t            mcastLplist;
  bcmx_lplist_t            taggedLplist;
  bcmx_lplist_t            untaggedLplist;
  int                      rv;
  int                      bcmTxRv = BCM_E_NONE;
  L7_ushort16              sendVlanId;
  L7_BOOL                  frameSent = L7_FALSE;
  DAPI_USP_t               lag_member_usp;
  L7_BOOL                  eapolRobo = L7_FALSE;
#ifdef HAPI_BROAD_PKT_DEBUG
  L7_BOOL                  skip = L7_FALSE;
#endif

#if defined(FEAT_METRO_CPE_V1_0)
  L7_BOOL                  isMgmtPkt = L7_FALSE;
#endif

  /* The CPU transmits frames at the highest priority. This may interfere 
  ** with frame reception. Yield the CPU after every frame to make sure
  ** that transmitters don't monopolize the CPU.
  ** BRR - This has been removed because the scheduling overhead on Linux
  **       is too high. Yield only on transmission failure.
  */
  /*osapiTaskYield ();*/


  /* If we don't clear this structure the send may not work. */
  memset (&bcm_pkt, 0, sizeof (bcm_pkt));

  if (isValidUsp(usp, dapi_g) == L7_FALSE)
  {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Non valid usp={%d,%d,%d}", usp->unit, usp->slot, usp->port);

      /* Free the frame from the buffer */
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);

      return(L7_SUCCESS);
  }

  if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL3)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Packet going to be transmited on usp={%d,%d,%d}", usp->unit, usp->slot, usp->port);
  }

  /* Get the Driver Ports */
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  /* Get the Packet into a local buffer, Just a sample of how you handle */
  /* buffers, not necessary in all cases */

  /* If frame is smaller than 60, set it to be 60 */
  frameLength = sysapiNetMbufGetFrameLength(cmdInfo->cmdData.send.frameHdl);
  sysapiNetMbufGetNextBuffer( cmdInfo->cmdData.send.frameHdl, &blockHandle,
                              &frameData, &buf_len );

  if (frameData == L7_NULL)
  {
    if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
      PT_LOG_ERR(LOG_CTX_HAPI,"Non valid frameData");

    L7_LOG_ERROR(0);
    return L7_FAILURE;
  }

  bcm_pkt.pkt_data  = &bcm_pkt_blk;
  bcm_pkt.blk_count = 1;
  bcm_pkt.call_back = L7_NULLPTR;  /* only used for async responses which we do not use */
  /* Allocate based on the frame length  and accomodating for VLAN Tag. Also, the 
   * min packet length must be 64.
   */
  bcm_pkt_alloc_len = (frameLength < 64) ? 68 : (frameLength+4);
  bcm_pkt.pkt_data->data = sal_dma_alloc(bcm_pkt_alloc_len, "hapiBuf");

  bcm_pkt.cos = hapiBroadTxCosTable[BROAD_TX_PRIO_NORMAL];

#ifdef L7_CHASSIS
  bcm_pkt.src_mod = 0;
  bcm_pkt.flags   |= BCM_TX_SRC_MOD;
#endif

  /* PTIN added: PTP Timestamp BCM_PKT_F_xxx flags. */
  bcm_pkt.flags   |= cmdInfo->cmdData.send.flags;

  if (bcm_pkt.pkt_data->data == NULL)
  {
    /* Free the frame from the buffer */
    sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);

    if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
      PT_LOG_ERR(LOG_CTX_HAPI,"sal_dma_alloc failed");

    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "%s %d: In %s - sal_dma_alloc failed.\n",
                       __FILE__, __LINE__, __FUNCTION__);
    return(L7_SUCCESS);
  }

  /* set EAPOL ethertype flag */
  if ((frameData[12] == 0x88) && (frameData[13] == 0x8e))
  {
    etype_eapol = L7_TRUE;
    eapolRobo   = L7_TRUE;
  }

  if (hapiPortPtr->voiceVlanPort.voiceVlanUnauthMode==L7_ENABLE)
  {
    if ((frameData[12] == 0x88) && (frameData[13] == 0xcc))
      etype_eapol = L7_TRUE;
  }
  /* copy into buffer */
  while ( blockHandle != L7_NULL )
  {
    if (buf_len + pkt_idx > bcm_pkt_alloc_len)
    {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"packet too big: result=%d", result);

      SYSAPI_PRINTF(hapiDriverPrintfFlag,
                    "%s %d: In %s packet too big\n",
                    __FILE__, __LINE__, __FUNCTION__);

      /* Free the frame from the buffer */
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);

      return(result);
    }

    /* copy into our buffer */
    memcpy(&bcm_pkt.pkt_data->data[pkt_idx],frameData,buf_len);

    /* increment our buffer offset for next piece */
    pkt_idx+=buf_len;

    /* get the next buffer, if availible */
    sysapiNetMbufGetNextBuffer( cmdInfo->cmdData.send.frameHdl, &blockHandle,
                                &frameData, &buf_len );
  } /* ends copy of packet into local buffer */

  frameType = cmdInfo->cmdData.send.type;
  destUsp = *usp;

  if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    hapiBroadL3RoutingSend(&destUsp, &frameType, &bcm_pkt.pkt_data->data[pkt_start_idx], dapi_g);

    /* Retrieve HAPI pointer only if we are sending unicast frame. For multicast 
    ** frames the pointer is not available and is not used.
    */
    if ((frameType == DAPI_FRAME_TYPE_DATA_TO_PORT) ||
        (frameType == DAPI_FRAME_TYPE_NO_L2_EGRESS_DATA_TO_PORT))
    {
      if (isValidUsp(&destUsp, dapi_g) == L7_FALSE)
      {
        /* Free the frame from the buffer */
        sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
        sal_dma_free(bcm_pkt.pkt_data->data);
        return L7_SUCCESS;
      }

      hapiPortPtr = HAPI_PORT_GET(&destUsp, dapi_g);
      dapiPortPtr = DAPI_PORT_GET(&destUsp, dapi_g);
    }
  }

  /* Is the port a LAG port */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    /* send frame out first member port in LAG */
    lagEntryPtr = GET_DAPI_PORT(dapi_g, &destUsp)->modeparm.lag.memberSet;
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      lag_member_usp = lagEntryPtr[i].usp;
      if ((lagEntryPtr[i].inUse == L7_TRUE) &&
          (isValidUsp (&lag_member_usp, dapi_g) == L7_TRUE))
      {
        hapiPortPtr = HAPI_PORT_GET(&lag_member_usp, dapi_g);
        dapiPortPtr = DAPI_PORT_GET(&lag_member_usp, dapi_g);
        break;
      }
    }

    /* if there are no member ports, just free the buffer and return */
    if (i == L7_MAX_MEMBERS_PER_LAG)
    {
      /* Free the frame from the buffer */
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);

      return L7_SUCCESS;
    }
  }

  /* If this is a port-based routing interface, get the assigned VLAN */
  if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) && 
      (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE))
  {
    sendVlanId = hapiPortPtr->port_based_routing_vlanid;
  }
  else
  {
    sendVlanId = cmdInfo->cmdData.send.vlanID;
  }

  if(hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
  {
    /* if packet is untagged, add one */
  if ( (*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12] != osapiHtons(hapiBroadDvlanEthertype)) &&
       (*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12] != osapiHtons(0x8100)) &&
       (*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12] != osapiHtons(0x88A8)) && /* PTin added: ethertypes */ 
       (*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12] != osapiHtons(0x9100))    /* PTin added: ethertypes */
     ) /* Is this correct way ? */
    {
        memcpy(&bcm_pkt.pkt_data->data[0], &bcm_pkt.pkt_data->data[4], 12);
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[12] = osapiHtons(hapiBroadDvlanEthertype);
    *(L7_ushort16 *)&bcm_pkt.pkt_data->data[14] = osapiHtons(BCM_VLAN_CTRL(cmdInfo->cmdData.send.priority , 0, sendVlanId));
      pkt_start_idx = 0;
      frameLength+=4;
    }
    else
    {
      /* remove the initial offset(pkt_idx:4) if the packet is already tagged.*/
      memcpy(&bcm_pkt.pkt_data->data[0], &bcm_pkt.pkt_data->data[4], frameLength);
      #if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
      {
        bcm_chip_family_t    board_family;

        if (hapiBroadGetSystemBoardFamily(&board_family) == L7_SUCCESS)
        {
           if ( (board_family == BCM_FAMILY_TRIUMPH) ||
                (board_family == BCM_FAMILY_TRIUMPH2)
              )
           {
             if (*(L7_ushort16 *)&bcm_pkt.pkt_data->data[14] == osapiHtons(0)) /* It an untagged PKt */
             {
               memcpy(&bcm_pkt.pkt_data->data[12], &bcm_pkt.pkt_data->data[16], frameLength-16);
               frameLength-=4;
             }
           }
        }
      }
     #endif
    }
  }
  else
  {
    #if defined(FEAT_METRO_CPE_V1_0)
      BROAD_SYSTEM_t               *hapiSystemPtr;

      hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

      /* Identify the Mgmt traffic */
      if ( memcmp( ((L7_ushort16 *)&bcm_pkt.pkt_data->data[4+6]),
                  (hapiSystemPtr->bridgeMacAddr.addr),
                  6) == 0)
      {
        /* Assuming that All mgmt traffic will be untagged */
        /* All IP/IPV6/ARP will be tagged.*/ 
        if ( (osapiNtohs(*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12]) != 0x8100) &&
             (osapiNtohs(*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12]) != 0x88a8) &&
             (osapiNtohs(*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12]) != 0x9100)  /* PTin added: ethertypes */
           )
        {
          if ( (osapiNtohs(*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12]) == 0x0800) ||
               (osapiNtohs(*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12]) == 0x0806) ||
               (osapiNtohs(*(L7_ushort16 *)&bcm_pkt.pkt_data->data[4+12]) == 0x86dd)
             )

          {
            if (isMgmtTrafficTagged == L7_TRUE)
            {
              isMgmtPkt = L7_TRUE;
            }
          }
        }

     }

     if (isMgmtPkt == L7_TRUE)
     {
        memcpy(&bcm_pkt.pkt_data->data[0], &bcm_pkt.pkt_data->data[4], 12);
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[12] = osapiHtons(hapiBroadDvlanEthertype);
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[14] = osapiHtons(BCM_VLAN_CTRL(0, 0, hapiSystemPtr->mgmtVlanId));
        pkt_start_idx = 0;
        frameLength+=4;
        /* Placing the mgmtVlan info here to mimic the Mgmt traffic */
        cmdInfo->cmdData.send.vlanID = hapiSystemPtr->mgmtVlanId; 
    }
    else
   #endif
    {
    memcpy(&bcm_pkt.pkt_data->data[0], &bcm_pkt.pkt_data->data[4], frameLength);
    bcm_pkt.flags |= (BCM_PKT_F_NO_VTAG | BCM_PKT_F_TX_UNTAG);
  }
  }

  /* On XGS3, the packet length must be min of 64 bytes (incl VLAN tag).
   * Pad it to make it 64. The TX CRC of 4 bytes gets appended and VLAN
   * tag of 4 bytes gets stripped on egress.
   */
  if (frameLength < 64) 
  {
    /* clear the padded out area */
    memset(&bcm_pkt.pkt_data->data[frameLength], 0, (64 - frameLength));
    sysapiNetMbufSetDataLength(cmdInfo->cmdData.send.frameHdl,64);
    frameLength = 64;
  }
  bcm_pkt.pkt_data->len = frameLength;

#ifndef PC_LINUX_HOST /* no CRC in simulation environment */
  bcm_pkt.flags |= BCM_TX_CRC_APPEND;
#endif

#ifdef HAPI_BROAD_PKT_DEBUG
  if(pktDebugFilter & HAPI_DEBUG_PKT_FILTER_PORT)
  {
    if ( (destUsp.unit != dbg_unit) || (destUsp.slot != dbg_slot) || (destUsp.port != dbg_port))
      skip = L7_TRUE;
  }

  if (!skip && pktTxDebug)
  {
    if (!hapiBroadDebugPktIsFiltered(bcm_pkt.pkt_data->data))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Sending packet to %d.%d.%d with frameType %d\n",destUsp.unit, destUsp.slot,destUsp.port, frameType);
    }
  }
  if (!skip && pktTxDebugLevel)
  {
    hapiBroadDebugPktDump(bcm_pkt.pkt_data->data);
  }
#endif


  /* If this is a l2 tunnel interface */
  if (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr) == L7_TRUE)
  {
    if (wlanVpDebugTx)
    {
      L7_uint32 k;
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n\n %s: Sending to usp %d.%d.%d, wlanvp %d with frameType %d pktflags %x \n", 
                    __FUNCTION__, destUsp.unit, destUsp.slot, destUsp.port, hapiPortPtr->bcmx_lport, frameType, bcm_pkt.flags);
      for (k=0; k<wlanVpDebugSize && k<bcm_pkt.pkt_data->len; k++)
      {
        if (k%32 == 0 && k != 0)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
        }
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "%02x", bcm_pkt.pkt_data->data[k]);
      }
    }
    hapiBroadWlanPortSend(&bcm_pkt, hapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL, dapi_g);

    /* Free the frame from the buffer */
    sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
    sal_dma_free(bcm_pkt.pkt_data->data);
    return L7_SUCCESS;
  }


  dapiTraceFrame(&destUsp,bcm_pkt.pkt_data->data,DAPI_TRACE_FRAME_SEND,frameType);

  /* PTin added: transmission */
  /* Dump first 64 bytes */
  if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL3)
  {
    PT_LOG_DEBUG(LOG_CTX_HAPI,"Packet to be transmited on usp={%d,%d,%d} (lport=0x%08x) with sendVLAN=%u (frameType=%u, flags=0x%08x)",
              destUsp.unit, destUsp.slot, destUsp.port, hapiPortPtr->bcmx_lport, cmdInfo->cmdData.send.vlanID, frameType, bcm_pkt.flags);
  }

  /* determine how the packet is to be sent */
  switch (frameType)
  {
  case DAPI_FRAME_TYPE_DATA_TO_PORT:
    /*
     * Transmit the frame to port
     */
    /* If the port is unauthorized by 802.1x then only allow EAPOL ethertype */
    if ((etype_eapol == L7_FALSE) && (!HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPortPtr)))
    {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Failed transmission: etype_eapol=%u HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPortPtr)=%u",
                etype_eapol, HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPortPtr));
      /* Don't send - free the frame from the buffer */
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);
      return L7_SUCCESS;
    }

    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) && 
        (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE))
    {
      /* send pkt w/o a tag */
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_TRUE);
    }
    else if (BROAD_IS_VLAN_TAGGING(&destUsp, cmdInfo->cmdData.send.vlanID, dapi_g))
    {
      /* send packet w/ tag */
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_FALSE);
    }
    else
    {
      /* send pkt w/o a tag */
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_TRUE);
    }

    portIsForwarding = hapiBroadPortIsForwarding(&destUsp, cmdInfo->cmdData.send.vlanID, dapi_g);

    if (portIsForwarding == L7_TRUE)
    {
            
#ifndef L7_CHASSIS 
      rv = bcmx_tx_uc (&bcm_pkt, hapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL);
#else
      rv = bcmx_tx_uc (&bcm_pkt, hapiPortPtr->bcmx_lport, 0); 
#endif

      if (bcm_pkt.flags & BCM_PKT_F_TIMESYNC) /* Packet is for Time Sync protocol. */
      {       
        #if (PTIN_BOARD == PTIN_BOARD_TG16G)
        {
          L7_uint32 counter = 1000;
          L7_uint32 regvalue;

          /* Poll Status Register */
          while (counter > 0)
          {
            counter--;  /* put a limit on this loop */

            soc_reg32_get(hapiPortPtr->bcm_unit, TS_STATUS_CNTRLr, hapiPortPtr->bcm_port, 0, &regvalue);

            if ((regvalue & 0x02) == 0) /* TS_STATUS_CNTRL.TX_TS_FIFO_EMPTY bit indicator */
            {
              // getreg TX_TS_DATA.TX_TS_DATA;
              soc_reg32_get(hapiPortPtr->bcm_unit, TX_TS_DATAr, hapiPortPtr->bcm_port, 0, &regvalue);

              cmdInfo->cmdData.receive.timestamp = regvalue;
              break;
            }

          }
        }
        #else
        {
          PT_LOG_ERR(LOG_CTX_HAPI,"This HW Variant (%d) is not currently supported for Time Sync protocol", hapiBroadRoboVariantCheck());

          cmdInfo->cmdData.receive.timestamp = 0;
        }
        #endif
      }

      if ( (hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID) &&
           (dapi_g->system->dvlanEnable == L7_FALSE)
         )
      {
        if(eapolRobo == L7_TRUE)
        {
          /* copy into our buffer */
         memcpy(&bcm_pkt.pkt_data->data[0],frameData,12);
         memcpy(&bcm_pkt.pkt_data->data[0],default_mac_dst,6);
         *(L7_ushort16 *)&bcm_pkt.pkt_data->data[12] = osapiHtons(L7_ETYPE_8021Q);
         *(L7_ushort16 *)&bcm_pkt.pkt_data->data[14] = osapiHtons(BCM_VLAN_CTRL(0, 0, 1));
         *(L7_ushort16 *)&bcm_pkt.pkt_data->data[16] = 0x1111;
         frameLength = 18;
         /* clear the padded out area */
         memset(&bcm_pkt.pkt_data->data[frameLength], 0, (64 - frameLength));
         bcm_pkt.pkt_data->len = 64;
         bcm_pkt.flags = BCM_TX_CRC_APPEND ;

         if (pktTxDebugLevel)
         {
           hapiBroadDebugPktDump(bcm_pkt.pkt_data->data);
         }
         bcmx_tx_uc (&bcm_pkt, hapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL );
        }
      }

      bcmTxRv = rv;
      frameSent = L7_TRUE;
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        hapiTxDebugCounters[frameType]++;
      }
    }
    break;

  case DAPI_FRAME_TYPE_NO_L2_EGRESS_DATA_TO_PORT:

    /*
     * Transmit the frame to port
     */

    /* If the port is unauthorized by 802.1x then only allow EAPOL ethertype */
    if ((etype_eapol == L7_FALSE) && (!HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPortPtr)))
    {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Failed transmission: etype_eapol=%u HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPortPtr)=%u",
                etype_eapol, HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPortPtr));
      /* Don't send - free the frame from the buffer */
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);
      return L7_SUCCESS;
    }

    /* Send these frames at higher priority */
    bcm_pkt.cos = hapiBroadTxCosTable[BROAD_TX_PRIO_HIGH];

    /* Always send without a tag.
    */
#ifdef L7_DOT1AG_PACKAGE
    if ((frameData[16] == 0x89) && (frameData[17] == 0x02))
    {
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_FALSE);
    }
    else
#endif
    {
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_TRUE);
    }

    rv = bcmx_tx_uc (&bcm_pkt, hapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL ); 

    if ( (hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID) &&
         (dapi_g->system->dvlanEnable == L7_FALSE)
       )
    {
      if(eapolRobo == L7_TRUE)
      {
        /* copy into our buffer */
        memcpy(&bcm_pkt.pkt_data->data[0],frameData,12);
        memcpy(&bcm_pkt.pkt_data->data[0],default_mac_dst,6);
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[12] = osapiHtons(L7_ETYPE_8021Q);
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[14] = osapiHtons(BCM_VLAN_CTRL(0, 0, 1));
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[16] = 0x1111;
        frameLength = 18;
        /* clear the padded out area */
        memset(&bcm_pkt.pkt_data->data[frameLength], 0, (64 - frameLength));
        bcm_pkt.pkt_data->len = 64;
        bcm_pkt.flags = BCM_TX_CRC_APPEND ;
        if (pktTxDebugLevel)
        {
          hapiBroadDebugPktDump(bcm_pkt.pkt_data->data);
        }
        bcmx_tx_uc (&bcm_pkt, hapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL ); 
      }
    } 
    
    bcmTxRv = rv;
    frameSent = L7_TRUE;
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      hapiTxDebugCounters[frameType]++;
    }
    break;

  case DAPI_FRAME_TYPE_MCAST_DOMAIN:

    rv = bcmx_lplist_init(&mcastLplist,L7_MAX_INTERFACE_COUNT,0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Failed transmission: rv=%d", rv);
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);
      return L7_FAILURE;
    }
 
    rv = bcmx_lplist_init(&taggedLplist,L7_MAX_INTERFACE_COUNT,0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Failed transmission: rv=%d", rv);
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);
      bcmx_lplist_free(&mcastLplist);
      return L7_FAILURE;
    }

    rv = bcmx_lplist_init(&untaggedLplist,L7_MAX_INTERFACE_COUNT,0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Failed transmission: rv=%d", rv);
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);
      bcmx_lplist_free(&mcastLplist);
      bcmx_lplist_free(&taggedLplist);
      return L7_FAILURE;
    }

    /*
     * Get port bitmap for this vlan from BCM
     */
    hapiBroadGetSameVlanPbmpTx(cmdInfo->cmdData.send.vlanID, &mcastLplist, L7_TRUE, dapi_g);
    hapiBroadPruneTxDiscardingPorts(cmdInfo->cmdData.send.vlanID, &mcastLplist, dapi_g);

    /* if this is not eapol, only send on authorized ports */
    if (etype_eapol == L7_FALSE)
    {
      hapiBroadPruneTxUnauthorizedPorts(&mcastLplist,dapi_g);
    }
    if (hapiBroadTaggedStatusLplistSet(cmdInfo->cmdData.send.vlanID,
                                       &mcastLplist,&taggedLplist,&untaggedLplist,
                                       dapi_g) != L7_SUCCESS)
    {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Failed transmission");
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);
      bcmx_lplist_free(&mcastLplist);
      bcmx_lplist_free(&taggedLplist);
      bcmx_lplist_free(&untaggedLplist);
      return L7_FAILURE;
    }

    if(hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
    {
      /* Always send without a tag.
      */
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_TRUE);
    }

#ifndef L7_CHASSIS 
    rv = bcmx_tx_lplist_intercept (&bcm_pkt, &mcastLplist, &untaggedLplist, BCMX_TX_F_CPU_TUNNEL, dapi_g); /* sdk 5.3.1 change */
#else
    rv = bcmx_tx_port_list(&mcastLplist, &bcm_pkt); 
#endif
    if ( (hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID) &&
         (dapi_g->system->dvlanEnable == L7_FALSE)
       )
    {
      if(eapolRobo == L7_TRUE)
      {
        /* copy into our buffer */
        memcpy(&bcm_pkt.pkt_data->data[0],frameData,12);
        memcpy(&bcm_pkt.pkt_data->data[0],default_mac_dst,6);
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[12] = osapiHtons(L7_ETYPE_8021Q);
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[14] = osapiHtons(BCM_VLAN_CTRL(0, 0, 1));
        *(L7_ushort16 *)&bcm_pkt.pkt_data->data[16] = 0x1111;
        frameLength = 18;
        /* clear the padded out area */
        memset(&bcm_pkt.pkt_data->data[frameLength], 0, (64 - frameLength));
        bcm_pkt.pkt_data->len = 64;
        bcm_pkt.flags = BCM_TX_CRC_APPEND ;
        if (pktTxDebugLevel)
        {
          hapiBroadDebugPktDump(bcm_pkt.pkt_data->data);
        }
        rv = bcmx_tx_lplist_intercept (&bcm_pkt, &mcastLplist, &untaggedLplist, BCMX_TX_F_CPU_TUNNEL, dapi_g); /* sdk 5.3.1 change */
      }
    } 
      bcmTxRv = rv;
      frameSent = L7_TRUE;
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        hapiTxDebugCounters[frameType]++;
      }
    break;

  case DAPI_FRAME_TYPE_NO_L2_EGRESS_MCAST_DOMAIN:

    rv = bcmx_lplist_init(&mcastLplist,L7_MAX_INTERFACE_COUNT,0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Failed transmission: rv=%d", rv);
      sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
      sal_dma_free(bcm_pkt.pkt_data->data);
      return L7_FAILURE;
    }

    /*
     * Get port bitmap for this vlan from BCM
     */
    hapiBroadGetSameVlanPbmpTx(cmdInfo->cmdData.send.vlanID, &mcastLplist, L7_TRUE, dapi_g);

    /* if this is not eapol, only send on authorized ports */
    if (etype_eapol == L7_FALSE)
    {
      hapiBroadPruneTxUnauthorizedPorts(&mcastLplist,dapi_g);
    }

    if (BCMX_LPLIST_IS_EMPTY(&mcastLplist) == L7_FALSE)
    {
      /* will be proccessed in hardware */
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_FALSE);

      rv = bcmx_tx_lplist_intercept (&bcm_pkt, &mcastLplist, NULL, BCMX_TX_F_CPU_TUNNEL, dapi_g); /* sdk 5.3.1 change */
      bcmTxRv = rv;
      frameSent = L7_TRUE;
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        hapiTxDebugCounters[frameType]++;
      }
    }
    break;

  default:
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s unsupported connection type\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }

  /* Handle mirror */
  if ((frameSent == L7_TRUE) && hapiBroadProbeActive(dapi_g))
  {
    DAPI_USP_t searchUsp;
    DAPI_PORT_t  *mirrorDapiPortPtr;
    DAPI_PORT_t  *cpuDapiPortPtr;
    BROAD_PORT_t *mirrorHapiPortPtr = L7_NULLPTR;
    BROAD_PORT_t *probeHapiPortPtr = HAPI_PORT_GET(&dapi_g->system->probeUsp,dapi_g);
        
    /* As the packet sent by CPU does not go through the ingress pipeline, we need to
     * explicitly mirror the packet(send a copy). There a few possibilities: 
     * (a) The outgoing/egress port is enabled for mirroring (egress), in which case
     *     pkt needs to be mirrored on to the probe port.
     * (b) The CPU port is enabled for mirroring (ingress), in which case
     *     pkt needs to be mirrored on to the probe port.
     * (c) Both outgoing/egress port and CPU port is enabled for mirroring. To match
     *     the XGS3 mirroring behavior, we need to send 2 copies of packet (one copy
     *     for ingress mirrored pkt, and another for egress mirrored pkt). As the pkt
     *     sent from CPU does not undergo any modifications, the ingress and egress
     *     mirrored copy will be exactly the same.
     */

    /* Check if mirroring is enabled on CPU port */
    if (CPU_USP_GET(&searchUsp) == L7_SUCCESS)
    {
      cpuDapiPortPtr = DAPI_PORT_GET(&searchUsp, dapi_g);
      if ((cpuDapiPortPtr->isMirrored == L7_TRUE) &&
          ((cpuDapiPortPtr->mirrorType == DAPI_MIRROR_INGRESS) ||
           (cpuDapiPortPtr->mirrorType == DAPI_MIRROR_BIDIRECTIONAL)))
      {
        /* Always send mirrored packet w/ tag, to help debug VLAN associations */
        bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_FALSE);

        rv = bcmx_tx_uc(&bcm_pkt, probeHapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL);   
      }
    }
    
    /* Check if mirroring is enabled on outgoing/egress port (port on which pkt was
     * transimitted)
     */
    switch (frameType)
    {
    case DAPI_FRAME_TYPE_DATA_TO_PORT:
    case DAPI_FRAME_TYPE_NO_L2_EGRESS_DATA_TO_PORT:
      /* Check if destined to mirror */
      mirrorDapiPortPtr = DAPI_PORT_GET(&destUsp, dapi_g);
      if (((IS_PORT_TYPE_PHYSICAL(mirrorDapiPortPtr) == L7_TRUE) || 
           (IS_PORT_TYPE_LOGICAL_LAG(mirrorDapiPortPtr) == L7_TRUE)) &&
              ((mirrorDapiPortPtr->isMirrored == L7_TRUE) && 
                   ((mirrorDapiPortPtr->mirrorType == DAPI_MIRROR_BIDIRECTIONAL) || 
                    (mirrorDapiPortPtr->mirrorType == DAPI_MIRROR_EGRESS)))) 
      {
        if(hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
        {
          rv = bcmx_tx_uc(&bcm_pkt, probeHapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL); 
        }
      }
      break;

    case DAPI_FRAME_TYPE_MCAST_DOMAIN:
     
      for (searchUsp.unit = 0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++) 
      {
        for (searchUsp.slot = 0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++) 
        {
          if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardId != L7_NULL) 
          {
            for (searchUsp.port = 0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++) 
            {
              if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
              {
                continue;
              }

              mirrorDapiPortPtr = DAPI_PORT_GET(&searchUsp, dapi_g);
              if (((IS_PORT_TYPE_PHYSICAL(mirrorDapiPortPtr) == L7_TRUE) || 
                   (IS_PORT_TYPE_LOGICAL_LAG(mirrorDapiPortPtr) == L7_TRUE)) &&
                      ((mirrorDapiPortPtr->isMirrored == L7_TRUE) && 
                           ((mirrorDapiPortPtr->mirrorType == DAPI_MIRROR_BIDIRECTIONAL) || 
                            (mirrorDapiPortPtr->mirrorType == DAPI_MIRROR_EGRESS)))) 
              {
                if (IS_PORT_TYPE_LOGICAL_LAG(mirrorDapiPortPtr) == L7_TRUE)
                {
                  hapiBroadLagCritSecEnter ();
                  for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
                  {
                    if (mirrorDapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
                    {
                      mirrorHapiPortPtr = HAPI_PORT_GET(&mirrorDapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
                      break;
                    }
                  }
                  hapiBroadLagCritSecExit ();
                  if (i == L7_MAX_MEMBERS_PER_LAG)
                  {
                    continue;
                  }
                }
                else
                {
                  mirrorHapiPortPtr = HAPI_PORT_GET(&(searchUsp), dapi_g);
                }
                  
                if (BROAD_IS_VLAN_TAGGING(&(searchUsp),sendVlanId,dapi_g))
                {
                  rv = bcmx_lplist_index_get(&taggedLplist,mirrorHapiPortPtr->bcmx_lport);
                  if (rv >= 0)
                  {
                    bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_FALSE);
          
                    rv = bcmx_tx_uc(&bcm_pkt, probeHapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL ); 
                  }
                }
                else
                {
                  rv = bcmx_lplist_index_get(&untaggedLplist,mirrorHapiPortPtr->bcmx_lport);
                  if (rv >= 0)
                  {
                    bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_TRUE);
          
                    rv = bcmx_tx_uc(&bcm_pkt, probeHapiPortPtr->bcmx_lport,BCMX_TX_F_CPU_TUNNEL); 
                  }
                }
              }
            }
          }
        }
      }
                  break;

    case DAPI_FRAME_TYPE_NO_L2_EGRESS_MCAST_DOMAIN:
      
      for (searchUsp.unit = 0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++) 
      {
        for (searchUsp.slot = 0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++) 
        {
          if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardId != L7_NULL) 
          {
            for (searchUsp.port = 0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++) 
            {
              if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
              {
                continue;
              }

              mirrorDapiPortPtr = DAPI_PORT_GET(&searchUsp, dapi_g);
              if (((IS_PORT_TYPE_PHYSICAL(mirrorDapiPortPtr) == L7_TRUE) || 
                   (IS_PORT_TYPE_LOGICAL_LAG(mirrorDapiPortPtr) == L7_TRUE)) &&
                      ((mirrorDapiPortPtr->isMirrored == L7_TRUE) && 
                           ((mirrorDapiPortPtr->mirrorType == DAPI_MIRROR_BIDIRECTIONAL) || 
                            (mirrorDapiPortPtr->mirrorType == DAPI_MIRROR_EGRESS)))) 
              {
                if (IS_PORT_TYPE_LOGICAL_LAG(mirrorDapiPortPtr) == L7_TRUE)
                {
                  hapiBroadLagCritSecEnter ();
                  for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
                  {
                    if (mirrorDapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
                    {
                      mirrorHapiPortPtr = HAPI_PORT_GET(&mirrorDapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
                      break;
                    }
                  }
                  hapiBroadLagCritSecExit ();
                  if (i == L7_MAX_MEMBERS_PER_LAG)
                  {
                    continue;
                  }
                }
                else
                {
                  mirrorHapiPortPtr = HAPI_PORT_GET(&(searchUsp), dapi_g);
                }
                  
                rv = bcmx_lplist_index_get(&mcastLplist,mirrorHapiPortPtr->bcmx_lport);
                if (rv >= 0)
                {
                  rv = bcmx_tx_uc(&bcm_pkt, probeHapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL); 
                }
              }
            }
          }
        }
      }
                break;

    default:
      break;
    }
  }
  
    /* added for 16728 */ 
    switch (frameType)
    {
    
    case DAPI_FRAME_TYPE_MCAST_DOMAIN:
      bcmx_lplist_free(&mcastLplist);
      bcmx_lplist_free(&taggedLplist);
      bcmx_lplist_free(&untaggedLplist);
      break;
    case DAPI_FRAME_TYPE_NO_L2_EGRESS_MCAST_DOMAIN:
      bcmx_lplist_free(&mcastLplist);
      break;
    default:
      break;
    }
    /* end of added for 16728 */
  
#if 0 /* PTIN Removed */
  /* increment the CPU stats */
  hapiBroadStatsCpuIncrement(bcm_pkt.pkt_data->data, 
                             bcm_pkt.pkt_data->len,
                             usp,dapi_g,L7_FALSE,L7_FALSE);
#endif

  /* PTin added: transmission */
  /* Dump first 64 bytes */
  if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_STDOUT)
  {
    printf("Packet transmited on usp={%d,%d,%d} (lport=0x%08x) with sendVLAN=%u (frameType=%u, flags=0x%x): bcmTxRv=%d result=%d\r\n",
           destUsp.unit, destUsp.slot, destUsp.port, hapiPortPtr->bcmx_lport, cmdInfo->cmdData.send.vlanID, frameType, bcm_pkt.flags, bcmTxRv, result);
    fflush(stdout);
  }

  if (cpu_transmited_packets_dump)
  {
    int i;
    for (i=0; i<bcm_pkt.pkt_data->len && i<64; i++)
    {
      if (i%16==0)
      {
        if (i!=0)
          printf("\r\n");
        printf(" 0x%02x:",i);
      }
      printf(" %02x",bcm_pkt.pkt_data->data[i]);
    }
    printf("\r\n");
  }
  fflush(stdout);

  /* Free the frame from the buffer */
  sysapiNetMbufFree(cmdInfo->cmdData.send.frameHdl);
  sal_dma_free(bcm_pkt.pkt_data->data);

  if (bcmTxRv != BCM_E_NONE)
  {
    osapiTaskYield();
  }

  if (cpu_transmit_debug & CPU_INTERCEPT_DEBUG_LEVEL1)
  {
    PT_LOG_DEBUG(LOG_CTX_HAPI, "Sending to usp %d.%d.%d (bcmx_lport 0x%08x), with sendVLAN %u (frameType %u, pktflags 0x%x): bcmTxRv=%d result=%d",
              destUsp.unit, destUsp.slot, destUsp.port, hapiPortPtr->bcmx_lport, cmdInfo->cmdData.send.vlanID, frameType, bcm_pkt.flags, bcmTxRv, result);
  }

  return result;

}

/* PTin added */
void ptin_ReplaceVid(L7_ushort16 vlanId, L7_uchar8 *data)
{
  /* Replace tag */

  data[14] &= 0xF0;
  data[15] =  0;

  data[14] |= (L7_uchar8) ((vlanId >> 8) & 0x0F);
  data[15] = (L7_uchar8) ( vlanId       & 0xFF);
}
/* PTin end */

L7_uint32 hapiRxQueueLostMsgs = 0;
/*********************************************************************
*
* @purpose All data frames come through this
*
* @param   L7_netBufHandle  frameHdl - frame handle for this data 
* @param   DAPI_USP_t      *usp      - Used to notify the application of the frame.
*                                      Needs to be a valid usp.
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
bcm_rx_t hapiBroadReceive(L7_int32 unit, bcm_pkt_t *bcm_pkt, void *cookie)
{
  DAPI_USP_t           usp;
  DAPI_USP_t           tempUsp;
  DAPI_t              *dapi_g = (DAPI_t*)cookie;
  DAPI_FRAME_CMD_t     cmdInfo;
  L7_uchar8           *pkt;
  L7_8021QTag_t       *vlanTagPtr;
  L7_netBufHandle      frameHdl=L7_NULL;
  bcm_rx_t             result = BCM_RX_NOT_HANDLED;
  BROAD_PKT_RX_MSG_t   pktRxMsg;
  bcmx_uport_t         uport;
  L7_uint32            frameLength = bcm_pkt->pkt_len;
  L7_uchar8            gmrpPduMac[] = {0x01,0x80,0xC2,0x00,0x00,0x20};
  L7_uchar8            gvrpPduMac[] = {0x01,0x80,0xC2,0x00,0x00,0x21};
  L7_uchar8            lacPduMac[] = {0x01,0x80,0xC2,0x00,0x00,0x02};
  L7_uchar8            bpduMac[] = {0x01,0x80,0xC2,0x00,0x00,0x00};
  L7_uchar8            cdpMac[6]={0x01,0x00,0x0c,0xcc,0xcc,0xcc};
  L7_uchar8            lldpEthType[] = {0x88, 0xCC};
  L7_uchar8            dot3ahEthType[] = {0x88, 0x09};
  DAPI_USP_t           gmrpUspCheck;
  int                  lport;
  DAPI_ADDR_MGMT_CMD_t macAddressInfo;
  BROAD_PORT_t        *hapiPortPtr;
  L7_DOT1X_PORT_STATUS_t dot1xStatus;
  L7_ushort16          innerVlanId=0,outerVlanId=0;
  static L7_BOOL       idChecked = L7_FALSE;
  static L7_BOOL       weAreHawkeye = L7_FALSE;
  bcm_chip_family_t    board_family;
  L7_ushort16          ether_type;
  L7_BOOL              isTriumphFamily = L7_FALSE;

  /* PTin added: IGMP snooping */
  #if 1
  if (!debug_pktTimer.first_pkt)
  {
    debug_pktTimer.time_start = osapiTimeMillisecondsGet();
    debug_pktTimer.first_pkt = L7_TRUE;
  }
  debug_pktTimer.pkt_cpu_counter++;

  /* Increment number of received packets */
  hapiBroadReceive_packets_count++;
  #endif

  // PTin
  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_STDOUT)
  {
    printf("%s(%d) Lowest level reception: (reason=%u [%u,%u,%u]) %u rxport:%u, srcport=%u, vid=%u, length=%d\n", __FUNCTION__, __LINE__,
           bcm_pkt->rx_reason,bcm_pkt->rx_reasons.pbits[0],bcm_pkt->rx_reasons.pbits[1],bcm_pkt->rx_reasons.pbits[2],bcm_pkt->cos,
           bcm_pkt->rx_port,bcm_pkt->src_port,bcm_pkt->vlan, bcm_pkt->pkt_len);

    printf("rx_timestamp %d, rx_timestamp_upper %d, timestamp_flags %d\n\r", bcm_pkt->rx_timestamp, bcm_pkt->rx_timestamp_upper, bcm_pkt->timestamp_flags);
    fflush(stdout);
  }
  else if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL1)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI, "Lowest level reception: (reason=%u [%u,%u,%u]) %u rxport:%u, srcport=%u, vid=%d, length=%d\n",
              bcm_pkt->rx_reason,bcm_pkt->rx_reasons.pbits[0],bcm_pkt->rx_reasons.pbits[1],bcm_pkt->rx_reasons.pbits[2],bcm_pkt->cos,
              bcm_pkt->rx_port,bcm_pkt->src_port,bcm_pkt->vlan, bcm_pkt->pkt_len);

    PT_LOG_TRACE(LOG_CTX_HAPI, "rx_timestamp %d, rx_timestamp_upper %d, timestamp_flags %d\n\r", bcm_pkt->rx_timestamp, bcm_pkt->rx_timestamp_upper, bcm_pkt->timestamp_flags);
  }
  /* PTIN added: PTP Timestamp BCM_PKT_F_xxx flags. */
  else if ((cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL3) && (bcm_pkt->pkt_data->data[0x26]==0x01 && bcm_pkt->pkt_data->data[0x27]==0x3f))
  {
    PT_LOG_TRACE(LOG_CTX_HAPI, "rx_timestamp %d\n\r", bcm_pkt->rx_timestamp);
  }

  // PTin
  ptin_ReplaceVid(bcm_pkt->vlan, bcm_pkt->pkt_data->data);

  /* Dump the number of bytes defined by cpu_intercepted_packets_dump */
  if (cpu_intercepted_packets_dump)
  {
    int i;

    printf("Packet received on rxport %u, srcport %u, length=%d:\n\r",bcm_pkt->rx_port, bcm_pkt->src_port, bcm_pkt->pkt_len);
    for (i=0; i<bcm_pkt->pkt_len && i<cpu_intercepted_packets_dump; i++)
    {
      if (i%16==0)
      {
        if (i!=0)
          printf("\n\r");
        printf(" 0x%02x:",i);
      }
      printf(" %02x",bcm_pkt->pkt_data->data[i]);
    }
    printf("\n\r");
    fflush(stdout);
  }

  /* Check if packet needs to be printed out */
  ptin_debug_trap_packets_show(bcm_pkt->rx_port, bcm_pkt);

  memset(&cmdInfo, 0, sizeof(cmdInfo));

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

#ifdef HAPI_BROAD_PKT_DEBUG
  /* This is for the DebugPktSend test cases -- see broad_debug.c */
  if (hapiBroadDebugPktReceiveEnabled)
  {
    hapiBroadDebugReceiveCounter++;
    return result;
  }
#endif

  if (idChecked == L7_FALSE)
  {
    idChecked = L7_TRUE;
    if (hpcBoardGet()->npd_id == __BROADCOM_53314_ID)
    {
      weAreHawkeye = L7_TRUE;
    }
  }
  gmrpUspCheck.unit = gmrpUspCheck.slot = gmrpUspCheck.port = 0;

  if (dot1adVlanInfoDebug)
  {
    printf ("Rx: OuterVlan -%u-%d-%d\r\n", bcm_pkt->vlan,bcm_pkt->vlan_pri,bcm_pkt->vlan_cfi);
    printf ("Rx: InnerVlan -%u-%d-%d\r\n", bcm_pkt->inner_vlan,bcm_pkt->inner_vlan_pri,
                                              bcm_pkt->inner_vlan_cfi);
    fflush(stdout);
  }

  if (voiceVlanAuthDebug)
  {
    L7_uchar8 i;
    sysapiPrintf("hapiBroadReceive() -> Pkt received:\n\r");
    for (i = 0; i < 20; i++)
    {
      sysapiPrintf("%02x ", bcm_pkt->pkt_data->data[i]);
    }
    sysapiPrintf("\n\r");
  }
#ifdef HAPI_BROAD_PKT_DEBUG
    if (pktRxCallbackDebug)
    {
      SYSAPI_PRINTF (SYSAPI_LOGGING_ALWAYS, ">Received pkt on rx_unit %d, rx_port %d, src_port %d src_trunk %d"
              " flags 0x%x, src_mod %d vlan %d\n", bcm_pkt->rx_unit, bcm_pkt->rx_port,
              bcm_pkt->src_port, bcm_pkt->src_trunk, bcm_pkt->flags, bcm_pkt->src_mod, bcm_pkt->vlan);
    }
#endif

    if (wlanVpDebugRx)
    {
      if (bcm_pkt->rx_port == 54) /* 54 is loopback port */
      {
        L7_uint32 k;
        SYSAPI_PRINTF (SYSAPI_LOGGING_ALWAYS, "\n\n %s: wlanvp len %d rx_unit %d, rx_port %d, src_port %d, src_gport %d, "
                       " flags 0x%x, cos %d, vlan %d \n", __FUNCTION__, bcm_pkt->pkt_len, bcm_pkt->rx_unit, bcm_pkt->rx_port,
                       bcm_pkt->src_port, bcm_pkt->src_gport, bcm_pkt->flags, bcm_pkt->cos, bcm_pkt->vlan);
        for (k=0; k<wlanVpDebugSize && k<bcm_pkt->pkt_len; k++)
        {
          if (k%32 == 0 && k != 0)
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
          }
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "%02x", bcm_pkt->pkt_data->data[k]);
        }
      }
      if (bcm_pkt->pkt_data->data[47] == 0x10 && 
           bcm_pkt->pkt_data->data[48] == 0x02)
      {
        SYSAPI_PRINTF (SYSAPI_LOGGING_ALWAYS,"<<<CAPWAP ENCAPPED pkt RX on unit %d, port %d, src_port %d src_trunk %d"
                " flags 0x%x, src_mod %d, cos %d, vlan %d\n", bcm_pkt->rx_unit, bcm_pkt->rx_port,
                bcm_pkt->src_port, bcm_pkt->src_trunk, bcm_pkt->flags, bcm_pkt->src_mod, bcm_pkt->cos, bcm_pkt->vlan);

      }
    }

  /* Using bcm_pkt, there is no clean way of identifying whether pkt ingress
   * on a LAG port or normal port. Also, using trunk128 mode on XGS3/5695,
   * changes the way bcm_pkt reports unit/port information. In trunk128 mode, 
   * trunk id (tgid) is not carried over Higig ports .
   */
   
  usp.unit = -1;
  usp.slot = -1;
  usp.port = -1;

  gmrpUspCheck.unit = -1;
  gmrpUspCheck.slot = -1;
  gmrpUspCheck.port = -1;

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  if (hapiBroadGetSystemBoardFamily(&board_family) == L7_SUCCESS)
  {
     if ( (board_family == BCM_FAMILY_TRIUMPH) ||
          (board_family == BCM_FAMILY_TRIUMPH2)
        ) /* Remove the inner tag if the frame is double tagged */
          /* Currently All the double tagged frames comes with
             inner inner TPID as 0x8100.*/
     {
       outerVlanId = osapiNtohs(*(L7_ushort16 *)&bcm_pkt->pkt_data->data[14]) & 0xFFF;

       if (dapi_g->system->dvlanEnable)
       {
         hapiBroadModidModportToLportGet (bcm_pkt->src_mod, bcm_pkt->src_port, &lport);
         uport = BCMX_UPORT_GET(lport);
         if (uport == BCMX_UPORT_INVALID_DEFAULT)
         {
           return result;
         }
         HAPI_BROAD_UPORT_TO_USP(uport,&usp);

         hapiPortPtr = HAPI_PORT_GET(&usp,dapi_g);
         memcpy(&ether_type, &bcm_pkt->pkt_data->data[16], sizeof(ether_type));
         ether_type = osapiNtohs(ether_type);
         if(hapiPortPtr->locked == L7_FALSE)
         {
           outerVlanId = osapiNtohs(*(L7_ushort16 *)&bcm_pkt->pkt_data->data[14]) & 0xFFF;
           if (ether_type == FD_DVLANTAG_802_1Q_ETHERTYPE) /* DT frame */
           {
             innerVlanId = osapiNtohs(*(L7_ushort16 *)&bcm_pkt->pkt_data->data[18]) & 0xFFF;
           }
           hapiBroadInnerTagRemove(bcm_pkt->pkt_data->data, &frameLength);
         }
         else
         {
           outerVlanId = bcm_pkt->vlan;
           if (ether_type == FD_DVLANTAG_802_1Q_ETHERTYPE) /* DT frame */
           {
             innerVlanId = osapiNtohs(*(L7_ushort16 *)&bcm_pkt->pkt_data->data[14]) & 0xFFF;
           }
           *(L7_ushort16 *)&bcm_pkt->pkt_data->data[14] = osapiHtons(bcm_pkt->vlan);
         }
       }
       /* These conditions from the below conditions needes to be removed */
       if ( (outerVlanId > L7_MAX_VLAN_ID) ||
            (innerVlanId > L7_MAX_VLAN_ID)
         )
      {
        return result;
      }
      isTriumphFamily = L7_TRUE;
      if (dot1adVlanInfoDebug) /* Using the existing Debug Message */
      {
        L7_uchar8 i;
        sysapiPrintf("hapiBroadReceive() ->After Inner tag Removal:\n\r");
        for (i = 0; i < 20; i++)
        {
          sysapiPrintf("%02x ", bcm_pkt->pkt_data->data[i]);
        }
        sysapiPrintf("\n\r");
      }
     }
  
  } /* Frames from unknown boards will be dropped further*/

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  if(hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
  {
    outerVlanId = osapiNtohs((*(L7_ushort16 *)&bcm_pkt->pkt_data->data[14]) & 0xFFF);
    if (dapi_g->system->dvlanEnable)
    {
      innerVlanId = osapiNtohs((*(L7_ushort16 *)&bcm_pkt->pkt_data->data[18]) & 0xFFF);
    }
    if ( (outerVlanId > L7_MAX_VLAN_ID) ||
         (innerVlanId > L7_MAX_VLAN_ID)
       )
    {
      return result;
    }
    /* Remove the Outer tag */
    hapiBroadSTagRemove(bcm_pkt->pkt_data->data, &frameLength);
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  {
    if (bcm_pkt->rx_port == 54 && BCM_GPORT_IS_WLAN_PORT(bcm_pkt->src_gport))
    {
#ifdef L7_WIRELESS_PACKAGE
      if (hapiBroadWlanUspGet(dapi_g, bcm_pkt->src_gport, &usp) != L7_SUCCESS)
      {
        /* WLAN usp not completely up yet */
        return result;
      }
#endif
    }
    else
    {
#ifndef L7_ROBO_SUPPORT
     int bcm_unit=0,bcm_port=0;
#endif
     /* This code assumes that trunk128 mode is enabled on XGS3 devices.
     ** At this point we need to know the physical port.
     */
#if L7_ROBO_SUPPORT
     hapiBroadModidModportToLportGet (bcm_pkt->rx_unit, bcm_pkt->rx_port, &lport);
#else 

   if (weAreHawkeye == L7_TRUE) 
     { 
        bcm_unit = bcm_pkt->src_mod;
        bcm_port = bcm_pkt->rx_port;
     }
    else
     {
        bcm_unit = bcm_pkt->src_mod;
        bcm_port = bcm_pkt->src_port;
     }
     hapiBroadModidModportToLportGet (bcm_unit, bcm_port, &lport);
#endif
          

     uport = BCMX_UPORT_GET(lport);
     if (uport == BCMX_UPORT_INVALID_DEFAULT)
     {
       ++hapiLportToUportConvertErrors;             
       return result;
     }
 
     HAPI_BROAD_UPORT_TO_USP(uport,&usp);
     gmrpUspCheck = usp;
  }

  if (isValidUsp(&usp,dapi_g) == L7_FALSE)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                      "hapiBroadReceive: Invalid USP (%d.%d.%d)\n", usp.unit,usp.slot,usp.port);
     return result;
  }
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  /* check if the receiving port is locked and if this pkt is a source lookup failure 
  */
  tempUsp = usp;
  hapiPortPtr = HAPI_PORT_GET(&tempUsp,dapi_g);

  dot1xStatus = hapiPortPtr->dot1x.dot1xStatus;
  if (dot1xStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
  {
    L7_uchar8   srcPort[] = {0x00, 0x44};
    L7_uchar8   dstPort[] = {0x00, 0x43};
    L7_uchar8   bootRequest = 0x01;
    L7_BOOL     unAuthMode = L7_DISABLE;

    extern L7_RC_t voiceVlanPortAuthGet(L7_uint32 intIfNum,L7_BOOL *state);
    extern L7_RC_t nimGetIntIfNumFromUSP(nimUSP_t* usp, L7_uint32 *intIfNum);
    extern L7_RC_t hapiBroadVoiceVlanStpSet(bcmx_lport_t lport, L7_BOOL state, DAPI_t *dapi_g);

    unAuthMode = hapiPortPtr->voiceVlanPort.voiceVlanUnauthMode;

    if (voiceVlanAuthDebug)
    {
      sysapiPrintf("--> DOT1X PORT UNAUTH MODE\n\r");
      sysapiPrintf("--> Voice VLAN UnauthMode:[%s]\n\r",((unAuthMode == L7_ENABLE)?"ENABLE":"DISABLE"));
      if (!memcmp(cdpMac, &bcm_pkt->pkt_data->data[0], 6))
      {
        sysapiPrintf("--> CDP Pkt Received on port[%d]\n\r",bcm_pkt->rx_port);
      }
      else if  ((!memcmp(srcPort, &bcm_pkt->pkt_data->data[38], 2) &&
                !memcmp(dstPort, &bcm_pkt->pkt_data->data[40], 2) &&
                (bcm_pkt->pkt_data->data[46] == bootRequest)))
      {
        sysapiPrintf("--> DHCP Pkt Received on port[%d]\n\r",bcm_pkt->rx_port);
      }
      else if (!memcmp(lldpEthType, &bcm_pkt->pkt_data->data[16], 2))
      {
        sysapiPrintf("--> LLDP Pkt Received on port[%d]\n\r",bcm_pkt->rx_port);
      }
      sysapiPrintf("--> unit:[%d] src_port:[%d] dest_port[%d]\n\r",bcm_pkt->unit, bcm_pkt->src_port,bcm_pkt->dest_port);
      sysapiPrintf("--> rx_unit:[%d] rx_port:[%d] rx_reason:[%d]\n\r",bcm_pkt->rx_unit, bcm_pkt->rx_port,bcm_pkt->rx_reason);
    }

    /* If the packet is not EAPOL, and is from an unauthorized client, notify dot1x application. */
    /* Even though we do not want unauthorized ports to process bpdus, we need 
     to send the pdu to application in case bpdu guard is enabled, so appln can
     disable the port
  */
      if (!hapiBroadPktIsEapol(&bcm_pkt->pkt_data->data[0]) &&
          (hapiPortPtr->bpduGuardMode != L7_TRUE ||
           (!hapiBroadPktIsBPDU(&bcm_pkt->pkt_data->data[0]))
           ))
      {
        if (hapiPortPtr->dot1x.violationCallbackEnabled == L7_TRUE)
        {
            if ((hapiPortPtr->dot1x.authorizedClientCount == 0) ||
                (hapiBroadDot1xIsAuthorizedClient(&tempUsp, &bcm_pkt->pkt_data->data[6], dapi_g) == L7_FALSE))
            {
                 macAddressInfo.cmdData.unsolIntfMacUnknownAddr.getOrSet = DAPI_CMD_SET;
                 macAddressInfo.cmdData.unsolIntfMacUnknownAddr.vlanID   = osapiNtohs((*(L7_ushort16 *)(&bcm_pkt->pkt_data->data[14])) & 0x0fff);
                 memcpy( macAddressInfo.cmdData.unsolIntfMacUnknownAddr.macAddr.addr,
                         &bcm_pkt->pkt_data->data[6],
                         6 );
                 if ( dapiCallback( &tempUsp,
                                    DAPI_FAMILY_ADDR_MGMT,
                                    DAPI_CMD_ADDR_UNSOLICITED_EVENT,
                                    DAPI_EVENT_ADDR_DOT1X_UNAUTH,
                                    &macAddressInfo ) == L7_FAILURE )
                 {
                   SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                                  "\n%s %d: call to dapiCallback\n",
                                  __FILE__,
                                  __LINE__ );
                 }
        }
      }
    }

    if ( (unAuthMode == L7_ENABLE) && (!memcmp(lldpEthType, &bcm_pkt->pkt_data->data[16], 2) ||
         !memcmp(cdpMac, &bcm_pkt->pkt_data->data[0], 6)) )
    {
      /*
       CDP or LLDP PKT
       */
    }
    else if ( (unAuthMode == L7_ENABLE) && (!memcmp(srcPort, &bcm_pkt->pkt_data->data[38], 2) &&
              !memcmp(dstPort, &bcm_pkt->pkt_data->data[40], 2) &&
              (bcm_pkt->pkt_data->data[46] == bootRequest)) )
    {
      /*
       DHCP PKT

       */
    }
    else
    {
      /* Only allow EAP packets and packets from authorized clients */
      /* Even though we do not want unauthorized ports to process bpdus, 
         we need to send the pdu to application in case bpdu guard is enabled, 
         so appln can disable the port */
      if ((!hapiBroadPktIsEapol(&bcm_pkt->pkt_data->data[0])) &&
          (hapiPortPtr->bpduGuardMode != L7_TRUE ||
           (!hapiBroadPktIsBPDU(&bcm_pkt->pkt_data->data[0]))))
      {
        if (hapiPortPtr->dot1x.violationCallbackEnabled == L7_TRUE)
        {
            if ((hapiPortPtr->dot1x.authorizedClientCount == 0) ||
                (hapiBroadDot1xIsAuthorizedClient(&tempUsp, &bcm_pkt->pkt_data->data[6], dapi_g) == L7_FALSE))
            {
                 return BCM_RX_HANDLED;
          }
        }
        else
        {
          return BCM_RX_HANDLED;
        }
      }
    }
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
  {
    tempUsp = hapiPortPtr->hapiModeparm.physical.lagUsp;
    hapiPortPtr = HAPI_PORT_GET(&tempUsp,dapi_g);
  }

  /* If ingress filtering is enabled on the port and the port is not a member of the 
     VLAN, drop the packet. Note that the classifier may be forcing packets to the CPU
     that the HW would have otherwise dropped due to ingress filtering. 
     The exception here will be to allow EAP packets and packets to the reserved MAC 
     addresses (BPDUs, LACPDUs, GVRP PDUs) */
  if (hapiPortPtr->ingressFilteringEnabled)
  {
    L7_ushort16 vlanId;
    L7_uchar8   reservedMacDa[] = {0x01,0x80,0xc2,0x00,0x00};
    DAPI_PORT_t *dapiPortPtr;

    dapiPortPtr = GET_DAPI_PORT(dapi_g, &tempUsp);
    /* Do not check for ingress filtering on port based routing interface. 
     * Reserved VLAN membership is not stored in port's HAPI structure.
     */
    if (dapiPortPtr->modeparm.physical.routerIntfEnabled != L7_TRUE)
    {
      vlanId = osapiNtohs(*(L7_ushort16 *)&bcm_pkt->pkt_data->data[14]) & 0xFFF;

      if(hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
      {
        /* Ingress filtering should take place based on the 
           outer vlan */
        if (!BROAD_IS_VLAN_MEMBER(&tempUsp, outerVlanId, dapi_g))
        {
          if (!hapiBroadPktIsEapol(&bcm_pkt->pkt_data->data[0]))
          {
            if (memcmp(&bcm_pkt->pkt_data->data[0], reservedMacDa, sizeof(reservedMacDa)) != 0)
            {
              return BCM_RX_HANDLED;
            }
          }
        }
      }
      else if (!BROAD_IS_VLAN_MEMBER(&tempUsp, vlanId, dapi_g))
      {
        if (!hapiBroadPktIsEapol(&bcm_pkt->pkt_data->data[0]))
        {
          if (memcmp(&bcm_pkt->pkt_data->data[0], reservedMacDa, sizeof(reservedMacDa)) != 0)
          {
            return BCM_RX_HANDLED;
          }
        }
      }
    }
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  if ((hapiPortPtr->locked == L7_TRUE) && (dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED))
  {
#ifndef L7_ROBO_SUPPORT
    if ((BCM_RX_REASON_GET(bcm_pkt->rx_reasons, bcmRxReasonL2SourceMiss)) ||
        (BCM_RX_REASON_GET(bcm_pkt->rx_reasons, bcmRxReasonL2Move)))
#else
    if(1) /* MAC-LOCK fix for ROBO */
#endif /* L7_ROBO_SUPPORT */
    {
      /* If DHCP Snooping is enabled on this port and this packet is DHCP,
            allow it to go to the application. */
      if (!((hapiPortPtr->dhcpSnoopingEnabled == L7_TRUE) && (hapiBroadPktIsDhcp(&bcm_pkt->pkt_data->data[0]))))
      {
        /* discard pkt, notify application about violator */
        macAddressInfo.cmdData.unsolIntfMacUnknownAddr.getOrSet = DAPI_CMD_SET;
        macAddressInfo.cmdData.unsolIntfMacUnknownAddr.vlanID   = (osapiNtohs(*(L7_ushort16 *)(&bcm_pkt->pkt_data->data[14]))) & 0x0fff;
        memcpy (macAddressInfo.cmdData.unsolIntfMacUnknownAddr.macAddr.addr, &bcm_pkt->pkt_data->data[6], 6);

        if ( (macAddressInfo.cmdData.unsolIntfMacUnknownAddr.vlanID <= L7_MAX_VLAN_ID ) &&
            ( hapiBroadQvlanVlanExists(macAddressInfo.cmdData.unsolIntfMacUnknownAddr.vlanID) == L7_TRUE) )
        {

          if (dapiCallback(&tempUsp,
                           DAPI_FAMILY_ADDR_MGMT,
                           DAPI_CMD_ADDR_UNSOLICITED_EVENT,
                           DAPI_EVENT_ADDR_LOCK_UNKNOWN_ADDRESS,
                           &macAddressInfo) == L7_FAILURE)
          {
            /* do nothing */
          }
        }

        return BCM_RX_HANDLED;
      }
    }
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  /* If port is not locked, Captive portal is not enabled and 802.1X-authorized and software learning is enabled 
  ** then learn the frame MAC address.
  */
  if ((hapiPortPtr->locked == L7_FALSE) && 
      (hapiPortPtr->cpEnabled == L7_FALSE) &&
      (dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED) &&
      (hpcSoftwareLearningEnabled () == L7_TRUE))
  {

#ifdef L7_STACKING_PACKAGE
    /* In stacking builds the hardware unhelpfully loses rx_reson codes for packets
    ** received on remote switches. This breaks software learning mode.
    **
    ** Learning packets from remote switches are always received with COS 0, 
    ** so if COS is 0 and the source MAC
    ** of the packet is not already learned then we assume that this is a learning packet. 
    */
    if (bcm_pkt->cos == 0)
    {
      BCM_RX_REASON_SET(bcm_pkt->rx_reasons, bcmRxReasonL2SourceMiss);
    }

    /* In stacking builds the driver maintains a software table of learned MAC addresses.
    ** Since the switch may receive multiple copies of a learning frame it saves
    ** a lot of CPU resources to first check whether the address is already learned
    ** before attempting to learn it again.
    */
    if ((BCM_RX_REASON_GET(bcm_pkt->rx_reasons, bcmRxReasonL2SourceMiss)) ||
        (BCM_RX_REASON_GET(bcm_pkt->rx_reasons, bcmRxReasonL2Move)))
    {
      if (usl_mac_frame_learned(bcm_pkt) == 0) 
      {
        /* Pretend that we have a learning frame.
         */

        hapiBroadAddrMacFrameLearn(bcm_pkt, dapi_g);
      }
    }
#else
    /* Note than on 5690 if a frame misses the SA and DA lookup then CPU gets
    ** two copies of the frame. One with only the BCM_RX_PR_L2SRC_MISS set, 
    ** and the other with both, the L2DST_MISS and L2SRC_MISS. 
    ** If other architectures don't send two copies of the frame then the 
    ** learning needs to be moved before the L2DST_MISS drop.
    */
    if ((BCM_RX_REASON_GET(bcm_pkt->rx_reasons, bcmRxReasonL2SourceMiss)) ||
        (BCM_RX_REASON_GET(bcm_pkt->rx_reasons, bcmRxReasonL2Move)))
    {
      hapiBroadAddrMacFrameLearn(bcm_pkt, dapi_g);
    }
#endif /* L7_STACKING_PACKAGE */

    /* If this frame arrived due to a destination miss then drop the frame.
    ** The CPU is not interested in frames that were not captured by specific
    ** filter or FDB entry.
    */
#ifndef PC_LINUX_HOST /* packets not received by DestMiss any longer, just remove for sim though */
    if (BCM_RX_REASON_GET(bcm_pkt->rx_reasons, bcmRxReasonL2DestMiss))
    {
      return BCM_RX_NOT_HANDLED;
    }
#endif
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  /* If the port is not forwarding, drop the frame. The exceptions to this 
     are BPDUs, LACPDUs, EAPOLs, LLDP, ISDP, DOT3AH, and DOT1AG CCMs...
     they must be processed regardless of STP state. */
  outerVlanId = osapiNtohs(*(L7_ushort16 *)&bcm_pkt->pkt_data->data[14]) & 0xFFF;
  if (!hapiBroadPortIsForwarding(&tempUsp, outerVlanId, dapi_g))
  {
    L7_BOOL isDot1agCcmPdu = L7_FALSE;

    if (memcmp(bcm_pkt->pkt_data->data, bpduMac, 5) == 0)
    {
      if ((bcm_pkt->pkt_data->data[5] >= 0x30) && 
          (bcm_pkt->pkt_data->data[5] <= 0x37))
      {
        isDot1agCcmPdu = L7_TRUE;
      }
    }

    if ((memcmp(bcm_pkt->pkt_data->data, bpduMac, sizeof(bpduMac)) != 0) &&
        (memcmp(bcm_pkt->pkt_data->data, lacPduMac, sizeof(lacPduMac)) != 0) &&
        !hapiBroadPktIsEapol(bcm_pkt->pkt_data->data) &&
        (memcmp(lldpEthType, &bcm_pkt->pkt_data->data[16], 2) != 0) &&
        (memcmp(dot3ahEthType, &bcm_pkt->pkt_data->data[16], 2) != 0) &&
        (memcmp(cdpMac, &bcm_pkt->pkt_data->data[0], 6) != 0) &&
        (isDot1agCcmPdu == L7_FALSE))
    {
      return BCM_RX_HANDLED;
    }
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  /* At this point the USP will be a valid physical USP in the system */

  /* This check is to see if an untagged gmrp control pdu is coming into 
  ** an interface which has been set up to accept only vlan tagged frames
  ** If it is, then this frame is ignored (not processed further).
  */
  if ((memcmp(bcm_pkt->pkt_data->data, gmrpPduMac, L7_MAC_ADDR_LEN) == 0) &&
      (isValidUsp(&gmrpUspCheck, dapi_g) == L7_TRUE))
  {
    hapiPortPtr = HAPI_PORT_GET(&gmrpUspCheck, dapi_g);

    if ((hapiPortPtr->acceptFrameType == L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED) &&
        (bcm_pkt->rx_untagged & BCM_PKT_OUTER_UNTAGGED))
    {
      return BCM_RX_HANDLED;
    }
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

   /*Drop tagged GVRP PDU's*/
  if ((memcmp(bcm_pkt->pkt_data->data, gvrpPduMac, L7_MAC_ADDR_LEN) == 0) && 
      (!(bcm_pkt->rx_untagged & BCM_PKT_OUTER_UNTAGGED)))
  {
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
    if (!((dapi_g->system->dvlanEnable == L7_TRUE) && 
         ((isTriumphFamily == L7_TRUE) || (hapiBroadRoboCheck() == L7_TRUE))))
    {
      return BCM_RX_HANDLED;
    } 
#else
      return BCM_RX_HANDLED;
#endif
  }
 
  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  /* Check for possible duplicate */
  if (hapiBroadRxDuplicatePkt(bcm_pkt) == L7_TRUE)
  {
    return BCM_RX_HANDLED;
  }
 
  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  /* Get MBUF to carry the frame to DTL */
  if (bcm_pkt->cos >= HAPI_BROAD_INGRESS_BPDU_COS)
  {
    frameHdl = sysapiRxNetMbufGet(L7_MBUF_RX_PRIORITY_HIGH,
                                  L7_MBUF_FRAME_ALIGNED);
  }
  else
  {
    if (bcm_pkt->cos == HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS)
    {
      /* Check for OSPF and RIP packets here... ensure that they have a higher
         Mbuf allocation. */
      L7_uint32        offset;
      L7_uchar8       *data;
      L7_char8        *pEtype;
      L7_ipHeader_t   *ip_header;
      L7_uchar8        ip_ethtype[]  = {0x08, 0x00};
      L7_uchar8        rip_dport[]   = {0x02, 0x08};
      L7_ushort16      dport;
      L7_BOOL          priority = L7_FALSE;
      L7_udp_header_t *udp_header;

      data = bcm_pkt->pkt_data->data;
      offset = sysNetDataOffsetGet(data);
      ip_header = (L7_ipHeader_t *)(data + offset);
      pEtype = &data[16];

      priority = L7_FALSE;
      if (memcmp(pEtype, ip_ethtype, sizeof(ip_ethtype)) == 0)
      {
        switch (ip_header->iph_prot)
        {
        case IP_PROT_OSPFIGP:
          priority = L7_TRUE;
          break;

        case IP_PROT_UDP:
          udp_header = (L7_udp_header_t *)(data + offset + L7_IP_HDR_LEN);
          dport = osapiNtohs(udp_header->destPort);

          if (memcmp(&dport, rip_dport, sizeof(rip_dport)) == 0)
          {
            priority = L7_TRUE;
          }

          break;

        default:
          break;
        }
      }

      if (priority == L7_TRUE)
      {
        frameHdl = sysapiRxNetMbufGet(L7_MBUF_RX_PRIORITY_MID0,
                                      L7_MBUF_FRAME_ALIGNED);
      }
      else
      {
        frameHdl = sysapiRxNetMbufGet(L7_MBUF_RX_PRIORITY_MID1,
                                      L7_MBUF_FRAME_ALIGNED);
      }
    } 
    else
    {
      if ((bcm_pkt->cos == HAPI_BROAD_INGRESS_MED_PRIORITY_COS) ||
          (bcm_pkt->cos == HAPI_BROAD_INGRESS_LOW_PRIORITY_COS))

      {
        frameHdl = sysapiRxNetMbufGet(L7_MBUF_RX_PRIORITY_MID2,
                                      L7_MBUF_FRAME_ALIGNED);
      } 
      else
      {
        frameHdl = sysapiRxNetMbufGet(L7_MBUF_RX_PRIORITY_NORMAL,
                                      L7_MBUF_FRAME_ALIGNED);
      }
    }
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  if (frameHdl == L7_NULL)
  {
    /* As we are out of mbuf, let other tasks run so that mbufs can be freed up.*/

    osapiTaskYield();
    /* just return if we can't get an mbuf... this pkt will not be processed */
    return result;
  }

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL4)
    PT_LOG_TRACE(LOG_CTX_HAPI,"...");

  cmdInfo.cmdData.receive.type      = DAPI_FRAME_TYPE_DATA_TO_PORT;
  cmdInfo.cmdData.receive.frameHdl  = frameHdl;

#ifndef PC_LINUX_HOST /* no CRC in simulation environment */
  /* Reduce the frame length by 4 to keep the size
   * consistant with L7_MAX_FRAME_SIZE. Hence 
   * CRC is excluded here */
#ifndef L7_ROBO_SUPPORT 
   frameLength = frameLength - 4;
#endif
#endif

  /* Copy the packet data to mbuf and set the length */
  pkt = sysapiNetMbufGetDataStart(frameHdl);
  memcpy(pkt, bcm_pkt->pkt_data[0].data, frameLength);

  sysapiNetMbufSetDataLength(frameHdl, frameLength);

  /* parse vlan from frame */
  if ( (hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID) ||
       (isTriumphFamily == L7_TRUE)
     )
  {
    cmdInfo.cmdData.receive.vlanID = outerVlanId;
    cmdInfo.cmdData.receive.innerVlanId = innerVlanId;
  }
  else
  {
    vlanTagPtr = (L7_8021QTag_t*)&pkt[L7_ENET_HDR_SIZE];
    if ((osapiNtohs(vlanTagPtr->tpid) == L7_ETYPE_8021Q))
    {
      cmdInfo.cmdData.receive.vlanID = osapiNtohs(vlanTagPtr->tci) & L7_VLAN_TAG_VLAN_ID_MASK;
    }
    else
    {
      cmdInfo.cmdData.receive.vlanID = 1;
    }
    if (cmdInfo.cmdData.receive.vlanID > L7_MAX_VLAN_ID)
    {
      sysapiNetMbufFree((L7_netBufHandle)frameHdl);
      return result;
    }
    cmdInfo.cmdData.receive.innerVlanId = 0;
  }
  pktRxMsg.usp = usp;
  pktRxMsg.reasons = bcm_pkt->rx_reasons;
  pktRxMsg.rx_untagged = bcm_pkt->rx_untagged;
  cmdInfo.cmdData.receive.timestamp = bcm_pkt->rx_timestamp;       //PTIN added
  pktRxMsg.cmdInfo = cmdInfo;
  pktRxMsg.cos = bcm_pkt->cos;

  if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL3)
    PT_LOG_TRACE(LOG_CTX_HAPI,"Going to send packet to hapiRxQueue queue");

  if (osapiMessageSend(hapiRxQueue,(void *)&pktRxMsg,sizeof(BROAD_PKT_RX_MSG_t),L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    hapiRxQueueLostMsgs++;
    sysapiNetMbufFree((L7_netBufHandle)frameHdl);
  }

  return BCM_RX_HANDLED;
}


/*********************************************************************
*
* @purpose Check for possible duplicate packets
*
* @param   bcm_pkt_t *bcm_pkt - Incoming BCM packet pointer
*
* @returns L7_BOOL result - TRUE if the packet is a duplicate.
*
* @notes   Duplicate packets result in unnecessary overhead and incorrect
*          protocol counters. The duplicate packets to CPU typically 
*          applies to stacking configurations, when the packets arrives on
*          remote units, gets flooded on VLAN and also gets copied to local 
*          CPU. 
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadRxDuplicatePkt(bcm_pkt_t *bcm_pkt)
{

  /* NOTE: For each case, please describe the root cause as to why duplicate 
   * packets can occur.
   *
   * This logic can be removed once we modify the code such that the CPU port is
   * not a member of all VLANs.
   */

  /* Drop duplicate IGMP packets */
  
  /* When L3 is enabled on a port, packets with TTL=1 or packets with IP
   * options are treated as L3 SlowPath and are copied to local CPU. The 
   * mgmt CPU can get 2 copies of the packet (a) tunneled from remote unit
   * (b) flood on incoming VLAN.
   */
   /* Use the Tunneled RX flag to determine the tunneled copy and drop it */ 
   if ((bcm_pkt->flags & BCM_RX_TUNNELLED) &&
       (BCM_RX_REASON_GET(bcm_pkt->rx_reasons, bcmRxReasonL3Slowpath)))
   {
     /* If etherType == 0x0800 */
     if ((bcm_pkt->pkt_data->data[16] == 0x08) && 
         (bcm_pkt->pkt_data->data[17] == 0x00))
     {
       /* If IP protocol == IGMP */ 
       if (bcm_pkt->pkt_data->data[27] == IP_PROT_IGMP)
       {
         return L7_TRUE;  /* Drop the tunneled copy */
       }
     }
   }

  /* Drop duplicate MLD packets. */

  /* Use of the COPY to CPU option to get MLD packets to CPU and MLD packets
   * flood on the incoming VLAN. In stacking configuration, the MLD 
   * packet gets copied to local CPU and gets tunneled to mgmt CPU. As the 
   * mgmt CPU is also part of incoming VLAN (all VLANs basically), it receives
   * another copy of the flooded MLD packet. 
   * Tunneled copy will be on a high COS (e.g. COS 4), and the other is switched
   * on the VLAN and typically comes to the CPU on COS 1. Drop MLD packets on 
   * COS 1.
   *
   */
  if ((hapiBroadMldHwIssueCheck() == L7_FALSE) &&
      (bcm_pkt->cos < HAPI_BROAD_INGRESS_MED_PRIORITY_COS))
  {
    /* If MAC DA == 33:33:xx:xx:xx:xx */
    if ((bcm_pkt->pkt_data->data[0] == 0x33) && (bcm_pkt->pkt_data->data[1] == 0x33))
    {
      /* If etherType == 0x86DD */
      if ((bcm_pkt->pkt_data->data[16] == 0x86) && (bcm_pkt->pkt_data->data[17] == 0xDD))
      {
        /* If nextHeader == ICMPv6 */
        if (bcm_pkt->pkt_data->data[24] == 0x3A)
        {
          /* If ICMPv6 type == 130 to 132 */
          if ((bcm_pkt->pkt_data->data[58] == 130) ||
              (bcm_pkt->pkt_data->data[58] == 131) ||
              (bcm_pkt->pkt_data->data[58] == 132))
          {
            return L7_TRUE;
          }
        }
        /* If nextHeader == hop-by-hop extension header. */
        else if (bcm_pkt->pkt_data->data[24] == 0x00)
        {
          L7_uint32 hopByHopExtensionSize;

          hopByHopExtensionSize = (bcm_pkt->pkt_data->data[59] + 1) * 8;

          /* If nextHeader == ICMPv6 */
          if (bcm_pkt->pkt_data->data[58] == 0x3A)
          {
            /* If ICMPv6 type == 130 to 132 */
            if ((bcm_pkt->pkt_data->data[58 + hopByHopExtensionSize] == 130) ||
                (bcm_pkt->pkt_data->data[58 + hopByHopExtensionSize] == 131) ||
                (bcm_pkt->pkt_data->data[58 + hopByHopExtensionSize] == 132))
            {
              return L7_TRUE;
            }
          }
        }
      }
    }
  }

  return L7_FALSE;
}

void txStats()
{
  int i;

  for (i = 0; i < DAPI_NUM_OF_FRAME_TYPES; i++) {
          printf(" FrameType %d Dropped %10u\n", i, hapiTxDebugCounters[i]);
  }
  fflush(stdout);
}

void rxStats(int clear)
{
  int i;
  unsigned int totalPktsReceived = 0;
  unsigned int totalPktsDropped = 0;
  printf("hapiRxQueueLostMsgs == %d\n",hapiRxQueueLostMsgs);
  printf("COS Stats in Receive Task\n");
  for (i =0; i < 8;i++)
  {
    printf("COS[%d] Received %10u  Dropped %10u\n",i,
           hapiRxDebugCosCounters[i].pktRxCosRecv,
           hapiRxDebugCosCounters[i].pktRxCosDrops);

    totalPktsReceived += hapiRxDebugCosCounters[i].pktRxCosRecv;
    totalPktsDropped +=  hapiRxDebugCosCounters[i].pktRxCosDrops;
  }
  printf("Packets delivered to Application = %10u\n", 
         totalPktsReceived - totalPktsDropped);
  printf("Packets Dropped on HAPI Receive Task  = %10u\n",totalPktsDropped);
  printf("Packets dropped due to lport to uport conversion error: %d\n",
         hapiLportToUportConvertErrors);
  if (clear != 0) {
    hapiRxQueueLostMsgs = 0;
    for (i =0; i < 8;i++)
    {
      hapiRxDebugCosCounters[i].pktRxCosRecv  = 0;
      hapiRxDebugCosCounters[i].pktRxCosDrops = 0;
    }
    hapiLportToUportConvertErrors = 0;
  }
  fflush(stdout);
}


/*********************************************************************
*
* @purpose Passes received frames to the DTL
*
* @param   L7_uint32        numArgs  - number of arguments
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadReceiveTask(L7_uint32 numArgs, DAPI_t *dapi_g)
{
  L7_netBufHandle      frameHdl=L7_NULL;
  L7_uint32            frameLen;
  BROAD_PKT_RX_MSG_t   pktRxMsg;
  bcm_chip_family_t    board_family;
  L7_uchar8           *pkt;
  L7_BOOL              dropFrame, pktUntagged;
#ifdef HAPI_BROAD_PKT_DEBUG
  L7_BOOL              skip;
#endif


  memset(hapiRxDebugCosCounters,0,sizeof(hapiRxDebugCosCounters));

  while (1)
  {
    if (osapiMessageReceive(hapiRxQueue,(void *)&pktRxMsg,sizeof(BROAD_PKT_RX_MSG_t),L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOG_ERROR(0);
    }

    debug_pktTimer.pkt_cpu2_counter++;

    if ((pktRxMsg.cos < 8) && (pktRxMsg.cos >= 0))
    {
      hapiRxDebugCosCounters[pktRxMsg.cos].pktRxCosRecv++;
    }

    /* default to forward frame */
    dropFrame = L7_FALSE;

#ifdef HAPI_BROAD_PKT_DEBUG
    skip = L7_FALSE;
    if(pktDebugFilter & HAPI_DEBUG_PKT_FILTER_PORT)
    {
      if ((pktRxMsg.usp.unit != dbg_unit) || (pktRxMsg.usp.slot != dbg_slot) || (pktRxMsg.usp.port != dbg_port))
        skip = L7_TRUE;
    }

    if (!skip && pktRxDebug)
    {
      pkt = sysapiNetMbufGetDataStart (pktRxMsg.cmdInfo.cmdData.receive.frameHdl);

      if ((pkt != L7_NULLPTR && !hapiBroadDebugPktIsFiltered(pkt) &&
           ((pktRxDebugQueue == 0) ||
            (pktRxDebugQueue != 0 && (pktRxMsg.cos == (pktRxDebugQueue - 1))))))
      {
        L7_uint32 reason;

        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                      "Received packet on port %d.%d.%d, COS %d\n",
                      pktRxMsg.usp.unit, pktRxMsg.usp.slot,pktRxMsg.usp.port, pktRxMsg.cos);
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Reasons: ");
        for (reason = 0; reason < bcmRxReasonCount; reason++)
        {
          if (BCM_RX_REASON_GET(pktRxMsg.reasons, reason))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "%s ", pkt_reason_names[reason]);
          }
        }
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
      }
    }
    if (dot1adVlanInfoDebug)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Rx:Outer Vlanid: %d, inner Vlanid %d\n",
                    pktRxMsg.cmdInfo.cmdData.receive.vlanID,
                    pktRxMsg.cmdInfo.cmdData.receive.innerVlanId);
    }
    if ((skip == L7_FALSE) && (pktRxDebugLevel) &&
        ((pktRxDebugQueue == 0) ||
         (pktRxDebugQueue != 0 && (pktRxMsg.cos == (pktRxDebugQueue - 1)))))
    {
      pkt = sysapiNetMbufGetDataStart (pktRxMsg.cmdInfo.cmdData.receive.frameHdl);
      hapiBroadDebugPktDump(pkt);
    }
#endif

    frameHdl = pktRxMsg.cmdInfo.cmdData.receive.frameHdl;

    /* Do CPU Mirroring */
    if (hapiBroadProbeActive(dapi_g))
    {
      bcm_pkt_t     bcm_pkt;
      bcm_pkt_blk_t bcm_pkt_blk;
      DAPI_USP_t    cpuUsp;
      DAPI_PORT_t  *cpuDapiPortPtr;
      BROAD_PORT_t *probeHapiPortPtr = HAPI_PORT_GET(&dapi_g->system->probeUsp,dapi_g);
      L7_uchar8    *pkt_start;
      L7_uint32     pkt_len;

      /* Check if mirroring is enabled on CPU port */
      if (CPU_USP_GET(&cpuUsp) == L7_SUCCESS)
      {
        cpuDapiPortPtr = DAPI_PORT_GET(&cpuUsp, dapi_g);
        if ((cpuDapiPortPtr->isMirrored == L7_TRUE) &&
            ((cpuDapiPortPtr->mirrorType == DAPI_MIRROR_EGRESS) ||
             (cpuDapiPortPtr->mirrorType == DAPI_MIRROR_BIDIRECTIONAL)))
        {
          memset (&bcm_pkt,     0, sizeof (bcm_pkt_t));
          memset (&bcm_pkt_blk, 0, sizeof (bcm_pkt_blk_t));

          pkt_start = sysapiNetMbufGetDataStart (frameHdl);
          pkt_len   = sysapiNetMbufGetDataLength(frameHdl);

          bcm_pkt.pkt_data       = &bcm_pkt_blk;
          bcm_pkt.blk_count      = 1;
          bcm_pkt.call_back      = L7_NULLPTR;  /* only used for async responses which we do not use */
          bcm_pkt.pkt_data->data = sal_dma_alloc(pkt_len, "hapiCpuMirrorBuf");
          bcm_pkt.pkt_data->len  = pkt_len;
          bcm_pkt.flags          = BCM_TX_CRC_APPEND;
          bcm_pkt.cos            = pktRxMsg.cos;

          if (bcm_pkt.pkt_data->data != L7_NULL)
          {
            memcpy(bcm_pkt.pkt_data->data, pkt_start, pkt_len);

            /* Always send mirrored packet w/ tag, to help debug VLAN associations */
            bcmx_tx_pkt_untagged_set(&bcm_pkt, L7_FALSE);
            bcmx_tx_uc (&bcm_pkt, probeHapiPortPtr->bcmx_lport, BCMX_TX_F_CPU_TUNNEL);

            sal_dma_free(bcm_pkt.pkt_data->data);
          }
        }
      }
    }

    dapiCardRemovalReadLockTake ();

    do
    {

    memset(&board_family, 0, sizeof(board_family));

    if (hapiBroadGetSystemBoardFamily(&board_family) == L7_FAILURE)
    {
      /* unknown board family drop frame */
      sysapiPrintf("Unkown Board Family 0x%x in Receive\n",board_family);

      if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"Unkown Board Family 0x%x in Receive",board_family);

      dropFrame = L7_TRUE;
    }
    else if (isValidUsp(&pktRxMsg.usp,dapi_g) == L7_FALSE)
    {
      static L7_uint32 log_count = 0;

      /* Log every 1000th packet
      */
      if ((log_count % 1000) == 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
                "Invalid USP unit = %d, slot = %d, port = %d."
                "A port was not able to be translated correctly during the receive.",
                pktRxMsg.usp.unit,
                pktRxMsg.usp.slot,
                pktRxMsg.usp.port);
      }
      log_count++;

      dropFrame = L7_TRUE;
    }
    /* check for desired MAC DAs */
    else if (hapiBroadMacDaCheck(&pktRxMsg, board_family, dapi_g) == L7_FALSE) 
    {
      if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL2)
        PT_LOG_ERR(LOG_CTX_HAPI,"hapiBroadMacDaCheck failed! dropping packet");

      dropFrame = L7_TRUE;
    }
    else if ((board_family == BCM_FAMILY_DRACO)  || 
             (board_family == BCM_FAMILY_TUCANA))
    {
      hapiBroadRxLegacy(&pktRxMsg, &dropFrame, board_family, dapi_g);
    }
    else if ((board_family == BCM_FAMILY_FIREBOLT) ||
             (board_family == BCM_FAMILY_BRADLEY)  ||
             (board_family == BCM_FAMILY_TRIUMPH)  ||
             (board_family == BCM_FAMILY_TRIUMPH2)  ||
             (board_family == BCM_FAMILY_SCORPION)  ||
             (board_family == BCM_FAMILY_ROBO)      ||
             (board_family == BCM_FAMILY_TRIDENT))      /* PTin added: new switch 56843 (Trident) */
    {
      L7_BOOL fwdFrame = L7_TRUE;

      hapiBroadRxXgs3(&pktRxMsg,&fwdFrame,dapi_g);

      dropFrame = (fwdFrame == L7_TRUE)?L7_FALSE:L7_TRUE;

      if ((cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL2) && dropFrame)
        PT_LOG_ERR(LOG_CTX_HAPI,"hapiBroadRxXgs3 failed! dropping packet");
    }
    else
    {
      /* unknown board family drop frame */
      sysapiPrintf("Unkown Board Family 0x%x in Receive. Dropping the frame!!!\n",board_family);

      if ((cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL2) && dropFrame)
        PT_LOG_ERR(LOG_CTX_HAPI,"Unkown Board Family 0x%x in Receive. Dropping the frame!!!\n",board_family);

      dropFrame = L7_TRUE;
    }
   }while(0);
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
    if(hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
    {

      /* On bcm53115 if dot1ad is selected, we need to modify inner-tag
       and outer-tag of protocol packets according to subscription information.*/
      if(hapiBroadRxProtoSnoopModify(&pktRxMsg,dapi_g) != L7_SUCCESS)
      {
        /* packet does not match any subscription information */
        dropFrame = L7_TRUE;
      }
    }
#endif

    pkt = sysapiNetMbufGetDataStart(frameHdl);
    dapiTraceFrame(&pktRxMsg.usp,pkt,(dropFrame)?DAPI_TRACE_FRAME_DROP:DAPI_TRACE_FRAME_FWD,((pktRxMsg.cos << 28) & 0xf0000000));

    if (dropFrame == L7_TRUE) 
    {
      /* drop frame */
      sysapiNetMbufFree((L7_netBufHandle)frameHdl);
      if ((pktRxMsg.cos >= 0) && (pktRxMsg.cos < 8))
      {
        hapiRxDebugCosCounters[pktRxMsg.cos].pktRxCosDrops++;
      }
    }
    else
    {
          
      frameLen = sysapiNetMbufGetDataLength(frameHdl);
      pkt = sysapiNetMbufGetDataStart(frameHdl);

      /* increment the CPU Receive stats */
      pktUntagged = (pktRxMsg.rx_untagged & BCM_PKT_OUTER_UNTAGGED) ? 
                      (L7_TRUE) : (L7_FALSE);
      hapiBroadStatsCpuIncrement(pkt, frameLen, &pktRxMsg.usp, dapi_g, 
                                 L7_TRUE, pktUntagged);

  #ifdef HAPI_BROAD_PKT_DEBUG
      if (!skip && pktRxDebug)
      {
        pkt = sysapiNetMbufGetDataStart (pktRxMsg.cmdInfo.cmdData.receive.frameHdl);
        if ((!hapiBroadDebugPktIsFiltered(pkt)) &&
            ((pktRxDebugQueue == 0) ||
             (pktRxDebugQueue != 0 && (pktRxMsg.cos == (pktRxDebugQueue - 1)))))

        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                        "   Passing packet to application\n");
        }
      }
  #endif
      /* Set the reason code in the RX mbuf */
      hapiBroadRxReasonCodeSet(&pktRxMsg);

      if (cpu_intercept_debug & CPU_INTERCEPT_DEBUG_LEVEL3)
        PT_LOG_TRACE(LOG_CTX_HAPI,"Going to call callback DAPI_FAMILY_FRAME, DAPI_EVENT_FRAME_RX");

      /* notify the application of the frame */
      if (dapiCallback(&pktRxMsg.usp,
                       DAPI_FAMILY_FRAME,
                       DAPI_CMD_FRAME_UNSOLICITED_EVENT,
                       DAPI_EVENT_FRAME_RX,
                       &pktRxMsg.cmdInfo) != L7_SUCCESS)
      {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "%s %d: In %s call to 'dapiCallback'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        sysapiNetMbufFree((L7_netBufHandle)frameHdl);
      }

    }

    dapiCardRemovalReadLockGive ();
  }
}

/*********************************************************************
*
* @purpose Set the RX reason code in the receive MBUF
*
* @param   BROAD_PKT_RX_MSG_t *pktRxMsg
*
* @returns none
*
* @notes   Not all BCM reason codes are mapped as of now. Add more rx
*          reason codes as and when application requirement crops up.
*
* @end
*
*********************************************************************/
static void hapiBroadRxReasonCodeSet(BROAD_PKT_RX_MSG_t *pktRxMsg)
{
  L7_uint32 rxCode = L7_MBUF_RX_REASON_NONE;
  L7_netBufHandle      frameHdl = L7_NULL;

  frameHdl = pktRxMsg->cmdInfo.cmdData.receive.frameHdl; 

  /* A single RX packet can have multiple reason codes */

  /* Map the BCM reason code to sysapi reason code */
  if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonIcmpRedirect))
    rxCode |= L7_MBUF_RX_REASON_ICMP_REDIR;
  if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL3DestMiss))
    rxCode |= L7_MBUF_RX_REASON_L3_MISS;
  if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonSampleSource))
    rxCode |= L7_MBUF_RX_SAMPLE_SOURCE;
  if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonSampleDest))
    rxCode |= L7_MBUF_RX_SAMPLE_DEST;

  sysapiNetMbufSetRxReasonCode(frameHdl, rxCode);
}

/*********************************************************************
*
* @purpose Determines whether frame should be dropped or passed to application
*          when received on a XGS
*
* @param   BROAD_PKT_RX_MSG_t *pktRxMsg 
* @param   DAPI_t             *dapi_g
*
* @returns L7_BOOL L7_TRUE  - packet should be forwarded to application
*                      L7_FALSE - packet should be dropped
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadXgsRxCheck(BROAD_PKT_RX_MSG_t *pktRxMsg, DAPI_t *dapi_g)
{
  L7_netBufHandle      frameHdl=L7_NULL;
  BROAD_PORT_t        *hapiPort;
  L7_BOOL          result = L7_FALSE;
  L7_uchar8           *user_data;
  L7_uchar8            reservedMacDa[] = {0x01,0x80,0xc2,0x00,0x00};
  L7_ushort16        innerVlanId = 0;

  frameHdl = pktRxMsg->cmdInfo.cmdData.receive.frameHdl;
  hapiPort = HAPI_PORT_GET(&pktRxMsg->usp, dapi_g);

  /* Filter out the packet depending on the reason
   */

  if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonUnknownVlan) ||
      (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL2SourceMiss) &&
       (hapiPort->cpEnabled == L7_FALSE))                            ||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL2DestMiss) ||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL2Move) ||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL2Cpu) ||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL3SourceMiss) ||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL3SourceMove))
  {
    /* drop the packet */
  }
  /* we changed the code so that the FFP catches PDUs, so they will come up w/ two reason codes set */
  else if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonProtocol))
  {
    /* Note - the chip automatically sends all frame with MAC addresses 0180C2000001 to
       0180C2000010 and 0180C2000020 to 0180C200002F to the CPU. During init, an 
       additional entry is added to EPC_BPDU to pass up 0180C2000000. */
    if (hapiBroadReceivePdu(frameHdl, pktRxMsg->cmdInfo.cmdData.receive.vlanID, 
                            &pktRxMsg->usp,pktRxMsg->rx_untagged, dapi_g,innerVlanId) == L7_TRUE)
    {
      result = L7_TRUE;
    }
  }
  else if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonIpMcastMiss))
  {
    /* This reason code is set for IP multicast packets sent to the CPU 
       when IP multicast is enabled. It could occur for an RPF failure
       or a case where a packet misses the IP multicast table. Routing PDU
       frames will hit here when IP multicast is enabled */

    /* If this was received on a LAG member port, convert USP to
       the LAG USP */
    if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      pktRxMsg->usp = hapiPort->hapiModeparm.physical.lagUsp;
    }

#ifdef L7_MCAST_PACKAGE
    /* Check for RPF failures - if it's a true RPF failure and the action is
       drop, the function below will drop the frame. Otherwise it will return
       SUCCESS and we will break out to send it up */
    if (hapiBroadL3McastRPF(frameHdl, pktRxMsg->cmdInfo.cmdData.receive.vlanID, &pktRxMsg->usp, 
                            dapi_g) == L7_SUCCESS)
    {
      result = L7_TRUE;
    }
#else
    result = L7_TRUE;
#endif
  }
  else if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonMcastMiss))
  {
    /* This reason code is set for MAC multicast packets. It could occur for cases
       such as when IP multicast is not enabled and the MARL search fails -  the
       packet gets flooded on the vlan which includes the CPU. */

    /* If IP multicast is disabled, routing PDUs come up w/ this reason code
       so check to see if this is a routing PDU and if it was received on a routing
       interface. If so, pass it up to the application, otherwise drop it */

    /* If this was received on a LAG member port, convert USP to
       the LAG USP */
    if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      pktRxMsg->usp = hapiPort->hapiModeparm.physical.lagUsp;
    }
  }
  else if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonFilterMatch))
  {
    /* since we're now using the FFP to guarantee that MAC reserved pkts get to the 
       CPU, we need to check here to see if the MAC DA is reserved. Normally both 
       BCM_RX_PR_PROTOCOL_PKT and BCM_RX_PR_FFP are set, but if the port accepts
       only VLAN tagged pkts, then only BCM_RX_PR_FFP is set */
    user_data = sysapiNetMbufGetDataStart (frameHdl);
    if (memcmp(user_data, reservedMacDa, 5) == 0)
    {
      if (hapiBroadReceivePdu(frameHdl, pktRxMsg->cmdInfo.cmdData.receive.vlanID, 
                              &pktRxMsg->usp,pktRxMsg->rx_untagged, dapi_g,
                              innerVlanId) == L7_TRUE)
      {
        result = L7_TRUE;
      }
    }
    else
    {
      if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
      {
        pktRxMsg->usp = hapiPort->hapiModeparm.physical.lagUsp;
      }
      result = L7_TRUE;
    }
  }
  else
  {
    if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      pktRxMsg->usp = hapiPort->hapiModeparm.physical.lagUsp;
    }
    result = L7_TRUE;
  }

  return result;
}

/*********************************************************************
*
* @purpose Determines whether MAC SA matches that of a routing intf.
*          This indicates that the packet was purposefully routed to 
*          the CPU and should be passed to the application.
*
* @param   L7_uchar8          *macSa 
* @param   DAPI_t             *dapi_g
*
* @returns L7_BOOL L7_TRUE  - macSa matches a L3 intf
*                      L7_FALSE - macSa doesn't match a L3 intf
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadRxMacSaIsL3Intf(L7_uchar8 *macSa, DAPI_t *dapi_g)
{
  DAPI_USP_t             searchUsp;
  DAPI_PORT_t           *dapiPortPtr;
  static L7_BOOL        sa_found = L7_FALSE;
  static L7_uchar8      rt_sa[6];

  /* Router MAC address can't change dynamically, and it is the same for 
  ** all routing interfaces. Therefore once we find the address for the 
  ** first time, we can use for ever.
  */
  if (sa_found == L7_TRUE)
  {
    if (memcmp (macSa, rt_sa, 6) == 0) 
    {
      return L7_TRUE;
    }
    return L7_FALSE;
  }

  /* search VLAN interfaces first */
  searchUsp.unit = L7_LOGICAL_UNIT;
  searchUsp.slot = L7_VLAN_SLOT_NUM;

  for (searchUsp.port = 0; searchUsp.port < L7_MAX_NUM_ROUTER_INTF; searchUsp.port++)
  {
    if (isValidUsp(&searchUsp,dapi_g) == L7_FALSE)
    {
      continue;
    }

    dapiPortPtr = GET_DAPI_PORT(dapi_g, &searchUsp);

    if (dapiPortPtr->modeparm.router.ipAddr  != 0)
    {
#ifdef HAPI_BROAD_PKT_DEBUG
      if (!hapiBroadDebugPktIsFiltered(macSa - 6))
      {
        if (pktRxDebugLevel)
        {
          printf("Comparing SA to %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                 dapiPortPtr->modeparm.router.macAddr[0],
                 dapiPortPtr->modeparm.router.macAddr[1],
                 dapiPortPtr->modeparm.router.macAddr[2],
                 dapiPortPtr->modeparm.router.macAddr[3],
                 dapiPortPtr->modeparm.router.macAddr[4],
                 dapiPortPtr->modeparm.router.macAddr[5]);
        }
      }
#endif
      sa_found = L7_TRUE;
      memcpy (rt_sa, dapiPortPtr->modeparm.router.macAddr, 6);
      if (memcmp(dapiPortPtr->modeparm.router.macAddr, macSa, 6) == 0)
      {
        return L7_TRUE;
      }
    }
  }

  /* then search physical routing interfaces */
  for (searchUsp.unit = 0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
  {
    for (searchUsp.slot = 0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
    {
      if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE)
      {
        for (searchUsp.port = 0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++)
        {
          if (isValidUsp(&searchUsp,dapi_g) == L7_FALSE)
          {
            continue;
          }

          dapiPortPtr = GET_DAPI_PORT(dapi_g, &searchUsp);

          if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
          {
            continue;
          }

          if (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE)
          {
#ifdef HAPI_BROAD_PKT_DEBUG
            if (!hapiBroadDebugPktIsFiltered(macSa - 6))
            {
              if (pktRxDebugLevel)
              {
                printf("Comparing SA to %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                       dapiPortPtr->modeparm.physical.macAddr[0],
                       dapiPortPtr->modeparm.physical.macAddr[1],
                       dapiPortPtr->modeparm.physical.macAddr[2],
                       dapiPortPtr->modeparm.physical.macAddr[3],
                       dapiPortPtr->modeparm.physical.macAddr[4],
                       dapiPortPtr->modeparm.physical.macAddr[5]);
              }
            }
#endif
            sa_found = L7_TRUE;
            memcpy (rt_sa, dapiPortPtr->modeparm.physical.macAddr, 6);
            if (memcmp(dapiPortPtr->modeparm.physical.macAddr, macSa, 6) == 0)
            {
              return L7_TRUE;
            }
          }
        }
      }
    }
  }
  return L7_FALSE;
}


/*********************************************************************
*
* @purpose Function to process PDU frames
*
* @param   L7_netBufHandle  frameHdl - frame handle for this data 
* @param   L7_ushort        vlanID  - vlan ID of the port 
* @param   DAPI_USP_t      *usp      - Used to notify the application of the frame.
*                                      Needs to be a valid usp.
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_BOOL
*
* @notes   In DOT1AD tunneling supported devices, CPU receives either double tagged
*          or single outer tagged frames to the CPU. Control PDUs needs to be forwarded
*          based on both Outer and inner vlans. To achieve this introducing the new
*          inner VLAN info to this function argument.
*          Frame will not have this information as we strip this at hapiBroadReceive.
*          
*          
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadReceivePdu(L7_netBufHandle frameHdl, 
                            L7_ushort16 vlanID, DAPI_USP_t *usp, 
                            L7_uchar8 rx_untagged,DAPI_t *dapi_g,
                            L7_ushort16 innerVlanId)
{
  L7_uchar8             *user_data;
  L7_BOOL                dropFrame = L7_FALSE;
  L7_BOOL                sendFrame = L7_FALSE;
  L7_BOOL                sendBpdu  = L7_FALSE;
  L7_BOOL                floodBpdu = L7_FALSE; 
  BROAD_PORT_t          *hapiPort;
  L7_ushort16            user_data_size;
  L7_uchar8              nullDa[] = {0,0,0,0,0,0};
  L7_ushort16            ethertype;
#if (L7_DOT1S_BPDUFLOOD_SET == L7_TRUE_DEF)
  BROAD_TX_BPDU_MSG_t    bpdu_msg;
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE

  BROAD_TX_PDU_MSG_t    pdu_msg;
  DOT1AD_TUNNEL_ACTION_t tunnelAction;
  L7_enetMacAddr_t        protocolMac;
  L7_ushort16 protoId;
  L7_BOOL found=L7_FALSE;
  L7_ushort16           pvid=0;
  L7_ushort16           cvid=0;
  BROAD_PORT_t          *hapiLagMemberPort=L7_NULLPTR;
  L7_BOOL                isLagMember = L7_FALSE;
  DAPI_USP_t             *tmpUsp = usp;
  bcm_chip_family_t      board_family;
  L7_BOOL                isTriumphFamily = L7_FALSE;

#endif
#endif

  L7_uchar8             reservedMacDa[] = {0x01,0x80,0xc2,0x00,0x00};
  L7_uchar8             isdp_macda[]  = {0x01,0x00,0x0c,0xcc,0xcc,0xcc};

  hapiPort = HAPI_PORT_GET(usp, dapi_g);
  user_data = sysapiNetMbufGetDataStart (frameHdl);
  user_data_size = sysapiNetMbufGetDataLength (frameHdl);

  /* It seems that pkts w/ NULL DA come up this path... drop them here */
  if (memcmp(user_data, nullDa, 6) == 0)
  {
    /* Discard the frame.
    */

    return L7_FALSE;
  }

  /* If the port is unauthorized by 802.1x then only allow EAPOL packets */
  /* Even though we do not want unauthorized ports to process bpdus, we need 
     to send the pdu to application in case bpdu guard is enabled, so appln can
     disable the port
  */
  if ((!HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPort)) &&
      ((user_data[16] != 0x88 || user_data[17] != 0x8e) &&   /* EAPOL */
       (user_data[16] != 0x88 || user_data[17] != 0xcc) &&  /* LLDP */
       ((L7_ENABLE != hapiPort->voiceVlanPort.voiceVlanUnauthMode) ||
        (0 != memcmp(user_data, isdp_macda, sizeof(isdp_macda)))) &&
       (hapiPort->bpduGuardMode != L7_TRUE)
      ))
  {
    /* Discard the frame.
     */
    sysapiPrintf("Discard\n\r");
    return L7_FALSE;
  }

  /* The following checks are only valid on Reserved MAC DA */
  if (memcmp(user_data, reservedMacDa, 5) == 0)
  {

#ifdef L7_DOT3AH_PACKAGE
/* Check for TYPE:SLOW PROTOCOL(0x8809)  and SUB-TYPE:OAM(0x03) */
   if( user_data[16] == 0x88 && user_data[17] ==0x09 && user_data[18] == 0x03 )
   {
      if(hapiPort->hapiDot1adintfCfg.intfType == DOT1AD_INTFERFACE_TYPE_NNI)
      {
        /* OAM PDU...Just passthrough */
        if (dot1adVlanInfoDebug)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Oam Pdu pass through.\n");
        }
        return L7_TRUE;
      }
      else
      {
       /* Drop the OAM PDU's received on UNI-x ports */
        return L7_FALSE;
      }

   }
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE


    if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE ) /* LAG member */
    {
      tmpUsp = &hapiPort->hapiModeparm.physical.lagUsp;
      hapiPort = HAPI_PORT_GET(&hapiPort->hapiModeparm.physical.lagUsp, dapi_g);
      hapiLagMemberPort = HAPI_PORT_GET(usp, dapi_g);
      isLagMember = L7_TRUE;
    }
    hapiPort->hapiDot1adIntfStats.numPduReceived++;

    if (hapiPort->hapiDot1adintfCfg.intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)
    {
      /* This logic is written based on Triumph platform behaviour */
       if (hapiBroadGetSystemBoardFamily(&board_family) == L7_SUCCESS)
       {
         if ( (board_family == BCM_FAMILY_TRIUMPH) ||
              (board_family == BCM_FAMILY_TRIUMPH2)
             )
         {
           isTriumphFamily = L7_TRUE;
         }
       }
       /* On UNI_P port, we shoudl get only UT and SIT PDUs */
       if ( (isTriumphFamily == L7_TRUE) &&
            ( (rx_untagged & BCM_PKT_OUTER_UNTAGGED) != (BCM_PKT_OUTER_UNTAGGED) )
          )
       {

         if (dot1adVlanInfoDebug)
         {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Discarding the frame because of tag status on %d-%d-%d\n",
                            tmpUsp->unit,
                            tmpUsp->slot,
                            tmpUsp->port);
         }
         hapiPort->hapiDot1adIntfStats.numPduDiscarded++;
         return L7_FALSE;
       }
     }


#ifdef L7_DOT1AG_PACKAGE
    if (user_data[5] > 0x3f)
#else
    if (user_data[5] > 0x2f)
#endif
    {
      /*discard frame */
      hapiPort->hapiDot1adIntfStats.numPduDiscarded++;
      return L7_FALSE;
    }

    pvid=hapiPort->pvid;
    cvid=innerVlanId;
    
    if (hapiPort->hapiDot1adintfCfg.intfType == DOT1AD_INTFERFACE_TYPE_SWITCHPORT)
    {
       /* Terminate reserved bridge pdus on a normal port */
       tunnelAction = DOT1AD_TERMINATE;
    }
    else
    {
      if ( (user_data[5] == 0x02) &&
           (user_data[18]== 0x01) /* SUB-TYPE octect */
         )  /* LAG PDU needs to be terminated Always */
      {
        if (dot1adVlanInfoDebug)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Lag PDU is received.\n");
        }
        tunnelAction = DOT1AD_TERMINATE;
      }
      else
      {
      
        pdu_msg.dot1adSVlanId = vlanID;
        pdu_msg.dot1adRemarkCVlanId = innerVlanId;
        if (user_data[5] == 0x21 && 
            hapiPort->hapiDot1adintfCfg.intfType !=  DOT1AD_INTFERFACE_TYPE_NNI)
        {
          /*discard GVRP frames which are received on user ports */
          hapiPort->hapiDot1adIntfStats.numPduDiscarded++;
  
          if (dot1adVlanInfoDebug)
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "GVRP pdu received on a UNI interface %d-%d-%d\n",
                             tmpUsp->unit,
                             tmpUsp->slot,
                             tmpUsp->port);
          }
          return L7_FALSE;
        }
   
        /* Checking the VLAN participation part */
        if (!BROAD_IS_VLAN_MEMBER(tmpUsp, vlanID, dapi_g))
        {
          /*discard frame */
          hapiPort->hapiDot1adIntfStats.numPduDiscarded++;

          if (dot1adVlanInfoDebug)
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Discarding the frame because of the ingress rule on %d-%d-%d\n",
                           tmpUsp->unit,
                           tmpUsp->slot,
                           tmpUsp->port);
          }
          return L7_FALSE;
        }
 
      }

      memcpy(protocolMac.addr,user_data,L7_ENET_MAC_ADDR_LEN);
      protoId=osapiNtohs(*((L7_ushort16 *)(user_data+16)));

      /* Find out the L2PT action specified */
      {
        BROAD_L2PT_ENTRY_t *entry = L7_NULLPTR;

        do
        {
          found = L7_FALSE;
          if (hapiBroadL2ptEntryTreeSearch (vlanID,
                                            &protocolMac,
                                            protoId,
                                            L7_MATCH_EXACT,
                                            &entry) == L7_SUCCESS)
          {
            found = L7_TRUE;
          }
          else
          {
            if (hapiBroadL2ptEntryTreeSearch (vlanID,
                                              &protocolMac,
                                              0,
                                              L7_MATCH_EXACT,
                                              &entry) == L7_SUCCESS)
            {
              if (entry->l2ptEntryKey.protocolId == 0)
              {
                found = L7_TRUE;
              }
            }
            else
            {
              if (dot1adVlanInfoDebug)
              {
                SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "RX: L2PT entry does not exist for VID -%d, MAC-%0x.\n",
                                       vlanID,user_data[5]);
              }
            }
          }
        }while(0);

        if ( (found) &&
             (entry != L7_NULLPTR)
           )
        {
          tunnelAction = entry->tunnelAction;
        }
        else
        {
         /* If the L2PT entry is not found,       
            Set the tunnelAction to default L2PT action. */
          tunnelAction = FD_DOT1AD_DEFAULT_L2PT_ACTION;
        }
      }
    }/* Check if it is a switch port */

    switch(tunnelAction)
    {
      case DOT1AD_TUNNEL:
      case DOT1AD_DETUNNEL:

      if (dot1adVlanInfoDebug)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "RX: L2PT Tunnel Action \n");
      }

        /* send the message to transmit task through message queue */
        if (user_data_size > L7_MAX_FRAME_SIZE)
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
                  "Dropping the packet as the length %d is greater than max allowed frame size %d\n",
                  user_data_size, L7_MAX_FRAME_SIZE);
          return L7_FALSE;
        }
        pdu_msg.usp = *tmpUsp;

        if (isLagMember == L7_TRUE)
        {
          pdu_msg.bcmx_lport = hapiLagMemberPort->bcmx_lport; 
        }
        else
        {
        pdu_msg.bcmx_lport = hapiPort->bcmx_lport;
        }
        pdu_msg.vlanID = vlanID;
        pdu_msg.hapiPortPtr = hapiPort;
        if(tunnelAction == DOT1AD_TUNNEL)
        {
          pdu_msg.tunnel = L7_TRUE;
        }
        else
        {
          pdu_msg.tunnel = L7_FALSE;
        }
        pdu_msg.user_data_size = user_data_size;
        pdu_msg.proto_byte = user_data[5];
        pdu_msg.sendBpdu = sendBpdu;
        pdu_msg.sendFrame = sendFrame;
        pdu_msg.rx_untagged = rx_untagged;
        pdu_msg.pkt_data = sal_dma_alloc (user_data_size+4, "hapiBuf");

        if (pdu_msg.pkt_data != NULL)
        {
          memcpy(pdu_msg.pkt_data, user_data, user_data_size);
          if (osapiMessageSend(hapiTxPduQueue,
                               (void *)&pdu_msg,
                               sizeof(BROAD_TX_PDU_MSG_t),
                               L7_NO_WAIT, 
                               L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
          {
            sal_dma_free (pdu_msg.pkt_data);
          }
        }

        return L7_FALSE;

      case DOT1AD_TERMINATE:
        if (dot1adVlanInfoDebug)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Rx: L2PT Termination Action \n");
        }
        hapiPort->hapiDot1adIntfStats.numPduTerminated++;
        break;

      case DOT1AD_DISCARD:
        /*discard the packet */
        if (dot1adVlanInfoDebug)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Rx: L2PT Discard Action \n");
        }
        hapiPort->hapiDot1adIntfStats.numPduDiscarded++;
        return L7_FALSE;

      case DOT1AD_DISCARD_SHUTDOWN:
        hapiPort->hapiDot1adIntfStats.numPduDiscarded_shutdown++;
        /*discard and shutdown the interface */
        if (dot1adVlanInfoDebug)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Rx: L2PT Discard Shutdown Action \n");
        }
        (nimSetIntfAdminStatePtr)((usp->port + 1),L7_DISABLE);
        return L7_FALSE;

      default:
         return L7_FALSE;
    }

#endif
#endif

     /* For metro, we populate the hapiPort with LAG USP 
        on a condition. So reverting here to the
        physical interface value.
      */
                 
     hapiPort = HAPI_PORT_GET(usp, dapi_g);
    /*Discard Frames on the Probe*/
    if (hapiBroadSystemMirroringUSPIsProbe(usp,dapi_g))
      return L7_FALSE;


    switch (user_data[5])
    {
      case 0x00:
        /* if this was received on a LAG member port, convert USP to
           the LAG USP */
        if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
        {
          *usp = hapiPort->hapiModeparm.physical.lagUsp;
          hapiPort = HAPI_PORT_GET(usp, dapi_g);
        }

        if (L7_BRIDGE_SPEC_SUPPORTED == L7_BRIDGE_SPEC_802DOT1D)
        {
          if (hapiPort->stp_state == DAPI_PORT_STP_ADMIN_DISABLED)
          {
            dropFrame = L7_TRUE;
          }
          else if (hapiPort->stp_state == DAPI_PORT_STP_NOT_PARTICIPATING)
          {
            sendBpdu = L7_TRUE;
          }
        }
        else
        {
          if (hapiPort->dot1s_state[0] == DAPI_PORT_DOT1S_ADMIN_DISABLED)
          {
            dropFrame = L7_TRUE;
          }
          else if(hapiPort->bpduFilterMode == L7_TRUE)
          {
            dropFrame = L7_TRUE;
          }
          else if (hapiPort->dot1s_state[0] == DAPI_PORT_DOT1S_NOT_PARTICIPATING)
          {
            sendBpdu = L7_TRUE;
            if(hapiPort->bpduFloodMode == L7_TRUE)
            {
              floodBpdu = L7_TRUE;
            }
            else /* STP frame should be dropped here */
            {
              dropFrame = L7_TRUE;
            }
          }
        }

        if ((dropFrame == L7_FALSE) && (sendBpdu == L7_FALSE))
        {
          /* This BPDU needs to go to DTL. It comes from the HW
             w/ a VLAN tag. Remove the VLAN tag now */
          unsigned char tmp_buffer[12];
          memcpy(tmp_buffer,user_data,12);
          memcpy(user_data+4,tmp_buffer,12);
          user_data += 4;
          user_data_size -= 4;
          sysapiNetMbufSetDataStart(frameHdl,user_data);
          sysapiNetMbufSetDataLength(frameHdl,user_data_size);
        }
        break;

      case 0x02:
        /* Do not convert USP to LAG USP... application needs to know 
           which physical port received this PDU */
        if (hapiPort->hapiModeparm.physical.acceptLAPDU != L7_TRUE)
        {
          dropFrame = L7_TRUE;
        }

        break;

      case 0x20:
        /* if this was received on a LAG member port, convert USP to
           the LAG USP */
        if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
        {
          *usp = hapiPort->hapiModeparm.physical.lagUsp;
          hapiPort = HAPI_PORT_GET(usp, dapi_g);
        }

        /* drop the PDU if this port is not forwarding */
        if (hapiBroadPortIsForwarding(usp, vlanID, dapi_g) == L7_FALSE)
        {
          dropFrame = L7_TRUE;
        }

        if (dapi_g->system->gmrpConfigEnabled)
        {
          if (hapiPort->gmrpEnabled != L7_TRUE)
          {
            dropFrame = L7_TRUE;
          }
        }
        else
        {
          sendFrame = L7_TRUE;
        }

        break;

      case 0x21:
        /* if this was received on a LAG member port, convert USP to
           the LAG USP */
        if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
        {
          *usp = hapiPort->hapiModeparm.physical.lagUsp;
          hapiPort = HAPI_PORT_GET(usp, dapi_g);
        }

        if (dapi_g->system->gvrpConfigEnabled)
        {
          if (hapiPort->gvrpEnabled != L7_TRUE)
          {
            dropFrame = L7_TRUE;
          }
        }
        else
        {
          /* drop the PDU if this port is not forwarding */
          if (hapiBroadPortIsForwarding(usp, vlanID, dapi_g) == L7_FALSE)
          {
            dropFrame = L7_TRUE;
          }
          else
          {
            sendFrame = L7_TRUE;
          }
        }

        break;

      /* allow destination MAC 01:80:C2:00:00:0E for 802.1AB */
      case 0x0E:

        /* check if physical port is enabled to receive LLDP packets */
        if (hapiPort->hapiModeparm.physical.acceptLLDPDU == L7_TRUE)
        {
          if ( voiceVlanAuthDebug )
          {
            sysapiPrintf("%s(): acceptLLDPDU is TRUE\n",__FUNCTION__);
          }
          /* remove VLAN tag before sending to CPU,
             LLDP is only concerned with physical ports,
             do not consider LAG membership */
          unsigned char tmp_buffer[12];
          memcpy(tmp_buffer,user_data,12);
          memcpy(user_data+4,tmp_buffer,12);
          user_data += 4;
          user_data_size -= 4;
          sysapiNetMbufSetDataStart(frameHdl,user_data);
          sysapiNetMbufSetDataLength(frameHdl,user_data_size);
        }
        else
        {
          if ( voiceVlanAuthDebug )
          {
            sysapiPrintf("%s(): acceptLLDPDU is FALSE\n",__FUNCTION__);
            if (L7_ENABLE == hapiPort->voiceVlanPort.voiceVlanUnauthMode)
            {
              sysapiPrintf("%s(): Allow the Pkt Since Voice VLAN UnauthMode is ENABLE\n",__FUNCTION__);
            }
          }
          if (L7_ENABLE == hapiPort->voiceVlanPort.voiceVlanUnauthMode)
          {
             /* remove VLAN tag before sending to CPU,
             LLDP is only concerned with physical ports,
             do not consider LAG membership */
             unsigned char tmp_buffer[12];
             memcpy(tmp_buffer,user_data,12);
             memcpy(user_data+4,tmp_buffer,12);
             user_data += 4;
             user_data_size -= 4;
             sysapiNetMbufSetDataStart(frameHdl,user_data);
             sysapiNetMbufSetDataLength(frameHdl,user_data_size);
             break;
          }
          /* this port is not configured to receive LLDP packets */
          dropFrame = L7_TRUE;
        }
        break;
        default:
#ifdef L7_DOT1AG_PACKAGE
        if(user_data[5] >= 0x30 && user_data[5] <= 0x3F) /* CFM Packet */
        {
          /* if this was received on a LAG member port, convert USP to
             the LAG USP */
          if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
          {
            *usp = hapiPort->hapiModeparm.physical.lagUsp;
            hapiPort = HAPI_PORT_GET(usp, dapi_g);
          }
        }
#endif
        break;
    }

    if ((user_data[5] > 0x03) && (user_data[5] <= 0x0f))  /* Reserved Address */
    {
      /* LLDPDU reserved address is handled in case 0x0E */
      if (user_data[5] != 0x0E)
      {
        dropFrame = L7_TRUE;
      }
    }
    else if ((user_data[5] >= 0x10) && (user_data[5] <= 0x1f))  /* Reserved Address */
    {
      /* these frames should be forwarded */
      sendFrame = L7_TRUE;
      floodBpdu = L7_RESERVED_ADDRESS_FLOOD;
    }
    else if ((user_data[5] >= 0x22) && (user_data[5] <= 0x2f))  /* Reserved Address */
    {
      /* these frames should be forwarded */
      sendFrame = L7_TRUE;
      floodBpdu = L7_RESERVED_ADDRESS_FLOOD;
    }

    if (dropFrame)
    {
      /* Discard the frame.
      */
      return L7_FALSE;
    }
    else if (sendFrame || sendBpdu)
    {
#if (L7_DOT1S_BPDUFLOOD_SET == L7_TRUE_DEF)
      /* We decided that forwarding BPDUs on ports with protocols disabled causes a lot of
      ** network problems.
      ** We are now dropping these frames.
      */
      if(floodBpdu != L7_TRUE)
      {
        return L7_FALSE;
      }
        
      if (user_data_size > L7_MAX_FRAME_SIZE)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
                "Dropping the packet as the length %d is greater than max allowed frame size %d\n",
                user_data_size, L7_MAX_FRAME_SIZE);
  
        return L7_FALSE;
      }
  
      bpdu_msg.usp = *usp;
      bpdu_msg.bcmx_lport = hapiPort->bcmx_lport;
      bpdu_msg.vlanID = vlanID;
      bpdu_msg.dtag_mode = hapiPort->dtag_mode;
      bpdu_msg.user_data_size = user_data_size;
      bpdu_msg.proto_byte = user_data[5];
      bpdu_msg.sendBpdu = sendBpdu;
      bpdu_msg.sendFrame = sendFrame;
      bpdu_msg.pkt_data = sal_dma_alloc (user_data_size+4, "hapiBuf");
      if (bpdu_msg.pkt_data != NULL)
      {
        memcpy(bpdu_msg.pkt_data, user_data, user_data_size);
        if (osapiMessageSend(hapiTxBpduQueue,
                             (void *)&bpdu_msg,
                             sizeof(BROAD_TX_BPDU_MSG_t),
                             L7_NO_WAIT, 
                             L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
        {
          sal_dma_free (bpdu_msg.pkt_data);
        }
      }
  
      return L7_FALSE;
#else  /* (L7_DOT1S_BPDUFLOOD_SET == L7_TRUE_DEF) */
      return L7_FALSE;
#endif /* (L7_DOT1S_BPDUFLOOD_SET == L7_TRUE_DEF) */
    }
    
  } /* ends the reserved MAC DA processing */
  else 
  {
    /* ISDP works on physical interface */
    if (memcmp(user_data, isdp_macda, 6) != 0) 
    {  
      if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
      {
        *usp = hapiPort->hapiModeparm.physical.lagUsp;
      }
    }
  }

  ethertype = osapiNtohs(*(L7_ushort16 *)(&user_data[12]));

  if ((ethertype != L7_ETYPE_8021Q) && (ethertype == hapiBroadDvlanEthertype))
  {
    /* Replace the ethertype to the default 802.1q ethertype*/
    *(L7_ushort16 *)&user_data[12] = osapiHtons(L7_ETYPE_8021Q);
  }

  return L7_TRUE;
}


/*********************************************************************
*
* @purpose This task forwards PDUs to ports in cases when 
*          STP, GMRP, or GVRP protocols are disabled on the ingress port.
*
* @param   L7_uint32        numArgs  - number of arguments
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadBpduTxTask(L7_uint32 numArgs, DAPI_t *dapi_g)
{
  BROAD_TX_BPDU_MSG_t    bpdu_msg;
  bcmx_lplist_t          lplist;
  bcmx_lplist_t          untaggedLplist;
  bcmx_lplist_t          taggedLplist;
  bcm_pkt_t              bcm_pkt;
  bcm_pkt_blk_t          bcm_pkt_blk;
  int                    rv = 0;
  L7_int32               mode;
  L7_int32               i;

  while (1)
  {
    if (osapiMessageReceive(hapiTxBpduQueue,
                            (void *)&bpdu_msg,
                            sizeof(BROAD_TX_BPDU_MSG_t),
                            L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOG_ERROR(0);
    }

    memset (&bcm_pkt, 0, sizeof (bcm_pkt_t));
    memset (&bcm_pkt_blk, 0, sizeof (bcm_pkt_blk_t));

    bcm_pkt.pkt_data  = &bcm_pkt_blk;
    bcm_pkt.blk_count = 1;
    bcm_pkt.call_back = L7_NULLPTR;  /* only used for async responses which we do not use */
    bcm_pkt.pkt_data->data = bpdu_msg.pkt_data;

    bcm_pkt.pkt_data->len  = bpdu_msg.user_data_size;
#ifndef PC_LINUX_HOST /* no CRC in simulation environment */
    bcm_pkt.flags = BCM_TX_CRC_APPEND;
#endif
    bcm_pkt.cos = hapiBroadTxCosTable[BROAD_TX_PRIO_NORMAL];

    /* if sending a BPDU, make sure it goes to all ports that are not 
       participating in STP */
    rv = bcmx_lplist_init(&lplist,L7_MAX_INTERFACE_COUNT,0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOG_ERROR(rv);
    }
    rv = bcmx_lplist_init(&untaggedLplist,L7_MAX_INTERFACE_COUNT,0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOG_ERROR(rv);
    }
    rv = bcmx_lplist_init(&taggedLplist,L7_MAX_INTERFACE_COUNT,0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOG_ERROR(rv);
    }

    if (bpdu_msg.sendBpdu)
    {
      /* clear ports from bit mask that are not link up, or originally received the PDU */
      hapiBroadGetStpNotParticipatingPbmp(&bpdu_msg.usp,&lplist,dapi_g);
      hapiBroadPruneTxUnauthorizedPorts(&lplist,dapi_g);
    }
    /* otherwise send the packet to all ports in the VLAN */
    else
    {
      /* clear ports from bit mask that are not link up, or originally received the PDU */
      hapiBroadGetSameVlanPbmp(bpdu_msg.vlanID,&bpdu_msg.usp,&lplist,dapi_g);
      hapiBroadPruneTxDiscardingPorts(bpdu_msg.vlanID, &lplist, dapi_g);
      hapiBroadPruneTxUnauthorizedPorts(&lplist,dapi_g);
    }

    /*
    ** Check for GMRP, ISIS, they must honor egress tagging
    */
    mode = bpdu_msg.dtag_mode;

    if ((bpdu_msg.proto_byte == 0x20) ||                           /* Forward GMRP tagged (double or not) */
        (bpdu_msg.proto_byte == 0x14) ||                           /* ISIS */
        (bpdu_msg.proto_byte == 0x15) ||                           /* ISIS */
        ((mode == HAPI_BROAD_DTAG_MODE_EXTERNAL) &&         /* Forward BPDU's and GVRP if double tagged */
         ((bpdu_msg.proto_byte == 0x00) || (bpdu_msg.proto_byte == 0x21))))
    {
      hapiBroadTaggedStatusLplistSet(bpdu_msg.vlanID, &lplist, &taggedLplist, &untaggedLplist, dapi_g);
    }
    else
    {
      bcmx_lplist_copy(&untaggedLplist,&lplist);
    }

    /* Check for mirror case */

    if (hapiBroadProbeActive(dapi_g))
    {
      DAPI_USP_t searchUsp;
      DAPI_PORT_t  *mirrorDapiPortPtr;
      BROAD_PORT_t *mirrorHapiPortPtr = L7_NULLPTR;
      BROAD_PORT_t *probeHapiPortPtr = HAPI_PORT_GET(&dapi_g->system->probeUsp,dapi_g);

      for (searchUsp.unit = 0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++) 
      {
        for (searchUsp.slot = 0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++) 
        {
          if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardId != L7_NULL) 
          {
            for (searchUsp.port = 0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++) 
            {
              if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
              {
                continue;
              }

              mirrorDapiPortPtr = DAPI_PORT_GET(&searchUsp, dapi_g);
              if (((IS_PORT_TYPE_PHYSICAL(mirrorDapiPortPtr) == L7_TRUE) || 
                   (IS_PORT_TYPE_LOGICAL_LAG(mirrorDapiPortPtr) == L7_TRUE)) &&
                      ((mirrorDapiPortPtr->isMirrored == L7_TRUE) && 
                           ((mirrorDapiPortPtr->mirrorType == DAPI_MIRROR_BIDIRECTIONAL) || 
                            (mirrorDapiPortPtr->mirrorType == DAPI_MIRROR_EGRESS)))) 
              {
                if (IS_PORT_TYPE_LOGICAL_LAG(mirrorDapiPortPtr) == L7_TRUE)
                {
                  hapiBroadLagCritSecEnter ();
                  for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
                  {
                    if (mirrorDapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
                    {
                      mirrorHapiPortPtr = HAPI_PORT_GET(&mirrorDapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
                      break;
                    }
                  }
                  hapiBroadLagCritSecExit ();
                  if (i == L7_MAX_MEMBERS_PER_LAG)
                  {
                    continue;
                  }
                }
                else
                {
                  mirrorHapiPortPtr = HAPI_PORT_GET(&(searchUsp), dapi_g);
                }
  
                rv = bcmx_lplist_index_get(&untaggedLplist,mirrorHapiPortPtr->bcmx_lport);
                if (rv >= 0)
                {
                   BCMX_LPLIST_ADD(&untaggedLplist,probeHapiPortPtr->bcmx_lport);
                }
  
                rv = bcmx_lplist_index_get(&taggedLplist,mirrorHapiPortPtr->bcmx_lport);
                if (rv >= 0)
                {
                   BCMX_LPLIST_ADD(&taggedLplist,probeHapiPortPtr->bcmx_lport);
                }
              }
            }
          }
        }
      }
    }

    /* Check for untagged and send them */
    if (BCMX_LPLIST_IS_EMPTY(&untaggedLplist) == L7_FALSE)
    {
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_TRUE);

      rv = bcmx_tx_lplist_intercept (&bcm_pkt, &untaggedLplist, &untaggedLplist, BCMX_TX_F_CPU_TUNNEL, dapi_g); /* sdk 5.3.1 change */
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
      }
    }
    /* Check for tagged and send them */
    if (BCMX_LPLIST_IS_EMPTY(&taggedLplist) == L7_FALSE)
    {
      bcmx_tx_pkt_untagged_set(&bcm_pkt,L7_FALSE);

      rv = bcmx_tx_lplist_intercept (&bcm_pkt, &taggedLplist, NULL, BCMX_TX_F_CPU_TUNNEL, dapi_g); /* sdk 5.3.1 change */
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
      }
    }

    bcmx_lplist_free(&lplist);
    bcmx_lplist_free(&untaggedLplist);
    bcmx_lplist_free(&taggedLplist);
    sal_dma_free(bcm_pkt.pkt_data->data);
  }
}

/*********************************************************************
*
* @purpose Determines if a port is forwarding
*
* @param   DAPI_USP_t      *usp      - port of interest
* @param   L7_ushort16      vlanId   - vlan of interest
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_BOOL
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadPortIsForwarding(DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g)
{
  L7_BOOL     rc = L7_FALSE;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_SYSTEM_t *hapiSystemPtr;
  DAPI_PORT_t    *dapiPortPtr;
  L7_uint32       instNumber;
  bcm_stg_t       stg;
  L7_uint32       stg_index;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  dapiPortPtr = GET_DAPI_PORT(dapi_g, usp);

  /* If this is a router port, then it is forwarding */
  if ((IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE) &&
      (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE))
    return L7_TRUE;

  if (L7_BRIDGE_SPEC_SUPPORTED == L7_BRIDGE_SPEC_802DOT1D)
  {
    if ((hapiPortPtr->stp_state == DAPI_PORT_STP_NOT_PARTICIPATING) ||
        (hapiPortPtr->stp_state == DAPI_PORT_STP_FORWARDING))
    {
      rc = L7_TRUE;
    }
  }
  else
  {
    if (vlanId > L7_MAX_VLAN_ID)
    {
      L7_LOG_ERROR(vlanId);
    }

    instNumber = hapiSystemPtr->stg_instNumber[vlanId];

    if (hapiBroadDot1sBcmStgMapGet(instNumber,&stg,&stg_index) == L7_SUCCESS)
    {
      if ((hapiPortPtr->dot1s_state[stg_index] == DAPI_PORT_DOT1S_NOT_PARTICIPATING) ||
          (hapiPortPtr->dot1s_state[stg_index] == DAPI_PORT_DOT1S_FORWARDING))
      {
        rc = L7_TRUE;
      }
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose This routine get the port bit map of same VLAN which are link up
*
* @param   L7_int32    unit
* @param   L7_short16  vlanID
* @param   DAPI_USP_t *usp
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   This routine is for use on reception... it includes all ports
*          w/ link up that are in the VLAN, but excludes the port
*          that originally received the pkt.
*
* @end
*
*********************************************************************/
void hapiBroadGetSameVlanPbmp(L7_ushort16 vlanID, DAPI_USP_t *usp, bcmx_lplist_t *memberLplist, DAPI_t *dapi_g)
{
  DAPI_USP_t             searchUsp;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;

  for (searchUsp.unit=0;searchUsp.unit < dapi_g->system->totalNumOfUnits;searchUsp.unit++)
  {
    for (searchUsp.slot=0;searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots;searchUsp.slot++)
    {
      if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE)
      {
        for (searchUsp.port=0;searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot;searchUsp.port++)
        {
          if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
          {
            continue;
          }

          dapiPortPtr = DAPI_PORT_GET(&searchUsp,dapi_g);
          hapiPortPtr = HAPI_PORT_GET(&searchUsp,dapi_g);

          if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) ||
              (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr) == L7_TRUE))
          {
            /* Get the true hardware vlan membership
            ** Lags will be accounted for in later function
            ** calls
            */
            if (BROAD_IS_HW_VLAN_MEMBER(&searchUsp,vlanID,dapi_g))
            {
              BCMX_LPLIST_ADD(memberLplist,hapiPortPtr->bcmx_lport);
            }
          }
        }
      }
    }
  }

  hapiBroadPruneRxPort(usp, memberLplist, dapi_g);

  hapiBroadPruneTxPorts(memberLplist, dapi_g);

  return;
}

/*********************************************************************
*
* @purpose This routine get the port bit map of same VLAN which are link up
*
* @param   L7_int32    unit
* @param   L7_short16  vlanID
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   This routine is for use on transmission... it includes all ports
*          w/ link up that are in the VLAN.
*
* @end
*
*********************************************************************/
void hapiBroadGetSameVlanPbmpTx(L7_ushort16 vlanID, bcmx_lplist_t *lplist, L7_BOOL filter_linkup, DAPI_t *dapi_g)   /* PTin modified: filter_linkup */
{
  DAPI_USP_t             searchUsp;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  int                    link_status;     /* PTin modified: link status */

  for (searchUsp.unit=0;searchUsp.unit < dapi_g->system->totalNumOfUnits;searchUsp.unit++)
  {
    for (searchUsp.slot=0;searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots;searchUsp.slot++)
    {
      if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE)
      {
        for (searchUsp.port=0;searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot;searchUsp.port++)
        {
          if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
          {
            continue;
          }

          dapiPortPtr = DAPI_PORT_GET(&searchUsp,dapi_g);
          hapiPortPtr = HAPI_PORT_GET(&searchUsp,dapi_g);

          if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) ||
              (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr) == L7_TRUE))
          {
            /* Get the true hardware vlan membership
            ** Lags will be accounted for in later function
            ** calls
            */
            if (BROAD_IS_HW_VLAN_MEMBER(&searchUsp,vlanID,dapi_g))
            {
              /* PTin modified: filter considering link status */
              if (!filter_linkup ||
                  (bcmx_port_link_status_get(hapiPortPtr->bcmx_lport, &link_status) == BCM_E_NONE && link_status == BCM_PORT_LINK_STATUS_UP))
              {
                BCMX_LPLIST_ADD(lplist,hapiPortPtr->bcmx_lport);
              }
            }
          }
        }
      }
    }
  }
  hapiBroadPruneTxPorts(lplist, dapi_g);

  return;
}

/*********************************************************************
*
* @purpose This routine clears bits in a port bitmap of the src port
*
* @param   L7_int32    unit
* @param   DAPI_USP_t *usp
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPruneRxPort(DAPI_USP_t *usp, bcmx_lplist_t *lplist, DAPI_t *dapi_g)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  L7_int32      i;
  DAPI_USP_t   lag_member_usp;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

    BCMX_LPLIST_REMOVE(lplist, hapiPortPtr->bcmx_lport);
  }
  else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      lag_member_usp = dapiPortPtr->modeparm.lag.memberSet[i].usp;

      if ((dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE) &&
          (isValidUsp (&lag_member_usp, dapi_g) == L7_TRUE))
      {
        hapiPortPtr = HAPI_PORT_GET(&lag_member_usp, dapi_g);

        BCMX_LPLIST_REMOVE(lplist, hapiPortPtr->bcmx_lport);
      }
    }
  }
  else if (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr) == L7_TRUE)
  {
    printf(" %s: fixme \n", __FUNCTION__);

  }
  else
  {
    L7_LOG_ERROR(0);
  }
}

/*********************************************************************
*
* @purpose This routine clears bits in a port bitmap of the destination
*          ports that are not link up, and makes sure only one LAG member
*          will forward the packet for any given LAG.
*          This routine also removes front panel stacking ports from the
*          port list.
*
* @param   L7_int32    unit
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPruneTxPorts(bcmx_lplist_t *lplist, DAPI_t *dapi_g)
{
  DAPI_PORT_t          *tempDapiPort;
  BROAD_PORT_t         *tempHapiPort;
  DAPI_PORT_t          *dapiLagPort;
  DAPI_USP_t            tempUsp;
  L7_int32              i;
  L7_BOOL               foundFirstMember;
  bcmx_lport_t          lport;
  bcmx_uport_t          uport;
  int                   count;
  bcmx_lplist_t         removeList;
  int                   rv;

  rv = bcmx_lplist_init(&removeList,L7_MAX_INTERFACE_COUNT,0);
  if (L7_BCMX_OK(rv) != L7_TRUE)
    L7_LOG_ERROR(rv);

  /* clear ports from bit mask that are not link up, or originally received the PDU */
  BCMX_LPLIST_IDX_ITER(lplist, lport, count)
  {
    uport = BCMX_UPORT_GET(lport);

    if (uport == BCMX_UPORT_INVALID_DEFAULT)
    {
      BCMX_LPLIST_ADD(&removeList, lport);
      continue;
    }

    HAPI_BROAD_UPORT_TO_USP(uport,&tempUsp);

    if (isValidUsp(&tempUsp,dapi_g) == L7_FALSE)
    {
      BCMX_LPLIST_ADD(&removeList, lport);
      continue; /* Remove invalid ports */
    }

    tempDapiPort = DAPI_PORT_GET(&tempUsp, dapi_g);
    tempHapiPort = HAPI_PORT_GET(&tempUsp, dapi_g);

    if (tempHapiPort->bcmx_lport != lport)
    {
      L7_LOG_ERROR((L7_ulong32)uport);
    }

    /* if this physical port is link down, clear the corresponding bit */
    if (tempDapiPort->modeparm.physical.isLinkUp == L7_FALSE)
    {
      BCMX_LPLIST_ADD(&removeList, lport);
      continue;
    }

    /* if this physical port is a router port, clear the corresponding bit */
    if (tempDapiPort->modeparm.physical.routerIntfEnabled == L7_TRUE)
    {
      BCMX_LPLIST_ADD(&removeList, lport);
      continue;
    }

    /* if this port is a LAG member, make sure only one member of that LAG sends the pkt */
    if (tempHapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      dapiLagPort = DAPI_PORT_GET(&tempHapiPort->hapiModeparm.physical.lagUsp, dapi_g);

      foundFirstMember = L7_FALSE;

      /* find the lowest member port, and allow it to forward the frame.
         all other members should not forward the frame */
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        tempUsp = dapiLagPort->modeparm.lag.memberSet[i].usp;
        if ((dapiLagPort->modeparm.lag.memberSet[i].inUse == L7_TRUE) &&
            (isValidUsp(&tempUsp, dapi_g) == L7_TRUE))
        {
          /* if we've already found the first member, then clear this one from the mask */
          if (foundFirstMember == L7_TRUE)
          {

            tempHapiPort = HAPI_PORT_GET(&tempUsp, dapi_g);

            BCMX_LPLIST_ADD(&removeList, tempHapiPort->bcmx_lport);
          }
          else
          {
            foundFirstMember = L7_TRUE;
          }
        }/* if */
      }
    }
  }

  /* Now remove the port from the list */
  BCMX_LPLIST_IDX_ITER(&removeList, lport, count)
  {
    if (!BCM_GPORT_IS_WLAN_PORT(lport))
    {
      BCMX_LPLIST_REMOVE(lplist, lport);
    }
  }

  bcmx_lplist_free(&removeList);
}

/*********************************************************************
*
* @purpose This routine clears bits in a port bitmap of the destination
*          ports that are not forwarding.
*
* @param   L7_int32    unit
* @param   L7_ushort16 vlanId
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPruneTxDiscardingPorts(L7_ushort16 vlanId, bcmx_lplist_t *lplist, DAPI_t *dapi_g)
{
  BROAD_PORT_t         *tempHapiPort;
  DAPI_PORT_t          *dapiLagPort;
  DAPI_USP_t            tempUsp;
  DAPI_USP_t            memberUsp;
  L7_int32              i;
  L7_BOOL               foundFirstMember;

  bcmx_lport_t          lport;
  bcmx_uport_t          uport;
  int                   count;
  bcmx_lplist_t         removeList;
  int                   rv;

  rv = bcmx_lplist_init(&removeList,L7_MAX_INTERFACE_COUNT,0);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOG_ERROR(rv);
  }

  /* clear ports from bit mask that are not link up, or originally received the PDU */
  BCMX_LPLIST_IDX_ITER(lplist, lport, count)
  {
    uport = BCMX_UPORT_GET(lport);

    if (uport == BCMX_UPORT_INVALID_DEFAULT)
    {
      BCMX_LPLIST_ADD(&removeList, lport);
      continue;
    }

    HAPI_BROAD_UPORT_TO_USP(uport,&tempUsp);

    if (isValidUsp(&tempUsp,dapi_g) == L7_FALSE)
    {
      continue; /* Ignore invalid ports */
    }

    tempHapiPort = HAPI_PORT_GET(&tempUsp, dapi_g);

    if (tempHapiPort->bcmx_lport != lport)
    {
      L7_LOG_ERROR((L7_ulong32)uport);
    }

    /* if this port is a LAG member, make sure only one member of that LAG sends the pkt */
    if (tempHapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      tempUsp = tempHapiPort->hapiModeparm.physical.lagUsp;
      dapiLagPort = DAPI_PORT_GET(&tempUsp, dapi_g);

      foundFirstMember = L7_FALSE;

      /* find the lowest member port, and allow it to forward the frame.
         all other members should not forward the frame */
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        memberUsp = dapiLagPort->modeparm.lag.memberSet[i].usp;
        if ((dapiLagPort->modeparm.lag.memberSet[i].inUse == L7_TRUE) &&
            (isValidUsp (&memberUsp, dapi_g) == L7_TRUE))
        {

          /* if we've already found the first member, then clear this one from the mask */
          if (foundFirstMember == L7_TRUE)
          {
            tempUsp = dapiLagPort->modeparm.lag.memberSet[i].usp;

            tempHapiPort = HAPI_PORT_GET(&tempUsp, dapi_g);

            BCMX_LPLIST_ADD(&removeList, tempHapiPort->bcmx_lport);
          }
          else
          {
            /* if the LAG is not forwarding, clear the member bit */
            if (hapiBroadPortIsForwarding(&tempUsp, vlanId, dapi_g) == L7_FALSE)
            {
              tempUsp = dapiLagPort->modeparm.lag.memberSet[i].usp;

              tempHapiPort = HAPI_PORT_GET(&tempUsp, dapi_g);

              BCMX_LPLIST_ADD(&removeList, tempHapiPort->bcmx_lport);
            }
          }
          foundFirstMember = L7_TRUE;
        }
      }
    }
    else
    {
      if (hapiBroadPortIsForwarding(&tempUsp, vlanId, dapi_g) == L7_FALSE)
      {
        BCMX_LPLIST_ADD(&removeList, lport);
      }
    }
  }

  /* Now remove the port from the list */
  BCMX_LPLIST_IDX_ITER(&removeList, lport, count)
  {
    if (!BCM_GPORT_IS_WLAN_PORT(lport))
    {
      BCMX_LPLIST_REMOVE(lplist, lport);
    }
  }

  bcmx_lplist_free(&removeList);
}


/*********************************************************************
*
* @purpose This routine sets the tagging status of the ports passed in the lplist
*
* @param   vlanID          vlan to use for tagging status
* @param   *lplist         starting list of members both tagged and untagged
* @param   *tagedlplist    resulting list of members tagged members
* @param   *untagedlplist  resulting list of members untagged members
* @param   *dapi_g
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadTaggedStatusLplistSet(L7_ushort16 vlanId, bcmx_lplist_t *lplist, bcmx_lplist_t *taggedLplist,
                                       bcmx_lplist_t *untaggedLplist, DAPI_t *dapi_g)
{
  int           count;
  bcmx_lport_t  lport;
  DAPI_USP_t    usp;
  BROAD_PORT_t *hapiPortPtr;
  L7_uint32     localUnitNumber;

  /* Grab the local unit to know whether or not to tag */
  if (unitMgrNumberGet(&localUnitNumber) != L7_SUCCESS)
    return L7_FAILURE;

  BCMX_LPLIST_IDX_ITER(lplist,lport, count)
  {
    if (!BCM_GPORT_IS_WLAN_PORT(lport))
    {
    HAPI_BROAD_UPORT_TO_USP(BCMX_UPORT_GET(lport),&usp);

    if (isValidUsp(&usp,dapi_g) == L7_FALSE)
    {
      continue; /* Ignore invalid ports */
    }

    hapiPortPtr = HAPI_PORT_GET(&usp,dapi_g);

    if (hapiPortPtr->bcmx_lport != lport)
    {
      L7_LOG_ERROR((L7_ulong32)lport);
    }

    /* If its a member of a lag, use the lags tagging status */
    if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      if (BROAD_IS_VLAN_TAGGING(&hapiPortPtr->hapiModeparm.physical.lagUsp,vlanId,dapi_g))
      {
        BCMX_LPLIST_ADD(taggedLplist,lport);
      }
      else
      {
        BCMX_LPLIST_ADD(untaggedLplist,lport);
      }
    }
    /* Otherwise use the tagging status of the port itself */
    else
    {
      if (BROAD_IS_VLAN_TAGGING(&usp,vlanId,dapi_g))
      {
        BCMX_LPLIST_ADD(taggedLplist,lport);
      }
      else
      {
        BCMX_LPLIST_ADD(untaggedLplist,lport);
      }
    }
  }
    else
    {
      /* l2 tunnel ports are always tagging */
      BCMX_LPLIST_ADD(taggedLplist,lport);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose This routine clears bits in a port bitmap of the destination
*          ports that are not dot1x authorized.
*
* @param   L7_int32    unit
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPruneTxUnauthorizedPorts(bcmx_lplist_t *lplist, DAPI_t *dapi_g)
{
  BROAD_PORT_t         *tempHapiPort;
  DAPI_USP_t            tempUsp;
  bcmx_lport_t          lport;
  bcmx_uport_t          uport;
  int                   count;
  bcmx_lplist_t         removeList;
  int                   rv;

  rv = bcmx_lplist_init(&removeList,L7_MAX_INTERFACE_COUNT,0);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOG_ERROR(rv);
  }

  /* clear ports from bit mask that are not link up, or originally received the PDU */
  BCMX_LPLIST_IDX_ITER(lplist, lport, count)
  {
    uport = BCMX_UPORT_GET(lport);

    if (uport == BCMX_UPORT_INVALID_DEFAULT)
    {
      BCMX_LPLIST_ADD(&removeList, lport);
      continue;
    }

    HAPI_BROAD_UPORT_TO_USP(uport,&tempUsp);

    if (isValidUsp(&tempUsp,dapi_g) == L7_FALSE)
    {
      continue;  /* Ignore invalid ports. */
    }

    tempHapiPort = HAPI_PORT_GET(&tempUsp, dapi_g);

    if (tempHapiPort->bcmx_lport != lport)
    {
      L7_LOG_ERROR((L7_ulong32)uport);
    }

    /* if this physical port is not authorized, clear the corresponding bit */
    if (!HAPI_DOT1X_PORT_IS_AUTHORIZED(tempHapiPort))
    {
      BCMX_LPLIST_ADD(&removeList, lport);
    }
  }

  /* Now remove the port from the list */
  BCMX_LPLIST_IDX_ITER(&removeList, lport, count)
  {
    if (!BCM_GPORT_IS_WLAN_PORT(lport))
    {
      BCMX_LPLIST_REMOVE(lplist, lport);
    }
  }

  bcmx_lplist_free(&removeList);
}


void dumpHapiDot1SState(int unit, int slot, int port)
{
  DAPI_USP_t usp;
  BROAD_PORT_t             *hapiPortPtr;
  int i;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;
  hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
  for (i=0;i<HAPI_MAX_MULTIPLE_STP_INSTANCES;i++)
  {
    printf("index is vlanId\n");
    printf("dot1s_state[%d] = %d\n",i,hapiPortPtr->dot1s_state[i]);
  } 
}

void hapiBroadRxTtlFixup(L7_uchar8 *pkt)
{
  L7_ipHeader_t *ipHdr;
  L7_uint32      hlen;
  L7_uint32      sum = 0;
  L7_ushort16   *ptr;

  /* this code assumes that the packet comes from the HW w/ a VLAN tag */
  ipHdr = (L7_ipHeader_t *) &pkt[18];

  ipHdr->iph_ttl += 1;
  hlen = (ipHdr->iph_versLen & 0x0f) << 2;
  ipHdr->iph_csum = 0;

  ptr = (L7_ushort16 *) ipHdr;

  while (hlen > 1)
  {
    sum += *ptr;
    ptr++;
    if (sum & 0x80000000)
      sum = (sum & 0xffff) + (sum >> 16);
    hlen -= 2;
  }

  while (sum >> 16)
  {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  ipHdr->iph_csum = ~sum;
}
static L7_BOOL hapiBroadProbeActive(DAPI_t *dapi_g)
{
    BROAD_PORT_t *probePortPtr;
    if((isValidUsp(&dapi_g->system->probeUsp, dapi_g)) && (dapi_g->system->mirrorEnable))
    {
        probePortPtr = HAPI_PORT_GET(&dapi_g->system->probeUsp,dapi_g);
        if(probePortPtr->hapiModeparm.physical.admin_enabled == L7_TRUE)
        {
            return L7_TRUE;
        }
    }
    return L7_FALSE;
}



/*********************************************************************
*
* @purpose Determines whether frame should be dropped or passed to application
*          when received , based on MAC DA . 
*
* @param   BROAD_PKT_RX_MSG_t *pktRxMsg 
* @param   DAPI_t             *dapi_g
*
* @returns L7_BOOL L7_TRUE  - packet should be forwarded to application
*          L7_BOOL L7_FALSE - packet should be dropped
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadMacDaCheck(BROAD_PKT_RX_MSG_t *pktRxMsg, 
                            bcm_chip_family_t board_family, 
                            DAPI_t *dapi_g)
{
  L7_netBufHandle        frameHdl = L7_NULL;
  L7_uchar8             *user_data;
  L7_uchar8              vrrp_mac_da[6] = {0x00,0x00,0x5e,0x00,0x01,0x00};
  BROAD_SYSTEM_t        *hapiSystemPtr = L7_NULLPTR;
  BROAD_PORT_t          *hapiPortPtr = L7_NULLPTR;
  L7_ushort16            etype,dstPort,srcPort;
  L7_uint32              protocol =0;
  L7_udp_header_t        *updHeader;

  frameHdl = pktRxMsg->cmdInfo.cmdData.receive.frameHdl;
  user_data = sysapiNetMbufGetDataStart (frameHdl);
  
  /* Drop the packet is source mac address is Multicast or Broadcast address */
  if (user_data[6] & 0x01)
  {
    return L7_FALSE;
  }
  if (isValidUsp(&pktRxMsg->usp,dapi_g) == L7_FALSE) return L7_FALSE;

  hapiPortPtr = HAPI_PORT_GET(&pktRxMsg->usp, dapi_g);

  /* Allow everything to go up to the application if Captive Portal is enabled on the port */
  if (hapiPortPtr->cpEnabled == L7_TRUE) 
  {
    return L7_TRUE;
  }

  /* if multicast (also catches broadcast), don't drop */
  if (user_data[0] & 0x01)
  {
    return L7_TRUE;
  }

  /* if the packet is to a VRRP router interface, don't drop */
  if (memcmp(user_data, vrrp_mac_da, 5) == 0)
  {
    return L7_TRUE;
  }

  /* if dot1x EAPOL or IPv6, don't drop */
  /* Ipv6 packets to any mac DA should be allowed */
  etype = osapiNtohs(*(L7_ushort16 *)(&user_data[16]));
  if ((etype == L7_ETYPE_EAPOL) || (etype == L7_ETYPE_IPV6))
  {
    return L7_TRUE;
  }
  /* PTin added: PPPoE */
  if (etype == L7_ETYPE_PPPOE)
  {
    return L7_TRUE;
  }

#ifdef BCM_ROBO_SUPPORT
  if((etype==L7_ETYPE_IP))
#else
  /* Send sFlow sampled pkts to CPU */
  if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonSampleSource) || 
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonSampleDest))
  {
    return L7_TRUE;
  }

  if ((BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonFilterMatch)) && (etype==L7_ETYPE_IP))
#endif
  {
    if (hapiPortPtr->voipPolicy != BROAD_POLICY_INVALID)
    {
    updHeader = (L7_udp_header_t *)(user_data+L7_ENET_HDR_SIZE+ L7_8021Q_ENCAPS_HDR_SIZE + L7_IP_HDR_LEN+2);

    dstPort= osapiNtohs(updHeader->destPort);
    srcPort =osapiNtohs(updHeader->sourcePort);


    if ((srcPort == L7_SIP_L4_PORT) || (dstPort == L7_SIP_L4_PORT))
        protocol = L7_QOS_VOIP_PROTOCOL_SIP;
    else if ((srcPort == L7_MGCP_L4_PORT) || (dstPort == L7_MGCP_L4_PORT))
        protocol = L7_QOS_VOIP_PROTOCOL_MGCP;
    else if ((srcPort == L7_SCCP_L4_PORT) || (dstPort == L7_SCCP_L4_PORT))
        protocol = L7_QOS_VOIP_PROTOCOL_SCCP;
    else if ((srcPort == L7_H323_L4_PORT) || (dstPort == L7_H323_L4_PORT))
       protocol = L7_QOS_VOIP_PROTOCOL_H323;
   
     
    if (protocol != 0)
    {
       return L7_TRUE;
    }
     }
   } 

  /* if MAC DA == bridge MAC, don't drop */
  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

#ifdef HAPI_BROAD_PKT_DEBUG
  if (!hapiBroadDebugPktIsFiltered(user_data))
  {
    if (pktRxDebugLevel)
    {
      printf("Comparing DA to %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
             hapiSystemPtr->bridgeMacAddr.addr[0],
             hapiSystemPtr->bridgeMacAddr.addr[1],
             hapiSystemPtr->bridgeMacAddr.addr[2],
             hapiSystemPtr->bridgeMacAddr.addr[3],
             hapiSystemPtr->bridgeMacAddr.addr[4],
             hapiSystemPtr->bridgeMacAddr.addr[5]);
    }
  }
#endif

  if (memcmp(hapiSystemPtr->bridgeMacAddr.addr, user_data, 3 /*6*/) == 0)
  {
    return L7_TRUE;
  }

  if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
  {
    hapiPortPtr = HAPI_PORT_GET(&hapiPortPtr->hapiModeparm.physical.lagUsp, dapi_g);
  }
  if ((hapiPortPtr->dhcpSnoopingEnabled == L7_TRUE) && (hapiBroadPktIsDhcp(user_data)))
  {
    return L7_TRUE;
  }
  if ((hapiPortPtr->dynamicArpInspectionTrusted == L7_FALSE) && (hapiBroadPktIsArp(user_data)))
  {
    return L7_TRUE;
  }

#ifdef L7_ROUTING_PACKAGE
  {
    L7_uchar8              router_mac[6];
    L7_RC_t                result;

    memset(router_mac, 0, sizeof(router_mac));

    /* Compare the DA to Router DA. Do not cache the router
    ** mac as router mac can change on a warm failover
     */
    result = sysapiHpcIfaceMacGet(L7_LOGICAL_VLAN_INTF, L7_VLAN_SLOT_NUM, 0,
                                  L7_NULLPTR, (L7_uchar8 *)router_mac);
    if (result == L7_SUCCESS)
    {
#ifdef HAPI_BROAD_PKT_DEBUG
        if (!hapiBroadDebugPktIsFiltered(user_data))
        {
          if (pktRxDebugLevel)
          {
            printf("Comparing DA to %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                 router_mac[0], router_mac[1], router_mac[2],
                 router_mac[3], router_mac[4], router_mac[5]);
        }
      }
#endif
      if (memcmp (user_data, router_mac, 3 /*6*/) == 0)
      {
          return L7_TRUE;
        }
    }

#ifdef L7_WIRELESS_PACKAGE
    {
      L7_BOOL destIsProxy;
      /*
      ** For wireless packages, we need to validate whether
      ** the destination address is being proxied for.
      */
      destIsProxy = hapiBroadL3MacDestIsProxy(user_data,
                                              pktRxMsg->cmdInfo.cmdData.receive.vlanID);
      if (destIsProxy == L7_TRUE)
      {
          return L7_TRUE;
        }
    }
#endif /* !L7_WIRELESS_PACKAGE */
  }
#endif /* End L7_ROUTING_PACKAGE */


#ifdef L7_ISCSI_PACKAGE
  if ((BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonFilterMatch)) &&
      (etype==L7_ETYPE_IP) &&
      (hapiBroadQosIscsiActiveStatusGet() == L7_TRUE))
    return L7_TRUE;
#endif /* L7_ISCSI_PACKAGE */


  /* drop the pkt */
  return L7_FALSE;
}

L7_RC_t hapiBroadRxLegacy(BROAD_PKT_RX_MSG_t *pktRxMsg, 
                          L7_BOOL *dropFrame, 
                          bcm_chip_family_t board_family, 
                          DAPI_t *dapi_g)
{
#ifdef L7_ROUTING_PACKAGE
  L7_uchar8           *pkt;
  L7_netBufHandle      frameHdl=L7_NULL;
  DAPI_PORT_t         *dapiPortPtr;
  DAPI_PORT_t         *dapiMemberPortPtr;
  DAPI_USP_t           vlanIntfUsp;
  DAPI_USP_t           memberUsp;
  L7_uint32            done;
#endif
  L7_RC_t rc  = L7_SUCCESS;

  /* preset the dropFrame */
  *dropFrame = L7_FALSE;

#ifdef L7_ROUTING_PACKAGE
  /* Check to see if the packet was routed to the CPU. If so:
     If it arrived on a router vlan interface, set the MAC DA to the router vlan
     MAC address and make sure the vlan passed up is valid in case it's going to
     a port interface. This is because the application will drop the packet if we indicate a
     VLAN greater than L7_PLATFORM_MAX_VLAN_ID. 
     If it arrived on a router port, set the MAC DA to this port's router MAC address.
     This is a workaround for the fact that L3 entries in the HW always route the 
     packet, even when going to the CPU */

  /* We only test IP frames, otherwise we may corrupt spanning tree BPDUs or other frames
  ** that were received due to a loopback cable.
  */
  frameHdl = pktRxMsg->cmdInfo.cmdData.receive.frameHdl;

  pkt = sysapiNetMbufGetDataStart(frameHdl);

  if ((pkt[16] == 0x08) && (pkt[17] == 0x00) &&
      (hapiBroadRxMacSaIsL3Intf(&pkt[6], dapi_g) == L7_TRUE))
  {
    /* increment the TTL and recalculate the IP checksum to undo what was done
       when the HW routed this packet to the CPU */
    hapiBroadRxTtlFixup(pkt);

    dapiPortPtr = GET_DAPI_PORT(dapi_g, &pktRxMsg->usp);

    if (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_FALSE)
    {
      /* packet arrived on a vlan router interface */
      vlanIntfUsp.unit = L7_LOGICAL_UNIT;
      vlanIntfUsp.slot = L7_VLAN_SLOT_NUM;

      done = 0;

      for (vlanIntfUsp.port = 0; vlanIntfUsp.port < L7_MAX_NUM_VLAN_INTF; vlanIntfUsp.port++)
      {
        if (isValidUsp(&vlanIntfUsp, dapi_g) == L7_FALSE)
        {
          continue;
        }

        dapiPortPtr = GET_DAPI_PORT(dapi_g, &vlanIntfUsp);

        /* if this intf is valid... */
        if (dapiPortPtr->modeparm.router.ipAddr)
        {
          /* change MAC DA to match vlan router mac */
          memcpy(pkt, dapiPortPtr->modeparm.router.macAddr, 6); 

          /* make sure intf has 'link up' */
          /* if any member ports of the VLAN have link up, then the VLAN has link up */
          for (memberUsp.unit = 0; memberUsp.unit < dapi_g->system->totalNumOfUnits; memberUsp.unit++)
          {
            for (memberUsp.slot = 0; memberUsp.slot < dapi_g->unit[memberUsp.unit]->numOfSlots; memberUsp.slot++)
            {
              if (dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->cardPresent == L7_TRUE)
              {
                for (memberUsp.port = 0; memberUsp.port < dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->numOfPortsInSlot; memberUsp.port++)
                {
                  if (isValidUsp(&memberUsp,dapi_g) == L7_FALSE)
                  {
                    continue;
                  }

                  if (BROAD_IS_VLAN_MEMBER(&memberUsp, dapiPortPtr->modeparm.router.vlanID, dapi_g))
                  {
                    dapiMemberPortPtr = GET_DAPI_PORT(dapi_g, &memberUsp);

                    if (dapiMemberPortPtr->modeparm.physical.isLinkUp == L7_TRUE)
                    {
                      pktRxMsg->cmdInfo.cmdData.receive.vlanID = dapiPortPtr->modeparm.router.vlanID;
                      done = 1;
                      break;
                    }
                  }
                }
              }
              if (done)
              {
                break;
              }
            }
          }

          if (done)
          {
            break;
          }
        }
      }
    }
    else
    {
      /* packet arrived on a router port interface */
      memcpy(pkt, dapiPortPtr->modeparm.physical.macAddr, 6);  /* change MAC DA to match port router mac */

      /* write an invalid VLAN, so the application doesn't think this was received on a VLAN routing intf */
      *(L7_ushort16 *)&pkt[14] = 0x0000;
      pktRxMsg->cmdInfo.cmdData.receive.vlanID = 0;
    }
  }
#endif

    if ((board_family == BCM_FAMILY_DRACO) || 
             (board_family == BCM_FAMILY_TUCANA))
    {
      if (hapiBroadXgsRxCheck(pktRxMsg, dapi_g) == L7_FALSE)
      {
        *dropFrame = L7_TRUE;
      }
    }
    else
    {
      /* unknown board family drop frame */
      sysapiPrintf("Unkown Board Family 0x%x in Receive\n",board_family);
      *dropFrame = L7_TRUE;
    }

    return rc;
}


L7_RC_t hapiBroadRxXgs3(BROAD_PKT_RX_MSG_t *pktRxMsg,L7_BOOL *fwdFrame,DAPI_t *dapi_g)
{
  L7_RC_t               rc = L7_SUCCESS; 

  /* default to forward the frame */
  *fwdFrame = L7_FALSE;

  *fwdFrame = hapiBroadXgs3RxCheck(pktRxMsg, dapi_g);

  return rc;
}

/*********************************************************************
*
* @purpose Determines whether frame should be dropped or passed to application
*          when received on a XGS

* @param   DAPI_t             *dapi_g
*
* @returns L7_BOOL L7_TRUE  - packet should be forwarded to application
*                      L7_FALSE - packet should be dropped
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadXgs3RxCheck(BROAD_PKT_RX_MSG_t *pktRxMsg, DAPI_t *dapi_g)
{
  L7_netBufHandle      frameHdl=L7_NULL;
  BROAD_PORT_t        *hapiPort;
  DAPI_PORT_t         *dapiPort;
  L7_BOOL              result = L7_TRUE;
  L7_uchar8           *user_data;
  L7_uchar8            reservedMacDa[] = {0x01,0x80,0xc2,0x00,0x00};
  L7_uchar8            isdp_macda[]    = {0x01,0x00,0x0c,0xcc,0xcc,0xcc};


  frameHdl = pktRxMsg->cmdInfo.cmdData.receive.frameHdl;
  hapiPort = HAPI_PORT_GET(&pktRxMsg->usp, dapi_g);
  dapiPort = DAPI_PORT_GET(&pktRxMsg->usp, dapi_g);

  /* Filter out the packet depending on the reason
   */

  user_data = sysapiNetMbufGetDataStart(frameHdl);

  if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonUnknownVlan) ||
      (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL2SourceMiss) &&
       (hpcSoftwareLearningEnabled () != L7_TRUE) && (hapiPort->cpEnabled == L7_FALSE) && (hapiPort->locked == L7_FALSE))||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL2DestMiss) ||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL2Cpu) ||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL3SourceMiss) ||
      BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonL3SourceMove))
  {
    /* drop the packet */
    result = L7_FALSE;
  }
  else if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonIcmpRedirect))
  {
    /* Drop the packet if ingress port is not part of the VLAN. Else packets get
     * duplicated, as XGS3 device forwards the datagram and so does FastPath.
     */  
    {
      DAPI_PORT_t  *dapiPortPtr = GET_DAPI_PORT(dapi_g, &pktRxMsg->usp);

      if (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_FALSE) 
      { 
        if (BROAD_IS_HW_VLAN_MEMBER(&pktRxMsg->usp, 
                   pktRxMsg->cmdInfo.cmdData.receive.vlanID, dapi_g) == L7_FALSE)
        {
            result = L7_FALSE;
        }
      }
    }
  }
  else if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonIpMcastMiss))
  {
    /* This reason code is set for IP multicast packets sent to the CPU 
       when IP multicast is enabled. It could occur for an RPF failure
       or a case where a packet misses the IP multicast table. Routing PDU
       frames will hit here when IP multicast is enabled */

    /* If this was received on a LAG member port, convert USP to
       the LAG USP */
    if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      pktRxMsg->usp = hapiPort->hapiModeparm.physical.lagUsp;
    }

#ifdef L7_MCAST_PACKAGE
    /* Check for RPF failures - if it's a true RPF failure and the action is
       drop, the function below will drop the frame. Otherwise it will return
       SUCCESS and we will break out to send it up */
    if (hapiBroadL3McastRPF(frameHdl, pktRxMsg->cmdInfo.cmdData.receive.vlanID, &pktRxMsg->usp, 
                            dapi_g) == L7_SUCCESS)
    {
      result = L7_TRUE;
    }
    else
    {
      /* drop the packet */
      result = L7_FALSE;
    }
#else
    result = L7_TRUE;
#endif
  }
  else if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonMcastMiss))
  {
    /* This reason code is set for MAC multicast packets. It could occur for cases
       such as when IP multicast is not enabled and the MARL search fails -  the
       packet gets flooded on the vlan which includes the CPU. */

    /* If IP multicast is disabled, routing PDUs come up w/ this reason code
       so check to see if this is a routing PDU and if it was received on a routing
       interface. If so, pass it up to the application, otherwise drop it */

    /* If this was received on a LAG member port, convert USP to
       the LAG USP */
    if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
    {
      pktRxMsg->usp = hapiPort->hapiModeparm.physical.lagUsp;
    }

    /* drop the packet */
    result = L7_FALSE;
  }
  else if (BCM_RX_REASON_GET(pktRxMsg->reasons, bcmRxReasonFilterMatch))
  {
    /* since we're now using the FFP to guarantee that MAC reserved pkts get to the 
       CPU, we need to check here to see if the MAC DA is reserved. Normally both 
       BCM_RX_PR_PROTOCOL_PKT and BCM_RX_PR_FFP are set, but if the port accepts
       only VLAN tagged pkts, then only BCM_RX_PR_FFP is set */
    if ( (memcmp(user_data, reservedMacDa, 5) == 0)  || 
         (memcmp(user_data, isdp_macda, 6) == 0) )
    {
      if (hapiBroadReceivePdu(frameHdl, pktRxMsg->cmdInfo.cmdData.receive.vlanID, 
                              &pktRxMsg->usp,pktRxMsg->rx_untagged, dapi_g,
                              pktRxMsg->cmdInfo.cmdData.receive.innerVlanId) == L7_TRUE)
      {
        result = L7_TRUE;
      }
      else
      {
        /* drop the packet */
        result = L7_FALSE;
      }
    }
    else
    {
      if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
      {
        pktRxMsg->usp = hapiPort->hapiModeparm.physical.lagUsp;
      }
      result = L7_TRUE;

      /* Perform VLAN checks since FFP may have sent packets to CPU that
         should have been dropped. */
      /* If received packet on port based routing interface */
      if (dapiPort->modeparm.physical.routerIntfEnabled == L7_TRUE)
      {
        if (hapiPort->port_based_routing_vlanid != pktRxMsg->cmdInfo.cmdData.receive.vlanID)
        {
          result = L7_FALSE;
        }
      }
      else /* not a port based routing intf */
      {
        if (hapiBroadQvlanVlanExists(pktRxMsg->cmdInfo.cmdData.receive.vlanID) == L7_TRUE)
        {
          if (((BROAD_PORT_t *)HAPI_PORT_GET(&pktRxMsg->usp, dapi_g))->ingressFilteringEnabled == L7_TRUE)
          {
            if (BROAD_IS_VLAN_MEMBER(&pktRxMsg->usp, 
                                      pktRxMsg->cmdInfo.cmdData.receive.vlanID, dapi_g) == L7_FALSE)
            {
                result = L7_FALSE;
            }
          }
        }
        else
        {
          result = L7_FALSE;
        }
      }
    }
  }
  else
  {
    if ( (memcmp(user_data, reservedMacDa, 5) == 0) || 
         (memcmp(user_data, isdp_macda, 6) == 0) )
    {
      if (hapiBroadReceivePdu(frameHdl, pktRxMsg->cmdInfo.cmdData.receive.vlanID, 
                              &pktRxMsg->usp,pktRxMsg->rx_untagged, dapi_g,
                              pktRxMsg->cmdInfo.cmdData.receive.innerVlanId) == L7_TRUE)
      {
        result = L7_TRUE;
      }
      else
      {
        /* drop the packet */
        result = L7_FALSE;
      }
    }
    else
    {
      /* liberal policy in the default due to reason code issues */
      result = L7_TRUE;

      if (hapiPort->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
      {
        pktRxMsg->usp = hapiPort->hapiModeparm.physical.lagUsp;
      }
    }
  }

  return result;
}

/* This can be used to debug CPU tx into different COS queues */
void hapiBroadDebugCpuTxCosSet(int highPrio, int normalPrio)
{
   if ((highPrio < 0 || highPrio > 7) || (normalPrio < 0 || normalPrio > 7))
   {
       printf("Cos Priorities for HIGH/NORMAL should be with in the range: [0-7] !!!\n");
       return;
   }

   printf("Updating COS PRIORITY :: HIGH (from %d to %d) NORMAL (from %d to %d)!!!\n", 
             hapiBroadTxCosTable[BROAD_TX_PRIO_HIGH], highPrio, 
             hapiBroadTxCosTable[BROAD_TX_PRIO_NORMAL], normalPrio);

   hapiBroadTxCosTable[BROAD_TX_PRIO_HIGH] =  highPrio;
   hapiBroadTxCosTable[BROAD_TX_PRIO_NORMAL] =  normalPrio;

   return;
}

static 
void hapiBroadSTagRemove(L7_uchar8 *pkt, L7_uint32 *frameLen)
{
  L7_ushort16 ether_type;
  if (dapi_g->system->dvlanEnable)
  {
    memcpy(&ether_type, &pkt[12], sizeof(ether_type));
    ether_type = osapiNtohs(ether_type);
    if (ether_type == hapiBroadDvlanEthertype)
    {
      memmove((pkt + 12), (pkt + 16),(*frameLen - 16));
      *frameLen = *frameLen - 4;
    }
  }
}

static
void hapiBroadInnerTagRemove(L7_uchar8 *pkt, L7_uint32 *frameLen)
{
  L7_ushort16 ether_type;
  if (dapi_g->system->dvlanEnable)
  {
    memcpy(&ether_type, &pkt[16], sizeof(ether_type));
    ether_type = osapiNtohs(ether_type);
    if (ether_type == FD_DVLANTAG_802_1Q_ETHERTYPE)
    {
      memmove((pkt + 16), (pkt + 20),(*frameLen - 20));
      *frameLen = *frameLen - 4;
    }
  }
}



#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
extern avlTree_t hapiBroadDot1adSubupTree;

void hapiBroadTagRemove(L7_uchar8 *pkt, L7_uint32 *frameLen)
{
  L7_ushort16 ether_type;

  memcpy(&ether_type, &pkt[12], sizeof(ether_type));
  ether_type = osapiNtohs(ether_type);
  if (ether_type == FD_DVLANTAG_802_1Q_ETHERTYPE)
  {
    memmove((pkt + 12), (pkt + 16),(*frameLen - 16));
    *frameLen = *frameLen - 4;
  }
}
/*********************************************************************
*
* @purpose This task forwards L2 PDUs to WAN ports when action
*                is tunnel and forwards to Lan port when action is
*                detunnel
*
* @param   L7_uint32        numArgs  - number of arguments
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t result
*
* @notes  This function gets SOT packet always. We shuffle the frame
*         based on inner VLAN id.
*
* @end
*
*********************************************************************/
void hapiBroadPduTransmitTask(L7_uint32 numArgs, DAPI_t *dapi_g)
{
  BROAD_TX_PDU_MSG_t              pdu_msg;
  bcm_pkt_t                       bcm_pkt;
  bcm_pkt_blk_t                   bcm_pkt_blk;
  DAPI_USP_t                      searchUsp;
  BROAD_PORT_t                    *hapiPort;
  BROAD_PORT_t                    *tmpHapiPort;
  BROAD_PORT_t                    *searchHapiPortPtr;
  DAPI_PORT_t                     *dapiPortPtr;
  bcmx_lport_t                    lPort=0;
  L7_ushort16                     svid=0;
  L7_ushort16                     remarkCVID=0;
  L7_uchar8                       *user_data;
  L7_BOOL                         tunnel;
  L7_BOOL                         isDownStreamFrame = L7_FALSE;
  L7_BOOL                         sendFrame = L7_FALSE;
  L7_uint32                       frameLength;
  BROAD_DOT1AD_SUBUP_ENTRY_t      *pUpEntry;
  BROAD_DOT1AD_SUBUP_KEY_t         key;
  L7_uchar8                        index = 0;
  

  while (1)
  {
    if (osapiMessageReceive(hapiTxPduQueue,
                            (void *)&pdu_msg,
                            sizeof(BROAD_TX_PDU_MSG_t),
                            L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOG_ERROR(0);
    }

    memset (&bcm_pkt, 0, sizeof (bcm_pkt_t));
    memset (&bcm_pkt_blk, 0, sizeof (bcm_pkt_blk_t));
    memset(&key, 0, sizeof(key));

    bcm_pkt.pkt_data  = &bcm_pkt_blk;
    bcm_pkt.blk_count = 1;
    bcm_pkt.call_back = L7_NULLPTR;  /*used for async responses which we do not use */
    bcm_pkt.pkt_data->data = pdu_msg.pkt_data;
    bcm_pkt.rx_untagged = pdu_msg.rx_untagged;
    bcm_pkt.pkt_data->len  = pdu_msg.user_data_size;

#ifndef PC_LINUX_HOST /* no CRC in simulation environment */
    bcm_pkt.flags = BCM_TX_CRC_APPEND;
#endif

    tunnel=pdu_msg.tunnel;
    hapiPort = pdu_msg.hapiPortPtr;
    user_data = bcm_pkt.pkt_data->data;
    frameLength = bcm_pkt.pkt_data->len;

   if (dot1adVlanInfoDebug)
    {
       SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Tx: Outer Vlanid: %d, inner Vlanid %d\n",
                     pdu_msg.vlanID,
                     pdu_msg.dot1adRemarkCVlanId);
   }

   svid = pdu_msg.vlanID;
      remarkCVID = pdu_msg.dot1adRemarkCVlanId;

   if(hapiPort->hapiDot1adintfCfg.intfType ==
      DOT1AD_INTFERFACE_TYPE_NNI)
   {
     isDownStreamFrame = L7_TRUE;
     if (dot1adVlanInfoDebug)
     {
       SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Tx: Down stream frame is getting tunneled\n");
     }
     key.SVID =  svid;
     key.CVID =  remarkCVID;
     memcpy (&key.usp,&pdu_msg.usp, sizeof(key.usp));

     pUpEntry = avlSearchLVL7(&hapiBroadDot1adSubupTree,
                              (void *)&key,
                               AVL_EXACT);
     if ( (pUpEntry != L7_NULLPTR) )
     {
       if (pUpEntry->serviceType == L7_DOT1AD_SVCTYPE_ETREE)
       {
         remarkCVID = pUpEntry->matchCvid;
       }
     }
   }
   if(pdu_msg.tunnel)  
   {

     {
        for (searchUsp.unit = 0; searchUsp.unit < 
                   dapi_g->system->totalNumOfUnits; searchUsp.unit++) 
        {
          for (searchUsp.slot = 0; searchUsp.slot < 
                       dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++) 
          {
            if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardId != L7_NULL) 
            {
              for (searchUsp.port = 0; 
                searchUsp.port < 
                dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; 
                searchUsp.port++) 
              {
                if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
                {
                  continue;
                }
                dapiPortPtr = DAPI_PORT_GET(&searchUsp, dapi_g);
                if ( (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) != L7_TRUE) &&
                     (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) != L7_TRUE)
                   )
      
                {
                  continue;
                }
                searchHapiPortPtr = HAPI_PORT_GET(&(searchUsp), dapi_g);

                if (hapiPort->bcmx_lport == searchHapiPortPtr->bcmx_lport)
                {
                 continue;
                }
               /* If the port is LAG member skip further processing on this port */
               if (searchHapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
               {
                 continue;
               }
                  lPort=searchHapiPortPtr->bcmx_lport;

               if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE) /* Adjust the lport */
               {
                 for (index = 0; index < L7_MAX_MEMBERS_PER_LAG; index++)
                 {
                   if (L7_TRUE == dapiPortPtr->modeparm.lag.memberSet[index].inUse)
                   {
                     break;
                   }
                 }
                 if (index == L7_MAX_MEMBERS_PER_LAG)
                 {
                   continue; 
                 }
                 tmpHapiPort = HAPI_PORT_GET(&(dapiPortPtr->modeparm.lag.memberSet[index].usp),
                                                  dapi_g);
                 lPort = tmpHapiPort->bcmx_lport;
              }


               if( searchHapiPortPtr->hapiDot1adintfCfg.intfType == 
                    DOT1AD_INTFERFACE_TYPE_NNI 
                  )
               {
               if (BROAD_IS_VLAN_MEMBER(&searchUsp, svid, dapi_g))
               {
                 *((L7_ushort16 *)(user_data+12))=osapiHtons(hapiBroadDvlanEthertype);
                 if (remarkCVID > 0)
                 {
                    memmove(user_data+20,user_data+16,pdu_msg.user_data_size-16);
                    pdu_msg.user_data_size +=4;
                    bcm_pkt.pkt_data->len = pdu_msg.user_data_size;
                    /* Inner TPID should be always DOT1Q */
                    *((L7_ushort16 *)(user_data+16))=osapiHtons(L7_ETYPE_8021Q);
                    *((L7_ushort16 *)(user_data+18))=osapiHtons(remarkCVID & 0x0FFF); 
                 }

                 if (dot1adVlanInfoDebug)
                 {
                   SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Tx:Getting tunneled on NNI:%d/%d/%d\n",
                                                    searchUsp.unit,
                                                    searchUsp.slot,
                                                    searchUsp.port);
                 }
                  
                  bcmx_tx_uc(&bcm_pkt, lPort, BCMX_TX_F_CPU_TUNNEL);
                  hapiPort->hapiDot1adIntfStats.numPduTunneled++;

                  if (remarkCVID > 0)
                  {
                    /* Restore back to SOT */
                     memmove(user_data+16,user_data+20,pdu_msg.user_data_size-20);
                     pdu_msg.user_data_size -=4;
                     bcm_pkt.pkt_data->len = pdu_msg.user_data_size;
                  }
                }
              }   
              else /* It's UNI based port. It can be UNI_P, UNI_S or UNI */
              {

                key.SVID =  svid;
                key.CVID =  remarkCVID;
                memcpy (&key.usp,&searchUsp, sizeof(key.usp));
           
                pUpEntry = avlSearchLVL7(&hapiBroadDot1adSubupTree,
                                         (void *)&key,
                                          AVL_EXACT);
                if (pUpEntry == L7_NULLPTR)
                {
                  key.SVID = svid;
                  key.CVID = 0;
                  pUpEntry = avlSearchLVL7(&hapiBroadDot1adSubupTree,
                                           (void *)&key,
                                            AVL_EXACT);
                  if (pUpEntry == L7_NULLPTR)
                  {
                     pUpEntry = avlSearchLVL7(&hapiBroadDot1adSubupTree,
                                             (void *)&key,
                                              AVL_NEXT);

                     if (pUpEntry != L7_NULLPTR)
                     {
                       if ( (pUpEntry->key.SVID != svid) ||
                            memcmp(&key.usp,&pUpEntry->key.usp,sizeof(key.usp) != 0) ||
                            ( (pUpEntry->key.CVID > 0) &&
                              (remarkCVID != pUpEntry->key.CVID)
                            )

                          )
                       {
                         continue;
                       }
                    }
                  }
                }

                if (pUpEntry != L7_NULLPTR)
                {
                  switch (pUpEntry->serviceType)
                  {
                    case L7_DOT1AD_SVCTYPE_TLS:
                    case L7_DOT1AD_SVCTYPE_ELAN:
                     sendFrame = L7_TRUE;
                    break;

                    case L7_DOT1AD_SVCTYPE_ELINE:
                    case L7_DOT1AD_SVCTYPE_ETREE:
                     if (isDownStreamFrame == L7_TRUE)
                     {
                       sendFrame = L7_TRUE;
                     }
                     else
                     {
                       sendFrame = L7_FALSE;
                     }
                     break;
                     
                     default:
                       sendFrame = L7_FALSE;
                      break;
                  }
                  if (sendFrame == L7_TRUE)
                  {
                    if (pUpEntry->matchSvid & 0x0FFF) 
                    {
                      *((L7_ushort16 *)(user_data+12))=osapiHtons(hapiBroadDvlanEthertype);
                      *((L7_ushort16 *)(user_data+14))=osapiHtons(pUpEntry->matchSvid & 0x0FFF);
                    }
                    if (pUpEntry->matchCvid & 0x0FFF)
                    {
                       memmove(user_data+20,user_data+16,pdu_msg.user_data_size-16);
                       pdu_msg.user_data_size +=4;
                       bcm_pkt.pkt_data->len = pdu_msg.user_data_size;
                       /* Inner TPID should be always DOT1Q */
                       *((L7_ushort16 *)(user_data+16)) = osapiHtons(L7_ETYPE_8021Q);
                       *((L7_ushort16 *)(user_data+18)) = osapiHtons(pUpEntry->matchCvid & 0x0FFF);
                    }
                    if ( (pUpEntry->matchCvid == 0) || 
                         ((pUpEntry->matchCvid & 0x0FFF) == 0)
                       )
                    {
                      if ( (searchHapiPortPtr->hapiDot1adintfCfg.intfType ==
                            DOT1AD_INTFERFACE_TYPE_UNI_S) &&
                            (remarkCVID > 0)
                         )
                      {
                        memmove(user_data+20,user_data+16,pdu_msg.user_data_size-16);
                        pdu_msg.user_data_size +=4;
                        bcm_pkt.pkt_data->len = pdu_msg.user_data_size;
                        /* Inner TPID should be always DOT1Q */
                        *((L7_ushort16 *)(user_data+16)) = osapiHtons(L7_ETYPE_8021Q);
                        *((L7_ushort16 *)(user_data+18)) = osapiHtons(remarkCVID);
                      }

                    }
                    if ( (pUpEntry->matchSvid == 0) || 
                         ((pUpEntry->matchSvid & 0x0FFF) == 0)
                       )
                    {
                        /* Remove the outer Info */
                       memmove(user_data+12,user_data+16,pdu_msg.user_data_size-16);
                       pdu_msg.user_data_size -=4;
                       bcm_pkt.pkt_data->len = pdu_msg.user_data_size;
                    }
                    bcmx_tx_uc(&bcm_pkt, lPort, BCMX_TX_F_CPU_TUNNEL);
                    hapiPort->hapiDot1adIntfStats.numPduTunneled++;
                    if (frameLength > pdu_msg.user_data_size)
                    {
                      if (dot1adVlanInfoDebug)
                      {
                        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Tx: UT Got tunneled on UNI:%d/%d/%d\n",
                                                    searchUsp.unit,
                                                    searchUsp.slot,
                                                    searchUsp.port);
                      }
                       /* Untagged frame went out, So adjust back the PDU */
                       memmove(user_data+16,user_data+12,pdu_msg.user_data_size-12);
                       pdu_msg.user_data_size +=4;
                       bcm_pkt.pkt_data->len = pdu_msg.user_data_size;
                    }
                    else if (frameLength < pdu_msg.user_data_size)
                    {
                      if (dot1adVlanInfoDebug)
                      {
                        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Tx: DT Got tunneled on UNI:%d/%d/%d\n",
                                                    searchUsp.unit,
                                                    searchUsp.slot,
                                                    searchUsp.port);
                      }
                      /* DT frame went out, So adjust back the PDU */
                       memmove(user_data+16,user_data+20,pdu_msg.user_data_size-20);
                      pdu_msg.user_data_size -=4;
                      bcm_pkt.pkt_data->len = pdu_msg.user_data_size;
                    }
                    else
                    {
                      if (dot1adVlanInfoDebug)
                      {
                        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Tx: SIT Got tunneled on UNI:%d/%d/%d\n",
                                                    searchUsp.unit,
                                                    searchUsp.slot,
                                                    searchUsp.port);
                      }
                    }
                      /* In any case, Restore back to the Normal */
                    *((L7_ushort16 *)(user_data+12))=osapiHtons(hapiBroadDvlanEthertype);
                    *((L7_ushort16 *)(user_data+14))=osapiHtons(svid & 0x0FFF);
                 }
               } 
             }
              sendFrame = L7_FALSE;
           }
         }
        }
      }
      hapiPort->hapiDot1adIntfStats.numPduTunneled++;
      }
    }
    sal_dma_free(bcm_pkt.pkt_data->data);
  }
}

L7_RC_t hapiBroadRxProtoSnoopModify(BROAD_PKT_RX_MSG_t *pktRxMsg,DAPI_t *dapi_g)
{
  L7_char8        *pEtype;
  L7_uchar8        ip_ethtype[]  = {0x08, 0x00};
  L7_ipHeader_t   *ip_header;
  BROAD_PORT_t    *hapiPortPtr = L7_NULLPTR;
  L7_uchar8       *user_data;
  L7_netBufHandle  frameHdl = L7_NULL;
  L7_ushort16 SVID=0, cvid = L7_NULL;
  L7_ushort16 remarkCVID=0,numIntf,tlsRemarkCVID=(L7_DOT1AD_CVID_MAX + 1000);
  L7_uint32        offset, frameLen;
  BROAD_DOT1AD_SUBDOWN_INFO_t  infoList[L7_MAX_INTERFACE_COUNT];

  memset((void *)infoList, 0, sizeof(infoList));

  hapiPortPtr = HAPI_PORT_GET(&pktRxMsg->usp, dapi_g);

  /* Take the first Active member information to 
     get the configured rules on the LAG. 
     Otherwise snooping on LAG will not work.
  */

  if (hapiPortPtr->port_is_lag) /* Take the first Active member information */
  {
    DAPI_PORT_t                *dapiPortPtr;
    int               i;
    DAPI_LAG_ENTRY_t *lagMemberSet;

    dapiPortPtr = DAPI_PORT_GET(&pktRxMsg->usp, dapi_g);
    lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

    /* Find the first Active LAG member */
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (L7_TRUE == lagMemberSet[i].inUse)
       {
         hapiPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);
         break;
       }
     }
   }

  /* If dvlan is not enabled, return without modifying the packet */
  if (dapi_g->system->dvlanEnable != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  frameHdl  = pktRxMsg->cmdInfo.cmdData.receive.frameHdl;
  user_data = sysapiNetMbufGetDataStart (frameHdl);
  offset    = sysNetDataOffsetGet(user_data);

  ip_header = (L7_ipHeader_t *)(user_data + offset);
  pEtype = &user_data[16];

  if (memcmp(pEtype, ip_ethtype, sizeof(ip_ethtype)) == 0)
  {
    switch (ip_header->iph_prot)
    {
      case IP_PROT_IGMP:

        if(hapiPortPtr->hapiDot1adintfCfg.intfType == DOT1AD_INTFERFACE_TYPE_NNI)
        {
          if((L7_ushort16)pktRxMsg->cmdInfo.cmdData.receive.innerVlanId == hapiPortPtr->pvid)
          {
            remarkCVID = 0;
          }
          else
          {
            remarkCVID = pktRxMsg->cmdInfo.cmdData.receive.innerVlanId;
          }

           if(hapiBroadDot1adDownstreamInfoGet(pktRxMsg->cmdInfo.cmdData.receive.vlanID,
                                        (L7_ushort16)remarkCVID,
                                        infoList,
                                        &numIntf) != L7_SUCCESS)
           {
             return L7_FAILURE;
           }

           if (numIntf == 0)
           {
             if (remarkCVID == 0)
             {
                if (hapiBroadDot1adDownstreamInfoGet(pktRxMsg->cmdInfo.cmdData.receive.vlanID,
                                          hapiPortPtr->pvid,
                                          infoList,
                                          &numIntf) != L7_SUCCESS)
                {
                  return L7_FAILURE;
                }

                if (numIntf == 0)
                  infoList[0].CVID = 0;
                else
                infoList[0].CVID = hapiPortPtr->pvid;
             }
             else
             {
               if (hapiBroadDot1adDownstreamInfoGet(pktRxMsg->cmdInfo.cmdData.receive.vlanID,
                  (L7_ushort16)tlsRemarkCVID,
                   infoList,
                   &numIntf) == L7_SUCCESS)
               { 
                 if (!numIntf) /* NON-TLS serviece */
                 {
                   return L7_FAILURE;              
                 }
                 else /* TLS serviece */
                 {
                   infoList[0].CVID = pktRxMsg->cmdInfo.cmdData.receive.innerVlanId;
                 }
               }
                /* This case should not happen */
               infoList[0].CVID = (L7_ushort16)pktRxMsg->cmdInfo.cmdData.receive.innerVlanId;
             }
           }
           else
           {
              infoList[0].CVID = remarkCVID;
           }
           
           if(infoList[0].CVID == 0)
           {
             frameLen = sysapiNetMbufGetDataLength(frameHdl);
             memmove((user_data + 12), (user_data + 16),(frameLen - 16));
             frameLen = frameLen - 4;
             sysapiNetMbufSetDataLength(frameHdl,frameLen);
             pktRxMsg->cmdInfo.cmdData.receive.innerVlanId = 0;
           }
           else
           {
             memcpy(&user_data[14],&infoList[0].CVID,sizeof(L7_ushort16));
             pktRxMsg->cmdInfo.cmdData.receive.innerVlanId = infoList[0].CVID;
           }
          return L7_SUCCESS;
        }
        else if(hapiPortPtr->hapiDot1adintfCfg.intfType != DOT1AD_INTFERFACE_TYPE_SWITCHPORT)
        {
          cvid = (L7_ushort16)pktRxMsg->cmdInfo.cmdData.receive.innerVlanId;
          if (hapiPortPtr->pvid == cvid)
          {
            cvid = L7_NULL;
          }
          if(hapiBroadDot1adUpstreamInfoGet((bcmx_lport_t)hapiPortPtr->bcmx_lport,
                                  cvid,  
                                  (L7_ushort16 *)&SVID,
                                  (L7_ushort16 *)&remarkCVID)!=L7_SUCCESS)
          {
            if (cvid == L7_NULL)
            {
              if (hapiBroadDot1adUpstreamInfoGet((bcmx_lport_t)hapiPortPtr->bcmx_lport,
                                  hapiPortPtr->pvid,
                                  (L7_ushort16 *)&SVID,
                                  (L7_ushort16 *)&remarkCVID)!=L7_SUCCESS)
              {
                return L7_FAILURE;
              }
            }
            else 
            {
            return L7_FAILURE;
          }
          }
    
          if(remarkCVID==0)
          {
            pktRxMsg->rx_untagged = L7_TRUE;
            frameLen = sysapiNetMbufGetDataLength(frameHdl);
            memmove((user_data + 12), (user_data + 16),(frameLen - 16));
            frameLen = frameLen - 4;
            sysapiNetMbufSetDataLength(frameHdl,frameLen);
            pktRxMsg->cmdInfo.cmdData.receive.innerVlanId = remarkCVID;
          }
          else
          {
            L7_ushort16 shortVal;
            shortVal = (user_data[14] & 0xF0) << 8;

            shortVal |= (0x0FFF & remarkCVID);

            memcpy(&user_data[14],&shortVal,sizeof(L7_ushort16));
            pktRxMsg->cmdInfo.cmdData.receive.innerVlanId = remarkCVID;
          }
          pktRxMsg->cmdInfo.cmdData.receive.vlanID = SVID;
        }
                                      
        break;
      default:
        return L7_SUCCESS;
    }
  }
  return L7_SUCCESS;
}
#endif
#endif
