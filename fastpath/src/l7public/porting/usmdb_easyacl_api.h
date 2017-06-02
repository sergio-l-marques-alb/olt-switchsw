/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename     usmdb_easyacl_api.h
 *
 * @purpose      Contains Easy Access Control List API offerings
 *
 * @component    Access Control List
 *
 * @comments 
 *
 * @create       07/21/2007
 *
 * @author       bviswanath
 *
 * @end
 *
 **********************************************************************/
#ifndef USMDB_EASYACL_API_H
#define USMDB_EASYACL_API_H

#include "comm_mask.h"
#include "acl_exports.h"

/*********************************************************************
 * @purpose  This function imports rules from the XML file 
 *
 * @param    fileName    @b{(input)}  XML file to import from
 *
 * @returns  L7_SUCCESS
 * @returns  L7_TABLE_IS_FULL  ACL table is currently full
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclImport(L7_char8 *fileName);

/*********************************************************************
 * @purpose  This function exports rules to the XML file 
 *
 * @param    fileName    @b{(input)}  XML file to import from
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments File will be created if necessary
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclExport(L7_char8 *fileName);

/*********************************************************************
 * @purpose  This function imports rules from the XML file 
 *
 * @param    fileName    @b{(input)}  XML file to import from
 *
 * @returns  L7_SUCCESS
 * @returns  L7_TABLE_IS_FULL  ACL table is currently full
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclCountGet(L7_ushort16 *count) ;

L7_RC_t usmDbEasyAclRuleGetFirst(L7_uint32 *id);

L7_RC_t usmDbEasyAclRuleGetNext(L7_uint32 first, L7_uint32 *next);

L7_RC_t usmDbEasyAclRuleNameGet(L7_uint32 id, L7_char8 *name);

L7_RC_t usmDbEasyAclRuleNameSet(L7_uint32 id, L7_char8 *name);

L7_RC_t usmDbEasyAclRuleTypeGet(L7_uint32 id, L7_ACL_TYPE_t *type);

L7_RC_t usmDbEasyAclRuleMatchEveryGet(L7_uint32 id, L7_BOOL *match);

L7_RC_t usmDbEasyAclRuleMatchEverySet(L7_uint32 id, L7_BOOL match);

/*********************************************************************
 * @purpose  This function returns the rule corresponding to a name 
 *
 * @param    name    @b{(input)}  Name of the ACL
 * @param    ruleId  @b{(output)} Rule Id
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments File will be created if necessary
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleByNameGet(L7_char8 *name, L7_uint32 *ruleId);

/*********************************************************************
 * @purpose  This function creates a new ACL 
 *
 * @param    name    @b{(input)}  Name of the ACL
 * @param    type    @b{(input)}  Type of the ACL
 * @param    ruleId  @b{(output)} Rule Id
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleCreate(L7_char8 *name, L7_ACL_TYPE_t type, 
        L7_uint32 *ruleId);

/*********************************************************************
 * @purpose  This function retrieves the action for an ACL 
 *
 * @param    ruleId  @b{(input)}  Rule Id
 * @param    action  @b{(output)} Action
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleActionGet(L7_uint32 id, L7_ACL_ACTION_t *action);

/*********************************************************************
 * @purpose  This function sets the action for an ACL 
 *
 * @param    ruleId  @b{(input)}  Rule Id
 * @param    action  @b{(input)} Action
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleActionSet(L7_uint32 id, L7_ACL_ACTION_t action);

/*********************************************************************
 * @purpose  This function retrieves the priority for an ACL 
 *
 * @param    ruleId  @b{(input)}  Rule Id
 * @param    priority  @b{(output)} Priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRulePriorityGet(L7_uint32 id, L7_ushort16 *priority);

/*********************************************************************
 * @purpose  This function sets the priority for an ACL 
 *
 * @param    ruleId  @b{(input)}  Rule Id
 * @param    priority  @b{(input)} Priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRulePrioritySet(L7_uint32 id, L7_ushort16 priority);

/*********************************************************************
 * @purpose  this function applies a given rule on an interface
 *
 * @param    ruleId     @b{(input)} Rule Id
 * @param    intfIndx   @b{(input)} Interface Index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleIntfAdd(L7_uint32 id, L7_ushort16 intfIndx);

/*********************************************************************
 * @purpose  this function applies a given rule on an interface
 *
 * @param    ruleId     @b{(input)} Rule Id
 * @param    intfIndx   @b{(input)} Interface Index
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleIntfRemove(L7_uint32 id, L7_ushort16 intfIndx);
/*********************************************************************
 * @purpose  this function retrieves the list of interfaces on which
 *           a particular ACL is applied
 *
 * @param    ruleId     @b{(input)} Rule Id
 * @param    *intfMask   @b{(input)} Interface List
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleIntfListGet(L7_uint32 id, L7_INTF_MASK_t *intfMask);

/*********************************************************************
 * @purpose  This function retrieves the Src IP address for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    srcIp  @b{(output)} Source IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleSrcIpAddrMaskGet(L7_uint32 id, 
        L7_uint32 *srcIp, L7_uint32 *srcMask);

/*********************************************************************
 * @purpose  This function sets the Src IP address for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    srcIp  @b{(input)} Source IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleSrcIpAddrMaskSet(L7_uint32 id, 
        L7_uint32 srcIp, L7_uint32 srcMask);

/*********************************************************************
 * @purpose  This function retrieves the Dst IP address for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    srcIp  @b{(output)} Destination IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleDstIpAddrMaskGet(L7_uint32 id, 
        L7_uint32 *dstIp, L7_uint32 *dstMask);

/*********************************************************************
 * @purpose  This function sets the Destination IP address for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    srcIp  @b{(input)} Destination IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleDstIpAddrMaskSet(L7_uint32 id, 
        L7_uint32 dstIp, L7_uint32 dstMask);

/*********************************************************************
 * @purpose  This function retrieves the Source port Range for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    srcIp  @b{(output)} Destination IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleSrcPortRangeGet(L7_uint32 id, 
        L7_ushort16 *srcStartPort, L7_ushort16 *srcEndPort);

/*********************************************************************
 * @purpose  This function sets the Source port Range for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    srcIp  @b{(output)} Destination IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleSrcPortRangeSet(L7_uint32 id, 
        L7_ushort16 srcStartPort, L7_ushort16 srcEndPort);

/*********************************************************************
 * @purpose  This function retrieves the Destination port Range for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    dstIp  @b{(output)} Destination IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleDstPortRangeGet(L7_uint32 id, 
        L7_ushort16 *dstStartPort, L7_ushort16 *dstEndPort);

/*********************************************************************
 * @purpose  This function sets the Destination port Range for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    dstIp  @b{(output)} Destination IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleDstPortRangeSet(L7_uint32 id, 
        L7_ushort16 dstStartPort, L7_ushort16 dstEndPort);

/*********************************************************************
 * @purpose  This function retrieves the Source MAC for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    srcMac  @b{(output)} Source MAC address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleSrcMacAddrGet(L7_uint32 id, L7_uchar8 *srcMac);

/*********************************************************************
 * @purpose  This function Sets the Source MAC for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    srcMac  @b{(output)} Source MAC address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleSrcMacAddrSet(L7_uint32 id, L7_uchar8 *srcMac);

/*********************************************************************
 * @purpose  This function retrieves the Destination MAC for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    dstMac  @b{(output)} Destination MAC address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleDstMacAddrGet(L7_uint32 id, L7_uchar8 *dstMac);

/*********************************************************************
 * @purpose  This function Sets the Destination MAC for an ACL 
 *
 * @param    ruleId @b{(input)}  Rule Id
 * @param    dstMac  @b{(output)} Destination MAC address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE        all other failures
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEasyAclRuleDstMacAddrSet(L7_uint32 id, L7_uchar8 *dstMac);

L7_RC_t usmDbEasyAclRuleDelete(L7_uint32 ruleId);

#endif


