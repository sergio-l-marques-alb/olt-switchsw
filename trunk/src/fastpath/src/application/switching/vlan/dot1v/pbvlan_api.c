/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\layer2\pbvlan\pbVlan_api.c
*
* @purpose Contains LVL7 Protocol-Based VLAN offerings 
*
* @component Protocol-based VLAN
*
* @comments None
*
* @create 01/07/2002
*
* @author rjindal
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <string.h>
#include "l7_common.h"
#include "defaultconfig.h"
#include "comm_mask.h"
#include "osapi.h"
#include "dtlapi.h"
#include "nimapi.h"
#include "usmdb_util_api.h"
#include "pbvlan.h"
#include "pbvlan_api.h"

#include "dot1q_api.h"


extern pbVlanCfgData_t     pbVlanCfgData;
extern L7_uint32          *pbVlanMaskOffsetToIntIfNum;


/*********************************************************************
*
* @purpose  To check if a group name already exists.
*
* @param    L7_char8  *Name  name of the group
*
* @returns  L7_TRUE, if the group name is found
* @returns  L7_FALSE, if the group name is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL pbVlanGroupExist(L7_char8 *Name)
{
  L7_uint32 i;

  /* check if the group name already exists */
  for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    if ( (pbVlanCfgData.group[i].inUse == L7_TRUE)  &&  (strcmp(pbVlanCfgData.group[i].name, Name)) == 0)
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To create a new group.
*
* @param    L7_uint32 groupId ID of the group
*
* @returns  L7_SUCCESS, if the group ID is added
* @returns  L7_FAILURE, if the group ID is not added
* @returns  L7_ALREADY_CONFIGURED, if the group ID already exists
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupCreate(L7_uint32 groupId)
{

  /* check if the group name already exists */
  if (pbVlanGroupGetExact(groupId) == L7_SUCCESS)
  {
    return L7_ALREADY_CONFIGURED;
  }

  /* check validity of the group */
  if ((groupId < L7_PBVLAN_MIN_NUM_GROUPS) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS))  
  {
    return L7_FAILURE;
  }

  pbVlanCfgData.group[groupId].groupID = groupId;
  pbVlanCfgData.group[groupId].inUse = L7_TRUE;
  pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To create a new group by name.
