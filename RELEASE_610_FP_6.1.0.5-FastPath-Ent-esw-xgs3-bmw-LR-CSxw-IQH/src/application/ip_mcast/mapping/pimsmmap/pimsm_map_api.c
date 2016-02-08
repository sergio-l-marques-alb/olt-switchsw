/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  pimsm_map_api.c
*
* @purpose   PIM-SM API functions
*
* @component PIM-SM Mapping Layer
*
* @comments  none
*
* @create    03/11/2002
*
* @author    gkiran/dsatyanarayana
*
* @end
*
**********************************************************************/

#include "l7_pimsminclude.h"
#include "mcast_wrap.h"
#include "pimsm_map_cfg.h"
#include "l7_pimsm_api.h"
#include "l7_mcast_api.h"
#include "pimsm_vend_exten.h"
#include "trap_layer3_mcast_api.h"
#include "pimsm_vend_ctrl.h"
#include "pimsm_map.h"


/*********************************************************************
* @purpose  Test is inhe PIM-SSM Range.
*
* @param    familyType       @b{(input)} Address Family type
* @param    groupAddr        @b{(input)} Group Address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none    
*
* @end
*********************************************************************/
L7_BOOL pimsmMapIsInSsmRange(L7_uchar8 familyType, L7_inet_addr_t *groupAddr)
{
  pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
  pimsmCfgSsmRange_t  *ssmEntryPtr = L7_NULLPTR;
  L7_uint32           ssmEntryPrefixLen = L7_NULL;
  L7_uint32           index = L7_NULL;
  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (groupAddr == L7_NULLPTR)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "GroupAddr is NULLPTR");
    return L7_FALSE;
  }
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FALSE;
  }

  /* Do not allow Reserved, Node Local and Site Local Groups */
  if (((((L7_in6_addr_t)(groupAddr->addr.ipv6)).in6.addr16[0]) == 0xff30) ||
      ((((L7_in6_addr_t)(groupAddr->addr.ipv6)).in6.addr16[0]) == 0xff31) ||
      ((((L7_in6_addr_t)(groupAddr->addr.ipv6)).in6.addr16[0]) == 0xff35))
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Group - %s is Reserved or Node/Site Local",
                     inetAddrPrint(groupAddr,grpAddr));
    return L7_FALSE;
  }

  for (index = 0; index < L7_MAX_SSM_RANGE; index++)
  {
    ssmEntryPtr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
    if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_FALSE)
    {
      /* Check for default/Reserved SSM Range */
      if ((((L7_in6_addr_t)(ssmEntryPtr->groupAddr.addr.ipv6)).in6.addr16[0]) == 0xff30)
      {
        ssmEntryPrefixLen = 12;
      }
      else
      {
        ssmEntryPrefixLen = ssmEntryPtr->prefixLen;
      }

      if (inetAddrCompareAddrWithMask(&ssmEntryPtr->groupAddr, 
                                      ssmEntryPrefixLen, 
                                      groupAddr, ssmEntryPrefixLen) == L7_NULL)
      {
        /* found ssm entry */
        return L7_TRUE;
      }
    }
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "Group address (%s) is not in SSM Range table",
                    inetAddrPrint(groupAddr,grpAddr));
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Enqueue the SSM Range Set Information
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    grpAddr       @b{(input)} Group address for the SSM Range
* @param    prefixLen     @b{(input)} Prefix Len for SSM Range Grp Address
* @param    ssmMode       @b{(input)} SSM Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
static L7_RC_t
pimsmMapSsmRangeQueue (pimsmMapCB_t *pimsmMapCbPtr,
                       L7_inet_addr_t *grpAddr,
                       L7_uchar8 prefixLen,
                       L7_uint32 ssmMode)
{
  pimsmMapSSMdata_t ssmInfo;

  memset (&ssmInfo, 0, sizeof (pimsmMapSSMdata_t));

  inetCopy (&ssmInfo.grpAddr, grpAddr);
  ssmInfo.prefixLen = prefixLen;
  ssmInfo.operMode = ssmMode;

  if (pimsmMapCfgSetEventQueue (pimsmMapCbPtr, PIMSMMAP_SSM_EVENT, &ssmInfo,
                                sizeof (pimsmMapSSMdata_t))
                             != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the PIM-SM SSM Range.
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmMode          @b{(input)} ssm Mode
* @param    groupAddr        @b{(input)} SSM Range Addr
* @param    groupMask        @b{(input)} mask
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeSet(L7_uchar8 familyType, L7_uint32 ssmMode, 
                    L7_inet_addr_t *groupAddr, L7_inet_addr_t *groupMask)
{
  pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32           index = L7_NULL, freeSlot = L7_NULL;
  L7_uchar8           paramPrefixLen = L7_NULL, cmpMaskLen = L7_NULL;
  pimsmCfgSsmRange_t  *ssmRange = L7_NULLPTR;
  pimsmCfgSsmRange_t  *freeSsmRange = L7_NULLPTR;
  L7_BOOL             freeSlotFound = L7_FALSE, updatedEntry = L7_FALSE;
  L7_RC_t             retCode = L7_FAILURE;
  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8 grpMask[IPV6_DISP_ADDR_LEN];
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  /* PIM-SM component is not started by configurator */
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULLPTR)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                       "PIM-SM component not started by configurator");
     return L7_FAILURE;
  }
  
  if (inetIsInMulticast(groupAddr) != L7_TRUE)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Group Address(%s) is not a multicast address",
                      inetAddrPrint(groupAddr,grpAddr));
    return L7_FAILURE;
  }

  if (((((L7_in6_addr_t)(groupAddr->addr.ipv6)).in6.addr16[0]) == 0xff31) ||
      ((((L7_in6_addr_t)(groupAddr->addr.ipv6)).in6.addr16[0]) == 0xff35))
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Group - %s is Node/Site Local",
                     inetAddrPrint(groupAddr,grpAddr));
    return L7_NOT_SUPPORTED;
  }
  if (inetMaskToMaskLen(groupMask, &paramPrefixLen) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert Mask(%s) to MaskLen",
                      inetAddrPrint(groupMask,grpMask));
    return L7_FAILURE;
  }
  
  if (ssmMode == L7_ENABLE)
  {
    for (index = 0; index < L7_MAX_SSM_RANGE; index++)
    {
      ssmRange = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
      /* To make sure same prefix length groups are not added
       * in SSM range table.
       */
      if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_FALSE)
      { 
        /* The address comparision should be done with ANDed group addresses but not with
           absolute group addresses.*/
        cmpMaskLen = min(paramPrefixLen, ssmRange->prefixLen);
        
        if (inetAddrCompareAddrWithMask (&ssmRange->groupAddr, cmpMaskLen,
                                         groupAddr, cmpMaskLen) == 0)
        {
          /* Update the address with lowest prefix length */
          if (ssmRange->prefixLen != paramPrefixLen)
          {
            if (paramPrefixLen < ssmRange->prefixLen)
            {
              if (updatedEntry == L7_FALSE)
              {
                ssmRange->prefixLen = paramPrefixLen;
                inetCopy(&ssmRange->groupAddr, groupAddr);
                PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Prefix Length - %d updated for "
                                 "SSM Group - %s", paramPrefixLen,
                                 inetAddrPrint(groupAddr,grpAddr));
                /* return L7_SUCCESS; */
                updatedEntry = L7_TRUE;
              }
              else
              {
                /* Delete this entry, as a bigger range has newly configured
                   has updated an entry and no need of this old entry as this 
                   already falls under it.*/
                inetAddressZeroSet(familyType, &ssmRange->groupAddr);
                pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_TRUE;
                ssmRange->prefixLen = L7_NULL;
                ssmRange->ssmMode = L7_FALSE;
                pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
              }
              continue;
            }
            else if (ssmRange->prefixLen < paramPrefixLen)
            {
              PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Prefix Length - %d updated for "
                               "SSM Group - %s", paramPrefixLen,
                               inetAddrPrint(groupAddr,grpAddr));
              return L7_ALREADY_CONFIGURED;
            }
            else
            {
              /* Do Nothing */
            }
          }
          else
          {
            PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Trying to add duplicate entry for "
                             "SSM Group - %s with Prefix Length - %d.",
                             inetAddrPrint(groupAddr,grpAddr), paramPrefixLen);
            return L7_ALREADY_CONFIGURED;
          }
        }
      }
      else if ((freeSlotFound == L7_FALSE) && 
            (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_TRUE))
      {
         freeSlotFound = L7_TRUE;
         freeSlot = index;
      }
    }
    if (updatedEntry == L7_TRUE)
    {
      PIMSM_MAP_LOG_MSG (familyType, "Updated entry(s), no new entry added.");
      return L7_SUCCESS;
    }
    if ((index >= L7_MAX_SSM_RANGE) && (freeSlotFound != L7_TRUE))
    {
      PIMSM_MAP_LOG_MSG (familyType, "PIMSM SSM Range Table is Full");
      return L7_TABLE_IS_FULL;
    }
    freeSsmRange = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[freeSlot];
    pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[freeSlot] = L7_FALSE;
    freeSsmRange->prefixLen = paramPrefixLen;
    freeSsmRange->ssmMode = ssmMode;
    if (inetCopy(&freeSsmRange->groupAddr, groupAddr) != L7_SUCCESS)
    {
       return L7_FAILURE;
    }
    pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    if (pimsmMapSsmRangeQueue (pimsmMapCbPtr, groupAddr, paramPrefixLen, ssmMode)
                            != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_EVENTS, "SSM Range Set Event Queue Failed.\n");
      retCode = L7_FAILURE;
    }

    retCode = L7_SUCCESS;
  }
  else if (ssmMode == L7_DISABLE)
  {
    for (index = 0; index < L7_MAX_SSM_RANGE; index++)
    {
      ssmRange = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
      /* To find the ssm table entry to be deleted.
       */
      if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_FALSE)
      {   
       if ((L7_INET_IS_ADDR_EQUAL (&ssmRange->groupAddr, groupAddr) != 0) &&
           (ssmRange->prefixLen == paramPrefixLen))
       {
         /*clearing the SSM range entry.*/
         inetAddressZeroSet(familyType, &ssmRange->groupAddr);
         pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] = L7_TRUE;
         ssmRange->prefixLen = L7_NULL;
         ssmRange->ssmMode = ssmMode;
         pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
         return L7_SUCCESS;
       }
      }
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "This ssm range combination could not be removed");     
    retCode = L7_FAILURE;
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid command on SSM Range");     
    retCode = L7_FAILURE;
  }
  return retCode;

}
/*********************************************************************
* @purpose  Check whether ssm range entry exists.
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(input)} SSM Range Addr
* @param    prefixLen        @b{(input)} length of the mask
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeEntryGet(L7_uchar8 familyType, 
                                 L7_inet_addr_t *ssmRangeAddr, 
                                 L7_uchar8 prefixLen)
{
  pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
  pimsmCfgSsmRange_t  *ssmEntryPtr = L7_NULLPTR;
  L7_uint32           index = L7_NULL;
  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  for (index = 0; index < L7_MAX_SSM_RANGE; index++)
  {
    ssmEntryPtr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
    if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_FALSE)
    {
      if ((L7_INET_IS_ADDR_EQUAL(&ssmEntryPtr->groupAddr, ssmRangeAddr)) &&
          (prefixLen == ssmEntryPtr->prefixLen))
      {
        /* found ssm entry */
         return L7_SUCCESS;
      }

    }
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Group address (%s) is not in SSM Range table",
                    inetAddrPrint(ssmRangeAddr,grpAddr));
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain next  ssm range entry  in the ssmrange TABLE
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(inout)} SSM Range Addr
* @param    prefixLen        @b{(inout)} length of the mask
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeEntryNextGet(L7_uchar8 familyType, 
                                     L7_inet_addr_t *ssmRangeAddr, 
                                     L7_uchar8 *prefixLen)
{
  pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
  pimsmCfgSsmRange_t  *ssmEntryPtr = L7_NULLPTR;
  L7_uint32           index = L7_NULL;
  L7_inet_addr_t     grpAddrFind,grpAddrLocal;
  L7_uchar8          grpPrefixLenLocal,grpPrefixLenFind;
  L7_BOOL            foundEntry = L7_FALSE;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  
  if ((ssmRangeAddr == L7_NULLPTR) || (prefixLen == L7_NULLPTR))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid input paramsters");
    return L7_FAILURE;
  }

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSM is not initialized");
    return L7_FAILURE;
  }

  /* Assuming no valid entry is found.*/
  inetAddressZeroSet(familyType, &grpAddrFind);
  grpPrefixLenFind = L7_NULL;
  foundEntry = L7_FALSE;

  for (index = 0; index < L7_MAX_SSM_RANGE; index++)
  {
    ssmEntryPtr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
    if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_TRUE)
    {
      continue;
    }
    inetCopy(&grpAddrLocal, &ssmEntryPtr->groupAddr);
    grpPrefixLenLocal = ssmEntryPtr->prefixLen;

    if (inetIsAddressZero(&grpAddrLocal) == L7_TRUE)
    {
      continue;
    }

    if (foundEntry == L7_FALSE &&
        (inetAddrCompareAddrWithMaskIndividual(&grpAddrLocal,grpPrefixLenLocal,ssmRangeAddr, 
                                            *prefixLen) > L7_NULL))
    {
      inetCopy(&grpAddrFind, &grpAddrLocal);
      grpPrefixLenFind = grpPrefixLenLocal;
      foundEntry = L7_TRUE;
    }
    else if (foundEntry == L7_TRUE &&
             (inetAddrCompareAddrWithMaskIndividual(&grpAddrLocal,grpPrefixLenLocal,&grpAddrFind, 
                                            grpPrefixLenFind) < L7_NULL) &&
             (inetAddrCompareAddrWithMaskIndividual(&grpAddrLocal,grpPrefixLenLocal,ssmRangeAddr, 
                                            *prefixLen) > L7_NULL))
    {
      inetCopy(&grpAddrFind, &grpAddrLocal);
      grpPrefixLenFind = grpPrefixLenLocal;
    }
  }

  if (foundEntry == L7_FALSE) /* No Next entry found */
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES," SSM Range next entry not found");
    return L7_FAILURE;
  }
  inetCopy(ssmRangeAddr,&grpAddrFind);
  *prefixLen = grpPrefixLenFind;
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, 
                  "\nFunction Exit.\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Obtain next  ssm range entry  in the ssmrange TABLE
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(inout)} SSM Range Addr
* @param    prefixLen        @b{(inout)} length of the mask
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeEntryNextGetOld(L7_uchar8 familyType, 
                                     L7_inet_addr_t *ssmRangeAddr, 
                                     L7_uchar8 *prefixLen)
{
  pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
  pimsmCfgSsmRange_t  *ssmEntryPtr = L7_NULLPTR;
  L7_inet_addr_t      ssmRangeMask;
  L7_inet_addr_t      ssmAddrFind;
  L7_inet_addr_t      ssmAddrLocal;
  L7_BOOL             found = L7_FALSE;
  L7_uint32           index = L7_NULL;
  L7_uint32           indexStart = L7_NULL;
  L7_uint32           ssmPrefixLenFind = L7_NULL;
  L7_uint32           ssmPrefixLenLocal = L7_NULL;
  L7_int32           ssmAddrEqual = L7_NULL,cmpResult = L7_NULL;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  
  if ((ssmRangeAddr == L7_NULLPTR) || (prefixLen == L7_NULLPTR))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid input paramsters");
    return L7_FAILURE;
  }

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSM is not initialized");
    return L7_FAILURE;
  }

  inetAddressZeroSet(familyType, &ssmRangeMask);
  inetAddressZeroSet(familyType, &ssmAddrFind);

  if (inetMaskLenToMask(familyType, *prefixLen, &ssmRangeMask) != L7_SUCCESS)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert Mask Len (%d) to Mask",
                       *prefixLen);
     return L7_FAILURE;
  }
  for (index = 0; index < L7_MAX_SSM_RANGE; index++)
  {
    ssmEntryPtr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
    if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_TRUE)
    {
      continue;
    }
    /* Assuming first valid entry as max entry */
    inetCopy(&ssmAddrFind, &ssmEntryPtr->groupAddr);
    ssmPrefixLenFind = ssmEntryPtr->prefixLen;

    if(inetAddrCompareAddrWithMask(&ssmAddrFind,ssmPrefixLenFind,ssmRangeAddr, 
                                            *prefixLen) <=L7_NULL)
    {
      continue;
    }

    found = L7_TRUE;
    indexStart = index;
    break;
  }
  if (found == L7_FALSE)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"ssm range not configured");
     return L7_FAILURE;
  }
 
  for (index = indexStart; index < L7_MAX_SSM_RANGE; index++)
  {
    ssmEntryPtr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
    /* No valid entry in table, continue. */
    if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_TRUE)
    {
      continue;
    }
    inetCopy(&ssmAddrLocal, &ssmEntryPtr->groupAddr);
    ssmPrefixLenLocal = ssmEntryPtr->prefixLen;
    /* Anything less than the passed ssmRangeAddr or greater 
     * than found ssmRangeAddr, continue.
     */
    ssmAddrEqual = inetAddrCompareAddrWithMask(&ssmAddrLocal,ssmPrefixLenLocal,&ssmAddrFind, 
                                               ssmPrefixLenFind);
    cmpResult = inetAddrCompareAddrWithMask(&ssmAddrLocal,ssmPrefixLenLocal,ssmRangeAddr, 
                                            *prefixLen);

    if(cmpResult > L7_NULL)
    {
      if(ssmAddrEqual >= L7_NULL)
      {
        continue;
      }
      inetCopy(&ssmAddrFind, &ssmEntryPtr->groupAddr);
      ssmPrefixLenFind = ssmEntryPtr->prefixLen;
      found = L7_TRUE;
    }

  }
  if ((inetIsAddressZero(ssmRangeAddr) == L7_TRUE) && (*prefixLen == L7_NULL))
  {
     found =  L7_TRUE;
  }
   
  if (found == L7_FALSE) /* No Next entry found */
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"ssm range next entry not found");
    return L7_FAILURE;
  }
  /* returning next ssm entry */
  inetCopy(ssmRangeAddr, &ssmAddrFind);
  *prefixLen = ssmPrefixLenFind;
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, 
                  "\nFunction Exit.\n");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gets the admin mode for the ssm range entry.
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(input)} SSM Range Addr
* @param    prefixLen        @b{(input)} length of the mask
* @param    ssmMode          @b{(output)} ssm Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeModeGet(L7_uchar8 familyType,
                                L7_inet_addr_t *ssmRangeAddr, 
                                L7_uchar8 prefixLen, L7_uint32 *ssmMode)
{
  pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
  pimsmCfgSsmRange_t  *ssmEntryPtr = L7_NULLPTR;
  L7_uint32           index = L7_NULL;
  L7_uchar8 grpAddr[IPV6_DISP_ADDR_LEN];

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  for (index = 0; index < L7_MAX_SSM_RANGE; index++)
  {
    ssmEntryPtr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
    if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_FALSE)
    {
      if (inetAddrCompareAddrWithMask(&ssmEntryPtr->groupAddr, 
                                      ssmEntryPtr->prefixLen,
                                      ssmRangeAddr, prefixLen) == L7_NULL)
      {
        *ssmMode = ssmEntryPtr->ssmMode; 
        /* found ssm entry */
        return L7_SUCCESS;
      }
    }
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Group address (%s) is not in SSM Range table",
                    inetAddrPrint(ssmRangeAddr,grpAddr));
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the admin mode for the ssm range entry.
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(input)} SSM Range Addr
* @param    prefixLen        @b{(input)} length of the mask
* @param    ssmMode          @b{(input)} ssm Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeModeSet(L7_uchar8 familyType,
                                L7_inet_addr_t *ssmRangeAddr, 
                                L7_uchar8 prefixLen, L7_uint32 ssmMode)
{
  pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
  pimsmCfgSsmRange_t  *ssmEntryPtr = L7_NULLPTR;
  L7_inet_addr_t      ssmRangeMask;
  L7_uint32           index = L7_NULL;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  inetAddressZeroSet(familyType, &ssmRangeMask);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  if (inetMaskLenToMask(familyType, prefixLen, &ssmRangeMask) != L7_SUCCESS)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                       prefixLen);
     return L7_FAILURE;
  }
  for (index = 0; index < L7_MAX_SSM_RANGE; index++)
  {
    ssmEntryPtr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmSsmRange[index];
    if (pimsmMapCbPtr->pPimsmInfo->pimsmSsmIsEmpty[index] == L7_FALSE)
    {
      if (inetAddrCompareAddrWithMask(&ssmEntryPtr->groupAddr, 
                                      ssmEntryPtr->prefixLen,
                                      ssmRangeAddr, prefixLen) == L7_NULL)
      {
        /* found ssm entry & set ssm Mode. */
        ssmEntryPtr->ssmMode = ssmMode; 
        return L7_SUCCESS;
      }
    }
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,  
  "\nGroup Address is not in SSM Range Table.\n");
  /* Entry not found, add new entry in the ssm table. */
  return pimsmMapSsmRangeSet(familyType, ssmMode, 
                             ssmRangeAddr, &ssmRangeMask);
}

/*********************************************************************
* @purpose  Set the PIM-SM administrative mode
*
* @param    familyType      @b{(input)}     Address Family type
* @param    mode            @b{(input)}     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Dynamic configurability is supported
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimsmAdminModeSet(L7_uchar8 familyType, L7_uint32 mode)
{
  pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32     currentMcastProtocol;
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  /* this fails if PIM-SM component not started by configurator */
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  { 
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not initialized");
    return L7_FAILURE;
  }

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid mode(%d)",mode);
    return L7_FAILURE;
  }

  /* assumes current admin mode already established, 
                                            so nothing more to do here */
  if (mode == pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode)
  {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_CONFIGURED,
                      "\nAdmin Mode already set\n");
      return L7_SUCCESS;
  }

  if (mode == L7_ENABLE)
  {
    if (mcastMapIpCurrentMcastProtocolGet(familyType, &currentMcastProtocol) == 
        L7_SUCCESS  && currentMcastProtocol != L7_MCAST_IANA_MROUTE_PIM_SM &&
        currentMcastProtocol != L7_MCAST_IANA_MROUTE_UNASSIGNED) {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                          "Another MCAST protocol(%d) is already enabled",
                           currentMcastProtocol);
     {
        /* Here L7_ERROR  return status is used by CLI to recognize the failure
           as "other MRP is configured" and other return status for representing 
           other failure states. The return status can be enhanced to capture
           various return states in multicast. */
        return L7_ERROR;
     }
    }
  
    if (mcastMapIpCurrentMcastProtocolSet(familyType, L7_MCAST_IANA_MROUTE_PIM_SM)
                                          != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to set current Mcast Protocol to PIMSM",
                      currentMcastProtocol);
      return L7_FAILURE;
    }
    if(pimsmMapMemoryInit(pimsmMapCbPtr) != L7_SUCCESS)
    {
     PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Another MCAST protocol might enabled");
     return L7_FAILURE;                     
    }      
    pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode = mode;
    pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  
    pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_ADMIN_MODE_EVENT, 
        &mode, sizeof(L7_uint32));    
    
  }
  else if (mode == L7_DISABLE)
  {
    if ((mcastMapIpCurrentMcastProtocolGet(familyType, &currentMcastProtocol) == L7_SUCCESS) && 
        (currentMcastProtocol == L7_MCAST_IANA_MROUTE_PIM_SM))
    {
      if (mcastMapIpCurrentMcastProtocolSet(familyType, L7_MCAST_IANA_MROUTE_UNASSIGNED)
                                            != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                        "Failed to reset current Mcast Protocol",
                        currentMcastProtocol);
      }
    }
    pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode = mode;
    pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  
    if( pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_ADMIN_MODE_EVENT, 
        &mode, sizeof(L7_uint32)) != L7_SUCCESS)
    {
        PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "PIMSM Map Event Post Failed for eventType:%d.\n",
                 PIMSMMAP_ADMIN_MODE_EVENT);
        /*Reset everything back to previous values*/
        pimsmMapMemoryDeInit(pimsmMapCbPtr);
        pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode = L7_DISABLE;
        pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
        mcastMapIpCurrentMcastProtocolSet (familyType, 
                                                 L7_MCAST_IANA_MROUTE_UNASSIGNED); 
    }
  }



  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the PIM-SM administrative mode
