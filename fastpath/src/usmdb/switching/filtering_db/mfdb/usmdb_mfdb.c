/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_mfdb.c
*
* @purpose Provide interface to MFDB APIs for unitmgr component
*
* @component unitmgr
*
* @comments In an SVL environment, the VLAN ID, in the VLAN ID-MAC 
*           address combination shall be 0 (zero), or the default value.
*
* @create 03/27/2002
*
* @author rjindal
*
* @end
*
*********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <string.h>
#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_mfdb_api.h"
#include "comm_mask.h"
#include "defaultconfig.h"

#include "mfdb_api.h"


/*********************************************************************
*
* @purpose  To get the information of a VLANID-MAC entry in the
*           MFDB table.
*
* @param    L7_uint32         UnitIndex  @b((input)) the operation unit
* @param    usmdbMfdbEntry_t  *entry     @b((input)) pointer to structure
*
* @returns  L7_SUCCESS, if the entry is found
* @returns  L7_FAILURE, if the entry is not found
*
* @comments The entry's information gets copied in the input
*           'entry' structure.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbEntryInfoGet(L7_uint32 UnitIndex, usmdbMfdbEntry_t *entry)
{
  L7_uint32 i;
  L7_RC_t rc;
  mfdbData_t pData;

  memset(&pData, 0, sizeof(mfdbData_t));
  memcpy(pData.mfdbVidMacAddr, entry->usmdbMfdbVidMac, L7_MFDB_KEY_SIZE);
  
  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc == L7_SUCCESS)
  {
    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      entry->usmdbMfdbUser[i].usmdbMfdbCompId = pData.mfdbUser[i].componentId;
      entry->usmdbMfdbUser[i].usmdbMfdbType = pData.mfdbUser[i].type;
      memcpy(entry->usmdbMfdbUser[i].usmdbMfdbDescr, 
             pData.mfdbUser[i].description, L7_MFDB_COMPONENT_DESCR_STRING_LEN);
      
      /* set fwd and filter masks of 'entry' equal to masks of 'pData' */
      L7_INTF_MASKEQ(entry->usmdbMfdbUser[i].usmdbMfdbFwdMask, pData.mfdbUser[i].fwdPorts);
      L7_INTF_MASKEQ(entry->usmdbMfdbUser[i].usmdbMfdbFltMask, pData.mfdbUser[i].fltPorts);
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, get the next VLANID-MAC entry.
*
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *nextProt  @b((input)) pointer to protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
* @param    usmdbMfdbUserInfo_t   *compInfo  @b((output)) pointer to next
*                                            component's info structure
*
* @returns  L7_SUCCESS, if the next entry is found
* @returns  L7_FAILURE, if the next entry is not found
*
* @notes    The entry is indexed by vlanId, mac address,
*           and the protocol type.
* @notes    To get the first entry, the reference entry shall
*           be 0 (zero).
* @notes    The next entry's information gets copied in the input
*           'compInfo' structure.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbEntryGetNext(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                              L7_uint32 *nextProt, usmdbMfdbUserInfo_t *compInfo)
{
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbMfdbEntryProtocolGetNext(UnitIndex, vidMac, nextProt);
  if (rc == L7_SUCCESS)
  {
    rc = usmDbMfdbEntryProtocolInfoGet(UnitIndex, vidMac, *nextProt, compInfo);
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Given a user's VLANID-MAC entry, get the next VLANID-MAC
*           entry of the same user.
*
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  compId     @b((input)) component id, start
* @param    usmdbMfdbUserInfo_t  *compInfo  @b((output)) pointer to next
*                                           component's info structure
*
* @returns  L7_SUCCESS, if the next entry is found
* @returns  L7_FAILURE, if the next entry is not found
*
* @notes    The entry is indexed by vlanId, mac address,
*           and component id.
* @notes    To get the first entry, the reference entry shall
*           be 0 (zero).
* @notes    The entry's information gets copied in the input
*           'compInfo' structure.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbComponentEntryGetNext(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                       L7_uint32 compId, usmdbMfdbUserInfo_t *compInfo)
{
  L7_RC_t rc;
  mfdbData_t pData;
  mfdbUserInfo_t userInfo;

  memset(&pData, 0, sizeof(mfdbData_t));
  memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);

  memset(&userInfo, 0, sizeof(mfdbUserInfo_t));
  rc = mfdbComponentEntryGetNext(&pData, compId, &userInfo);
  if (rc == L7_SUCCESS)
  {
    memcpy(vidMac, pData.mfdbVidMacAddr, L7_MFDB_KEY_SIZE);

    compInfo->usmdbMfdbCompId = userInfo.componentId;
    compInfo->usmdbMfdbType = userInfo.type;
    memcpy(compInfo->usmdbMfdbDescr, 
           userInfo.description, L7_MFDB_COMPONENT_DESCR_STRING_LEN);

    /* set fwd and filter masks of 'compInfo' equal to masks of 'userInfo' */
    L7_INTF_MASKEQ(compInfo->usmdbMfdbFwdMask, userInfo.fwdPorts);
    L7_INTF_MASKEQ(compInfo->usmdbMfdbFltMask, userInfo.fltPorts);
    
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  To get the maximum number of entries that the MFDB table
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
L7_RC_t usmDbMfdbMaxTableEntriesGet(L7_uint32 UnitIndex, L7_uint32 *maxEntries)
{
  return mfdbMaxTableEntriesGet(maxEntries);
}


/*********************************************************************
*
* @purpose  To get the most number of entries ever in the MFDB table.
*               
* @param    L7_uint32  UnitIndex     @b((input)) the operation unit
* @param    L7_uint32  *mostEntries  @b((output)) pointer to the number
*                                                 of most entries
*	
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbMostEntriesGet(L7_uint32 UnitIndex, L7_uint32 *mostEntries)
{
  return mfdbMostEntriesGet(mostEntries);
}


/*********************************************************************
*
* @purpose  To get the number of current entries in the MFDB table.
*               
* @param    L7_uint32  UnitIndex     @b((input)) the operation unit
* @param    L7_uint32  *currEntries  @b((output)) pointer to the number
*                                                 of current entries
*	
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbCurrEntriesGet(L7_uint32 UnitIndex, L7_uint32 *currEntries)
{
  return mfdbCurrEntriesGet(currEntries);
}


/*********************************************************************
*
* @purpose  To check if a VLANID-MAC entry exists in the MFDB table.
*          
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
*	
* @returns  L7_SUCCESS, if the entry is found
* @returns  L7_FAILURE, if the entry is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbEntryFwdIntfGetExact(L7_uint32 UnitIndex, L7_uchar8 *vidMac)
{
  mfdbData_t pData;

  memset(&pData, 0, sizeof(mfdbData_t));
  memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);

  return mfdbEntryFind(&pData, L7_MATCH_EXACT);
}


/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, get the forwarding interfaces
*           of the next VLANID-MAC entry.
*
* @param    L7_uint32       UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8       *vidMac    @b((input)) pointer to vid-mac combo.
*                                      (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32       matchType  @b((input)) L7_MATCH_EXACT or
*                                                  L7_MATCH_GETNEXT
* @param    L7_INTF_MASK_t  *absMask   @b((input)) pointer to mask
*
* @returns  L7_SUCCESS, if the next entry is found
* @returns  L7_FAILURE, if the next entry is not found
*
* @notes    The entry is indexed by VLANID-MAC address combination.
* @notes    To get the first entry's forwarding interfaces, the 
*           reference entry's VLANID-MAC address shall be 0 (zero).
* @notes    To get the forwarding interfaces of a given VLANID-MAC
*           entry, the 'matchType' shall be L7_MATCH_EXACT.
* @notes    To get the forwarding interfaces of all VLANID-MAC entries,
*           the 'matchType' shall be L7_MATCH_GETNEXT.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbEntryFwdIntfGetNext(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                     L7_uint32 matchType, L7_INTF_MASK_t *absMask)
{
  L7_RC_t rc;
  mfdbData_t pData;
  L7_INTF_MASK_t tmpMask;

  if (matchType == L7_MATCH_GETNEXT)
  {
    memset(&pData, 0, sizeof(mfdbData_t));
    memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);

    rc = mfdbEntryFind(&pData, matchType);
    if (rc == L7_SUCCESS)
    {
      memcpy(vidMac, pData.mfdbVidMacAddr, L7_MFDB_KEY_SIZE);
    }
    else
    {
      return L7_FAILURE;
    }
  }

  memset(&tmpMask, 0, sizeof(L7_INTF_MASK_t));
  
  /* get the entry's forwarding interfaces only */
  rc = mfdbEntryAbsolutePortMaskGet(vidMac, &tmpMask);
  memcpy(absMask, &tmpMask, L7_INTF_INDICES);

  return rc;
}

/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, get the forwarding interfaces
*           of the next VLANID-MAC entry.
*
* @param    L7_uint32       UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8       *vidMac    @b((input)) pointer to vid-mac combo.
*                                      (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32       matchType  @b((input)) L7_MATCH_EXACT or
*                                                  L7_MATCH_GETNEXT
* @param    L7_INTF_MASK_t  *absMask   @b((input)) pointer to mask
*
* @returns  L7_SUCCESS, if the next entry is found
* @returns  L7_FAILURE, if the next entry is not found
*
* @notes    The entry is indexed by VLANID-MAC address combination.
* @notes    To get the first entry's forwarding interfaces, the 
*           reference entry's VLANID-MAC address shall be 0 (zero).
* @notes    To get the forwarding interfaces of a given VLANID-MAC
*           entry, the 'matchType' shall be L7_MATCH_EXACT.
* @notes    To get the forwarding interfaces of all VLANID-MAC entries,
*           the 'matchType' shall be L7_MATCH_GETNEXT.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbEntryLearntIntfGetNext(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                         L7_uint32 matchType, L7_INTF_MASK_t *absMask)
{
  L7_RC_t rc;
  mfdbData_t pData;
  L7_INTF_MASK_t tmpMask;

  if (matchType == L7_MATCH_GETNEXT)
  {
    memset(&pData, 0, sizeof(mfdbData_t));
    memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);

    rc = mfdbEntryFind(&pData, matchType);
    if (rc == L7_SUCCESS)
    {
      memcpy(vidMac, pData.mfdbVidMacAddr, L7_MFDB_KEY_SIZE);
    }
    else
    {
      return L7_FAILURE;
    }
  }

  memset(&tmpMask, 0, sizeof(L7_INTF_MASK_t));
  
  /* get the entry's forwarding interfaces only */
  rc = mfdbEntryLearntPortMaskGet(vidMac, &tmpMask);
  memcpy(absMask, &tmpMask, L7_INTF_INDICES);

  return rc;
}


/*********************************************************************
*
* @purpose  To check if a VLANID-MAC entry, that uses a specific
*           protocol, exists in the MFDB table.
*          
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  protocol   @b((input)) the protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
*	
* @returns  L7_SUCCESS, if the entry using that protocol exists
* @returns  L7_FAILURE, if the entry using that protocol does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbEntryProtocolGetExact(L7_uint32 UnitIndex, 
                                       L7_uchar8 *vidMac, L7_uint32 protocol)
{
  return mfdbEntryProtocolGetExact(vidMac, protocol);
}


/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, that uses a specific protocol, 
*           get the protocol of the next VLANID-MAC entry in the
*           MFDB table.
*
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *nextProt  @b((input)) pointer to protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
*
* @returns  L7_SUCCESS, if the next protocol is found
* @returns  L7_FAILURE, if the next protocol is not found
*
* @notes    The entry is indexed by vlanId, mac address, and the
*           protocol type.
* @notes    To get the first entry, the reference entry shall
*           be 0 (zero).
* @notes    The next protocol gets copied in the input pointer
*           'nextProt'.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbEntryProtocolGetNext(L7_uint32 UnitIndex, 
                                      L7_uchar8 *vidMac, L7_uint32 *nextProt)
{
  return mfdbEntryProtocolGetNext(vidMac, nextProt);
}


/*********************************************************************
*
* @purpose  To get the information of a given VLANID-MAC entry
*           that uses a specific protocol.
*
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  protocol   @b((input)) the protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
* @param    usmdbMfdbUserInfo_t  *compInfo  @b((output)) pointer to the
*                                           component's info structure
*
* @returns  L7_SUCCESS, if the entry using that protocol is found
* @returns  L7_FAILURE, if the entry using that protocol is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMfdbEntryProtocolInfoGet(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                      L7_uint32 protocol, usmdbMfdbUserInfo_t *compInfo)
{
  L7_RC_t rc;
  mfdbUserInfo_t userInfo;

  /* check validity of the protocol type */
  if ( (protocol <= L7_NULL) || (protocol >= L7_MFDB_TOTAL_NUM_PROTOCOLS) )
  {
    return L7_FAILURE;
  }

  else
  {
    rc = mfdbEntryProtocolInfoGet(vidMac, protocol, &userInfo);
    if (rc == L7_SUCCESS)
    {
      compInfo->usmdbMfdbCompId = userInfo.componentId;
      compInfo->usmdbMfdbType = userInfo.type;
      memcpy(compInfo->usmdbMfdbDescr, 
             userInfo.description, L7_MFDB_COMPONENT_DESCR_STRING_LEN);

      /* set fwd and filter masks of 'compInfo' equal to masks of 'userInfo' */
      L7_INTF_MASKEQ(compInfo->usmdbMfdbFwdMask, userInfo.fwdPorts);
      L7_INTF_MASKEQ(compInfo->usmdbMfdbFltMask, userInfo.fltPorts);

      return L7_SUCCESS;
    }

    return L7_FAILURE;
  }
}

