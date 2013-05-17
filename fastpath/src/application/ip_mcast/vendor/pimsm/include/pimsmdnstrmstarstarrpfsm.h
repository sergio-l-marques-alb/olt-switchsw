/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmDNStrmStarStarRpFSM.h
*
* @purpose Contains PIM-SM Down stream per interface State Machine 
* declarations for (*,*,RP) entry.
*
* @component    pimsm 
*
* @comments 
*
* @create 01/01/2006
*
* @author dsatyanarayana / vmurali
* @end
*
***********************************************************************/
#ifndef _PIMSM_DNSTRM_PER_INTF_STAR_STAR_RP_FSM_H_
#define _PIMSM_DNSTRM_PER_INTF_STAR_STAR_RP_FSM_H_
/*#include "pimsmmain.h"*/

struct pimsmCB_s;

typedef enum
{
  PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO = 0,
  PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_JOIN,
  PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_PRUNE_PENDING,
  PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_MAX
} pimsmDnStrmPerIntfStarStarRPStates_t;

typedef enum
{
  PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_RECV_JOIN = 0,
  PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_RECV_PRUNE,
  /* NOTE: These events will be generated when the entry 
   * timer and prune pending timer expires 
   */
  PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES,
  PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_EXPIRY_TIMER_EXPIRES,
  PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_MAX
} pimsmDnStrmPerIntfStarStarRPEvents_t;


typedef struct pimsmDnStrmPerIntfStarStarRPEventInfo_s
{
  pimsmDnStrmPerIntfStarStarRPEvents_t    eventType;
  L7_uint32 rtrIfNum;
  L7_uint32 holdtime;
}pimsmDnStrmPerIntfStarStarRPEventInfo_t;

struct pimsmStarStarRpNode_s;

extern L7_RC_t pimsmDnStrmPerIntfStarStarRPExecute 
(struct pimsmCB_s * pimsmCb,
 struct pimsmStarStarRpNode_s  *pStarStarRpNode, 
 pimsmDnStrmPerIntfStarStarRPEventInfo_t  *pDnStrmPerIntfStarStarRpEventInfo);

#endif /* _PIMSM_DNSTRM_PER_INTF_STAR_STAR_RP_FSM_H_ */
