/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pppoe_util.h
*
* @purpose  PPPoE Intermediate Agent
*
* @component  PPPoE Intermediate Agent
*
* @comments 
*
* @create 14/05/2013
*
* @author Daniel Figueira
* @end
*
**********************************************************************/

#ifndef DS_UTIL_H
#define DS_UTIL_H


#include "dot1q_api.h"
#include "cli_txt_cfg_api.h"
#include "avl_api.h"
#include "l7_packet.h"
#include "osapi_support.h"
#include "l7_cnfgr_api.h"


#define PPPOE_PACKET_SIZE_MAX  1518

typedef struct
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];  /*Client MAC Address*/
  L7_uint16 rootVlan;                     /*Outer Vlan*/
} ptinPppoeClientDataKey_t;

typedef struct
{
  ptinPppoeClientDataKey_t   dataKey;
  L7_uint32                  interface;   /*Interface throught which the client request arrived*/
  L7_uint16                  inner_vlan;  /* Internal inner vlan */

  void *next;
} ptinPppoeBindingInfoData_t;

typedef struct {
    avlTree_t                  avlTree;
    avlTreeTables_t            *treeHeap;
    ptinPppoeBindingInfoData_t *dataHeap;
} ptinPppoeBindingAvlTree_t;
ptinPppoeBindingAvlTree_t pppoeBindingTable;

#define PPPOE_PACKET_SIZE_MAX  1518
typedef enum 
{
    PPPOE_CNFGR_INIT = 0
} pppoeMessages_t;


typedef struct pppoeFrameMsg_s
{
  /* Receive interface. Internal interface number. */
  L7_uint32 rxIntf;

  /* VLAN on which message was received */
  L7_ushort16 vlanId;

  /* Inner VLAN on which DHCP packet was received,
     if the received packet is double-tagged. */
  L7_ushort16 innerVlanId;

  /* Entire ethernet frame. Don't want to hold mbuf on msg queue. */
  L7_uchar8 frameBuf[PPPOE_PACKET_SIZE_MAX];

  /* Frame Len rx'ed fron sysnet.*/
  L7_uint32 frameLen;

  /* PTin added: DHCP snooping */
  L7_uint   client_idx;

} pppoeFrameMsg_t;


typedef struct
{
  pppoeMessages_t msgType;

  union
  {
    L7_CNFGR_CMD_DATA_t cmdData;
  } pppoeMsgData;

} pppoeEventMsg_t;


/* pppoe_main.c */
void pppoeTask(void);

L7_RC_t pppoePduReceive(L7_netBufHandle bufHandle,
                        sysnet_pdu_info_t *pduInfo);
L7_RC_t pppoePacketQueue(L7_uchar8 *ethHeader, L7_uint32 dataLen,
                         L7_ushort16 vlanId, L7_uint32 intIfNum,
                         L7_ushort16 innerVlanId, L7_uint *client_idx);
L7_RC_t pppoeFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
                          L7_uchar8 *frame, L7_uint32 frameLen,
                          L7_ushort16 innerVlanId, L7_uint client_idx);

#endif
