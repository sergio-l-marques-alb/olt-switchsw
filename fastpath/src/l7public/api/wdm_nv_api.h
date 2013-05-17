/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_nv_api.h
*
* @purpose      Wireless Data Manager Network Visualization API header
*
* @component    WDM
*
* @comments     none
*
* @create       10/20/2005
*
* @author       dfowler
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_NV_API_H
#define INCLUDE_WDM_NV_API_H

#include "datatypes.h"
#include "comm_structs.h"
#include "wireless_commdefs.h"

#define L7_MAC_STRING_LENGTH 17

typedef enum
{
  L7_WNV_MEAS_SYS_ENGLISH=0, 
  L7_WNV_MEAS_SYS_METRIC
} L7_WNV_MEAS_SYS_t;

typedef struct wdmNVUserData_s
{
  L7_uchar8 showPowerDisplay;
  L7_uchar8 showTabView;
  L7_uchar8 showMAPs;
  L7_uchar8 showRAPs;
  L7_uchar8 showMAPClients;
  L7_uchar8 showDetectedClients;
  L7_uchar8 showLocationFlag;
} wdmNVUserData_t;

typedef struct wdmNVGraphLoc_s
{
  L7_uint32    bldgNum;
  L7_uint32    flrNum;
} wdmNVGraphLoc_t;

typedef struct wdmNVGraphCfgData_s
{
  L7_uchar8  index;
  L7_BOOL    inUse;
  L7_char8   graphName[L7_WDM_MAX_NV_GRAPH_NAME+1];
  L7_char8   fileName[L7_WDM_MAX_NV_FILE_NAME+1];
  L7_uchar8  imageMeasurement; 
  L7_uint32  imageWidth;
  L7_uint32  imageHeight;
  wdmNVGraphLoc_t graphLoc;
} wdmNVGraphCfgData_t;

typedef struct wdmNVCoordCfgData_s
{
  L7_uchar8                 graphIndex;  /* 1 - L7_WDM_MAX_NV_GRAPHS */
  L7_uint32                 x;
  L7_uint32                 y;
} wdmNVCoordCfgData_t;

/* For server convienence */
typedef struct wnvWSData_s
{
  L7_IP_ADDR_t              IP;
} wnvWSData_t;

/* For server convienence */
typedef struct wnvAPData_s
{
  L7_char8                  mac[L7_MAC_STRING_LENGTH];
  L7_uint32                 len;
  L7_char8                  location[L7_WDM_MAX_LOCATION_NAME+1];
  L7_uchar8                 GCId;
  L7_uint32                 X;
  L7_uint32                 Y;
  L7_WDM_AP_MANAGED_MODE_t  mode;
  L7_uchar8                 radio1Power;
  L7_uchar8                 radio1Channel;
  L7_uchar8                 radio2Power;
  L7_uchar8                 radio2Channel;
  L7_uint32                 status;
} wnvAPData_t;

/* For server convienence */
typedef struct wnvCSData_s
{
  L7_IP_ADDR_t             IP;
  L7_char8                 clientMac[L7_MAC_STRING_LENGTH];
  L7_uchar8                radio;
} wnvCSData_t;

typedef struct wdmNVDevLocTriggerData_s
{
  L7_uint8 building;
  L7_uint8 floor;
  L7_enetMacAddr_t mac;
  L7_WDM_ONDEMAND_LOC_TARGET_TYPE_t devType;
  L7_WDM_ONDEMAND_LOC_USE_RADIO_TYPE_t useRadios;
  L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t searchStatus;
  L7_uint32 numOfAPs;
} wdmNVDevLocTriggerData_s;

/*********************************************************************
*
* @purpose  Set NV user data
*
* @param    wdmNVUserData_t *ud  @b{(input)} pointer to userData
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes - Provides user preference persistence
*
* @end
*********************************************************************/
L7_RC_t wdmNVUserDataSet(wdmNVUserData_t *ud);

/*********************************************************************
*
* @purpose  Get NV user data
*
* @param    wdmNVUserData_t *ud  @b{(input)} pointer to userData
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes - Provides user preference persistence
*
* @end
*********************************************************************/
L7_RC_t wdmNVUserDataGet(wdmNVUserData_t *ud);

