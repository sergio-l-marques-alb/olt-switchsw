/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename dot1q_qports.c
*
* @purpose   Manage Dot1q Port Parameters
*
* @component dot1q
*
* @comments 
*
* @create    06/22/2005
*
* @author    akulkarni
*
* @end
*             
**********************************************************************/
#include "flex.h"
#include "dot1q_common.h"
#include "osapi.h"
#include "dtlapi.h"
#include "nimapi.h"
#include "defaultconfig.h"
#include "dot1q_data.h"
#include "dot1q_cfg.h"
#include "dot1q_control.h"
#include "dot1q_outcalls.h"
#include "dot1q_debug.h"
#include "dot1q.h"
#include "dot1q_qports.h"


extern  dot1qIntfData_t    *dot1qQports;
extern  dot1qCfgData_t     *dot1qCfg;
extern  dot1qInfo_t        *dot1qInfo;

/*********************************************************************
* @purpose  Set dot1d port default user priority
*
* @param    intIfNum   internal interface number
* @param   priority    default user priority, in range of 0-7
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortDefaultUserPriorityProcess(L7_uint32 intIfNum, L7_uint32 priority)
{
    L7_uint32 trafficClass;


    /* only update interfaces that are settable */
  if (dot1qIntfIsSettable(intIfNum) == L7_TRUE)
  {
    if (dtlDot1dDefaultPortPrioritySet(intIfNum,priority) != L7_SUCCESS)
    {
      /* log a message to indicate an apply failure, but keep going */
      LOG_MSG("Unable to set dot1d default priority (%u) on intIfNum %u\n",
              priority, intIfNum);
    }
  }

  /* make outcall notifications when global value is updated */
  if (dot1dPortTrafficClassGet(intIfNum, priority, &trafficClass) == L7_SUCCESS)
  {
    dot1qPortDefaultPriorityUpdateNotification(intIfNum, priority, trafficClass);
  }

  return(L7_SUCCESS);
}   



/*********************************************************************
* @purpose  Set dot1d port default user priority globally for all interfaces
*
* @param    priority    default user priority, in range of 0-7
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortDefaultUserPriorityGlobalProcess(L7_uint32 priority)
{
    L7_uint32 i;
    L7_uint32 intIfNum;
    L7_uint32 trafficClass;

  for (i = 1; i < DOT1Q_INTF_MAX_COUNT; i++)
  {

    /* Note that only valid interfaces have an entry in the  config file, so
      there is no need to do a dot1qIntfIsValid check */
    if (nimIntIfFromConfigIDGet(&(dot1qCfg->Qports[i].configId), &intIfNum) != L7_SUCCESS)
        continue;

    if (dot1dPortDefaultUserPriorityProcess(intIfNum, priority) != L7_SUCCESS)
    {
      /* log a message to indicate an apply failure, but keep going */
      LOG_MSG("Unable to set global dot1d default priority (%u) on intIfNum %u\n",
              priority, intIfNum);
    }
  }


  /* make outcall notifications when global value is updated */
  if (dot1dPortTrafficClassGet(L7_ALL_INTERFACES, priority, &trafficClass) == L7_SUCCESS)
  {
    dot1qPortDefaultPriorityUpdateNotification(L7_ALL_INTERFACES, priority, trafficClass);
  }

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Set dot1d number of traffic classes globally for all interfaces
*
* @param    numTrafficClasses   number of traffic classes supported, in range of 1-8
*
* @returns  L7_SUCCESS
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortNumTrafficClassesGlobalProcess(L7_uint32 numTrafficClasses)
{
   L7_uint32 i;
   L7_uint32 intIfNum;
   L7_RC_t rc;

    for (i = 1; i < DOT1Q_INTF_MAX_COUNT; i++)
    {

      /* Note that only valid interfaces have an entry in the  config file, so
        there is no need to do a dot1qIntfIsValid check */
      if (nimIntIfFromConfigIDGet(&(dot1qCfg->Qports[i].configId), &intIfNum) != L7_SUCCESS)
          continue;

      rc = dot1dPortNumTrafficClassesProcess(intIfNum, numTrafficClasses);
      if (rc != L7_SUCCESS)
      {
        /* log a message to indicate an apply failure, but keep going */
        LOG_MSG("Unable to set global dot1d number of traffic classes (%u) on intIfNum %u\n",
                numTrafficClasses, intIfNum);
      }
    }

    rc = dot1qQportNumTrafficClassesUpdate(L7_ALL_INTERFACES, numTrafficClasses);
    if (rc == L7_SUCCESS)
    {

      /* make outcall notifications when global value is updated */
      dot1qPortNumTrafficClassesUpdateNotification(L7_ALL_INTERFACES, numTrafficClasses);
    }

    return(rc);
}


