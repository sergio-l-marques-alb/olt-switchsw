/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtrdisc_util.h
*
* @purpose   Specifies the interfaces for utility functions used by 
*            router discovery. Also specifies some structures used by
*            the router discovery processing task.
*
* @component 
*
* @comments  none
*
* @create    11/02/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/
/*************************************************************
    
*************************************************************/

#ifndef INCLUDE_RTR_DISC_UTIL_H
#define INCLUDE_RTR_DISC_UTIL_H

#include "osapi_support.h"
#include "l7_cnfgr_api.h"


/* Holds information required by the processing task */
typedef struct rtrDiscInfo_s
{
    L7_uint32       rtrDiscProcessingTaskId;
    void *          rtrDiscProcessingQueue;
} rtrDiscInfo_t;

extern rtrDiscInfo_t          *pRtrDiscInfo;
      

/* Message structure used for notifying the processing task of 
   occurence of routing events */
typedef struct rtrDiscRtrEventMsg_s
{
    L7_uint32                   intIfNum;       
    L7_uint32                   event;          /* one of L7_RTR_EVENT_CHANGE_t */
    void                        *pData;         /* event-specific data */
    L7_BOOL                     asyncResponseNeeded;
    ASYNC_EVENT_NOTIFY_INFO_t   response;     /* Asynchronous response info */
} rtrDiscRtrEventMsg_t;

/* Message structure used for notifying the processing task to handle the 
   event or configurator command*/
typedef struct rtrDiscMsg_s
{
    L7_uint32       msgId;

    union
    {
        rtrDiscRtrEventMsg_t       rtrEvent;
        L7_CNFGR_CMD_DATA_t        cnfgrCmdData;
    }type;
} rtrDiscMsg_t;

/* Different types of messages the processing task can receive and handle */
typedef enum
{
    RTR_DISC_FIRST_MSG_ID,
    RTR_DISC_RTR_EVENT,
    RTR_DISC_INTF_MODE_EVENT,
    RTR_DISC_RESTORE_EVENT,
    RTR_DISC_CNFGR,
    RTR_DISC_TICK,
    RTR_DISC_LAST_MSG_ID
}RTR_DISC_MSG_ID_t;

/* Structure used to keep track of creation and acquiring of the interfaces */
typedef struct rtrDiscIntfInfo_s     
{
    L7_BOOL         rtrDiscIntfCreated;
    L7_uint32       intIfNum;
} rtrDiscIntfInfo_t;


/* rtrdisc_util.c */

/*********************************************************************
* @purpose  Processes the timer tick messages
*
* @param    parm1       not used
* @param    parm2       not used
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscTimerTick(void * parm1, L7_uint32 parm2);

/*********************************************************************
* @purpose  Processes the timer tick messages
*
* @param    
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscTimerTickProcess();


/*********************************************************************
* @purpose  Processes the solication pkts from hosts
*
* @param    intIfNum		Interface number on which the packet arrived
* @param    netBufHandle	Pointer to the packet
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscSolicitPktProcess(L7_uint32 intIfNum, L7_netBufHandle netBufHandle);

/*********************************************************************
* @purpose  Send the router discovery advertisement packet
*
* @param    intIfNum	   Interface on which the packet has to be sent
* @param    pSrcIPAddrList List of IP Addresses configured on the interface
* @param    dstAddress     Destination address of the interface
*
* @returns  L7_SUCCESS	Packet has been sent successfully
* @returns  L7_FAILURE	Packet has not been sent
*       
* @notes    The parameters include source address because, in the future, if
* @notes    more than 1 ip address is configured on a interface, we can know
* @notes    the source address that has to be used.
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscAdvPktSend(L7_uint32 intIfNum, 
                          L7_rtrIntfIpAddr_t *pSrcIPAddrList,     
                          L7_IP_ADDR_t dstIPAddr, L7_int32 prefLevel, 
                          L7_ushort16 lifeTime);

/**************************************************************************
* @purpose  compute checksum
*
* @param    icmp		message to compute checksum
* @param	len			Length of the message
*
* @returns  calculated checksum (ones complement) of the icmp struct
*
* @comments    none
*
* @end
*************************************************************************/
L7_ushort16 rtrDiscChksmCalc (L7_ushort16 *data, L7_uint32 len);
/**************************************************************************
* @purpose  compute time of day
*
* @param    tv		    timeval structure
* @param	not_used	timezone structure
*
* @returns  
*
* @comments    none
*
* @end
*************************************************************************/
void rtrDiscGetTimeOfDay( struct timeval *tv , void *not_used );

/**************************************************************************
* @purpose  Apply the max adv interval
*
* @param    intIfNum	Internal interface number
* @param	time		Max adv interval time in sec
*
* @returns  
*
* @comments    
*
* @end
*************************************************************************/
void rtrDiscMaxAdvIntervalApply(L7_uint32 intIfNum, L7_uint32 time);

/*********************************************************************
* @purpose  Process router discovery frames
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_PROCESSED  if frame has been processed; continue processing it
* @returns  SYSNET_PDU_RC_IGNORED    if frame has been ignored; continue processing it
*
* @notes    none
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t rtrDiscPktIntercept(L7_uint32 hookId,
                                    L7_netBufHandle bufHandle,
                                    sysnet_pdu_info_t *pduInfo,
                                    L7_FUNCPTR_t continueFunc);

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL rtrDiscMapIntfConfigEntryGet(L7_uint32 intIfNum, rtrDiscIntfCfgData_t **pCfg);

L7_uint32 rdMinAdvIntervalDefGet(L7_uint32 intIfNum);
L7_uint32 rdAdvLifetimeDefGet(L7_uint32 intIfNum);

#endif /* INCLUDE_RTR_DISC_UTIL_H */
