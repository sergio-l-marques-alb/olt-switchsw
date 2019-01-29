/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    usmdb_qos_acl.c
* @purpose     usmdb access list API functions
* @component   base BP
* @comments    none
* @create      06/12/2002
* @author      djohnson
* @end
*             
**********************************************************************/


#include <string.h>
#include "l7_common.h"
#include "nimapi.h"
#include "usmdb_qos_acl_api.h"
#include "usmdb_util_api.h"
#include "trap_qos_api.h"
#include "acl_api.h"


/********************************************/
/*                 ACL API                  */
/********************************************/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new named ACL in the access list table
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    aclType     @b{(input)}  access list type
* @param    *pAclIndex  @b{(output)} pointer to ACL index value
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
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
L7_RC_t usmDbQosAclNamedIndexNextFree(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                      L7_uint32 *pAclIndex)
{
  return aclNamedIndexNextFree(aclType, pAclIndex);
}

/*************************************************************************
* @purpose  Retrieve the min/max index values allowed for the named ACL table 
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    aclType     @b{(input)}  access list type
* @param    *pMin       @b{(output)} pointer to index min output location
* @param    *pMax       @b{(output)} pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE
*
* @comments This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @end
*********************************************************************/
L7_RC_t usmDbQosAclNamedIndexMinMaxGet(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                       L7_uint32 *pMin, L7_uint32 *pMax)
{
  return aclNamedIndexMinMaxGet(aclType, pMin, pMax);
}

/*********************************************************************
*
* @purpose  Get the first named access list index created in the system.
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    aclType     @b{(input)}  access list type
* @param    *pAclIndex  @b{(output)} pointer to ACL index value
*
* @returns  L7_SUCCESS        First named access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE  
* @returns  L7_ERROR          No access lists have been created yet
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNamedIndexGetFirst(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                      L7_uint32 *pAclIndex)
{
  return aclNamedIndexGetFirst(aclType, pAclIndex);
}

/*********************************************************************
*
* @purpose  Get the first named access list index created in the system.
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  current ACL index value
* @param    *pAclIndex  @b{(output)} pointer to next ACL index value
*
* @returns  L7_SUCCESS        Next named access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_ERROR          No subsequent ACL indexes exist
* @returns  L7_FAILURE        All other failures
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNamedIndexGetNext(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                     L7_uint32 aclIndex, L7_uint32 *pAclIndex)
{
  return aclNamedIndexGetNext(aclType, aclIndex, pAclIndex);
}

/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured named access list
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE        Access list index not found
*
* @comments The only valid aclType values are L7_ACL_TYPE_IPV6 and 
*           L7_ACL_TYPE_IP.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNamedIndexCheckValid(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                        L7_uint32 aclIndex)
{
  return aclNamedIndexCheckValid(aclType, aclIndex);
}

/*********************************************************************
*
* @purpose  Checks if the aclIndex is within proper range
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index is in range
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE        Access list index is out of range
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNamedIndexRangeCheck(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                        L7_uint32 aclIndex)
{
  return aclNamedIndexRangeCheck(aclType, aclIndex);
}

/*********************************************************************
*
* @purpose  Get the first access list ID created on the system.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  *aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the first access list ID was found
* @returns  L7_ERROR,   if the first access list ID was not found
* @returns  L7_FAILURE, if any other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNumGetFirst(L7_uint32 UnitIndex, L7_uint32 *aclnum)
{
  return aclNumGetFirst(aclnum);
}

/*********************************************************************
*
* @purpose  Given aclnum, get the next access list ID
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32   aclnum   the current access-list ID
* @param    L7_uint32  *next     the next ACL ID
*
* @returns  L7_SUCCESS, if the next access list ID was found
* @returns  L7_ERROR,   if no subsequent access list ID was found
* @returns  L7_FAILURE, if any other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNumGetNext(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uint32 *next)
{
  return aclNumGetNext(aclnum,next);
}

/*********************************************************************
*
* @purpose  Checks if aclnum is a valid, configured access list
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNumCheckValid(L7_uint32 UnitIndex, L7_uint32 aclnum)
{
  return aclNumCheckValid(aclnum);
}

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the supported IPV4 number ranges
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This function does not distinguish between standard and extended
*           IP ACL numbers, but considers whether the ACL ID belongs to any
*           IPv4 ACL number range that is supported.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNumRangeCheck(L7_uint32 UnitIndex, L7_uint32 aclnum)
{
  return aclNumRangeCheck(aclnum);
}

/*********************************************************************
*
* @purpose  Get the index number of an access list, given its ACL type and name.
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    aclType     @b{(input)}  access list type
* @param    *pName      @b{(input)}  access list name pointer
* @param    *aclNum     @b{(output)} access list identifying number
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR    ACL does not exist
* @returns  L7_FAILURE
*
* @comments Uses the ACL type and name to output the list index
*           number.  For IPV4 numbered access lists, the ACL name
*           is its number represented in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t usmDbAclCommonNameToIndex(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                  L7_uchar8 *pName, L7_uint32 *pAclNum)
{
  return aclCommonNameToIndex(aclType, pName, pAclNum);
}

/*********************************************************************
*
* @purpose  Get a name string for any type of access list, given its index
*
* @param    UnitIndex   @b{(input)}  the unit for this operation
* @param    aclnum      @b{(input)}  access list identifying number
* @param    *aclType    @b{(output)} access list type
* @param    *name       @b{(output)} access list name
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @comments Determines the ACL type from its index number and outputs
*           the list name.  For IPV4 numbered access lists, the 
*           ACL number is output in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclCommonNameStringGet(L7_uint32 UnitIndex, L7_uint32 aclnum,
                                       L7_ACL_TYPE_t *aclType, L7_uchar8 *name)
{
  return aclCommonNameStringGet(aclnum, aclType, name);
}

/*********************************************************************
*
* @purpose  To create a new access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum  the access-list identifying number
*
* @returns  L7_SUCCESS, if the access-list is added
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list already exists
* @returns  L7_REQUEST_DENIED, if the access-list config change is not approved
* @returns  L7_TABLE_IS_FULL, if the maximum number of ACLs or rule nodes already created
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclCreate(L7_uint32 UnitIndex, L7_uint32 aclnum)
{
  return aclCreate(aclnum);
}

/*********************************************************************
*
* @purpose  To delete an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum  the access-list identifying number
*
* @returns  L7_SUCCESS, if the access-list is deleted
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list does not exist
* @returns  L7_REQUEST_DENIED, if the access-list config change is not approved
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclDelete(L7_uint32 UnitIndex, L7_uint32 aclnum)
{
  return aclDelete(aclnum, L7_FALSE);
}

/*********************************************************************
*
* @purpose  To add the name to this named access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
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
L7_RC_t usmDbQosAclNameAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uchar8 *name)
{
  return aclNameAdd(aclnum, name);
}

/*********************************************************************
*
* @purpose  To change the name of an existing named access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
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
L7_RC_t usmDbQosAclNameChange(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uchar8 *newname)
{
  return aclNameChange(aclnum, newname);
}

/*********************************************************************
*
* @purpose  To get the name of this named access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  name     the access-list name
*                                            
* @returns  L7_SUCCESS, if access list name is retrieved
* @returns  L7_FAILURE, if invalid name or identifying number
* @returns  L7_ERROR, if the access-list does not exist
*
* @comments Caller must provide a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNameGet(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uchar8 *name)
{
  return aclNameGet(aclnum, name);
}

/*********************************************************************
*
* @purpose  Checks if the named ACL name is a valid string
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uchar8  *name      the access-list name
*
* @returns  L7_SUCCESS, if the named access list name is valid
* @returns  L7_FAILURE, if the named access list name is invalid
*
* @comments This function only checks the name string syntax for a named ACL. 
*           It does not check if any named ACL currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNameStringCheck(L7_uint32 UnitIndex, L7_uchar8 *name)
{
  return aclNameStringCheck(name);
}

/*********************************************************************
*
* @purpose  To get the index of a named access list, given its name.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  *name      the access-list name
* @param    L7_uint32  *aclnum    the access-list identifying number
*                                            
* @returns  L7_SUCCESS, if named access list number is retrieved
* @returns  L7_FAILURE, if invalid parms, or other failure
* @returns  L7_ERROR,   if access list name does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclNameToIndex(L7_uint32 UnitIndex, L7_uchar8 *name, L7_uint32 *aclnum)
{
  return aclNameToIndex(name, aclnum);
}

/*********************************************************************
*
* @purpose  To add an action to a new access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  action   action type (permit/deny)
*
* @returns  L7_SUCCESS, if the access option is added
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list does not exist
* @returns  L7_HARDWARE_ERROR, if hardware update failed
* @returns  L7_REQUEST_DENIED, if access list config change is not approved
* @returns  L7_TABLE_IS_FULL, if storage unavailable for rule
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleActionAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                 L7_uint32 rulenum, L7_uint32 action)
{
  /* this API is used for creating a rule as well as changing the action
   * of an existing rule
   */
  return aclRuleActionAdd(aclnum,rulenum,action);
}