*
* @param    L7_char8  *Name  name of the group
*
* @returns  L7_SUCCESS, if the group name is added
* @returns  L7_FAILURE, if the group name is not added
* @returns  L7_ALREADY_CONFIGURED, if the group name already exists
*
* @comments If a group is created, it is also assigned a group id.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupNameCreate(L7_char8 *Name)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if (strlen(Name) > L7_PBVLAN_MAX_GROUP_NAME)
  {
    return L7_FAILURE;
  }
  /* check if the group name already exists */
  if (pbVlanGroupExist(Name) == L7_TRUE)
  {
    return L7_ALREADY_CONFIGURED;
  }

  /* if an empty slot is found in the database, add the new group's name */
  for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    if (pbVlanCfgData.group[i].inUse == L7_FALSE)
    {
      bzero(pbVlanCfgData.group[i].name, sizeof(pbVlanCfgData.group[i].name));
      osapiStrncpy(pbVlanCfgData.group[i].name, Name, L7_PBVLAN_MAX_GROUP_NAME);
      pbVlanCfgData.group[i].groupID = i;
      pbVlanCfgData.group[i].inUse = L7_TRUE;

      pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To delete a group.
*
* @param    L7_uint32  groupId  group id of an existing group 
*
* @returns  L7_SUCCESS, if the group is deleted
* @returns  L7_FAILURE, if invalid groupid or unconfigured group 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupDelete(L7_uint32 groupId)
{
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;

  /* check validity of the group */
  if ( ((groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS))  ||
       (pbVlanCfgData.group[groupId].inUse == L7_FALSE) )
  {
    return L7_FAILURE;
  }

  /* delete this group's vlanid */
  if (pbVlanCfgData.group[groupId].vlanid != L7_PBVLAN_NULL_VLANID)
  {
    rc = pbVlanGroupVlanIDDelete(groupId, pbVlanCfgData.group[groupId].vlanid);
  }

  /* delete this group's protocols */
  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if (pbVlanCfgData.group[groupId].protocol[i] != L7_PBVLAN_INVALID_PROTOCOL)
    {
      rc = pbVlanGroupProtocolDelete(groupId, pbVlanCfgData.group[groupId].protocol[i]);
    }
  }

  /* delete this group's ports */
  for (i = 1; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
  {
    /* turn off all bits in 'ports' mask */
    NIM_INTF_CLRMASKBIT(pbVlanCfgData.group[groupId].ports, i);
  }

  bzero(pbVlanCfgData.group[groupId].name, sizeof(pbVlanCfgData.group[groupId].name));
  pbVlanCfgData.group[groupId].inUse = L7_FALSE;
  pbVlanCfgData.group[groupId].groupID = L7_PBVLAN_NULL_GROUP;

  return rc;
}

/*********************************************************************
*
* @purpose  To add a protocol to a group.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  prtl     protocol to be added to the group

* @returns  L7_SUCCESS, if protocol is added to the group
* @returns  L7_FAILURE, if invalid groupid, protocol, or 
*                       unconfigured group
* @returns  L7_ALREADY_CONFIGURED, if this protocol already exists
*
* @notes    If A be a group to which 'prtl' is to be added, and there
*           are other groups (X = B,C,...) sharing ports of group A
*           and using 'prtl', then 'prtl' cannot be added to group A.
* @notes    In the sample implementation there are only three 
*           configurable protocols: IP, IPX, ARP.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupProtocolAdd(L7_uint32 groupId, L7_uint32 prtl, L7_uint32 ptype)
{
  L7_uint32 i;
  L7_uint32 index;
  L7_uint32 ports;
  L7_uint32 count1;
  L7_uint32 count2;
  L7_uint32 list1[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 list2[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 numPorts1;
  L7_uint32 numPorts2;
  L7_RC_t rc;
  L7_RC_t rc1;

  /* check validity of the group */
  if ( ((groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS) || 
       (groupId < L7_PBVLAN_MIN_NUM_GROUPS)) || (pbVlanCfgData.group[groupId].inUse == L7_FALSE) )
  {
    return L7_FAILURE;
  }

  rc = pbVlanGroupPrtlIndexGet(prtl, &index);
  
  if(rc == L7_FAILURE)
  {
     /* if index is not found that means its a new protocol, then add to the systemwide list */ 
     /* First add ethernet protocols to the table, if it is not in the list */ 
     if ((rc1 = pbVlanEtherProtocolAdd(prtl, ptype)) != L7_SUCCESS)
     {
       if(rc1 == L7_TABLE_IS_FULL)
       {
         return rc1;
       }
       else
       {
     return L7_ERROR;   
       }
     }

     rc = pbVlanGroupPrtlIndexGet(prtl, &index);
  }

  if (pbVlanCfgData.group[groupId].protocol[index] != L7_PBVLAN_INVALID_PROTOCOL)
  {
    return L7_ERROR;
  }

  else
  {
    memset(list1, 0x00, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
    rc = nimMaskToList(&pbVlanCfgData.group[groupId].ports, list1, &numPorts1);

    /* save value of 'numPorts1' for later...when calling 'dtl' */
    ports = numPorts1;

    /* if group does not have ports */
    if (numPorts1 == L7_NULL)
    {
      /* 'prtl' can be added to the database for this group */
      pbVlanCfgData.group[groupId].protocol[index] = prtl;
      pbVlanCfgData.group[groupId].type[index] = ptype;
      pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;
      return L7_SUCCESS;
    }

    /* if group has ports */
    else
    {
      /* check if its ports are already configured by another group */
      count1 = 0;
      while (numPorts1 > L7_NULL)
      {
        for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
        {
          if (i == (groupId))
          {
            continue;
          }

          if (pbVlanCfgData.group[i].inUse == L7_TRUE)
          {
            memset(list2, 0x00, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
            rc = nimMaskToList(&pbVlanCfgData.group[i].ports, list2, &numPorts2);

            count2 = 0;
            while (numPorts2 > L7_NULL)
            {
              if (list1[count1] == list2[count2++])
              {
                /* check if the port has 'prtl' */
                if (pbVlanCfgData.group[i].protocol[index] == prtl)
                {
                  return L7_ALREADY_CONFIGURED;
                }
                else
                {
                  break;
                }
              }
              else
              {
                numPorts2--;
              }
            }
          }
        } /* for */

        count1++;
        numPorts1--;
      } /* while numPorts1 */

      /* 'prtl' can be added to the database for this group */
      /* and also to all the ports of this group */
      pbVlanCfgData.group[groupId].protocol[index] = prtl;
      pbVlanCfgData.group[groupId].type[index] = ptype;
      pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;

      /* call the dtl function for each port */
      count1 = 0;
      while (ports > 0)
      {
        rc = dtlPbVlanPortConfig(pbVlanMaskOffsetToIntIfNum[list1[count1++]], 
                                 prtl, 
                                 pbVlanCfgData.group[groupId].vlanid,
                                 L7_TRUE);
        ports--;
      }

      return rc;
    }
  } /* else */
}

/*********************************************************************
*
* @purpose  To add a protocol to a system.
*
* @param    L7_uint32  prtl     protocol to be added to the system 

* @returns  L7_SUCCESS, if protocol is added to the group
* @returns  L7_FAILURE, if invalid  protocol, or space is not available
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanEtherProtocolAdd(L7_uint32 prtl, L7_uint32 ptype)
{
  L7_uint32 i = 0;

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if (pbVlanCfgData.etherProtocol[i] == L7_PBVLAN_INVALID_PROTOCOL)
    {
        if(dtlPbVlanProtocolConfig(prtl, i) == L7_SUCCESS)
        {
          pbVlanCfgData.etherProtocol[i] = prtl; 
          pbVlanCfgData.prototype[i] = ptype;
          pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;
          return L7_SUCCESS;
        }
        else
        {
          return L7_FAILURE;
        } 
    }
  }
 
  return L7_TABLE_IS_FULL;
}

/*********************************************************************
*
* @purpose  To delete a protocol from a group.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  prtl     protocol to be deleted from the group
*
* @returns  L7_SUCCESS, if protocol is deleted from the group
* @returns  L7_FAILURE, if invalid groupid, protocol, or
*                       unconfigured group
* @returns  L7_ERROR, if the group does not have this protocol
*
* @comments The protocol is deleted from all the ports in the group.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupProtocolDelete(L7_uint32 groupId, L7_uint32 prtl)
{
  L7_uint32 index;
  L7_uint32 i = 0;
  L7_uint32 list[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 numPorts;
  L7_RC_t rc;

  i=0;
  /* check validity of the group */
  if ( ((groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS) || 
       (groupId < L7_PBVLAN_MIN_NUM_GROUPS)) || (pbVlanCfgData.group[groupId].inUse == L7_FALSE) )
  {
    return L7_FAILURE;
  }

  rc = pbVlanGroupPrtlIndexGet(prtl, &index);
  if (rc != L7_SUCCESS || pbVlanCfgData.group[groupId].protocol[index] != prtl)
  {
    return L7_ERROR;
  }

  else
  {
    /* 'prtl' can be deleted from the database for this group */
    pbVlanCfgData.group[groupId].protocol[index] = L7_PBVLAN_INVALID_PROTOCOL;
    pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;

    memset(list, 0x00, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
    rc = nimMaskToList(&pbVlanCfgData.group[groupId].ports, list, &numPorts);

    /* if this group has ports */
    /* call the dtl function to delete 'prtl' from all its ports */
    while (numPorts > L7_NULL)
    {
      rc = dtlPbVlanPortConfig(pbVlanMaskOffsetToIntIfNum[list[i++]], 
                               prtl, 
                               pbVlanCfgData.group[groupId].vlanid, 
                               L7_FALSE);
      numPorts--;
    }

    pbVlanEtherProtocolDelete(groupId, prtl); 

    return rc;
  }
}

/*********************************************************************
*
* @purpose  To delete a protocol from a system.If the protol is used 
            by any other group it wont delete.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  prtl     protocol to be deleted from the group
*
* @returns  L7_SUCCESS, if protocol is deleted from the group
* @returns  L7_FAILURE, if invalid groupid, protocol, or
*                       unconfigured group
*
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanEtherProtocolDelete(L7_uint32 groupId, L7_uint32 prtl)
{

  L7_uint32 i = 0;
  L7_uint32 index;

 /* check validity of the group */
  if ( ((groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS) 
       ||(groupId < L7_PBVLAN_MIN_NUM_GROUPS)) || (pbVlanCfgData.group[groupId].inUse == L7_FALSE) )
  {
    return L7_FAILURE;
  }

  if(pbVlanGroupPrtlIndexGet(prtl, &index) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    if (i == (groupId))
    {
      continue;
    }

    if (pbVlanCfgData.group[i].inUse == L7_TRUE)
    {
       if (pbVlanCfgData.group[i].protocol[index] == prtl)
       {
          /* If any group use that protocol it should not delete that protocol from 
             the system */ 
          return L7_FAILURE;
       }
    }
  } 
  if(dtlPbVlanProtocolConfig(L7_PBVLAN_INVALID_PROTOCOL, index) == L7_SUCCESS)
  {
    pbVlanCfgData.etherProtocol[index] = L7_PBVLAN_INVALID_PROTOCOL;
    pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}


/*********************************************************************
*
* @purpose  To add a vlan id to a group.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  vid      vlan id of the group
*
* @returns  L7_SUCCESS, if vlan id is added to the group
* @returns  L7_FAILURE, if invalid groupid, vlanid, or unconfigured group
*
* @notes    Vlanid can be any number in the range 1-4094, 1 being
*           the default.
* @notes    Vlanid is added to all the ports in the group.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupVlanIDAdd(L7_uint32 groupId, L7_uint32 vid)
{
  L7_uint32 i;
  L7_uint32 j = 0;
  L7_uint32 list[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 numPorts;
  L7_RC_t rc;

  /* check validity of the group and vlanid */
  if ( ((groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS))  ||
       (pbVlanCfgData.group[groupId].inUse == L7_FALSE)  ||
       ((vid < L7_DOT1Q_MIN_VLAN_ID) || (vid > L7_DOT1Q_MAX_VLAN_ID)) )
  {
    return L7_FAILURE;
  }

  else
  {
    /* 'vid' can be added to the database for this group */
    pbVlanCfgData.group[groupId].vlanid = vid;
    pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;

    memset(list, 0x00, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
    rc = nimMaskToList(&pbVlanCfgData.group[groupId].ports, list, &numPorts);

    /* if this group has ports */
    while (numPorts > L7_NULL)
    {
      /* call the dtl function to add 'vid' to all its ports */
      for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
      {
        if (pbVlanCfgData.group[groupId].protocol[i] != L7_PBVLAN_INVALID_PROTOCOL)
        {
          rc = dtlPbVlanPortConfig(pbVlanMaskOffsetToIntIfNum[list[j]], 
                                   pbVlanCfgData.group[groupId].protocol[i], 
                                   pbVlanCfgData.group[groupId].vlanid,
                                   L7_TRUE);
        }
      }
      numPorts--;
      j++;
    }

    return rc;
  }
}

/*********************************************************************
*
* @purpose  To delete a vlan id from a group.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  vid      vlan id of the group
*
* @returns  L7_SUCCESS, if vlan id is deleted from the group
* @returns  L7_FAILURE, if invalid groupid, vlanid, or unconfigured group
* @returns  L7_ERROR, if the group does not have this vlan id
*
* @comments Vlanid is deleted from all the ports in the group.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupVlanIDDelete(L7_uint32 groupId, L7_uint32 vid)
{
  L7_uint32 i;
  L7_uint32 j = 0;
  L7_uint32 list[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 numPorts;
  L7_RC_t rc;

  /* check validity of the group and vlanid */
  if ( ((groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS))  ||
       (pbVlanCfgData.group[groupId].inUse == L7_FALSE)  ||
       ((vid < L7_DOT1Q_MIN_VLAN_ID) || (vid > L7_DOT1Q_MAX_VLAN_ID)) )
  {
    return L7_FAILURE;
  }

  else if (pbVlanCfgData.group[groupId].vlanid != vid)
  {
    return L7_ERROR;
  }

  else
  {
    /* 'vid' can be deleted from the database for this group */
    pbVlanCfgData.group[groupId].vlanid = L7_PBVLAN_NULL_VLANID;
    pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;

    memset(list, 0x00, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
    rc = nimMaskToList(&pbVlanCfgData.group[groupId].ports, list, &numPorts);

    /* if this group has ports */
    while (numPorts > L7_NULL)
    {
      /* call the dtl function to add 'vid' to all its ports */
      for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
      {
        if (pbVlanCfgData.group[groupId].protocol[i] != L7_PBVLAN_INVALID_PROTOCOL)
        {
          rc = dtlPbVlanPortConfig(pbVlanMaskOffsetToIntIfNum[list[j]], 
                                   pbVlanCfgData.group[groupId].protocol[i], 
                                   pbVlanCfgData.group[groupId].vlanid, 
                                   L7_FALSE);
        }
      }
      numPorts--;
      j++;
    }

    return rc;
  }
}

/*********************************************************************
*
* @purpose  To add a port to a group.
*
* @param    L7_uint32  groupId   group id of an existing group
* @param    L7_uint32  intIfNum  internal intf number to be added
*
* @returns  L7_SUCCESS, if the port is added to the group
* @returns  L7_FAILURE, if invalid groupid, port, or unconfigured group
* @returns  L7_ALREADY_CONFIGURED, if the port is already configured
*
* @notes    If A be a group to which 'intIfNum' is to be added, and
*           there are other groups (X = B,C,...) sharing the same 
*           port and using same protocols as group A, then 'intIfNum'
*           cannot be added to group A.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupPortAdd(L7_uint32 groupId, L7_uint32 intIfNum)
{
  L7_uint32 i;
  L7_uint32 j;
  L7_uint32 prot[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 maskOffset;

  if (nimConfigIdMaskOffsetGet(intIfNum,&maskOffset) != L7_SUCCESS)
    return L7_FAILURE;

  /* check validity of the group and port */
  if ( ((groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS))  ||
       (pbVlanCfgData.group[groupId].inUse == L7_FALSE)  ||
       (pbVlanIsValidIntf(intIfNum) == L7_FALSE) )
  {
    return L7_FAILURE;
  }

  else
  {
    /* check if 'intIfNum' is present in this group */
    if (NIM_INTF_ISMASKBITSET(pbVlanCfgData.group[groupId].ports, intIfNum) != L7_NULL)
    {
      return L7_SUCCESS;
    }

    /* continue with the adding procedure         */
    /* if 'intIfNum' is not present in this group */

    /* initializing prot[]. It will later be filled with the     */
    /* protocols that have already been configured on 'intIfNum' */
    for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
    {
      prot[j] = L7_PBVLAN_INVALID_PROTOCOL;
    }

    /* check if 'intIfNum' is already configured by another group */
    for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
    {
      if (i == (groupId))
      {
        continue;
      }

      if (pbVlanCfgData.group[i].inUse == L7_TRUE &&
          NIM_INTF_ISMASKBITSET(pbVlanCfgData.group[i].ports, intIfNum))
          {
            for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
            {
              if (pbVlanCfgData.group[i].protocol[j] != L7_PBVLAN_INVALID_PROTOCOL)
              {
                /* Collect all protocols configured for this interface
                   in all configured member groups */
                 prot[j] = pbVlanCfgData.group[i].protocol[j];
              }
            } /* for j */
      } /* if */
    } /* for */

    /* check prot[] to see if 'intIfNum' can be added for the remaining protocols */
    for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
    {
      if (prot[j] != L7_PBVLAN_INVALID_PROTOCOL)
      {
        if (pbVlanCfgData.group[groupId].protocol[j] == prot[j])
        {
          return L7_ALREADY_CONFIGURED;
        }
      }
    }

      NIM_INTF_SETMASKBIT(pbVlanCfgData.group[groupId].ports, maskOffset);
      pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;

      for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
      {
        if (pbVlanCfgData.group[groupId].protocol[i] != L7_PBVLAN_INVALID_PROTOCOL)
        {
          /* call the dtl function to add 'intIfNum' to this group */
          rc = dtlPbVlanPortConfig(intIfNum, 
                                   pbVlanCfgData.group[groupId].protocol[i], 
                                   pbVlanCfgData.group[groupId].vlanid,
                                   L7_TRUE);
        }
      }
  } /* else */

  return rc;
}

/*********************************************************************
*
* @purpose  To delete a port from a group.
*
* @param    L7_uint32  groupId   group id of an existing group
* @param    L7_uint32  intIfNum  internal intf number to be deleted
*
* @returns  L7_SUCCESS, if port is deleted from the group
* @returns  L7_FAILURE, if invalid groupid, port, or unconfigured group
* @returns  L7_ERROR, if no ports in the group or the specified port
*                     is not in use
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupPortDelete(L7_uint32 groupId, L7_uint32 intIfNum)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  /* check validity of the group and port */
  if ( ((groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS))  ||
       (pbVlanCfgData.group[groupId].inUse == L7_FALSE)  ||
       (pbVlanIsValidIntf(intIfNum) == L7_FALSE) )
  {
    return L7_FAILURE;
  }

  else
  {
    /* check if 'intIfNum' is in use */
    if (NIM_INTF_ISMASKBITSET(pbVlanCfgData.group[groupId].ports, intIfNum) == L7_NULL)
    {
      return L7_ERROR;
    }

    /* if in use, delete 'intIfNum' from the database for this group */
    else
    {
      /* CLRMASKBIT turns off bit index # intIfNum in mask ports */
      NIM_INTF_CLRMASKBIT(pbVlanCfgData.group[groupId].ports, intIfNum);
      rc = L7_SUCCESS;
      pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;

      /* call the dtl function to delete 'intIfNum' from this group */
      for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
      {
        if (pbVlanCfgData.group[groupId].protocol[i] != L7_PBVLAN_INVALID_PROTOCOL)
        {
          rc = dtlPbVlanPortConfig(intIfNum, 
                                   pbVlanCfgData.group[groupId].protocol[i], 
                                   pbVlanCfgData.group[groupId].vlanid,
                                   L7_FALSE);
        }
      }
    }

  } /* else */

  return rc;
}

