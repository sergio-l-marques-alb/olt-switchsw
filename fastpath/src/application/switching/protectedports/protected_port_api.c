/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_api.c
*
* @purpose   Protected Port API file
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
#include "dot1ad_l2tp_api.h"


/* Begin Function Definitions: usmdb_protected_port.c */

/*********************************************************************
* @purpose  Add an interface to a Protected port group
*          
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfAdd(L7_uint32 groupId, L7_uint32 intIfNum)
{
    protectedPortGroup_t *pCfg;
    L7_uint32             i;
    L7_RC_t rc = L7_FAILURE;

    #if defined L7_DOT1AD_PACKAGE
    DOT1AD_INTFERFACE_TYPE_t intfType;

    if (dot1adInterfaceTypeGet(intIfNum, &intfType) == L7_SUCCESS && 
        intfType != DOT1AD_INTFERFACE_TYPE_SWITCHPORT)
      {
         return L7_NOT_SUPPORTED; 
      }
    #endif

    /* Validity check 
    */
    if (groupId >= L7_PROTECTED_PORT_MAX_GROUPS)
    {
        return(rc);
    }

    /* Check for valid parameters */
    if (protectedPortIntfIsValid(intIfNum) == L7_FAILURE)
    {
        return(rc);
    }

    osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);

    if (protectedPortIntfIsConfigurable(groupId, intIfNum, &pCfg) != L7_TRUE)
    {
        osapiReadLockGive(protectedPortCfgRWLock);
        return(rc);
    }


    /* Check if the interface is already set in the **config** file structure
       for **any** group.  If it is already set in the group specified,
       simply return L7_SUCCESS.  If it is already set, but in a different
       group, return L7_FAILURE. */

    for (i = 0; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
    {
        if (L7_INTF_ISMASKBITSET(protectedPortCfg->protectedPortGroup[i].
                             protectedPortPortMask,
                             intIfNum))
        {
            if (i == groupId)
            {
                rc = L7_SUCCESS;
            }
            osapiReadLockGive(protectedPortCfgRWLock);
            return(rc);
        }
    }

    /* Parameter checking has succeeded so config can successfully be saved.
       An invocation LOG_MSG in underlying layers will signal an inability to 
       actually apply configuration */

    osapiReadLockGive(protectedPortCfgRWLock);
    osapiWriteLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);

    /* Apply the config change */
    rc = L7_SUCCESS;
    if (protectedPortIntfConfigCanApplyToHw(intIfNum))
    {
      rc = protectedPortGroupIntfAddApply(groupId, intIfNum);
    }

    if ( L7_SUCCESS == rc)
    {
      /*Set it in the cfg structure */
      L7_INTF_SETMASKBIT(pCfg->protectedPortPortMask, intIfNum);

      protectedPortCfg->cfgHdr.dataChanged = L7_TRUE;
    }

    osapiWriteLockGive(protectedPortCfgRWLock);

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
L7_RC_t protectedPortGroupIntfDelete(L7_uint32 groupId, L7_uint32 intIfNum)
{
  protectedPortGroup_t *pCfg;
  L7_RC_t rc = L7_FAILURE;

  /* Validity check */
  if (groupId >= L7_PROTECTED_PORT_MAX_GROUPS)
  {
    return(rc);
  }

  /* Check for valid parameters */
  if (protectedPortIntfIsValid(intIfNum) == L7_FAILURE)
  {
    return(rc);
  }

  osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);

  if (protectedPortIntfIsConfigurable(groupId, intIfNum, &pCfg) != L7_TRUE)
  {
    osapiReadLockGive(protectedPortCfgRWLock);
    return(rc);
  }

  /* Check for intIfNum is in the group */
  if ((L7_INTF_ISMASKBITSET(protectedPortCfg->protectedPortGroup[groupId].
                            protectedPortPortMask,
                            intIfNum)) == 0)
  {
    rc = L7_SUCCESS;
    osapiReadLockGive(protectedPortCfgRWLock);
    return(rc);
  }


  /* Parameter checking has succeeded so config can successfully be saved.
     An invocation LOG_MSG in underlying layers will signal an inability to 
     actually apply configuration */

  rc = L7_SUCCESS;
  osapiReadLockGive(protectedPortCfgRWLock);


  osapiWriteLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);
  /* Apply the config change */
  if (protectedPortIntfConfigCanApplyToHw(intIfNum))
  {
    protectedPortGroupIntfDeleteApply(groupId, intIfNum);
  }


  /* CODING NOTE:  Remember,to convert the intIfNum to a configIdMaskOffset
     prior to storing it in the config structure */
  L7_INTF_CLRMASKBIT(pCfg->protectedPortPortMask, intIfNum);

  protectedPortCfg->cfgHdr.dataChanged = L7_TRUE;
  osapiWriteLockGive(protectedPortCfgRWLock);
  return(rc);
}


