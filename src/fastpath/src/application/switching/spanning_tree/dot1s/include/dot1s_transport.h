
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot1s_transport.c
* @purpose     Spanning tree: conduit between core and the helpers
* @component   dot1s
* @comments    none
* @create      1/23/09
* @author      akulkarn
* @end
*             
**********************************************************************/
#ifndef __DOT1S_TRANSPORT_H_
#define __DOT1S_TRANSPORT_H_

#include "ms_api.h"
#include "dot1s_helper.h"

extern void *dot1sTransSema;
extern void *dot1sTransSyncSema;
extern dot1s_helper_msg_t *dot1sTransportPduDb;
extern L7_INTF_MASK_t dot1sPduChangedMask;

/*********************************************************************
* @purpose  Initializes the dot1s transport subsystem.
*
* @param    None
*
* @returns  L7_SUCCESS on success
*           L7_NO_MEM if it cannot allocate the memory
*           L7_FAILURE if it cannot create semaphore   
*
* @notes   
*           
*       
* @end
*********************************************************************/
L7_RC_t dot1sTransportInit();

/*********************************************************************
* @purpose  releases resources allocated byt the dot1s transport subsystem.
*
* @param    None
*
* @returns  None
*
* @notes   
*           
*       
* @end
*********************************************************************/
void dot1sTransportFini();

L7_RC_t dot1sTransportCallbacksRegister();
/*********************************************************************
* @purpose  dot1s core transmit task.
*
* @param    
*
* @returns  void
*
* @notes   This task serves as a conduit between the core and the helper(s).
*          It distributes the information from the core to the appropriate 
*          helper w/o blocking the core.
*           
*       
* @end
*********************************************************************/
void dot1s_transport_task();

/*********************************************************************
* @purpose  Callback from the message service for the core
*
* @param    
*
* @returns  void
*
* @notes   Currently no communication from Helper to core.
*           
*       
* @end
*********************************************************************/
void dot1sTransportCoreCallback(msSyncMsgHdr_t         *msg,
                                    L7_uint32               msg_len);

/*********************************************************************
* @purpose  Callback from the message service for the helper
*
* @param    
*
* @returns  void
*
* @notes   
*           
*       
* @end
*********************************************************************/
void dot1sTransportHelperCallback(msSyncMsgHdr_t         *msg,
                                    L7_uint32               msg_len);


#endif /* __DOT1S_TRANSPORT_H_*/
