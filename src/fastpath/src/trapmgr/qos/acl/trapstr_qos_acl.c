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
L7_char8 qosAclRuleLogEventNumIp_str[] =    "IP ACL %u, Rule %u, %s Match:  %s hit(s)";
L7_char8 qosAclRuleLogEventNameIp_str[] =    "IP ACL %s, Rule %u, %s Match:  %s hit(s)";
L7_char8 qosAclRuleLogEventNameIp6_str[] =    "IPv6 ACL %s, Rule %u, %s Match:  %s hit(s)";
L7_char8 qosAclMacRuleLogEvent_str[] = "MAC ACL %s, Rule %u, %s Match:  %s hit(s)";

L7_char8 qosAclRuleActionDeny_str[] = "Deny";
L7_char8 qosAclRuleActionPermit_str[] = "Permit";
L7_char8 qosAclRuleNotificationActivate_str[] = "L7_TIMERANGE_EVENT_ACTIVATE";
L7_char8 qosAclRuleNotificationDeactivate_str[] = "L7_TIMERANGE_EVENT_DEACTIVATE";
L7_char8 qosAclRuleNotificationDelete_str[] = "L7_TIMERANGE_EVENT_DELETE";
L7_char8 qosAclRuleInstallStatusFail_str[] = "Failure";
L7_char8 qosAclRuleInstallStatusSucc_str[] = "Success";

L7_char8 qosAclRuleTimeRangeEventNumIp_str[] =    "IP ACL %u, Rule %u, Action %s, Time Range %s, Time Range Notification %s, Installation status %s";
L7_char8 qosAclRuleTimeRangeEventNameIp_str[] =  "IP ACL %s, Rule %u, Action %s, Time Range %s, Time Range Notification %s, Installation status %s";
L7_char8 qosAclRuleTimeRangeEventNameIp6_str[] =    "IPv6 ACL %s, Rule %u, Action %s, Time Range %s, Time Range Notification %s, Installation status %s";
L7_char8 qosAclMacRuleTimeRangeEvent_str[] = "MAC ACL %s, Rule %u, Action %s, Time Range %s, Time Range Notification %s, Installation status %s";


