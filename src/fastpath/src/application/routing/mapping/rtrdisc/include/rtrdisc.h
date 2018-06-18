/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rdisc.h
*
* @purpose   To discover router protocols
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

#ifndef INCLUDE_RTR_DISC_H
#define INCLUDE_RTR_DISC_H


#include  "async_event_api.h"

/* ------------ timer ----------*/
#define RTR_DISC_TASK_TIMER 		1000 /*ms*/

#define L7_RTR_DISC_INTF_MAX_COUNT        L7_IPMAP_INTF_MAX_COUNT

/* Function prototypes */

/* rtrdisc.c*/


/*********************************************************************
* @purpose  This routine is called if an interface is enabled for 
* @purpose  for routing.   
*
* @param    intIfnum    internal interface number
* @param    state    @b{(input)} an event listed in L7_RTR_EVENT_CHANGE_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*       
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                          L7_uint32 event,
                                          void *pData, 
                                          ASYNC_EVENT_NOTIFY_INFO_t *response);


/*********************************************************************
* @purpose  Handles router being enabled    
*
* @param    intIfnum    internal interface number
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscRtrIntfEnable(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Handles router being disabled    
*
* @param    intIfnum    internal interface number
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscRtrIntfDisable(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Checks if rtr disc adv pkts can be sent out of this rtr intf
*
* @param    intIfnum    internal interface number
*
* @returns  L7_TRUE		if rtrdisc can send out adv pkts
* @returns  L7_FALSE  	if rtrdisc cannot send out adv pkts
*       
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL rtrDiscMayEnableInterface(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t rtrDiscIntfCreate(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t rtrDiscIntfDelete(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  This routine is called asynchronously when a routing event 
*           occurs that may change the operational state of the router
*           discovery component.
*
* @param    intIfnum    port changing state
* @param    event       one of L7_PORT_EVENTS_t
* @param    *pData      pointer to event specific data
* @param    *pResponse  pointer to event specific data
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Indicates to IP map, completion of event handling through 
*           asyncEventCompleteTally().
*       
* @end
*********************************************************************/

L7_RC_t rtrDiscRoutingEventChangeProcess (L7_uint32 intIfNum, 
                                          L7_uint32 event,
                                          void *pData, 
                                          ASYNC_EVENT_NOTIFY_INFO_t *pResponse);


/*********************************************************************
* @purpose  Router Discovery's processing task. Handles cnfgr commands,
*           interface events (from NIM), routing events (from IP map)
*           and the RESTORE event (invoked asynchronously on execution 
*           of the "clear config" command).
*
* @param    intIfnum    internal interface number
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscProcessingTask(void);



#endif /* INCLUDE_RTR_DISC_H */
