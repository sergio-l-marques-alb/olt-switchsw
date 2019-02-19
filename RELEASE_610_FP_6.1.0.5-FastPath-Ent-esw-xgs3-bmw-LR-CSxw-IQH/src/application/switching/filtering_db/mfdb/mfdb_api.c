/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\layer2\mfdb\mfdb_api.c
*
* @purpose Multicast Forwarding Database API Implementation File
*
* @component Multicast Forwarding Database
*
* @comments None
*
* @create 03/26/2002
*
* @author rjindal
*
* @end
*
*********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <string.h>
#include "flex.h"
#include "l7_common.h"
#include "comm_mask.h"
#include "defaultconfig.h"
#include "mfdb_api.h"
#include "mfdb.h"
#include "avl_api.h"

extern avlTree_t           mfdbTree;

/*********************************************************************
*
* @purpose  To check if a VLANID-MAC entry exists in the MFDB table.
*
* @param    char       *mac    the entry's mac address
* @param    L7_uint32  vlanId  the entry's vlan id
*
* @returns  L7_SUCCESS, if the entry exists
* @returns  L7_NOT_EXIST, if the entry does not exist
*
* @comments The user component has to perform this check before
*           adding the VLANID-MAC entry interfaces. 
*           If L7_SUCCESS, call mfdbEntryPortsAdd()
*           If L7_NOT_EXIST, call mfdbEntryAdd()
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryExist(char *mac, L7_uint32 vlanId)
{
  L7_ushort16 vid;
  mfdbData_t pData;

  memset(&pData, 0, sizeof(mfdbData_t));
  vid = (L7_ushort16)vlanId;
  memcpy(pData.mfdbVidMacAddr, &vid, L7_MFDB_VLANID_LEN);
  memcpy(&pData.mfdbVidMacAddr[L7_MFDB_VLANID_LEN], mac, L7_MAC_ADDR_LEN);

  return mfdbEntryFind(&pData, L7_MATCH_EXACT);
}


/*********************************************************************
*
* @purpose  To add a new VLANID-MAC entry to the MFDB table.
*
* @param    mfdbMemberInfo_t  *mfdbMemberInfo  pointer to the member
*                                              info structure
*
* @returns  L7_SUCCESS, if the entry is added
* @returns  L7_FAILURE, if the entry already exists
*
* @comments The user provides the VLAN ID, MAC address, component ID,
*           entry description, and forwarding-filtering port masks.
* @notes    This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryAdd(mfdbMemberInfo_t *mfdbMemberInfo)
{
  L7_uint32 firstIndex = 0;
  L7_ushort16 vid;
  L7_RC_t rc;
  mfdbData_t pData;

  memset(&pData, 0, sizeof(mfdbData_t));
  vid = (L7_ushort16)mfdbMemberInfo->vlanId;
  memcpy(pData.mfdbVidMacAddr, &vid, L7_MFDB_VLANID_LEN);
  memcpy(&pData.mfdbVidMacAddr[L7_MFDB_VLANID_LEN], mfdbMemberInfo->macAddr, L7_MAC_ADDR_LEN);

  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc == L7_SUCCESS)
  {
    return L7_FAILURE;
  }
      
  /* else, this is a new VLANID-MAC address entry */
  else
  {
    pData.mfdbUser[firstIndex].componentId = mfdbMemberInfo->user.componentId;
    pData.mfdbUser[firstIndex].type = mfdbMemberInfo->user.type;
    memcpy(pData.mfdbUser[firstIndex].description, 
           mfdbMemberInfo->user.description, L7_MFDB_COMPONENT_DESCR_STRING_LEN);
    
    /* set pData's fwd mask equal to the mfdbMemberInfo's fwd mask */
    L7_INTF_MASKEQ(pData.mfdbUser[firstIndex].fwdPorts, mfdbMemberInfo->user.fwdPorts);

    /* set pData's filter mask equal to the mfdbMemberInfo's filter mask */
    L7_INTF_MASKEQ(pData.mfdbUser[firstIndex].fltPorts, mfdbMemberInfo->user.fltPorts);
  }

  /* AVL does not guard against inserting too many entires .. yet */
  if (mfdbTree.count == L7_MFDB_MAX_MAC_ENTRIES)
  {
    /* tree full, skip insert */
    return L7_FAILURE;
  }
  
  /* insert the entry into the tree */
  rc = mfdbTreeEntryInsert(&pData);
  if (rc == L7_SUCCESS)
  {
    /*if (mfdbMemberInfo->user.type == L7_MFDB_TYPE_DYNAMIC)*/
    {
      /* call dtl from within mfdbDtlAdd() */
      rc = mfdbDtlAdd(&pData);
    }

    if (rc == L7_SUCCESS)
    {
      /* notify all user components */
      mfdbUserComponentsNotify(&pData, firstIndex, MFDB_ENTRY_ADD_NOTIFY);
    }
    else
    {
      (void) mfdbTreeEntryDelete(&pData);
      return L7_FAILURE;
    }
  }
  
  return rc;
}

