/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_events.h
*
* @purpose    The functions that handle events from other components
*
* @component  NIM
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#ifndef NIM_EVENTS_H
#define NIM_EVENTS_H

#include "l7_common.h"
#include "nim_data.h"
#include "dtlapi.h"

#define      NIM_QUEUE       "NIM-Q "

/*********************************************************************
* @purpose  go through registered users and notify them of interface changes.
*
* @param    correlator    The correlator to match with the request
* @param    eventInfo     The event, intf, component, callback func  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
void nimDoNotify(NIM_CORRELATOR_t correlator, NIM_EVENT_NOTIFY_INFO_t eventInfo);

/*********************************************************************
* @purpose  Notify the system application layer of changes in interfaces
*
* @param    unit      	unit number (stack unit)
* @param    slot      	slot number 
* @param 	port		port number
* @param 	cardType	a card descriptor as given to Card Manager from HPC
* @param 	event		an event designator such as L7_ENABLE
* @param	interfaceType	the type of interface such as PHYSICAL
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t   nimCmgrIntfChangeCallback(L7_uint32 unit, L7_uint32 slot, L7_uint32 port,
                                     L7_uint32 cardType,L7_PORT_EVENTS_t event,
                                     SYSAPI_HPC_PORT_DESCRIPTOR_t *portData);


/*********************************************************************
* @purpose  Notify all recepients of nim notifications of the link up event 
*          
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine is called only once when the system initialization is complete.
*           This event was suppressed when the system comes up and is only 
*           propagated when all the components have initialized.
*
* @end
*
*********************************************************************/
L7_RC_t nimNotifyLinkUp();


/*********************************************************************
* @purpose  Notify the system application layer of a change in link status.
*
*
* @notes    This is the OLD function, once CM is in place, it SHALL be removed
*       
* @end
*********************************************************************/
void nimDtlIntfChangeCallback(nimUSP_t *usp, L7_uint32 event, void * dapiIntmgmt);



/*********************************************************************
* @purpose  Notifies registered routines of interface changes.
*
* @param    correlator    The correlator to match with the request
* @param    eventInfo     The event, intf, component, callback func  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t nimNotifyUserOfIntfChange(NIM_CORRELATOR_t correlator, NIM_EVENT_NOTIFY_INFO_t eventInfo);

/*********************************************************************
* @purpose  callback function for interface to mac addr translation 
*          
* @param    intfNum   Internal Interface Number
* @param    dtlEvent  DTL_EVENT_t DTL_EVENT_ADDR_INTF_MAC_QUERY
* @param    data      pointer to mac address
* @param    numOfElements pointer to the number of mac address in the list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    here the number of mac address requested is always 1 as it is 
*           a translation of only 1 interface number
*
* @end
*
*********************************************************************/
L7_RC_t nimMacAddrQueryCallback(L7_uint32 intfNum, DTL_EVENT_t dtlEvent, void *data, L7_uint32 *numOfElements);

/*********************************************************************
* @purpose  Card Manager Task
*
* @param    none
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void nimTask();


/*********************************************************************
*
* @purpose  Delete a correlator for the event
*
* @param    correlator        @b{(input)}  The correlator to delete
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @end
*********************************************************************/
L7_RC_t nimEventCorrelatorDelete(NIM_CORRELATOR_t correlator);

/*********************************************************************
*
* @purpose  Create a correlator for the event
*
* @param    correlator        @b{(output)}  A returned correlator
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @end
*********************************************************************/
L7_RC_t nimEventCorrelatorCreate(NIM_CORRELATOR_t *correlator);

/*********************************************************************
* @purpose  Remove the Event Handler Resources
*          
* @param    status    @b{(input)}   The returned value from the caller
*
* @param    complete  @b{(output)}  Boolean value indicating whether event is complete
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimEventTally(NIM_EVENT_COMPLETE_INFO_t status,L7_BOOL *complete);

/*********************************************************************
* @purpose  Initialize the Event Handler Resources
*          
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimEventHdlrInit();

/*********************************************************************
* @purpose  Remove the Event Handler Resources
*          
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
void nimEventHdlrFini();

/*********************************************************************
* @purpose  Clear the data to a reset state
*          
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    No data is freed
*
* @end
*
*********************************************************************/
L7_RC_t nimEventHdlrDataClear();

/*********************************************************************
* @purpose  Post Processor for Events
*          
* @param    eventInfo    @b{(input)}   The event information as assigned by the generator
* @param    status       @b{(output)}  The status of the event by either Tally or Timeout
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
void nimEventPostProcessor(NIM_EVENT_NOTIFY_INFO_t eventInfo, NIM_NOTIFY_CB_INFO_t status);



#endif /* NIM_EVENTS_H */
