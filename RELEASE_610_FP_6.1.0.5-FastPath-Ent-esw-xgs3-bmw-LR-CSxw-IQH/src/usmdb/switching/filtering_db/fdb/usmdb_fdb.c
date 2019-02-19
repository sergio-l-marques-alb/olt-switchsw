/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_fdb.c
*
* @purpose Provide interface to hardware API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 03-Nov-2000
*
* @author gaunce
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_filter_api.h"
#include "usmdb_mib_bridge_api.h"

#include "fdb_api.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*********************************************************************
*
* @purpose Get an entry from the forwarding database based on the
*          MAC address.
*               
*          
* @param L7_uint32       UnitIndex  the unit for this operation
* @param usmdbFdbEntry_t *entry     pointer to the structure
*	
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFDBEntryGet(L7_uint32 UnitIndex, usmdbFdbEntry_t *entry)
{
  dot1dTpFdbData_t data;

  if (fdbFind(entry->usmdbFdbMac, L7_MATCH_EXACT, &data) == L7_SUCCESS)
  {
    memcpy(entry, &data, sizeof(usmdbFdbEntry_t));
    /*Note: Structures usmdbFdbEntry_t and dot1dTpFdbData_t differ by 
    one *next pointer. So any change in the structures, would require
    the above memcopy also to be changed appropriately
    */
    return L7_SUCCESS;
  }
  return L7_FAILURE;

}


