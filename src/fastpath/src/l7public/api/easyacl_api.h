/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename     easyacl_api.h
*
* @purpose      Contains Easy Access Control List offerings 
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

#ifndef INCLUDE_EASYACL_API_H
#define INCLUDE_EASYACL_API_H

#include "l7_common.h"
#include "acl_exports.h"
#include "nimapi.h"
#include "l3_commdefs.h"
#include "l3_addrdefs.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "acl_api.h"
#include "comm_mask.h"


typedef struct _easyacl_ip_ {

    L7_BOOL        ipMaskValid;
    L7_BOOL        startPortValid;
    L7_BOOL        endPortValid;

    L7_uint32    ipAddr;
    L7_uint32    ipMask;
    L7_ushort16    startPort;
    L7_ushort16    endPort;

} easyacl_ip_t;

typedef struct _easyacl_ipacl_ {

    L7_BOOL         dstIpValid;
    L7_BOOL         srcIpValid;
    easyacl_ip_t    srcIp;
    easyacl_ip_t    dstIp;

} easyacl_ipacl_t;

typedef struct _easyacl_macacl_ {

    L7_BOOL     srcMacValid;
    L7_BOOL     dstMacValid;
    L7_uchar8    srcMac[L7_MAC_ADDR_LEN];
    L7_uchar8    dstMac[L7_MAC_ADDR_LEN];

} easyacl_macacl_t;

typedef struct _easyacl_rule {

    L7_BOOL         isValid;

    L7_char8        name[L7_ACL_NAME_LEN_MAX];

    L7_ushort16        priority;
    L7_ACL_ACTION_t action;    /* permit / drop action */

    L7_ACL_TYPE_t    type;   /* IP / MAC ACLs */

    L7_BOOL         anyPacket;

    union {
        easyacl_ipacl_t  ipAcl;
        easyacl_macacl_t macAcl;
    }acl;

    L7_uint32       aclId;

    L7_BOOL         onAllIntf;  /* True if to be applied on all interfaces */
                                /* if true, the following member doesn't hold
                                 * any valid value.
                                 * */
    L7_INTF_MASK_t  intf;

} easyacl_rule_t;

/* This function imports rules from the XML file - fileName */
L7_RC_t easyAclImport(L7_char8 *fileName);
void easyAclReadXML(L7_char8 *fileName);
void easyAclWriteXML(L7_char8 *fileName);
L7_RC_t easyAclApply();
/* This function exports rules to the XML file - fileName */
L7_RC_t easyAclExport(L7_char8 *fileName);

/* This function returns the rule corresponding to a name */
L7_RC_t easyAclRuleByNameGet(L7_char8 *name, L7_uint32 *ruleId);

/* 
 * This function modifies an existing rule
 * This function always deletes any existing rule and applies the
 * new rule 
 */

/* L7_RC_t easyAclRuleModify(easyacl_rule_t *rule); */

/* This function creates an ACL of the specific type */
L7_RC_t easyAclRuleCreate(L7_char8 *name, L7_ACL_TYPE_t type, L7_uint32 *ruleId);

/* this function associates/retrieves action for a given rule */

L7_RC_t easyAclRuleActionGet(L7_uint32 ruleId, L7_ACL_ACTION_t *action);
L7_RC_t easyAclRuleActionSet(L7_uint32 ruleId, L7_ACL_ACTION_t action);

L7_RC_t easyAclRulePriorityGet(L7_uint32 ruleId, L7_ushort16 *priority);
L7_RC_t easyAclRulePrioritySet(L7_uint32 ruleId, L7_ushort16 priority);

/* this function applies a given rule on an interface */
L7_RC_t easyAclRuleIntfAdd(L7_uint32 ruleId, L7_ushort16 intfIndx);
/*L7_RC_t easyAclRuleIntfAddAll(L7_uint32 ruleId); */
L7_RC_t easyAclRuleIntfRemove(L7_uint32 ruleId, L7_ushort16 intfIndx);
/*L7_RC_t easyAclRuleIntfRemoveAll(L7_uint32 ruleId); */
L7_RC_t easyAclRuleIntfListGet(L7_uint32 ruleId, L7_INTF_MASK_t *intfMask);

