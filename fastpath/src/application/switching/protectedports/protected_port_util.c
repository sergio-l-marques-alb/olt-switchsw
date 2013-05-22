/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_util.c
*
* @purpose   Protected Port util file
*
* @component protectedPort
*
* @comments
*
* @create    6/6/2005
*
* @author    ryadagiri
*
* @end
*
**********************************************************************/

#include "protected_port_include.h"
#include "mirror_api.h"
#include "dot3ad_api.h"
#include "l7_ip_api.h"

PORTEVENT_MASK_t protectedPortEventMask_g;

/*********************************************************************
* @purpose  Checks if the intf is attached.
*
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL protectedPortIntfIsAttached(L7_uint32 intIfNum)
{
  L7_NIM_QUERY_DATA_t       nimQueryData;

  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.request  = L7_NIM_QRY_RQST_STATE;

  nimQueryData.intIfNum = intIfNum;
  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* should never get here */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PROTECTED_PORT_COMPONENT_ID,
              " nimIntfQuery failed for intf %s\n", ifName);
      return L7_FALSE;
  }
  if (nimQueryData.data.state == L7_INTF_ATTACHED ||
      nimQueryData.data.state == L7_INTF_ATTACHING ||
      nimQueryData.data.state == L7_INTF_DETACHING)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Checks if the intf is acquired by mirroring, dot3ad, or
*           routing.
*
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL protectedPortIntfIsAcquired(L7_uint32 intIfNum)
{
  L7_uint32 routingMode;

  if (ipMapRtrIntfModeGet(intIfNum, &routingMode) != L7_SUCCESS)
  {
    routingMode = L7_DISABLE;
  }

  if ((mirrorIsActiveProbePort(intIfNum)) ||
      (dot3adIsLagActiveMember(intIfNum)) ||
      (routingMode == L7_ENABLE))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Checks if the intf config can be applied to HW.
*           Specifically, the intf needs to be attached and
*           not acquired by mirroring, dot3ad, or routing.
*
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL protectedPortIntfConfigCanApplyToHw(L7_uint32 intIfNum)
{
  return ((protectedPortIntfIsAttached(intIfNum)) && (!protectedPortIntfIsAcquired(intIfNum)));
}

/*********************************************************************
* @purpose  Add an interface to a Protected Port group
*
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfAddApply(L7_uint32 groupId, L7_uint32 intIfNum)
{
  L7_INTF_MASK_t tempMask;
  L7_RC_t rc;

  rc = L7_SUCCESS;

  memcpy(&tempMask,
         &protectedPortCfg->protectedPortGroup[groupId].protectedPortPortMask,
         L7_INTF_INDICES);

  L7_INTF_SETMASKBIT(tempMask, intIfNum);

  rc = protectedPortGroupCreateApply(groupId, tempMask);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Cannot add intf %s to group %d. "
            "This appears when an interface could not be added to a particular group.\n", ifName, groupId);
  }
  return(rc);
}



/*********************************************************************
* @purpose  Add a Protected Port group
*
* @param    groupId      @b{(input)}    group identifier
* @param    intfMask     @b{(input)}    an interface mask defined by L7_INTF_MASK_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupCreateApply(L7_uint32 groupId, L7_INTF_MASK_t intfMask)
{
    L7_uint32      isNotEmpty = L7_TRUE;
    L7_RC_t        rc = L7_FAILURE;

    /* If there are no ports in the protected group, don't pass
    ** the configuration to the driver.
    */
    L7_INTF_NONZEROMASK(intfMask, isNotEmpty);
    if (isNotEmpty == L7_FALSE)
    {
       rc = L7_SUCCESS;
       return rc;
    }

    protectedPortIntfMaskChange(&intfMask);
    L7_INTF_NONZEROMASK(intfMask, isNotEmpty);
    if (isNotEmpty == L7_FALSE)
    {
      rc = L7_SUCCESS;
      return rc;
    }


    /* Add the entire group at once */
    rc = dtlProtectedPortGroupCreate(groupId, intfMask);

    if (rc != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "unable to set protected port group %d. "
                        "This appears when a dtl call fails to add interface mask at the driver level\n", groupId);
        rc = L7_FAILURE;
    }

    return(rc);
}


