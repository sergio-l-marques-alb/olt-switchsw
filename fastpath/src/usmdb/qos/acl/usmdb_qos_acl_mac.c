/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    usmdb_qos_acl_mac.c
* @purpose     usmdb MAC access list API functions
* @component   acl
* @comments    none
* @create      08/29/2004
* @author      gpaussa
* @end
*             
**********************************************************************/


#include <string.h>
#include "l7_common.h"
#include "nimapi.h"
#include "usmdb_qos_acl_api.h"
#include "usmdb_util_api.h"
#include "acl_api.h"


/********************************************/
/*             MAC ACL API                  */
/********************************************/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new MAC ACL in the access list table
*
* @param    UnitIndex     @b{(input)} the unit for this operation
* @param    *pIndex       @b{(output)} Pointer to MAC ACL index value
*
* @returns  L7_SUCCESS
* @returns  L7_TABLE_IS_FULL  ACL table is currently full
* @returns  L7_FAILURE
*
* @comments The index value remains free until used in a subsequent ACL create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new ACL
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, so they must call this
*           function again to get a new value.
*
* @comments Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosAclMacIndexNext(L7_uint32 UnitIndex, L7_uint32 *pIndex)
{
  return aclMacIndexNext(pIndex);
}

/*************************************************************************
* @purpose  Retrieve the min/max index values allowed for the MAC ACL table 
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosAclMacIndexMinMaxGet(L7_uint32 UnitIndex, L7_uint32 *pMin, L7_uint32 *pMax)
{
  return aclMacIndexMinMaxGet(pMin, pMax);
}

/*********************************************************************
*
* @purpose  To create a new MAC access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS, if the access-list is added
* @returns  L7_FAILURE, if invalid identifying index
* @returns  L7_ERROR, if the access-list already exists
* @returns  L7_REQUEST_DENIED, if the access-list config change is not approved
* @returns  L7_TABLE_IS_FULL, if maximum number of ACLs already created
*
* @comments The MAC ACL name must be set after the access-list is created.
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacCreate(L7_uint32 UnitIndex, L7_uint32 aclIndex)
{
  return aclMacCreate(aclIndex);
}

/*********************************************************************
*
* @purpose  To delete an existing MAC access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS, if the access-list is deleted
* @returns  L7_FAILURE, if invalid identifying index
* @returns  L7_ERROR, if the access-list does not exist
* @returns  L7_REQUEST_DENIED, if the access-list config change is not approved
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacDelete(L7_uint32 UnitIndex, L7_uint32 aclIndex)
{
  return aclMacDelete(aclIndex, L7_FALSE);
}

/*********************************************************************
*
* @purpose  To add the name to this MAC access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  name     the access-list name
*                                            
* @returns  L7_SUCCESS, if name is added
* @returns  L7_FAILURE, if invalid name or other failure
* @returns  L7_ERROR, if the access-list does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacNameAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uchar8 *name)
{
  return aclMacNameAdd(aclIndex,name);
}

/*********************************************************************
*
* @purpose  To change the name of an existing MAC access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  newname  the new access-list name
*                                            
* @returns  L7_SUCCESS, if name is added
* @returns  L7_FAILURE, if invalid name or other failure
* @returns  L7_ERROR, if the access-list does not exist
* @returns  L7_ALREADY_CONFIGURED, if the name is in use by another ACL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacNameChange(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uchar8 *newname)
{
  return aclMacNameChange(aclIndex,newname);
}

/*********************************************************************
*
* @purpose  To get the name of this MAC access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  name     the access-list name
*                                            
* @returns  L7_SUCCESS, if MAC access list name is retrieved
* @returns  L7_FAILURE, if invalid name or access list index
* @returns  L7_ERROR, if the access-list does not exist
*
* @comments Caller must provide a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacNameGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uchar8 *name)
{
  return aclMacNameGet(aclIndex,name);
}

/*********************************************************************
*
* @purpose  Checks if the MAC ACL name is a valid string
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uchar8  *name      the MAC access-list name
*
* @returns  L7_SUCCESS, if the MAC access list name is valid
* @returns  L7_FAILURE, if the MAC access list name is invalid
*
* @comments This function only checks the name string syntax for a MAC ACL. 
*           It does not check if an MAC ACL currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacNameStringCheck(L7_uint32 UnitIndex, L7_uchar8 *name)
{
  return aclMacNameStringCheck(name);
}

/*********************************************************************
*
* @purpose  To get the index of a MAC access list, given its name.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  *name      the access-list name
* @param    L7_uint32  *aclIndex  the access-list identifying index
*                                            
* @returns  L7_SUCCESS, if MAC access list index is retrieved
* @returns  L7_FAILURE, if invalid parms, or other failure
* @returns  L7_ERROR,   if access list name does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacNameToIndex(L7_uint32 UnitIndex, L7_uchar8 *name, L7_uint32 *aclIndex)
{
  return aclMacNameToIndex(name,aclIndex);
}

/*********************************************************************
*
* @purpose  To add an action to a MAC access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  action     action type (permit/deny)
*
* @returns  L7_SUCCESS, if the rule action is added
* @returns  L7_FAILURE, if invalid identifying index
* @returns  L7_ERROR, if the access-list does not exist
* @returns  L7_HARDWARE_ERROR, if hardware update failed
* @returns  L7_REQUEST_DENIED, fi access list config change is not approved
* @returns  L7_TABLE_IS_FULL, if storage unavailable for rule
*
* @comments Creates a new rule if the rulenum does not exist.
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleActionAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                    L7_uint32 rulenum, L7_uint32 action)
{
  /* this API is used for creating a rule as well as changing the action
   * of an existing rule
   */
  return aclMacRuleActionAdd(aclIndex,rulenum,action);
}