/*********************************************************************
*
* @purpose  To remove a rule from an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
*                                            
* @returns  L7_SUCCESS, if the rule number is removed
* @returns  L7_FAILURE, if invalid access-list number or rule number
* @returns  L7_ERROR, if the access-list or rule number does not exist
* @returns  L7_HARDWARE_ERROR, if hardware update failed
* @returns  L7_REQUEST_DENIED, if access list config change is not approved
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleRemove(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uint32 rulenum)
{
  return aclRuleRemove(aclnum,rulenum);
}

/*********************************************************************
*
* @purpose  To set the assigned queue id for an access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  queueId  the queue id value
*                                            
* @returns  L7_SUCCESS, if assigned queue id is added
* @returns  L7_FAILURE, if invalid queue id or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleAssignQueueIdAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                        L7_uint32 rulenum, L7_uint32 queueId)
{
  return aclRuleAssignQueueIdAdd(aclnum,rulenum,queueId);
}

/*********************************************************************
*
* @purpose  To set the redirect interface for an access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  intIfNum the redirect internal interface number
*                                            
* @returns  L7_SUCCESS, if redirect interface is added
* @returns  L7_FAILURE, if invalid redirect interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleRedirectIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
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

  return aclRuleRedirectConfigIdAdd(aclnum,rulenum,&configId);
}

/*********************************************************************
*
* @purpose  To set the mirror interface for an access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  intIfNum the mirror internal interface number
*                                            
* @returns  L7_SUCCESS, if mirror interface is added
* @returns  L7_FAILURE, if invalid mirror interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleMirrorIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
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

  return aclRuleMirrorConfigIdAdd(aclnum,rulenum,&configId);
}

/*********************************************************************
*
* @purpose  To set the logging flag for an access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  logFlag  the rule logging flag value
*                                            
* @returns  L7_SUCCESS, if logging flag is added
* @returns  L7_FAILURE, if invalid logging flag or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleLoggingAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                  L7_uint32 rulenum, L7_BOOL logFlag)
{
  return aclRuleLoggingAdd(aclnum,rulenum,logFlag);
}

/*********************************************************************
*
* @purpose  To add the destination ip address and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  ipAddr   the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleDstIpMaskAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 ipAddr, L7_uint32 mask)
{
  return aclRuleDstIpMaskAdd(aclnum,rulenum,ipAddr,mask);
}

/*********************************************************************
*
* @purpose  To add the destination IPv6 address and prefix length.
*
* @param    UnitIndex       @b{(input)} the unit for this operation
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if destination ipv6 address and prefix length are added
* @returns  L7_FAILURE, if invalid list number, entry id, or other
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleDstIpv6AddrAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                      L7_uint32 rulenum, L7_in6_prefix_t *addr6)
{
  return aclRuleDstIpv6AddrAdd(aclnum, rulenum, addr6);
}

/*********************************************************************
*
* @purpose  To add the range of the source layer 4 ports to a rule
*           in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleDstL4PortRangeAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                         L7_uint32 rulenum, 
                                         L7_uint32 startport, L7_uint32 endport)
{
  return aclRuleDstL4PortRangeAdd(aclnum,rulenum,startport,endport);
}

/*********************************************************************
*
* @purpose  To add a single destination layer 4 port to a rule
*           in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  port       the port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleDstL4PortAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                    L7_uint32 rulenum, L7_uint32 port)
{
  return aclRuleDstL4PortAdd(aclnum,rulenum,port);
}

/*********************************************************************
*
* @purpose  Set the match condition whereby all packets match for an
*           ACL rule
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_BOOL    match    the match condition (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list or rule number
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments This routine sets the range of all the filtering criteria
*           in a rule to the maximum, in an existing access list.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleEveryAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                L7_uint32 rulenum, L7_BOOL match)
{
  return aclRuleEveryAdd(aclnum, rulenum, match);
}

/*********************************************************************
*
* @purpose  To set the range of all the filtering criteria in a rule 
*           to the maximum, in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_BOOL    every    true or false, match every packet
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list or rule number
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleEveryGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                L7_uint32 rulenum, L7_BOOL *every)
{
  return aclRuleEveryGet(aclnum, rulenum, every);
}

/*********************************************************************
*
* @purpose  To add the differentiated services code point (dscp) value
*           to a rule in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  dscpval  the diffserv code point value
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleIPDscpAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                 L7_uint32 rulenum, L7_uint32 dscpval)
{
  return aclRuleIPDscpAdd(aclnum,rulenum,dscpval);
}

/*********************************************************************
*
* @purpose  To add the precedence value to a rule in an existing
*           access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum         access-list identifying number
* @param    L7_uint32  rulenum        the acl rule number
* @param    L7_uint32  precedenceval  the precedence value
*
* @returns  L7_SUCCESS, if the precedence value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleIPPrecedenceAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                       L7_uint32 rulenum, L7_uint32 precedenceval)
{
  return aclRuleIPPrecedenceAdd(aclnum,rulenum,precedenceval);
}

/*********************************************************************
*
* @purpose  To add the type of service bits and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  tosbits  tos bits
* @param    L7_uint32  tosmask  tos mask
*
* @returns  L7_SUCCESS, if the service type parameters are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleIPTosAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                L7_uint32 rulenum, 
                                L7_uint32 tosbits, L7_uint32 tosmask)
{
  return aclRuleIPTosAdd(aclnum,rulenum,tosbits,tosmask);
}

/*********************************************************************
*
* @purpose  To add the IPv6 flow label.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  flowlbl  flow label value
*
* @returns  L7_SUCCESS, if the value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleIpv6FlowLabelAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                        L7_uint32 rulenum, L7_uint32 flowlbl)
{
  return aclRuleIpv6FlowLabelAdd(aclnum,rulenum,flowlbl);
}

/*********************************************************************
*
* @purpose  To add a protocol to an existing access list entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  protocol  the protocol number
*
* @returns  L7_SUCCESS, if the protocol is added
* @returns  L7_FAILURE, if invalid list number, rulenum, or protocol
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleProtocolAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                   L7_uint32 rulenum, L7_uint32 protocol)
{
  return aclRuleProtocolAdd(aclnum,rulenum,protocol);
}

/*********************************************************************
*
* @purpose  To add the source ip address and net mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  entryId  the access-list entry id
* @param    L7_uint32  ip       the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleSrcIpMaskAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                    L7_uint32 rulenum, L7_uint32 ipAddr, L7_uint32 mask)
{
  return aclRuleSrcIpMaskAdd(aclnum,rulenum,ipAddr,mask);
}

/*********************************************************************
*
* @purpose  To add the source IPv6 address and prefix length.
*
* @param    UnitIndex       @b{(input)} the unit for this operation
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if source ipv6 address and prefix length are added
* @returns  L7_FAILURE, if invalid list number, entry id, or other
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleSrcIpv6AddrAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                      L7_uint32 rulenum, L7_in6_prefix_t *addr6)
{
  return aclRuleSrcIpv6AddrAdd(aclnum, rulenum, addr6);
}

/*********************************************************************
*
* @purpose  To add the source port.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  port     the ip address
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleSrcL4PortAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                    L7_uint32 rulenum, L7_uint32 port)
{
  return aclRuleSrcL4PortAdd(aclnum,rulenum,port);
}

/*********************************************************************
*
* @purpose  To add the range of the source layer 4 ports to a rule
*           in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end                     
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleSrcL4PortRangeAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                         L7_uint32 rulenum, 
                                         L7_uint32 startport, L7_uint32 endport)
{
  return aclRuleSrcL4PortRangeAdd(aclnum,rulenum,startport,endport);
}

/*********************************************************************
*
* @purpose  Get the first access list rule given an ACL ID
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_uint32   *rule    the next rule ID
*
* @returns  L7_SUCCESS, if the first rule for this ACL ID was found
* @returns  L7_FAILURE, 
* @returns  L7_ERROR,   if no rules have been created for this ACL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleGetFirst(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uint32 *rule)
{
  return aclRuleGetFirst(aclnum, rule);
}

/*********************************************************************
*
* @purpose  Get the next access list rule given an ACL ID
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32   aclnum   the access-list ID
* @param    L7_uint32   rule     the current rule ID
* @param    L7_uint32  *next     the next rule ID
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_FAILURE, if rule is the last valid rule ID for this ACL
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleGetNext(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                               L7_uint32 rule, L7_uint32 *next)
{
  return aclRuleGetNext(aclnum,rule,next);
}

/*********************************************************************
*
* @purpose  Checks if aclnum is a valid, configured access list
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleCheckValid(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uint32 rulenum)
{
  return aclRuleCheckValid(aclnum, rulenum);
}

/*********************************************************************
*
* @purpose  Checks if the ACL rule number belongs to a supported number range
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32   aclnum     the access-list identifying number
* @param    L7_uint32   rulenum    the current rule ID
*
* @returns  L7_SUCCESS, if the rule ID belongs to a supported range
* @returns  L7_FAILURE, if the rule ID is out of range
*
* @comments The aclnum parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleNumRangeCheck(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uint32 rulenum)
{
  return aclRuleNumRangeCheck(aclnum, rulenum);
}

/*********************************************************************
*
* @purpose  Checks if aclnum is currently assigned to this intf,dir pair
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  intf       the interface number
* @param    L7_uint32  direction  the interface direction
*
* @returns  L7_SUCCESS, if the ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the ACL is not assigned to this intf,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclIntfDirCheckValid(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                     L7_uint32 intf, L7_uint32 direction)
{
  return aclIntfDirCheckValid(aclnum, intf, direction);
}

/*********************************************************************
*
* @purpose  Checks if aclnum is currently assigned to this vlan,dir pair
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  vlan       the VLAN number
* @param    L7_uint32  direction  the traffic direction
*
* @returns  L7_SUCCESS, if the ACL is assigned to this vlan,dir
* @returns  L7_FAILURE, if the ACL is not assigned to this vlan,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanDirCheckValid(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                     L7_uint32 vlan, L7_uint32 direction)
{
  return aclVlanDirCheckValid(aclnum, vlan, direction);
}

/*********************************************************************
*
* @purpose  Checks to see if a user has configured a field in a rule
*           in an access list
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum    the access-list identifying number
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
L7_BOOL usmDbQosAclIsFieldConfigured(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                     L7_uint32 rule, L7_uint32 field)
{
  return aclIsFieldConfigured(aclnum,rule,field);
}

/*********************************************************************
*
* @purpose  To get the action for a rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *action  action type (permit/deny) (aclActionIndex_t)
*
* @returns  L7_SUCCESS, if the access option is retrieved
* @returns  L7_FAILURE, if there is no action set
* @returns  L7_ERROR,   if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleActionGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                 L7_uint32 rulenum, L7_uint32 *action)
{
  return aclRuleActionGet(aclnum,rulenum,action);
}

/*********************************************************************
*
* @purpose  To get the assigned queue id from an existing access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
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
L7_RC_t usmDbQosAclRuleAssignQueueIdGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                        L7_uint32 rulenum, L7_uint32 *queueId)
{
  return aclRuleAssignQueueIdGet(aclnum,rulenum,queueId);
}

/*********************************************************************
*
* @purpose  To get the redirect interface from an existing access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
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
L7_RC_t usmDbQosAclRuleRedirectIntfGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                       L7_uint32 rulenum, L7_uint32 *intIfNum)
{
  L7_RC_t       rc;
  nimConfigID_t configId;

  rc = aclRuleRedirectConfigIdGet(aclnum,rulenum,&configId);
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
* @param    L7_uint32  aclnum     the access-list identifying number
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
L7_RC_t usmDbQosAclRuleMirrorIntfGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                     L7_uint32 rulenum, L7_uint32 *intIfNum)
{
  L7_RC_t       rc = L7_FAILURE;
  nimConfigID_t configId;

  rc = aclRuleMirrorConfigIdGet(aclnum,rulenum,&configId);
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
* @purpose  To get the logging flag from an existing access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
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
L7_RC_t usmDbQosAclRuleLoggingGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                  L7_uint32 rulenum, L7_BOOL *logFlag)
{
  return aclRuleLoggingGet(aclnum,rulenum,logFlag);
}

/*********************************************************************
*
* @purpose  To get a protocol from an existing access list entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *protocol  the protocol number
*
* @returns  L7_SUCCESS, if the protocol is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleProtocolGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                   L7_uint32 rulenum, L7_uint32 *protocol)
{
  return aclRuleProtocolGet(aclnum,rulenum,protocol);
}

/*********************************************************************
*
* @purpose  To get the source ip address and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  aclnum   the access-list entry id
* @param    L7_uint32  *ipAddr  the ip address
* @param    L7_uint32  *mask    the ip address mask
*
* @returns  L7_SUCCESS, if source ip address and mask are added
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleSrcIpMaskGet(L7_uint32 UnitIndex, L7_uint32 aclnum,  
                                    L7_uint32 rulenum, 
                                    L7_uint32 *ipAddr, L7_uint32 *mask)
{
  return aclRuleSrcIpMaskGet(aclnum,rulenum,ipAddr,mask);
}

/*********************************************************************
*
* @purpose  Get the source IPv6 address and prefix length.
*
* @param    UnitIndex       @b{(input)} the unit for this operation
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if source ipv6 address and prefix length was obtained
* @returns  L7_FAILURE, if invalid list number, entry id, or other
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleSrcIpv6AddrGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                      L7_uint32 rulenum, L7_in6_prefix_t *addr6)
{
  return aclRuleSrcIpv6AddrGet(aclnum, rulenum, addr6);
}

/*********************************************************************
*
* @purpose  To get the L4 source port.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *port    L4 port number
*
* @returns  L7_SUCCESS, if L4 source port is retrieved
* @returns  L7_FAILURE, if invalid list number or entry id 
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleSrcL4PortGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                    L7_uint32 rulenum, L7_uint32 *port)
{
  return aclRuleSrcL4PortGet(aclnum,rulenum,port);
}

/*********************************************************************
*
* @purpose  To get the range of the source layer 4 ports of a rule
*           in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  rulenum     the acl rule number
* @param    L7_uint32  *startport  the start port number
* @param    L7_uint32  *endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is retrieved
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleSrcL4PortRangeGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                         L7_uint32 rulenum, 
                                         L7_uint32 *startport, L7_uint32 *endport)
{
  return aclRuleSrcL4PortRangeGet(aclnum,rulenum,startport,endport);
}

/*********************************************************************
*
* @purpose  Get the destination ip address and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *ipAddr  the ip address
* @param    L7_uint32  *mask    the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are retrieved
* @returns  L7_FAILURE, if invalid list number, entry id, or port
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleDstIpMaskGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                    L7_uint32 rulenum, 
                                    L7_uint32 *ipAddr, L7_uint32 *mask)
{
  return aclRuleDstIpMaskGet(aclnum,rulenum,ipAddr,mask);
}

/*********************************************************************
*
* @purpose  Get the destination IPv6 address and prefix length.
*
* @param    UnitIndex       @b{(input)} the unit for this operation
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if destination ipv6 address and prefix length was obtained
* @returns  L7_FAILURE, if invalid list number, entry id, or other
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleDstIpv6AddrGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                      L7_uint32 rulenum, L7_in6_prefix_t *addr6)
{
  return aclRuleDstIpv6AddrGet(aclnum, rulenum, addr6);
}

/*********************************************************************
*
* @purpose  Get the L4 destination port.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *port    the L4 port number
*
* @returns  L7_SUCCESS, if destination ip address, net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or ip
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleDstL4PortGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                    L7_uint32 rulenum, L7_uint32 *port)
{
  return aclRuleDstL4PortGet(aclnum,rulenum,port);
}

/*********************************************************************
*
* @purpose  Get the range of the destination layer 4 ports for a rule
*           in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  rulenum     the acl rule number
* @param    L7_uint32  *startport  the start port number
* @param    L7_uint32  *endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is retrieved
* @returns  L7_FAILURE, if invalid list number, entry id, or port
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleDstL4PortRangeGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                         L7_uint32 rulenum, 
                                         L7_uint32 *startport, L7_uint32 *endport)
{
  return aclRuleDstL4PortRangeGet(aclnum,rulenum,startport,endport);
}

/*********************************************************************
*
* @purpose  To get the differentiated services code point (dscp) value
*           for a rule in an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *dscpval  the diffserv code point value
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleIPDscpGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                 L7_uint32 rulenum, L7_uint32 *dscpval)
{
  return aclRuleIPDscpGet(aclnum,rulenum,dscpval);
}

/*********************************************************************
*
* @purpose  To get the precedence value for a rule in an existing
*           access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum          access-list identifying number
* @param    L7_uint32  rulenum         the acl rule number
* @param    L7_uint32  *precedenceval  the precedence value
*
* @returns  L7_SUCCESS, if the precedence value is retrieved
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleIPPrecedenceGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                       L7_uint32 rulenum, L7_uint32 *precedenceval)
{
  return aclRuleIPPrecedenceGet(aclnum,rulenum,precedenceval);
}

/*********************************************************************
*
* @purpose  Check if logging is allowed for this access list rule action
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
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
L7_RC_t usmDbQosAclRuleLoggingAllowed(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                      L7_uint32 rulenum)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     action, featureId;

  /* check if the logging feature is supported for the current rule action */
  if (aclIsFieldConfigured(aclnum, rulenum, ACL_ACTION) == L7_TRUE)
  {
    if (aclRuleActionGet(aclnum, rulenum, &action) == L7_SUCCESS)
    {
      featureId = (action == L7_ACL_DENY) ? L7_ACL_LOG_DENY_FEATURE_ID : L7_ACL_LOG_PERMIT_FEATURE_ID;
      if (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, featureId) == L7_TRUE)
        rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To get the type of service bits and mask.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *tosbits  tos bits
* @param    L7_uint32  *tosmask  tos mask
*
* @returns  L7_SUCCESS, if the service type parameters are added
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleIPTosGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                L7_uint32 rulenum, 
                                L7_uint32 *tosbits, L7_uint32 *tosmask)
{
  return aclRuleIPTosGet(aclnum,rulenum,tosbits,tosmask);
}