*
* @param    familyType         @b{(input)}   Address Family type
* @param    mode               @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimsmAdminModeGet(L7_uchar8 familyType,
                                  L7_uint32 *mode)
{
  pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not initialized");
    return L7_FAILURE;
  }

  if (mode != L7_NULLPTR)
  {
     *mode = pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmAdminMode;
     return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns whether PIM-SM is operational on router or not
*
* @param    familyType       @b{(input)}     Address Family type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmMapPimsmIsOperational(L7_uchar8 familyType)
{
    pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FALSE;
    }

    if (pimsmMapCbPtr->pPimsmInfo == L7_NULLPTR)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
    }
    else
    {
       if(pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
       {
          return L7_TRUE;
       }
       else
       {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not operational");
          return L7_FALSE;
       }
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component was not started");
    return L7_FALSE;
}
/*********************************************************************
* @purpose  Set the data threshold rate for switching to SPT
*
* @param    pimsmMapCbPtr     @b{(input)}   Mapping Control Block.
* @param    dataThresholdRate @b{(input)}   Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done before this function is invoked
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapDataThresholdQueue(pimsmMapCB_t *pimsmMapCbPtr, 
                                               L7_uint32 dataThresholdRate)
{

  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_SPT_THRESHOLD_EVENT, 
      &dataThresholdRate, sizeof(L7_uint32));

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the data threshold rate
*
* @param    familyType          @b{(input)} Address Family type
* @param    dataThresholdRate   @b{(input)} Data Threshold Rate in kbits/sec
*

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range (0-2000) checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapDataThresholdRateSet(L7_uchar8 familyType, 
                                     L7_uint32 dataThresholdRate)
{
   pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   /* this fails if PIM-SM component not started by configurator */
   if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
      return L7_FAILURE;
   }
   /* range checking */
   if ( (dataThresholdRate < L7_PIMSM_DATATHRESHOLD_RATE_MIN) ||
        (dataThresholdRate > L7_PIMSM_DATATHRESHOLD_RATE_MAX) )
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid dataThresholdRate(%d)",
                        dataThresholdRate);
      return L7_FAILURE;
   }
   if (dataThresholdRate == 
       pimsmMapCbPtr->pPimsmMapCfgData->rtr.dataThresholdRate)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_CONFIGURED,
      "\nData threshold rate already set\n");
      return L7_SUCCESS;
   }

   pimsmMapCbPtr->pPimsmMapCfgData->rtr.dataThresholdRate 
                                                    = dataThresholdRate;
   pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

   if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational != L7_TRUE)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n PIM-SM not:"
     "operational config paramter stored but not applied\n"); 
     
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                       "PIM-SM not operational.config paramter \
                        stored but not applied\n"); 
   }
   pimsmMapDataThresholdQueue(pimsmMapCbPtr, dataThresholdRate);
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the data threshold rate
*
* @param    familyType          @b{(input)}  Address Family type
* @param    dataThresholdRate   @b{(output)} Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapDataThresholdRateGet(L7_uchar8 familyType,
                                     L7_uint32 *dataThresholdRate)
{
    pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }
    
    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
      return L7_FAILURE;
    }

    if (dataThresholdRate != L7_NULLPTR)
    {
       *dataThresholdRate = pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                                        dataThresholdRate;
       return L7_SUCCESS;
    }
    return L7_FAILURE;
}
/*********************************************************************
* @purpose  Set the data threshold rate for switching to SPT
*
* @param    pimsmMapCbPtr    @b{(input)}    Mapping Control Block.
* @param    regThresholdRate @b{(input)}    Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done before this function is invoked
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapRegisterThresholdQueue(pimsmMapCB_t *pimsmMapCbPtr, 
                                               L7_uint32 regThresholdRate)
{

  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_REGISTER_THRESHOLD_EVENT, 
      &regThresholdRate, sizeof(L7_uint32));

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the register threshold rate
*
* @param    familyType         @b{(input)}  Address Family type
* @param    regThresholdRate   @b{(input)}  Register Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range (0-2000) checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRegisterThresholdRateSet(L7_uchar8 familyType, 
                                         L7_uint32 regThresholdRate)
{
   pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   /* this fails if PIM-SM component not started by configurator */
   if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
      return L7_FAILURE;
   }
   /* range checking */
   if ( (regThresholdRate < L7_PIMSM_REGTHRESHOLD_RATE_MIN) ||
        (regThresholdRate > L7_PIMSM_REGTHRESHOLD_RATE_MAX) )
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid regThresholdRate(%d)",
                        regThresholdRate);
      return L7_FAILURE;
   }
   if (regThresholdRate == pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                                         regThresholdRate)
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_CONFIGURED,
       "\nRegister threshold rate already set\n");
       return L7_SUCCESS;
   }

   pimsmMapCbPtr->pPimsmMapCfgData->rtr.regThresholdRate = regThresholdRate;
   pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

   if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational != L7_TRUE)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n PIM-SM not:"
     "operational config paramter stored but not applied\n"); 
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                       "PIM-SM not operational.config parameter \
                        stored but not applied\n"); 
   }
   
   pimsmMapRegisterThresholdQueue(pimsmMapCbPtr, 
                                             regThresholdRate);
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the register threshold rate
*
* @param    familyType        @b{(input)}   Address Family type
* @param    regThresholdRate  @b{(output)}  Register Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRegisterThresholdRateGet(L7_uchar8 familyType,
                                         L7_uint32 *regThresholdRate)
{
    pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }

    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
      return L7_FAILURE;
    }

    if (regThresholdRate != L7_NULLPTR)
    {
       *regThresholdRate = pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                                        regThresholdRate;
       return L7_SUCCESS;
    } 
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Apply the static RP information
*
* @param    pimsmMapCbPtr @b{(input)}       Mapping Control Block.
* @param    index         @b{(input)}       Index into the config structure 
*                                           for this static RP
* @param    rpIpAddr      @b{(input)}       Ip address of the RP
* @param    rpGrpAddr     @b{(input)}       Group address supported by the RP
* @param    rpGrpMask     @b{(input)}       Group mask for the group address
* @param    conflict      @b{(input)}       conflict
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapStaticRPQueue(pimsmMapCB_t *pimsmMapCbPtr,
                        L7_inet_addr_t *rpIpAddr, L7_inet_addr_t *rpGrpAddr, 
                        L7_uchar8 prefixLen, L7_BOOL conflict, L7_BOOL mode,
                        L7_uint32 index)
{
  pimsmMapStaticRPdata_t data;

  memset(&data,0 , sizeof(pimsmMapStaticRPdata_t));
  inetCopy(&data.grpAddr, rpGrpAddr);
  inetCopy(&data.rpAddr, rpIpAddr);
  data.prefixLen = prefixLen;
  data.overrideFlag = conflict;
  data.mode = mode;
  data.index = index;  
  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_STATIC_RP_EVENT, 
      &data, sizeof(pimsmMapStaticRPdata_t));

  return L7_SUCCESS;
  
}

/*********************************************************************
* @purpose  To get the static RP Entry.
*
* @param    familyType     @b{(input)}  Address Family type.
* @param    rpGrpAddr      @b{(input)}  static RP Group address.
* @param    prefixLength   @b{(input)}  prefix Length.
* @param    staticRpIndex  @b{(output)} array index of static Rp.
* @param    staticRpEntry  @b{(output)} Static RP Addr Entry.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapStaticRpEntryGet(L7_uchar8 familyType, 
                                L7_inet_addr_t *rpGrpAddr, 
                                L7_uchar8 prefixLength,
                                L7_uint32 *staticRpIndex,
                                pimsmCfgStaticRP_t **staticRpEntry)
{
    pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
    pimsmCfgStaticRP_t *staticRp = L7_NULLPTR;
    L7_uint32          index = L7_NULL;
    L7_inet_addr_t     rpGrpMask;
    L7_uchar8          grpAddr[IPV6_DISP_ADDR_LEN];

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    inetAddressZeroSet(familyType, &rpGrpMask);
    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }
    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
      return L7_FAILURE;
    }

    if (inetMaskLenToMask(familyType, prefixLength, &rpGrpMask) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                        prefixLength);
      return L7_FAILURE;
    }

    for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
    {
      staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];
      if ((L7_INET_IS_ADDR_EQUAL(&staticRp->rpGrpAddr, rpGrpAddr)) &&
          (L7_INET_IS_ADDR_EQUAL(&staticRp->rpGrpMask, &rpGrpMask)))
      {
         *staticRpEntry = staticRp;
         *staticRpIndex = index;
         return L7_SUCCESS;
      }
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to get Static RP entry for Grp Addr(%s",
                      inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the static RP information
*
* @param    familyType       @b{(input)}    Address Family type
* @param    rpIpAddr         @b{(input)}    Ip address of the RP
* @param    rpGrpAddr        @b{(input)}    Group address supported by the RP
* @param    prefixLen        @b{(input)}    prefix Length.
* @param    conflict         @b{(input)}    over ride.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPSet(L7_uchar8 familyType, L7_inet_addr_t *rpIpAddr,
    L7_inet_addr_t *rpGrpAddr, L7_uchar8 prefixLen, L7_BOOL conflict)
{
   pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
   pimsmCfgStaticRP_t  *staticRp = L7_NULLPTR;
   pimsmCfgStaticRP_t  *freeStaticRp = L7_NULLPTR;
   L7_uint32            index, freeSlot;
   L7_inet_addr_t       cfgGrpRt;
   L7_inet_addr_t       paramGrpRt;
   L7_inet_addr_t       rpGrpMask;
   L7_BOOL              freeSlotFound;
   L7_uchar8            rpAddr[IPV6_DISP_ADDR_LEN];
   L7_uchar8            grpAddr[IPV6_DISP_ADDR_LEN];

   inetAddressZeroSet(familyType, &cfgGrpRt);
   inetAddressZeroSet(familyType, &paramGrpRt);

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   /* this fails if PIM-SM component not started by configurator */
   if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
     return L7_FAILURE;
   }

   if (inetMaskLenToMask(familyType, prefixLen, &rpGrpMask) != L7_SUCCESS)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                        prefixLen);
      return L7_FAILURE;
   }
   
   /* range checking */
   /* an IP address or subnet mask of 0 is invalid */
   if (inetIsAddressZero(rpIpAddr) == L7_TRUE)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"RpAddress is 0.It is an invalid address");
     return L7_FAILURE;
   }
   
   /* reject the class A net 127 (loopback) address */
   /* accept all other class A */
   /* accept all class B */
   /* accept all class C */
   if (inetIsValidHostAddress(rpIpAddr) != L7_TRUE)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid RP address(%s)",
                       inetAddrPrint(rpIpAddr,rpAddr));
     return L7_FAILURE;
   }
   if (inetIsInMulticast(rpGrpAddr) != L7_TRUE)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Group Address(%s) not in multicast range",
                       inetAddrPrint(rpGrpAddr,grpAddr));
     return L7_FAILURE;
   }

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,
                   "\nCode Flow.\n");
   /* Check if it is same as configured info. */
   freeSlot = 0;
   freeSlotFound = L7_FALSE;
   for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
   {
     staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];

     if (L7_INET_IS_ADDR_EQUAL(&staticRp->rpIpAddr, rpIpAddr))
     {
       if (inetAddressAnd(&staticRp->rpGrpAddr, 
                          &staticRp->rpGrpMask, &cfgGrpRt)!= L7_SUCCESS)
       {
         return L7_FAILURE;
       }

       if (inetAddressAnd(rpGrpAddr, &rpGrpMask, &paramGrpRt)!= L7_SUCCESS)
       {
         return L7_FAILURE;
       }

       if (L7_INET_IS_ADDR_EQUAL(&cfgGrpRt, &paramGrpRt))
       {
         if (staticRp->overRide != conflict)
         {
           staticRp->overRide = conflict;
            pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
           if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational == L7_TRUE)
           {
             if (pimsmMapCbPtr->pPimsmInfo->pimsmStaticRPOper[index] == L7_TRUE)
             {
               return L7_SUCCESS;
             }   
           }
            pimsmMapStaticRPQueue(pimsmMapCbPtr, &staticRp->rpIpAddr, 
                  &staticRp->rpGrpAddr, 
                  prefixLen, staticRp->overRide, L7_TRUE, index);
            return L7_SUCCESS;
         }
         else
         {
           return L7_SUCCESS;
         }
       }
     }
     else /* RP Address Not Equal */
     {
       if ((inetAddressAnd(&staticRp->rpGrpAddr, &staticRp->rpGrpMask, &cfgGrpRt) == L7_SUCCESS) &&
           (inetAddressAnd(rpGrpAddr, &rpGrpMask, &paramGrpRt)== L7_SUCCESS))
       {
         if (L7_INET_IS_ADDR_EQUAL(&cfgGrpRt, &paramGrpRt))
         {
           PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,"Error! Trying to configure "
                            "a Duplicate RP Address for the same Group Range.");
           return L7_ALREADY_CONFIGURED;
         }
       }
     }

     if ((freeSlotFound == L7_FALSE) && 
         (inetIsAddressZero(&staticRp->rpIpAddr) == L7_TRUE))
     {
       freeSlotFound = L7_TRUE;
       freeSlot = index;
     }
   }
   if ((index >= L7_MAX_STATIC_RP_NUM) && (freeSlotFound != L7_TRUE))
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Index %d",index);
     return L7_TABLE_IS_FULL;
   }
  
   freeStaticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[freeSlot];

   freeStaticRp->overRide = conflict;
   if (inetCopy(&freeStaticRp->rpIpAddr, rpIpAddr) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }
   if (inetCopy(&freeStaticRp->rpGrpAddr, rpGrpAddr) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }
   if (inetCopy(&freeStaticRp->rpGrpMask, &rpGrpMask) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }
   pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
   pimsmMapStaticRPQueue(pimsmMapCbPtr, 
                  rpIpAddr, rpGrpAddr, 
                  prefixLen, conflict, L7_TRUE, freeSlot);

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, 
                   "Function Exit: SUCCESS.\n");
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Removes the static RP information
*
* @param    familyType      @b{(input)}     Address Family type
* @param    rpIpAddr        @b{(input)}     Ip address of the RP
* @param    rpGrpAddr       @b{(input)}     Group address supported by the RP
* @param    prefixLen       @b{(input)}     prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPRemove(L7_uchar8 familyType, L7_inet_addr_t *rpIpAddr, 
    L7_inet_addr_t *rpGrpAddr, L7_uchar8 prefixLen)
{
   L7_uint32           index;
   pimsmMapCB_t        *pimsmMapCbPtr = L7_NULLPTR;
   pimsmCfgStaticRP_t  *staticRp = L7_NULLPTR;
   L7_inet_addr_t      rpGrpMask;
   L7_uchar8           rpAddr[IPV6_DISP_ADDR_LEN];
   L7_uchar8           grpAddr[IPV6_DISP_ADDR_LEN];

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   /* this fails if PIM-SM component not started by configurator */
   if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
     return L7_FAILURE;
   }
   if (inetMaskLenToMask(familyType, prefixLen, &rpGrpMask) != L7_SUCCESS)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                        prefixLen);
      return L7_FAILURE;
   }
 
   if (inetIsAddressZero(rpIpAddr) == L7_TRUE)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid RP address(%s)",
                        inetAddrPrint(rpIpAddr,rpAddr));
      return L7_FAILURE;
   }
   
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,
                   "Code  Flow\n");
   /* Check if it is same as configured info. */
   for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
   {
     staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];
     if (L7_INET_IS_ADDR_EQUAL(&staticRp->rpIpAddr, rpIpAddr) &&
         (L7_INET_IS_ADDR_EQUAL(&staticRp->rpGrpAddr, rpGrpAddr)) &&
         (L7_INET_IS_ADDR_EQUAL(&staticRp->rpGrpMask, &rpGrpMask)))
     {
       if (inetAddressZeroSet(familyType, &staticRp->rpIpAddr) != L7_SUCCESS)
       {
         return L7_FAILURE;
       }
       if (inetAddressZeroSet(familyType, &staticRp->rpGrpAddr) != L7_SUCCESS)
       {
         return L7_FAILURE;
       }
       if (inetAddressZeroSet(familyType, &staticRp->rpGrpMask) != L7_SUCCESS)
       {
         return L7_FAILURE;
       }
       staticRp->overRide = L7_FALSE;
       pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,
                        "Code  Flow\n");
       if (pimsmMapCbPtr->pPimsmInfo->pimsmStaticRPOper[index] == L7_FALSE)
       {
         return L7_SUCCESS;
       }   
       pimsmMapStaticRPQueue(pimsmMapCbPtr, 
                  rpIpAddr, rpGrpAddr, 
                  prefixLen, L7_FALSE, L7_FALSE, index);
       return L7_SUCCESS;
     }
   }
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nThis static Rp and group"
   "route combination could not be removed\n");
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to remove static RP(%s) for \
                     Grp Address(%s)",inetAddrPrint(rpIpAddr,rpAddr),
                     inetAddrPrint(rpGrpAddr,grpAddr));
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the static RP entry.
*
* @param    familyType     @b{(input)}      Address Family type
* @param    rpGrpAddr      @b{(input)}      Group address supported by the RP
* @param    prefixLen      @b{(input)}      prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPEntryGet(L7_uchar8 familyType, 
                                 L7_inet_addr_t *rpGrpAddr, 
                                 L7_uchar8 prefixLen)
{
    pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
    pimsmCfgStaticRP_t *staticRp = L7_NULLPTR;
    L7_uint32          index = L7_NULL;
    L7_inet_addr_t     rpGrpMask;
    L7_uchar8          grpAddr[IPV6_DISP_ADDR_LEN];

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    inetAddressZeroSet(familyType, &rpGrpMask);
    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }
    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
       return L7_FAILURE;
    }
    if (inetMaskLenToMask(familyType, prefixLen, &rpGrpMask) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                        prefixLen);
      return L7_FAILURE;
    }
   
    for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
    {
      staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];
      if ((L7_INET_IS_ADDR_EQUAL(&staticRp->rpGrpAddr, rpGrpAddr)) &&
          (L7_INET_IS_ADDR_EQUAL(&staticRp->rpGrpMask, &rpGrpMask)))
      {
         return L7_SUCCESS;
      }
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get static RP entry for \
                     Grp Address(%s)",inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Gets the next static RP information after the supplied info
*
* @param    familyType       @b{(input)}    Address Family type
* @param    rpGrpAddr        @b{(input)}    Group address supported by the RP
* @param    prefixLen        @b{(output)}   prefix Length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPNextGet(L7_uchar8 familyType, 
                                L7_inet_addr_t *rpGrpAddr, 
                                L7_uchar8 *prefixLen)
{
    L7_uint32          index;
    pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
    pimsmCfgStaticRP_t *staticRp = L7_NULLPTR;
    L7_inet_addr_t     grpAddrFind,grpAddrLocal;
    L7_uchar8          grpPrefixLenLocal,grpPrefixLenFind;
    L7_BOOL            foundEntry = L7_FALSE;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* Validations */
    if (rpGrpAddr == L7_NULLPTR)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Input RP Group Address is NULL");
       return L7_FAILURE;
    }
    if (prefixLen == L7_NULLPTR)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Input Prefix Length is NULL");
       return L7_FAILURE;
    }

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }

    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
       return L7_FAILURE;
    }

    /* Assuming no valid entry is found.*/
    inetAddressZeroSet(familyType, &grpAddrFind);
    grpPrefixLenFind = L7_NULL;
    foundEntry = L7_FALSE;


    for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
    {
      staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];

      inetCopy(&grpAddrLocal, &staticRp->rpGrpAddr);
      inetMaskToMaskLen(&staticRp->rpGrpMask,&grpPrefixLenLocal);

      if (inetIsAddressZero(&grpAddrLocal) == L7_TRUE)
      {
        continue;
      }

      if (foundEntry == L7_FALSE &&
          (inetAddrCompareAddrWithMaskIndividual(&grpAddrLocal,grpPrefixLenLocal,rpGrpAddr, 
                                              *prefixLen) > L7_NULL))
      {
        inetCopy(&grpAddrFind, &grpAddrLocal);
        grpPrefixLenFind = grpPrefixLenLocal;
        foundEntry = L7_TRUE;
      }
      else if (foundEntry == L7_TRUE &&
               (inetAddrCompareAddrWithMaskIndividual(&grpAddrLocal,grpPrefixLenLocal,&grpAddrFind, 
                                              grpPrefixLenFind) < L7_NULL) &&
               (inetAddrCompareAddrWithMaskIndividual(&grpAddrLocal,grpPrefixLenLocal,rpGrpAddr, 
                                              *prefixLen) > L7_NULL))
      {
        inetCopy(&grpAddrFind, &grpAddrLocal);
        grpPrefixLenFind = grpPrefixLenLocal;
      }
    }

    if (foundEntry == L7_FALSE) /* No Next entry found */
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES," Static RP next entry not found");
      return L7_FAILURE;
    }
    inetCopy(rpGrpAddr,&grpAddrFind);
    *prefixLen = grpPrefixLenFind;
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, 
                    "\nFunction Exit.\n");
    return L7_SUCCESS;

}


/*********************************************************************
* @purpose  To get the StaticRPOverrideDynamic object.
*
* @param    familyType   @b{(input)}  Address Family type.
* @param    rpGrpAddr    @b{(input)}  static RP Group address.
* @param    prefixLength @b{(input)}  prefix Length.
* @param    overRide     @b{(output)} OverrideDynamic.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPOverrideDynamicGet(L7_uchar8 familyType, 
                                           L7_inet_addr_t *rpGrpAddr,
                                           L7_uchar8 prefixLength,
                                           L7_uint32 *overRide)
{
    pimsmCfgStaticRP_t *staticRpEntry;
    L7_uint32          rpIndex = L7_NULL;
    L7_uchar8          grpAddr[IPV6_DISP_ADDR_LEN];

    if (pimsmMapStaticRpEntryGet(familyType, rpGrpAddr, 
                prefixLength, &rpIndex, &staticRpEntry) == L7_SUCCESS)
    {
      *overRide = staticRpEntry->overRide;
      return L7_SUCCESS;
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find next Static RP entry for \
                     Grp Address(%s)",inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  To set the StaticRPOverrideDynamic object.
*
* @param    familyType   @b{(input)}  Address Family type.
* @param    rpGrpAddr    @b{(input)}  static RP Group address.
* @param    prefixLength @b{(input)}  prefix Length.
* @param    overRide     @b{(input)}  OverrideDynamic.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPOverrideDynamicSet(L7_uchar8 familyType, 
                                           L7_inet_addr_t *rpGrpAddr,
                                           L7_uchar8 prefixLength,
                                           L7_uint32 overRide)
{
    pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
    pimsmCfgStaticRP_t *staticRp = L7_NULLPTR;
    L7_uint32          index = L7_NULL;
    L7_inet_addr_t     rpGrpMask;
    L7_uchar8          grpAddr[IPV6_DISP_ADDR_LEN];
    L7_uchar8          maskAddr[IPV6_DISP_ADDR_LEN];


    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    inetAddressZeroSet(familyType, &rpGrpMask);
    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }
    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
       return L7_FAILURE;
    }

    if (inetMaskLenToMask(familyType, prefixLength, &rpGrpMask) != L7_SUCCESS)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                        prefixLength);
       return L7_FAILURE;
    }

    if ((inetIsAddressZero(rpGrpAddr) == L7_TRUE) || 
        (inetIsAddressZero(&rpGrpMask) == L7_TRUE))
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid rpGrpAddr(%s) or invalid \
                         rpGrpMask (%s)",inetAddrPrint(rpGrpAddr,grpAddr),
                         inetAddrPrint(&rpGrpMask,maskAddr));
       return L7_FAILURE;
    }

    for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
    {
      staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];
      if ((L7_INET_IS_ADDR_EQUAL(&staticRp->rpGrpAddr, rpGrpAddr)) &&
          (L7_INET_IS_ADDR_EQUAL(&staticRp->rpGrpMask, &rpGrpMask)))
      {
         staticRp->overRide = overRide;
         /* send event to vendor code. */
         return pimsmMapStaticRPApply(pimsmMapCbPtr, 
         &staticRp->rpIpAddr, &staticRp->rpGrpAddr, 
                &staticRp->rpGrpMask, overRide, index);
      }
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find RP Entry for GrpAddr(%s)",
                      inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the static RP Address.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    rpGrpAddr     @b{(input)}  static RP Group address.
