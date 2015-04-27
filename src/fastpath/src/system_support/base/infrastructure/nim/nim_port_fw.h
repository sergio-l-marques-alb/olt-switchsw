/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2008
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename   nim_port_fw.h
*
* @purpose    Operations with physical port firmware
*
* @component  NIM
*
* @comments   none
*
* @create     14/04/2008
*
* @author     vkozlov
* @end
*
**********************************************************************/

#ifndef NIM_PORT_FW_H
#define NIM_PORT_FW_H

#if L7_FEAT_SF10GBT

#include "nim.h"
#include "nimapi.h"
#include "l7_common.h"
#include "l7_cnfgr_api.h"

/********************************************************/
/*    type definitions for NIM                          */
/*    firmware udpate HPC messages                      */
/********************************************************/

typedef enum 
{
  NIM_HPC_FIRMWARE = 0,
  NIM_HPC_LAST
} nimHpcMessageFamily_t;

typedef enum 
{
  NIM_HPC_FW_GET_REVISION,
  NIM_HPC_FW_LAST
} nimHpcFirmwareMessageID_t;

typedef struct 
{
  nimHpcMessageFamily_t   msgFamily;
  L7_uint32               msgID;
  L7_BOOL                 bResponse;
  L7_RC_t                 result;
  L7_uint32               targetUnit;
  L7_uint32               srcUnit;
  L7_uint32               port;      /* in */
} nimHpcMessageHeader_t;

typedef struct 
{
  nimHpcMessageHeader_t   hdr;            /* in/out */
  nimIntfPortFWRev_t      ifFWRevision;   /* out */
} nimHpcMsgFwGetRevision_t;

typedef struct 
{
  nimHpcMessageHeader_t hdr;
  L7_BOOL updateStatus;
} nimHpcMsgFwUpdateStatus_t;


/**************************************************************
* Message queue definitions                                   *
***************************************************************/

#define NIM_HPC_FW_RSP_Q_NAME       "nimHpcFwRspQ"
#define NIM_HPC_FWMSG_COUNT         8 
#define NIM_HPC_FWMSG_SIZE          sizeof(nimHpcMsgFwGetRevision_t)
#define NIM_HPC_FWMSG_WAIT          5000

/*********************************************************************
* @purpose  Callback for HPC messaging
*           
* @param    src_key {(input)}  Key (mac-address) of the unit that sent the msg
* @param    msg     {(input)}  The buffer holding the message
* @param    msg_len {(input)}  The buffer length of the message
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimHPCReceiveCallback(L7_enetMacAddr_t src_key,
                           L7_uchar8* buffer, L7_uint32 msg_len);


/* External declaration */
extern void* nimFwRspQ;

#endif
#endif /* NIM_PORT_FW_H */
