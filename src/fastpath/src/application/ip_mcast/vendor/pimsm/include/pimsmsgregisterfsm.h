/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmSGRegisterFSM.h
*
* @purpose Contains PIM-SM per (S,G) register State Machine 
* declarations
*
* @component 
*
* @comments 
*
* @create 08/18/2005
*
* @author Muralidhar Vakkalagadda
* @end
*
**********************************************************************/
#ifndef _PIMSM_SG_REGISTER_FSM_H_
#define _PIMSM_SG_REGISTER_FSM_H_
/*#include "pimsmsgtree.h"*/
/*#include "pimsmmain.h"*/

struct pimsmCB_s;
typedef enum
{
  PIMSM_REG_PER_S_G_SM_STATE_NO_INFO = 0,
  PIMSM_REG_PER_S_G_SM_STATE_JOIN,
  PIMSM_REG_PER_S_G_SM_STATE_JOIN_PENDING,
  PIMSM_REG_PER_S_G_SM_STATE_PRUNE,
  PIMSM_REG_PER_S_G_SM_STATE_MAX
} pimsmRegPerSGStates_t;

typedef enum
{
  PIMSM_REG_PER_S_G_SM_EVENT_REG_STOP_TIMER_EXPIRES = 0,
  PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_TRUE,
  PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_FALSE,
  PIMSM_REG_PER_S_G_SM_EVENT_RECV_REG_STOP,
  PIMSM_REG_PER_S_G_SM_EVENT_RP_CHANGED,
  PIMSM_REG_PER_S_G_SM_EVENT_MAX
} pimsmRegPerSGEvents_t;

typedef struct pimsmPerSGRegisterEventInfo_s 
{
  pimsmRegPerSGEvents_t    eventType;
}pimsmPerSGRegisterEventInfo_t;

struct pimsmSGNode_s;

extern L7_RC_t pimsmPerSGRegisterExecute (struct pimsmCB_s * pimsmCb, 
                                          struct pimsmSGNode_s * pimsmDnStrmSGNode, 
                                          pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);                   

#endif /* _PIMSM_SG_REGISTER_FSM_H_ */
