/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot1s_helper.c
* @purpose     Helper module for spanning tree component
* @component   dot1s
* @comments    none
* @create      01/7/09
* @author      akulkarn
* @end
*             
**********************************************************************/

#ifndef __DOT1S_HELPER_H__
#define __DOT1S_HELPER_H__

#include "nimapi.h"
#include "dot1s_txrx.h"
#include "ms_api.h"

typedef enum
{
  DOT1S_HELPER_PDU_START = 0,
  DOT1S_HELPER_PDU_STOP,
  DOT1S_HELPER_PDU_MODIFY,
  DOT1S_HELPER_TIMER_EXPIRE,
 /* The control message beyond this msg type
    These messages are not based on port. The port based precede ctl
  */
  DOT1S_HELPER_CONTROL_START, 
  DOT1S_HELPER_SWITCHOVER_START,
  DOT1S_HELPER_SWITCHOVER_DONE,
  DOT1S_HELPER_PURGE,
  DOT1S_HELPER_CONTROL_END, 
  DOT1S_HELPER_MSG_TYPE_LAST
} dot1s_helper_msgType_t;

typedef struct
{
  L7_uint32      ctrlMsgType;
} dot1s_helper_ctrl_data_t;

typedef struct 
{
  msSyncMsgHdr_t msHeader;
  dot1s_helper_ctrl_data_t ctrlData;
}dot1s_helper_ctrl_msg_t;

typedef struct dot1s_helper_msg_s
{
  L7_uint32 intIfNum;
  L7_uint32 interval;
  nimUSP_t  usp;
  dot1s_helper_msgType_t msgType;
  L7_uint32 pduLen;
  DOT1S_BYTE_MSTP_ENCAPS_t pdu;
} dot1s_helper_msg_t;

typedef struct 
{
  msSyncMsgHdr_t msHeader;
  dot1s_helper_msg_t helperData;
}dot1s_helper_data_msg_t;

#define DOT1S_HELPER_PRIO (L7_DOT1P_NUM_TRAFFIC_CLASSES - 1)
#define DOT1S_HELPER_MAX_PORTS (L7_MAX_PHYSICAL_PORTS_PER_UNIT + L7_MAX_NUM_LAG_INTF + 1)
extern L7_uint32 dot1sUserHandle;
#define DOT1S_HELPER_CTL_MSG_LEN (sizeof(dot1s_helper_ctrl_msg_t))
#define DOT1S_HELPER_IS_CTL_MSG(_msgType) (((_msgType) > DOT1S_HELPER_CONTROL_START) && \
                                             ((_msgType) < DOT1S_HELPER_CONTROL_END)) 
/*********************************************************************
* @purpose  Allocate resources required by the dot1s helper subsystem
*
* @param    None 
*
* @returns  L7_SUCCESS if succesful
*           L7_NO_MEMORY if cannot allocate memory
*           L7_FAILURE   for any other error.
*
* @notes   
*       
* @end
*********************************************************************/
L7_RC_t dot1sHelperInit();

/*********************************************************************
* @purpose  Return resources allocated by the dot1s helper subsystem
*
* @param    None 
*
* @returns  void
*
* @notes   
*       
* @end
*********************************************************************/
L7_RC_t dot1sHelperFini();

L7_RC_t dot1sHelperCallbacksRegister();
/*********************************************************************
* @purpose  dot1s helper task which serves the local unit
*
* @param    
*
* @returns  void
*
* @notes    This task upon receiving notification from core tx task, 
*           stores the pdu locally and transmits them periodically 
*           until told otherwise
*       
* @end
*********************************************************************/
void dot1sHelperTask();

/*********************************************************************
* @purpose  Put a message on the helper queue
*
* @param    msg @b{(input)} the msg to be put on the queue 
*
* @returns  L7_FAILURE if cannot put message on the queue
*           L7_SUCCESS otherwise
*
* @notes   
*       
* @end
*********************************************************************/
L7_RC_t dot1sHelperIssueCmd(void *msg, L7_BOOL controlMsg);

/*********************************************************************
* @purpose  Callback registered with the helper timer utility
*
* @param    intIfNum @b{(input)} the interface num on which the timer has expired 
*
* @returns  void
*
* @notes   This callback is processed on the timer thread. So cannot take semaphore 
*          and generally want to minimize the processing done on this thread.
*       
* @end
*********************************************************************/
void dot1sHelperTimerCallback(L7_uint32 portIndex);

L7_BOOL dot1sIsPortLag(nimUSP_t *usp);
#endif /*__DOT1S_HELPER_H__*/