/*********************************************************************
*
* @purpose  To remove a rule from an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
*                                            
* @returns  L7_SUCCESS, if the rule number is removed
* @returns  L7_FAILURE, if invalid access-list index or rule number
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_HARDWARE_ERROR, if hardware update failed
* @returns  L7_REQUEST_DENIED, if access list config change is not approved
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleRemove(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum)
{
  return aclMacRuleRemove(aclIndex,rulenum);
}

/*********************************************************************
*
* @purpose  To set the assigned queue id for a MAC access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  queueId  the queue id value
*                                            
* @returns  L7_SUCCESS, if assigned queue id is added
* @returns  L7_FAILURE, if invalid queue id or other failure
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleAssignQueueIdAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                           L7_uint32 rulenum, L7_uint32 queueId)
{
  return aclMacRuleAssignQueueIdAdd(aclIndex,rulenum,queueId);
}

/*********************************************************************
*
* @purpose  To set the redirect interface for a MAC access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  intIfNum the redirect internal interface number
*                                            
* @returns  L7_SUCCESS, if redirect interface is added
* @returns  L7_FAILURE, if invalid redirect interface or other failure
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleRedirectIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                          L7_uint32 rulenum, L7_uint32 intIfNum)
{
  nimConfigID_t configId;

  if ((intIfNum < L7_USMDB_ACL_REDIRECT_INTF_MIN) || 
      (intIfNum > L7_USMDB_ACL_REDIRECT_INTF_MAX))
    return L7_FAILURE;

  /* translate intIfNum to configId for use within application
   * (cannot use intIfNum directly, since this may not persist across a reset)
   */
  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  return aclMacRuleRedirectConfigIdAdd(aclIndex,rulenum,&configId);
}

/*********************************************************************
*
* @purpose  To set the mirror interface for a MAC access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  intIfNum the mirror internal interface number
*                                            
* @returns  L7_SUCCESS, if mirror interface is added
* @returns  L7_FAILURE, if invalid mirror interface or other failure
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleMirrorIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                        L7_uint32 rulenum, L7_uint32 intIfNum)
{
  nimConfigID_t configId;

  if ((intIfNum < L7_USMDB_ACL_MIRROR_INTF_MIN) || 
      (intIfNum > L7_USMDB_ACL_MIRROR_INTF_MAX))
    return L7_FAILURE;

  /* translate intIfNum to configId for use within application
   * (cannot use intIfNum directly, since this may not persist across a reset)
   */
  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  return aclMacRuleMirrorConfigIdAdd(aclIndex,rulenum,&configId);
}

