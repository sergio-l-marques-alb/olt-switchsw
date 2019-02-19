/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    usmdb_qos_acl_api.h
* @purpose     ACL APIs
* @component   QoS
* @comments    none
* @create      6/17/2002
* @author      djohnson
* @end
*             
**********************************************************************/
#ifndef _USMDB_QOS_ACL_API_H_
#define _USMDB_QOS_ACL_API_H_

#include "l7_common.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "acl_api.h"


/* User Keyword Translations for IP DSCP Values */
#define L7_USMDB_ACL_IP_DSCP_AF11     10
#define L7_USMDB_ACL_IP_DSCP_AF12     12
#define L7_USMDB_ACL_IP_DSCP_AF13     14
#define L7_USMDB_ACL_IP_DSCP_AF21     18
#define L7_USMDB_ACL_IP_DSCP_AF22     20
#define L7_USMDB_ACL_IP_DSCP_AF23     22
#define L7_USMDB_ACL_IP_DSCP_AF31     26
#define L7_USMDB_ACL_IP_DSCP_AF32     28
#define L7_USMDB_ACL_IP_DSCP_AF33     30
#define L7_USMDB_ACL_IP_DSCP_AF41     34
#define L7_USMDB_ACL_IP_DSCP_AF42     36
#define L7_USMDB_ACL_IP_DSCP_AF43     38
#define L7_USMDB_ACL_IP_DSCP_BE       0
#define L7_USMDB_ACL_IP_DSCP_CS0      0
#define L7_USMDB_ACL_IP_DSCP_CS1      8
#define L7_USMDB_ACL_IP_DSCP_CS2      16
#define L7_USMDB_ACL_IP_DSCP_CS3      24
#define L7_USMDB_ACL_IP_DSCP_CS4      32
#define L7_USMDB_ACL_IP_DSCP_CS5      40
#define L7_USMDB_ACL_IP_DSCP_CS6      48
#define L7_USMDB_ACL_IP_DSCP_CS7      56
#define L7_USMDB_ACL_IP_DSCP_EF       46

/* assign queue id limits */
#define L7_USMDB_ACL_ASSIGN_QUEUE_ID_MIN        L7_ACL_ASSIGN_QUEUE_ID_MIN
#define L7_USMDB_ACL_ASSIGN_QUEUE_ID_MAX        L7_ACL_ASSIGN_QUEUE_ID_MAX

/* redirect interface number limits */
#define L7_USMDB_ACL_REDIRECT_INTF_MIN          1
#define L7_USMDB_ACL_REDIRECT_INTF_MAX          (L7_MAX_INTERFACE_COUNT-1)

/* mirror interface number limits */
#define L7_USMDB_ACL_MIRROR_INTF_MIN            1
#define L7_USMDB_ACL_MIRROR_INTF_MAX            (L7_MAX_INTERFACE_COUNT-1)


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
extern L7_RC_t usmDbQosAclNamedIndexNextFree(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                             L7_uint32 *pAclIndex);

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
extern L7_RC_t usmDbQosAclNamedIndexMinMaxGet(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                              L7_uint32 *pMin, L7_uint32 *pMax);

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
extern L7_RC_t usmDbQosAclNamedIndexGetFirst(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                             L7_uint32 *pAclIndex);

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
extern L7_RC_t usmDbQosAclNamedIndexGetNext(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                            L7_uint32 aclIndex, L7_uint32 *pAclIndex);

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
extern L7_RC_t usmDbQosAclNamedIndexCheckValid(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                               L7_uint32 aclIndex);

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
extern L7_RC_t usmDbQosAclNamedIndexRangeCheck(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                               L7_uint32 aclIndex);

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
extern L7_RC_t usmDbQosAclNumGetFirst(L7_uint32 UnitIndex, L7_uint32 *aclnum);