/*********************************************************************
* @purpose  Set dot1d number of traffic classes
*
* @param    intIfNum            internal interface number
* @param    numTrafficClasses   number of traffic classes supported, in range of 1-8
*
* @returns  L7_SUCCESS
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortNumTrafficClassesProcess(L7_uint32 intIfNum, L7_uint32 numTrafficClasses)
{
  L7_RC_t rc;
  L7_uint32 j;
  L7_BOOL applyFailed = L7_FALSE;
  dot1qIntfCfgData_t *pCfg;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;                  /* improper usage */

  if ((numTrafficClasses > L7_DOT1P_NUM_TRAFFIC_CLASSES) || 
      (numTrafficClasses < L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT))
  {
    return L7_FAILURE;
  }

  /* NOTE:  When applying the initial configuration data, the caller is
   *        required to have set the operational priority.NumTrafficClasses
   *        field equal to the number of traffic classes being set so that
   *        the desired action occurs here.
   */
  if (numTrafficClasses != dot1dPortNumTrafficClassesGet(intIfNum))
  {
    /* Regenerate new dot1p mapping */
    rc = dot1qQportNumTrafficClassesUpdate(intIfNum, numTrafficClasses);
  }
  else
  {
    if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
       return L7_FAILURE;

    /* Use existing dot1p mapping. */
    memcpy(dot1qQports[intIfNum].priority.Mapping, pCfg->portCfg.priority.Mapping,
           sizeof(dot1qQports[intIfNum].priority.Mapping));
    rc = L7_SUCCESS;
  }

  if (rc == L7_SUCCESS)
  {
    /* set the operational NumTrafficClasses value before calling the 
     * dot1dPortTrafficClassSet() function for each priority level
     * (this value is referenced during the call)
     */
    dot1qQports[intIfNum].priority.NumTrafficClasses = numTrafficClasses;

    /* Set the new priority/traffic class map in the driver.
    */
    for (j = 0; j < (L7_DOT1P_MAX_PRIORITY+1); j++) 
    {
      if (dot1dPortTrafficClassProcess(intIfNum, j, dot1qQports[intIfNum].priority.Mapping[j]) != L7_SUCCESS)
        applyFailed = L7_TRUE;
    }

    if (applyFailed == L7_TRUE)
    {
      /* log a message to indicate an apply failure, but keep going */
      LOG_MSG("Unable to set one or more dot1d port priority mappings on intIfNum %u\n",
              intIfNum);
    }

    /* make outcall notifications when per-interface value is updated */
    dot1qPortNumTrafficClassesUpdateNotification(intIfNum, numTrafficClasses);
  }

  return rc;
}



