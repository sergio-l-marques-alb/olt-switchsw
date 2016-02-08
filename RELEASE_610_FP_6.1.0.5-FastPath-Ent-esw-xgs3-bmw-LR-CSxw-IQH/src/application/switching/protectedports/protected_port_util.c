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

L7_INTF_MASK_t protectedPortAttachedPortMask;


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

    rc = L7_FAILURE;
    
  memcpy(&tempMask,
         &protectedPortCfg->protectedPortGroup[groupId].protectedPortPortMask,
           L7_INTF_INDICES);

  L7_INTF_SETMASKBIT(tempMask, intIfNum);

    rc = protectedPortGroupCreateApply(groupId, tempMask);
    if (rc != L7_SUCCESS) 
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Cannot add intIfNum %d to group %d. "
				"This appears when an interface could not be added to a particular group.\n", intIfNum, groupId);
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

  rc = dtlProtectedPortDelete( intIfNum );
  if (rc != L7_SUCCESS)
  {
	L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Cannot delete intIfNum %d from group %d. "
				  "This appears when a dtl call to delete an interface from a group fails\n", intIfNum, groupId);
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
	  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Cannot update group %d after deleting"
						  " interface %d. This message appears when an update group for a interface deletion"
						  " fails.\n", groupId, intIfNum );
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
      if ( dtlProtectedPortDelete( i ) != L7_SUCCESS )
      {
        LOG_MSG("unable to delete protected port %d in group %d\n", i, groupId);
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
    L7_BOOL currAcquired, prevAcquired;

    status.intIfNum = intIfNum;
    status.component = L7_PROTECTED_PORT_COMPONENT_ID;
    status.event = intfEvent;
    status.correlator = correlator;
	status.response.reason = NIM_ERR_RC_UNUSED;

    if (PROTECTED_PORT_IS_READY == L7_FALSE)
    {
		L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Received an interface change callback while "
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

        /* Flag the port as being attached */
        L7_INTF_SETMASKBIT(protectedPortAttachedPortMask,intIfNum);           
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
        if (i == L7_PROTECTED_PORT_MAX_GROUPS)
        {
            rc = L7_SUCCESS;
        }
        else
        {   
            (void) protectedPortGroupIntfAddApply(groupId, intIfNum);
        }

        rc = L7_SUCCESS;
      osapiReadLockGive(protectedPortCfgRWLock);
        break;

    case L7_DETACH:

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

        if (i == L7_PROTECTED_PORT_MAX_GROUPS)
        {
            rc = L7_SUCCESS;
        }
        else
        {
            (void) protectedPortGroupIntfDeleteApply(groupId, intIfNum);
			/* L7_INTF_CLRMASKBIT(protectedPortCfg->protectedPortGroup[i].protectedPortPortMask, 
							   intIfNum); */
        }

        rc = L7_SUCCESS;
        osapiReadLockGive(protectedPortCfgRWLock);
        
        /* Flag the port as being detached */
        L7_INTF_CLRMASKBIT(protectedPortAttachedPortMask,intIfNum);           
        break;

    case L7_DELETE:
        /* Since this feature only deals with interface masks for config,
           there is no need to do the typical deletion and deallocation of an
           interface config structure. */
        rc = L7_SUCCESS;
        break;

    case L7_LAG_ACQUIRE:
    case L7_PROBE_SETUP:
    case L7_PORT_ROUTING_ENABLED:

        /* When a port is enabled for a LAG/port-mirroring or Routing , remove the
         * individual port from the ProtectedPort*/
         
        COMPONENT_ACQ_NONZEROMASK(protectedPortIntfInfo[intIfNum].acquiredList, 
                                   prevAcquired);
        /* update acquired list */
        COMPONENT_ACQ_SETMASKBIT(protectedPortIntfInfo[intIfNum].acquiredList,
                  (intfEvent == L7_LAG_ACQUIRE) ? L7_DOT3AD_COMPONENT_ID :
                  (intfEvent == L7_PORT_ROUTING_ENABLED) ? L7_IP_MAP_COMPONENT_ID :
                                                      L7_PORT_MIRROR_COMPONENT_ID);
        /* If the interface was not previously acquired, disable protectedPort. */
        if (prevAcquired == L7_FALSE)
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

           if (i == L7_PROTECTED_PORT_MAX_GROUPS)
           {
             rc = L7_SUCCESS;
           }
           else
           {
              (void) protectedPortGroupIntfDeleteApply(groupId, intIfNum);
           }
           osapiReadLockGive(protectedPortCfgRWLock);
        }
        rc = L7_SUCCESS;
        break;

    case L7_LAG_RELEASE:
    case (L7_PROBE_TEARDOWN):
    case L7_PORT_ROUTING_DISABLED:

       /* When a port is disabled for a LAG/port-mirroring or Routing , and if no other
       * component has acquired the port, e.g. port mirroring, and the port
       * originally had protected enabled, then enable protection again. */
     
       COMPONENT_ACQ_CLRMASKBIT(protectedPortIntfInfo[intIfNum].acquiredList,
                (intfEvent == L7_LAG_RELEASE) ? L7_DOT3AD_COMPONENT_ID :
                (intfEvent == L7_PORT_ROUTING_DISABLED) ? L7_IP_MAP_COMPONENT_ID :
                                           L7_PORT_MIRROR_COMPONENT_ID);

       COMPONENT_ACQ_NONZEROMASK(protectedPortIntfInfo[intIfNum].acquiredList, 
                                 currAcquired);
       if (currAcquired == L7_FALSE)
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
         if (i == L7_PROTECTED_PORT_MAX_GROUPS)
         {
            rc = L7_SUCCESS;
         }
         else
         {
            (void) protectedPortGroupIntfAddApply(groupId, intIfNum);
         }
         osapiReadLockGive(protectedPortCfgRWLock);
       }  
       rc = L7_SUCCESS;
       break;

    default:
        rc = L7_SUCCESS;
    }

    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
}

/*********************************************************************
* @purpose  check if the interface is acquired by another component
*           and is therefore unavailable to protectedPort
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL protectedPortIntfIsAcquired(L7_uint32 intIfNum)
{
  L7_BOOL result;

  /*********************************************************
   * This routine checks if the interface has been acquired
   * by any component. We don't check for any COMPONENT-IDs
   * if they have acquired the interface.
   ********************************************************/
  COMPONENT_NONZEROMASK(protectedPortIntfInfo[intIfNum].acquiredList, result);

  return result;
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
        if (L7_INTF_ISMASKBITSET(protectedPortAttachedPortMask, intIfNum) == 0)
        {
            L7_INTF_CLRMASKBIT(*intfMask,intIfNum);           
        }
      }
    }
}

