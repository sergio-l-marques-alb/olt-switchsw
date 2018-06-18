/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ospf_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component 
*
* @comments
*
* @create 07/22/2003
*
* @author markl
* @end
*
**********************************************************************/


#ifndef INCLUDE_OSPF_CNFGR_H
#define INCLUDE_OSPF_CNFGR_H

#include "nimapi.h"
#include "acl_api.h"

typedef enum {
    OSPF_CNFGR_INIT = 0,
    OSPF_RTR_EVENT,
    OSPF_RESTORE_EVENT,
    OSPF_RTO_EVENT,
    OSPF_ACL_EVENT
} ospfMessages_t;

typedef struct ospfMapRtrEventMsg_s
{
    L7_uint32                   intIfNum;       
    L7_uint32                   event;          /* one of L7_RTR_EVENT_CHANGE_t */
    void                        *pData;         /* event-specific data */
    L7_BOOL                     asyncResponseNeeded;
    ASYNC_EVENT_NOTIFY_INFO_t   response;     /* Asynchronous response info */
} ospfMapRtrEventMsg_t;

typedef struct ospfMapAclEventMsg_s
{
    L7_uint32                   aclNum;       
    aclRouteFilter_t            event;          
} ospfMapAclEventMsg_t;

typedef struct ospfMapAdminModeEventMsg_s
{
    L7_uint32           intIfNum;       
    L7_uint32           mode;          /* L7_ENABLE or L7_DISABLE */
} ospfMapAdminModeEventMsg_t;
      
typedef struct
{
    ospfMessages_t msg;

    union
    {
        ospfMapRtrEventMsg_t rtrEvent;
        ospfMapAclEventMsg_t aclEvent;
        ospfMapAdminModeEventMsg_t adminModeEvent;
        L7_CNFGR_CMD_DATA_t cmdData;
    } type;

} ospfMapMsg_t;

/****************************************
*
*  OSPF Cnfgr State
*
*****************************************/
typedef enum {
  OSPFMAP_PHASE_INIT_0 = 0,
  OSPFMAP_PHASE_INIT_1,
  OSPFMAP_PHASE_INIT_2,
  OSPFMAP_PHASE_WMU,
  OSPFMAP_PHASE_INIT_3,
  OSPFMAP_PHASE_EXECUTE,
  OSPFMAP_PHASE_UNCONFIG_1,
  OSPFMAP_PHASE_UNCONFIG_2,
  OSPFMAP_PHASE_LAST
} ospfMapCnfgrState_t;

extern L7_char8 *ospfCfgPhaseNames[OSPFMAP_PHASE_LAST];


/****************************************
*
*  OSPF Deregistrations
*
*****************************************/
typedef struct {
  L7_BOOL ospfMapSave;
  L7_BOOL ospfMapRestore;
  L7_BOOL ospfMapHasDataChanged;
} ospfMapDeregister_t;
                            
/*********************************************************************
*
* @purpose  Determine whether OSPF is in a configuration state where it
*           is ready to handle interface events.
*
* @param    void
*                            
* @returns  L7_TRUE if OSPF can handle interface events
* @returns  L7_FALSE otherwise
*
* @notes    OSPF can handle interface events in INIT_3, EXECUTE, or
*           UNCONFIG_1 states. INIT_3 is allowed because NIM may go
*           to EXECUTE and issue events before OSPF processes its
*           notification to transition to EXECUTE. UNCONFIG_1 is
*           allowed because a stack may transition through the 
*           unconfig states when a unit leaves the stack and NIM 
*           will issue events to delete the interfaces associated
*           with the lost unit during UNCONFIG_1.
*
* @end
*********************************************************************/
L7_BOOL ospfMapReadyForIntfEvents(void);
                            
/*********************************************************************
*
* @purpose  Handle initialization events from the configurator.
*
* @param    pCmdData - @b{(input)}  Indicates the command and request 
*                                   from the configurator
*                            
* @returns  void
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the OSPF Map component.  This function is re-entrant.
*           Most actions are passed to the OSPF mapping thread instead of 
*           being processed on the configurator's thread.
*
* @end
*********************************************************************/
void ospfMapApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  Handles initialization messages from the configurator. 
*
* @param    pCmdData - @b{(input)}  Indicates the command and request 
*                                   from the configurator
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void ospfCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_OSPF_CNFGR_H */