// PTin added
L7_RC_t mfdbEntryModifyType(L7_uint16 vid, L7_uchar8 *macAddr, L7_uint32 newType)
{
  L7_uint32 i;
  L7_RC_t rc;
  mfdbData_t pData, tmpData;
  
  memset(&pData, 0, sizeof(mfdbData_t));
  memset(&tmpData, 0, sizeof(mfdbData_t));

  memcpy(pData.mfdbVidMacAddr, &vid, L7_MFDB_VLANID_LEN);
  memcpy(&pData.mfdbVidMacAddr[L7_MFDB_VLANID_LEN], macAddr, L7_MAC_ADDR_LEN);

  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc != L7_SUCCESS)
  {
    return L7_NOT_EXIST;
  }      
  /* => this VLANID-MAC address entry exists */
  else
  {
    /* save a copy of 'pData' in case 'dtl' returns L7_FAILURE, later in the routine */
    memcpy(&tmpData, &pData, sizeof(mfdbData_t));

    /* check if this user already exists */
    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      if (pData.mfdbUser[i].componentId != L7_NULL)
      {
        pData.mfdbUser[i].type = newType;
      }
    }

    /* update the tree entry */
    rc = mfdbTreeEntryUpdate(&pData);
    if (rc != L7_SUCCESS)
    {
      /* undo the recent tree entry update */
      (void) mfdbTreeEntryUpdate(&tmpData);
      return L7_FAILURE;
    }

    return rc;
  }
}
// PTin end

