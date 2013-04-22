/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dvmrp_map.h
*
* @purpose    DVMRP Mapping layer internal function prototypes
*
* @component  DVMRP Mapping Layer
*
* @comments   none
*
* @create     24/03/2006
*
* @author     gkiran
* @end
*
**********************************************************************/

#ifndef _DVMRP_MAP_H_
#define _DVMRP_MAP_H_

typedef enum
{
  DVMRPMAP_ADMIN_MODE_SET = 1,
  DVMRPMAP_INTF_MODE_SET,
  DVMRPMAP_INTF_METRIC_SET
} DVMRPMAP_UI_EVENT_TYPE_t;
/*
**********************************************************************
*                    FUNCTION PROTOTYPES
**********************************************************************
*/

/* dvmrpmap.c */

/*********************************************************************
*
* @purpose task to handle all Dvmrp Mapping messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void dvmrpMapTask();

/*********************************************************************
* @purpose  Save DVMRP user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpSave(void);

/*********************************************************************
* @purpose  Restore DVMRP user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpRestore(void);

/*********************************************************************
* @purpose  Check if DVMRP user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dvmrpHasDataChanged(void);
void dvmrpResetDataChanged(void);
/*********************************************************************
* @purpose  Build default DVMRP config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Apply DVMRP config data
*
* @param    void
*
* @returns  L7_SUCCESS  Config data applied, or not a DVMRP interface
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpApplyConfigData(void);

/*********************************************************************
* @purpose  Send event to DVMRP Vendor (to register with MFC)
*
* @param    familyType           @b{(input)} Address Family type
* @param    eventType            @b{(input)} Event Type
* @param    msgLen               @b{(input)} Message Length
* @param    eventMsg             @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapEventChangeCallback(L7_uchar8 familyType,
                                    L7_uint32  eventType,
                                    L7_uint32  msgLen, void *eventMsg);
/*********************************************************************
* @purpose  Admin scope call back function (to register with MFC)
*
* @param    eventType            @b{(input)} Event Type
* @param    msgLen               @b{(input)} Message Length
* @param    eventMsg             @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapAdminScopeCallback(L7_uint32  eventType,
                                   L7_uint32  msgLen, void *eventMsg);
/*********************************************************************
* @purpose  Process UI events
*
* @param    addrFamily  @b{(input)} Address Family Identifier
* @param    event       @b{(input)} UI Event
* @param    pData       @b{(input)} UI Event Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapUIEventChangeProcess (DVMRPMAP_UI_EVENT_TYPE_t event, 
                                      void *pData);

/*********************************************************************
* @purpose  Post UI event to the Mapping layer
*
* @param    addrFamily  @b{(input)} Address Family Identifier
* @param    event       @b{(input)} UI Event
* @param    pData       @b{(input)} UI Event Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapUIEventSend (DVMRPMAP_UI_EVENT_TYPE_t eventType, 
                             L7_uint32 mode,
                             L7_uint32 intIfNum,
                             L7_uint32 metric);

/*********************************************************************
* @purpose  To send A messge to Queue
*
* @param    QIndex    @b{(input)} Index of the queue to which the message has to be sent
* @param    msg       @b{(input)} Pointer to the message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*********************************************************************/

L7_RC_t dvmrpMessageSend(L7_uint32 QIndex,void *msg);


/**********************DEBUG TRACE ROUTINE PROTOTYPES ***************/

/*********************************************************************
 * @purpose  Saves Dvmrp configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    DvmrpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpDebugSave(void);
/*********************************************************************
 * @purpose  Restores Dvmrp debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    dvmrpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpDebugRestore(void);
/*********************************************************************
 * @purpose  Checks if Dvmrp debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL dvmrpDebugHasDataChanged(void);

/*********************************************************************
 * @purpose  Build default Dvmrp config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void dvmrpDebugBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
 * @purpose  Apply Dvmrp debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpApplyDebugConfigData(void);


#endif /* _DVMRP_MAP_H_ */