* @param    prefixLength  @b{(input)}  prefix Length.
* @param    staticRpAddr  @b{(output)}  Static RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPAddrGet(L7_uchar8 familyType, 
                                L7_inet_addr_t *rpGrpAddr, 
                                L7_uchar8 prefixLength,
                                L7_inet_addr_t *staticRpAddr)
{
    pimsmCfgStaticRP_t *staticRpEntry;
    L7_uint32          rpIndex = L7_NULL;
    L7_uchar8          grpAddr[IPV6_DISP_ADDR_LEN];

    if (pimsmMapStaticRpEntryGet(familyType, rpGrpAddr, 
             prefixLength, &rpIndex, &staticRpEntry) == L7_SUCCESS)
    {
      /* copy rp address */
      inetCopy(staticRpAddr, &staticRpEntry->rpIpAddr);
      return L7_SUCCESS;
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find RP Entry for GrpAddr(%s)",
                      inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  To set the static RP Address.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    rpGrpAddr     @b{(input)}  static RP Group address.
* @param    prefixLength  @b{(input)}  prefix Length.
* @param    staticRpAddr  @b{(input)}  Static RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPAddrSet(L7_uchar8 familyType, 
                                L7_inet_addr_t *rpGrpAddr, 
                                L7_uchar8 prefixLength,
                                L7_inet_addr_t *staticRpAddr)
{
    pimsmCfgStaticRP_t *staticRpEntry;
    pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
    L7_uint32          rpIndex = L7_NULL;
    L7_RC_t            retCode = L7_FAILURE;
    L7_BOOL            conflict = L7_FALSE;

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }
    if (staticRpAddr == L7_NULLPTR)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid static Rp Address(%p)",
                         (void *)staticRpAddr);
      return L7_FAILURE;
    }

    if (pimsmMapStaticRpEntryGet(familyType, rpGrpAddr, 
             prefixLength, &rpIndex, &staticRpEntry) == L7_SUCCESS)
    {
      /* Found existing entry. Modify existing entry.*/
      inetCopy(&staticRpEntry->rpIpAddr, staticRpAddr);
      /* send event to vendor code. */
      retCode = pimsmMapStaticRPApply(pimsmMapCbPtr,  
                &staticRpEntry->rpIpAddr, &staticRpEntry->rpGrpAddr, 
                &staticRpEntry->rpGrpMask, staticRpEntry->overRide, rpIndex);
    }
    else
    {
      /* add new entry in mapping layer. */
      retCode = pimsmMapStaticRPSet(familyType, staticRpAddr, 
                                     rpGrpAddr, prefixLength, conflict);

    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "Function Exit.\n");
    return retCode;
}

/*********************************************************************
* @purpose  Gets the admin mode.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    rpGrpAddr     @b{(input)}  static RP Group address.
* @param    prefixLength  @b{(input)}  prefix Length.
* @param    rowStatus     @b{(output)}  Static RP Addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPRowStatusGet(L7_uchar8 familyType,
                                      L7_inet_addr_t *rpGrpAddr, 
                                      L7_uchar8 prefixLength, 
                                      L7_uint32 *rowStatus)
{
    pimsmCfgStaticRP_t *staticRpEntry;
    L7_uint32          rpIndex = L7_NULL;
    L7_uchar8          grpAddr[IPV6_DISP_ADDR_LEN];

    if (pimsmMapStaticRpEntryGet(familyType, rpGrpAddr, 
             prefixLength, &rpIndex, &staticRpEntry) == L7_SUCCESS)
    {
      *rowStatus = L7_TRUE;
      return L7_SUCCESS;
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find RP Entry for GrpAddr(%s)",
                      inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the admin mode.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    rpGrpAddr     @b{(input)}  static RP Group address.
* @param    prefixLength  @b{(input)}  prefix Length.
* @param    rowStatus     @b{(input)}  Static RP Addr.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPRowStatusSet(L7_uchar8 familyType,
                                     L7_inet_addr_t *rpGrpAddr, 
                                     L7_uchar8 prefixLength, 
                                     L7_uint32 rowStatus)
{
    pimsmCfgStaticRP_t *staticRpEntry;
    pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
    L7_uint32          rpIndex = L7_NULL;
    L7_uchar8          grpAddr[IPV6_DISP_ADDR_LEN];

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }

    if (pimsmMapStaticRpEntryGet(familyType, rpGrpAddr, 
             prefixLength, &rpIndex, &staticRpEntry) == L7_SUCCESS)
    {
      /* send event to vendor code. */
      return  pimsmMapStaticRPApply(pimsmMapCbPtr, 
                &staticRpEntry->rpIpAddr, &staticRpEntry->rpGrpAddr,
                &staticRpEntry->rpGrpMask, staticRpEntry->overRide, rpIndex);
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find RP Entry for GrpAddr(%s)",
                      inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the number of static RP entries
*
* @param    familyType  @b{(input)}   Address Family type
* @param    num         @b{(output)}  Number of entries.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNumStaticRPEntriesGet(L7_uchar8 familyType,
                                      L7_uint32 *num)
{

  /*This API should not be called to get Max Number of Static RP Entries */
  #ifdef PIMSM_TBD
    pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
    pimsmCfgStaticRP_t *staticRp = L7_NULLPTR;
    L7_uint32           index, tmpNum;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }

    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
       return L7_FAILURE;
    }

    if (num == L7_NULLPTR)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid number of entries(%p)", (void *)num);
       return L7_FAILURE;
    }
    
    tmpNum = 0;
    for (index = 0; index < L7_MAX_STATIC_RP_NUM; index++)
    {
      staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];

      if ((inetIsAddressZero(&staticRp->rpIpAddr) == L7_TRUE) ||
          (inetIsAddressZero(&staticRp->rpGrpAddr) == L7_TRUE)||
          (inetIsAddressZero(&staticRp->rpGrpMask) == L7_TRUE))
      {
        continue;
      }
      tmpNum += 1;
    }

    *num = tmpNum;
    #endif
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the PIMSM admin mode for the specified interface
*
* @param    pimsmMapCbPtr @b{(input)} Mapping Control Block.
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    mode          @b{(input)} Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes:
*           1. PIMSM is enabled
*           2. Interface has been created and configured for PIMSM at
*              the PIMSM vendor layer
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapInterfaceModeQueue(pimsmMapCB_t *pimsmMapCbPtr,
                           L7_uint32 intIfNum, L7_uint32 mode)
{
  pimsmMapIntfData_t data;

  memset(&data,0 , sizeof(pimsmMapIntfData_t));
  data.intIfNum= intIfNum;
  data.opaqueData= mode;
  
  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_INTF_MODE_EVENT,
      &data, sizeof(pimsmMapIntfData_t));

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set the administrative mode of a PIM-SM routing interface
*
* @param    familyType @b{(input)}  Address Family type
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    mode       @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceModeSet(L7_uchar8 familyType, 
                                 L7_uint32 intIfNum, L7_uint32 mode)
{
  pimsmCfgCkt_t   *pCfg;
  pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid mode(%d) on intIfNum(%d)",mode,
                    intIfNum);
       return L7_FAILURE;
  }

  if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
  {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
       return L7_FAILURE;
  }

  /* current admin mode already established, so nothing more to do here */
  if (mode == pCfg->mode)
  {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_CONFIGURED,
      "Mode already set\n");
      return L7_SUCCESS;
  }
  pCfg->mode = mode;
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  if (mode == L7_DISABLE)
  {
     if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational != L7_TRUE)
     {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not operational",intIfNum);
        return L7_SUCCESS;
     }
  }
  else if (mode == L7_ENABLE)
  {
    if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational == L7_TRUE)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, 
                "\nInterface already operational\n");
       return L7_SUCCESS;
    }
#if 0
    if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational != L7_TRUE)
     {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, 
                "\nglobal mode is not operational\n");
       return L7_SUCCESS;
     }
#endif
  }
  pimsmMapInterfaceModeQueue(pimsmMapCbPtr, intIfNum, mode);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the administrative mode of a PIM-SM routing interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceModeGet(L7_uchar8 familyType,
           L7_uint32 intIfNum, L7_uint32 *mode)
{
  pimsmCfgCkt_t   *pCfg;
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
    return L7_FAILURE;
  }

  if (mode != L7_NULLPTR)
  {
     *mode = pCfg->mode;
     return L7_SUCCESS;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid mode(%p) on intIfNum(%d)", (void *)mode,
                    intIfNum);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns whether PIM-SM is operational on interface or not
*
*
* @param    familyType  @b{(input)}     Address Family type
* @param    intIfNum    @b{(input)}     Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmMapIntfIsOperational(L7_uchar8 familyType,
                                  L7_uint32 intIfNum)
{
    pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FALSE;
    }

    if (pimsmMapCbPtr->pPimsmIntfInfo == L7_NULL)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component was not \
                         started on intIfNum(%d)",intIfNum);
    }
    else
    {
       if(pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational == L7_TRUE)
       {
          return L7_TRUE;
       }
       else
       {
         PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not \
                         started on intIfNum(%d)",intIfNum);
         return L7_FALSE;
       }
    }

    return L7_FALSE;
}

/*********************************************************************
* @purpose  Set the hello interval for an interface
*
* @param    pimsmMapCbPtr        Mapping Control Block.
* @param    intIfNum             Interface num.
* @param    helloIntvl           Hello Interval in secs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapInterfaceHelloIntervalQueue(pimsmMapCB_t *pimsmMapCbPtr,
                           L7_uint32 intIfNum, L7_uint32 helloIntvl)
{
  pimsmMapIntfData_t data;

  memset(&data,0 , sizeof(pimsmMapIntfData_t));
  data.intIfNum= intIfNum;
  data.opaqueData= helloIntvl;
  
  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_INTF_HELLO_INTV_EVENT,
      &data, sizeof(pimsmMapIntfData_t));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Hello interval of a PIM-SM routing interface
*
* @param    familyType  @b{(input)}   Address Family type
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    helloIntvl  @b{(input)}   Hello Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceHelloIntervalSet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum, 
                                          L7_uint32 helloIntvl)
{
  pimsmCfgCkt_t   *pCfg;
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
    return L7_FAILURE;
  }
  /* range checking */
  if ((helloIntvl < L7_PIMSM_INTERFACE_HELLO_INTERVAL_MIN) ||
      (helloIntvl > L7_PIMSM_INTERFACE_HELLO_INTERVAL_MAX) )
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid hello interval(%d)",helloIntvl);
    return L7_FAILURE;
  }
  if (helloIntvl == pCfg->intfHelloInterval)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_CONFIGURED,
    "hello interval already set\n");

    return L7_SUCCESS;
  }

  pCfg->intfHelloInterval = helloIntvl;
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational != L7_TRUE)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :PIM-SM not:"
    "operational config paramter stored but not applied\n", 
    __FUNCTION__, __LINE__);
  }
  pimsmMapInterfaceHelloIntervalQueue(pimsmMapCbPtr, intIfNum, 
                                                  helloIntvl);
  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  Get the Hello interval of a PIM-SM routing interface
*
* @param    familyType  @b{(input)}   Address Family type
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    helloIntvl  @b{(output)}  Hello Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceHelloIntervalGet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum, 
                                          L7_uint32 *helloIntvl)
{
  pimsmCfgCkt_t   *pCfg;
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
    return L7_FAILURE;
  }

  if (helloIntvl != L7_NULLPTR)
  {
     *helloIntvl= pCfg->intfHelloInterval;
     return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the join prune interval for an interface
*
* @param    pimsmMapCbPtr        Mapping Control Block.
* @param    intIfNum             Interface num.
* @param    joinPruneIntvl       Join-prune Interval in secs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapInterfaceJoinPruneIntervalQueue(pimsmMapCB_t *pimsmMapCbPtr, 
          L7_uint32 intIfNum, L7_uint32 joinPruneIntvl)
{
  pimsmMapIntfData_t data;

  memset(&data,0 , sizeof(pimsmMapIntfData_t));
  data.intIfNum= intIfNum;
  data.opaqueData= joinPruneIntvl;
  
  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_INTF_JP_INTV_EVENT,
      &data, sizeof(pimsmMapIntfData_t));

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the Join/Prune Interval for the specified interface
*
* @param    familyType     @b{(input)}     Address Family type
* @param    intIfNum       @b{(input)}     Internal Interface Number
* @param    joinpruneIntvl @b{(input)}     Join/Prune Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceJoinPruneIntervalSet(L7_uchar8 familyType, 
                                              L7_uint32 intIfNum, 
                                              L7_uint32 joinpruneIntvl)
{
   pimsmCfgCkt_t   *pCfg;
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
     return L7_FAILURE;
   }
   if ((joinpruneIntvl < L7_PIMSM_INTERFACE_JOINPRUNE_INTERVAL_MIN) ||
       (joinpruneIntvl > L7_PIMSM_INTERFACE_JOINPRUNE_INTERVAL_MAX))
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Join/Prune Interval(%d) in seconds",
                        joinpruneIntvl);
      return L7_FAILURE;
   }
   if (joinpruneIntvl == pCfg->intfJoinPruneInterval)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_CONFIGURED, 
      "\njoin/prune interval already set\n");
      return L7_SUCCESS;
   }
   pCfg->intfJoinPruneInterval = joinpruneIntvl;
   pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
   if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational != L7_TRUE)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :PIM-SM not:"
     "operational config paramter stored but not applied\n", 
     __FUNCTION__, __LINE__);
   }
   pimsmMapInterfaceJoinPruneIntervalQueue(pimsmMapCbPtr, intIfNum, 
                                                  joinpruneIntvl);
   return L7_SUCCESS;   
}

/*********************************************************************
* @purpose  Gets the Join/Prune Interval for the specified interface
*
* @param    familyType      @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    joinpruneIntvl  @b{(output)}  Join/Prune Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceJoinPruneIntervalGet(L7_uchar8 familyType, 
                                              L7_uint32 intIfNum, 
                                              L7_uint32 *joinpruneIntvl)
{
   pimsmCfgCkt_t   *pCfg;
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
      return L7_FAILURE;
   }

   if (joinpruneIntvl != L7_NULLPTR)
   {
      *joinpruneIntvl = pCfg->intfJoinPruneInterval;
      return L7_SUCCESS;
   }
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Join/Prune Interval(%p)",
                     (void *)joinpruneIntvl);
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the IP Address for the specified interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Interface Number
* @param    inetIpAddr  @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceIPAddressGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,  
                                      L7_inet_addr_t *inetIpAddr)
{
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   if (inetIpAddr != L7_NULLPTR)
   {
     if (mcastIpMapRtrIntfIpAddressGet(familyType, 
                                       intIfNum, inetIpAddr) == L7_SUCCESS)
     {
       return L7_SUCCESS;
     }
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get IP address on intIfNum(%d)",
                       intIfNum); 
   }
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Subnet Mask for the specified interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Interface Number
* @param    netMask     @b{(output)} Subnet Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceNetMaskGet(L7_uchar8 familyType, 
                                    L7_uint32 intIfNum,
                                    L7_inet_addr_t *inetNetMask)
{
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (inetNetMask != L7_NULLPTR)
    {
      if (mcastIpMapRtrIntfNetMaskGet(familyType, 
                                      intIfNum, inetNetMask) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get Subnet Mask for intIfNum(%d)",
                        intIfNum); 
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the DR IP address for the specified specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  Internal Interface Number
* @param    dsgRtrIpAddr      @b{(output)} IP Address of the DR
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceDRGet(L7_uchar8 familyType,
                               L7_uint32 intIfNum, 
                               L7_inet_addr_t *dsgRtrIpAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (dsgRtrIpAddr != L7_NULLPTR)
  {
    return pimsmMapExtenInterfaceDRGet(pimsmMapCbPtr,
                             intIfNum, dsgRtrIpAddr);
  }
  else
  {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid DR Ip Address(%p)",
                        (void *)dsgRtrIpAddr); 
      return L7_FAILURE;
  }
}


/*********************************************************************
* @purpose  To get the candidate BSR Entry for given family.
*
* @param    familyType     @b{(input)}  Address Family type.
* @param    cbsrEntry      @b{(output)} candidate Bsr Entry.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapCbsrEntryGet(L7_uchar8 familyType, 
                                     pimsmCfgCandBSRdata_t **cbsrEntry)
{
   pimsmMapCB_t          *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   /* this fails if PIM-SM component not started by configurator */
   if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
      return L7_FAILURE;
   }

   if (pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr.intIfNum <=0)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidateBsr  is not configured");
     return L7_FAILURE;
   }
  
   *cbsrEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, 
    "\nFunction Exit.\n");
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Check whether candidateBSR entry exists.
*
* @param    familyType       @b{(input)}  Address Family type
*
* @returns  L7_SUCCESS if entry exists
* @returns  L7_FAILURE if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSREntryGet(L7_uchar8 familyType)
{
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   /* this fails if PIM-SM component not started by configurator */
   if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
      return L7_FAILURE;
   }

   /*Interface Number will be greater than 0 only if cand BSR is configured */
   if(pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr.intIfNum > 0)
     return L7_SUCCESS;
    

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate Bsr is not found.\n");
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain the next entry in the bsrCandidateRP TABLE
*
* @param    familyType       @b{(input)}  Address Family type
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSREntryNextGet(L7_uchar8 familyType)
{
  /* only one entry exist in candidate BSR */
  return pimsmMapCandBSREntryGet(familyType);
}