/*********************************************************************
*
* @purpose  To add ports to an existing VLANID-MAC entry in MFDB table.
*
* @param    mfdbMemberInfo_t  *mfdbMemberInfo  pointer to the member
*                                              info structure
*
* @returns  L7_SUCCESS, if the ports are added
* @returns  L7_FAILURE, if the ports are not added
* @returns  L7_NOT_EXIST, if the entry does not exist
*
* @comments The user provides the VLAN ID, MAC address, component ID,
*           entry description, and forwarding-filtering port masks.
* @notes     This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryPortsAdd(mfdbMemberInfo_t *mfdbMemberInfo)
{
  L7_uint32 i;
  L7_int32 index = -1;
  L7_ushort16 vid;
  L7_RC_t rc;
  mfdbData_t pData, tmpData;
  
  memset(&pData, 0, sizeof(mfdbData_t));
  memset(&tmpData, 0, sizeof(mfdbData_t));
  vid = (L7_ushort16)mfdbMemberInfo->vlanId;
  memcpy(pData.mfdbVidMacAddr, &vid, L7_MFDB_VLANID_LEN);
  memcpy(&pData.mfdbVidMacAddr[L7_MFDB_VLANID_LEN], mfdbMemberInfo->macAddr, L7_MAC_ADDR_LEN);

  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc != L7_SUCCESS)
  {
    return L7_NOT_EXIST;
  }
      
  /* => this VLANID-MAC address entry exists */
  else
  {
    /* save a copy of 'pData' in case 'dtl' returns L7_FAILURE, later in the routine */
    memcpy(&tmpData, &pData, sizeof(mfdbData_t));

    /* check if this user already exists */
    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      if (pData.mfdbUser[i].componentId != L7_NULL)
      {
        if (pData.mfdbUser[i].componentId == mfdbMemberInfo->user.componentId)
        {
          index = (L7_int32)i;
          break;
        }
      }
      /* if it does not exist, save the first empty index position for it */
      else if (index < L7_NULL)
      {
        index = (L7_int32)i;
      }
    }

    if (index < L7_NULL)
    {
      return L7_FAILURE;
    }

    /* add THIS USER'S info to the entry */
    
    pData.mfdbUser[index].componentId = mfdbMemberInfo->user.componentId;
    pData.mfdbUser[index].type = mfdbMemberInfo->user.type;
    memcpy(pData.mfdbUser[index].description, 
           mfdbMemberInfo->user.description, L7_MFDB_COMPONENT_DESCR_STRING_LEN);

    /* turn 'on' bits in pData's fwd mask that are 'on' in */
    /* either pData's fwd mask or mfdbMemberInfo's fwd mask */
    L7_INTF_MASKOREQ(pData.mfdbUser[index].fwdPorts, mfdbMemberInfo->user.fwdPorts);
    
    /* turn 'on' bits in pData's filter mask that are 'on' in    */
    /* either pData's filter mask or mfdbMemberInfo's filter mask */
    L7_INTF_MASKOREQ(pData.mfdbUser[index].fltPorts, mfdbMemberInfo->user.fltPorts);

    /* update the tree entry */
    rc = mfdbTreeEntryUpdate(&pData);
    if (rc == L7_SUCCESS)
    {
      /*if (mfdbMemberInfo->user.type == L7_MFDB_TYPE_DYNAMIC)*/
      {
        /* call dtl from within mfdbDtlAdd() */
        rc = mfdbDtlAdd(&pData);
      }
      
      if (rc == L7_SUCCESS)
      {
        /* notify all user components */
        mfdbUserComponentsNotify(&pData, (L7_uint32)index, MFDB_ENTRY_PORT_ADD_NOTIFY);
      }
      else
      {
        /* undo the recent tree entry update */
        (void) mfdbTreeEntryUpdate(&tmpData);
        return L7_FAILURE;
      }
    }

    return rc;
  }
}