/*********************************************************************
* @purpose  Set dot1d traffic class 
*
* @param    intIfNum   internal interface number
* @param    priority   traffic class priority
* @param    traffic class       In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t dot1dPortTrafficClassProcess(L7_uint32 intIfNum, L7_uint32 priority, 
                                     L7_uint32 trafficClass)
{
  L7_uint32 numTrafficClasses;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;                  /* improper usage */

  if (priority > L7_DOT1P_MAX_PRIORITY)
    return L7_FAILURE;

  DOT1Q_DEBUG_MSG("dot1qProcessPortTrafficClassSet : intIfNum %d priority %d trafficClass %d \n",
				  intIfNum, priority, trafficClass);

  numTrafficClasses = dot1dPortNumTrafficClassesGet(intIfNum);

  /* Note: numTrafficClasses is 0 for interfaces that NIM thinks are valid,
   *       but which do not get created (e.g. CPU interface).
   */
  if ((nimCheckIfNumber(intIfNum) == L7_SUCCESS) &&
      (numTrafficClasses > 0))
  {
    if (trafficClass > (numTrafficClasses - 1))
      return L7_FAILURE;

    /* only update interfaces that are settable and trusting 802.1p priority */
    if (dot1qIntfIsSettable(intIfNum) == L7_TRUE)
    {
      if (dot1qDot1pMappingIsActive(intIfNum) == L7_TRUE)
      {
        if (dtlDot1dTrafficClassSet(intIfNum, priority, trafficClass) != L7_SUCCESS)
        {
          /* log a message to indicate an apply failure, but keep going */
          LOG_MSG("Unable to set dot1d priority %u to traffic class %u on intIfNum %u\n",
                  priority, trafficClass, intIfNum);
        }
      }
    }


    /* make outcall notifications when per-interface value is updated */
    dot1qPortTrafficClassUpdateNotification(intIfNum, priority, trafficClass);
  }
  
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Set dot1d traffic class globally for all interfaces 
*
* @param    priority      traffic class priority
* @param    traffic class In range of 0-(dot1dPortNumTrafficClasses-1)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dPortTrafficClassGlobalProcess(L7_uint32 priority, L7_uint32 trafficClass)
{
  L7_uint32 numTrafficClasses;
  L7_uint32 i;
  L7_uint32 intIfNum;

  if (priority > L7_DOT1P_MAX_PRIORITY)
    return L7_FAILURE;

  numTrafficClasses = dot1dPortNumTrafficClassesGet(L7_ALL_INTERFACES);
  if (trafficClass > (numTrafficClasses - 1))
    return L7_FAILURE;

  for (i = 1; i < DOT1Q_INTF_MAX_COUNT; i++)
  {

    /* Note that only valid interfaces have an entry in the  config file, so
      there is no need to do a dot1qIntfIsValid check */
    if (nimIntIfFromConfigIDGet(&(dot1qCfg->Qports[i].configId), &intIfNum) != L7_SUCCESS)
        continue;

    if (dot1dPortTrafficClassProcess(intIfNum, priority, trafficClass) != L7_SUCCESS)
    {
      /* log a message to indicate an apply failure, but keep going */
      LOG_MSG("Unable to set global dot1d priority %u to traffic class %u on intIfNum %u\n",
              priority, trafficClass, intIfNum);
    }
  }

  /* make outcall notifications when per-interface value is updated */
  dot1qPortTrafficClassUpdateNotification(L7_ALL_INTERFACES, priority, trafficClass);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Set the traffic class capability
*
* @param    status    status value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A status of L7_TRUE indicates more than one priority level
*           is enabled, while L7_FALSE indicates a single priority level
*           is enabled.
*
* @end
*********************************************************************/
L7_RC_t dot1dTrafficClassesEnabledProcess(L7_BOOL status)
{
  dot1qInfo->TrafficClassesEnabled = (L7_uint32)status;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Set the Ingress Filtering configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    status      L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsIngressFilterProcess(L7_uint32 intIfNum, L7_uint32 status)
{
  L7_uint32 i,start,end;
  L7_RC_t rc;
  dot1qIntfCfgData_t *pCfg;
  DOT1Q_SWPORT_MODE_t currSwPortMode;

  if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
  {
    return L7_NOT_EXISTS;
  }

  if (intIfNum == L7_ALL_INTERFACES)
  {
    start = 1;
    end = intIfNum;
  }
  else
  {
    start = intIfNum;
    end = intIfNum+1;
  }

  DOT1Q_TRACE("%s(%d) intf %d  value %d \n", __FUNCTION__, __LINE__, intIfNum, status);
  for (i = start; i < end; i++)
  {
      if (dot1qMapIntfIsConfigurable(i, &pCfg) != L7_TRUE)
      {

          LOG_MSG("failure getting pCfg for intIfNum %d\n", i);
          continue;

      }

      currSwPortMode = dot1qOperSwitchPortModeGet(i);

      if ((currSwPortMode == DOT1Q_SWPORT_MODE_ACCESS) || 
           (currSwPortMode == DOT1Q_SWPORT_MODE_TRUNK))
      {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                 "Vlan acceptable fram type can only be configured for General port mode %d",
                 i);
          continue;
      }


      /* set the operational ingress filter */
      dot1qQports[i].ingressFilter = status;

      rc = dtlDot1qPortConfig(i,
                              DTL_L2_DOT1Q_PORT_CONFIG_INGRESS_FILTER,
                              pCfg->portCfg.VID[DOT1Q_DEFAULT_PVID],
                              pCfg->portCfg.acceptFrameType,
                              status);

      if (rc != L7_SUCCESS)
      {
          LOG_MSG("dtl failure while modifying ingress filter value to %d for intIfNum %d\n",
                  status, i); 
      }
  }


  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Set the VID configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    vid         port vlan ID
* @param    vidType     L7_DOT1Q_DEFAULT_VID_INDEX, L7_DOT1Q_IP_UNUSED_VID, L7_DOT1Q_IPX_UNUSED_VID,
*                       L7_DOT1Q_NETBIOS_UNUSED_VID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist(configuration is applied)
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t   dot1qQportsVIDProcess(L7_uint32 intIfNum, L7_uint32 vid, 
                                L7_uint32 vid_type,L7_uint32 requestor)
{
  L7_uint32 i,start,end;
  L7_RC_t rc = L7_SUCCESS;
  dot1qIntfCfgData_t *pCfg;
  L7_uint32 result;
  L7_uint32 org_vid;
  DOT1Q_SWPORT_MODE_t currSwPortMode;
  dot1qNotifyData_t vlanData;

  vlanData.numVlans = 1;

  if ((vid < L7_DOT1Q_MIN_VLAN_ID) || (vid > L7_DOT1Q_MAX_VLAN_ID))
  {
    return L7_FAILURE;
  }
  
  if (intIfNum == L7_ALL_INTERFACES)
  {
    start = 1;
    end = intIfNum;
  }
  else
  {
    start = intIfNum;
    end = intIfNum+1;
  }

  DOT1Q_TRACE("%s(%d) intf %d vid %d \n", __FUNCTION__, __LINE__, intIfNum, vid);
  for (i=start;i<end;i++)
  {
      if (dot1qMapIntfIsConfigurable(i, &pCfg) != L7_TRUE)
      {

          LOG_MSG("failure getting pCfg for intIfNum %d\n", i);
          continue;

      }
      org_vid = dot1qQports[i].pvid[vid_type];

      if (org_vid == vid)
      {
        /* Nothing to do. continue.*/
        continue;
      }

      currSwPortMode = dot1qOperSwitchPortModeGet(i);

      if ((currSwPortMode == DOT1Q_SWPORT_MODE_ACCESS) || 
           (currSwPortMode == DOT1Q_SWPORT_MODE_TRUNK))
      {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                 "Vlan PVID can only be configured for General port mode %d",
                 i);
          continue;
      }


      /* check if interface has been accquired by DOT1X*/
      /* Do not apply changes to the hardware ,if the port has been accquired by DOT1X , unless DOT1X is the requestor */  
      result = 0;
      COMPONENT_MASKNUMBITSETGET(dot1qQports[i].acquiredList,result);
      if ((COMPONENT_ISMASKBITSET(dot1qQports[i].acquiredList,L7_DOT1X_COMPONENT_ID)==0) || 
           ((COMPONENT_ISMASKBITSET(dot1qQports[i].acquiredList,L7_DOT1X_COMPONENT_ID)!=0) && (requestor == DOT1Q_DOT1X )&& (result == 1))) 
      {
          /* set the operational pvid for the port */
          vlanData.data.vlanId = org_vid;
          dot1qQports[i].pvid[vid_type] =vid;
    
          rc = dtlDot1qPortConfig(i,
                                  DTL_L2_DOT1Q_PORT_CONFIG_VID_SET,
                                  vid,
                                  pCfg->portCfg.acceptFrameType,
                                  pCfg->portCfg.enableIngressFiltering);
    
          if (rc != L7_SUCCESS) 
          {
              LOG_MSG("dtl failure while modifying pvid configuration to %d for intIfNum %d and vid_type %d\n",
                      vid, i, vid_type); 
          }

          /* Notify registered components of PVID change for the port  */
          if (rc==L7_SUCCESS) 
          {
            vlanNotifyRegisteredUsers(&vlanData,i,VLAN_PVID_CHANGE_NOTIFY);
          }
      }
  }


  return(rc);
}


/*********************************************************************
* @purpose  Set the Acceptable Frame Type configuration for a port
*
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    type        L7_DOT1Q_ADMIT_ALL, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsAcceptFrameTypeProcess(L7_uint32 intIfNum, L7_uint32 type)
{
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 start,end;
  dot1qIntfCfgData_t *pCfg;
  DOT1Q_SWPORT_MODE_t currSwPortMode;

  if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
  {
    return L7_NOT_EXISTS;
  }

  if (intIfNum == L7_ALL_INTERFACES)
  {
    start = 1;
    end = intIfNum;
  }
  else
  {
    start = intIfNum;
    end = intIfNum+1;
  }

  DOT1Q_TRACE("%s(%d) intf %d frameType %d \n", __FUNCTION__, __LINE__, intIfNum, type);
  for (i = start; i < end; i++)
  {
      if (dot1qMapIntfIsConfigurable(i, &pCfg) != L7_TRUE)
      {

          LOG_MSG("failure getting pCfg for intIfNum %d\n", i);
          continue;

      }

      currSwPortMode = dot1qOperSwitchPortModeGet(i);

      if ((currSwPortMode == DOT1Q_SWPORT_MODE_ACCESS) || 
           (currSwPortMode == DOT1Q_SWPORT_MODE_TRUNK))
      {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                 "Vlan acceptable fram type can only be configured for General port mode %d",
                 i);
          continue;
      }


      rc = dtlDot1qPortConfig(i,
                              DTL_L2_DOT1Q_PORT_CONFIG_ACCEPT_FRAME_TYPE,
                              pCfg->portCfg.VID[DOT1Q_DEFAULT_PVID],
                              type,
                              pCfg->portCfg.enableIngressFiltering);

      if (rc != L7_SUCCESS)
      {
          LOG_MSG("dtl failure while modifying accpet frame type to %d for intIfNum %d\n",
                  type, intIfNum); 
      }
  }


  return(rc);
}

/*********************************************************************
* @purpose  Sets the pvid, acceptable frame type and ingress filtering to the given value in a single dtl call 
*
* @param    intIfNum             Interface for which to set the values
* @param    vid                  pvid to be set for the port
* @param    acceptFrameType      as admit all , admit tagged only or untagged only
* @param    ingressFiltering     ingressFiltering enabled or disabled
*
* @returns  L7_SUCCESS
*
* @notes    The code block below is the equivalent of invoking each of the following routines
       individually.
       
        dot1qQportsVIDProcess()
        dot1qQportsAcceptFrameTypeProcess()
        dot1qQportsIngressFilterProcess()
    
        Here, the individual dot1q process calls for these routines are purposely being
        avoided.  This allows for the reduction of DTL/driver calls by at least 2/3 
*
* @end
*********************************************************************/
L7_RC_t dot1qQportsVlanParamSet(L7_uint32 intIfNum,L7_uint32 vid,
                                L7_uchar8 acceptFrameType, 
                                L7_uchar8 ingressFiltering)
{

    L7_uint32 portCfgFlag = 0;
    L7_RC_t rc;

    if ((vid < L7_DOT1Q_MIN_VLAN_ID) || (vid > L7_DOT1Q_MAX_VLAN_ID))
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
               "Vlan not valid %d", vid);
        return L7_FAILURE;
    }

    DOT1Q_TRACE("%s(%d) intf %d \n", __FUNCTION__, __LINE__, intIfNum);
    DOT1Q_TRACE("Port(%d) Setting PVID %d frame type %d Ingres filtering %d\n",
            intIfNum, vid, acceptFrameType, ingressFiltering);
    /* 
    dot1qQports[intIfNum].acceptFrameType = acceptFrameType; */
    dot1qQports[intIfNum].ingressFilter = ingressFiltering;
    dot1qQports[intIfNum].pvid[DOT1Q_DEFAULT_PVID] = vid;

    portCfgFlag |=  DTL_L2_DOT1Q_PORT_CONFIG_ACCEPT_FRAME_TYPE;
    portCfgFlag |=  DTL_L2_DOT1Q_PORT_CONFIG_VID_SET;
    portCfgFlag |=  DTL_L2_DOT1Q_PORT_CONFIG_INGRESS_FILTER;


    rc = dtlDot1qPortConfig(intIfNum, portCfgFlag,
                            vid,
                            acceptFrameType,
                            ingressFiltering);
    if (rc != L7_SUCCESS) 
    {
       LOG_MSG("dtl failed with err(%d) applying port cfg: intIfNum %d:"
               "  pvid = %d,acceptFrameType = %d, ingressFiltering = %d\n",
               rc, intIfNum, vid, acceptFrameType, ingressFiltering);
    }


    return rc;

}

