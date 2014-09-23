/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_outcalls.c
*
* @purpose 802.1Q Outcalls File
*
* @component dot1q
*
* @comments none
*
* @create 09/15/2000
*
* @author w. jacobs,djohnson
*
* @end
*
**********************************************************************/
#include <string.h>
#include <math.h>
#include "flex.h"
#include "dot1q_common.h"
#include "log.h"
#include "dot3ad_api.h"
#include "nimapi.h"
#include "dot1q_data.h"
#ifdef L7_QOS_FLEX_PACKAGE_COS
  #include "l7_cos_api.h"
#endif

extern dot1qCfgData_t  *dot1qCfg;
extern dot1qIntfData_t *dot1qQports;
extern vlanTree_t      *pVlanTree;
extern dot1qInfo_t     *dot1qInfo;

dot1qVlanRequestNotifyList_t vlanRequestNotifyList[L7_LAST_COMPONENT_ID];
dot1qVlanCheckNotifyList_t   vlanRequestCheckList[L7_LAST_COMPONENT_ID];
static L7_uint32 vlanEventTime[11][L7_LAST_COMPONENT_ID];
/*********************************************************************
* @purpose  dot1q sys init outcalls
*
* @param    none
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, otherwise
*
* @notes   Hooks for interactions with other components or
*          for platform-specific extensions to the standard
*          dot1q offering
*
*
* @end
*********************************************************************/
L7_RC_t dot1qSysInitOutcalls()
{

  /* Future:
       - Add registration to nim to be notified of port creations,
         particularly if a config file change results from the
         creation.
       - Add hook for probe
       - Assume that trunking changes will come from NIM
    */


  return(L7_SUCCESS);

} /* dot1qSysInitOutcalls */

/*********************************************************************
* @purpose  dot1q vlan delete outcalls
*
* @param    vlanID      VLAN ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, otherwise
*
* @notes   Hooks for interactions with other components or
*          for platform-specific extensions to the standard
*          dot1q offering.  This outcall occurs after the
*          update request to DTL, but before updating the config
*          file overlay.  It also only occurs if DTL request was
*          returned successfully.
* @notes    This routine is a tests response time on various network
*           processor platforms.  The choice of how to proceed upon a
*           failure is dependent upon the platform.
*
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanDeleteOutcalls(L7_uint32 vlanID, L7_uint32 requestor)
{

  /* Future:
       - Add hook to notify fdb code of vlan being added or removed
         from fdb id

    */

  return(L7_SUCCESS);

} /* dot1qVlanDeleteOutcalls */



/*********************************************************************
* @purpose  dot1q vlan transient update outcalls
*
* @param    vlanID              VLAN ID
* @param    pCurrentDataEntry   Pointer to current VLAN data. Pointer is
*                               NULL if no data existed previously.
* @param    pDataEntry          Pointer to new VLAN data.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, otherwise
*
* @notes   Hooks for interactions with other components or
*          for platform-specific extensions to the standard
*          dot1q offering.  This outcall occurs after adding
*          the updated VLAN structure to the VLAN tree, and
*          before sending the request to the DTL layer. It is
*          useful when any changes to the VLAN data structure
*          needs to be made only for this invocation of the
*          DTL layer.  CAUTION: Users of this facility should
*          only make updates to the operational portion of the
*          dot1q_vlan_dataEntry structure as the dot1q config
*          file update  is made after this routine is executed.
*
*          This routine is invoked when a VLAN is being created
*          or modified.
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanTransientUpdate(L7_uint32 vlanID,
                                 dot1q_vlan_dataEntry_t *pCurrentDataEntry,
                                 dot1q_vlan_dataEntry_t *pDataEntry)
{

  /* Future:
       1. Possible hook for trunking, if trunking is executed the same
         as in RR)
       2. Need good name.
       3. Did not go for registration so that the user has easier granularity
          over the order in which changes are made.

    */


  return(L7_SUCCESS);

} /* dot1qVlanTransientUpdate*/