/*********************************************************************
*
* @purpose  To set the logging flag for a MAC access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  logFlag  the rule logging flag value
*                                            
* @returns  L7_SUCCESS, if logging flag is added
* @returns  L7_FAILURE, if invalid logging flag or other failure
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleLoggingAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                     L7_uint32 rulenum, L7_BOOL logFlag)
{
  return aclMacRuleLoggingAdd(aclIndex,rulenum,logFlag);
}

/*********************************************************************
*
* @purpose  To add the class of service (cos) value.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index 
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  cos      the class of service (cos) value
*
* @returns  L7_SUCCESS, if the cos value was added
* @returns  L7_FAILURE, if invalid list index, rulenum, or cos value
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleCosAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                 L7_uint32 rulenum, L7_uint32 cos)
{
  return aclMacRuleCosAdd(aclIndex,rulenum,cos);
}

/*********************************************************************
*
* @purpose  To add the secondary class of service (cos2) value.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  cos2     the secondary class of service (cos2) value
*
* @returns  L7_SUCCESS, if the cos2 value was added
* @returns  L7_FAILURE, if invalid list index, rulenum, or cos2 value
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleCos2Add(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                  L7_uint32 rulenum, L7_uint32 cos2)
{
  return aclMacRuleCos2Add(aclIndex,rulenum,cos2);
}

/*********************************************************************
*
* @purpose  To add the destination MAC address and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uchar8  *dstmac  the destination MAC address
* @param    L7_uchar8  *dstmask the destination MAC mask
*
* @returns  L7_SUCCESS, if the dst MAC address and mask are added
* @returns  L7_FAILURE, if invalid list index, rulenum, dst addr or mask
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleDstMacAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                    L7_uint32 rulenum,
                                    L7_uchar8 *dstmac, L7_uchar8 *dstmask)
{
  return aclMacRuleDstMacAdd(aclIndex,rulenum,dstmac,dstmask);
}

/*********************************************************************
*
* @purpose  To add the Ethertype keyword identifier.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_QOS_ETYPE_KEYID_t  keyid    the Ethertype keyword identifier
* @param    L7_uint32  value    the custom Ethertype value
*
* @returns  L7_SUCCESS, if the Ethertype keyword id and value is added
* @returns  L7_FAILURE, if invalid list index, rulenum, keyword id
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments All Ethertype match rules have a keyword.  A keyword of    
*           'custom' requires the Ethertype value to be set as well.
*           The value parameter is only meaningful when keyid is set to 
*           L7_QOS_ETYPE_KEYID_CUSTOM.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleEtypeKeyAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                      L7_uint32 rulenum, L7_QOS_ETYPE_KEYID_t keyid,
                                      L7_uint32 value)
{
  return aclMacRuleEtypeKeyAdd(aclIndex,rulenum,keyid,value);
}

/*********************************************************************
*
* @purpose  Set the match condition whereby all packets match for a
*           MAC ACL rule
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_BOOL    every    the match condition (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS, if the match every condition was added
* @returns  L7_FAILURE, if invalid list index or rule number
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleEveryAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                   L7_uint32 rulenum, L7_BOOL every)
{
  return aclMacRuleEveryAdd(aclIndex, rulenum, every);
}

/*********************************************************************
*
* @purpose  To add the source MAC address and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uchar8  *srcmac  the source MAC address
* @param    L7_uchar8  *srcmask the source MAC mask
*
* @returns  L7_SUCCESS, if the src MAC address and mask are added
* @returns  L7_FAILURE, if invalid list index, rulenum, src addr or mask
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleSrcMacAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                    L7_uint32 rulenum, 
                                    L7_uchar8 *srcmac, L7_uchar8 *srcmask)
{
  return aclMacRuleSrcMacAdd(aclIndex,rulenum,srcmac,srcmask);
}

/*********************************************************************
*
* @purpose  To add a single VLAN ID.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  vlan       the VLAN id 
*
* @returns  L7_SUCCESS, if the VLAN id is added
* @returns  L7_FAILURE, if invalid list index, rulenum, or VLAN id
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleVlanIdAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                    L7_uint32 rulenum, L7_uint32 vlan)
{
  return aclMacRuleVlanIdAdd(aclIndex,rulenum,vlan);
}

/*********************************************************************
*
* @purpose  To add a VLAN ID range.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  startvlan  the start VLAN id
* @param    L7_uint32  endvlan    the end VLAN id
*
* @returns  L7_SUCCESS, if the VLAN id range is added
* @returns  L7_FAILURE, if invalid list index, rulenum, or VLAN id
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleVlanIdRangeAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                         L7_uint32 rulenum, 
                                         L7_uint32 startvlan, L7_uint32 endvlan)
{
  return aclMacRuleVlanIdRangeAdd(aclIndex,rulenum,startvlan,endvlan);
}

/*********************************************************************
*
* @purpose  To add a single Secondary VLAN ID (vlan2).
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  vlan2      the Secondary VLAN id 
*
* @returns  L7_SUCCESS, if the vlan2 id is added
* @returns  L7_FAILURE, if invalid list index, rulenum, or vlan2 id
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleVlanId2Add(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                     L7_uint32 rulenum, L7_uint32 vlan2)
{
  return aclMacRuleVlanId2Add(aclIndex,rulenum,vlan2);
}

/*********************************************************************
*
* @purpose  To add a Secondary VLAN ID (vlan2) range.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  startvlan2 the start Secondary VLAN id
* @param    L7_uint32  endvlan2   the end Secondary VLAN id
*
* @returns  L7_SUCCESS, if the Secondary VLAN id range is added
* @returns  L7_FAILURE, if invalid list index, rulenum, or Secondary VLAN id
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleVlanId2RangeAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                          L7_uint32 rulenum, 
                                          L7_uint32 startvlan2, L7_uint32 endvlan2)
{
  return aclMacRuleVlanId2RangeAdd(aclIndex,rulenum,startvlan2,endvlan2);
}

/*********************************************************************
*
* @purpose  To add a MAC access list to the specified interface and direction.
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
* @param    direction   @b{(input)} interface direction to apply ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  intf, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacInterfaceDirectionAdd(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                            L7_uint32 direction, L7_uint32 aclIndex,
                                            L7_uint32 seqNum)
{
  return aclMacInterfaceDirectionAdd(intIfNum,direction,aclIndex,seqNum);
}

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified interface and direction.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS  intIfNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or interface does not exist
* @returns  L7_ERROR    ACL does not exist for this index
* @returns  L7_REQUEST_DENIED     error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacInterfaceDirectionRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                               L7_uint32 direction, L7_uint32 aclIndex)
{
  return aclMacInterfaceDirectionRemove(intIfNum,direction,aclIndex);
}

/*********************************************************************
*
* @purpose  To add a MAC access list to the specified vlan and direction.
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    vlanNum     @b{(input)} internal vlan number
* @param    direction   @b{(input)} vlan direction to apply ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  vlan, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for vlan, dir
* @returns  L7_REQUEST_DENIED error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacVlanDirectionAdd( L7_uint32 UnitIndex,
                                        L7_uint32 vlanNum,
                                        L7_uint32 direction,
                                        L7_uint32 aclIndex,
                                        L7_uint32 seqNum )
{
  return( aclMacVlanDirectionAdd( vlanNum, direction, aclIndex, seqNum ) );
}

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified vlan and direction.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS  vlanNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or vlan does not exist
* @returns  L7_ERROR    ACL does not exist for this index
* @returns  L7_REQUEST_DENIED     error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacVlanDirectionRemove( L7_uint32 UnitIndex,
                                           L7_uint32 vlanNum,
                                           L7_uint32 direction,
                                           L7_uint32 aclIndex )
{
  return( aclMacVlanDirectionRemove( vlanNum, direction, aclIndex ) );
}

/*********************************************************************
*
* @purpose  Get the first MAC access list index created in the system.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  *aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS, if the first MAC access list ID was found
* @returns  L7_FAILURE, 
* @returns  L7_ERROR,   if no MAC access lists have been created yet
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacIndexGetFirst(L7_uint32 UnitIndex, L7_uint32 *aclIndex)
{
  return aclMacIndexGetFirst(aclIndex);
}

/*********************************************************************
*
* @purpose  Given aclIndex, get the next MAC access list index
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the current MAC access-list index
* @param    L7_uint32  *next      the next MAC access-list index
*
* @returns  L7_SUCCESS, if the next MAC access list index was found
* @returns  L7_FAILURE, if aclIndex is the last MAC ACL index created
* @returns  L7_ERROR,   if aclIndex does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacIndexGetNext(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 *next)
{
  return aclMacIndexGetNext(aclIndex,next);
}

/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured MAC access list
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS, if the MAC access list was found
* @returns  L7_FAILURE, if the MAC access list does not exist 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacIndexCheckValid(L7_uint32 UnitIndex, L7_uint32 aclIndex)
{
  return aclMacIndexCheckValid(aclIndex);
}

/*********************************************************************
*
* @purpose  Checks if the MAC ACL index is within proper range
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS, if the MAC access list index is in range
* @returns  L7_FAILURE, if the MAC access list index is out of range
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacIndexRangeCheck(L7_uint32 UnitIndex, L7_uint32 aclIndex)
{
  return aclMacIndexRangeCheck(aclIndex);
}

/*********************************************************************
*
* @purpose  Get the first access list rule given a MAC ACL index
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  *rulenum   the next rulenum
*
* @returns  L7_SUCCESS, if the first rule for this MAC ACL index was found
* @returns  L7_FAILURE, 
* @returns  L7_ERROR,   if no rules have been created for this MAC ACL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleGetFirst(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 *rulenum)
{
  return aclMacRuleGetFirst(aclIndex,rulenum);
}

/*********************************************************************
*
* @purpose  Get the next access list rule given a MAC ACL index
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list index
* @param    L7_uint32  rulenum    the current rulenum
* @param    L7_uint32  *next      the next rulenum
*
* @returns  L7_SUCCESS, if the next rulenum was found
* @returns  L7_FAILURE, if rule is the last valid rulenum for this MAC ACL
* @returns  L7_ERROR,   if aclIndex or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleGetNext(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                  L7_uint32 rulenum, L7_uint32 *next)
{
  return aclMacRuleGetNext(aclIndex,rulenum,next);
}

/*********************************************************************
*
* @purpose  Checks if rulenum is valid, configured for this MAC access list
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex    the access-list identifying index
* @param    L7_uint32  rulenum    the current rulenum
*
* @returns  L7_SUCCESS, if the rule is valid
* @returns  L7_ERROR,   if the rule is not valid 
* @returns  L7_FAILURE, if the access-list does not exist, or other failures 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleCheckValid(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum)
{
  return aclMacRuleCheckValid(aclIndex,rulenum);
}

/*********************************************************************
*
* @purpose  Checks if the MAC ACL rule number is within proper range
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32   aclIndex   the access-list identifying index
* @param    L7_uint32   rulenum    the current rulenum
*
* @returns  L7_SUCCESS, if the rulenum is in range
* @returns  L7_FAILURE, if the rulenum is out of range
*
* @comments The aclIndex parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleNumRangeCheck(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum)
{
  return aclMacRuleNumRangeCheck(aclIndex,rulenum);
}

/*********************************************************************
*
* @purpose  Checks if aclIndex is currently assigned to this intf,dir pair
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  intf       the interface number
* @param    L7_uint32  direction  the interface direction
*
* @returns  L7_SUCCESS, if the MAC ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the MAC ACL is not assigned to this intf,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosMacAclIntfDirCheckValid(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                        L7_uint32 intf, L7_uint32 direction)
{
  return aclMacIntfDirCheckValid(aclIndex, intf, direction);
}

/*********************************************************************
*
* @purpose  Checks if aclIndex is currently assigned to this vlan,dir pair
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  vlan       the VLAN number
* @param    L7_uint32  direction  the VLAN direction
*
* @returns  L7_SUCCESS, if the MAC ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the MAC ACL is not assigned to this intf,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosMacAclVlanDirCheckValid( L7_uint32 UnitIndex,
                                         L7_uint32 aclIndex,
                                         L7_uint32 vlan,
                                         L7_uint32 direction )
{
  return( aclMacVlanDirCheckValid( aclIndex, vlan, direction ) );
}

/*********************************************************************
*
* @purpose  Checks to see if a user has configured a field in a rule
*           in a MAC access list
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex  the access-list identifying index
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  field     ruleFields_t enum 
*
* @returns  L7_TRUE, if the field has been configured
* @returns  L7_FALSE, if the field has not been configured
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL usmDbQosAclMacIsFieldConfigured(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                        L7_uint32 rule, L7_uint32 field)
{
  return aclMacIsFieldConfigured(aclIndex,rule,field);
}

/*********************************************************************
*
* @purpose  Gets a list of interfaces a MAC ACL is associated with for the
*           specified direction
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex    the access-list identifying index
* @param    L7_uint32  direction   the direction of application of ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  *intfList   list of interfaces (L7_ACL_INTF_MAX_COUNT long)
*
* @returns  L7_SUCCESS, if a list of interfaces was built
* @returns  L7_FAILURE, if invalid identifying number or 
*                       internal interface number does not exist
* @returns  L7_ERROR,   if access-list not assigned to any interface
*                       in this direction (no interface list was built)
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacAssignedIntfDirListGet(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                             L7_uint32 direction,
                                             L7_ACL_ASSIGNED_INTF_LIST_t *intfList)
{
  return aclMacAssignedIntfDirListGet(aclIndex,direction,intfList);
}

/*********************************************************************
*
* @purpose  Gets a list of vlan a MAC ACL is associated with for the
*           specified direction
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex    the access-list identifying index
* @param    L7_uint32  direction   the direction of application of ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  *vlanList   list of vlans (L7_ACL_VLAN_MAX_COUNT long)
*
* @returns  L7_SUCCESS, if a list of vlans was built
* @returns  L7_FAILURE, if invalid identifying number or 
*                       internal vlan number does not exist
* @returns  L7_ERROR,   if access-list not assigned to any vlan
*                       in this direction (no vlan list was built)
*
* @comments An ACL can be shared/associated with multiple vlans.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacAssignedVlanDirListGet( L7_uint32 UnitIndex,
                                              L7_uint32 aclIndex,
                                              L7_uint32 direction,
                                              L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList )
{
  return( aclMacAssignedVlanDirListGet( aclIndex, direction, vlanList ) );
}

/*********************************************************************
*
* @purpose  To get an action for a MAC access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *action    action type (permit/deny)
*
* @returns  L7_SUCCESS, if the rule action is retrieved
* @returns  L7_FAILURE, if rule action is not set
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleActionGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                    L7_uint32 rulenum, L7_uint32 *action)
{
  return aclMacRuleActionGet(aclIndex,rulenum,action);
}

/*********************************************************************
*
* @purpose  To get the assigned queue id from an existing MAC access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *queueId   the queue id         
*
* @returns  L7_SUCCESS, if the queue id is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleAssignQueueIdGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                           L7_uint32 rulenum, L7_uint32 *queueId)
{
  return aclMacRuleAssignQueueIdGet(aclIndex,rulenum,queueId);
}

/*********************************************************************
*
* @purpose  To get the redirect interface from an existing access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *intIfNum  the redirect internal interface number
*
* @returns  L7_SUCCESS, if the interface number is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleRedirectIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                          L7_uint32 rulenum, L7_uint32 *intIfNum)
{
  L7_RC_t       rc;
  nimConfigID_t configId;

  if (intIfNum == L7_NULLPTR)
    return L7_FAILURE;

  rc = aclMacRuleRedirectConfigIdGet(aclIndex,rulenum,&configId);
  if (rc == L7_SUCCESS)
  {
    /* translate configId to intIfNum for use by caller */
    if (nimIntIfFromConfigIDGet(&configId, intIfNum) != L7_SUCCESS)
      rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get the mirror interface from an existing access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *intIfNum  the mirror internal interface number