/*********************************************************************
* @purpose  Given a interface number,gets a groupid,for which an 
*           interface is associated with
*          
* @param    groupId         @b{(input)}    group identifier
* @param    intIfNum        @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfGet(L7_uint32 intIfNum, L7_uint32 *groupId)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  /*check for valid parameters */
  if (protectedPortIntfIsValid(intIfNum) == L7_FAILURE)
  {
    return(rc);
  }

  osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);

  /* Coding note: Use the macros to get the next interface. 
     The interface are stored in an ordered list.  You will have
     to determine whether they are in ascending or descending order
     as SNMP only takes ascending order.  */
  for (i = 0; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
  {
    if (L7_INTF_ISMASKBITSET(protectedPortCfg->protectedPortGroup[i].
                             protectedPortPortMask,
                             intIfNum))
    {
      *groupId = i;
      osapiReadLockGive(protectedPortCfgRWLock);
      rc = L7_SUCCESS;
      return(rc);
    }
  }

  osapiReadLockGive(protectedPortCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Given a group and an interface, get the next group and interface
*          
* @param    groupId         @b{(input)}    group identifier
* @param    intIfNum        @b{(input)}    internal interface number
* @param    *nextGroupId    @b{(output)}   pointer to next group identifier
* @param    *nextIntIfNum   @b{(output)}   pointer to next internal interface number
*
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfGetNext(L7_uint32 groupId, L7_uint32 intIfNum,
                                      L7_uint32 * nextGroupId, L7_uint32 * nextIntIfNum)
{
  L7_uint32 i, j;
  L7_RC_t rc = L7_FAILURE;

  if (groupId >= L7_PROTECTED_PORT_MAX_GROUPS)
  {
    return(rc);
  }

  osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);

  for (i = groupId; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
  {
    for (j = intIfNum + 1; j <= L7_MAX_INTERFACE_COUNT; j++)
    {
      if (L7_INTF_ISMASKBITSET(protectedPortCfg->
                               protectedPortGroup[groupId].protectedPortPortMask,
                               j))
      {
        *nextIntIfNum = j;
        *nextGroupId = i;
        osapiReadLockGive(protectedPortCfgRWLock);
        rc = L7_SUCCESS;
        return(rc);
      }
    }
  }
  osapiReadLockGive(protectedPortCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Sets a group identifer name
*
* @param    groupId         @b{(input)}    group identifier
* @param    buf             @b{(input)}    name of the group
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupNameSet(L7_uint32 groupId, L7_char8 *name)
{
    if (groupId >= L7_PROTECTED_PORT_MAX_GROUPS)
  {
        return L7_FAILURE;
  }

    osapiWriteLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);
    bzero(protectedPortCfg->protectedPortGroup[groupId].protectedPortName, 
          sizeof(protectedPortCfg->protectedPortGroup[groupId].protectedPortName));
  memcpy(protectedPortCfg->protectedPortGroup[groupId].protectedPortName,
         name,
           L7_PROTECTED_PORT_GROUP_NAME_SIZE);
    osapiWriteLockGive(protectedPortCfgRWLock);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a group ID, gets the name of that particular group
*
* @param    groupId         @b{(input)}    group identifier
* @param    buf             @b{(output)}    name of the group
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupNameGet(L7_uint32 groupId, L7_char8 *name)
{
    if (groupId >= L7_PROTECTED_PORT_MAX_GROUPS)
  {
        return L7_FAILURE;
  }

    osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);
  memcpy(name,
         protectedPortCfg->protectedPortGroup[groupId].protectedPortName,
           L7_PROTECTED_PORT_GROUP_NAME_SIZE);
    osapiReadLockGive(protectedPortCfgRWLock);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a group, get all the protected ports which are members of
*           that group  
*
* @param    groupId         @b{(input)}    group identifier
* @param    *intIfMask      @b{(output)}   Mask specifying the member ports
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfMaskGet(L7_uint32 groupId,L7_INTF_MASK_t *intIfMask) 
{
  /*check for valid parameters */    
  if ( ( groupId >= L7_PROTECTED_PORT_MAX_GROUPS ) ||
       ( intIfMask == L7_NULL ) )
  {
    return L7_FAILURE;
  }

  osapiReadLockTake(protectedPortCfgRWLock, L7_WAIT_FOREVER);    

  /* Copy the configuration mask in to the output param */    
  memcpy(intIfMask, &protectedPortCfg->protectedPortGroup[groupId].
         protectedPortPortMask, L7_INTF_INDICES);    

  osapiReadLockGive(protectedPortCfgRWLock); 
  return L7_SUCCESS;
}

/* End Function Definitions*/
