/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wnv_api.h
*
* @purpose      Wireless Network Visualization API header
*
* @component    WDM
*
* @comments     none
*
* @create       02/18/2006
*
* @author       dfowler
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WNV_API_H
#define INCLUDE_WNV_API_H

#include "datatypes.h"
#include "comm_structs.h"
#include "wireless_commdefs.h"

#define L7_NV_MAX_GC_NAME   16
#define L7_NV_MAX_FILE_NAME 32

#ifdef _L7_OS_VXWORKS_
#define RAM_NV_PATH	""
#define RAM_NV_NAME  "RamNV:"
#else
#define RAM_NV_PATH	"/usr/local/ptin/log/fastpath/"
#define RAM_NV_NAME "ramnv"
#endif  

typedef enum
{
  L7_WNV_EVENT_LOCAL_WS_UPDATE, /* no index, simple update for IP address */
  L7_WNV_EVENT_PEER_WS_ADD,     /* index is IP, use wdmPeerSwitch* APIs */
  L7_WNV_EVENT_PEER_WS_UPDATE,
  L7_WNV_EVENT_PEER_WS_DELETE,
  L7_WNV_EVENT_MANAGED_AP_ADD,  /* index is MAC, use wdmManagedAP* APIs */
  L7_WNV_EVENT_MANAGED_AP_UPDATE,
  L7_WNV_EVENT_MANAGED_AP_DELETE,
  L7_WNV_EVENT_RF_SCAN_AP_ADD,  /* index is MAC, use wdmRFScan* APIs */
  L7_WNV_EVENT_RF_SCAN_AP_UPDATE,
  L7_WNV_EVENT_RF_SCAN_AP_DELETE,
  L7_WNV_EVENT_CLIENT_ADD,    /* index is MAC, use wdmAssocClient* APIs */
  L7_WNV_EVENT_CLIENT_UPDATE,
  L7_WNV_EVENT_CLIENT_DELETE,
  L7_WNV_EVENT_CC_MOVED,
  L7_WNV_EVENT_LOST_MESSAGES
} L7_WNV_EVENT_t;

typedef struct wnvEventData_s
{
  L7_uint32 eventId;  /* L7_WNV_EVENT_t */
  union {
    L7_IP_ADDR_t      ip;
    L7_enetMacAddr_t  macAddr;
    struct {
      L7_enetMacAddr_t macAddr;
      L7_enetMacAddr_t apMacAddr;
      L7_uchar8        apRadioIf;
    } client;
  } wdmIndex;
} wnvEventData_t;


/*********************************************************************
*
* @purpose  Add entry to Network Visualization event queue.
*
* @param    wnvEventData_t *event @b{(input)} pointer to event data.
*
* @returns  void
*
* @notes  This API must be non-blocking, if queue is full lost
*           events counter is updated and returns immediately.
*
* @end
*********************************************************************/
void wnvEventAdd(wnvEventData_t *event);

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
L7_RC_t wnvEventGet(L7_short16 idx, wnvEventData_t *event);

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
L7_RC_t wnvEventsLostGet(L7_int32 *events);

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
void wnvEventsReset(L7_short16 idx);

/*********************************************************************
*
* @purpose  Load NV images from NVRAM format.
*
* @param    L7_uchar8 *imageCfg @b{(input)} pointer to NVRAM cfg,
*                                            L7_WIRELESS_NV_IMAGES_NVRAM_SIZE
* @returns  none
*
* @notes  This function is called on a configuration file load.
*
* @end
*********************************************************************/
void wnvImageCfgLoad(L7_uchar8 *imageCfg);

/*********************************************************************
*
* @purpose  Save NV images to NVRAM format.
*
* @param    L7_uchar8 *imageCfg @b{(output)} pointer to NVRAM cfg,
*                                             L7_WIRELESS_NV_IMAGES_NVRAM_SIZE
*
* @returns  none
*
* @notes  This function is called on a configuration file save.
*
* @end
*********************************************************************/
void wnvImageCfgSave(L7_uchar8 *imageCfg);

/*********************************************************************
*
* @purpose  Restore NV images NVRAM format to default.
*
* @param    L7_uchar8 *imageCfg @b{(output)} pointer to NVRAM cfg,
*                                             L7_WIRELESS_NV_IMAGES_NVRAM_SIZE
*
* @returns  none
*
* @notes  This function is called to build default data, this would
*           be done on a "clear config".
*
* @end
*********************************************************************/
void wnvImageCfgDefault(L7_uchar8 *imageCfg);

/*********************************************************************
*
* @purpose  Determine if NV image cfg has changed since last
*            time wnvImageCfgLoad or wnvImageCfgSave was called.
*
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  This is called to determine if the NV image
*          configuration needs to be saved.
*
* @end
*********************************************************************/
L7_BOOL wnvHasImageCfgChanged();

/*********************************************************************
*
* @purpose  Used to set the imageCfgChanged flag.
*
*
* @returns  none
* @returns  none
*
* @notes   This is primarily used by the web server whenever it creates
*          (or deletes) an image file.
*
* @end
*********************************************************************/
void wnvSetImageCfgChanged();

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
void wnvConnectionActiveSet();

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
void wnvConnectionActiveClear();

#endif /* INCLUDE_WNV_API_H */