/*********************************************************************
*
* @purpose Get the next entry from the forwarding database.
*               
* @param L7_uint32       UnitIndex  the unit for this operation
* @param usmdbFdbEntry_t *entry     pointer to the structure
*	
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFDBEntryNextGet(L7_uint32 UnitIndex, usmdbFdbEntry_t *entry)
{
  dot1dTpFdbData_t data;

  if (fdbFind(entry->usmdbFdbMac, L7_MATCH_GETNEXT, &data) == L7_SUCCESS)
  {
    memcpy(entry, &data, sizeof(usmdbFdbEntry_t));
    /*Note: Structures usmdbFdbEntry_t and dot1dTpFdbData_t differ by 
    one *next pointer. So any change in the structures, would require
    the above memcopy also to be changed appropriately
    */
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the next entry from the FDB for a specific interface
*               
* @param L7_uint32       intIfNum @b((input)) internal interface number
* @param usmdbFdbEntry_t *entry   @b((input/output)) pointer to the structure
*	
* @returns L7_SUCCESS
* @returns L7_FAILURE, no more entries for this interface
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFDBIntfEntryNextGet(L7_uint32 intIfNum, usmdbFdbEntry_t *entry)
{
  dot1dTpFdbData_t data;

  while (fdbFind(entry->usmdbFdbMac, L7_MATCH_GETNEXT, &data) == L7_SUCCESS)
  {
    memcpy(entry, &data, sizeof(usmdbFdbEntry_t));
    if (entry->usmdbFdbIntIfNum == intIfNum)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get the next entry from the FDB for a specific vlan
*               
* @param L7_ushort16      vlanId   @b((input)) vlan identifier
* @param usmdbFdbEntry_t *entry    @b((input/output)) pointer to the structure
*	
* @returns L7_SUCCESS
* @returns L7_FAILURE, no more entries for this VLAN
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFDBVlanEntryNextGet(L7_ushort16 vlanId, usmdbFdbEntry_t *entry)
{
  dot1dTpFdbData_t data;

  memcpy(entry->usmdbFdbMac, &vlanId, L7_FDB_IVL_ID_LEN);

  while (fdbFind(entry->usmdbFdbMac, L7_MATCH_GETNEXT, &data) == L7_SUCCESS)
  {
    memcpy(entry, &data, sizeof(usmdbFdbEntry_t));
    if ((*(L7_ushort16 *)entry->usmdbFdbMac) == vlanId)
    {
      return L7_SUCCESS;
    }
    else
    {
      return L7_FAILURE;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  return the type of supported VL
*
* @param    type       pointer to the type
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbFDBTypeOfVLGet(L7_uint32 UnitIndex, L7_FDB_TYPE_t *type)
{
  return fdbGetTypeOfVL(type);
}


/*********************************************************************
* @purpose      get the next fdbID starting from a certain index.
*
* @param        UnitIndex   @b{(input)} unit index
* @param        startfrom   @b{(input)} index to start search from
* @param        *fdbId      @b{(output)} pointer to the return value
*
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbFdbIdNextGet(L7_uint32 UnitIndex, L7_uint32 startfrom, L7_uint32 *fdbId)
{
  return fdbIdNextGet(startfrom, fdbId);
}

/*********************************************************************
* @purpose      check to see if an FDB exists based on an FDB ID.
*
* @param        UnitIndex   @b{(input)} unit index
* @param        fdbId       @b{(input(} FDB ID to be checked
*
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbFdbIdGet(L7_uint32 UnitIndex, L7_uint32 fdbId)
{
  return fdbIdGet(fdbId);
}

/*********************************************************************
* @purpose      Gets the aging timeout value for an FDB
*               
*          
* @param        UnitIndex   @b{(input)} unit index
* @param        fdbId       @b{(input)} FDB ID
* @param        *timeOut    @b{(output)} aging timeout value
*	
* @returns      L7_SUCCESS
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFDBAddressAgingTimeoutGet(L7_uint32 UnitIndex, L7_uint32 fdbId, L7_uint32 *timeOut)
{
  *timeOut = fdbGetAddressAgingTimeOut(fdbId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose      Sets the aging timeout value for an FDB
*               
*          
* @param        UnitIndex   @b{(input)} unit index
* @param        fdbId       @b{(input)} FDB ID
* @param        timeOut     @b{(input)} aging timeout value
*	
* @returns      L7_SUCCESS
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFDBAddressAgingTimeoutSet(L7_uint32 UnitIndex, L7_uint32 fdbId, L7_uint32 timeOut)
{
  L7_RC_t rc = L7_FAILURE;

  if ((timeOut < L7_FDB_MIN_AGING_TIMEOUT) || (timeOut > L7_FDB_MAX_AGING_TIMEOUT))
  {
    return rc;
  }

  rc = fdbSetAddressAgingTimeOut(timeOut, fdbId);
  return rc;
}

/*********************************************************************
* @purpose  Returns the number of failed FDB insert attempts due to
*           failure to allocate space.
*
* @param    *badAdds     pointer to the number of failed FDB inserts to
*                        be returned
*
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbBadAddsGet(L7_uint32 UnitIndex, L7_uint32 *badAdds)
{
  return fdbBadAddsGet(badAdds);
}

/*********************************************************************
*
* @purpose  To get the maximum number of entries that the FDB table
*           can hold.
*
* @param    L7_uint32  UnitIndex    @b((input)) the operation unit
* @param    L7_uint32  *maxEntries  @b((output)) pointer to the number
*                                                of total entries
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbFdbMaxTableEntriesGet(L7_uint32 UnitIndex, L7_uint32 *maxEntries)
{
  return fdbMaxTableEntriesGet(maxEntries);
}
/*********************************************************************
* @purpose      gets a entry from the forwarding database
*               
*          
* @param        UnitIndex   @b{(input)} unit index
* @param        *val        @b{(output)} pointer to the value
*	
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbActiveAddrEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val=fdbActiveEntries();
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose      function to find the maximun number of entries in the forwarding database
*               
* @param        UnitIndex   @b{(input)} unit index
* @param        *val        @b{(output)}pointer to the value
*	
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMostAddrEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val=fdbMaxEntries();
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose      function to reset the maximun number of entries in the forwarding database
*               
* @param        UnitIndex   @b{(input)} unit index
*	
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMostAddrEntriesReset(L7_uint32 UnitIndex)
{
  fdbMaxEntriesReset();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Flush all learned entries from the L2FDB.
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
L7_RC_t usmDbFdbFlush(void)
{
    return fdbFlush();
}

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB.
*
* @param    L7_uint32     *entries   @{{output}}  Number of dynamic entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbDynamicEntriesGet(L7_uint32 *entries)
{
    return fdbDynamicEntriesGet(entries);
}

/*********************************************************************
* @purpose  Get count of internally added entries in the L2FDB.
*
* @param    L7_uint32     *entries   @{{output}}  Number of dynamic entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbInternalEntriesGet(L7_uint32 *entries)
{
    return fdbInternalEntriesGet(entries);
}

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB for a specific interface
*
* @param    L7_uint32     intIfNum   @((input))   Internal interface number
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbIntfDynamicEntriesGet(L7_uint32 intIfNum,
                                      L7_uint32 *entries)
{
    return fdbIntfDynamicEntriesGet(intIfNum, entries);;
}

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB for a specific VLAN
*
* @param    L7_ushort16    vlanId    @((input))   VLAN identifier
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbVlanDynamicEntriesGet(L7_ushort16 vlanId,
                                      L7_uint32   *entries)
{
    return fdbVlanDynamicEntriesGet(vlanId, entries);
}

/*********************************************************************
* @purpose  Get count of internal entries in the L2FDB for a specific interface
*
* @param    L7_uint32     intIfNum   @((input))   Internal interface number
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbIntfInternalEntriesGet(L7_uint32 intIfNum,
                                       L7_uint32 *entries)
{
    return fdbIntfInternalEntriesGet(intIfNum, entries);
}

/*********************************************************************
* @purpose  Get count of internal entries in the L2FDB for a specific VLAN
*
* @param    L7_ushort16    vlanId    @((input))   VLAN identifier
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbVlanInternalEntriesGet(L7_ushort16  vlanId,
                                       L7_uint32   *entries)
{
    return fdbVlanInternalEntriesGet(vlanId, entries);
}

/*********************************************************************
* @purpose  Determine if the specified MAC address is a member of the restricted set.
*
* @param  *macaddr - pointer to mac address 
*
* @returns L7_TRUE if restricted mac, L7_FALSE otherwise
*
* @end
*
*********************************************************************/
L7_BOOL usmDbFilterIsRestrictedFilterMac(L7_uchar8 *macaddr)
{
    L7_BOOL    rc;
    L7_uint32  i;

    /* --------------------------- */
    /* Check for 00:00:00:00:00:00 */
    /* --------------------------- */
    rc = L7_TRUE;
    for (i=0; i<6; i++)
    {
      if (macaddr[i] != 0x00)
      {
        rc = L7_FALSE;
        break;
      }
    }
    if (rc == L7_TRUE)
      return L7_TRUE;


    /* --------------------------- */
    /* Check for FF:FF:FF:FF:FF:FF */
    /* --------------------------- */
    rc = L7_TRUE;
    for (i=0; i<6; i++)
    {
      if (macaddr[i] != 0xFF)
      {
        rc = L7_FALSE;
        break;
      }
    }
    if (rc == L7_TRUE)
      return L7_TRUE;


    /* ------------------------------------------------ */
    /* Check for 01:80:C2:00:00:00 to 01:80:C2:00:00:10 */
    /* ------------------------------------------------ */
    rc = L7_FALSE;
    if (macaddr[0] == 0x01)
    {
      if (macaddr[1] == 0x80)
      {
        if (macaddr[2] == 0xC2)
        {
          if (macaddr[3] == 0x00)
          {
            if (macaddr[4] == 0x00)
            {
              if ((macaddr[5] == 0x10) || (macaddr[5] & 0xF0) == 0x00)
              {
                rc = L7_TRUE;
              }
            }
          }
        }
      }
    }
    if (rc == L7_TRUE)
      return L7_TRUE;


    /* ------------------------------------------------ */
    /* Check for 01:80:C2:00:00:20 to 01:80:C2:00:00:2F */
    /* ------------------------------------------------ */
    rc = L7_FALSE;
    if (macaddr[0] == 0x01)
    {
      if (macaddr[1] == 0x80)
      {
        if (macaddr[2] == 0xC2)
        {
          if (macaddr[3] == 0x00)
          {
            if (macaddr[4] == 0x00)
            {
              if ((macaddr[5] & 0xF0) == 0x20) 
              {
                rc = L7_TRUE;
              }
            }
          }
        }
      }
    }
    if (rc == L7_TRUE)
      return L7_TRUE;

    return L7_FALSE;

}