*
* @returns  L7_SUCCESS, if the interface number is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleMirrorIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                        L7_uint32 rulenum, L7_uint32 *intIfNum)
{
  L7_RC_t       rc = L7_FAILURE;
  nimConfigID_t configId;

  if (intIfNum == L7_NULLPTR)
    return L7_FAILURE;

  rc = aclMacRuleMirrorConfigIdGet(aclIndex,rulenum,&configId);
  if (rc == L7_SUCCESS)
  {
    /* translate configId to intIfNum for use by caller */
    if (nimIntIfFromConfigIDGet(&configId, intIfNum) != L7_SUCCESS)
      rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get the logging flag from an existing MAC access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *logFlag   the rule logging flag value
*
* @returns  L7_SUCCESS, if the logging flag is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleLoggingGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                     L7_uint32 rulenum, L7_BOOL *logFlag)
{
  return aclMacRuleLoggingGet(aclIndex,rulenum,logFlag);
}

/*********************************************************************
*
* @purpose  To get the class of service (cos) value.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index 
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *cos     the class of service (cos) value
*
* @returns  L7_SUCCESS, if the cos value was retrieved
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptr
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleCosGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                 L7_uint32 rulenum, L7_uint32 *cos)
{
  return aclMacRuleCosGet(aclIndex,rulenum,cos);
}

/*********************************************************************
*
* @purpose  To get the secondary class of service (cos2) value.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *cos2    the secondary class of service (cos2) value
*
* @returns  L7_SUCCESS, if the cos2 value was retrieved
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptr
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleCos2Get(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                  L7_uint32 rulenum, L7_uint32 *cos2)
{
  return aclMacRuleCos2Get(aclIndex,rulenum,cos2);
}

/*********************************************************************
*
* @purpose  To get the destination MAC address and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uchar8  *dstmac  the destination MAC address
* @param    L7_uchar8  *dstmask the destination MAC mask
*
* @returns  L7_SUCCESS, if the dst MAC address and mask are added
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptrs
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleDstMacAddrMaskGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                            L7_uint32 rulenum,
                                            L7_uchar8 *dstmac, L7_uchar8 *dstmask)
{
  return aclMacRuleDstMacGet(aclIndex,rulenum,dstmac,dstmask);
}

/*********************************************************************
*
* @purpose  To get the Ethertype keyword identifier.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_QOS_ETYPE_KEYID_t  *keyid   the Ethertype keyword identifier
* @param    L7_uint32  value    the Ethertype custom value
*
* @returns  L7_SUCCESS, if the Ethertype keyword id is retrieved
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptr
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments All Ethertype match rules have a keyword.  The *value       
*           output is only meaningful when the *keyid reads 'custom'
*           It is set to zero otherwise.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleEtypeKeyGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                      L7_uint32 rulenum, L7_QOS_ETYPE_KEYID_t *keyid,
                                      L7_uint32 *value)
{
  return aclMacRuleEtypeKeyGet(aclIndex,rulenum,keyid, value);
}

/*********************************************************************
*
* @purpose  To get the range of all the filtering criteria in a rule 
*           to the maximum, in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_BOOL    *every   true or false, match every packet
*
* @returns  L7_SUCCESS, if the match every flag value was retrieved
* @returns  L7_FAILURE, if invalid list index or rule number
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleEveryGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                   L7_uint32 rulenum, L7_BOOL *every)
{
  return aclMacRuleEveryGet(aclIndex, rulenum, every);
}

/*********************************************************************
*
* @purpose  To get the source MAC address and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uchar8  *srcmac  the source MAC address
* @param    L7_uchar8  *srcmask the source MAC mask
*
* @returns  L7_SUCCESS, if the src MAC address and mask are retrieved
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptrs
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleSrcMacAddrMaskGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                            L7_uint32 rulenum,
                                            L7_uchar8 *srcmac, L7_uchar8 *srcmask)
{
  return aclMacRuleSrcMacGet(aclIndex,rulenum,srcmac,srcmask);
}

/*********************************************************************
*
* @purpose  To get a single VLAN ID.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *vlan      the VLAN id 
*
* @returns  L7_SUCCESS, if the VLAN id is retrieved
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptr
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleVlanIdGet(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                    L7_uint32 rulenum, L7_uint32 *vlan)
{
  return aclMacRuleVlanIdGet(aclIndex,rulenum,vlan);
}

/*********************************************************************
*
* @purpose  To get a VLAN ID range.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *startvlan the start VLAN id
* @param    L7_uint32  *endvlan   the end VLAN id
*
* @returns  L7_SUCCESS, if the VLAN id range is retrieved
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptrs
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleVlanIdRangeGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                         L7_uint32 rulenum, 
                                         L7_uint32 *startvlan, L7_uint32 *endvlan)
{
  return aclMacRuleVlanIdRangeGet(aclIndex,rulenum,startvlan,endvlan);
}

/*********************************************************************
*
* @purpose  To get a single Secondary VLAN ID (vlan2).
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *vlan2     the Secondary VLAN id 
*
* @returns  L7_SUCCESS, if the vlan2 id is retrieved
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptr
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleVlanId2Get(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                     L7_uint32 rulenum, L7_uint32 *vlan2)
{
  return aclMacRuleVlanId2Get(aclIndex,rulenum,vlan2);
}

/*********************************************************************
*
* @purpose  To get a Secondary VLAN ID (vlan2) range.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *startvlan2 the start Secondary VLAN id
* @param    L7_uint32  *endvlan2   the end Secondary VLAN id
*
* @returns  L7_SUCCESS, if the Secondary VLAN id range is retireved
* @returns  L7_FAILURE, if invalid list index, rulenum, or input ptrs
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleVlanId2RangeGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                          L7_uint32 rulenum, 
                                          L7_uint32 *startvlan2, L7_uint32 *endvlan2)
{
  return aclMacRuleVlanId2RangeGet(aclIndex,rulenum,startvlan2,endvlan2);
}

/*********************************************************************
*
* @purpose  Check if logging is allowed for this MAC access list rule action.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
*                                            
* @returns  L7_SUCCESS,  if logging is allowed for current rule action
* @returns  L7_FAILURE,  if logging not allowed for current rule action
*
* @comments Checks logging feature support against current rule action value.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleLoggingAllowed(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                         L7_uint32 rulenum)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     action, featureId;

  /* check if the logging feature is supported for the current rule action */
  if (aclMacIsFieldConfigured(aclIndex, rulenum, ACL_MAC_ACTION) == L7_TRUE)
  {
    if (aclMacRuleActionGet(aclIndex, rulenum, &action) == L7_SUCCESS)
    {
      featureId = (action == L7_ACL_DENY) ? L7_ACL_LOG_DENY_FEATURE_ID : L7_ACL_LOG_PERMIT_FEATURE_ID;
      if (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, featureId) == L7_TRUE)
        rc = L7_SUCCESS;
    }
  }

  return rc;
}