/*********************************************************************
*
* @purpose  To get the IPv6 flow label.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *flowlbl  flow label value ptr
*
* @returns  L7_SUCCESS, if the value is added
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleIpv6FlowLabelGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                        L7_uint32 rulenum, L7_uint32 *flowlbl)
{
  return aclRuleIpv6FlowLabelGet(aclnum, rulenum, flowlbl);
}

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  seqNum     the ACL evaluation order sequence number
*
* @returns  L7_SUCCESS, if ACL successfully added to intf,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED, if intf,dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED, if feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL, if ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED, if error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclInterfaceDirectionAdd(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                         L7_uint32 direction, L7_uint32 aclnum,
                                         L7_uint32 seqNum)
{
  return aclInterfaceDirectionAdd(intIfNum,direction,aclnum,seqNum);
}

/*********************************************************************
*
* @purpose  To remove an interface and the access list application
*           direction to an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if ACL successfully removed from intf,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL does not exist for this aclnum
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclInterfaceDirectionRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                            L7_uint32 direction, L7_uint32 aclnum)
{
  return aclInterfaceDirectionRemove(intIfNum,direction,aclnum);
}

/*********************************************************************
*
* @purpose  Verifies that the specified interface and direction is valid
*           for use with ACL
*
* @param    UnitIndex   @b{(input)}  unit for this operation
* @param    intIfNum    @b{(input)}  interal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS  intf,dir is valid for ACL
* @returns  L7_ERROR    intf,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The interface type and direction supported for ACL use   
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the interface
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the interface
*           and direction (see usmDbQosAclIsInterfaceInUse).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclIntfDirGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                              L7_uint32 direction)
{
  return aclIntfDirGet(intIfNum, direction);
}

/*********************************************************************
*
* @purpose  Get next sequential interface and direction that is valid
*           for use with ACL
*
* @param    UnitIndex       @b{(input)}  unit for this operation
* @param    intIfNum        @b{(input)}  interal interface number
* @param    direction       @b{(input)}  interface direction
*                                        (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *nextIntIfNum   @b{(output)} next interal interface number
* @param    *nextDirection  @b{(output)} next interface direction (internal)
*
* @returns  L7_SUCCESS  next intf,dir for ACL was obtained
* @returns  L7_ERROR    no more valid intf,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The interface type and direction supported for ACL use   
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an interface and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the interface
*           and direction (see usmDbQosAclIsInterfaceInUse).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclIntfDirGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 direction,
                                  L7_uint32 *nextIntIfNum, L7_uint32 *nextDirection)
{
  return aclIntfDirGetNext(intIfNum, direction, nextIntIfNum, nextDirection);
}

/*********************************************************************
*
* @purpose  Verifies that the ACL sequence number is in use for the  
*           specified interface and direction.
*
* @param    UnitIndex   @b{(input)}  unit for this operation
* @param    intIfNum    @b{(input)}  interal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL interface sequence number
*
* @returns  L7_SUCCESS  sequence number exists for intf,dir
* @returns  L7_ERROR    sequence number does not exist for intf,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments A sequence number value may be used only once for a given
*           interface and direction pair.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclIntfDirSequenceGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                      L7_uint32 direction, L7_uint32 seqNum)
{
  return aclIntfDirSequenceGet(intIfNum, direction, seqNum);
}

/*********************************************************************
*
* @purpose  Get next sequential ACL interface sequence number in use 
*           for the specified interface and direction.
*
* @param    UnitIndex   @b{(input)}  unit for this operation
* @param    intIfNum    @b{(input)}  interal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL interface sequence number
* @param    *nextSeqNum @b{(output)} next ACL interface sequence number
*
* @returns  L7_SUCCESS  next sequence number was obtained
* @returns  L7_ERROR    no more sequence numbers for intf,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments Use a seqNum value of 0 to find the first ACL sequence   
*           number for this interface and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclIntfDirSequenceGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                          L7_uint32 direction, L7_uint32 seqNum,
                                          L7_uint32 *nextSeqNum)
{
  return aclIntfDirSequenceGetNext(intIfNum, direction, seqNum, nextSeqNum);
}

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           interface and direction
*
* @param    UnitIndex   @b{(input)}  unit for this operation
* @param    intIfNum    @b{(input)}  interal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid interface, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the interface in this 
*                       direction
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @comments The aclId contained in the output listInfo must be interpreted 
*           based on the aclType (for example, aclnum for IP, aclIndex for 
*           MAC).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclIntfDirAclListGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 direction,
                                     L7_ACL_INTF_DIR_LIST_t *listInfo)
{
  return aclIntfDirAclListGet(intIfNum, direction, listInfo);
}

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which an ACL is assigned in the
*           specified direction
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum      the access-list identifying number
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
L7_RC_t usmDbQosAclAssignedIntfDirListGet(L7_uint32 UnitIndex, L7_uint32 aclnum,
                                          L7_uint32 direction,
                                          L7_ACL_ASSIGNED_INTF_LIST_t *intfList)
{
  return aclAssignedIntfDirListGet(aclnum,direction,intfList);
}

/*********************************************************************
*
* @purpose  To check if an interface and direction is in use by any access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  intIfNum  the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE, if the intf,dir has at least one ACL attached
* @returns  L7_FALSE, if the intf,dir has no ACL attached
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL usmDbQosAclIsInterfaceInUse(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                    L7_uint32 direction)
{
  return aclIsInterfaceInUse(intIfNum, direction);
}

/*********************************************************************
*
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  seqNum     the ACL evaluation order sequence number
*
* @returns  L7_SUCCESS, if ACL successfully added to vlan,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED, if vlan,dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED, if feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL, if ACL assignment at capacity for vlan, dir
* @returns  L7_REQUEST_DENIED, if error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanDirectionAdd(L7_uint32 UnitIndex, L7_uint32 vlanNum,
                                    L7_uint32 direction, L7_uint32 aclnum, L7_uint32 seqNum)
{
  return aclVlanDirectionAdd(vlanNum, direction, aclnum, seqNum);
}

/*********************************************************************
*
* @purpose  To remove an vlan and the access list application
*           direction to an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if ACL successfully removed from vlan,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL does not exist for this aclnum
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanDirectionRemove(L7_uint32 UnitIndex, L7_uint32 vlanNum,
                                       L7_uint32 direction, L7_uint32 aclnum)
{
  return aclVlanDirectionRemove(vlanNum, direction, aclnum);
}

/*********************************************************************
*
* @purpose  Verifies that the specified vlan and direction is valid
*           for use with ACL
*
* @param    UnitIndex   @b{(input)}  unit for this operation
* @param    vlanNum    @b{(input)}   interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS  vlan,dir is valid for ACL
* @returns  L7_ERROR    vlan,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the vlan
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the vlan
*           and direction (see usmDbQosAclIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanDirGet(L7_uint32 UnitIndex, L7_uint32 vlanNum,
                              L7_uint32 direction)
{
  return aclVlanDirGet(vlanNum, direction);
}

/*********************************************************************
*
* @purpose  Get next sequential vlan and direction that is valid
*           for use with ACL
*
* @param    UnitIndex       @b{(input)}  unit for this operation
* @param    vlanNum         @b{(input)}  interal vlan number
* @param    direction       @b{(input)}  vlan direction
*                                        (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *nextVlanNum    @b{(output)} next interal vlan number
* @param    *nextDirection  @b{(output)} next vlan direction (internal)
*
* @returns  L7_SUCCESS  next vlan,dir for ACL was obtained
* @returns  L7_ERROR    no more valid vlan,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an vlan and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the vlan
*           and direction (see usmDbQosAclIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanDirGetNext(L7_uint32 UnitIndex, L7_uint32 vlanNum,
                                  L7_uint32 direction, L7_uint32 *nextVlanNum, L7_uint32 *nextDirection)
{
  return aclVlanDirGetNext(vlanNum, direction, nextVlanNum, nextDirection);
}

/*********************************************************************
*
* @purpose  Get next sequential vlan ID configured with an ACL binding.
*
* @param    UnitIndex     @b{(input)}  unit for this operation
* @param    vlanNum       @b{(input)}  current vlan ID
* @param    *nextVlanNum  @b{(input)}  next vlan ID, if any
*
* @returns  L7_SUCCESS  next vlan ID was obtained
* @returns  L7_ERROR    invalid parameter or other failure
* @returns  L7_FAILURE  no more vlan IDs found
*
* @comments Use a vlanid value of 0 to find the first vlan ID configured
*           with an ACL binding.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanIdGetNext(L7_uint32 UnitIndex, L7_uint32 vlanNum, L7_uint32 *nextVlanNum)
{
  return aclVlanIdGetNext(vlanNum, nextVlanNum);
}

/*********************************************************************
*
* @purpose  Verifies that the ACL sequence number is in use for the
*           specified vlan and direction.
*
* @param    UnitIndex   @b{(input)}  unit for this operation
* @param    vlanNum    @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
*
* @returns  L7_SUCCESS  sequence number exists for vlan,dir
* @returns  L7_ERROR    sequence number does not exist for vlan,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments A sequence number value may be used only once for a given
*           vlan and direction pair.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanDirSequenceGet(L7_uint32 UnitIndex, L7_uint32 vlanNum,
                                       L7_uint32 direction, L7_uint32 seqNum)
{
  return aclVlanDirSequenceGet(vlanNum, direction, seqNum);
}

/*********************************************************************
*
* @purpose  Get next sequential ACL vlan sequence number in use
*           for the specified vlan and direction.
*
* @param    UnitIndex   @b{(input)}  unit for this operation
* @param    vlanNum     @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
* @param    *nextSeqNum @b{(output)} next ACL vlan sequence number
*
* @returns  L7_SUCCESS  next sequence number was obtained
* @returns  L7_ERROR    no more sequence numbers for vlan,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments Use a seqNum value of 0 to find the first ACL sequence
*           number for this vlan and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanDirSequenceGetNext(L7_uint32 UnitIndex, L7_uint32 vlanNum,
                                          L7_uint32 direction, L7_uint32 seqNum, L7_uint32 *nextSeqNum)
{
  return aclVlanDirSequenceGetNext(vlanNum, direction, seqNum, nextSeqNum);
}

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           vlan and direction
*
* @param    UnitIndex   @b{(input)}  unit for this operation
* @param    vlanNum     @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid vlan, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the vlan in this
*                       direction
*
* @comments An ACL can be shared/associated with multiple vlans.
*
* @comments The aclId contained in the output listInfo must be interpreted
*           based on the aclType (for example, aclnum for IP, aclIndex for
*           MAC).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclVlanDirAclListGet(L7_uint32 UnitIndex, L7_uint32 vlanNum,
                                     L7_uint32 direction, L7_ACL_VLAN_DIR_LIST_t *listInfo)
{
  return aclVlanDirAclListGet(vlanNum, direction, listInfo);
}

/*********************************************************************
*
* @purpose  Gets a list of vlans to which an ACL is assigned in the
*           specified direction
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum      the access-list identifying number
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
L7_RC_t usmDbQosAclAssignedVlanDirListGet(L7_uint32 UnitIndex, L7_uint32 aclnum,
                                          L7_uint32 direction, L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList)
{
  return aclAssignedVlanDirListGet(aclnum, direction, vlanList);
}

/*********************************************************************
*
* @purpose  To check if an vlan and direction is in use by any access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE, if the vlan,dir has at least one ACL attached
* @returns  L7_FALSE, if the vlan,dir has no ACL attached
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL usmDbQosAclIsVlanInUse(L7_uint32 UnitIndex, L7_uint32 vlanNum,
                               L7_uint32 direction)
{
  return aclIsVlanInUse(vlanNum, direction);
}

/*********************************************************************
* @purpose  Check if the specified Internal Interface Number is in
*           use by the DiffServ component
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    direction  @b{(input)} Interface direction       
*                                  (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments None
*       
* @end
*********************************************************************/
L7_BOOL usmDbQosAclIsDiffServIntfInUse(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                       L7_uint32 direction)
{
  return aclIsDiffServIntfInUse(intIfNum, direction);
}

