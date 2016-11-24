/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_sip.h
*
* @purpose SIP message parsing routines
*
* @component VOIP |SIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef VOIP_SIP_H
#define VOIP_SIP_H
#include "voip_parse.h"

/* SIP Message */
typedef struct voipSipMessage_s 
{
    L7_uchar8 *sip_method;                 /* METHOD (SIP request only) */
    L7_uint32 status_code;                 /* Status Code (SIP answer only) */
    L7_uchar8 *call_id;                    /* Call-ID header */
    L7_uchar8 *content_type;               /* Content-Type header */
}voipSipMessage_t;

/* SIP message can be REQUEST / RESPONSE */

#define MSG_IS_RESPONSE(msg) ((msg)->status_code!=0)
#define MSG_IS_REQUEST(msg)  ((msg)->status_code==0)
/* REQUEST TYPE */
#define MSG_IS_INVITE(msg)   (0==strcmp((msg)->sip_method,"INVITE"))
#define MSG_IS_BYE(msg)      (0==strcmp((msg)->sip_method,"BYE"))
#define MSG_IS_CANCEL(msg)   (0==strcmp((msg)->sip_method,"CANCEL"))

/* RESPONSE TYPE */
#define SIP_OK  200

#define MSG_IS_STATUS_1XX(msg) ((msg)->status_code >= 100 && \
                                (msg)->status_code < 200)
#define MSG_IS_STATUS_2XX(msg) ((msg)->status_code >= 200 && \
                                (msg)->status_code < 300)
#define MSG_IS_STATUS_3XX(msg) ((msg)->status_code >= 300 && \
                                (msg)->status_code < 400)
#define MSG_IS_STATUS_4XX(msg) ((msg)->status_code >= 400 && \
                                (msg)->status_code < 500)
#define MSG_IS_STATUS_5XX(msg) ((msg)->status_code >= 500 && \
                                (msg)->status_code < 600)
#define MSG_IS_STATUS_6XX(msg) ((msg)->status_code >= 600 && \
                                (msg)->status_code < 700)
#define MSG_TEST_CODE(msg,code) (MSG_IS_RESPONSE(msg) && \
                                 (code)==(msg)->status_code)


typedef struct voipSipCall_s
{
    L7_uchar8                 *call_id;
    L7_uint32                 valid;
    L7_ulong64                prev_counter;
    voipFpEntry_t             db;
    struct voipSipCall_s      *next;
} voipSipCall_t;


/*********************************************************************
* @purpose Allocate memory and initialize SIP Message
*
* @param   voipSipMessage_t ** sip (input) Pointer to SIP message 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSipMessageInit (voipSipMessage_t ** sip);

/*********************************************************************
* @purpose Free the memory allcoated to SIP Message
*
* @param   voipSipMessage_t * sip (input) Pointer to SIP message
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipSipMessageFree (voipSipMessage_t * sip);

/*********************************************************************
* @purpose Parse SIP message
*
* @param   const L7_uchar8 *ipstr    Pointer to IpAddress(Destn Ip)
* @param   const L7_uchar8 *buf      Pointer to packet data
* @param   L7_uint32       length    length of message
* @param   L7_uint32       interface interface number  
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSipMessageParse (const L7_uchar8 *ipstr, const L7_uchar8 *buf, 
                             L7_uint32 length);
/*********************************************************************
* @purpose Enable/disable a SIP session
*
* @param   voipFpEntry_t *s         Pointer to voipFpEntry_t
* @param   L7_uint32 enable         L7_ENABLE | L7_DISABLE
* @param   L7_uint32 interface      interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSetupSipSession(voipFpEntry_t *s, L7_uint32 enable);
/*********************************************************************
* @purpose  Stop prioritizing VoIP packets/Remove the call from SIP
*           database
*
* @param    const L7_uchar8 *id      call Id to be removed
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSipCallEnd(L7_uchar8 *callId);
/*********************************************************************
* @purpose Remove SIP Call entry
*
* @param   voipSipCall_t   **list   List of SIP calls
* @param   const L7_uchar8 *id      call Id to be removed
* @param   voipSipCall_t   **del     Pointer to SIP call remeved entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
voipSipCallRemove(voipSipCall_t **list, const L7_uchar8 *id, voipSipCall_t **del);
/*********************************************************************
* @purpose  Clear all call entries corrosponding to an interface
*
* @param    L7_uint32 interface       Interface number for which entries
*           need to be cleared
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSipClearInterfaceEntries();
void voipSipCallDump();
#endif