/*********************************************************************
* @purpose  To get the Candidate BSR interface.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    interface        @b{(output)} interface number.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRInterfaceGet(L7_uchar8 familyType, 
                                         L7_uint32 *interface)
{
  pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
     /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }
   /* found entry */
     *interface = pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr.intIfNum;
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  To get the Candidate BSR Address.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    cbsrAddr         @b{(output)} To get bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRAddrGet(L7_uchar8 familyType, 
                                    L7_inet_addr_t *cbsrAddr)
{
   pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;


   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }
   
    /* found entry */
    inetCopy(cbsrAddr, 
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To set the candidate BSR Address.
*
* @param    familyType       @b{(input)} Address Family type
* @param    cbsrAddr         @b{(input)} bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRAddrSet(L7_uchar8 familyType, 
                                    L7_inet_addr_t *cbsrAddr)
{
#ifdef PIMSM_MAP_TBD
   pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;
   pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;


   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }
   candidateBsr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr;
   pimsmMapCandBSRApply(pimsmMapCbPtr, 
                                    cbsrAddr,
                                    candidateBsr->candBSRPriority, 
                                    L7_ENABLE, 
                                    candidateBsr->candBSRHashMaskLength, 
                                    candidateBsr->candBSRScope);
   return L7_SUCCESS;
#endif
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the Candidate BSR priority.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    cbsrPriority     @b{(output)} To get bsr priority.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRPriorityGet(L7_uchar8 familyType, 
                                        L7_uint32 *cbsrPriority)
{
  pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;

  if (pimsmMapCbsrEntryGet(familyType, &candidateBsr) == L7_SUCCESS)
  {
    /* found entry */
    *cbsrPriority = candidateBsr->candBSRPriority;
    return L7_SUCCESS;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate Bsr is not found");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To set the bsrCandtBSR priority.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    cbsrPriority     @b{(input)}  bsr priority.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRPrioritySet(L7_uchar8 familyType, 
                                        L7_uint32 cbsrPriority)
{
#ifdef PIMSM_MAP_TBD   
   pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;
   pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   if (pimsmMapCbsrEntryGet(familyType, &candidateBsr) == L7_SUCCESS)
   {
     /* found entry */
     return pimsmMapCandBSRApply(pimsmMapCbPtr, 
            &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress,
                                     candidateBsr->candBSRPriority, 
                                     L7_ENABLE, 
                                     candidateBsr->candBSRHashMaskLength, 
                                     candidateBsr->candBSRScope);
   }
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate Bsr is not found");
   return L7_FAILURE;
#endif
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  To get the bsrCandtBSR hashMaskLen.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    hashMaskLen      @b{(output)} get Hash mask Length.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRHashMaskLengthGet(L7_uchar8 familyType, 
                                              L7_uint32 *hashMaskLen)
{
  pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;

  if (pimsmMapCbsrEntryGet(familyType, &candidateBsr) == L7_SUCCESS)
  {
    /* found entry */
    *hashMaskLen = candidateBsr->candBSRHashMaskLength;
     return L7_SUCCESS;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate Bsr is not found");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To set the bsrCandtBSR hashMaskLen.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    hashMaskLen      @b{(output)} get Hash mask Length.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRHashMaskLengthSet(L7_uchar8 familyType, 
                                              L7_uint32 hashMaskLen)
{

   #ifdef PIMSM_MAP_TBD   
   pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;
   pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   if (pimsmMapCbsrEntryGet(familyType, &candidateBsr) == L7_SUCCESS)
   {
     /* found entry */
     candidateBsr->candBSRHashMaskLength = hashMaskLen;
     return pimsmMapCandBSRApply(pimsmMapCbPtr, 
             &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress,
                                     candidateBsr->candBSRPriority, 
                                     L7_ENABLE, 
                                     candidateBsr->candBSRHashMaskLength, 
                                     candidateBsr->candBSRScope);
   }
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate Bsr is not found");
   return L7_FAILURE;
   #endif
   return L7_FAILURE;
 
}

/*********************************************************************
* @purpose  Gets the admin mode for the BSR Candidate.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    mode             @b{(output)} get bsr mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRModeGet(L7_uchar8 familyType, 
                                    L7_uint32 *mode)
{
  pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

    /* found entry */
    *mode = pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.operMode;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the admin mode for the BSR Candidate.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    mode             @b{(input)}  BSR mode.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRModeSet(L7_uchar8 familyType, 
                                    L7_uint32 mode)
{
#ifdef PIMSM_MAP_TBD     
   pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;
   pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   if (pimsmMapCbsrEntryGet(familyType, &candidateBsr) == L7_SUCCESS)
   {
     /* found entry */
     return pimsmMapCandBSRApply(pimsmMapCbPtr, 
              &pimsmMapCbPtr->pPimsmInfo->pimsmCandBSROper.candBSRAddress,
                                     candidateBsr->candBSRPriority, 
                                     mode, 
                                     candidateBsr->candBSRHashMaskLength, 
                                     candidateBsr->candBSRScope);
   }
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate Bsr is not found");
   return L7_FAILURE;
#endif
   return L7_FAILURE;
}


/*********************************************************************
* @purpose  To get the bsrCandidate Bootstrap Timer.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    bsTimer          @b{(output)} BootStap Timer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRBootstrapTimerGet(L7_uchar8 familyType, 
                                              L7_uint32 *bsTimer)
{
  pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;

  if (pimsmMapCbsrEntryGet(familyType, &candidateBsr) == L7_SUCCESS)
  {
    /* found entry */
    /*TBD*/
    return L7_SUCCESS;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate Bsr is not found");
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  To get the  electedBSR.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    electedBsr       @b{(output)} BSR is elected/not.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRElectedBSRGet(L7_uchar8 familyType, 
                                          L7_uint32 *electedBsr)
{
  pimsmCfgCandBSRdata_t *candidateBsr = L7_NULLPTR;

  if (pimsmMapCbsrEntryGet(familyType, &candidateBsr) == L7_SUCCESS)
  {
    /* found entry */
    /* To be confirmed. */
    /* *electedBsr = candidateBsr->electedBsr; */  
    return L7_SUCCESS;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate Bsr is not found");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Elected BSR Priority. 
*
* @param    familyType       @b{(input)}  Address Family type
* @param    elecBsrPriority  @b{(output)} Elected BSR Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapElectedBSRPriorityGet(L7_uchar8 familyType,
                                      L7_int32 *elecBsrPriority)
{
    pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }

    if (elecBsrPriority != L7_NULLPTR)
    {
       return pimsmMapExtenElectedBsrPriroityGet(pimsmMapCbPtr, 
                                                 elecBsrPriority);
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid  Elected BSR Priority(%p)",
                     (void *) elecBsrPriority);
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Elected BSR Hash mask Length for the specified 
*           interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    hashMskLen     @b{(output)} Elected BSR Hash mask Length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapElectedBSRHashMaskLengthGet(L7_uchar8 familyType, 
                                            L7_uint32 *hashMskLen)
{
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   if (hashMskLen != L7_NULLPTR)
   {
       return pimsmMapExtenElectedBsrHashMaskLengthGet(pimsmMapCbPtr, 
                                                      hashMskLen);
   }
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid BSR Hash mask Length(%p)",(void *)hashMskLen);
   return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Uptime for the specified neighbour
* 
* @param    familyType   @b{(input)}   Address Family type
* @param    intIfNum     @b{(input)}   Internal Interface Number
* @param    nbrIpAddr    @b{(input)}   IP Address of Neighbor
* @param    nbrUpTime    @b{(output)}  Uptime in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborUpTimeGet(L7_uchar8 familyType, 
          L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr,
          L7_uint32 *nbrUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (nbrUpTime != L7_NULLPTR)
  {
     return pimsmMapExtenNeighborUpTimeGet(pimsmMapCbPtr, intIfNum,
                                           nbrIpAddr, nbrUpTime);
  }
  else
  {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Uptime(%p)",(void *)nbrUpTime); 
      return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Get the Expiry time for the specified neighbour
*
* @param    familyType    @b{(input)}    Address Family type
* @param    intIfNum      @b{(input)}    Internal Interface Number
* @param    nbrIpAddr     @b{(input)}    IP Address of Neighbor
* @param    nbrExpiryTime @b{(output)}   Expiry time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborExpiryTimeGet(L7_uchar8 familyType,
             L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr,
             L7_uint32 *nbrExpiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (nbrExpiryTime != L7_NULLPTR)
  {
      return pimsmMapExtenNeighborExpiryTimeGet(pimsmMapCbPtr, intIfNum, 
                                                nbrIpAddr, nbrExpiryTime);
  }
  else
  {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Uptime(%p)",(void *)nbrExpiryTime); 
      return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Get the DR Priority for the specified neighbour
*
* @param    familyType  @b{(input)}    Address Family type 
* @param    intIfNum    @b{(input)}    Internal Interface Number
* @param    nbrIpAddr   @b{(input)}    IP Address of Neighbor
* @param    drPriority  @b{(output)}    DR Priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborDrPriorityGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum, 
                                      L7_inet_addr_t *nbrIpAddr,
                                      L7_uint32 *drPriority)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (drPriority != L7_NULLPTR)
  {
     return pimsmMapExtenNeighborDrPriorityGet(pimsmMapCbPtr, intIfNum,
                                               nbrIpAddr, drPriority);
  }
  else
  {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid DR Priority(%p)",(void *)drPriority);
      return L7_FAILURE;
  }
}
/*********************************************************************
* @purpose  Get the Neighbor count for specified interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    nbrCount     @b{(output)} Neighbor Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborCountGet(L7_uchar8 familyType,
          L7_uint32 intIfNum, L7_uint32 *nbrCount)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (nbrCount != L7_NULLPTR)
  {
     return pimsmMapExtenNeighborCountGet(pimsmMapCbPtr,
                                    intIfNum, nbrCount);
  }
  else
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Neighbor Count(%p)",(void *)nbrCount);
     return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    familyType   @b{(input)} Address Family type
* @param    rtrIntNum    @b{(input)} Router interface on which neighbor is lost.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimTrapNeighborLoss(L7_uchar8 familyType, L7_uint32 rtrIntNum, L7_uint32 neighborUpTime)
{
    L7_uint32 intIfNum;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if (mcastIpMapRtrIntfToIntIfNum(familyType, 
                                  rtrIntNum, &intIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert rtrIfNum(%d) to \
                        intIfNum",rtrIntNum);
      return L7_FAILURE;
    }
     
    return trapMgrPimTrapNeighborLoss(intIfNum, neighborUpTime, L7_TRUE);
}

/*********************************************************************
* @purpose  Get time remaining before the router changes its upstream
*           neighbor back to its RPF neighbor
*
* @param    familyType         @b{(input)}   Address Family type
* @param    ipMRouteGroup      @b{(input)}   IP multicast group address
* @param    ipMRouteSource     @b{(input)}   Network Address
* @param    ipMRouteSourceMask @b{(input)}   Network Mask
* @param    assertTimer        @b{(output)}  Assert timer in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteUpstreamAssertTimerGet(L7_uchar8 familyType, 
                                            L7_inet_addr_t *ipMRouteGroup, 
                                            L7_inet_addr_t *ipMRouteSource, 
                                            L7_inet_addr_t *ipMRouteSourceMask, 
                                            L7_uint32 *assertTimer)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD   
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (assertTimer != L7_NULLPTR)
  {
    return pimsmMapExtenIpMRouteUpstreamAssertTimerGet(pimsmMapCbPtr,
    ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, assertTimer);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Assert timer.\n");
    return L7_FAILURE;
  }
#endif
}

/*********************************************************************
* @purpose  Get metric advertised by the assert winner on the upstream
*           interface, or 0 if no such assert is in received
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask
* @param    assertMetric        @b{(output)} Metric value advertised
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteAssertMetricGet(L7_uchar8 familyType, 
                                        L7_inet_addr_t *ipMRouteGroup, 
                                        L7_inet_addr_t *ipMRouteSource, 
                                        L7_inet_addr_t *ipMRouteSourceMask, 
                                        L7_uint32 *assertMetric)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD  
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (assertMetric != L7_NULLPTR)
  {
     return pimsmMapExtenIpMRouteAssertMetricGet(pimsmMapCbPtr, 
     ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, assertMetric);
  }
  else
  { 
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: assertMetric is not Null Pointer\n");
     return L7_FAILURE;
  }
#endif  
}

/*********************************************************************
* @purpose  Get preference advertised by the assert winner on the upstream
*           interface, or 0 if no such assert is in received
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask
* @param    assertMetricPref    @b{(output)} Metric preference advertised
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteAssertMetricPrefGet(L7_uchar8 familyType,
     L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
     L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetricPref)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD  
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (assertMetricPref != L7_NULLPTR)
  {
    return pimsmMapExtenIpMRouteAssertMetricPrefGet(pimsmMapCbPtr,
    ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask,assertMetricPref);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Metric preference.\n");
    return L7_FAILURE;
  }
#endif
}

/*********************************************************************
* @purpose  Get the value of the RPT-bit advertised by the assert winner on
*           the upstream interface, or false if no such assert is in effect
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask
* @param    assertRPTBit        @b{(output)} Value of RPT bit (1 or 0)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteAssertRPTBitGet(L7_uchar8 familyType, 
                                        L7_inet_addr_t *ipMRouteGroup, 
                                        L7_inet_addr_t *ipMRouteSource,
                                        L7_inet_addr_t *ipMRouteSourceMask, 
                                        L7_uint32 *assertRPTBit)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD  
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (assertRPTBit != L7_NULLPTR)
  {
    return pimsmMapExtenIpMRouteAssertRPTBitGet(pimsmMapCbPtr,
    ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, assertRPTBit);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Value of RPT bit.\n");
    return L7_FAILURE;
  }
#endif
}

/*********************************************************************
* @purpose  Get PIM-specific flags related to a multicast state entry
*
* @param    familyType         @b{(input)} Address Family type
* @param    ipMRouteGroup      @b{(input)} IP multicast group address
* @param    ipMRouteSource     @b{(input)} Network Address
* @param    ipMRouteSourceMask @b{(input)} Network Mask
* @param    ipMRouteFlags      @b{(output)}Flag value is 0 or 1
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments Flag is L7_MCAST_PIMSM_FLAG_RPT (0) for RP shared tree, 
*                   L7_MCAST_PIMSM_FLAG_SPT (1) for source tree
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteFlagsGet(L7_uchar8 familyType, 
                                 L7_inet_addr_t *ipMRouteGroup, 
                                 L7_inet_addr_t *ipMRouteSource, 
                                 L7_inet_addr_t *ipMRouteSourceMask, 
                                 L7_uint32 *ipMRouteFlags)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (ipMRouteFlags != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      return pimsmMapStarGFlagsGet(familyType, ipMRouteGroup, ipMRouteFlags);
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      return pimsmMapSGFlagsGet(familyType, ipMRouteGroup, ipMRouteSource, ipMRouteFlags);
    }

  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid ipMRouteFlags.\n");
    return L7_FAILURE;
  }
  return  L7_SUCCESS;
}
/************************************************************************
* @purpose  Get the holdtime of a Candidate-RP.  If the local router is not
*           the BSR, this value is 0.
*
* @param    familyType        @b{(input)}  Address Family type
* @param    rpSetGroupAddress @b{(input)}  IP multicast group address
* @param    rpSetGroupMask    @b{(input)}  Multicast group address mask
* @param    rpSetAddress      @b{(input)}  IP address of the Candidate-RP
* @param    rpSetComponent    @b{(input)}  Number uniquely identifying the 
*                                          component
* @param    rpSetHoldTime     @b{(output)} Hold time of C-RP in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapRPSetHoldTimeGet(L7_uchar8 familyType, 
    L7_inet_addr_t *rpSetGroupAddress, L7_inet_addr_t *rpSetGroupMask, 
    L7_inet_addr_t *rpSetAddress, L7_uint32 rpSetComponent, 
    L7_uint32 *rpSetHoldTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (rpSetHoldTime != L7_NULLPTR)
  {
    return pimsmMapExtenRPSetHoldTimeGet(pimsmMapCbPtr, 
          rpSetGroupAddress, rpSetGroupMask, rpSetAddress, 
          rpSetComponent, rpSetHoldTime);

  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid value of rp-set holdtime(%p)",
                      (void *)rpSetHoldTime);
    return L7_FAILURE;
  }
}

/************************************************************************
* @purpose  Get the minimum time remaining before the Candidate-RP will be
*           declared down
*
* @param    familyType         @b{(input)} Address Family type
* @param    rpSetGroupAddress  @b{(input)} IP multicast group address
* @param    rpSetGroupMask     @b{(input)} Multicast group address mask
* @param    rpSetAddress       @b{(input)} IP address of the Candidate-RP
* @param    rpSetComponent     @b{(input)} Number uniquely identifying the 
*                                          component
* @param    rpSetExpiryTime    @b{(output)} Expiry time of C-RP in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapRPSetExpiryTimeGet(L7_uchar8 familyType, 
    L7_inet_addr_t *rpSetGroupAddress, L7_inet_addr_t *rpSetGroupMask, 
    L7_inet_addr_t *rpSetAddress, L7_uint32 rpSetComponent, 
    L7_uint32 *rpSetExpiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (rpSetExpiryTime != L7_NULLPTR)
  {
      return pimsmMapExtenRPSetExpiryTimeGet(pimsmMapCbPtr,
      rpSetGroupAddress, rpSetGroupMask, rpSetAddress, 
      rpSetComponent, rpSetExpiryTime);
            
  }
  else
  {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid expiry time of C-RP (%p)",
                        (void *)rpSetExpiryTime);
      return L7_FAILURE;
  }
}

/************************************************************************
* @purpose  Get the unicast address of the interface which will
*           be advertised as a Candidate RP
*
* @param    familyType         @b{(input)}  Address Family type
* @param    candRPGroupAddress @b{(input)}  Candidate RP Group Address
* @param    candRPGroupMask    @b{(input)}  Candidate RP Group Mask
* @param    candRPAddr         @b{(output)}  Candidate RP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapCandRPAddressGet(L7_uchar8 familyType,
 L7_inet_addr_t *candidateRPGroupAddress, L7_inet_addr_t *candidateRPGroupMask, 
 L7_inet_addr_t *candRPAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (candRPAddr != L7_NULLPTR)
  {
      /* Should be maintained locally in mapping layer.*/
    return L7_FAILURE;
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Candidate RP Address (%p)",
                      (void *)candRPAddr);
    return L7_FAILURE;
  }
}

/************************************************************************
* @purpose  Get IP address of the elected bootstrap router (BSR).
*
* @param    familyType       @b{(input)}    Address Family type
* @param    elecBSRAddress   @b{(output)}   Component BSR Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapElectedBSRAddressGet(L7_uchar8 familyType, 
                                     L7_inet_addr_t *elecBSRAddress)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (elecBSRAddress != L7_NULLPTR)
  {
    return pimsmMapExtenElectedBSRAddressGet(pimsmMapCbPtr,
                                             elecBSRAddress);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Candidate RP Address(%p)",
                      (void *)elecBSRAddress);
    return L7_FAILURE;
  }
}

/************************************************************************
* @purpose  Get minimum time remaining before the bootstrap router in
*           the local domain will be declared down
*
* @param    familyType        @b{(input)}   Address Family type
* @param    compBSRExpiryTime @b{(output)}  Expiry time of BSR in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapElectedBSRExpiryTimeGet(L7_uchar8 familyType,
                                        L7_uint32 *compBSRExpiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (compBSRExpiryTime != L7_NULLPTR)
  {
    return pimsmMapExtenElectedBSRExpiryTimeGet(pimsmMapCbPtr,
                                                compBSRExpiryTime);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Expiry time of BSR (%p)",
                      (void *)compBSRExpiryTime);
    return L7_FAILURE;
  }
}

/************************************************************************
* @purpose  Get the Advertisement time of the elected BSR.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    elecCRPAdvTime   @b{(input)}  Hold time of component when it is C-RP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapElectedCRPAdvTimeGet(L7_uchar8 familyType,
                                     L7_uint32 *elecCRPAdvTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (elecCRPAdvTime != L7_NULLPTR)
  {
    return pimsmMapExtenElectedCRPAdvTimeGet(pimsmMapCbPtr, 
                                             elecCRPAdvTime);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid elecCRPAdvTime(%p)",(void *)elecCRPAdvTime);
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Check whether interface entry exists for the specified interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceEntryGet(L7_uchar8 familyType,
                                  L7_uint32 intIfNum)
{
    L7_uint32 mode = L7_DISABLE;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    if ((pimsmMapInterfaceModeGet(familyType, intIfNum,  &mode) == L7_SUCCESS)
         && (mode == L7_ENABLE))
    {
        return L7_SUCCESS;
    }
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,"PIMSM not enabled on \
                     intIfNum(%d) for familyType %d",intIfNum,familyType);
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Obtain the next sequential routing interface entry
*           after the entry of the specified interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    pIntIfNum    @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceEntryNextGet(L7_uchar8 familyType,
                                      L7_uint32 *pIntIfNum)
{
  L7_uint32 rtrIfNum = L7_NULL;
  L7_uint32 tmpIntIfNum = L7_NULL;
  L7_uint32 mode = L7_DISABLE;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  for (tmpIntIfNum = *pIntIfNum+1; tmpIntIfNum <= L7_MAX_INTERFACE_COUNT;
       tmpIntIfNum++)
  {
    if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                   tmpIntIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if ((pimsmMapInterfaceModeGet(familyType, tmpIntIfNum,
          &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
      {
        *pIntIfNum = tmpIntIfNum;
        return L7_SUCCESS;
      }
    }
  }
  PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,"Failed to get the next PIMSM \
                    enabled interface after intfIfNum(%d) for familyType %d\n",
                    *pIntIfNum,familyType);
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Check whether neighbor entry exists for the specified IP address
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddress   @b{(inout)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborEntryGet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum, 
                                 L7_inet_addr_t *ipAddress)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  return pimsmMapExtenNeighborEntryGet(pimsmMapCbPtr, 
                                       intIfNum, ipAddress);
}

/*********************************************************************
* @purpose  Obtain the next sequential neighbor entry after the entry of the
*           specified neighbor
*
* @param    familyType    @b{(input)}  Address Family type
* @param    pIntIfNum     @b{(input)}  Internal Interface Number
* @param    ipAddress     @b{(inout)}  IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborEntryNextGet(L7_uchar8 familyType,
       L7_uint32 *pIntIfNum, L7_inet_addr_t *ipAddress)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if (ipAddress != L7_NULLPTR)
  {
     return pimsmMapExtenNeighborEntryNextGet(pimsmMapCbPtr, 
                                              pIntIfNum, ipAddress);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid ip Address(%p) of the neighbor",
                      (void *)ipAddress);
     return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Check whether IP Multicast route entry exists for the specified
*           group, source and mask
*
* @param    familyType          @b{(input)} Address Family type
* @param    ipMRouteGroup       @b{(input)} IP Multicast route group
* @param    ipMRouteSource      @b{(input)} IP Multicast route source
* @param    ipMRouteSourceMask  @b{(input)} IP Multicast route mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteEntryGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD   
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  return pimsmMapExtenIpMRouteEntryGet(pimsmMapCbPtr, 
   ipMRouteGroup,ipMRouteSource, ipMRouteSourceMask);
#endif  
}

/*********************************************************************
* @purpose  Get the existing IP Multicast Route entry next to the entry of the
*           specified group, source and mask
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(inout)} IP Multicast route group
* @param    ipMRouteSource      @b{(inout)} IP Multicast route source
* @param    ipMRouteSourceMask  @b{(inout)} IP Multicast route mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteEntryNextGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  L7_RC_t          rc = L7_FAILURE;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if ((ipMRouteGroup == L7_NULLPTR) || (ipMRouteSource == L7_NULLPTR))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid input parameters");
    return L7_FAILURE;
    
  }
  if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
  {
    rc = pimsmMapStarGEntryNextGet(familyType, ipMRouteGroup);
    if (rc == L7_FAILURE)
    {
      inetAddressZeroSet(familyType,ipMRouteGroup); 
      inetAddressZeroSet(familyType,ipMRouteSource);
    }
  }
  if (rc == L7_FAILURE)
  {
    rc = pimsmMapSGEntryNextGet(familyType, ipMRouteGroup, ipMRouteSource);
  }
  return rc;
}

/*********************************************************************
* @purpose  Check whether PIMSM routing entry exists for the specified
*           group, source and mask
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  IP Multicast route group
* @param    ipMRouteSource       @b{(input)}  IP Multicast route source
* @param    ipMRouteSourceMask   @b{(input)}  IP Multicast route source mask
*                                             (source mask not being used)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteEntryGet(L7_uchar8 familyType, L7_inet_addr_t *ipMRouteGroup, 
    L7_inet_addr_t *ipMRouteSource, L7_inet_addr_t *ipMRouteSourceMask)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD 
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  return pimsmMapExtenRouteEntryGet(pimsmMapCbPtr, 
  ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask);
#endif
}

/*********************************************************************
* @purpose  Get the existing PIMSM routing entry next to the entry of the
*           specified group, source and mask
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(inout)}  IP Multicast route group
* @param    ipMRouteSource       @b{(inout)}  IP Multicast route source
* @param    ipMRouteSourceMask   @b{(inout)}   IP Multicast route source mask
*                                  (source mask not being used)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteEntryNextGet(L7_uchar8 familyType, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD   
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if ((ipMRouteGroup != L7_NULLPTR) && (ipMRouteSource != L7_NULLPTR)
                                    && (ipMRouteSourceMask != L7_NULLPTR))
  {
    return pimsmMapExtenRouteEntryNextGet(pimsmMapCbPtr,
                  ipMRouteGroup,ipMRouteSource, ipMRouteSourceMask);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid ipMRouteGroup or ipMRouteSource or ipMRouteSourceMask\n");
    return L7_FAILURE;
  }
#endif
}

/*********************************************************************
* @purpose  Get expiry time of the routing entry
*
* @param    familyType         @b{(input)}     Address Family type
* @param    ipMRouteGroup      @b{(input)}     IP multicast group address
* @param    ipMRouteSource     @b{(input)}     Network Address
* @param    ipMRouteSourceMask @b{(input)}     Network Mask (currently not used)
* @param    expiryTime         @b{(output)}    Expiry time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteExpiryTimeGet(L7_uchar8 familyType, 
                                   L7_inet_addr_t *ipMRouteGroup, 
                                   L7_inet_addr_t *ipMRouteSource,
                                   L7_inet_addr_t *ipMRouteSourceMask, 
                                   L7_uint32 *expiryTime)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD 
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (expiryTime != L7_NULLPTR)
  {
    return pimsmMapExtenRouteExpiryTimeGet(pimsmMapCbPtr, ipMRouteGroup,
                 ipMRouteSource, ipMRouteSourceMask, expiryTime);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid expiry time\n");
    return L7_FAILURE;
  }
#endif
}

/*********************************************************************
* @purpose  Get up time of the routing entry
*
* @param    familyType         @b{(input)} Address Family type
* @param    ipMRouteGroup      @b{(input)} IP multicast group address
* @param    ipMRouteSource     @b{(input)} Network Address
* @param    ipMRouteSourceMask @b{(input)} Network Mask (currently not used)
* @param    upTime             @b{(output)} Up time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteUpTimeGet(L7_uchar8 familyType, 
                               L7_inet_addr_t *ipMRouteGroup, 
                               L7_inet_addr_t *ipMRouteSource, 
                               L7_inet_addr_t *ipMRouteSourceMask, 
                               L7_uint32 *upTime)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD   
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (upTime != L7_NULLPTR)
  {
     return pimsmMapExtenRouteUpTimeGet(pimsmMapCbPtr, 
                                        ipMRouteGroup,
                                        ipMRouteSource,
                                        ipMRouteSourceMask,
                                        upTime);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Up time\n");
    return L7_FAILURE;
  }
#endif
}

/*********************************************************************
* @purpose  Get RPF address of the routing entry
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask (currently not used)
* @param    rpfAddr             @b{(output)} Address of RPF Neighbor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteRpfAddrGet(L7_uchar8 familyType, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, L7_inet_addr_t *rpfAddr)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD   
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (rpfAddr != L7_NULLPTR)
  {
    return pimsmMapExtenRouteRpfAddrGet(pimsmMapCbPtr, 
                                           ipMRouteGroup,
                                           ipMRouteSource,
                                           ipMRouteSourceMask,
                                           rpfAddr);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Address of RPF Neighbor\n");
    return L7_FAILURE;
  }
#endif
}

/*********************************************************************
* @purpose  Get flags of the routing entry
*
* @param    familyType           @b{(input)} Address Family type
* @param    ipMRouteGroup        @b{(input)} IP multicast group address
* @param    ipMRouteSource       @b{(input)} Network Address
* @param    ipMRouteSourceMask   @b{(input)} Network Mask (currently not used)
* @param    flags                @b{(output)}RPT or SPT flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteFlagsGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, 
    L7_uint32 *flags)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD   
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (flags != L7_NULLPTR)
  {
    return pimsmMapExtenRouteFlagsGet(pimsmMapCbPtr, ipMRouteGroup,
                                      ipMRouteSource,
                                      ipMRouteSourceMask,
                                      flags);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid RPT or SPT flag\n");
    return L7_FAILURE;
  }
#endif
}

/*********************************************************************
* @purpose  Get incoming interface of the routing entry
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask (currently not used)
* @param    inIfIndex           @b{(output)} Internal Interface Number 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteIfIndexGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *inIfIndex)
{
    return L7_NOT_SUPPORTED;   
#ifdef PIMSM_MAP_TBD   
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (inIfIndex != L7_NULLPTR)
  {
    return pimsmMapExtenRouteIfIndexGet(pimsmMapCbPtr, 
                                        ipMRouteGroup,
                                        ipMRouteSource,
                                        ipMRouteSourceMask,
                                        inIfIndex);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Internal Interface number\n");
    return L7_FAILURE;
  }
#endif
}

#ifdef PIMSM_MAP_TBD 
/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}    Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal 
*                                               Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapRouteOutIntfEntryNextGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *outIntIfNum)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (outIntIfNum != L7_NULLPTR)
  {
    return pimsmMapExtenRouteOutIntfEntryNextGet(pimsmMapCbPtr, 
    ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask, outIntIfNum);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Outgoing Internal Interface Number\n");
    return L7_FAILURE;
  }
}

/***************************************************************
* @purpose  Gets number of forwarded packets for the given Index
*           through the router
*
* @param    familyType           @b{(input)}    Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    numOfPackets         @b{(output)}   Number of Forwarded Packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*
***************************************************************/
L7_RC_t pimsmMapRoutePktsGet(L7_uchar8 familyType, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *numOfPackets)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (numOfPackets != L7_NULLPTR)
  {
     return pimsmMapExtenRoutePktsGet(pimsmMapCbPtr, ipMRouteGroup,
                ipMRouteSource, ipMRouteSourceMask, numOfPackets);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Number of Forwarded Packets\n");
    return L7_FAILURE;
  }
}
  }
#endif
/*********************************************************************
* @purpose  Check whether RP Group entry exists for the specified
*           group address, group mask, address and component number
*
* @param    familyType       @b{(input)}  Address Family type
* @param    origin           @b{(input)}  Origin
* @param    rpGroupAddress   @b{(input)}  IP Multicast group address
* @param    rpGroupMask      @b{(input)}  Multicast group address mask
* @param    rpAddress        @b{(input)}  IP address of candidate RP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRPGroupEntryGet(L7_uchar8 familyType, 
    L7_uchar8 origin, L7_inet_addr_t *rpGroupAddress, 
    L7_inet_addr_t *rpGroupMask, L7_inet_addr_t *rpAddress)
{
    pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }
    return pimsmMapExtenRpGroupEntryGet(pimsmMapCbPtr, origin, 
                rpGroupAddress, rpGroupMask, rpAddress);
}

/*********************************************************************
* @purpose  Get the existing Group RP entry next to the entry of 
*           the specified group address, group mask, 
*           address.
*
* @param    familyType     @b{(input)} Address Family type
* @param    origin         @b{(inout)} Origin
* @param    groupAddress   @b{(inout)} IP Multicast group address
* @param    groupMask      @b{(inout)} Multicast group address mask
* @param    rpAddress      @b{(inout)} IP address of candidate RP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRpGroupEntryNextGet(L7_uchar8 familyType, 
    L7_uchar8 *origin, L7_inet_addr_t *rpGroupAddress, 
    L7_inet_addr_t *rpGroupMask, L7_inet_addr_t *rpAddress)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  if ((rpGroupAddress != L7_NULLPTR) && (rpGroupMask != L7_NULLPTR) && 
      (rpAddress != L7_NULLPTR))
  {
     return pimsmMapExtenRpGroupEntryNextGet(pimsmMapCbPtr,
                 origin, rpGroupAddress, rpGroupMask, rpAddress);
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid args:rpGroupAddress(%p) or \
                      rpGroupMask(%p) or rpAddress(%p) passed",
                      (void *)rpGroupAddress,(void *)rpGroupMask,(void *)rpAddress);
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Get the existing elected BSR next entry. 
*
* @param    familyType   @b{(input)} Address Family type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapElectedBSREntryNextGet(L7_uchar8 familyType)
{ 
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  return pimsmMapExtenElectedBSREntryNextGet(pimsmMapCbPtr);
}

/*********************************************************************
* @purpose  Obtain the RP that the specified group will be mapped to.
*
* @param    familyType     @b{(input)} Address Family type
* @param    grpIpAddr      @b{(inout)} Group ip address
* @param    rpIpAddr       @b{(inout)} RP ip address
* @param    origin         @b{(inout)} Origin
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapGroupToRPMappingGet(L7_uchar8 familyType,
    L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *rpIpAddr, 
    L7_uchar8 *origin)
{ 
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  return pimsmMapExtenGroupToRPMappingGet(pimsmMapCbPtr, grpIpAddr, 
                                          rpIpAddr, origin);
}
/*********************************************************************
* @purpose  Sets the BSR Border for the specified interface
*
* @param    familyType    @b{(input)}   Address Family type
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    mode          @b{(input)}   Mode (ENABLE/DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceBsrBorderSet(L7_uchar8 familyType,
               L7_uint32 intIfNum, L7_uint32 mode)
{
   pimsmCfgCkt_t   *pCfg;
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }


   if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
       return L7_FAILURE;
   }

   if ((mode != L7_DISABLE) && (mode != L7_ENABLE))
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid mode(%d) on intIfNum(%d)",mode,
                    intIfNum);
       return L7_FAILURE;
   }

   if (mode == pCfg->bsrBorderMode)
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_CONFIGURED,
       "\nBSR mode is same\n");
       return L7_SUCCESS;
   }

   pCfg->bsrBorderMode = mode;
   pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,
                   "Function Exit\n");
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the BSR Border for the specified interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface Number
* @param    bsrBorder      @b{(output)}  BSR Border (ENABLE/DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceBsrBorderGet(L7_uchar8 familyType,
              L7_uint32 intIfNum, L7_uint32 *bsrBorder)
{
   pimsmCfgCkt_t   *pCfg;
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
       return L7_FAILURE;
   }

   if (bsrBorder != L7_NULLPTR)
   {
      *bsrBorder = pCfg->bsrBorderMode;
      return L7_SUCCESS;
   }
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid value of bsrBorder(%p)",(void *)bsrBorder);
   return L7_FAILURE;
}
/*********************************************************************
* @purpose  Set the DR Priority for the specified interface
*
* @param    pimsmMapCbPtr     @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    priority          @b{(input)} Designated Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapInterfaceDRPriorityQueue(pimsmMapCB_t *pimsmMapCbPtr,
                             L7_uint32 intIfNum, L7_uint32 priority)
{
  pimsmMapIntfData_t data;

  memset(&data,0 , sizeof(pimsmMapIntfData_t));
  data.intIfNum= intIfNum;
  data.opaqueData= priority;
  
  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_INTF_DR_PRIORITY_EVENT,
      &data, sizeof(pimsmMapIntfData_t));

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the DR Priroity for the specified interface 
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    priority     @b{(output)} Designated Router priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceDRPrioritySet(L7_uchar8 familyType,
          L7_uint32 intIfNum, L7_uint32 priority)
{
   pimsmCfgCkt_t   *pCfg;
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
       return L7_FAILURE;
   }

   if ((priority < L7_PIMSM_INTERFACE_DR_PRIORITY_MIN) ||
       (priority > L7_PIMSM_INTERFACE_DR_PRIORITY_MAX))
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid priority(%d)",priority);
       return L7_FAILURE;
   }

   if (priority == pCfg->drPriority)
   {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_CONFIGURED,
       "\nDR Priority already set\n");
       return L7_SUCCESS;
   }

   pCfg->drPriority = priority;
   pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

   if (pimsmMapCbPtr->pPimsmIntfInfo[intIfNum].pimsmOperational != L7_TRUE)
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :PIM-SM not:"
     "operational config paramter stored but not applied\n", 
     __FUNCTION__, __LINE__);
   }   
   pimsmMapInterfaceDRPriorityQueue(pimsmMapCbPtr,
                                     intIfNum, priority);
   return L7_SUCCESS;      
}

/*********************************************************************
* @purpose  Gets the  DR Priority for the specified interface.
*
* @param    familyType   @b{(input)}     Address Family type
* @param    intIfNum     @b{(input)}     Internal Interface Number
* @param    drPriority   @b{(output)}     DR Prioirity
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceDRPriorityGet(L7_uchar8 familyType, 
        L7_uint32 intIfNum, L7_uint32 *drPriority)
{
   pimsmCfgCkt_t   *pCfg = L7_NULLPTR;
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr, intIfNum, &pCfg) != L7_TRUE)
   {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"intIfNum(%d) not valid for \
                         PIM-SM configuration",intIfNum);
      return L7_FAILURE;
   }

   if (drPriority != L7_NULLPTR)
   {
      *drPriority = pCfg->drPriority;
      return L7_SUCCESS;
   }
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid DR priority (%p)",(void *)drPriority); 
   return L7_FAILURE;
}


/*********************************************************************
* @purpose  To get the candidate RP Entry.
*
* @param    familyType     @b{(input)}  Address Family type.
* @param    candRPAddr     @b{(input)}  cand RP Group address.
* @param    candRPGrpAddr  @b{(input)}  cand RP Group address.
* @param    prefixLen      @b{(input)}  prefix Length.
* @param    candRpEntry    @b{(output)} candidate RP entry.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapCandRPEntryIndexGet(pimsmMapCB_t *pimsmMapCbPtr, 
                                L7_inet_addr_t *candRPAddr, 
                                L7_inet_addr_t *candRPGrpAddr, 
                                L7_uchar8 prefixLen,
                                L7_uint32 *pIndex)
{
  pimsmCfgCandRPdata_t *candRpEntry = L7_NULLPTR;
  L7_uint32            index = L7_NULL;
  L7_uchar8            grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8            rpAddr[IPV6_DISP_ADDR_LEN];

  *pIndex = L7_MAX_CAND_RP_NUM;
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if ((inetIsAddressZero(candRPGrpAddr) == L7_TRUE) ||
     (prefixLen == L7_NULL))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Rp Address(%s) or Grp Address(%s) or \
                      prefix Len(%d) input",inetAddrPrint(candRPAddr,rpAddr),
                      inetAddrPrint(candRPGrpAddr,grpAddr),prefixLen);
    return L7_FAILURE;
  }
  for (index = 0; index < L7_MAX_CAND_RP_NUM; index++)
  {
    candRpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                  pimsmCandidateRp[index];
    if ((L7_INET_IS_ADDR_EQUAL(&candRpEntry->grpAddress, 
                               candRPGrpAddr) == L7_TRUE) &&
        (candRpEntry->grpPrefixLength == prefixLen))
    {
      /* entry found. */
      *pIndex = index;
      return L7_SUCCESS;
    }
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find candidate RP Entry for \
                    RP(%s) and GrpAddr(%s)",inetAddrPrint(candRPAddr,rpAddr),
                    inetAddrPrint(candRPGrpAddr,grpAddr));
  return L7_FAILURE;
}

#if 0
/*********************************************************************
* @purpose  To get the candidate RP Entry.
*
* @param    familyType     @b{(input)}  Address Family type.
* @param    candRPAddr     @b{(input)}  cand RP Group address.
* @param    candRPGrpAddr  @b{(input)}  cand RP Group address.
* @param    prefixLen      @b{(input)}  prefix Length.
* @param    candRpEntry    @b{(output)} candidate RP entry.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapCandRPEntryIndexGet(pimsmMapCB_t *pimsmMapCbPtr, 
                                L7_inet_addr_t *candRPAddr, 
                                L7_inet_addr_t *candRPGrpAddr, 
                                L7_uchar8 prefixLen,
                                L7_uint32 *pIndex)
{
  pimsmCfgCandRPdata_t *candRpEntry = L7_NULLPTR;
  L7_uint32            index = L7_NULL;
  L7_uchar8            grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8            rpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8 familyType = pimsmMapCbPtr->familyType;

  *pIndex = L7_MAX_CAND_RP_NUM;
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if ((inetIsAddressZero(candRPAddr) == L7_TRUE) ||
     (inetIsAddressZero(candRPGrpAddr) == L7_TRUE) ||
     (prefixLen == L7_NULL))
  {
    PIMSM_MAP_LOG_MSG(familyType,"Invalid Rp Address(%s) or Grp Address(%s) or \
                      prefix Len(%d) input",inetAddrPrint(candRPAddr,rpAddr),
                      inetAddrPrint(candRPGrpAddr,grpAddr),prefixLen);
    return L7_FAILURE;
  }
  for (index = 0; index < L7_MAX_CAND_RP_NUM; index++)
  {
    candRpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                  pimsmCandidateRp[index];
    if ((L7_INET_IS_ADDR_EQUAL(
      &pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress,
                                   candRPAddr) == L7_TRUE) &&
        (L7_INET_IS_ADDR_EQUAL(&candRpEntry->grpAddress,
                               candRPGrpAddr) == L7_TRUE) &&
        (candRpEntry->grpPrefixLength == prefixLen))
    {
      /* entry found. */
      *pIndex = index;
      return L7_SUCCESS;
    }
  }  
  PIMSM_MAP_LOG_MSG(familyType,"Failed to find candidate RP Entry for \
                    RP(%s) and GrpAddr(%s)",inetAddrPrint(candRPAddr,rpAddr),
                    inetAddrPrint(candRPGrpAddr,grpAddr));
  return L7_FAILURE;
}
#endif



