/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmIntf.h
*
* @purpose  All interface related APIs/operations.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author vmurali
* @end
*
**********************************************************************/
#ifndef _PIMSM_INTF_H_
#define _PIMSM_INTF_H_
#include "l7sll_api.h"
#include "l3_addrdefs.h"
#include "l7apptimer_api.h"
#include "pimsmneighbor.h"
#include "pimsmtimer.h"

/* packet send/recv mode */
typedef enum
{
  PIMSM_RX = 1,
  PIMSM_TX,
  PIMSM_INVALID
} pimsmPktRxTxMode_t;

/* if incoming is PIMSM_REGISTER_INTF_NUM then we have to expect
   an encapulated packet as we are RP. if out-going list has
   PIMSM_REGISTER_INTF_NUM then we need to send encapulated packet if we are DR.
 */
#define   PIMSM_REGISTER_INTF_NUM               L7_PIMSM_REGISTER_INTF

typedef struct pimsmPerIntfStats_s
{
  L7_uint32 pimsmHelloPktRx;
  L7_uint32 pimsmHelloPktTx;
  L7_uint32 pimsmRegisterPktRx;
  L7_uint32 pimsmRegisterPktTx;
  L7_uint32 pimsmRegisterStopPktRx;
  L7_uint32 pimsmRegisterStopPktTx;
  L7_uint32 pimsmJoinPrunePktRx;
  L7_uint32 pimsmJoinPrunePktTx;
  L7_uint32 pimsmBsrPktRx;
  L7_uint32 pimsmBsrPktTx;
  L7_uint32 pimsmCandRpPktRx;
  L7_uint32 pimsmCandRpPktTx;
  L7_uint32 pimsmAssertPktRx;
  L7_uint32 pimsmAssertPktTx;
  L7_uint32 pimsmInvalidPktRx;
  L7_uint32 pimsmNoCacheRx;
  L7_uint32 pimsmWrongiifRx;
}pimsmPerIntfStats_t;

typedef struct pimsmInterfaceEntry_s
{
  L7_uint32   pimsmInterfaceIfIndex;
  L7_uint32   intIfNum;
  L7_inet_addr_t  pimsmInterfaceDR;
  L7_uint32     pimsmInterfaceDRPriority; 
  L7_BOOL       pimsmIamDR;
  L7_inet_addr_t  pimsmInterfaceAddr;
  pimsmPerIntfStats_t pimsmPerIntfStats;
  L7_uint32     pimsmInterfaceGenerationIDValue;    
  L7_uint32     pimsmInterfaceHelloInterval;
  L7_uint32     pimsmInterfaceJoinPruneInterval; 
  L7_uint32     pimsmInterfaceHelloHoldtime;
  L7_uint32     pimsmInterfaceJoinPruneHoldtime;
  L7_sll_t        pimsmNbrList;
  L7_uint32     pimsmNbrCount;
  L7_BOOL       pimsmLANPruneDelayPresent;
  L7_uint32     pimsmPropagationDelay;
  L7_uint32     pimsmOverrideInterval;
  L7_APP_TMR_HNDL_t  pimsmHelloTimer;
  pimsmTimerData_t   pimsmHelloTimerParams;
  L7_uint32  pimsmHelloTimerHandle;   
}pimsmInterfaceEntry_t;


struct pimsmNeighborEntry_s;
struct pimsmCB_s;

extern L7_RC_t pimsmIntfInit(struct pimsmCB_s * pimsmCb);
extern L7_RC_t pimsmIntfDeInit(struct pimsmCB_s * pimsmCb);
extern L7_RC_t pimsmIntfDown(struct pimsmCB_s * pimsmCb, L7_uint32 rtrIfNum);
extern L7_RC_t pimsmIntfUp(struct pimsmCB_s * pimsmCb, L7_uint32 rtrIfNum,
            L7_uint32 intIfNum, L7_inet_addr_t *intfAddr);

extern L7_BOOL pimsmIAmDR(struct pimsmCB_s * pimsmCb, L7_uint32 rtrIfNum);

extern L7_RC_t pimsmIntfEntryGet(struct pimsmCB_s * pimsmCb, 
                                 L7_uint32 rtrIfNum, pimsmInterfaceEntry_t **intfEntry);

extern L7_RC_t pimsmIntfNbrEntryGet(struct pimsmCB_s *pimsmCb,
                                    L7_uint32 rtrIfNum,
                                    L7_inet_addr_t *ipAddress,
                                    pimsmNeighborEntry_t **nbrEntry);

extern L7_RC_t pimsmIntfDRUpdate(struct pimsmCB_s  *pimsmCb,
                                 pimsmInterfaceEntry_t   *intfEntry);

extern void pimsmStatsIncrement(struct pimsmCB_s *pimsmCb, L7_uint32 rtrIfNum, 
                                L7_uchar8 pktType, pimsmPktRxTxMode_t RxOrTx);

extern L7_RC_t pimsmProcessDRChange(struct pimsmCB_s * pimsmCb, L7_uint32 rtrIfNum);
extern L7_RC_t pimsmIntfPurge(struct pimsmCB_s  *pimsmCb);

extern L7_RC_t pimsmInterfaceJoinPruneHoldtimeGet(struct pimsmCB_s *pimsmCb,  
                                                  L7_uint32 rtrIfNum, L7_ushort16 *pHoldtime);
extern L7_RC_t pimsmInterfaceJoinPruneIntervalGet(struct pimsmCB_s *pimsmCb, L7_uint32 rtrIfNum,
                                                  L7_uint32 *pInterval);
extern L7_RC_t pimsmInterfaceHelloHoldtimeGet(struct pimsmCB_s *pimsmCb,  
                                              L7_uint32 rtrIfNum, L7_ushort16 *pHoldtime);
extern L7_RC_t pimsmInterfaceHelloIntervalGet(struct pimsmCB_s *pimsmCb, L7_uint32 rtrIfNum,
                                                  L7_uint32 *pInterval);

#endif /* _PIMSM_INTF_H_ */