/*********************************************************************
*
* @purpose  To delete a VLANID-MAC entry from the MFDB table.
*
* @param    mfdbMemberInfo_t  *mfdbMemberInfo  pointer to the member
*                                              info structure
*
* @returns  L7_SUCCESS, if the entry is deleted
* @returns  L7_NOT_EXIST, if the entry does not exist, or
*                         the user-component wanting to delete itself
*                         from the entry does not exist
*
* @comments The user provides the VLAN ID, MAC address, and
*           component ID.
* @notes     This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryDelete(mfdbMemberInfo_t *mfdbMemberInfo)
{
  L7_uint32 i;
  L7_uint32 index = 0;
  L7_uint32 numUsers = 0;
  L7_ushort16 vid;
  L7_BOOL found = L7_FALSE;
  L7_RC_t rc;
  mfdbData_t pData;

  memset(&pData, 0, sizeof(mfdbData_t));
  vid = (L7_ushort16)mfdbMemberInfo->vlanId;
  memcpy(pData.mfdbVidMacAddr, &vid, L7_MFDB_VLANID_LEN);
  memcpy(&pData.mfdbVidMacAddr[L7_MFDB_VLANID_LEN], mfdbMemberInfo->macAddr, L7_MAC_ADDR_LEN);

  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc != L7_SUCCESS)
  {
    return L7_NOT_EXIST;
  }
      
  /* => this VLANID-MAC address entry exists */
  else
  {
    /* check if this user exists */
    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      if (pData.mfdbUser[i].componentId != L7_NULL)
      {
        numUsers++;
        if (pData.mfdbUser[i].componentId == mfdbMemberInfo->user.componentId)
        {
          found = L7_TRUE;
          index = i;
        }
      }
    }

    /* if this user does not exist */
    if (found == L7_FALSE)
    {
      return L7_NOT_EXIST;
    }

    /* remove ONLY THIS USER from the entry's list of users */
    if (numUsers > 1)
    {
      memset(&pData.mfdbUser[index], 0, sizeof(mfdbUserInfo_t));
      rc = mfdbTreeEntryUpdate(&pData);
      if (rc == L7_SUCCESS)
      {
        /*if (mfdbMemberInfo->user.type == L7_MFDB_TYPE_DYNAMIC)*/
        {
          /* call dtl from within mfdbDtlAdd() */
          rc = mfdbDtlAdd(&pData);
        }
      }
    }
    /* delete the ENTIRE ENTRY, if this is the only user */
    else
    {
      /*if (mfdbMemberInfo->user.type == L7_MFDB_TYPE_DYNAMIC)*/
      {
        /* call dtl from within mfdbDtlRemove() */
        rc = mfdbDtlRemove(&pData);
      }

      if (rc == L7_SUCCESS)
      {
        rc = mfdbTreeEntryDelete(&pData);
      }
    }

    /* notify the entry user components */
    mfdbUserComponentsNotify(&pData, index, MFDB_ENTRY_DELETE_NOTIFY);

    return rc;
  }
}


/*********************************************************************
*
* @purpose  To delete ports from a VLANID-MAC entry in MFDB table.
*
* @param    mfdbMemberInfo_t  *mfdbMemberInfo  pointer to the member
*                                              info structure
*
* @returns  L7_SUCCESS, if the ports are deleted
* @returns  L7_NOT_EXIST, if the entry does not exist, or
*                         the user-component wanting to delete ports
*                         does not exist
*
* @comments The user provides the VLAN ID, MAC address, component ID,
*           and forwarding-filtering port masks.
* @notes    This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryPortsDelete(mfdbMemberInfo_t *mfdbMemberInfo)
{
  L7_uint32 i;
  L7_int32 index = 0;
  L7_ushort16 vid;
  L7_BOOL found = L7_FALSE;
  L7_RC_t rc;
  mfdbData_t pData;
  

  memset(&pData, 0, sizeof(mfdbData_t));
  vid = (L7_ushort16)mfdbMemberInfo->vlanId;
  memcpy(pData.mfdbVidMacAddr, &vid, L7_MFDB_VLANID_LEN);
  memcpy(&pData.mfdbVidMacAddr[L7_MFDB_VLANID_LEN], mfdbMemberInfo->macAddr, L7_MAC_ADDR_LEN);

  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc != L7_SUCCESS)
  {
    return L7_NOT_EXIST;
  }

  /* => this VLANID-MAC address entry exists */
  else
  {
    /* check if this user exists */
    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      if (pData.mfdbUser[i].componentId != L7_NULL)
      {
        if (pData.mfdbUser[i].componentId == mfdbMemberInfo->user.componentId)
        {
          found = L7_TRUE;
          index = (L7_int32)i;
          break;
        }
      }
    }
    
    /* if this user does not exist */
    if (found == L7_FALSE)
    {
      return L7_NOT_EXIST;
    }

    /* modify THIS USER'S info in the entry */

    /* turn 'off' bits in pData's fwd mask that are 'on' in mfdbMemberInfo's fwd mask */
    L7_INTF_MASKANDEQINV(pData.mfdbUser[index].fwdPorts, mfdbMemberInfo->user.fwdPorts);

    /* turn 'off' bits in pData's filter mask that are 'on' in mfdbMemberInfo's filter mask */
    L7_INTF_MASKANDEQINV(pData.mfdbUser[index].fltPorts, mfdbMemberInfo->user.fltPorts);

    /* update the tree entry */
    rc = mfdbTreeEntryUpdate(&pData);
    if (rc == L7_SUCCESS)
    {
      /*if (mfdbMemberInfo->user.type == L7_MFDB_TYPE_DYNAMIC)*/
      {
        /* call dtl from within mfdbDtlAdd() */
        rc = mfdbDtlAdd(&pData);
      }

      if (rc == L7_SUCCESS)
      {
        /* notify the entry user components */
        mfdbUserComponentsNotify(&pData, (L7_uint32)index, MFDB_ENTRY_PORT_DELETE_NOTIFY);
      }
    }

    return rc;
  }
}