/********************************************************************
*
* Purpose  To get the assigned time range name from an existing MAC access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uchar8  *timeRangeName  @b{(output)}  the time-range name is a string   
* identifying the  time range associated with the  ACL Rule
*
* @returns  L7_SUCCESS, if the time range name is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleTimeRangeNameGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum, L7_uchar8 *timeRangeName)
{
   return aclMacRuleTimeRangeNameGet(aclIndex, rulenum, timeRangeName);
}

/*********************************************************************
*
* @purpose  To set the assigned time range for a MAC access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclIndex the access-list identifying index
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uchar8  *timeRangeName  the time-range name is a string   
* identifying the  time range associated with the  ACL Rule
*
* @returns  L7_SUCCESS, if assigned time range name is added
* @returns  L7_FAILURE, if invalid time range name or other failure
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleTimeRangeNameAdd(L7_uint32 UnitIndex, L7_uint32 
aclIndex, L7_uint32 rulenum,  L7_uchar8 *timeRangeName)
{
  return aclMacRuleTimeRangeNameAdd(aclIndex, rulenum, timeRangeName);
}

/*********************************************************************
*
* Purpose  To get the rule status of MAC access list rule entry.
*
* @param    UnitIndex  @b{(input)}  the unit for this operation
* @param    aclIndex   @b{(input)}  the access-list identifying index
* @param    rulenum    @b{(input)}  the acl rule number
* @param    status     @b{(output)} rule status 
*
* @returns  L7_SUCCESS, if acl rule status is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclMacRuleStatusGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *status)
{
   return aclMacRuleStatusGet(aclIndex, rulenum, status);
}