/*********************************************************************
* @purpose  Register a routine to be called when a VLAN is created,
*           deleted, or modified.
*
* @param    *notify      Notification routine with the following parm
*                        @param    vlanId              VLAN ID
*                        @param    intIfNum            internal interface number
*                        @param    event               VLAN_ADD_NOTIFY,
*                                                      VLAN_DELETE_NOTIFY,
*                                                      VLAN_ADD_PORT_NOTIFY ,
*                                                      VLAN_DELETE_PORT_NOTIFY or
*                                                      VLAN_PVID_CHANGE_NOTIFY
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    eventMask    combined mask of the events this component is interested in receiving.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, otherwise
*
* @notes    Comnponent will not be notified of events NOT in the mask.
*           Compoenets should be extra careful in not modifying the pointer as
*           the same porinter is made avialable to all registered users
*           Registered user should make a local copy if they would like to set/reset
*           any vlan bits.
*
* @end
*********************************************************************/
L7_RC_t vlanRegisterForChange(L7_RC_t (*notify)(dot1qNotifyData_t *vlanData,
                                                L7_uint32 intIfNum,
                                                L7_uint32 event),
                                         L7_COMPONENT_IDS_t  registrarID, L7_uint32 eventMask)
{

  if (registrarID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "VLAN registrar ID %d greater than L7_LAST_COMPONENT_ID\n", (int)registrarID);
    return(L7_FAILURE);
  }
  else
  {

    memset(&vlanRequestNotifyList[registrarID], 0, sizeof(dot1qVlanRequestNotifyList_t) );
    memset(&vlanEventTime, 0, sizeof(L7_uint32)*11*L7_LAST_COMPONENT_ID);
    vlanRequestNotifyList[registrarID].notify_request =  notify;

    /* Future:  Save function name     */
    bzero(vlanRequestNotifyList[registrarID].func_name, MAX_DOT1Q_FUNC_NAME);

    /* In sysinit, register set_vlan_configure as the standard handler
      for everything */

    vlanRequestNotifyList[registrarID].registrarID = registrarID;
    vlanRequestNotifyList[registrarID].eventMask = eventMask;
    return(L7_SUCCESS);
  }

} /* vlanRegisterForAddedRequests */

/*********************************************************************
* @purpose  Register a routine to be called when a VLAN is created,
*           deleted, or modified.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vlanDeregisterForChange(L7_COMPONENT_IDS_t registrarID)
{

  if (registrarID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "VLAN registrar ID %d greater than L7_LAST_COMPONENT_ID\n", (int)registrarID);
    return(L7_FAILURE);
  }
  else
  {
    memset(&vlanRequestNotifyList[registrarID], 0, sizeof(dot1qVlanRequestNotifyList_t) );

    return(L7_SUCCESS);
  }

} /* vlanRegisterForAddedRequests */



