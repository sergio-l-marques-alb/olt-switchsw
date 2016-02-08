/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trapstr_qos.c
*
* @purpose Trap Manager QOS String File
*
* @component trapmgr
*
* @comments none
*
* @created 06/27/2005
*
* @author gpaussa
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "trapstr_qos_acl.h"

/* ACL */
L7_char8 qosAclRuleLogEvent_str[] =    "IP ACL %u, Rule %u, %s Match:  %s hit(s)";
L7_char8 qosAclMacRuleLogEvent_str[] = "MAC ACL %s, Rule %u, %s Match:  %s hit(s)";

L7_char8 qosAclRuleActionDeny_str[] = "Deny";
L7_char8 qosAclRuleActionPermit_str[] = "Permit";