/*********************************************************************
* @purpose  To get the candidate RP Entry.
*
* @param    familyType     @b{(input)}  Address Family type.
* @param    candRPAddr     @b{(input)}  cand RP Group address.
* @param    candRPGrpAddr  @b{(input)}  cand RP Group address.
* @param    prefixLen      @b{(input)}  prefix Length.
* @param    candRpEntry    @b{(output)} candidate RP entry.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapCandidateRpEntryGet(L7_uchar8 familyType, 
                                L7_inet_addr_t *candRPAddr, 
                                L7_inet_addr_t *candRPGrpAddr, 
                                L7_uchar8 prefixLen,
                                pimsmCfgCandRPdata_t **candRpEntryGet)
{
  pimsmMapCB_t         *pimsmMapCbPtr = L7_NULLPTR;
  pimsmCfgCandRPdata_t *candRpEntry = L7_NULLPTR;
  L7_uint32            index = L7_NULL;
  L7_uchar8            grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8            rpAddr[IPV6_DISP_ADDR_LEN];

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  /* this fails if PIM-SM component not started by configurator */
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
    return L7_FAILURE;
  }
  if ((inetIsAddressZero(candRPAddr) == L7_TRUE) ||
     (inetIsAddressZero(candRPGrpAddr) == L7_TRUE) ||
     (prefixLen == L7_NULL))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Rp Address(%s) or Grp Address(%s) or \
                      prefix Len(%d) input",inetAddrPrint(candRPAddr,rpAddr),
                      inetAddrPrint(candRPGrpAddr,grpAddr),prefixLen);
    return L7_FAILURE;
  }
  for (index = 0; index < L7_MAX_CAND_RP_NUM; index++)
  {
    candRpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                  pimsmCandidateRp[index];

    if ((pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[index].intIfNum > 0) &&
        (L7_INET_IS_ADDR_EQUAL(&candRpEntry->grpAddress, 
                               candRPGrpAddr) == L7_TRUE) &&
        (candRpEntry->grpPrefixLength == prefixLen))
    {
      /* entry found. */
      *candRpEntryGet = candRpEntry;
      return L7_SUCCESS;
    }
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find candidate RP Entry for  \
                    RP(%s) and GrpAddr(%s)",inetAddrPrint(candRPAddr,rpAddr),
                    inetAddrPrint(candRPGrpAddr,grpAddr));
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Apply the static RP information
*
* @param    pimsmMapCbPtr @b{(input)}       Mapping Control Block.
* @param    index         @b{(input)}       Index into the config structure 
*                                           for this static RP
* @param    rpGrpAddr     @b{(input)}       Group address supported by the RP
* @param    rpGrpMask     @b{(input)}       Group mask for the group address
* @param    conflict      @b{(input)}       conflict
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapCandRPAddressQueue(pimsmMapCB_t *pimsmMapCbPtr,
                        L7_uint32 intIfNum, L7_inet_addr_t *rpGrpAddr, 
                        L7_uchar8 prefixLen,  L7_uint32 mode, L7_uint32 index)
{
  pimsmMapCandRPdata_t data;

  memset(&data,0 , sizeof(pimsmMapCandRPdata_t));
  inetCopy(&data.grpAddr, rpGrpAddr);
  data.prefixLen = prefixLen;
  data.mode = mode;
  data.index = index;
  data.intIfNum = intIfNum;
  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_CAND_RP_EVENT, 
      &data, sizeof(pimsmMapCandRPdata_t));

  return L7_SUCCESS;
  
}

/*********************************************************************
* @purpose  Sets the RP candidate
*
* @param    familyType   @b{(input)}        Address Family type
* @param    intIfNum     @b{(input)}        interface Number.
* @param    rpGrpAddr    @b{(input)}        Group address supported by the RP
* @param    rpGrpMask    @b{(input)}        Group mask for the group address
* @param    mode         @b{(input)}        mode (ENABLE/DISABLE).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPSet(L7_uchar8 familyType,
                               L7_uint32 intIfNum, L7_inet_addr_t *rpGrpAddr, 
    L7_inet_addr_t *rpGrpMask, L7_uint32 mode)
{
  pimsmCfgCandRPdata_t *pimsmCrpEntry = L7_NULLPTR;
  pimsmCfgCandRPdata_t *freeCrpEntry = L7_NULLPTR;
  pimsmMapCB_t      *pimsmMapCbPtr = L7_NULLPTR;
  L7_uchar8         paramPrefixLen = L7_NULL;
  L7_BOOL           freeSlotFound = L7_FALSE;
  L7_uint32         freeSlot = L7_NULL, index = L7_NULL, i ;
  L7_uchar8          grpAddr[IPV6_DISP_ADDR_LEN];
  L7_uchar8          maskAddr[IPV6_DISP_ADDR_LEN];
   
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");


  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  /* this fails if PIM-SM component not started by configurator */
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
     return L7_FAILURE;
  }
  
  /* group address should be class D */
  if (inetIsInMulticast(rpGrpAddr) != L7_TRUE)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Group Address(%s) not in multicast \
			  range", inetAddrPrint(rpGrpAddr,grpAddr));
    return L7_FAILURE;
  }
  if (inetMaskToMaskLen(rpGrpMask, &paramPrefixLen) != L7_SUCCESS || paramPrefixLen == 0)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert Mask(%s) to MaskLen",
                       inetAddrPrint(rpGrpMask,maskAddr));
     return L7_FAILURE;
  }

  if (mode == L7_ENABLE)
  {
    for (index = 0; index < L7_MAX_CAND_RP_NUM; index++)
    {
      pimsmCrpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                                  pimsmCandidateRp[index];
      /* To make sure same prefix length groups are not added
       * in candidate RP table.
       */
      
      if(pimsmCrpEntry->intIfNum > 0)
      {
        if(pimsmCrpEntry->intIfNum != intIfNum)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
          "Candidate RP is already configured on intIfNum %d",
          pimsmCrpEntry->intIfNum);
          return L7_ALREADY_CONFIGURED;
        } 
        if (inetAddrCompareAddrWithMask(&pimsmCrpEntry->grpAddress, 
                                        pimsmCrpEntry->grpPrefixLength, 
                                        rpGrpAddr, paramPrefixLen) == L7_NULL)
        {
           PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"This candidate RP group combination \
                             is already set for GrpAddress(%s) \
                             and prefixLen(%d)",inetAddrPrint(rpGrpAddr,grpAddr),
                             paramPrefixLen);
           return L7_ALREADY_CONFIGURED;
        }
      }
      else if (freeSlotFound == L7_FALSE)
      {
         freeSlotFound = L7_TRUE;
         freeSlot = index;
      }

    }
    if (freeSlotFound != L7_TRUE)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"candidate RP Table Full");
      return L7_FAILURE;
    }
    freeCrpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                               pimsmCandidateRp[freeSlot];
    freeCrpEntry->grpPrefixLength = paramPrefixLen;
    freeCrpEntry->intIfNum = intIfNum;
    
    if (inetCopy(&freeCrpEntry->grpAddress, rpGrpAddr) != L7_SUCCESS)
    {
       return L7_FAILURE;
    }
    pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    index = freeSlot ;
  }
  else if (mode == L7_DISABLE)
  {
    index = L7_MAX_CAND_RP_NUM;
    for (i = 0; i < L7_MAX_CAND_RP_NUM; i++)
    {
      pimsmCrpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                              pimsmCandidateRp[i];
      if(pimsmCrpEntry->intIfNum >0)
      { 
       if (inetAddrCompareAddrWithMask(&pimsmCrpEntry->grpAddress, 
                                       pimsmCrpEntry->grpPrefixLength, 
                                       rpGrpAddr, paramPrefixLen) == L7_NULL)
       {
          index= i;
          break;
       }
      }
    }
    if ( index < L7_MAX_CAND_RP_NUM)
    {
      if(pimsmCrpEntry->intIfNum >0)
      {
        inetAddressZeroSet(familyType, &pimsmCrpEntry->grpAddress);
        pimsmCrpEntry->grpPrefixLength = L7_NULL;
        pimsmCrpEntry->intIfNum =L7_NULL;
        pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
      }
    }
    else
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES," GrpAddress(%s) \
                             and prefixLen(%d) not found in RP table",inetAddrPrint(rpGrpAddr,grpAddr),
                             paramPrefixLen);
      return L7_FAILURE;
    }
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,
    "\nRP configuration table cleared successfully.\n");
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid command on candidate RP Table");
    return L7_FAILURE;
  }
  if (pimsmMapCbPtr->pPimsmInfo->pimsmOperational != L7_TRUE)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n PIM-SM not operational:"
    "config paramter stored but not applied\n");

    return L7_SUCCESS;
  }
  if (pimsmMapCandRPAddressQueue(pimsmMapCbPtr, intIfNum, rpGrpAddr, 
                               paramPrefixLen, mode, index) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to apply the RP candidate \
                      (ENABLE/DISABLE)");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the interface on which Candidate Rp is configured
*
* @param    familyType   @b{(input)}        Address Family type
* @param    intIfNum     @b{(output)}       interface Number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPConfiguredIntfGet(L7_uchar8 familyType,
                               L7_uint32 *intIfNum)
{
  pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
  pimsmCfgCandRPdata_t *pimsmCrpEntry = L7_NULLPTR;
  L7_uint32 i = L7_NULL;
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  /* this fails if PIM-SM component not started by configurator */
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
    return L7_FAILURE;
  }

  for (i = 0; i < L7_MAX_CAND_RP_NUM; i++)
  {
    pimsmCrpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                    pimsmCandidateRp[i];
    if (pimsmCrpEntry->intIfNum > L7_NULL)
    {
      *intIfNum = pimsmCrpEntry->intIfNum;
      return L7_SUCCESS;
    }
  }

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Rp Candidate Entry not found");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Rp Candidate admin mode.
*
* @param    familyType    @b{(input)}       Address Family type
* @param    candRPAddr    @b{(input)}       Candidate Addr
* @param    candRPGrpAddr @b{(input)}       Candidate RP Grp Addr
* @param    prefixLen     @b{(input)}       prefix length of the address
* @param    mode          @b{(output)}      L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPModeGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *candRPAddr,
                                   L7_inet_addr_t *candRPGrpAddr, 
                                   L7_uint32 prefixLen, L7_uint32 *mode)
{
  pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32 index;
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  /* this fails if PIM-SM component not started by configurator */
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
    return L7_FAILURE;
  }

  if (pimsmMapCandRPEntryIndexGet(pimsmMapCbPtr, candRPAddr, candRPGrpAddr, 
                                  prefixLen, &index) == L7_SUCCESS)
  {
    /* found entry */
    *mode = pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].operMode;
    return L7_SUCCESS;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Rp Candidate Entry not found");
    return L7_FAILURE;
  }

