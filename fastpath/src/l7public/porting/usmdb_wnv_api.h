/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wnv_api.h
*
* @purpose    Wireless Network Visualization USMDB API header
*
* @component  USMDB
*
* @comments   none
*
* @create     02/01/06
*
* @author     dfowler
*
* @end
*             
*********************************************************************/
#ifndef INCLUDE_USMDB_WNV_API_H
#define INCLUDE_USMDB_WNV_API_H
#include "wnv_api.h"

/*********************************************************************
*
* @purpose  Add entry to Network Visualization event queue.
*
* @param    wnvEventData_t *event @b{(input)} pointer to event data.
*
* @returns  void
*
* @notes  This API must not block on message queue, if queue is full lost
*           events counter is updated and returns immediately.
*
* @end
*********************************************************************/
void usmDbWnvEventAdd(wnvEventData_t *event);

/*********************************************************************
*
* @purpose  Get entry from top of Network Visualization event queue.
*
* @param    wnvEventData_t *event @b{(output)} pointer to event data.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, queue is empty.
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t usmDbWnvEventGet(L7_short16 idx, wnvEventData_t *event);

/*********************************************************************
*
* @purpose  Get count of Network Visualization lost events.
*
* @param    L7_uint32 *events @b{(output)} pointer to lost events count.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, queue is empty.
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t usmDbWnvEventsLostGet(L7_uint32 *events);

/*********************************************************************
*
* @purpose  Reset Network Visualization event queue.
*
* @returns  none
*
* @notes  
*
* @end
*********************************************************************/
void usmDbWnvEventsReset(L7_short16 idx);

/*********************************************************************
 *
 * @purpose  Used to set the imageCfgChanged flag.
 *
 *
 * @returns  none
 *
 * @notes   This is primarily used by the web server whenever it creates
 *          (or deletes) an image file.
 *
 * @end
 *********************************************************************/
void usmDbWnvSetImageCfgChanged();

/*********************************************************************
 *
 * @purpose  Used to set the connectionActive flag.
 *
 *
 * @returns  none
 * @returns  none
 *
 * @notes   This is primarily used by the wnv to determine whether to log events
 *
 * @end
 *********************************************************************/
void usmDbWnvConnectionActiveSet();

/*********************************************************************
 *
 * @purpose  Used to clear the connectionActive flag.
 *
 *
 * @returns  none
 * @returns  none
 *
 * @notes   This is primarily used by the wnv to determine whether to log events
 *
 * @end
 *********************************************************************/
void usmDbWnvConnectionActiveClear();

#endif /* INCLUDE_USMDB_WNV_API_H */