/*********************************************************************
* @purpose  Notify registered users of registered event request
*
* @param    vlanID              VLAN ID
* @param    intIfNum            internal interface number
* @param    event               VLAN_ADD_NOTIFY, VLAN_DELETE_NOTIFY, VLAN_ADD_PORT_NOTIFY,
*                               VLAN_DELETE_PORT_NOTIFY,VLAN_PVID_CHANGE_NOTIFY
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, otherwise
*
* @notes    none
*
* @end
*********************************************************************/
void vlanNotifyRegisteredUsers(dot1qNotifyData_t *vlanData,
                               L7_uint32 intIfNum,
                               L7_uint32 event)
{
  L7_uint32 i;
  L7_RC_t rc;
  L7_uint32 eventIndex = 0, callTime = 0;

  switch (event)
  {
  case VLAN_ADD_NOTIFY:
    eventIndex = 1;
      break;
  case VLAN_DELETE_PENDING_NOTIFY:
    eventIndex = 2;
      break;
  case VLAN_DELETE_NOTIFY:
    eventIndex = 3;
       break;
  case VLAN_ADD_PORT_NOTIFY:
    eventIndex = 4;
      break;
  case VLAN_DELETE_PORT_NOTIFY:
    eventIndex = 5;
      break;
  case VLAN_START_TAGGING_PORT_NOTIFY:
    eventIndex = 6;
      break;
  case VLAN_STOP_TAGGING_PORT_NOTIFY:
    eventIndex = 7;
      break;
  case VLAN_INITIALIZED_NOTIFY:
    eventIndex = 8;
      break;
  case VLAN_RESTORE_NOTIFY:
    eventIndex = 9;
      break;
  case VLAN_PVID_CHANGE_NOTIFY:
    eventIndex = 10;
      break;
    default:
      eventIndex = 0;
      break;
  }

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

    /* do not notify FDB during init */
    if ( (vlanRequestNotifyList[i].registrarID == L7_FDB_COMPONENT_ID) &&
         (dot1qInfo->initialized != L7_TRUE) )
    {
      rc = L7_SUCCESS;
    }

    else if (vlanRequestNotifyList[i].registrarID &&
             (vlanRequestNotifyList[i].eventMask & event))
    {
      callTime = osapiTimeMillisecondsGet();
      rc = (vlanRequestNotifyList[i].notify_request(vlanData, intIfNum, event));
      if (rc != L7_SUCCESS)
      {
        if (cnfgrApiComponentNameGet(i, name) != L7_SUCCESS)
        {
          osapiStrncpySafe(name, "Unknown", 8);
        }

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
                "Bad rc %d in vlanNotifyRegisteredUsers for registrar %d, %s\n", (int)rc, (int)i, name);
      }
      callTime = osapiTimeMillisecondsGet() - callTime;
      vlanEventTime[eventIndex][i] = vlanEventTime[eventIndex][i]+callTime;
    }
  }
} /* vlanNotifyRegisteredUsers */

/*********************************************************************
*
* @purpose  Determine if the port is eligible to participate in dot1q
*
* @param intIfNum     internal interface number
*
* @return   L7_SUCCESS if the interface is valid
* @return   L7_FAILURE, otherwise
*
* @end
*
*********************************************************************/
L7_RC_t dot1qVlanIntfValidate(L7_uint32 intIfNum)
{
    L7_INTF_TYPES_t sysIntfType;
    L7_RC_t rc;

    rc = L7_FAILURE;

    if ( nimGetIntfType(intIfNum,&sysIntfType) == L7_SUCCESS)
    {
        switch (sysIntfType)
        {
        case L7_PHYSICAL_INTF:
            rc = L7_SUCCESS;
            break;
        case L7_LAG_INTF:
            if (dot3adIsLagConfigured(intIfNum) == L7_FALSE)
                {
                  break;
                }
            rc = L7_SUCCESS;
            break;
        default:
            break;
        }
    }
    return rc;
}

/*********************************************************************
* @purpose  Notify COS component of change in port default priority
*           and/or traffic class
*
* @param    intIfNum                  internal interface number
* @param    portDefaultPriority       port default priority
* @param    portDefaultTrafficClass   port default traffic class
*
* @return   void
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/

void dot1qPortDefaultPriorityUpdateNotification(L7_uint32 intIfNum,
                                                L7_uint32 portDefaultPriority,
                                                L7_uint32 portDefaultTrafficClass)
{
#ifdef L7_QOS_FLEX_PACKAGE_COS
  cosMapPortDefaultPriorityUpdate(intIfNum, portDefaultPriority, portDefaultTrafficClass);
#endif
}

/*********************************************************************
* @purpose  Notify COS component of change in operational number of
*           traffic classes
*
* @param    intIfNum            internal interface number
* @param    numTrafficClasses   number of traffic classes
*
* @return   void
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
void dot1qPortNumTrafficClassesUpdateNotification(L7_uint32 intIfNum,
                                                  L7_uint32 numTrafficClasses)
{
#ifdef L7_QOS_FLEX_PACKAGE_COS
  cosMapNumTrafficClassesUpdate(intIfNum, numTrafficClasses);
#endif
}

/*********************************************************************
* @purpose  Notify COS component of change in 802.1p priority to
*           traffic class mapping
*
* @param    intIfNum      internal interface number
* @param    priority      traffic class priority
* @param    trafficClass  In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @return   void
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
void dot1qPortTrafficClassUpdateNotification(L7_uint32 intIfNum,
                                             L7_uint32 priority,
                                             L7_uint32 trafficClass)
{
#ifdef L7_QOS_FLEX_PACKAGE_COS
  {
    L7_uint32 portDefaultPriority;

    /* COS is only interested when the port default priority traffic class changes */
    portDefaultPriority = dot1dPortDefaultUserPriorityGet(intIfNum);
    if (priority == portDefaultPriority)
      dot1qPortDefaultPriorityUpdateNotification(intIfNum, portDefaultPriority, trafficClass);
  }