/*********************************************************************
* @purpose  Gets the Rp Candidate interface number.
*
* @param    familyType    @b{(input)}       Address Family type
* @param    candRPAddr    @b{(input)}       Candidate Addr
* @param    candRPGrpAddr @b{(input)}       Candidate RP Grp Addr
* @param    prefixLen     @b{(input)}       prefix length of the address
* @param    intIfNum      @b{(output)}      interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPInterfaceGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *candRPAddr,
                                   L7_inet_addr_t *candRPGrpAddr, 
                                   L7_uint32 prefixLen, L7_uint32 *intIfNum)
{
  pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32 index;
  
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  /* this fails if PIM-SM component not started by configurator */
  if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
    return L7_FAILURE;
  }
  if (pimsmMapCandRPEntryIndexGet(pimsmMapCbPtr, candRPAddr, candRPGrpAddr, 
                                  prefixLen, &index) == L7_SUCCESS)
  {
    /* found entry */
      *intIfNum = pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[index].intIfNum;
      return L7_SUCCESS;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Rp Candidate Entry not found");
  return L7_FAILURE;
}



  /*********************************************************************
* @purpose  Check whether candidate RP entry exists for the specified 
*           group address and group mask.
*
* @param    familyType       @b{(input)}    Address Family type
* @param    candRpAddress    @b{(input)}    candidate Rp address.
* @param    cRPGroupAddress  @b{(input)}    candidate RP group address.
* @param    candPrefixLen    @b{(input)}    candidate prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPEntryGet(L7_uint32 unitIndex, L7_uchar8 familyType,
    L7_inet_addr_t *candRpAddress, L7_inet_addr_t *candGroupAddress,
    L7_uchar8 candPrefixLen)
{
  pimsmCfgCandRPdata_t *candRpEntry = L7_NULLPTR;
   
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (pimsmMapCandidateRpEntryGet(familyType, candRpAddress, candGroupAddress, 
                                  candPrefixLen, &candRpEntry) == L7_SUCCESS)
  {
    /* found entry */
      return L7_SUCCESS;
  }
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Rp Candidate Entry not found");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the existing Cand-RP entry next to the entry of the specified
*           group address and group mask
*
* @param    familyType      @b{(input)}      Address Family type
* @param    candRpAddress   @b{(output)}     candidate Rp address.
* @param    cRPGroupAddress @b{(output)}     candidate RP group address.
* @param    candPrefixLen   @b{(output)}     candidate prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments To get the first entry, candRpAddress, cRpGroupAddress, 
*           candPrefixLen should be zero.
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPEntryNextGet(L7_uchar8 familyType,
    L7_inet_addr_t *candRpAddress, L7_inet_addr_t *cRpGroupAddress, 
    L7_uchar8 *candPrefixLen)
{
    L7_uint32            index = L7_NULL, indexStart= 0;
    pimsmMapCB_t         *pimsmMapCbPtr = L7_NULLPTR;
    pimsmCfgCandRPdata_t *candRpEntry = L7_NULLPTR;
    L7_inet_addr_t       rpGrpAddrFind,rpAddrFind;
    L7_uchar8            rpPrefixLenFind = 0;
    L7_inet_addr_t       rpGrpAddrLocal;
    L7_uchar8            rpPrefixLenLocal = 0;
    L7_BOOL              found = L7_FALSE;
    L7_int32 rpGrpAddrEqual,cmpResult;
    L7_inet_addr_t       zeroAddress;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    if ((candRpAddress == L7_NULLPTR) || (cRpGroupAddress == L7_NULLPTR) ||
        (candPrefixLen == L7_NULLPTR))
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid input parameters: \
                        candRpAddress(%p) or cRpGroupAddress(%p) or \
                        candPrefixLen(%p)",(void *)candRpAddress,(void *)cRpGroupAddress,
                        (void *)candPrefixLen);
      return L7_FAILURE;
    }

   
    inetAddressZeroSet(familyType, &rpGrpAddrFind);
    inetAddressZeroSet(familyType, &rpGrpAddrLocal);
    inetAddressZeroSet(familyType, &zeroAddress);
    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }
    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
       return L7_FAILURE;
    }

    for (index = 0; index < L7_MAX_CAND_RP_NUM; index++)
    {
      candRpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                            pimsmCandidateRp[index];
       if(candRpEntry->intIfNum <=0)
        continue;
  
      /* Assuming first valid entry as max entry */
      inetCopy(&rpAddrFind,&pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress);
      inetCopy(&rpGrpAddrFind, &candRpEntry->grpAddress);
      rpPrefixLenFind = candRpEntry->grpPrefixLength;
 
      if(*candPrefixLen == L7_NULL && L7_INET_ADDR_COMPARE(cRpGroupAddress, &zeroAddress))
      {
        if (!(L7_INET_ADDR_COMPARE(&rpGrpAddrFind, cRpGroupAddress) == 0))
        {
          continue;
        }
      } else if(inetAddrCompareAddrWithMaskIndividual(&rpGrpAddrFind,rpPrefixLenFind,cRpGroupAddress, 
                                              *candPrefixLen) <=L7_NULL)
      {
        continue;
      }


      indexStart = index;
      found = L7_TRUE;
      break;
    }
    if (found == L7_FALSE)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Cand RP not configured");
       return L7_FAILURE;
    }
    for (index = indexStart; index < L7_MAX_CAND_RP_NUM; index++)
    {
      candRpEntry = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                            pimsmCandidateRp[index];
       if(candRpEntry->intIfNum <=0)
        continue;

      inetCopy(&rpGrpAddrLocal, &candRpEntry->grpAddress);
      rpPrefixLenLocal = candRpEntry->grpPrefixLength;
 
      /* Anything less than the passed Rp Grp Address or greater 
       * than found RP Grp Address, continue.
       */

      if(*candPrefixLen == L7_NULL && L7_INET_ADDR_COMPARE(cRpGroupAddress, &zeroAddress))
      {
        if (L7_INET_ADDR_COMPARE(&rpGrpAddrLocal, cRpGroupAddress)< L7_NULL)
        {
          continue;
        }
      }
      rpGrpAddrEqual = inetAddrCompareAddrWithMaskIndividual(&rpGrpAddrLocal,rpPrefixLenLocal,&rpGrpAddrFind, 
                                           rpPrefixLenFind);

      cmpResult = inetAddrCompareAddrWithMaskIndividual(&rpGrpAddrLocal,rpPrefixLenLocal,cRpGroupAddress, 
                                              *candPrefixLen);

      if(cmpResult > L7_NULL)
      {
        if(rpGrpAddrEqual >= L7_NULL)
        {
          continue;
        }

        /* Next Entry found. */
        inetCopy(&rpAddrFind,&pimsmMapCbPtr->pPimsmInfo->pimsmCandRPOper[index].candRPAddress);
        inetCopy(&rpGrpAddrFind, &candRpEntry->grpAddress);
        rpPrefixLenFind = candRpEntry->grpPrefixLength;

        found = L7_TRUE;
      }

    }

    if ((inetIsAddressZero(cRpGroupAddress) == L7_TRUE) && (*candPrefixLen == L7_NULL))
    {
       found =  L7_TRUE;
    }

    if (found == L7_FALSE) /* No Next entry found */
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Cand RP next entry not found");
      return L7_FAILURE;
    }
    inetCopy(candRpAddress,&rpAddrFind);
    inetCopy(cRpGroupAddress, &rpGrpAddrFind);
    *candPrefixLen = rpPrefixLenFind;
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, 
                    "\nFunction Exit.\n");
    return L7_SUCCESS;
  }

/*********************************************************************
* @purpose  Sets the BSR candidate
*
* @param    familyType  @b{(input)}         Address Family type
* @param    intIfNum    @b{(input)}         Interface Number.
* @param    priority    @b{(inout)}         Priority
* @param    mode        @b{(inout)}         mode (ENABLE/DISABLE).
* @param    length      @b{(inout)}         maskLen
* @param    scope       @b{(inout)}         scope Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmMapCandBSRAddressQueue(pimsmMapCB_t *pimsmMapCbPtr, 
                           L7_uint32 intIfNum,
                           L7_uint32 priority, 
                           L7_uint32 mode, L7_uint32 maskLen, 
                           L7_uint32 scope)
{
  pimsmMapCandBSRdata_t data;
  memset(&data,0 , sizeof(pimsmMapCandBSRdata_t));
  data.bsrHashMaskLength = maskLen;
  data.bsrPriority = priority;
  data.bsrScope = scope;
  data.mode = mode;
  data.intIfNum = intIfNum;
  pimsmMapCfgSetEventQueue(pimsmMapCbPtr, PIMSMMAP_CAND_BSR_EVENT, 
      &data, sizeof(pimsmMapCandBSRdata_t));

  return L7_SUCCESS;
  
}
/*********************************************************************
* @purpose  Sets the BSR candidate
*
* @param    familyType  @b{(input)}         Address Family type
* @param    intIfNum    @b{(input)}         Interface Number.
* @param    priority    @b{(inout)}         Priority
* @param    mode        @b{(inout)}         mode (ENABLE/DISABLE).
* @param    length      @b{(inout)}         maskLen
* @param    scope       @b{(inout)}         scope Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRSet(L7_uchar8 familyType, L7_uint32 intIfNum,
     L7_uint32 priority, L7_uint32 mode, 
    L7_uint32 maskLen, L7_uint32 scope)
{
   pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
   pimsmCfgCandBSRdata_t *candBsr = L7_NULLPTR;
   
   
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");

   /* Get the interface CB based on family & store family type in CB*/
   if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
   {
     /* Failed to get control block */
     
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
     return L7_FAILURE;
   }

   /* this fails if PIM-SM component not started by configurator */
   if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
   {
       
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
      return L7_FAILURE;
   }
   candBsr = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.candidateBsr;
   
   /* range checking */
   /* an IP address or subnet mask of 0 is invalid */
   /*if (inetIsAddressZero(bsrAddr) == L7_TRUE) 
   {
     
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"BSR address is 0");
     return L7_FAILURE;
   }*/
   if (mode == L7_ENABLE)
   {
     /* storing in Control Block */
     candBsr->candBSRPriority = priority;
     candBsr->candBSRHashMaskLength = maskLen;
     candBsr->candBSRScope = scope;

     candBsr->intIfNum = intIfNum;   
     
     pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
     
     pimsmMapCandBSRAddressQueue(pimsmMapCbPtr, intIfNum,
                                  priority, mode, maskLen, scope);
   }
   else if (mode == L7_DISABLE)
   {
     memset(candBsr,0,sizeof(pimsmCfgCandBSRdata_t));

     pimsmMapCandBSRAddressQueue(pimsmMapCbPtr, intIfNum,
                                  priority, mode, maskLen, scope);
     pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_TRUE;

   }
   else
   {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"wrong bsr-mode (%d)", mode);
     return L7_FAILURE;
   }
   return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the Ip Mroute Rt Protocol used.
*
* @param    familyType         @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  Group Address.
* @param    ipMRouteSource     @b{(input)}  Source Address.
* @param    ipMRouteSourceMask @b{(input)}  Source Mask.
* @param    rtProtocol         @b{(output)} routing protocol used
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRtProtoGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32*  rtProtocol)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (rtProtocol != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      return pimsmMapStarGRPFRouteProtocolGet(familyType, ipMRouteGroup, rtProtocol);
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      return pimsmMapSGRPFRouteProtocolGet(familyType, ipMRouteGroup, ipMRouteSource, rtProtocol);
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid route type pointer\n");
    return L7_FAILURE;
  }
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    familyType         @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  Group Address.
* @param    ipMRouteSource     @b{(input)}  Source Address.
* @param    ipMRouteSourceMask @b{(input)}  Source Mask.
* @param    rtAddr             @b{(output)}  rtAddr 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRtAddressGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }


  if (rtAddr != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      return pimsmMapStarGRPFRouteAddrGet(familyType, ipMRouteGroup, rtAddr);
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      return pimsmMapSGRPFRouteAddressGet(familyType, ipMRouteGroup, ipMRouteSource, rtAddr);
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid route address pointer\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    familyType         @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  Group Address.
* @param    ipMRouteSource     @b{(input)}  Source Address.
* @param    ipMRouteSourceMask @b{(input)}  Source Mask.
* @param    rtMask             @b{(output)} rtMask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRtMaskGet(L7_uchar8 familyType, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtMask)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32        maskLen = 0;
  L7_RC_t          rc;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }


  if (rtMask != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      rc = pimsmMapStarGRPFRoutePrefixLenGet(familyType, ipMRouteGroup, &maskLen);
      inetMaskLenToMask(familyType, maskLen, rtMask);
      return rc;
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      rc = pimsmMapSGRPFRoutePrefixLengthGet(familyType, ipMRouteGroup, ipMRouteSource, &maskLen);
      inetMaskLenToMask(familyType, maskLen, rtMask);
      return rc;
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid route mask pointer\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    familyType          @b{(input)} Address Family type
* @param    ipMRouteGroup       @b{(input)} Group Address.
* @param    ipMRouteSource      @b{(input)} Source Address.
* @param    ipMRouteSourceMask  @b{(input)} Source Mask.
* @param    rtType              @b{(output)} rtType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRtTypeGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtType)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  *rtType = L7_UNICAST_ROUTE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Address.
*
* @param    familyType         @b{(input)}   Address Family type
* @param    ipMRouteGroup      @b{(input)}   Group Address.
* @param    ipMRouteSource     @b{(input)}   Source Address.
* @param    ipMRouteSourceMask @b{(input)}   Source Mask.
* @param    rpfAddr            @b{(output)}  rpfAddr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRpfAddrGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t *rpfAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }


  if (rpfAddr != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      return pimsmMapStarGRpfAddrGet(familyType, ipMRouteGroup, rpfAddr);
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      return pimsmMapSGRpfAddrGet(familyType, ipMRouteGroup, ipMRouteSource, rpfAddr);
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid RpfAddress pointer\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get expiry time of the routing entry
*
* @param    familyType         @b{(input)}     Address Family type
* @param    ipMRouteGroup      @b{(input)}     IP multicast group address
* @param    ipMRouteSource     @b{(input)}     Network Address
* @param    ipMRouteSourceMask @b{(input)}     Network Mask (currently not used)
* @param    expiryTime         @b{(output)}    Expiry time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteExpiryTimeGet(L7_uchar8 familyType, 
                                   L7_inet_addr_t *ipMRouteGroup, 
                                   L7_inet_addr_t *ipMRouteSource,
                                   L7_inet_addr_t *ipMRouteSourceMask, 
                                   L7_uint32 *expiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (expiryTime != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      return pimsmMapStarGExpiryTimeGet(familyType, ipMRouteGroup, expiryTime);
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      return pimsmMapSGExpiryTimeGet(familyType, ipMRouteGroup, ipMRouteSource, expiryTime);
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid expiry time\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get up time of the routing entry
*
* @param    familyType         @b{(input)} Address Family type
* @param    ipMRouteGroup      @b{(input)} IP multicast group address
* @param    ipMRouteSource     @b{(input)} Network Address
* @param    ipMRouteSourceMask @b{(input)} Network Mask (currently not used)
* @param    upTime             @b{(output)} Up time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteUpTimeGet(L7_uchar8 familyType, 
                               L7_inet_addr_t *ipMRouteGroup, 
                               L7_inet_addr_t *ipMRouteSource, 
                               L7_inet_addr_t *ipMRouteSourceMask, 
                               L7_uint32 *upTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (upTime != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      return pimsmMapStarGUpTimeGet(familyType, ipMRouteGroup, upTime);
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      return pimsmMapSGUpTimeGet(familyType, ipMRouteGroup, ipMRouteSource, upTime);
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Up time\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get incoming interface of the routing entry
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask (currently not used)
* @param    inIfIndex           @b{(output)} Internal Interface Number 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteIfIndexGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *inIfIndex)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Failed to get the control block\n");
    return L7_FAILURE;
  }

  if (inIfIndex != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      return pimsmMapStarGIfIndexGet(familyType, ipMRouteGroup, inIfIndex);
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      return pimsmMapSGIfIndexGet(familyType, ipMRouteGroup, ipMRouteSource, inIfIndex);
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid Internal Interface number\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count.
*
* @param    familyType   @b{(input)}        Address Family type
* @param    entryCount   @b{(output)}       entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteEntryCountGet(L7_uchar8 familyType,
                                      L7_uint32 *entryCount)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }
  return pimsmMapExtenIpMrouteEntryCountGet(pimsmMapCbPtr, entryCount); 
}

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count Get.
*
* @param    familyType   @b{(input)}        Address Family type
* @param    heCount      @b{(output)}       entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteHighestEntryCountGet(L7_uchar8 familyType,
                                             L7_uint32 *heCount)
{
    return L7_NOT_SUPPORTED;
}

/******************************************************************************
* @purpose  Get routine for DR-priority
*
* @param    familyType      @b{(input)}   Address Family type.
* @param    rtrIfNum        @b{(input)}   pointer to router interface number
* @param    drPriority      @b{(output)}  DR-priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
******************************************************************************/
L7_RC_t pimsmMapProtocolInterfaceDRPriorityGet( L7_uchar8 familyType, 
                  L7_uint32 rtrIfNum,L7_uint32 *drPriority)
{

  L7_uint32 intIfNum;
  if(drPriority == L7_NULLPTR)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid drPriority(%p)",(void *)drPriority);
      return L7_FAILURE;
    }

  if(mcastIpMapRtrIntfToIntIfNum(familyType, rtrIfNum, 
                                 &intIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to Convert rtrIfNum(%d) to intIfNum",
                      rtrIfNum);
    return L7_FAILURE;
  }

  return pimsmMapInterfaceDRPriorityGet(familyType, intIfNum, drPriority);

}
/******************************************************************************
* @purpose  Get the bsr border on an interface
*
* @param    familyType     @b{(input)}  Address Family type.
* @param    rtrIfNum       @b{(input)}  pointer to router interface number
* @param    bsrborder      @b{(output)} bsr border
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
******************************************************************************/
L7_RC_t pimsmMapProtocolInterfaceBsrBorderGet(L7_uchar8 familyType, 
                                              L7_uint32 rtrIfNum,
                                              L7_uint32 *bsrBorder)
{ 
  L7_uint32 intIfNum;
  
  if(mcastIpMapRtrIntfToIntIfNum(familyType, rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to Convert rtrIfNum(%d) to intIfNum",
                      rtrIfNum);
    return L7_FAILURE;
  }
  return pimsmMapInterfaceBsrBorderGet(familyType, intIfNum, bsrBorder);
}
/*********************************************************************
* @purpose  To get the next entry in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(inout)} Group Address
* @param    pSGSrcAddr    @b{(inout)} Source Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGEntryNextGet(L7_uchar8 familyType,  
                               L7_inet_addr_t *pSGGrpAddr, 
                               L7_inet_addr_t *pSGSrcAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenSGEntryNextGet(pimsmMapCbPtr, pSGGrpAddr, 
                                      pSGSrcAddr);
}

/*********************************************************************
* @purpose  Obtain the next incoming interface in the SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(inout)} Group Address
* @param    pSGSrcAddr    @b{(inout)} Source Address
* @param    pIntIfNum     @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIEntryNextGet(L7_uchar8 familyType,  
                                L7_inet_addr_t *pSGGrpAddr, 
                                L7_inet_addr_t *pSGSrcAddr,
                                L7_uint32      *pIntIfNum)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenSGIEntryNextGet(pimsmMapCbPtr, pSGGrpAddr, 
                                       pSGSrcAddr, pIntIfNum);
}

/*********************************************************************
* @purpose  To get the expiry time in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pExpiryTime   @b{(output)}expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGExpiryTimeGet(L7_uchar8 familyType,
                                L7_inet_addr_t *pSGGrpAddr, 
                                L7_inet_addr_t *pSGSrcAddr,
                                L7_uint32      *pExpiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenSGExpiryTimeGet(pimsmMapCbPtr, pSGGrpAddr, 
                                       pSGSrcAddr, pExpiryTime);
}

/*********************************************************************
* @purpose  To get the Up time in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pUpTime       @b{(output)}Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGUpTimeGet(L7_uchar8 familyType,
                            L7_inet_addr_t *pSGGrpAddr, 
                            L7_inet_addr_t *pSGSrcAddr,
                            L7_uint32      *pUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenSGUpTimeGet(pimsmMapCbPtr, pSGGrpAddr, 
                                   pSGSrcAddr, pUpTime);
}

/*********************************************************************
* @purpose  To get the Rpf Address in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pRpfAddr      @b{(output)} Rpf Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRpfAddrGet(L7_uchar8 familyType,
                             L7_inet_addr_t *pSGGrpAddr, 
                             L7_inet_addr_t *pSGSrcAddr,
                             L7_inet_addr_t *pRpfAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenSGRpfAddrGet(pimsmMapCbPtr, pSGGrpAddr, 
                                    pSGSrcAddr, pRpfAddr);
}

/*********************************************************************
* @purpose  To get the Flags in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pSGFalgs      @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGFlagsGet(L7_uchar8 familyType,
                           L7_inet_addr_t *pSGGrpAddr, 
                           L7_inet_addr_t *pSGSrcAddr,
                           L7_uint32      *pSGFlags)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenSGFlagsGet(pimsmMapCbPtr, pSGGrpAddr, 
                                  pSGSrcAddr, pSGFlags);
}

/*********************************************************************
* @purpose  To get the If Index in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pSGIfIndex    @b{(output)} SG If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIfIndexGet(L7_uchar8 familyType,
                             L7_inet_addr_t *pSGGrpAddr, 
                             L7_inet_addr_t *pSGSrcAddr,
                             L7_uint32      *pSGIfIndex)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenSGIfIndexGet(pimsmMapCbPtr, pSGGrpAddr, 
                                    pSGSrcAddr, pSGIfIndex);
}

/*********************************************************************
* @purpose  To get the next entry in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(inout)} Group Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGEntryNextGet(L7_uchar8 familyType,  
                                  L7_inet_addr_t *pSGGrpAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarGEntryNextGet(pimsmMapCbPtr, pSGGrpAddr);
}

/*********************************************************************
* @purpose  Obtain the next incoming interface in the Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(inout)} Group Address
* @param    pIntIfNum     @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIEntryNextGet(L7_uchar8 familyType,  
                                   L7_inet_addr_t *pGrpAddr,
                                   L7_uint32      *pIntIfNum)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarGIEntryNextGet(pimsmMapCbPtr, pGrpAddr, 
                                          pIntIfNum);
}

/*********************************************************************
* @purpose  To get the expiry time in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pExpiryTime   @b{(output)}expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGExpiryTimeGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *pGrpAddr, 
                                   L7_uint32      *pExpiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarGExpiryTimeGet(pimsmMapCbPtr, pGrpAddr, 
                                          pExpiryTime);
}

/*********************************************************************
* @purpose  To get the Up time in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pUpTime       @b{(output)}Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGUpTimeGet(L7_uchar8 familyType,
                               L7_inet_addr_t *pGrpAddr, 
                               L7_uint32      *pUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarGUpTimeGet(pimsmMapCbPtr, pGrpAddr, 
                                      pUpTime);
}

/*********************************************************************
* @purpose  To get the Rpf Address in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pRpfAddr      @b{(output)} Rpf Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRpfAddrGet(L7_uchar8 familyType,
                                L7_inet_addr_t *pGrpAddr, 
                                L7_inet_addr_t *pRpfAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarGRpfAddrGet(pimsmMapCbPtr, pGrpAddr, 
                                       pRpfAddr);
}

/*********************************************************************
* @purpose  To get the Flags in Star G TABLE.
*
* @param    familyType    @b{(input)}  Address Family type
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pFlags        @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGFlagsGet(L7_uchar8 familyType,
                              L7_inet_addr_t *pGrpAddr, 
                              L7_uint32      *pFlags)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarGFlagsGet(pimsmMapCbPtr, pGrpAddr,
                                     pFlags);
}

/*********************************************************************
* @purpose  To get the If Index in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pIfIndex      @b{(output)} StarG If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIfIndexGet(L7_uchar8 familyType,
                                L7_inet_addr_t *pGrpAddr, 
                                L7_uint32      *pIfIndex)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarGIfIndexGet(pimsmMapCbPtr, pGrpAddr, 
                                       pIfIndex);
}
#ifdef PIMSM_MAP_TBD
/* Star Star RP API's */
/*********************************************************************
* @purpose  To get the next entry in Star Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pRpAddr       @b{(inout)} RP Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPEntryNextGet(L7_uchar8 familyType,  
                                       L7_inet_addr_t *pRpAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarStarRPEntryNextGet(pimsmMapCbPtr, pRpAddr);
}

/*********************************************************************
* @purpose  Obtain the next incoming interface in the Star 
*           Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pRpAddr       @b{(inout)} RP Address.
* @param    pIntIfNum     @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPIEntryNextGet(L7_uchar8 familyType,  
                                        L7_inet_addr_t *pRpAddr,
                                        L7_uint32      *pIntIfNum)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarStarRPIEntryNextGet(pimsmMapCbPtr, pRpAddr, 
                                               pIntIfNum);
}

/*********************************************************************
* @purpose  To get the expiry time in Star Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pRpAddr       @b{(input)} RP Address
* @param    pExpiryTime   @b{(output)} expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPExpiryTimeGet(L7_uchar8 familyType,
                                        L7_inet_addr_t *pRpAddr, 
                                        L7_uint32      *pExpiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarStarRPExpiryTimeGet(pimsmMapCbPtr, pRpAddr, 
                                               pExpiryTime);
}

/*********************************************************************
* @purpose  To get the Up time in Star Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pRpAddr       @b{(input)} RP Address
* @param    pUpTime       @b{(output)} Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPUpTimeGet(L7_uchar8 familyType,
                                    L7_inet_addr_t *pRpAddr, 
                                    L7_uint32      *pUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarStarRPUpTimeGet(pimsmMapCbPtr, pRpAddr, 
                                           pUpTime);
}

/*********************************************************************
* @purpose  To get the Rpf Address in Star Star RP TABLE.
*
* @param    familyType    @b{(input)}  Address Family type
* @param    pRpAddr       @b{(input)}  RP Address
* @param    pRpfAddr      @b{(output)} Rpf Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPRpfAddrGet(L7_uchar8 familyType,
                                     L7_inet_addr_t *pRpAddr, 
                                     L7_inet_addr_t *pRpfAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarStarRPRpfAddrGet(pimsmMapCbPtr, pRpAddr, 
                                            pRpfAddr);
}

/*********************************************************************
* @purpose  To get the Flags in Star Star RP TABLE.
*
* @param    familyType    @b{(input)}  Address Family type
* @param    pRpAddr       @b{(input)}  RP Address
* @param    pFlags        @b{(output)} Flags
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPFlagsGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *pRpAddr, 
                                   L7_uint32      *pFlags)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarStarRPFlagsGet(pimsmMapCbPtr, pRpAddr,
                                          pFlags);
}

/*********************************************************************
* @purpose  To get the If Index in Star Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} RP Address
* @param    pIfIndex      @b{(output)} If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPIfIndexGet(L7_uchar8 familyType,
                                     L7_inet_addr_t *pRpAddr, 
                                     L7_uint32      *pIfIndex)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }
  return  pimsmMapExtenStarStarRPIfIndexGet(pimsmMapCbPtr, pRpAddr, 
                                            pIfIndex);
}
#endif
/*********************************************************************
* @purpose  Send event to PIMSM Vendor
*
* @param    familyType    @b{(input)}     Address Family type
* @param    eventType     @b{(input)}     Event Type
* @param    msgLen        @b{(input)}     Message Length
* @param    eventMsg      @b{(input)}     Event Message
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCommonCallback(L7_uchar8 familyType,
                                  L7_uint32  eventType, 
                                  L7_uint32  msgLen, 
                                  void *eventMsg)
{
  /*MCAST_CB_HNDL_t  cbHandle;*/
  pimsmMapCB_t     *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;
  L7_uint32 intIfNum = 0;
  L7_BOOL doIntfOperationalCheck = L7_FALSE;

  if ((familyType != L7_AF_INET) &&
      (familyType != L7_AF_INET6))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\nInput parameters are not" 
                    "valid for familyType:%d.\n", familyType);
    return L7_FAILURE;
  }
  
  if (pimsmMapPimsmIsOperational(familyType) != L7_TRUE)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Received event when pimsm is not operational");
    return L7_FAILURE;
  }
  
  if ((eventType != MCAST_MFC_NOCACHE_EVENT) &&
      (eventType != MCAST_MFC_WRONGIF_EVENT) &&
      (eventType != MCAST_MFC_WHOLEPKT_EVENT) &&
      (eventType != MCAST_MFC_ENTRY_EXPIRE_EVENT) &&      
      (eventType != MCAST_EVENT_MGMD_GROUP_UPDATE) &&
      (eventType != MCAST_EVENT_RTO_BEST_ROUTE_CHANGE) &&
      (eventType != MCAST_EVENT_PIMSM_CONTROL_PKT_RECV) &&
      (eventType != MCAST_EVENT_IPv6_CONTROL_PKT_RECV) &&
      (eventType != MCAST_EVENT_PIMSM_TIMER_EXPIRY)&&
      (eventType != MCAST_EVENT_STATIC_MROUTE_CHANGE)&&
      (eventType != MCAST_EVENT_ADMINSCOPE_BOUNDARY))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid eventType(%d)",eventType);
    return L7_FAILURE;
  }

  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }

  /* Check if PIM-SM is Operational on the interface to which the event is
   * addressed to.
   * As of now, we do it only for events from MFC and MGMD.
   */
  switch (eventType)
  {
    case MCAST_EVENT_MGMD_GROUP_UPDATE:
      rtrIfNum = ((mgmdMrpEventInfo_t*)eventMsg)->rtrIfNum;
      doIntfOperationalCheck = L7_TRUE;
      break;

    case MCAST_EVENT_ADMINSCOPE_BOUNDARY:
      rtrIfNum = ((mcastAdminMsgInfo_t *)eventMsg)->intIfNum;
      doIntfOperationalCheck = L7_TRUE;
      break;

    case MCAST_EVENT_STATIC_MROUTE_CHANGE:
      rtrIfNum = ((mcastRPFInfo_t *)eventMsg)->rpfIfIndex;
      doIntfOperationalCheck = L7_TRUE;
      break;

    case MCAST_MFC_NOCACHE_EVENT:
    case MCAST_MFC_WRONGIF_EVENT:
    case MCAST_MFC_ENTRY_EXPIRE_EVENT:
    case MCAST_MFC_WHOLEPKT_EVENT:
      rtrIfNum = ((mfcEntry_t*)eventMsg)->iif;
      doIntfOperationalCheck = L7_TRUE;
      break;

    default:
      break;
  }

  if (doIntfOperationalCheck == L7_TRUE)
  {
    if (mcastIpMapRtrIntfToIntIfNum (familyType, rtrIfNum, &intIfNum) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "Failed to convert rtrIfNum (%d) to intIfNum for "
                      "familyType %d", rtrIfNum, familyType);
      return L7_FAILURE;
    }
    if (pimsmMapIntfIsOperational (familyType, intIfNum) != L7_TRUE)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,
                      "PIM-SM is not Operational on rtrIfNum - %d for Family - %d"
                      " for eventType - %d", rtrIfNum, familyType, eventType);
      return L7_FAILURE;
    }
  }

  if (eventType == MCAST_MFC_WHOLEPKT_EVENT)
  {
    if(pimsmWholePacketEventQueue(familyType, (mfcEntry_t *)eventMsg) == L7_SUCCESS)
    {
      return L7_SUCCESS; 
    }
    else
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES," Failed to send whole pkt event ");
    }
  }
  else
  {
    return pimsmMapMessageQueueSend(familyType, eventType, msgLen, eventMsg);
  }
  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  RTO Best Route Callback for IPv4