/*********************************************************************
*
* @purpose  Get NV user data
*
* @param    wdmNVUserData_t *ud  @b{(input)} pointer to userData
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes - Provides user preference persistence
*
* @end
*********************************************************************/
L7_RC_t wdmNVCheckLocationGet(L7_WDM_ONDEMAND_LOC_TRIG_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get NV config migration flag
*
* @param    L7_BOOL *flag  @b{(output)} pointer to config migration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t wdmNVCfgMigrateGet(L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Reset NV config migration flag after message display in NV
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t wdmNVCfgMigrateReset(void);

/*********************************************************************
*
* @purpose  Get next entry in the NV graph configuration.
*
* @param    L7_uchar8            index     @b{(input)}  current graph component index
* @param    L7_uchar8           *next      @b{(output)} next graph component index
* @param    wdmNVGraphCfgData_t *graph     @b{(output)} pointer to graph configuration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, no more entries
*
* @notes  index = 0 returns first graph entry
*
* @end
*********************************************************************/
L7_RC_t wdmNVGraphEntryNextGet(L7_uchar8            index,
                               L7_uchar8           *next,
                               wdmNVGraphCfgData_t *graph);

/*********************************************************************
*
* @purpose  Add or update existing entry in the NV graph configuration.
*
* @param    L7_uchar8            index     @b{(input)}  graph component index
* @param    wdmNVGraphCfgData_t *graph     @b{(output)} pointer to graph configuration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVGraphEntryAddUpdate(L7_uchar8            index,
                                 wdmNVGraphCfgData_t *graph);

/*********************************************************************
*
* @purpose  Delete an entry in the NV graph configuration.
*
* @param    L7_uchar8            index     @b{(input)}  graph component index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVGraphEntryDelete(L7_uchar8 index);


/*********************************************************************
*
* @purpose  Get building number and floor number for the given graph.
*
* @param    L7_uchar8       index     @b{(input)}  graph component index
* @param    wdmNVGraphLoc_t graphLoc  @b{(output)} pointer to graph building and floor location 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVGraphBldgFlrNumGet(L7_uchar8          index, 
                                wdmNVGraphLoc_t   *graphLoc);

/*********************************************************************
*
* @purpose  Get measurement system for the given graph.
*
* @param    L7_uchar8       index     @b{(input)}  graph component index
* @param    L7_WDM_MEASUREMENT_SYS_NAME_t measSys @b{(output)} pointer to measurement system 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVGraphMeasSystemGet(L7_uchar8          index, 
                                L7_WDM_MEASUREMENT_SYS_NAME_t *measSys);

/*********************************************************************
*
* @purpose  Set NV configuration coordinates for this switch.
*
* @param    wdmNVCoordCfgData_t *coord  @b{(input)} pointer to switch coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVWSCoordSet(wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Get NV configuration coordinates for this switch.
*
* @param    wdmNVCoordCfgData_t *coord  @b{(output)} pointer to switch coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVWSCoordGet(wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Add or update a NV configuration entry for a peer switch.
*
* @param    L7_IP_ADDR_t         ip     @b{(input)} peer switch IP address
* @param    wdmNVCoordCfgData_t *graph  @b{(input)} pointer to peer coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVPeerSwitchEntryAddUpdate(L7_IP_ADDR_t         ip,
                                      wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Get an NV configuration entry for a peer switch.
*
* @param    L7_IP_ADDR_t         ip     @b{(input)}  peer switch IP address
* @param    wdmNVCoordCfgData_t *graph  @b{(output)} pointer to peer coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVPeerSwitchEntryGet(L7_IP_ADDR_t         ip,
                                wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Delete an NV configuration entry for a peer switch.
*
* @param    L7_IP_ADDR_t         ip     @b{(input)}  peer switch IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVPeerSwitchEntryDelete(L7_IP_ADDR_t ip);

/*********************************************************************
*
* @purpose  Add or update a NV configuration entry for an access point.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)}  AP ethernet address
* @param    wdmNVCoordCfgData_t *graph    @b{(input)}  pointer to AP coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVAPEntryAddUpdate(L7_enetMacAddr_t     macAddr,
                              wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Get an NV configuration entry for an access point.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)}  AP ethernet address.
* @param    wdmNVCoordCfgData_t *graph    @b{(output)} pointer to AP coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVAPEntryGet(L7_enetMacAddr_t     macAddr,
                        wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Delete an NV configuration entry for an access point.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)}  AP ethernet address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVAPEntryDelete(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Add or update an entry in the NV detected client tree list.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)}  detected client MAC address
* @param    wdmNVCoordCfgData_t *coord   @b{(output)} pointer to detected client coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmNVDCSEntryAddUpdate(L7_enetMacAddr_t   macAddr,
                               wdmNVCoordCfgData_t *coord);

/*********************************************************************
*
* @purpose  Delete an entry in the NV detected client tree list.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  detected client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmNVDCSEntryDelete(L7_enetMacAddr_t   macAddr);

/*********************************************************************
*
* @purpose  Get an NV configuration entry for an detected client.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)}  detected client address.
* @param    wdmNVCoordCfgData_t *graph    @b{(output)} pointer to detected client coordinate data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmNVDCSEntryGet(L7_enetMacAddr_t     macAddr,
                         wdmNVCoordCfgData_t *coord);
#endif /* INCLUDE_WDM_NV_API_H */
