/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   ms_api.h
*
* @purpose    Component APIs for the Message Service
*
* @component  ms
*
* @comments   
*
* @create     11/18/2008
*
* @author     bradyr
* @end
*
**********************************************************************/

#ifndef MS_API_H
#define MS_API_H

#include "l7_common.h"

/* Always add new registarId's at the end to keep the 
** message service backwards compatible.
*/
typedef enum
{
  L7_MS_FIRST_REGISTRAR_ID = 1,
 
  L7_MS_CORE_HELPER_REGISTRAR_ID = L7_MS_FIRST_REGISTRAR_ID,
  L7_MS_DOT3AD_REGISTRAR_ID,
  L7_MS_DOT1S_REGISTRAR_ID,
  L7_MS_CPKT_REGISTRAR_ID,
  L7_MS_DRIVER_STK_MGR_REGISTRAR_ID,
  L7_MS_DRIVER_USL_REGISTRAR_ID,
  L7_MS_SDM_REGISTRAR_ID,              /* SDM template manager */

  /* Always last */
  L7_MS_LAST_REGISTRAR_ID

} L7_MS_REGISTRAR_IDS;

typedef enum
{
  MS_TARGET_CORE = 0,
  MS_TARGET_HELPER,
} L7_MS_TARGET_t;

typedef enum
{
  MS_RELIABLE = 0,
  MS_BEST_EFFORT,
} L7_MS_DELIVERY_MODE_t;

typedef struct
{
  L7_uint32    appRegistar;
  L7_uint32    target;
  L7_uchar8    appData[];
} msSyncMsgHdr_t;

typedef struct
{
  void        *xmitCountSem;  /* Maintain integrity of xmitCount     */
  L7_uint32   xmitMask;       /* Mask of units with outstanding data */
  L7_uint32   xmitCount;      /* Number of incomplete transmits      */
} msXmitStruct_t;

/* Multicast response Data struct */
typedef struct
{
  L7_uchar8 *buf;         /* Buffer for response data */
  L7_uint32  buf_size;    /* Size of Buffer - max data that can be copied */
  L7_uint32  data_len;    /* Actual length of response data */
} msMcastData_t;

typedef void (*MsCallbackFcn)(msSyncMsgHdr_t         *msg,
                              L7_uint32               msg_len);

typedef struct
{
  L7_uint32   versionID;
  L7_uint32   msgType;
  L7_uint32   srcUnitID;
  L7_uint32   dstUnitMask;
  L7_uint32   msgSeqNum;
  L7_uint32   msgAckReq;
} MS_TRANSPORT_HDR_t;

/* Macros to set/get unit numbers in mask */
#define MS_UNIT_SETMASKBIT(_mask,_unit) ((_mask) |= (1 << ((_unit))))
#define MS_UNIT_ISMASKBITSET(_mask,_unit) ((_mask) & (1 << ((_unit))))
#define MS_UNIT_CLRMASKBIT(_mask,_unit)   ((_mask) &= (~(1 << ((_unit)))))

/*********************************************************************
* @purpose  Registers a callback function with the message service
*
* @param    registarId  ID to register
* @param    msCallback  Function pointer for message service to call
*                       when message is received
* @param    core_or_helper Message Service callback type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t msMsgRegister(L7_MS_REGISTRAR_IDS registarId,
                      MsCallbackFcn       msCallback,
                      L7_MS_TARGET_t      core_or_helper);

/*********************************************************************
* @purpose  Send a message to core or help function via the message service
*
* @param    msg         Pointer to the msg buffer
* @param    msg_len     Length of the msg buffer
* @param    dest_unit   Destination unit Id.
* @param    delivery_mode Use reliable or best effort delivery 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t msMsgSend(msSyncMsgHdr_t        *msg,
                  L7_uint32              msg_len,
                  L7_uint32              dest_unit,
                  L7_MS_DELIVERY_MODE_t  delivery_mode);

/*********************************************************************
* @purpose  Function registered with HPC to recieve messages from other
*           units
*
* @param    buffer      Pointer to the msg buffer
* @param    msg_len     Length of the msg buffer
*
* @returns  none
*
* @end
*********************************************************************/
void msMsgRcv(L7_uchar8 *buffer, L7_uint32 msg_len);