*
* @param    none
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments none
*
* @end
*********************************************************************/
void
pimsmMapRto4BestRouteClientCallback (void)
{
  pimsmMapCB_t* pimsmMapCbPtr = L7_NULLPTR;

  if (pimsmMapCtrlBlockGet(L7_AF_INET, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return;
  }
  pimsmRtoStartTimer (pimsmMapCbPtr->cbHandle);

  return;
}

/*********************************************************************
*
* @purpose  RTO Best Route Callback 
*
* @param    none
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments none
*
* @end
*********************************************************************/

void
pimsmMapRto6BestRouteClientCallback (void)
{
  pimsmMapCB_t* pimsm6MapCbPtr = L7_NULLPTR;

  if (pimsmMapCtrlBlockGet(L7_AF_INET6, &pimsm6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return;
  }
  pimsmRtoStartTimer (pimsm6MapCbPtr->cbHandle);

  return;
}

 
/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}     Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapSGOIFGet(L7_uchar8 familyType,
                                           L7_inet_addr_t *ipMRouteGroup,
                                           L7_inet_addr_t *ipMRouteSource,
                                           L7_inet_addr_t *ipMRouteSourceMask,
                                           L7_INTF_MASK_t      *outIntIfMask)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }   
   return pimsmMapExtenSGOIFGet(pimsmMapCbPtr, ipMRouteGroup,
        ipMRouteSource, ipMRouteSourceMask, outIntIfMask);
}


/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}     Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapStarGOIFGet(L7_uchar8 familyType,
                                           L7_inet_addr_t *ipMRouteGroup,
                                           L7_INTF_MASK_t      *outIntIfMask)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }   
   return pimsmMapExtenStarGOIFGet(pimsmMapCbPtr, ipMRouteGroup,
        outIntIfMask);
}
/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}     Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapOIFGet(L7_uchar8 familyType,
                       L7_inet_addr_t *ipMRouteGroup,
                       L7_inet_addr_t *ipMRouteSource,
                       L7_inet_addr_t *ipMRouteSourceMask,
                       L7_INTF_MASK_t      *outIntIfMask)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\nFunction Entered\n");
  /* Get the interface CB based on family */
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
    return L7_FAILURE;
  }   

  if (outIntIfMask != L7_NULLPTR)
  {
    if (inetIsAddressZero(ipMRouteSource) == L7_TRUE)
    {
      return pimsmMapStarGOIFGet(familyType, ipMRouteGroup, outIntIfMask);
    }
    else if ((inetIsAddressZero(ipMRouteSource) == L7_FALSE) &&
             (inetIsAddressZero(ipMRouteGroup) == L7_FALSE))
    {
      return pimsmMapSGOIFGet(familyType, ipMRouteGroup, ipMRouteSource, 
                              ipMRouteSourceMask, outIntIfMask);
    }
  }
  else
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIMSMMAP: Invalid Up time\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}



 /************************************************************************
   New implementation for the draft-ietf-pim-mib-v2-03.txt follows
 ************************************************************************/


/*********************************************************************
* @purpose  Gets the inetAddressType for the specified interface.
*
* @param    intIfNum     Internal Interface number
* @param    familyType    IPV4/V6
* @param    familyType     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceAddressTypeGet(L7_uint32 intIfNum, L7_uint32 familyType, L7_int32 *addrType)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32        rtrIfNum;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :"
    "ipMapIntIfNumToRtrIntf failed!!\n", __FUNCTION__, __LINE__);
     return  L7_FAILURE;
  }


   return  pimsmMapExtenInterfaceAddressTypeGet(pimsmMapCbPtr,
                                               rtrIfNum, addrType);
}


/*********************************************************************
* @purpose  Gets the Generation ID value for the specified interface.
*
* @param    intIfNum @b{(input)}  internal Interface number
* @param    familyType    IPV4/V6
* @param    *ipAddr   @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceGenerationIDValueGet(L7_uint32 intIfNum,
                                       L7_uint32 familyType, L7_uint32 *genIDValue)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32        rtrIfNum;
 
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :"
    "ipMapIntIfNumToRtrIntf failed!!\n", __FUNCTION__, __LINE__);
    return  L7_FAILURE;
  }


   return  pimsmMapExtenInterfaceGenerationIDValueGet(pimsmMapCbPtr,
                                               rtrIfNum, genIDValue);
}



/*********************************************************************
* @purpose  Sets the UseDRPriority option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    useDRPriority   Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceUseDRPrioritySet(L7_uint32 intIfNum,
                                                L7_uint32 familyType,
                                            L7_int32 useDRPriority)
{
  pimsmCfgCkt_t   *pCfg;
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32        rtrIfNum;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return L7_FAILURE;
  }
  if (pimsmMapInterfaceIsConfigurable(pimsmMapCbPtr,intIfNum, &pCfg) != L7_TRUE)
  {
       return L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :"
    "ipMapIntIfNumToRtrIntf failed!!\n", __FUNCTION__, __LINE__);
     return L7_FAILURE;
  }


    return  pimsmMapExtenInterfaceUseDRPrioritySet(pimsmMapCbPtr,
                                               rtrIfNum, useDRPriority);
}

/*********************************************************************
* @purpose Gets the UseDRPriority option for this interface
*
* @param    familyType       @b{(input)} Address Family type
* @param    intIfNum         @b{(input)} Internal Interface number
* @param    *useDRPriority   @b{(output)} Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceUseDRPriorityGet(L7_uint32 intIfNum,
                                          L7_uint32 familyType,
                                          L7_int32 *useDRPriority)
{
  pimsmCfgCkt_t   *pCfg;
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32        rtrIfNum;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (pimsmMapInterfaceIsConfigurable( pimsmMapCbPtr,intIfNum, &pCfg) != L7_TRUE)
  {
    
     return L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :"
    "ipMapIntIfNumToRtrIntf failed!!\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }


  return pimsmMapExtenInterfaceUseDRPriorityGet(pimsmMapCbPtr,
                                                rtrIfNum, useDRPriority);
}


/*********************************************************************
* @purpose  Sets the UseDRPriority option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    useDRPriority   Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  pimsmMapInterfaceBSRBorderSet(L7_uint32 intIfNum,
                                         L7_uint32 familyType,
                                        L7_uint32 bSRBorder)
                                        
{
  pimsmCfgCkt_t   *pCfg;
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  
   L7_uint32 rtrIfNum;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (pimsmMapInterfaceIsConfigurable( pimsmMapCbPtr,intIfNum, &pCfg) != L7_TRUE)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapInterfaceIsConfigurable failed!!\n");        
  
     return  L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "mcastIpMapIntIfNumToRtrIntf failed!!\n");
    
    return L7_FAILURE;
  }


     return  pimsmMapExtenInterfaceBSRBorderSet(pimsmMapCbPtr,
                                               rtrIfNum, bSRBorder);
}

/*********************************************************************
* @purpose  Gets the BSRBorder option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    *bSRBorder      BSRBorder option (TRUE/FALSE)
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceBSRBorderGet(L7_uint32 intIfNum,
                                                L7_uint32 familyType,
                                                L7_int32 *bSRBorder)
{
  pimsmCfgCkt_t   *pCfg;
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32        rtrIfNum;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }
  if (pimsmMapInterfaceIsConfigurable( pimsmMapCbPtr,intIfNum, &pCfg) != L7_TRUE)
  {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapInterfaceIsConfigurable failed!!\n");
      return L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
  
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "mcastIpMapIntIfNumToRtrIntf failed!!\n");
    
    return L7_FAILURE;
  }


  return  pimsmMapExtenInterfaceBSRBorderGet(pimsmMapCbPtr,
                                               rtrIfNum, bSRBorder);
}

/************ END OF PIMSM INTERFACE TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  To get if the neighbor DR Priority is present.
*
* @param    intIfNum    internal Interface number
* @param    familyType   IP Version
* @param    *inetIPAddr   IP Address
* @param    *bNbrDRPrio   Is Nbr Priority Set.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborDRPriorityPresentGet(L7_uint32 intIfNum, L7_uint32 familyType,
                                          L7_inet_addr_t *inetIPAddr, L7_uint32 *bNbrDRPrio)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  L7_uint32        rtrIfNum;
 
  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(familyType,
                                intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\n %s, %d :"
    "ipMapIntIfNumToRtrIntf failed!!\n", __FUNCTION__, __LINE__);
     return  L7_FAILURE;
  }


   return  pimsmMapExtenNeighborDRPriorityPresentGet(pimsmMapCbPtr,
                                               rtrIfNum, inetIPAddr, bNbrDRPrio);
}

/************ END OF PIMSM NEIGHBOUR TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SG entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGEntryGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGEntryGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SG TABLE
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_BOOL pimsmMapEntryNextGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                                   L7_inet_addr_t *pimSGSrcAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGEntryNextGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr);
}

/*********************************************************************
* @purpose  To get the SG UpstreamJoinState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGUpstreamJoinStateGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
                                       
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

    return  pimsmMapExtenSGUpstreamJoinStateGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}

/*********************************************************************
* @purpose  To get the SG UpstreamJoinTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGUpstreamJoinTimerGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

    return  pimsmMapExtenSGUpstreamJoinTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}

/*********************************************************************
* @purpose  To get the SG RPF UpstreamNeighbor.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGUpstreamNeighborGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                      L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGUpstreamNeighborGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, rpfNextHop);
}

/*********************************************************************
* @purpose  To get the SG RPFIfIndex.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFIfIndexGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

   return  pimsmMapExtenSGRPFIfIndexGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}

/*********************************************************************
* @purpose  To get the SG RPFRouteProtocol.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRouteProtocolGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

    return  pimsmMapExtenSGRPFRouteProtocolGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}

/*********************************************************************
* @purpose  To get the SG RPF RPFRouteAddress.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRouteAddressGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                      L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

  return pimsmMapExtenSGRPFRouteAddressGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, rpfNextHop);
}

/*********************************************************************
* @purpose  To get the SG RPFRoutePrefixLength.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRoutePrefixLengthGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

   return  pimsmMapExtenSGRPFRoutePrefixLengthGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}
/*********************************************************************
* @purpose  To get the SG RPFRouteMetricPref.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRouteMetricPrefGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGRPFRouteMetricPrefGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}
/*********************************************************************
* @purpose  To get the SG RPFRouteMetric.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRouteMetricGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

   return  pimsmMapExtenSGRPFRouteMetricGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}
/*********************************************************************
* @purpose  To get the SG KeepaliveTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGKeepaliveTimerGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

   return  pimsmMapExtenSGKeepaliveTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}
/*********************************************************************
* @purpose  To get the SG DRRegisterState.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGDRRegisterStateGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");  
    return L7_FAILURE;
  }

   return  pimsmMapExtenSGDRRegisterStateGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}
/*********************************************************************
* @purpose  To get the SG DRRegisterStopTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGDRRegisterStopTimerGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

   return pimsmMapExtenSGDRRegisterStopTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgUpTime);
}

/*********************************************************************
* @purpose  To get the SG RPF RPRegisterPMBRAddress
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPRegisterPMBRAddressGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                      L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    return L7_FAILURE;
  }

   return  pimsmMapExtenSGRPRegisterPMBRAddressGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, rpfNextHop);
}
/*********************************************************************
* @purpose  To get the SG RPF nextHop.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFNextHopGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                      L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

   return  pimsmMapExtenSGRPFNextHopGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, rpfNextHop);
}

/*********************************************************************
* @purpose  To get the SG SPTBit.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGSPTBitGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                   L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    return L7_FAILURE;
  }

    return  pimsmMapExtenSGSPTBitGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgSPTBit);
}

/************ END OF PIMSM SG TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGRpt entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptEntryGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                                 L7_inet_addr_t *pimSGSrcAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

     return  pimsmMapExtenSGRptEntryGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SGRpt TABLE
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptEntryNextGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                                          L7_inet_addr_t *pimSGSrcAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

    return  pimsmMapExtenSGRptEntryNextGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr);
}

/*********************************************************************
* @purpose  To get the SGRPT uptime.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptUpTimeGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgRPTUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    return  L7_FAILURE;
  }

     return  pimsmMapExtenSGRptUpTimeGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgRPTUpTime);
}

/*********************************************************************
* @purpose  To get the SGRPT upstream prune state.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPTUpstreamPruneStateGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                     L7_inet_addr_t *pimSGSrcAddr,
                                     L7_int32 *sgRPTPruneState)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

    return  pimsmMapExtenSGRPTUpstreamPruneStateGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgRPTPruneState);
}

/*********************************************************************
* @purpose  To get the SGRPT upstream Override Timer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPTUpstreamOverrideTimerGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgRPTPruneState)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

   return pimsmMapExtenSGRPTUpstreamOverrideTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, sgRPTPruneState);
}
/************ END OF PIMSM SGRPT TABLE USMDB APIs ***************/


/************ END OF PIMSM SSM RANGE TABLE USMDB APIs ***************/




/************ END OF PIMSM STATICRP TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether Group mapping entry exists for the specified index.
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapGroupMappingEntryGet(L7_uchar8 origin, L7_uchar8 familyType, 
                                             L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 length, 
                                             L7_inet_addr_t *pimMappingRPAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }
  
   return  pimsmMapExtenGroupMappingEntryGet(pimsmMapCbPtr, origin,
                                               pimMappingGrpAddr, length, pimMappingRPAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the Group mapping TABLE
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapGroupMappingEntryNextGet(L7_uchar8 *origin, L7_uchar8 *familyType, 
                                                 L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 *length, 
                                                 L7_inet_addr_t *pimMappingRPAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;
  

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(*familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }
  pimsmMapCbPtr->familyType = *familyType;
  
    return  pimsmMapExtenGroupMappingEntryNextGet(pimsmMapCbPtr, origin,
                                               pimMappingGrpAddr, length, pimMappingRPAddr);
}

/*********************************************************************
* @purpose  To get the groupMapping pim mode
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    pimGroupMappingMode  Group mapping mode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapGroupMappingPimModeGet(L7_uchar8 origin, L7_uchar8 familyType, 
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *pimGroupMappingMode)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

   return  pimsmMapExtenGroupMappingPimModeGet(pimsmMapCbPtr, origin,
                                               pimMappingGrpAddr, length, pimMappingRPAddr,
                                               pimGroupMappingMode);
}

/************ END OF PIMSM GROUP MAPPING TABLE USMDB APIs ***************/

/************ END OF PIMSM BSR CANDIDATE RP TABLE USMDB APIs ***************/


