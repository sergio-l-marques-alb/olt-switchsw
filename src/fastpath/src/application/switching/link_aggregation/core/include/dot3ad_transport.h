#ifndef __DOT3AD_TRANSPORT_H__
#define __DOT3AD_TRANSPORT_H__

#include "ms_api.h"


typedef struct
{
  L7_COMPONENT_IDS_t compId;
  
} dot3ad_trans_msg_t;

#define DOT3AD_MASK_STRING_LENGTH  (((L7_MAX_INTERFACE_COUNT/32)+1)*15)

/* Least significant bit/rightmost bit is lowest interface # */
/* this is opposite of what SNMP wants */
#define L7_INTF_GETMASKSTRING(j,_str) {                                        \
        L7_uint32 x;                                                           \
                                                                               \
        for (x = 0; x < (L7_MAX_INTERFACE_COUNT); x++) {                       \
           sprintf(_str,"%s 0x%.8x ",(j).value[((x)/(8*sizeof(L7_uchar8)))]); \
        }                                                                      \
}

extern void *dot3adTransSema;
extern void *dot3adTransSyncSema;
extern dot3adHelperMsg_t *dot3adTransportPduDb; 
extern L7_INTF_MASK_t pduChangedMask;

/*********************************************************************
* @purpose  Initializes the dot3ad transport subsystem.
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
L7_RC_t dot3adTransportInit();

/*********************************************************************
* @purpose  Returns the resources allocated by the transport subsystem
*
* @param    
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void dot3adTransportFini();

void  dot3adTransportCoreCallback(msSyncMsgHdr_t         *msg,
                                    L7_uint32               msg_len);

void dot3adTransportHelperCallback(msSyncMsgHdr_t         *msg,
                                    L7_uint32               msg_len);

/*********************************************************************
* @purpose  Send a control message to all the helpers
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
L7_RC_t dot3adTransportBcastHelperCtlMsg(dot3ad_helper_msgType_t msgType);

#endif /*__DOT3AD_TRANSPORT_H__*/
