/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_event.c
*
* Purpose: This file contains the functions to events. 
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: Shekhar Kalyanam  3/14/2001 
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_EVNT_GLOBALS               /* Enable global space   */    
#include "dtlinclude.h"


#if DTLCTRL_COMPONENT_EVENT

/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES 
**********************************************************************
*/

void (*dtlNotifyChange)(nimUSP_t *usp, L7_uint32 event,void * dapiIntmgmt);

/*
**********************************************************************
*                           API FUNCTIONS 
**********************************************************************
*/
/*********************************************************************
* @purpose  Registers for port event notifications from the Advanced Network Processing Layer
*
* @param    function to be called with the following parameters
*               - usp     @b{(input)} NIM USP structure
*               - event   @b{(input)} Event type                    
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    provide traffic parameterization
*
* @end
*********************************************************************/
void dtlRegister(void (*notifyChange)(nimUSP_t *usp, L7_uint32 event,
                                      void * dapiIntmgmt))
{
  dtlNotifyChange = notifyChange;
}

/*********************************************************************
* @purpose  Change callback based on event puts it in a message queue 
* @purpose  and releases the interrupt thread
*
* @param    *ddusp         @b{(input)} Device driver reference to unit slot and port
* @param	  family         @b{(input)} Device Driver family type 
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlLinkChangeCallback(DAPI_USP_t *ddusp, 
                                    DAPI_FAMILY_t family, 
                                    DAPI_CMD_t cmd, 
                                    DAPI_EVENT_t event,
                                    void *dapiEventInfo)
{

  L7_RC_t mrc = L7_SUCCESS; 
  L7_RC_t dr= L7_SUCCESS;
  L7_ushort16 restoredtlMsgTail;
  L7_ushort16 savedTail;

  if (event == DAPI_EVENT_INTF_DOT1S_STATE_QUERY ||
	  event == DAPI_EVENT_INTF_DOT1S_STATE_SET_RESPONSE)
  {
    dr = dtlGenericCallback(ddusp,family,cmd,event,dapiEventInfo);
    return dr;

  }

  /*enter critical section*/
  dtlEnterCS();

  if (dtlMsgQueue && ((dtlMsgTail+1)%DTL_MSG_COUNT != dtlMsgHead))
  {
    restoredtlMsgTail = dtlMsgTail;
    dtlMsgTail = (dtlMsgTail+1)%DTL_MSG_COUNT;
    savedTail = dtlMsgTail;
    /*exit critical section*/
    dtlExitCS();

    memcpy(&(dtlMsg[savedTail].cmdData.dapiIntfEventInfo),dapiEventInfo,sizeof(DAPI_INTF_MGMT_CMD_t));
    memcpy(&(dtlMsg[savedTail].ddusp),ddusp,sizeof(DAPI_USP_t));


    dtlMsg[savedTail].cmdFamilyType = family;
    dtlMsg[savedTail].cmdType = cmd;
    dtlMsg[savedTail].eventType = event;


    if ((mrc = osapiMessageSend(dtlMsgQueue, (void *)&savedTail, DTL_MSG_SIZE,L7_NO_WAIT, L7_MSG_PRIORITY_NORM))
        != L7_SUCCESS)
    {
      /*enter critical section*/
      dtlEnterCS();
      dtlMsgTail = restoredtlMsgTail;
      /*exit critical section*/
      dtlExitCS();

      LOG_MSG("DTL: link change msg send failed, mrc=%d\n", mrc);
      dr = L7_FAILURE;
    }
  }
  else
  {
    /*exit critical section*/
    dtlExitCS();
    LOG_MSG("DTL: link change msg queue full , mrc=%d\n", mrc);
    dr = L7_FAILURE;
  }


  return dr;



}



/*********************************************************************
* @purpose  Change callback based on event
*
* @param    *ddusp         @b{(input)} Device driver reference to unit slot and port
* @param	  family         @b{(input)} Device Driver family type 
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlLinkChange(DAPI_USP_t *ddusp, 
                      DAPI_FAMILY_t family, 
                      DAPI_CMD_t cmd, 
                      DAPI_EVENT_t event,
                      void *dapiEventInfo)
{

  L7_uint32 dtlEvent;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t *dei;

  dei = (DAPI_INTF_MGMT_CMD_t *)dapiEventInfo;

  if (dtlNotifyChange != L7_NULLPTR)
  {

    usp.unit = ddusp->unit;
    usp.slot = ddusp->slot;
    usp.port = ddusp->port + 1;

    switch (event)
    {
/*  case DAPI_EVENT_PORT_CREATE:
      dtlEvent = L7_CREATE;
      break;
    case DAPI_EVENT_INTF_DESTROY:
      dtlEvent = L7_DELETE;
      break;
*/
    case DAPI_EVENT_INTF_LINK_DOWN:
      dtlEvent = L7_DOWN;
      break;
    case DAPI_EVENT_INTF_LINK_UP:
      dtlEvent = L7_UP;
      break;
    case DAPI_EVENT_COMMAND_COMPLETE:
      return(L7_SUCCESS);
    case DAPI_EVENT_COMMAND_NOT_COMPLETE:
      return(L7_SUCCESS);
    case DAPI_NUM_OF_EVENTS:
      return(L7_SUCCESS);
    default:
      return(L7_SUCCESS);
    }

    (*dtlNotifyChange)(&usp, dtlEvent,dei);
  }
  return(L7_SUCCESS);
}

/*
**********************************************************************
*                           PRIVATE FUNCTIONS  
**********************************************************************
*/

#endif /*DTLCTRL_COMPONENT_EVENT*/
