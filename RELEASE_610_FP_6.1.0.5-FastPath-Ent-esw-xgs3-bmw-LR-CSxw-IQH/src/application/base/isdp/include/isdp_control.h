/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_control.h
*
* @purpose   isdp Control header
*
* @component isdp
*
* @comments
*
* @create    12/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/
#ifndef ISDP_CONTROL_H
#define ISDP_CONTROL_H

#include "isdp_txrx.h"

#define ISDP_BASE_TIMEOUT (1000) /*in milliseconds*/

typedef enum 
{
  ISDP_PDU_EVENT = 1,
  ISDP_TIMER_EVENT,
  ISDP_INTF_EVENT,
  ISDP_CNFGR_EVENT,
  ISDP_LAST_EVENT
}isdpMgmtEvents_t;

typedef struct isdpMsg_s
{
  isdpMgmtEvents_t event;
  L7_uint32 intIfNum;
  
  union
  {
    L7_CNFGR_CMD_DATA_t       CmdData;
    NIM_EVENT_COMPLETE_INFO_t status;
    L7_netBufHandle           bufHandle;
  }data;
  
}isdpMsg_t;

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
void isdpDispatchCmd(isdpMsg_t msg);
void isdpTask();
L7_RC_t isdpPduReceiveQueue( L7_uint32 intIfNum, L7_netBufHandle bufHandle);
L7_RC_t isdpProcessMsgQueue(isdpMsg_t msg);
L7_RC_t isdpTimerExpired(L7_uint32 arg1, L7_uint32 arg2);
L7_RC_t isdpPduReceiveProcess(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
L7_RC_t isdpPduSend(void);
L7_RC_t isdpTimerProcess(void);
L7_RC_t isdpDataPduRead(L7_uint32 intIfNum, L7_uchar8 *data, L7_uint32 length,
    isdpPacket_t* isdpPacket);
L7_RC_t isdpCtlApplyConfigData(void);

#endif /* INCLUDE_ISDP_CONTROL_H*/