/*********************************************************************
*
* @purpose  Given aclnum, get the next access list ID
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the current access-list ID
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
extern L7_RC_t usmDbQosAclNumGetNext(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                     L7_uint32 *next);

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
extern L7_RC_t usmDbQosAclNumCheckValid(L7_uint32 UnitIndex, L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the supported IPv4 number ranges
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
extern L7_RC_t usmDbQosAclNumRangeCheck(L7_uint32 UnitIndex, L7_uint32 aclnum);

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
extern L7_RC_t usmDbAclCommonNameToIndex(L7_uint32 UnitIndex, L7_ACL_TYPE_t aclType,
                                         L7_uchar8 *pName, L7_uint32 *pAclNum);

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
extern L7_RC_t usmDbQosAclCommonNameStringGet(L7_uint32 UnitIndex, L7_uint32 aclnum,
                                              L7_ACL_TYPE_t *aclType, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To create a new access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    aclnum     @b{(input)} the access-list identifying number
*
* @returns  L7_SUCCESS, if the access-list is added
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list already exists
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclCreate(L7_uint32 UnitIndex, L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  To delete an existing access list.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    aclnum     @b{(input)} the access-list identifying number
*
* @returns  L7_SUCCESS, if the access-list is deleted
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclDelete(L7_uint32 UnitIndex, L7_uint32 aclnum);

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
extern L7_RC_t usmDbQosAclNameAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uchar8 *name);

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
extern L7_RC_t usmDbQosAclNameChange(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uchar8 *newname);

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
extern L7_RC_t usmDbQosAclNameGet(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uchar8 *name);

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
extern L7_RC_t usmDbQosAclNameStringCheck(L7_uint32 UnitIndex, L7_uchar8 *name);

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
extern L7_RC_t usmDbQosAclNameToIndex(L7_uint32 UnitIndex, L7_uchar8 *name, L7_uint32 *aclnum);

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
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclRuleActionAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                        L7_uint32 rulenum, L7_uint32 action);

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
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclRuleRemove(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                     L7_uint32 rulenum);

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
extern L7_RC_t usmDbQosAclRuleAssignQueueIdAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                               L7_uint32 rulenum, L7_uint32 queueId);

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
extern L7_RC_t usmDbQosAclRuleRedirectIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                              L7_uint32 rulenum, L7_uint32 intIfNum);

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
extern L7_RC_t usmDbQosAclRuleMirrorIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                            L7_uint32 rulenum, L7_uint32 intIfNum);

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
* @returns  L7_NOT_SUPPORTED, if logging feature is not supported
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclRuleLoggingAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                         L7_uint32 rulenum, L7_BOOL logFlag);

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
extern L7_RC_t usmDbQosAclRuleDstIpMaskAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                           L7_uint32 rulenum,
                                           L7_uint32 ipAddr, L7_uint32 mask);

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
extern L7_RC_t usmDbQosAclRuleDstIpv6AddrAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                             L7_uint32 rulenum, L7_in6_prefix_t *addr6);

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
extern L7_RC_t usmDbQosAclRuleDstL4PortRangeAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                                L7_uint32 rulenum,
                                                L7_uint32 startport, L7_uint32 endport);

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
extern L7_RC_t usmDbQosAclRuleDstL4PortAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                           L7_uint32 rulenum, L7_uint32 port);

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
extern L7_RC_t usmDbQosAclRuleEveryAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                       L7_uint32 rulenum, L7_BOOL match);

/*********************************************************************
*
* @purpose  Gets the match-all (every) status for a rule
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
extern L7_RC_t usmDbQosAclRuleEveryGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                       L7_uint32 rulenum, L7_BOOL *every);

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
extern L7_RC_t usmDbQosAclRuleIPDscpAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                        L7_uint32 rulenum, L7_uint32 dscpval);

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
extern L7_RC_t usmDbQosAclRuleIPPrecedenceAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                              L7_uint32 rulenum, L7_uint32 precedenceval);

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
extern L7_RC_t usmDbQosAclRuleIPTosAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                       L7_uint32 rulenum, 
                                       L7_uint32 tosbits, L7_uint32 tosmask);

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
extern L7_RC_t usmDbQosAclRuleIpv6FlowLabelAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                               L7_uint32 rulenum, L7_uint32 flowlbl);

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
extern L7_RC_t usmDbQosAclRuleProtocolAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                          L7_uint32 rulenum, L7_uint32 protocol);

/*********************************************************************
*
* @purpose  To add the source ip address and net mask.
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
extern L7_RC_t usmDbQosAclRuleSrcIpMaskAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                           L7_uint32 rulenum,
                                           L7_uint32 ipAddr, L7_uint32 mask);

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
extern L7_RC_t usmDbQosAclRuleSrcIpv6AddrAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                             L7_uint32 rulenum, L7_in6_prefix_t *addr6);

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
extern L7_RC_t usmDbQosAclRuleSrcL4PortAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                           L7_uint32 rulenum, L7_uint32 port);

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
extern L7_RC_t usmDbQosAclRuleSrcL4PortRangeAdd(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                                L7_uint32 rulenum,
                                                L7_uint32 startport, L7_uint32 endport);

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
extern L7_RC_t usmDbQosAclRuleGetFirst(L7_uint32 UnitIndex, L7_uint32 aclnum, L7_uint32 *rule);

/*********************************************************************
*
* @purpose  Get the next access list rule given an ACL ID
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum   the access-list ID
* @param    L7_uint32  rule     the current rule ID
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
extern L7_RC_t usmDbQosAclRuleGetNext(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                      L7_uint32 rule, L7_uint32 *next);

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
extern L7_RC_t usmDbQosAclRuleCheckValid(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                         L7_uint32 rulenum);

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
extern L7_RC_t usmDbQosAclRuleNumRangeCheck(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                            L7_uint32 rulenum);

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
extern L7_RC_t usmDbQosAclIntfDirCheckValid(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                            L7_uint32 intf, L7_uint32 direction);

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
                                     L7_uint32 vlan, L7_uint32 direction);

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
extern L7_BOOL usmDbQosAclIsFieldConfigured(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                            L7_uint32 rule, L7_uint32 field);

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
extern L7_RC_t usmDbQosAclRuleActionGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                        L7_uint32 rulenum, L7_uint32 *action);

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
extern L7_RC_t usmDbQosAclRuleAssignQueueIdGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                               L7_uint32 rulenum, L7_uint32 *queueId);

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
extern L7_RC_t usmDbQosAclRuleRedirectIntfGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                              L7_uint32 rulenum, L7_uint32 *intIfNum);

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
extern L7_RC_t usmDbQosAclRuleMirrorIntfGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                            L7_uint32 rulenum, L7_uint32 *intIfNum);

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
extern L7_RC_t usmDbQosAclRuleLoggingGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                         L7_uint32 rulenum, L7_BOOL *logFlag);

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
extern L7_RC_t usmDbQosAclRuleProtocolGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                          L7_uint32 rulenum, L7_uint32 *protocol);

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
extern L7_RC_t usmDbQosAclRuleSrcIpMaskGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                           L7_uint32 rulenum,
                                           L7_uint32 *ipAddr, L7_uint32 *mask);

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
                                      L7_uint32 rulenum, L7_in6_prefix_t *addr6);

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
extern L7_RC_t usmDbQosAclRuleSrcL4PortGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                           L7_uint32 rulenum, L7_uint32 *port);

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
extern L7_RC_t usmDbQosAclRuleSrcL4PortRangeGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                                L7_uint32 rulenum,
                                                L7_uint32 *startport, L7_uint32 *endport);

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
extern L7_RC_t usmDbQosAclRuleDstIpMaskGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                           L7_uint32 rulenum,
                                           L7_uint32 *ipAddr, L7_uint32 *mask);

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
extern L7_RC_t usmDbQosAclRuleDstIpv6AddrGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                             L7_uint32 rulenum, L7_in6_prefix_t *addr6);

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
extern L7_RC_t usmDbQosAclRuleDstL4PortGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                           L7_uint32 rulenum, L7_uint32 *port);

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
extern L7_RC_t usmDbQosAclRuleDstL4PortRangeGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                                L7_uint32 rulenum,
                                                L7_uint32 *startport, L7_uint32 *endport);

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
extern L7_RC_t usmDbQosAclRuleIPDscpGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                        L7_uint32 rulenum, L7_uint32 *dscpval);

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
extern L7_RC_t usmDbQosAclRuleIPPrecedenceGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                              L7_uint32 rulenum, L7_uint32 *precedenceval);

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
extern L7_RC_t usmDbQosAclRuleLoggingAllowed(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                             L7_uint32 rulenum);

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
extern L7_RC_t usmDbQosAclRuleIPTosGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                       L7_uint32 rulenum, 
                                       L7_uint32 *tosbits, L7_uint32 *tosmask);

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
extern L7_RC_t usmDbQosAclRuleIpv6FlowLabelGet(L7_uint32 UnitIndex, L7_uint32 aclnum, 
                                               L7_uint32 rulenum, L7_uint32 *flowlbl);

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
                                         L7_uint32 seqNum);

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
extern L7_RC_t usmDbQosAclInterfaceDirectionRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                                   L7_uint32 direction, L7_uint32 aclnum);

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
extern L7_RC_t usmDbQosAclIntfDirGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                     L7_uint32 direction);

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
extern L7_RC_t usmDbQosAclIntfDirGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 direction,
                                         L7_uint32 *nextIntIfNum, L7_uint32 *nextDirection);

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
extern L7_RC_t usmDbQosAclIntfDirSequenceGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                             L7_uint32 direction, L7_uint32 seqNum);

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
extern L7_RC_t usmDbQosAclIntfDirSequenceGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                                 L7_uint32 direction, L7_uint32 seqNum,
                                                 L7_uint32 *nextSeqNum);

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
extern L7_RC_t usmDbQosAclIntfDirAclListGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 direction,
                                            L7_ACL_INTF_DIR_LIST_t *listInfo);

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
extern L7_RC_t usmDbQosAclAssignedIntfDirListGet(L7_uint32 UnitIndex, L7_uint32 aclnum,
                                                 L7_uint32 direction,
                                                 L7_ACL_ASSIGNED_INTF_LIST_t *intfList);

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
extern L7_BOOL usmDbQosAclIsInterfaceInUse(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                           L7_uint32 direction);

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
                                    L7_uint32 direction, L7_uint32 aclnum, L7_uint32 seqNum);

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
                                       L7_uint32 direction, L7_uint32 aclnum);

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
                              L7_uint32 direction);

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
                                  L7_uint32 direction, L7_uint32 *nextVlanNum, L7_uint32 *nextDirection);

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
L7_RC_t usmDbQosAclVlanIdGetNext(L7_uint32 UnitIndex, L7_uint32 vlanNum, L7_uint32 *nextVlanNum);

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
                                       L7_uint32 direction, L7_uint32 seqNum);

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
                                          L7_uint32 direction, L7_uint32 seqNum, L7_uint32 *nextSeqNum);

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
                                     L7_uint32 direction, L7_ACL_VLAN_DIR_LIST_t *listInfo);

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
                                          L7_uint32 direction, L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList);

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
                               L7_uint32 direction);

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
extern L7_BOOL usmDbQosAclIsDiffServIntfInUse(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                              L7_uint32 direction);

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
extern L7_RC_t usmDbQosAclMaxNumGet(L7_uint32 UnitIndex, L7_uint32 *pMax);

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
extern L7_RC_t usmDbQosAclCurrNumGet(L7_uint32 UnitIndex, L7_uint32 *pCurr);

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
extern L7_BOOL usmDbQosAclIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

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
L7_BOOL usmDbQosAclIsValidVlan(L7_uint32 UnitIndex, L7_uint32 vlanNum);

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
extern L7_RC_t usmDbQosAclTrapFlagGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the ACL trap flag
*          
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 flag       @b((input)) Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclTrapFlagSet(L7_uint32 UnitIndex, L7_uint32 val);

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
extern L7_RC_t usmDbQosAclMacIndexNext(L7_uint32 UnitIndex, L7_uint32 *pIndex);

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
extern L7_RC_t usmDbQosAclMacIndexMinMaxGet(L7_uint32 UnitIndex, L7_uint32 *pMin, L7_uint32 *pMax);

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
* @returns  L7_TABLE_IS_FULL, if maximum number of ACLs already created
*
* @comments The MAC ACL name must be set after the access-list is created.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclMacCreate(L7_uint32 UnitIndex, L7_uint32 aclIndex);

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
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclMacDelete(L7_uint32 UnitIndex, L7_uint32 aclIndex);

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
extern L7_RC_t usmDbQosAclMacNameAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uchar8 *name);

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
extern L7_RC_t usmDbQosAclMacNameChange(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uchar8 *newname);

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
extern L7_RC_t usmDbQosAclMacNameGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uchar8 *name);

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
extern L7_RC_t usmDbQosAclMacNameStringCheck(L7_uint32 UnitIndex, L7_uchar8 *name);

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
extern L7_RC_t usmDbQosAclMacNameToIndex(L7_uint32 UnitIndex, L7_uchar8 *name, L7_uint32 *aclIndex);

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
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments Creates a new rule if the rulenum does not exist.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclMacRuleActionAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                           L7_uint32 rulenum, L7_uint32 action);

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
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclMacRuleRemove(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum);

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
extern L7_RC_t usmDbQosAclMacRuleAssignQueueIdAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                  L7_uint32 rulenum, L7_uint32 queueId);

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
extern L7_RC_t usmDbQosAclMacRuleRedirectIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                 L7_uint32 rulenum, L7_uint32 intIfNum);

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
extern L7_RC_t usmDbQosAclMacRuleMirrorIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                               L7_uint32 rulenum, L7_uint32 intIfNum);

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
* @returns  L7_NOT_SUPPORTED, if logging feature is not supported
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclMacRuleLoggingAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                            L7_uint32 rulenum, L7_BOOL logFlag);

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
extern L7_RC_t usmDbQosAclMacRuleCosAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                        L7_uint32 rulenum, L7_uint32 cos);

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
extern L7_RC_t usmDbQosAclMacRuleCos2Add(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                         L7_uint32 rulenum, L7_uint32 cos2);

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
extern L7_RC_t usmDbQosAclMacRuleDstMacAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                           L7_uint32 rulenum,
                                           L7_uchar8 *dstmac, L7_uchar8 *dstmask);

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
extern L7_RC_t usmDbQosAclMacRuleEtypeKeyAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                             L7_uint32 rulenum, L7_QOS_ETYPE_KEYID_t keyid,
                                             L7_uint32 value);

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
extern L7_RC_t usmDbQosAclMacRuleEveryAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                          L7_uint32 rulenum, L7_BOOL every);

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
extern L7_RC_t usmDbQosAclMacRuleSrcMacAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                           L7_uint32 rulenum, 
                                           L7_uchar8 *srcmac, L7_uchar8 *srcmask);

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
extern L7_RC_t usmDbQosAclMacRuleVlanIdAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                           L7_uint32 rulenum, L7_uint32 vlan);

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
extern L7_RC_t usmDbQosAclMacRuleVlanIdRangeAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                L7_uint32 rulenum, 
                                                L7_uint32 startvlan, L7_uint32 endvlan);

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
extern L7_RC_t usmDbQosAclMacRuleVlanId2Add(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                            L7_uint32 rulenum, L7_uint32 vlan2);

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
extern L7_RC_t usmDbQosAclMacRuleVlanId2RangeAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                 L7_uint32 rulenum, 
                                                 L7_uint32 startvlan2, L7_uint32 endvlan2);

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
extern L7_RC_t usmDbQosAclMacInterfaceDirectionAdd(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                                   L7_uint32 direction, L7_uint32 aclIndex,
                                                   L7_uint32 seqNum);

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
* @returns  L7_SUCCESS, if intIfNum is removed from the access-list
* @returns  L7_FAILURE, if invalid identifying number or 
*                       internal interface number does not exist
* @returns  L7_ERROR, if intIfNum is in use by another access-list
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclMacInterfaceDirectionRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                                      L7_uint32 direction, L7_uint32 aclIndex);

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
                                        L7_uint32 seqNum );

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
                                           L7_uint32 aclIndex );

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
extern L7_RC_t usmDbQosAclMacIndexGetFirst(L7_uint32 UnitIndex, L7_uint32 *aclIndex);

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
extern L7_RC_t usmDbQosAclMacIndexGetNext(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 *next);

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
extern L7_RC_t usmDbQosAclMacIndexCheckValid(L7_uint32 UnitIndex, L7_uint32 aclIndex);

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
extern L7_RC_t usmDbQosAclMacIndexRangeCheck(L7_uint32 UnitIndex, L7_uint32 aclIndex);

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
extern L7_RC_t usmDbQosAclMacRuleGetFirst(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 *rulenum);

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
* @returns  L7_FAILURE, if rulenum is the last valid rulenum for this MAC ACL
* @returns  L7_ERROR,   if aclIndex or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbQosAclMacRuleGetNext(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                         L7_uint32 rulenum, L7_uint32 *next);

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
extern L7_RC_t usmDbQosAclMacRuleCheckValid(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum);

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
extern L7_RC_t usmDbQosAclMacRuleNumRangeCheck(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 rulenum);

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
extern L7_RC_t usmDbQosMacAclIntfDirCheckValid(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                               L7_uint32 intf, L7_uint32 direction);

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
                                         L7_uint32 direction );
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
extern L7_BOOL usmDbQosAclMacIsFieldConfigured(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                               L7_uint32 rule, L7_uint32 field);

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
extern L7_RC_t usmDbQosAclMacAssignedIntfDirListGet(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                                    L7_uint32 direction,
                                                    L7_ACL_ASSIGNED_INTF_LIST_t *intfList);

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
extern L7_RC_t usmDbQosAclMacRuleActionGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                           L7_uint32 rulenum, L7_uint32 *action);

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
extern L7_RC_t usmDbQosAclMacRuleAssignQueueIdGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                  L7_uint32 rulenum, L7_uint32 *queueId);

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
extern L7_RC_t usmDbQosAclMacRuleRedirectIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                 L7_uint32 rulenum, L7_uint32 *intIfNum);

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
extern L7_RC_t usmDbQosAclMacRuleMirrorIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                               L7_uint32 rulenum, L7_uint32 *intIfNum);

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
extern L7_RC_t usmDbQosAclMacRuleLoggingGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                            L7_uint32 rulenum, L7_BOOL *logFlag);

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
extern L7_RC_t usmDbQosAclMacRuleCosGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                        L7_uint32 rulenum, L7_uint32 *cos);

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
extern L7_RC_t usmDbQosAclMacRuleCos2Get(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                         L7_uint32 rulenum, L7_uint32 *cos2);

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
extern L7_RC_t usmDbQosAclMacRuleDstMacAddrMaskGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                   L7_uint32 rulenum,
                                                   L7_uchar8 *dstmac, L7_uchar8 *dstmask);

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
extern L7_RC_t usmDbQosAclMacRuleEtypeKeyGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                             L7_uint32 rulenum, L7_QOS_ETYPE_KEYID_t *keyid,
                                             L7_uint32 *value);

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
extern L7_RC_t usmDbQosAclMacRuleEveryGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                          L7_uint32 rulenum, L7_BOOL *every);

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
extern L7_RC_t usmDbQosAclMacRuleSrcMacAddrMaskGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                   L7_uint32 rulenum,
                                                   L7_uchar8 *srcmac, L7_uchar8 *srcmask);

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
extern L7_RC_t usmDbQosAclMacRuleVlanIdGet(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                           L7_uint32 rulenum, L7_uint32 *vlan);

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
extern L7_RC_t usmDbQosAclMacRuleVlanIdRangeGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                L7_uint32 rulenum, 
                                                L7_uint32 *startvlan, L7_uint32 *endvlan);

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
extern L7_RC_t usmDbQosAclMacRuleVlanId2Get(L7_uint32 UnitIndex, L7_uint32 aclIndex,
                                            L7_uint32 rulenum, L7_uint32 *vlan2);

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
extern L7_RC_t usmDbQosAclMacRuleVlanId2RangeGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                                 L7_uint32 rulenum, 
                                                 L7_uint32 *startvlan2, L7_uint32 *endvlan2);

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
                                         L7_uint32 rulenum);

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
                                              L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList );

#endif /* USMDB_QOS_ACL_API_H*/