/************ END OF PIMSM BSR CANDIDATE BSR TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether bsrCandidateBSR entry exists for the specified index.
*
* @param    familyType             whether IPV4/V6
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSREntryGet(L7_uint32 familyType)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
       return  L7_FAILURE;
  }

    return  pimsmMapExtenbsrElectedBSREntryGet(pimsmMapCbPtr);

}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrCandidateRP TABLE
*
* @param    familyType             whether IPV4/V6
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSREntryNextGet(L7_int32 *familyType)

{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(*familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }
  pimsmMapCbPtr->familyType = *familyType;

  return pimsmMapExtenbsrElectedBSREntryNextGet(pimsmMapCbPtr);
}

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRAddr    To get the bsrElectedBSR Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSRAddrGet(L7_uint32 familyType, L7_uint32 *bsrElectedBSRAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
   PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

   return  pimsmMapExtenbsrElectedBSRAddrGet(pimsmMapCbPtr, 
                                                        bsrElectedBSRAddr);
}

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRPriority  To get the bsrElectedBSR Priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSRPriorityGet(L7_uint32 familyType, L7_uint32 *bsrElectedBSRPriority)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

  return  pimsmMapExtenbsrElectedBSRPriorityGet(pimsmMapCbPtr,
                                               bsrElectedBSRPriority);
}
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    hashMaskLen  To get the bsrElectedBSR hashMaskLen
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSRHashMaskLengthGet(L7_uint32 familyType, L7_uint32 *hashMaskLen)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

   return  pimsmMapExtenbsrElectedBSRHashMaskLengthGet(pimsmMapCbPtr,
                                               hashMaskLen);
}
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRExpiryTime  To get the bsrElectedBSR ExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSRExpiryTimeGet(L7_uint32 familyType, L7_uint32 *bsrElectedBSRExpiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

   return  pimsmMapExtenbsrElectedBSRExpiryTimeGet(pimsmMapCbPtr,
                                               bsrElectedBSRExpiryTime);
}
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrCRPAdvTimer  To get the bsrElectedBSR AdvTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  pimsmMapbsrElectedBSRCRPAdvTimerGet(L7_uint32 familyType, L7_uint32 *bsrCRPAdvTimer)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

    return  pimsmMapExtenbsrElectedBSRCRPAdvTimerGet(pimsmMapCbPtr,
                                               bsrCRPAdvTimer);
}


/*********************************************************************
* @purpose  Check whether StarG entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGEntryGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr) 
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGEntryGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr);
}


/*********************************************************************
* @purpose  To get the StarG pimMode
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGpimMode   To get the pimStarGpimMode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL pimsmMapStarGpimModeGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGpimMode,
                                        L7_RC_t *pRetCode)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    *pRetCode = L7_FAILURE;
    return L7_TRUE;
  }

  *pRetCode = pimsmMapExtenStarGpimModeGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, pimStarGpimMode);
  return L7_TRUE;
}

/*********************************************************************
* @purpose  To get the starG RP address.
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPAddr  To get the pimStarGRPAddr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPAddrGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                L7_inet_addr_t *pimStarGRPAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

   return  pimsmMapExtenStarGRPAddrGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, pimStarGRPAddr);
}

/*********************************************************************
* @purpose  To get the StarGRPOrigin
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPOrigin  To get the StarGRPOrigin
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPOriginGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                        L7_uint32 *pimStarGRPOrigin)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGRPOriginGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, pimStarGRPOrigin);
}
/*********************************************************************
* @purpose  To get the pimStarGRPIsLocal
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPIsLocal  To get the pimStarGRPIsLocal
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPIsLocalGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                          L7_uint32 *pimStarGRPIsLocal)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

  return pimsmMapExtenStarGRPIsLocalGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, pimStarGRPIsLocal);
}
/*********************************************************************
* @purpose  To get the JoinState
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGUpstreamJoinStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                                L7_uint32 *JoinState)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return L7_FAILURE;
  }

  return  pimsmMapExtenStarGUpstreamJoinStateGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, JoinState);
}

/*********************************************************************
* @purpose  To get the UpstreamJoinTimer
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGUpstreamJoinTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                                 L7_uint32 *JoinTimer)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGUpstreamJoinTimerGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, JoinTimer);
}

/*********************************************************************
* @purpose  To get the UpstreamNeighbor
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    UpstreamNbr  To get the UpstreamNbr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGUpstreamNbrGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                     L7_inet_addr_t *UpstreamNbr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGUpstreamNbrGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, UpstreamNbr);
}

/*********************************************************************
* @purpose  To get the RPFIfIndex
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFIfIndex  To get the RPFIfIndex
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFIfIndexGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, 
                                         L7_uint32 *RPFIfIndex)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

     return  pimsmMapExtenStarGRPFIfIndexGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, RPFIfIndex);
}

/*********************************************************************
* @purpose  To get the RPFNextHop
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFNextHop  To get the RPFNextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFNextHopGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, 
                                                     L7_inet_addr_t *RPFNextHop)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGRPFNextHopGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, RPFNextHop);
}

/*********************************************************************
* @purpose  To get the RPFRouteProtocol
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteProtocol  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRouteProtocolGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                               L7_uint32 *RouteProtocol)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGRPFRouteProtocolGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, RouteProtocol);
}

/*********************************************************************
* @purpose  To get the RPFRouteAddress
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteAddress  To get the RPFRouteAddress
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRouteAddrGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                           L7_inet_addr_t *RPFRouteAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

    return  pimsmMapExtenStarGRPFRouteAddrGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, RPFRouteAddr);
}

/*********************************************************************
* @purpose  To get the RPFRoutePrefixLength
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRoutePrefixLength  To get the RPFRoutePrefixLength
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRoutePrefixLenGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, 
                                                L7_uint32 *RPFRoutePrefixLen)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

    return  pimsmMapExtenStarGRPFRoutePrefixLenGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, RPFRoutePrefixLen);
}

/*********************************************************************
* @purpose  To get the RPFRouteMetricPref
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteMetricPref  To get the RPFRouteMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRouteMetricPrefGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                                 L7_uint32 *RPFRouteMetricPref)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
      return L7_FAILURE;
  }

   return  pimsmMapExtenStarGRPFRouteMetricPrefGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, RPFRouteMetricPref);
}

/*********************************************************************
* @purpose  To get the RPFRouteMetric
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteMetric  To get the RPFRouteMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRouteMetricGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                              L7_uint32 *RPFRouteMetric)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGRPFRouteMetricGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, RPFRouteMetric);
}
/************ END OF PIMSM STARG TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether StarGI entry exists for the specified index.
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr      StarGI group address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIEntryGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGIEntryGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex);
}


/*********************************************************************
* @purpose  To get the StarGI uptime
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    pimStarGIUpTime    To get the pimStarGIUpTime 
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIUpTimeGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex,
                                  L7_uint32 *pimStarGIUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGIUpTimeGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex, pimStarGIUpTime);
}

/*********************************************************************
* @purpose  To get the starG LocalMembership
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    pimStarGIRPAddr  To get the pimStarGIRPAddr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGILocalMembershipGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *localMemShip)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

    return pimsmMapExtenStarGILocalMembershipGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex, localMemShip);
}

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIJoinPruneStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *JoinState)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }


   return  pimsmMapExtenStarGIJoinPruneStateGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex, JoinState);
}

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIPrunePendingTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *JoinTimer)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGIPrunePendingTimerGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex, JoinTimer);
}

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIJoinExpiryTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *JoinExpiry)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

    return  pimsmMapExtenStarGIJoinExpiryTimerGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex, JoinExpiry);
}

/*********************************************************************
* @purpose  To get the AssertState
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertState  To get the AssertState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIAssertStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *AssertState)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

     return  pimsmMapExtenStarGIAssertStateGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex, AssertState);
}

/*********************************************************************
* @purpose  To get the AssertTimer
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertTimer  To get the AssertTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIAssertTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *AssertTimer)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGIAssertTimerGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex, AssertTimer);
}

/*********************************************************************
* @purpose  To get the AssertWinnerAddr
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertWinnerAddr  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIAssertWinnerAddrGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_inet_addr_t *AssertWinner)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

    return  pimsmMapExtenStarGIAssertWinnerAddrGet(pimsmMapCbPtr,
                                               pimStarGIGrpAddr, IfIndex, AssertWinner);
}

/*********************************************************************
* @purpose  To get the AssertWinnerMetricPref
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    AssertWinnerMetricPref  To get the AssertWinnerMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIRPFRouteMetricPrefGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 ifindex, L7_uint32 *AssertWinnerMetricPref)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

    return  pimsmMapExtenStarGIRPFRouteMetricPrefGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, ifindex, AssertWinnerMetricPref);
}

/*********************************************************************
* @purpose  To get the AssertWinnerMetric
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    AssertWinnerMetric  To get the AssertWinnerMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIAssertWinnerMetricGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 ifindex, L7_uint32 *AssertWinnerMetric)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenStarGIAssertWinnerMetricGet(pimsmMapCbPtr,
                                               pimStarGGrpAddr, ifindex, AssertWinnerMetric);
}

/************ END OF PIMSM StarGI TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SG entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIEntryGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

   return  pimsmMapExtenSGIEntryGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg);
}

/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIUpTimeGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                     L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

   return  pimsmMapExtenSGIUpTimeGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg, sgUpTime);
}

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGILocalMembershipGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *LocalMembership)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGILocalMembershipGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg, LocalMembership);
}

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinPruneState JoinPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIJoinPruneStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                           L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *JoinPruneState)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

    return  pimsmMapExtenSGIJoinPruneStateGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg, JoinPruneState);
}

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIPrunePendingTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                    L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, L7_uint32 *JoinTimer)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGIPrunePendingTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, JoinTimer);
}

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIJoinExpiryTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                         L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, L7_uint32 *JoinExpiry)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }

   return  pimsmMapExtenSGIJoinExpiryTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, JoinExpiry);
}

/*********************************************************************
* @purpose  To get the AssertState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertState  To get the AssertState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIAssertStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, L7_uint32 *AssertState)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

   return pimsmMapExtenSGIAssertStateGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, AssertState);
}

/*********************************************************************
* @purpose  To get the AssertTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertTimer  To get the AssertTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIAssertTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                           L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertTimer)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

  return  pimsmMapExtenSGIAssertTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, AssertTimer);
}

/*********************************************************************
* @purpose  To get the AssertWinnerAddr
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerAddr  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIAssertWinnerAddrGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                    L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_inet_addr_t *AssertWinner)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return pimsmMapExtenSGIAssertWinnerAddrGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, AssertWinner);
}

/*********************************************************************
* @purpose  To get the AssertWinnerMetricPref
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerMetricPref  To get the AssertWinnerMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIRPFRouteMetricPrefGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetricPref)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

   return  pimsmMapExtenSGIRPFRouteMetricPrefGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, AssertWinnerMetricPref);
}

/*********************************************************************
* @purpose  To get the AssertWinnerMetric
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerMetric  To get the AssertWinnerMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIAssertWinnerMetricGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                         L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetric)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGIAssertWinnerMetricGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, AssertWinnerMetric);
}

/************ END OF PIMSM SGI TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGRptI entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIEntryGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                 L7_inet_addr_t *pimSGSrcAddr,L7_uint32 ifindexg)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

  return  pimsmMapExtenSGRptIEntryGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SGRptI TABLE
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIEntryNextGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                               L7_inet_addr_t *pimSGSrcAddr,L7_uint32 *ifindexg)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

   return pimsmMapExtenSGRptIEntryNextGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg);
}


/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIUpTimeGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                L7_inet_addr_t *pimSGSrcAddr,L7_uint32 ifindexg,L7_uint32 *sgUpTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return L7_FAILURE;
  }

    return pimsmMapExtenSGRptIUpTimeGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg, sgUpTime);
}

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptILocalMembershipGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                 L7_inet_addr_t *pimSGSrcAddr,L7_uint32 ifindexg,L7_uint32 *LocalMembership)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGRptILocalMembershipGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg, LocalMembership);
}

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinPruneState JoinPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIJoinPruneStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
            L7_inet_addr_t *pimSGSrcAddr,L7_uint32 ifindexg,L7_uint32 *JoinPruneState)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

  return pimsmMapExtenSGRptIJoinPruneStateGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, ifindexg, JoinPruneState);
}

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIPrunePendingTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                 L7_inet_addr_t *pimSGSrcAddr,L7_uint32 IfIndex,L7_uint32 *JoinTimer)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return L7_FAILURE;
  }

   return pimsmMapExtenSGRptIPrunePendingTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, JoinTimer);
}

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIJoinExpiryTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                      L7_inet_addr_t *pimSGSrcAddr,L7_uint32 IfIndex,L7_uint32 *JoinExpiry)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenSGRptIJoinExpiryTimerGet(pimsmMapCbPtr,
                                               pimSGGrpAddr, pimSGSrcAddr, IfIndex, JoinExpiry);
}

/*********************************************************************
* @purpose  Check whether Group mapping entry exists for the specified index.
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetEntryGet(L7_uchar8 origin, L7_uchar8 familyType, 
                                             L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 length, 
                                             L7_inet_addr_t *pimMappingRPAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
     return  L7_FAILURE;
  }

   return  pimsmMapExtenbsrRPSetEntryGet(pimsmMapCbPtr, origin,
                                               pimMappingGrpAddr, length, pimMappingRPAddr);
}

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the Group mapping TABLE
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetEntryNextGet(L7_uchar8 *origin,L7_uchar8 *familyType, 
                                                 L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 *length, 
                                                 L7_inet_addr_t *pimMappingRPAddr)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(*familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
    return  L7_FAILURE;
  }
  pimsmMapCbPtr->familyType = *familyType;

   return  pimsmMapExtenbsrRPSetEntryNextGet(pimsmMapCbPtr, origin,
                                               pimMappingGrpAddr, length, pimMappingRPAddr);
}
/*********************************************************************
* @purpose  To get the RPSetPriority
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetPriority  RPSetPriority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetPriorityGet(L7_uchar8 origin, L7_uchar8 familyType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetPriority)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n"); 
     return  L7_FAILURE;
  }

   return  pimsmMapExtenbsrRPSetPriorityGet(pimsmMapCbPtr, origin,
                                               pimMappingGrpAddr, length, pimMappingRPAddr,
                                               RPSetPriority);
}

/*********************************************************************
* @purpose  To get the RPSetHoldtime
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetHoldtime        RPSetHoldtime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetHoldtimeGet(L7_uchar8 origin, L7_uchar8 familyType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetHoldtime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "pimsmMapCtrlBlockGet failed!!\n");
      return L7_FAILURE;
  }

    return  pimsmMapExtenbsrRPSetHoldtimeGet(pimsmMapCbPtr, origin,
                                               pimMappingGrpAddr, length, pimMappingRPAddr,
                                               RPSetHoldtime);
}

/*********************************************************************
* @purpose  To get the RPSetExpiryTime
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetExpiryTime        RPSetExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetExpiryTimeGet(L7_uchar8 origin, L7_uchar8 familyType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetExpiryTime)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, "\n %s, %d :"
                        "Function Entered!!\n", __FUNCTION__, __LINE__);
  /* Get the interface CB based on family & store family type in CB*/
  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
     return  L7_FAILURE;
  }

    return  pimsmMapExtenbsrRPSetExpiryTimeGet(pimsmMapCbPtr, origin,
                                               pimMappingGrpAddr, length, pimMappingRPAddr,
                                               RPSetExpiryTime);
}



/***********************************************************************************
*@purpose To get staticRp entry from staticRp Table
*
*@param familyType                  whether IPV4/IPV6
*@param rpAddr                      Rp address
*@param rpGrpAddr                   Rp Group address
*@param prefixLen                   prefix length of the groupaddress
*@param indexstart                  pointer to the next entry
*@param override                    override flag
*
*@returns L7_SUCCESS    if entry exists
*@returns L7_FAILURE    if entry does not exist
*
*@notes
*@end
***********************************************************************************/

  L7_RC_t pimsmMapCliStaticRpNextGet(L7_uchar8 familyType,L7_inet_addr_t *rpAddr, L7_inet_addr_t *rpGrpAddr, 
                                                     L7_uchar8 *prefixLen, L7_uint32 *indexstart,L7_BOOL *override)

  {


   L7_uint32          index;
   pimsmMapCB_t       *pimsmMapCbPtr = L7_NULLPTR;
    pimsmCfgStaticRP_t *staticRp = L7_NULLPTR;  
   L7_inet_addr_t       cfgGrpRt;
   L7_inet_addr_t       paramGrpRt;
    L7_inet_addr_t      rpGrpMask;
    L7_BOOL            found = L7_FALSE; 
    L7_uchar8          maskAddr[IPV6_DISP_ADDR_LEN];
    L7_uchar8          groupAddr[IPV6_DISP_ADDR_LEN];
     L7_int32 a;
    /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }

    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
       return L7_FAILURE;
    }

    if (rpGrpAddr == L7_NULLPTR)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid group address(%p)",rpGrpAddr);
       return L7_FAILURE;
    }
    if (inetMaskLenToMask(familyType, *prefixLen, &rpGrpMask) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                        *prefixLen);
      return L7_FAILURE;
    }


    for(index=*indexstart;index<L7_MAX_STATIC_RP_NUM;index++)

   {
     staticRp = &pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmStaticRP[index];

     
      if (inetIsAddressZero(&staticRp->rpIpAddr) == L7_TRUE)
      {
         continue;
      }   
       a = L7_INET_ADDR_COMPARE(&staticRp->rpIpAddr,rpAddr);
      
      if(a==L7_NULL)
      
     {
       
       if (inetAddressAnd(&staticRp->rpGrpAddr,
                          &staticRp->rpGrpMask, &cfgGrpRt)!= L7_SUCCESS)
       {
         return L7_FAILURE;
       }
       
       if (inetAddressAnd(rpGrpAddr, &rpGrpMask, &paramGrpRt)!= L7_SUCCESS)
       {
         return L7_FAILURE;
       }
        
       if (L7_INET_IS_ADDR_EQUAL(&cfgGrpRt, &paramGrpRt))
       {
        
        continue;
       }
       
        found = L7_TRUE;
        if (inetCopy(rpAddr,&staticRp->rpIpAddr ) != L7_SUCCESS)
       {
          return L7_FAILURE;
       }

        if (inetCopy(rpGrpAddr, &staticRp->rpGrpAddr) != L7_SUCCESS)
       {
         return L7_FAILURE;
       }
         
       if (inetMaskToMaskLen(&staticRp->rpGrpMask, prefixLen) != L7_SUCCESS)
       {
         PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert grpMask(%s) to prefixLen",
                         inetAddrPrint(&staticRp->rpGrpMask,maskAddr));
         return L7_FAILURE;
       }
       
       *override = staticRp->overRide; 
       break;
       }
    
      else
       {
        found =L7_TRUE;

        if (inetCopy(rpAddr,&staticRp->rpIpAddr ) != L7_SUCCESS)
       {
          return L7_FAILURE;
       }

        if (inetCopy(rpGrpAddr, &staticRp->rpGrpAddr) != L7_SUCCESS)
       {
         return L7_FAILURE;
       }
         
       if (inetMaskToMaskLen(&staticRp->rpGrpMask, prefixLen) != L7_SUCCESS)
       {
         PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert grpMask(%s) to prefixLen",
                         inetAddrPrint(&staticRp->rpGrpMask,maskAddr));
         return L7_FAILURE;
       }
       *override = staticRp->overRide;           
       break;
        }

    }


     if (found !=L7_TRUE)
      {

         PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find next Static RP entry for \
                     Grp Address(%s)",inetAddrPrint(rpGrpAddr,groupAddr));
        return L7_FAILURE;
       }
      
       *indexstart=index++;
       return L7_SUCCESS;
   }
  
/****************************************************************************************************
*@purpose  To get the next candidateRp entry from candidateRp table
*
*@param  familyType           whether IPV4/IPV6
*@param  intIfNum             internal Interface Number
*@param  cRPGroupAddress      candidate Rp group address
*@param  cRPprefixLen         prefix length of the group address
*@param  indexstart           pointer to the next entry
*
*@returns L7_SUCCESS  if the entry exists
*@returns L7_FAILURE  if the does not exists
*
*@notes
*@end
****************************************************************************************************/


 L7_RC_t pimsmMapCliCandidateRPEntryNextGet(L7_uchar8 familyType,L7_uint32 *intIfNum,L7_inet_addr_t *cRPGroupAddress,
                                             L7_uchar8 *cRPprefixLen, L7_uint32 *indexstart)
 

  {


     L7_uint32            index ;

      pimsmMapCB_t         *pimsmMapCbPtr = L7_NULLPTR;
       pimsmCfgCandRPdata_t *candRpEntry = L7_NULLPTR;
      L7_inet_addr_t       cfgGrpRt;
      L7_inet_addr_t       paramGrpRt;
      L7_inet_addr_t      rpGrpMask;
      L7_BOOL            found = L7_FALSE; 
      L7_uchar8          groupAddr[IPV6_DISP_ADDR_LEN];        
      L7_inet_addr_t candgrpmask;

      /* Get the interface CB based on family & store family type in CB*/
    if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
    {
      /* Failed to get control block */
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to get the control block");
      return L7_FAILURE;
    }

    if (pimsmMapCbPtr->pPimsmMapCfgData == L7_NULL)
    {
       PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"PIM-SM component not started");
       return L7_FAILURE;
    }



   if ((intIfNum == L7_NULLPTR) || (cRPGroupAddress == L7_NULLPTR) ||
        (cRPprefixLen == L7_NULLPTR))
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid input parameters: \
                        intIfNum(%p) or cRpGroupAddress(%p) or \
                        candPrefixLen(%p)",intIfNum,cRPGroupAddress,
                        cRPprefixLen);
      return L7_FAILURE;
    }
 
  if (inetMaskLenToMask(familyType, *cRPprefixLen, &rpGrpMask) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                        *cRPprefixLen);
      return L7_FAILURE;
    }

  
  for(index=*indexstart;index<L7_MAX_CAND_RP_NUM;index++)
 
  {
    candRpEntry =  &pimsmMapCbPtr->pPimsmMapCfgData->rtr.
                                            pimsmCandidateRp[index];
    
      if(pimsmMapCbPtr->pPimsmMapCfgData->rtr.pimsmCandidateRp[index].intIfNum <= 0)
        continue;
           

           if (inetMaskLenToMask(familyType, candRpEntry-> grpPrefixLength, &candgrpmask) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to convert MaskLen(%d) to Mask",
                        candRpEntry-> grpPrefixLength);
        return L7_FAILURE;
       }        
       
       if (inetAddressAnd(&candRpEntry->grpAddress,
                          &candgrpmask, &cfgGrpRt)!= L7_SUCCESS)
       {
         return L7_FAILURE;
       }

       if (inetAddressAnd(cRPGroupAddress, &rpGrpMask, &paramGrpRt)!= L7_SUCCESS)
       {
         return L7_FAILURE;
       }

       if (L7_INET_ADDR_COMPARE(&cfgGrpRt, &paramGrpRt) == L7_NULL)
       {
         continue;
       }

        found = L7_TRUE;

        if (inetCopy(cRPGroupAddress, &candRpEntry->grpAddress) != L7_SUCCESS)
       {
         return L7_FAILURE;
       }
       
       *cRPprefixLen =  candRpEntry-> grpPrefixLength; 
       *intIfNum = candRpEntry->intIfNum;

       break;
  }

     if (found !=L7_TRUE)
      {

         PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Failed to find next Static RP entry for \
                     Grp Address(%s)",inetAddrPrint(cRPGroupAddress,groupAddr));
        return L7_FAILURE;
       }

       *indexstart=index +1;
       return L7_SUCCESS;

   }   