/*********************************************************************
* @purpose  Return the operational Value of the switch port mode for an interface
*
* @param    intIfNum   internal interface number
*
* @returns  DOT1Q_SWPORT_MODE_NONE on failure
* @returns  switchport mode value on success
*
* @notes     
*
* @end
*********************************************************************/
DOT1Q_SWPORT_MODE_t dot1qOperSwitchPortModeGet(L7_uint32 intIfNum)
{

  if (intIfNum <= L7_MAX_INTERFACE_COUNT)
      return dot1qQports[intIfNum].switchport_mode;

  return  DOT1Q_SWPORT_MODE_NONE;  
   
  
}

/*********************************************************************
* @purpose  Return the operational Value of the Access Vlan 
*
* @param    intIfNum   internal interface number
*
* @returns  L7_DOT1Q_NULL_VLAN_ID on failure
* @returns  iaccess vlan value on success
*
* @notes  Only valid if the switchport mode is access. The caller needs
*         ensure that this value is used only in case it is access mode
*
* @end
*********************************************************************/
L7_uint32 dot1qOperSwitchPortAccessVlanGet(L7_uint32 intIfNum)
{

  if (intIfNum <= L7_MAX_INTERFACE_COUNT)
      return dot1qQports[intIfNum].access_vlan;

  return  L7_DOT1Q_NULL_VLAN_ID;  
   
  
}
/*********************************************************************
* @purpose  Return the operational Value of the switch port mode for an interface
*
* @param    intIfNum   internal interface number
*
* @returns  DOT1Q_SWPORT_MODE_NONE on failure
* @returns  switchport mode value on success
*
* @notes     
*
* @end
*********************************************************************/
L7_BOOL dot1qOperSwitchPortVlanConfigAllowed(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 mode)
{

  if (intIfNum > L7_MAX_INTERFACE_COUNT)
    return L7_FALSE;

  if(dot1qOperSwitchPortModeGet(intIfNum) == DOT1Q_SWPORT_MODE_ACCESS)
  {
    if (dot1qOperSwitchPortAccessVlanGet(intIfNum) == vlanId)
    {
       return (mode == L7_DOT1Q_FIXED);
    }
    return (mode != L7_DOT1Q_FIXED);
  }

  return  L7_TRUE;  
}