/* below mentioned are the get/set routines for the IP ACL fields */ 

L7_RC_t easyAclRuleSrcIpAddrMaskGet(L7_uint32 ruleId, 
        L7_uint32 *srcIp, L7_uint32 *srcMask);
L7_RC_t easyAclRuleSrcIpAddrMaskSet(L7_uint32 ruleId, 
        L7_uint32 srcIp, L7_uint32 srcMask);
L7_RC_t easyAclRuleSrcPortGet(L7_uint32 ruleId, L7_ushort16 *srcPort);
L7_RC_t easyAclRuleSrcPortSet(L7_uint32 ruleId, L7_ushort16 srcPort);
L7_RC_t easyAclRuleSrcPortRangeGet(L7_uint32 ruleId, 
        L7_ushort16 *srcStartPort, L7_ushort16 *srcEndPort);
L7_RC_t easyAclRuleSrcPortRangeSet(L7_uint32 ruleId, 
        L7_ushort16 srcStartPort, L7_ushort16 srcEndPort);

L7_RC_t easyAclRuleDstIpAddrMaskGet(L7_uint32 ruleId, 
        L7_uint32 *dstIp, L7_uint32 *dstMask);
L7_RC_t easyAclRuleDstIpAddrMaskSet(L7_uint32 ruleId, 
        L7_uint32 dstIp, L7_uint32 dstMask);
L7_RC_t easyAclRuleDstPortGet(L7_uint32 ruleId, L7_ushort16 *dstPort);
L7_RC_t easyAclRuleDstPortSet(L7_uint32 ruleId, L7_ushort16 dstPort);
L7_RC_t easyAclRuleDstPortRangeGet(L7_uint32 ruleId, 
        L7_ushort16 *dstStartPort, L7_ushort16 *dstEndPort);
L7_RC_t easyAclRuleDstPortRangeSet(L7_uint32 ruleId, 
        L7_ushort16 dstStartPort, L7_ushort16 dstEndPort);

/* Below mentioned are the get/set routines for the MAC ACL fields */

L7_RC_t easyAclRuleSrcMacAddrGet(L7_uint32 ruleId, L7_uchar8 *srcMac);
L7_RC_t easyAclRuleSrcMacAddrSet(L7_uint32 ruleId, L7_uchar8 *srcMac);

L7_RC_t easyAclRuleDstMacAddrGet(L7_uint32 ruleId, L7_uchar8 *dstMac);
L7_RC_t easyAclRuleDstMacAddrSet(L7_uint32 ruleId, L7_uchar8 *dstMac);

L7_RC_t easyAclCountGet(L7_ushort16 *count);

L7_RC_t easyAclRuleGetFirst(L7_uint32 *id);

L7_RC_t easyAclRuleGetNext(L7_uint32 first, L7_uint32 *next);

L7_RC_t easyAclRuleNameGet(L7_uint32 id, L7_char8 *name);

L7_RC_t easyAclRuleNameSet(L7_uint32 id, L7_char8 *name);

L7_RC_t easyAclRuleTypeGet(L7_uint32 ruleId, L7_ACL_TYPE_t *type);

L7_RC_t easyAclRuleMatchEveryGet(L7_uint32 ruleId, L7_BOOL *match);

L7_RC_t easyAclRuleMatchEverySet(L7_uint32 ruleId, L7_BOOL match);

L7_RC_t easyACLRulesClear( );

L7_RC_t easyACLCopy(easyacl_rule_t *rules);

L7_RC_t easyACLSync(easyacl_rule_t *rules);

L7_RC_t easyAclRuleDelete(L7_uint32 ruleId);


#endif /* INCLUDE_EASYACL_API_H */