/*************************************************************************
* @purpose  Get the maximum number of ACLs that can be configured
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pMax        @b{(output)} Pointer to the maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosAclMaxNumGet(L7_uint32 UnitIndex, L7_uint32 *pMax)
{
  return aclMaxNumGet(pMax);
}

/*************************************************************************
* @purpose  Get the current number of ACLs configured
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pCurr       @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbQosAclCurrNumGet(L7_uint32 UnitIndex, L7_uint32 *pCurr)
{
  return aclCurrNumGet(pCurr);
}

/*********************************************************************
*
* @purpose check to see if intIfNum is a valid ACL Interface
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b((input)) Interface Number
*
* @returns L7_TRUE     If valid interface
* @returns L7_FALSE    If not valid interface 
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_BOOL usmDbQosAclIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return aclIsValidIntf(intIfNum);
}

/*********************************************************************
*
* @purpose check to see if vlanNum is a valid ACL VLAN
*
* @param UnitIndex   @b{(input)} System unit number
* @param vlanNum       @b((input)) VLAN Number
*
* @returns L7_TRUE     If valid VLAN
* @returns L7_FALSE    If not valid VLAN
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbQosAclIsValidVlan(L7_uint32 UnitIndex, L7_uint32 vlanNum)
{
  return aclIsValidVlan(vlanNum);
}

/*********************************************************************
*
* @purpose Get the ACL trap flag
*          
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclTrapFlagGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = trapMgrGetTrapAcl();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the ACL trap flag
*          
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclTrapFlagSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  trapMgrSetTrapAcl(val);
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  To set the assigned time range name for an access list rule.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uchar8  *timeRangeName time range is a alphanumeric string
* identifying the time range name associated to the rule
*
* @returns  L7_SUCCESS, if assigned queue id is added
* @returns  L7_FAILURE, if invalid queue id or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbQosAclRuleTimeRangeNameAdd(L7_uint32 UnitIndex, L7_uint32 aclnum,
                                        L7_uint32 rulenum, L7_uchar8 *timeRangeName)
{
  return aclRuleTimeRangeNameAdd(aclnum,rulenum,timeRangeName);
}
/*********************************************************************
*
* @purpose  To get the assigned time range name from an existing access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_char8   *timeRangeName  @b{(output)}  the time-range name is a string   
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
L7_RC_t usmDbQosAclRuleTimeRangeNameGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum, L7_uchar8 *timeRangeName)
{
  return aclRuleTimeRangeNameGet(aclIndex, rulenum, timeRangeName);
}
/*********************************************************************
*
* Purpose  To get the rule status of access list rule entry.
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
L7_RC_t usmDbQosAclRuleStatusGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *status)
{
   return aclRuleStatusGet(aclIndex, rulenum, status);
}