/*********************************************************************
* @purpose  Queries the platform component as to the largest message
*           size in bytes that the transport can handle.
*
* @param    none
*                                       
* @returns  the max size of message payload in bytes
*
* @comments    
*       
* @end
*********************************************************************/
L7_uint32 msTransportMaxMessageLengthGet(void);

/*********************************************************************
* @purpose  Register a routine to be called when an intrastack message
*           is received for the registrar.
*
* @param    *notify      Notification routine with the following parm
*                        @param    *buffer             pointer to buffer containing message
*                        @param    msg_len             length in bytes of message
* @param    registrarID  routine registrar ID 
*                                       
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is already used
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t msReceiveCallbackRegister(void (*notify)(L7_uchar8* buffer, L7_uint32 msg_len),
                                   L7_uint32 registrarID);

/*********************************************************************
* @purpose  Send a message to a specific unit in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in the
*           remote systems, the message is silently discarded there and
*           an error is returned to the sending caller.  This function will
*           hold the caller's thread until the message is either successfully
*           acknowledged or the send times out.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID routine registrar ID 
* @param    unit        destination unit number
* @param    msg_length  number of bytes in payload buffer
* @param    buffer      pointer to the payload to be sent
* @param    sendType    MS_RELIABLE or MS_BEST_EFFORT,
* @param    ackWait     Time (ms) to wait for acknowlegement (0 no wait)
*                                       
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport, message not acknowledged
* @returns  L7_ERROR registrar is not known to the message service
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t msMessageSend(L7_uint32 registrarID, L7_uint32 unit,
                      L7_uint32 msg_length,  L7_uchar8* buffer,
                      L7_MS_DELIVERY_MODE_t  sendType,
                      L7_uint32              ackWait);

/*********************************************************************
* @purpose  Acknowledge a message recieved from a unit in the stack.  Message
*           is delivered to corresponding registrarID in remote system.  If
*           there is no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in the
*           remote systems, the message is silently discarded there and
*           an error is returned to the sending caller.  This function will
*           hold the caller's thread until the message is either successfully
*           acknowledged or the send times out.
*
* @param    registrarID routine registrar ID 
* @param    unit        destination unit ID.
* @param    sendType    MS_RELIABLE or MS_BEST_EFFORT,
*
* @returns  L7_SUCCESS  message successfully dispatched to harware transport
* @returns  L7_FAILURE  problem occured in transport, message not acknowledged
* @returns  L7_ERROR    registrar is not known to the message service
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t msMessageAck(L7_uint32 registrarID, L7_uint32 unit,
                     L7_uint32 ack_length,  L7_uchar8* ack_buffer,
                     L7_MS_DELIVERY_MODE_t  sendType);

void msTransportMessageRecvHandler(L7_uint32           receive_id,
                                   MS_TRANSPORT_HDR_t * xportHdr,
                                   L7_uchar8*          buffer,
                                   L7_uint32           msg_len);
/*********************************************************************
* @purpose  Send a message to specified units in the stack via the message
*           service.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in the
*           remote systems, the message is silently discarded there and
*           an error is returned to the sending caller.  This function will
*           hold the caller's thread until the message is either successfully
*           acknowledged or the send times out.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID routine registrar ID 
* @param    unitMask    destination unit(s) mask
* @param    msg_length  number of bytes in payload buffer
* @param    buffer      pointer to the payload to be sent
* @param    snd_nexthop flag to request message transmit using next hop
* @param    resp_data   pointer to an array of response data structures.
* @param    ackWait     Time (milliseconds) to wait for ack(0 no wait)
*
* @returns  unit mask for units acknowledging the message
*
* @comments The caller must carefully setup the response data structure. The
*           array must be large enough to support the highest unit ID and each
*           data pointer must be valid. Only limited sanity checks can be
*           performed by the message service to verify its integrity.
*       
* @end
*********************************************************************/
L7_uint32 msMessageMcast(L7_uint32 registrarID, L7_uint32  unitMask,
                         L7_uint32 msg_length,  L7_uchar8* buffer,
                         L7_BOOL   snd_nexthop, msMcastData_t *resp_data,
                         L7_uint32 ackWait);

L7_uint32 msMessageMcastAck(L7_uint32 registrarID, L7_uint32  unit,
                         L7_uint32 msg_length,  L7_uchar8* buffer,
                         L7_BOOL   snd_nexthop);

#endif /*MS_API_H*/