/*********************************************************************
*
* @purpose  Given a group id, get the id of the next created group.
*
* @param    L7_uint32  groupId  @b((input)) group id, start
* @param    L7_uint32  *next    @b((input)) group id, next
*
* @returns  L7_SUCCESS, if the next created group is found
* @returns  L7_FAILURE, if invalid groupid or next group is not found
*
* @comments Using 'groupId' as reference, the function gets the id of
*           the next created group. The 'groupId' is zero (0) when
*           trying to find the very first created group.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupGetNext(L7_uint32 groupId, L7_uint32 *next)
{
  L7_uint32 i;

  /* check validity of the group */
  if (groupId > L7_PBVLAN_MAX_NUM_GROUPS)
  {
    return L7_FAILURE;
  }

  else
  {
    for (i = groupId; i < L7_PBVLAN_MAX_NUM_GROUPS; i++)
    {
      if (pbVlanCfgData.group[i + 1].inUse == L7_TRUE)
      {
        /* (i + 1) is the groupId of the next created group */
        *next = i + 1;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Given a group id, get the id of the next created group.
*
* @param    L7_uint32  groupId  @b((input)) group id, start
* @param    L7_uint32  *next    @b((input)) group id, next
*
* @returns  L7_SUCCESS, if the next created group is found
* @returns  L7_FAILURE, if invalid groupid or next group is not found
*
* @comments Using 'groupId' as reference, the function gets the id of
*           the next created group. The 'groupId' is zero (0) when
*           trying to find the very first created group.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupGetExact(L7_uint32 groupId)
{
  /* check validity of the group */
  if ( (groupId == L7_PBVLAN_NULL_GROUP) || (groupId > L7_PBVLAN_MAX_NUM_GROUPS) )
  {
    return L7_FAILURE;
  }

  else
  {
    if (pbVlanCfgData.group[groupId].inUse == L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  To set a group name.
*
* @param    L7_uint32 groupId ID of the group
* @param    L7_char8  *Name  name of the group
*
* @returns  L7_SUCCESS, if the group name is added
* @returns  L7_FAILURE, if the group name adding added
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupNameSet(L7_uint32 groupId, L7_char8 *Name)
{

  L7_RC_t rc;

  rc = pbVlanGroupGetExact(groupId);

  if(rc == L7_SUCCESS)
  {
    if (strlen(Name) > L7_PBVLAN_MAX_GROUP_NAME)
    {
      return L7_FAILURE;
    }
    bzero(pbVlanCfgData.group[groupId].name, sizeof(pbVlanCfgData.group[groupId].name));
    osapiStrncpy(pbVlanCfgData.group[groupId].name, Name, L7_PBVLAN_MAX_GROUP_NAME);
    pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get the name of the group identified by groupId.
*
* @param    L7_uint32  groupId  @b((input)) group id
* @param    L7_char8   *buf     @b{(Output)) buffer to be filled in
*                               with the name of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupNameGet(L7_uint32 groupId, L7_char8 *buf)
{
  L7_RC_t rc;

  rc = pbVlanGroupGetExact(groupId);

  if (rc == L7_SUCCESS)
  {
    osapiStrncpy(buf, pbVlanCfgData.group[groupId].name, L7_PBVLAN_MAX_GROUP_NAME);
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get the groupID by giving group name already exists.
*
* @param    L7_char8  *Name  name of the group
* @param    L7_uint32 *groupId  @b((output)) group id
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupIDGet (L7_char8 *Name, L7_uint32 *groupId)
{
  L7_uint32 i;

  /* check if the group name already exists */
  for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    if ( (pbVlanCfgData.group[i].inUse == L7_TRUE)  &&  (strcmp(pbVlanCfgData.group[i].name, Name)) == 0)
    {
      *groupId = i; 
      return L7_SUCCESS; 
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  To get the protocol(s) of the group identified by groupId.
*
* @param    L7_uint32  groupId  @b((input)) group id
* @param    L7_uint32  *prtl    @b{(Output)) protocol array containing
*                               the protocol(s) of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments A group can have up to three configurable protocols:
*           IP, ARP, and IPX.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupProtocolGet(L7_uint32 groupId, L7_uint32 *prtl, L7_uint32 *ptype)
{
  L7_uint32 i;
  L7_RC_t rc;

  rc = pbVlanGroupGetExact(groupId);

  if (rc == L7_SUCCESS)
  {
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      *(prtl + i) = pbVlanCfgData.group[groupId].protocol[i];
      *(ptype + i) = pbVlanCfgData.group[groupId].type[i];
    }
    rc = L7_SUCCESS;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To get the VLAN ID of the group identified by groupId.
*
* @param    L7_uint32  groupId  @b((input)) group id
* @param    L7_uint32  *vid     @b{(Output)) the vlan id of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupVIDGet(L7_uint32 groupId, L7_uint32 *vid)
{
  L7_RC_t rc;

  rc = pbVlanGroupGetExact(groupId);

  if (rc == L7_SUCCESS)
  {
    *vid = pbVlanCfgData.group[groupId].vlanid;
    rc = L7_SUCCESS;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To get the port(s) of the group identified by groupId.
*
* @param    L7_uint32  groupId     @b((input)) group id
* @param    L7_uint32  intIfNum    @b{(input)) group port, start
* @param    L7_uint32  *nextIfNum  @b{(Output)) group port, next
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid, group does not exist, or
*                       no more ports in this group      
*
* @comments The value of 'intIfNum' will be zero (0) when trying
*           to find the very first port of the group.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupPortGetNext(L7_uint32 groupId, L7_uint32 intIfNum, L7_uint32 *nextIfNum)
{
  L7_uint32 i;
  L7_uint32 list[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 numPorts;
  L7_RC_t rc = L7_FAILURE;

  rc = pbVlanGroupGetExact(groupId);
  if (rc == L7_SUCCESS)
  {
    memset(list, 0x00, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
    rc = nimMaskToList(&pbVlanCfgData.group[groupId].ports, list, &numPorts);

    if (numPorts > L7_NULL)
    {
      /* get the next port of this group, using 'intIfNum' as reference */
      i = 0;
      while (numPorts > L7_NULL)
      {
        if (intIfNum < list[i])
        {
          *nextIfNum = list[i];
          return L7_SUCCESS;
        }
        i++;
        numPorts--;
      }
    }
  }

  *nextIfNum = L7_NULL;
  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  To check if the specified port exists in the group.
*
* @param    L7_uint32  groupId   @b((input)) specified group's id
* @param    L7_uint32  intIfNum  @b{(input)) group port
*
* @returns  L7_SUCCESS, if the specified port exists
* @returns  L7_FAILURE, if invalid groupid, intIfNum, or 
*                       the specified group or port does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupPortGetExact(L7_uint32 groupId, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  if (intIfNum == L7_NULL)
  {
    return L7_FAILURE;
  }

  rc = pbVlanGroupGetExact(groupId);
  if (rc == L7_SUCCESS)
  {
    /* check if 'intIfNum' is in use by this group */
    if (NIM_INTF_ISMASKBITSET(pbVlanCfgData.group[groupId].ports, intIfNum) == L7_NULL)
    {
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get the next protocol of the group identified by groupId.
*
* @param    L7_uint32  groupId     @b((input)) group id
* @param    L7_uint32  proto       @b{(input)) group proto, start
* @param    L7_uint32  *nextProto  @b{(Output))group proto, next
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid, group does not exist, or
*                       no more protocol in this group
*
* @comments The value of 'proto' will be zero (0) when trying
*           to find the very first protocol of the group.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupProtocolGetNext(L7_uint32 groupId, L7_uint32 proto, L7_uint32 *nextProto)
{
  
  L7_RC_t rc = L7_FAILURE;

  if(proto == L7_NULL)
  {
    /*check groupid is valid or not */
    rc = pbVlanGroupGetExact(groupId);
    
    if(rc == L7_SUCCESS)
    {
      /*Search for valid protocol from the minimum ethertype */
      proto = L7_PBVLAN_ETYPE_MIN;
    } 
  }
  else
  {
    /*check group exist or not*/
    rc = pbVlanGroupGetExact(groupId);

    if(rc == L7_SUCCESS)
    {
      if((proto < L7_PBVLAN_ETYPE_MIN) || (proto > L7_PBVLAN_ETYPE_MAX)) 
      { 
        return L7_FAILURE;  
      }
      else
      {
        proto++;
      }
    }
  }

  if (rc == L7_SUCCESS)
  {
    for (; proto <= L7_PBVLAN_ETYPE_MAX; proto++)
    {
      if(pbVlanGroupProtocolGetExact(groupId, proto) == L7_SUCCESS)
      {
        *nextProto = proto;
        return L7_SUCCESS; 
      }
    }  
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  To check if the specified port exists in the group.
*
* @param    L7_uint32  groupId   @b((input)) specified group's id
* @param    L7_uint32  proto     @b{(input)) group proto
*
* @returns  L7_SUCCESS, if the specified protocol exists
* @returns  L7_FAILURE, if invalid groupid, proto, or
*                       the specified group or protocol does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupProtocolGetExact(L7_uint32 groupId, L7_uint32 proto)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 index;

  if (proto == L7_NULL)
  {
    return L7_FAILURE;
  }

  rc = pbVlanGroupGetExact(groupId);

  if (rc == L7_SUCCESS)
  {
    rc = pbVlanGroupPrtlIndexGet(proto, &index);

    if (rc == L7_SUCCESS)
    { 
      if (pbVlanCfgData.group[groupId].protocol[index] != proto)
      {
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Determine if pbVlan is interested in the given interface
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL pbVlanIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  if ((sysIntfType == L7_PHYSICAL_INTF) || (sysIntfType == L7_LAG_INTF))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}