#endif
}

/*********************************************************************
* @purpose  Check if 802.1p user priority mapping is active
*
* @param    intIfNum    internal interface number
*
* @return   L7_TRUE     802.1p mapping active, or COS component not available
* @return   L7_FALSE    802.1p mapping currently inactive
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*
* @end
*********************************************************************/
L7_BOOL dot1qDot1pMappingIsActive(L7_uint32 intIfNum)
{
  L7_BOOL       rc = L7_TRUE;
#ifdef L7_QOS_FLEX_PACKAGE_COS
  rc = cosMapDot1pMappingIsActive(intIfNum);
#endif
  return rc;
}
extern L7_char8 *nimDebugCompStringGet(L7_COMPONENT_IDS_t cid);


L7_uchar8* dot1qDebugVlanEventName(L7_uint32 eventIndex)
{

  switch (eventIndex)
  {
    case 1:
     return "VLAN_ADD_NOTIFY";
      break;
    case 2:
      return "VLAN_DELETE_PENDING_NOTIFY";
      break;
    case 3:
       return "VLAN_DELETE_NOTIFY";
       break;
    case 4:
      return "VLAN_ADD_PORT_NOTIFY";
      break;
    case 5:
      return "VLAN_DELETE_PORT_NOTIFY";
      break;
    case 6:
      return "VLAN_START_TAGGING_PORT_NOTIFY";
      break;
    case 7:
      return "VLAN_STOP_TAGGING_PORT_NOTIFY";
      break;
    case 8:
      return "VLAN_INITIALIZED_NOTIFY";
      break;
    case 9:
      return "VLAN_RESTORE_NOTIFY";
      break;
    case 10:
      return "VLAN_PVID_CHANGE_NOTIFY";
      break;
    default:
      return "N/A";
      break;
  }
}
void dot1qDebugVlanChangeEvent(L7_uint32 eventIndex)
{
  L7_uint32 cid;
  L7_uint32 event = 0;

  if (eventIndex >= 11)
  {
    printf("Event Index has to be between 0 and %d inclusive.\n", 11-1);
    return;
  }

  if (eventIndex == 0)
  {
    for (event = 1; event < 11; event++)
    {
      printf("\nVlan Event %s\n", dot1qDebugVlanEventName(event));
      for (cid = 1; cid < L7_LAST_COMPONENT_ID; cid++)
      {
        if (vlanRequestNotifyList[cid].registrarID == cid)
        {
          printf("%s took %d msec\n", nimDebugCompStringGet(cid), vlanEventTime[event][cid]);
        }
      }
    }
  }
  else
  {
    printf("\nVlan Event %s\n", dot1qDebugVlanEventName(eventIndex));
    for (cid = 1; cid < L7_LAST_COMPONENT_ID; cid++)
    {
      if (vlanRequestNotifyList[cid].registrarID == cid)
      {
        printf("%s took %d msec\n", nimDebugCompStringGet(cid), vlanEventTime[eventIndex][cid]);
      }
    }
  }
}
