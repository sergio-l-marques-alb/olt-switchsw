/**
 * ptin_acl.h 
 *  
 * Created on: 2013/19/30 
 * Author: Joao Mateiro
 * 
 */

#ifndef PTIN_ACL_H_
#define PTIN_ACL_H_

#include "ptin_msghandler.h"
#include "usmdb_util_api.h"
#include "usmdb_qos_acl_api.h"

/**
 * Clean All ACL
 * 
 * @author joaom (11/07/2013)
 *
 */
void ptin_aclCleanAll(void);

/**
 * Create an IP ACL Rule
 * 
 * @author joaom (11/04/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpRuleConfig(msg_ip_acl_t *msgAcl, ACL_OPERATION_t operation);

/**
 * Apply an IP ACL to an interface or VLAN
 * 
 * @author joaom (11/04/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpApply(msg_apply_acl_t *msgAcl, ACL_OPERATION_t operation);


/**
 * Create an IPv6 ACL Rule
 * 
 * @author joaom (11/04/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpv6RuleConfig(msg_ipv6_acl_t *msgAcl, ACL_OPERATION_t operation);

/**
 * Apply an IPv6 ACL to an interface or VLAN
 * 
 * @author joaom (11/04/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpv6Apply(msg_apply_acl_t *msgAcl, ACL_OPERATION_t operation);

/**
 * Create a MAC ACL Rule
 * 
 * @author joaom (10/30/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclMacRuleConfig(msg_mac_acl_t *msgAcl, ACL_OPERATION_t operation);

/**
 * Apply a MAC ACL to an interface or VLAN
 * 
 * @author joaom (10/30/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclMacApply(msg_apply_acl_t *msgAcl, ACL_OPERATION_t operation);

/**
 * Create an ARP ACL Rule
 * 
 * @author mruas (02/16/2015)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclArpRuleConfig(msg_arp_acl_t *msgAcl, ACL_OPERATION_t operation);

/**
 * Apply an ARP ACL to a VLAN
 * 
 * @author mruas (02/16/2015)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclArpApply(msg_apply_acl_t *msgAcl, ACL_OPERATION_t operation);

#endif //PTIN_ACL_H_
