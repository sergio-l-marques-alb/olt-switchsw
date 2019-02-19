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
*
* @purpose  To process NIM events.
*
* @param    L7_uint32  intIfNum   internal interface number
* @param    L7_uint32  event      event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t rtrDiscIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);

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
* @purpose  Handles an ip address being added or changed    
*
* @param    intIfnum    internal interface number
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscRtrIntfIpAddressChange(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Handles an ip address being deleted    
*
* @param    intIfnum    internal interface number
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscRtrIntfIpAddressDelete(L7_uint32 intIfNum);


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
* @purpose  Handles link becoming active
*
* @param    intIfnum    internal interface number
*
* @returns  L7_SUCCESS	if rtrdisc starts sending out adv pkts
* @returns  L7_FAILURE  if rtrdisc cannot start sending out adv pkts
*       
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscLinkActive(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Handles link becoming inactive
*
* @param    intIfnum    internal interface number
*
* @returns  L7_SUCCESS	if rtrdisc stops sending out adv pkts
* @returns  L7_FAILURE  if rtrdisc cannot stop sending out adv pkts
*       
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscLinkInActive(L7_uint32 intIfNum);

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
L7_BOOL rtrDiscLinkStateGet(L7_uint32 intIfNum);

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
* @purpose  To process the Callback for L7_DELETE_PENDING
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
L7_RC_t rtrDiscIntfDeletePending(L7_uint32 intIfNum);

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
* @purpose  Process interface-related events
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes    L7_PORT_ROUTING_ENABLED and L7_PORT_ROUTING_DISABLED events 
*           (other than updating information to reflect the interface 
*           being acquired/released by IP map for routing)
*           are not handled by this routine, as these events originate
*           from inside the IP Mapping layer. Since the router discovery mapping layer 
*           is a "slave" to the IP Mapping layer, these events are 
*           handled via its registration with IP map.
*
* @notes    Gets invoked asynchronously. nimEventStatusCallback() is invoked to
*           to tell NIM the event has been handled.
*       
* @end
*********************************************************************/

L7_RC_t rtrDiscIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, 
                                 NIM_CORRELATOR_t correlator);


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
