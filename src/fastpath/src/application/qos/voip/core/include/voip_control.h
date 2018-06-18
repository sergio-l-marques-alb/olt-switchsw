/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip.h
*
* @purpose VOIP definitions
*
* @component VOIP
*
* @comments none
*
* @create 05/10/2007
*
* @author parora
* @end
*
**********************************************************************/
#ifndef VOIP_CONTROL_H
#define VOIP_CONTROL_H

#include "nimapi.h"
#include "defaultconfig.h"
#include "avl_api.h"
#include "sysnet_api.h"
#include "l7_product.h"
#include "comm_mask.h"


typedef struct voipIntfChangeParms_s
{
  L7_uint32             intIfNum;
  L7_uint32 event;
  NIM_CORRELATOR_t correlator;
} voipIntfChangeParms_t;

typedef struct voipStartupNotifyParms_s
{
  NIM_STARTUP_PHASE_t startupPhase;
} voipStartupNotifyParms_t;

typedef enum
{
  voipMsgIntfChange = 1,
  voipMsgStartupNotify,
  voipMsgPduReceive,
  voipMsgTimer
}voipMsgTypes_t;

typedef struct
{
  L7_uint32       intIfNum;
  L7_uint32       protocol;
  L7_uint32       dataLength;
  L7_uchar8       *voipBuf;
} voipPduReceiveParms_t;

typedef struct
{
  L7_uint32             msgId;
  union
  { 
    voipIntfChangeParms_t    voipIntfChangeParms;
    voipStartupNotifyParms_t voipStartupNotifyParms;
    voipPduReceiveParms_t    voipPduReceiveParms;
  }u;
} voipMgmtMsg_t;

#define VOIP_QUEUE           "voip_Queue"
#define VOIP_MSG_COUNT       1024
#define VOIP_MSG_SIZE        sizeof(voipMgmtMsg_t)
#define VOIP_TIMER_TICK      5*60*1000 /*in milliseconds*/


/*********************************************************************
* @purpose  Take the semaphore for the VOIP DB
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void voipDbSemaTake();

/*********************************************************************
* @purpose  Give the semaphore for the VOIP DB
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void voipDbSemaGive();

/*********************************************************************
*
* @purpose task to handle all VOIP messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void voipTask();

/*********************************************************************
* @purpose  Start VOIP TASk
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipStartTask();

/*****************************************************************************
* @purpose  Callback function to Copy and process VOIP control frames
*
* @param    hookId        @b{(input)} The hook location
* @param    bufHandle     @b{(input)} Handle to the frame to be processed
* @param    pduInfo       @b{(input)} Pointer to info about this frame
* @param    continueFunc  @b{(input)} Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  If frame has been consumed;
* @returns  SYSNET_PDU_RC_IGNORED  If frame has been ignored
*                                  continue processing it
*
* @notes    none
*
* @end
****************************************************************************/
SYSNET_PDU_RC_t voipPktIntercept(L7_uint32 hookId,
                                 L7_netBufHandle bufHandle,
                                 sysnet_pdu_info_t *pduInfo,
                                 L7_FUNCPTR_t continueFunc);
/*********************************************************************
*
* @purpose  Maintain one timer resource for the component
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void voipTimerAdd(void);
/*********************************************************************
*
* @purpose  Maintain one timer resource for the component
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void voipTimerDelete(void);
/*********************************************************************
* @purpose  voipSipTimerAction
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes i) Get the total number of packets belongs to the each call and
*           compare with previous count.End the call if the previoues
*           and current count is equal
*
*
* @end
*********************************************************************/
L7_RC_t voipSipTimerAction();
/*********************************************************************
* @purpose  voipSccpTimerAction
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes i) Get the total number of packets belongs to the each call and
*           compare with previous count.End the call if the previoues
*           and current count is equal
*
*
* @end
*********************************************************************/
L7_RC_t voipSccpTimerAction();
/*********************************************************************
* @purpose  voipH323TimerAction
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes i) Get the total number of packets belongs to the each call and
*           compare with previous count.End the call if the previoues
*           and current count is equal
*
*
* @end
*********************************************************************/
L7_RC_t voipH323TimerAction();
#endif
