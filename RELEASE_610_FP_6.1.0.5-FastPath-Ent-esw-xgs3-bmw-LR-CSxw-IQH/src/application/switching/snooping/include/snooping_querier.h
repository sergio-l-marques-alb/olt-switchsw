/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename snooping_querier.h
*
* @purpose Contains prototypes of snooping querier apis
*
* @component
*
* @comments
*
* @create 06/16/2003
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SNOOPING_QUERIER_H
#define SNOOPING_QUERIER_H

#include "l7_packet.h"
#include "l3_addrdefs.h"

typedef enum
{
  SNOOP_QUERIER_QUERY_INTERVAL_TIMER = 0,
  SNOOP_QUERIER_QUERIER_EXPIRY_TIMER
} snoopQuerierTimerType_t;

typedef enum
{
  SNOOP_PERIODIC_QUERY = 0,
  SNOOP_LEAVE_PROCESS_QUERY,
  SNOOP_TCN_QUERY
} snoopQueryType_t;


typedef enum snoopQuerierState_s
{
  SNOOP_QUERIER_DISABLED = 0, /*S0*/
  SNOOP_QUERIER_QUERIER,      /*S1*/
  SNOOP_QUERIER_NON_QUERIER,  /*S2*/

  SNOOP_QUERIER_STATES
} snoopQuerierState_t;

typedef enum snoopQuerierSTEvents_s
{
   snoopQuerierBegin = 0,              /* E0 */
   snoopQuerierRxSuperiorQryMoveToNQ,  /* E1 */
   snoopQuerierRxInferiorQryMoveToQ,   /* E2 */
   snoopQuerierVersionConflict,        /* E3 */
   snoopQuerierDisable,                /* E4 */
   snoopQuerierTimerExpiry,            /* E5 */
   snoopQuerierQueryTimerExpiry,       /* E6 */
   snoopQuerierBeginNQ,               /* E7 */

   snoopQuerierSTEvents
} snoopQuerierSTEvents_t;


struct snoop_cb_s;
struct snoopOperData_s;
struct snoopMgmtMsg_s;
struct L7_mgmdQueryMsg_s;
struct mgmdSnoopControlPkt_s;

/*********************************************************************
  Event handling routines
*********************************************************************/
void snoopQuerierSMProcessEvent(struct snoopOperData_s *pSnoopOperEntry,
                                snoopQuerierSTEvents_t event,
                                struct snoop_cb_s *pSnoopCB);
void snoopQuerierStart(struct snoopOperData_s *pSnoopOperEntry, 
                       struct snoop_cb_s *pSnoopCB);
void snoopQuerierProcessMessage(struct snoopMgmtMsg_s *msg);
void snoopQuerierVlanAdd(L7_uint32 vlanId);
void snoopQuerierVlanDelete(L7_uint32 vlanId);

/*********************************************************************
  Operational Changes apply Routines
*********************************************************************/
L7_RC_t snoopQuerierAdminModeApply(L7_uint32 mode, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopQuerierVlanModeApply(L7_uint32 vlanId, L7_uint32 mode,
                                  struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopQuerierVlanElectionParticipateModeApply(L7_uint32 vlanId, 
                                                     L7_uint32 mode,
                                                     struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopQuerierAddressApply(struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopQuerierVersionApply(struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopQuerierVlanAddressApply(L7_uint32 vlanId, struct snoop_cb_s *pSnoopCB);
L7_RC_t snoopQuerierNetworkAddressApply(struct snoop_cb_s *pSnoopCB);
/*********************************************************************
  Helper rotuines
*********************************************************************/
L7_BOOL snoopQuerierAddressReady(L7_uint32 vlanId, struct snoop_cb_s *pSnoopCB, 
                                 L7_inet_addr_t *querierAddr);
L7_BOOL snoopQuerierVlanReady(L7_uint32 vlanId, struct snoop_cb_s *pSnoopCB);
/*********************************************************************
  PDU Send Routines
*********************************************************************/
L7_RC_t snoopQuerierQueryProcess(struct mgmdSnoopControlPkt_s *mcastPacket);
L7_RC_t snoopQuerierOtherQuerierDetected(struct snoopOperData_s  *pSnoopOperEntry,
                                         struct mgmdSnoopControlPkt_s *mcastPacket,
                                         struct L7_mgmdQueryMsg_s *mgmdMsg,
                                         L7_uint32 incomingVersion);
L7_RC_t snoopIGMPFrameBuild(L7_uint32           intIfNum,
                            L7_inet_addr_t     *destIp,
                            L7_uchar8           type,
                            L7_inet_addr_t     *groupAddr,
                            L7_uchar8          *buffer,
                            struct snoop_cb_s  *pSnoopCB,
                            L7_uint32           version,
                            struct snoopOperData_s *pSnoopOperEntry);
L7_RC_t snoopMLDFrameBuild(L7_uint32                intIfNum,
                            L7_inet_addr_t         *destIp,
                            L7_uchar8               type,
                            L7_inet_addr_t         *groupAddr,
                            L7_uchar8              *buffer,
                            struct snoop_cb_s      *pSnoopCB,
                            L7_uint32               version,
                            struct snoopOperData_s *pSnoopOperEntry);
L7_RC_t snoopGeneralQuerySend(L7_uint32 intIfNum, L7_uint32 vlanId,
                              struct snoop_cb_s *pSnoopCB, snoopQueryType_t qryType);
void snoopQuerierPeriodicQuerySend(struct snoopOperData_s *pSnoopOperEntry);

/*********************************************************************
  Querier Timer Routines
*********************************************************************/
L7_RC_t snoopQuerierTimerStart(struct snoopOperData_s *snoopOperEntry, 
                               snoopQuerierTimerType_t timerType,
                               L7_uint32 interval);
L7_RC_t snoopQuerierTimerUpdate(struct snoopOperData_s *snoopOperEntry, 
                                snoopQuerierTimerType_t timerType,
                                L7_uint32 interval);
L7_RC_t snoopQuerierTimerStop(struct snoopOperData_s *snoopOperEntry, 
                              snoopQuerierTimerType_t timerType);
void snoopQuerierExpiry(void *param);
void snoopQuerierQueryExpiry(void *param);

#endif /* SNOOPING_QUERIER_H */