/*********************************************************************
* @purpose  Delete an interface from a Protected Port group
*
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfDeleteApply(L7_uint32 groupId, L7_uint32 intIfNum)
{
  L7_INTF_MASK_t tempMask;
  L7_uint32      isNotEmpty;
  L7_RC_t rc;

  rc = L7_FAILURE;

  memcpy( &tempMask,
          &protectedPortCfg->protectedPortGroup[groupId].protectedPortPortMask,
          L7_INTF_INDICES);

  rc = dtlProtectedPortDelete(groupId, intIfNum);
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Cannot delete intf %s from group %d. "
            "This appears when a dtl call to delete an interface from a group fails\n", ifName, groupId);
  }

  L7_INTF_CLRMASKBIT( tempMask, intIfNum );

  /*
   * If there are any ports remaining in the mask, update the group.
   */

  L7_INTF_NONZEROMASK(tempMask, isNotEmpty);

  if ( isNotEmpty )
  {
    rc = protectedPortGroupCreateApply(groupId, tempMask);
    if (rc != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Cannot update group %d after deleting"
              " interface %s. This message appears when an update group for a interface deletion"
              " fails.\n", groupId, ifName);
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a Protected Port group
*
* @param    groupId      @b{(input)}    group identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupDeleteApply(L7_uint32 groupId)
{
  L7_INTF_MASK_t *tempMask;
  int             i;

  tempMask = &protectedPortCfg->protectedPortGroup[groupId].protectedPortPortMask;

  for ( i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    if ( L7_INTF_ISMASKBITSET( *tempMask, i ) )
    {
      if ( dtlProtectedPortDelete(groupId, i) != L7_SUCCESS )
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(i, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PROTECTED_PORT_COMPONENT_ID,
                "unable to delete protected port %d, %s in group %d\n", i, ifName, groupId);
      }
    }
}

  return( L7_SUCCESS );
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    groupId  @b{(input)} Group Id
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to protected port **group** config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.


* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
*
* @end
*********************************************************************/
L7_BOOL protectedPortIntfIsConfigurable(L7_uint32 groupId, L7_uint32 intIfNum,
                                        protectedPortGroup_t **pCfg)
{
    L7_int32 i;
    L7_int32 cfgIndex = -1;
    L7_uint32 maskOffset;

    if (!(PROTECTED_PORT_IS_READY))
  {
        return L7_FALSE;
  }

    /* Check boundary conditions */
    if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
  {
        return L7_FALSE;
  }

    if (nimConfigIdMaskOffsetGet(intIfNum, &maskOffset) != L7_SUCCESS)
    {
        return(L7_FALSE);
    }

    for (i = 0; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
    {
        if (L7_INTF_ISMASKBITSET(protectedPortCfg->protectedPortGroup[i].
                                 protectedPortPortMask, maskOffset))
        {
            cfgIndex = i;
            break;
        }
    }

    if (cfgIndex == -1 && i == L7_PROTECTED_PORT_MAX_GROUPS)
  {
        cfgIndex = groupId;
  }

    *pCfg = &protectedPortCfg->protectedPortGroup[cfgIndex];
    return L7_TRUE;
}

/*********************************************************************
* @purpose  NIM interface change callback processing
*
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    event      @b{(input)} Event to be processes
* @param    correlator @b{(input)} correlator
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t protectedPortIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intfEvent,
                                        NIM_CORRELATOR_t correlator)
{
    L7_uint32 i;
    L7_uint32 groupId = 0;
    L7_RC_t rc = L7_FAILURE;
    NIM_EVENT_COMPLETE_INFO_t status;

    status.intIfNum = intIfNum;
    status.component = L7_PROTECTED_PORT_COMPONENT_ID;
    status.event = intfEvent;
    status.correlator = correlator;
    status.response.reason = NIM_ERR_RC_UNUSED;

    if (PROTECTED_PORT_IS_READY == L7_FALSE)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Received an interface change callback while "
                      "not ready to receive it. This appears when an interface change call back has come before the "
                      "protected port component is ready.");
        rc = L7_SUCCESS;
        status.response.rc = rc;
        nimEventStatusCallback(status);
        return rc;
    }

   /* Check for valid parameters */
    if (protectedPortIntfIsValid(intIfNum) == L7_FAILURE)
    {
        rc = L7_SUCCESS;
        status.response.rc = rc;
        nimEventStatusCallback(status);
        return rc;
    }
    switch (intfEvent)
    {
    case L7_CREATE:

        /* Since this feature only deals with interface masks for config,
           there is no need to do the typical creation and allocation of an
           interface config structure. */
        rc = L7_SUCCESS;
        break;

    case L7_ATTACH:
    case L7_LAG_RELEASE:
    case L7_PROBE_TEARDOWN:
    case L7_PORT_ROUTING_DISABLED:
        /* Simply loop through the groups and determine if the interface
           is in a group. Then invoke the apply routine */
        if (protectedPortIntfConfigCanApplyToHw(intIfNum))
        {
          osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);
          for (i = 0; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
          {
              if (L7_INTF_ISMASKBITSET(protectedPortCfg->protectedPortGroup[i].
                                       protectedPortPortMask, intIfNum))
              {
                  groupId = i;
                  break;
              }
          }
          if (i != L7_PROTECTED_PORT_MAX_GROUPS)
          {
            (void) protectedPortGroupIntfAddApply(groupId, intIfNum);
          }

          osapiReadLockGive(protectedPortCfgRWLock);
        }
        rc = L7_SUCCESS;
        break;

    case L7_DETACH:
    case L7_LAG_ACQUIRE:
    case L7_PROBE_SETUP:
    case L7_PORT_ROUTING_ENABLED:
        /* Simply loop through the groups and determine if the interface
           is in a group. Then invoke the apply routine */
        osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);
        for (i = 0; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
        {
            if (L7_INTF_ISMASKBITSET(protectedPortCfg->protectedPortGroup[i].
                                     protectedPortPortMask, intIfNum))
            {
                groupId = i;
                break;
            }
        }

        if (i != L7_PROTECTED_PORT_MAX_GROUPS)
        {
          (void) protectedPortGroupIntfDeleteApply(groupId, intIfNum);
        }

        rc = L7_SUCCESS;
        osapiReadLockGive(protectedPortCfgRWLock);
        break;

    case L7_DELETE:
        /* Since this feature only deals with interface masks for config,
           there is no need to do the typical deletion and deallocation of an
           interface config structure. */
        rc = L7_SUCCESS;
        break;

    default:
        rc = L7_SUCCESS;
        break;
    }

    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
}

/*********************************************************************
* @purpose  Propogate Startup notifications to protected port component
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void protectedPortStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_uint32 i;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    switch (startupPhase)
    {
      case NIM_INTERFACE_CREATE_STARTUP:
        /* Since this feature only deals with interface masks for config,
           there is no need to do the typical creation and allocation of an
           interface config structure. */
        break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
        if (protectedPortIntfConfigCanApplyToHw(intIfNum))
        {
          osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);
          for (i = 0; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
          {
              if (L7_INTF_ISMASKBITSET(protectedPortCfg->protectedPortGroup[i].protectedPortPortMask, intIfNum))
              {
                break;
              }
          }
          if (i != L7_PROTECTED_PORT_MAX_GROUPS)
          {
            (void) protectedPortGroupIntfAddApply(i, intIfNum);
          }

          osapiReadLockGive(protectedPortCfgRWLock);
        }
        break;

      default:
        break;
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  switch (startupPhase)
  {
    case NIM_INTERFACE_CREATE_STARTUP:
      /* Register with NIM to receive port CREATE/DELETE events */
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_CREATE);
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_DELETE);
      nimRegisterIntfEvents(L7_PROTECTED_PORT_COMPONENT_ID, protectedPortEventMask_g);
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
      /* Add ATTACH/DETACH events to our NIM registration */
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_ATTACH);
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_DETACH);
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_LAG_ACQUIRE);
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_LAG_RELEASE);
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_PROBE_SETUP);
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_PROBE_TEARDOWN);
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_PORT_ROUTING_ENABLED);
      PORTEVENT_SETMASKBIT(protectedPortEventMask_g, L7_PORT_ROUTING_DISABLED);
      nimRegisterIntfEvents(L7_PROTECTED_PORT_COMPONENT_ID, protectedPortEventMask_g);
      break;

    default:
      break;
  }

  nimStartupEventDone(L7_PROTECTED_PORT_COMPONENT_ID);

  /* Inform cnfgr that protected port has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_PROTECTED_PORT_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);
  }
}

/*********************************************************************
* @purpose  check if the interface is acquired by another component
*           and is therefore unavailable to protectedPort
*
* @param    intfMask@b{(input)} Internal Interface Number
*
* @returns  void
* @returns  void
*
* @comments A interface mask is sent as pass by reference as this is the
*           same mask sent to dtl & hapi. It checks for any port is acquired
*           by the interested components, if they, the port bit is unmasked and
*           sent to Hapi & Dapi.
*
*
* @end
*********************************************************************/
void protectedPortIntfMaskChange(L7_INTF_MASK_t *intfMask)
{
  L7_uint32 intIfNum;

    for ( intIfNum = 1; intIfNum<L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      if (L7_INTF_ISMASKBITSET(*intfMask, intIfNum))
      {
        if( protectedPortIntfIsAcquired(intIfNum) == L7_TRUE)
        {
           L7_INTF_CLRMASKBIT(*intfMask,intIfNum);
        }
        if (protectedPortIntfIsAttached(intIfNum) == L7_FALSE)
        {
           L7_INTF_CLRMASKBIT(*intfMask,intIfNum);
        }
      }
    }
}