/*********************************************************************
*
* @purpose  Given a user's VLANID-MAC entry, get the next VLANID-MAC
*           entry of the same user from the MFDB table. 
*
* @param    mfdbData_t      *pData     pointer to the entry structure
* @param    L7_uint32       userId     specific user component's id
* @param    mfdbUserInfo_t  *userInfo  pointer to the user component's
*                                      info structure
*
* @returns  L7_SUCCESS, if the next user entry is found
* @returns  L7_FAILURE, if the next user entry is not found
*
* @notes    The entry is indexed by vlanId, mac address, and user id
*           which are provided by the user.
* @notes    To get the first entry, the user sent info (i.e., the
*           vlan id, mac address, and user id) shall be 0 (zero).
* @notes    The entry's information gets copied in the input
*           'userInfo' structure.
*
* @end
*
*********************************************************************/
L7_RC_t mfdbComponentEntryGetNext(mfdbData_t *pData, L7_uint32 userId, 
                                  mfdbUserInfo_t *userInfo)
{
  L7_uint32 i;
  L7_RC_t rc;

  rc = mfdbEntryFind(pData, L7_MATCH_GETNEXT);
  while (rc == L7_SUCCESS)
  {
    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      if (pData->mfdbUser[i].componentId == userId)
      {
        userInfo->componentId = pData->mfdbUser[i].componentId;
        userInfo->type = pData->mfdbUser[i].type;
        memcpy(userInfo->description, 
               pData->mfdbUser[i].description, L7_MFDB_COMPONENT_DESCR_STRING_LEN);

        /* set fwd and filter masks of 'userInfo' equal to masks of 'pData' */
        L7_INTF_MASKEQ(userInfo->fwdPorts, pData->mfdbUser[i].fwdPorts);
        L7_INTF_MASKEQ(userInfo->fltPorts, pData->mfdbUser[i].fltPorts);
        
        return L7_SUCCESS;
      }
    }
    
    rc = mfdbEntryFind(pData, L7_MATCH_GETNEXT);
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  To get only the forwarding ports of a VLANID-MAC entry
*           in the MFDB table.
*
* @param    L7_uchar8       *vidMac   pointer to entry vid, mac combo.
*                                     (2-byte vlanId) + (6-byte mac)
* @param    L7_INTF_MASK_t  *absMask  pointer to the resultant mask
*
* @returns  L7_SUCCESS
*
* @comments The absolute mask of a given VLANID-MAC address entry is
*           obtained by combining all its user-components' forwarding
*           interfaces and removing its static filtering interfaces.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryAbsolutePortMaskGet(L7_uchar8 *vidMac, L7_INTF_MASK_t *absMask)
{
  L7_uint32 i;
  L7_uint32 index = 0;
  L7_RC_t rc;
  L7_BOOL mgmtConfig = L7_FALSE;
  mfdbData_t pData;
  L7_INTF_MASK_t tmpMask;

  memset(&pData, 0, sizeof(mfdbData_t));
  memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);
  memset(&tmpMask, 0, sizeof(L7_INTF_MASK_t));

  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc == L7_SUCCESS)
  {
    for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
    {
      /* turn off all bits in 'absMask'.*/
      L7_INTF_CLRMASKBIT(tmpMask, i);
    }

    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      if (pData.mfdbUser[i].componentId != L7_NULL)
      {
        /* turn on the bits in 'tmpMask' that are on in     */
        /* either 'tmpMask' or 'pData.mfdbUser[i].fwdPorts' */
        L7_INTF_MASKOREQ(tmpMask, pData.mfdbUser[i].fwdPorts);
        
        /* check for the presence of a management configured entry */
        if (pData.mfdbUser[i].type == L7_MFDB_TYPE_STATIC)
        {
          /* save the index */
          mgmtConfig = L7_TRUE;
          index = i;
        }
      }
    }

    if (mgmtConfig == L7_TRUE)
    {
      /* turn off the bits in 'tmpMask' that are on in   */
      /* both 'tmpMask' and 'pData.mfdbUser[i].fltPorts' */
      L7_INTF_MASKANDEQINV(tmpMask, pData.mfdbUser[index].fltPorts);
    }

    memcpy(absMask, &tmpMask, L7_INTF_INDICES);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get only the forwarding ports of a VLANID-MAC entry
*           in the MFDB table.
*
* @param    L7_uchar8       *vidMac   pointer to entry vid, mac combo.
*                                     (2-byte vlanId) + (6-byte mac)
* @param    L7_INTF_MASK_t  *absMask  pointer to the resultant mask
*
* @returns  L7_SUCCESS
*
* @comments The egress port mask of a given VLANID-MAC address entry is
*           obtained by combining all its user-components' forwarding
*           interfaces.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryLearntPortMaskGet(L7_uchar8 *vidMac, L7_INTF_MASK_t *absMask)
{
  L7_uint32 i;
  L7_RC_t rc;
  mfdbData_t pData;
  L7_INTF_MASK_t tmpMask;

  memset(&pData, 0, sizeof(mfdbData_t));
  memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);
  memset(&tmpMask, 0, sizeof(L7_INTF_MASK_t));

  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc == L7_SUCCESS)
  {
    for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
    {
      /* turn off all bits in 'absMask'.*/
      L7_INTF_CLRMASKBIT(tmpMask, i);
    }

    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      if (pData.mfdbUser[i].componentId != L7_NULL &&
          pData.mfdbUser[i].type != L7_MFDB_TYPE_STATIC)
      {
        /* turn on the bits in 'tmpMask' that are on in     */
        /* either 'tmpMask' or 'pData.mfdbUser[i].fwdPorts' */
        L7_INTF_MASKOREQ(tmpMask, pData.mfdbUser[i].fwdPorts);
      }
    }

    memcpy(absMask, &tmpMask, L7_INTF_INDICES);
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To check if a VLANID-MAC entry, that uses a specific
*           protocol, exists in the MFDB table.
*
* @param    L7_uchar8  *vidMac   pointer to vid-mac combo.
*                                (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  protocol  the protocol type
*                                (see L7_MFDB_PROTOCOL_TYPE_t)   
*
* @returns  L7_SUCCESS, if the entry using that protocol exists
* @returns  L7_FAILURE, if the entry does not exist, or
*                       component using that protocol does not exist.
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryProtocolGetExact(L7_uchar8 *vidMac, L7_uint32 protocol)
{
  L7_uint32 i;
  L7_RC_t rc;
  mfdbData_t pData;

  /* check validity of the protocol type */
  if ( (protocol <= L7_NULL) || (protocol >= L7_MFDB_TOTAL_NUM_PROTOCOLS) )
  {
    return L7_FAILURE;
  }

  else
  {
    memset(&pData, 0, sizeof(mfdbData_t));
    memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);

    rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
    if (rc == L7_SUCCESS)
    {
      for (i = 0; i < L7_MFDB_MAX_USERS; i++)
      {
        if (pData.mfdbUser[i].componentId == protocol)
        {
          return L7_SUCCESS;
        }
      }
    }
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, that uses a specific protocol, 
*           get the protocol of the next VLANID-MAC entry in the
*           MFDB table.
*
* @param    L7_uchar8  *vidMac    pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *nextProt  pointer to protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
*
* @returns  L7_SUCCESS, if the next protocol, in use, is found
* @returns  L7_FAILURE, if the entry identified by 'vidMac' is not
*                       found, or the next protocol is not found
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
L7_RC_t mfdbEntryProtocolGetNext(L7_uchar8 *vidMac, L7_uint32 *nextProt)
{
  L7_uint32 i;
  L7_RC_t rc;
  mfdbData_t pData;

  memset(&pData, 0, sizeof(mfdbData_t));
  memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);

  if ( (*nextProt == L7_NULL) || (*nextProt >= (L7_uint32)(L7_MFDB_TOTAL_NUM_PROTOCOLS - 1)) )
  {
    rc = mfdbEntryFind(&pData, L7_MATCH_GETNEXT);
    *nextProt = L7_NULL;
  }
  else
  {
    rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  }

  while (rc == L7_SUCCESS)
  {
    while (*nextProt < (L7_uint32)(L7_MFDB_TOTAL_NUM_PROTOCOLS - 1))
    {
      *nextProt = *nextProt + 1;
      for (i = 0; i < L7_MFDB_MAX_USERS; i++)
      {
        if (pData.mfdbUser[i].componentId == *nextProt)
        {
          /* copy vlanid+macAddr combo. of pData       */
          /* (that has the 'nextCompId') into 'vidMac' */
          memcpy(vidMac, pData.mfdbVidMacAddr, L7_MFDB_KEY_SIZE);
          return L7_SUCCESS;
        }
      }
    }

    rc = mfdbEntryFind(&pData, L7_MATCH_GETNEXT);
    *nextProt = L7_NULL;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  To get the information of a given VLANID-MAC entry
*           that uses a specific protocol.
*
* @param    L7_uchar8  *vidMac   pointer to vid-mac combo.
*                                (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  protocol  the protocol type
*                                (see L7_MFDB_PROTOCOL_TYPE_t)   
* @param    mfdbUserInfo_t  *userInfo  pointer to the user component's
*                                      info structure
*
* @returns  L7_SUCCESS, if the entry using this protocol is found
* @returns  L7_FAILURE, if the entry using this protocol is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t mfdbEntryProtocolInfoGet(L7_uchar8 *vidMac, L7_uint32 protocol, 
                                 mfdbUserInfo_t *userInfo)
{
  L7_uint32 i;
  L7_RC_t rc;
  mfdbData_t pData;

  memset(&pData, 0, sizeof(mfdbData_t));
  memcpy(pData.mfdbVidMacAddr, vidMac, L7_MFDB_KEY_SIZE);

  rc = mfdbEntryFind(&pData, L7_MATCH_EXACT);
  if (rc == L7_SUCCESS)
  {
    for (i = 0; i < L7_MFDB_MAX_USERS; i++)
    {
      if (pData.mfdbUser[i].componentId == protocol)
      {
        userInfo->componentId = pData.mfdbUser[i].componentId;
        userInfo->type = pData.mfdbUser[i].type;
        memcpy(userInfo->description, 
               pData.mfdbUser[i].description, L7_MFDB_COMPONENT_DESCR_STRING_LEN);

        /* set fwd and filter masks of 'userInfo' equal to masks of 'pData' */
        L7_INTF_MASKEQ(userInfo->fwdPorts, pData.mfdbUser[i].fwdPorts);
        L7_INTF_MASKEQ(userInfo->fltPorts, pData.mfdbUser[i].fltPorts);

        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

