/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/acl/cli_show_running_config_acl.c
 *
 * @purpose show running config commands for the acl
 *
 * @component user interface
 *
 * @comments
 *
 * @create  18/08/2003
 *
 * @author  Samip
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "cli_web_exports.h"
#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_acl_api.h"
#endif

#include "comm_mask.h"


#ifdef L7_QOS_FLEX_PACKAGE_ACL

#include "acl_api.h"
#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "osapi_support.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"
#include "l3_comm_structs.h"
#include "cliutil_acl.h"
#include "config_script_api.h"
#include "clicommands_acl.h"
#include "cli_show_running_config.h"

static void cliRunningConfigIpv4AclInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 aclId);

/*********************************************************************
* @purpose  To print the running configuration of acl Info
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliRunningConfigAclInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  static L7_uint32 aclId;
  L7_uint32 val;

  if (usmDbQosAclTrapFlagGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_TRAP_ACL,pStrInfo_qos_AclTrapflags);
  }

  /* numbered IPv4 ACLs */
  if( usmDbQosAclNumGetFirst(unit, &aclId) == L7_SUCCESS)
  {
    do
    {
      cliRunningConfigIpv4AclInfo(ewsContext, unit, aclId);
    } while (usmDbQosAclNumGetNext(unit, aclId, &aclId) == L7_SUCCESS);
  }

  /* named IPv4 ACLs */
  if (usmDbQosAclNamedIndexGetFirst(unit, L7_ACL_TYPE_IP, &aclId) == L7_SUCCESS)
  {
    do
    {
      if (usmDbQosAclNameGet(unit, aclId, buf) == L7_SUCCESS)
      {
        /* enter named IPv4 ACL config mode */
        sprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, "ip access-list %s", buf);
        EWSWRITEBUFFER(ewsContext,stat);

        /* output permit/deny statments */
        cliRunningConfigIpv4AclInfo(ewsContext, unit, aclId);

        /* exit named IPv4 ACL config mode */
        osapiSnprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Exit);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    } while (usmDbQosAclNamedIndexGetNext(unit, L7_ACL_TYPE_IP, aclId, &aclId) == L7_SUCCESS);
  }

  return L7_SUCCESS;
}

void cliRunningConfigIpv4AclInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 aclId)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 dscpString[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 rulenum, ipMask;
  L7_char8 protVal[16];
  L7_uint32 val, val2, u, s, p;
  L7_char8 maskStr[16];
  L7_IP_ADDR_t ipAddr;
  L7_BOOL every, foundValidAction, foundValidCriterion, needTrailingCrlf = L7_FALSE;
  L7_BOOL srcIpAddrConfigured, srcIpMaskConfigured, dstIpAddrConfigured, dstIpMaskConfigured;
  L7_char8 timeRangeName[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbQosAclRuleGetFirst(unit, aclId, &rulenum) == L7_SUCCESS)
  {
    do
      /* this section is executed for both the first and successive times */
    {
      memset (stat, 0, sizeof(stat));
      foundValidAction = L7_FALSE;

      if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_ACTION) == L7_TRUE)
      {
        if (usmDbQosAclRuleActionGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
        {
          if (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) != L7_SUCCESS)
          {
            /* this is a numbered IPv4 access list (rather than a named IPv4 access list) */
            sprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_qos_AccessList_2, aclId);
          }

          if (val == L7_ACL_PERMIT )
          {
            strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Permit_1);
            foundValidAction = L7_TRUE;
          }
          else if (val == L7_ACL_DENY )
          {
            strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Deny_2);
            foundValidAction = L7_TRUE;
          }
        }
      }

      /* only process the remaining fields if a valid rule action was found */
      if (foundValidAction == L7_TRUE)
      {
        needTrailingCrlf = L7_TRUE;
        every = L7_FALSE;
        foundValidCriterion = L7_FALSE;
        srcIpAddrConfigured = srcIpMaskConfigured = dstIpAddrConfigured = dstIpMaskConfigured = L7_FALSE;

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_EVERY) == L7_TRUE)
        { /* if the match every is set to true */
          if (usmDbQosAclRuleEveryGet(unit, aclId, rulenum, &every) == L7_SUCCESS)
          {
            if (every == L7_TRUE)
            {
              foundValidCriterion = L7_TRUE;
              if (aclId < 100)  /*if standard ACL*/
              {
                strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_qos_AclEveryStr);
              }
              else
              {
                strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_qos_IpAnyAny);
              }
            }
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_PROTOCOL) == L7_TRUE)
        {
          if (usmDbQosAclRuleProtocolGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            memset (buf, 0, sizeof(buf));
            switch (val)
            {
            case L7_ACL_PROTOCOL_ICMP:
              sprintf(buf, "%s ",pStrInfo_common_Icmp_1);
              break;
            case L7_ACL_PROTOCOL_IGMP:
              sprintf(buf, "%s ",pStrInfo_common_Igmp_2);
              break;
            case L7_ACL_PROTOCOL_IP:
              sprintf(buf, "%s ",pStrInfo_common_IpOption);
              break;
            case L7_ACL_PROTOCOL_TCP:
              sprintf(buf, "%s ",pStrInfo_qos_Tcp_1);
              break;
            case L7_ACL_PROTOCOL_UDP:
              sprintf(buf, "%s ",pStrInfo_qos_Udp_1);
              break;
            default:
              /* use numeric value instead of keyword */
              sprintf(buf, "%d ",val);
            }
            strcat(stat, buf);
            foundValidCriterion = L7_TRUE;
          }
        }
        else if ((L7_FALSE == every) && (checkAcltype(aclId) == ACL_EXTENDED))
        {
          /* if protocol not configured for an extended ACL and rule is not "every", we need to output a protocol
           * keyword which defaults to 'ip' here
           */
          sprintf(buf, "%s ", pStrInfo_common_IpOption);
          strcat(stat, buf);
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCIP) == L7_TRUE)
        {
          if (usmDbQosAclRuleSrcIpMaskGet(unit, aclId, rulenum, &ipAddr, &ipMask) == L7_SUCCESS)
          {
            memset (buf, 0, sizeof(buf));
            if (usmDbInetNtoa(ipAddr,buf) == L7_SUCCESS)
            {
              strcat(stat, buf);
              strcat(stat, " ");
            }
            srcIpAddrConfigured = L7_TRUE;
          }
        }
        else if (every == L7_FALSE)
        {
          /* an unconfigured ipAddr means it was specified as 'any'for extended and 'every' for standard */
          if (checkAcltype(aclId) == ACL_STANDARD)
          {
            sprintf(buf, "%s ", pStrInfo_qos_AclEveryStr);
            strcat(stat, buf);
          }
          else
          {
            sprintf(buf, "%s ", pStrInfo_qos_AclAnyStr);
            strcat(stat, buf);
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCIP_MASK) == L7_TRUE)
        {
          memset (maskStr, 0, sizeof(maskStr));
          ipMask = ipMask^(~(L7_uint32 )0);
          if (usmDbInetNtoa(ipMask,maskStr) == L7_SUCCESS)
          {
            strcat(stat, maskStr);
            strcat(stat, " ");
          }
          srcIpMaskConfigured = L7_TRUE;
        }

        /* if either address or mask are not configured, the criteron is not complete */
        if ((L7_TRUE == srcIpAddrConfigured) && (L7_TRUE == srcIpMaskConfigured))
        {
          foundValidCriterion = L7_TRUE;
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCPORT) == L7_TRUE)
        {
          if (usmDbQosAclRuleSrcL4PortGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            memset (protVal, 0, sizeof(protVal));
            switch (val)
            {
            case L7_ACL_L4PORT_DOMAIN:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Domain_1);
              break;
            case L7_ACL_L4PORT_ECHO:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Echo);
              break;
            case L7_ACL_L4PORT_FTP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Ftp);
              break;
            case L7_ACL_L4PORT_FTPDATA:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Ftpdata);
              break;
            case L7_ACL_L4PORT_HTTP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Http_1);
              break;
            case L7_ACL_L4PORT_SMTP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Smtp);
              break;
            case L7_ACL_L4PORT_SNMP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Snmp_1);
              break;
            case L7_ACL_L4PORT_TELNET:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Telnet);
              break;
            case L7_ACL_L4PORT_TFTP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Tftp_1);
              break;
            default:
              /* use numeric value instead of keyword */
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq,val);
              break;
            }
            strcat(stat, protVal);
            foundValidCriterion = L7_TRUE;
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCSTARTPORT) == L7_TRUE)
        {
          if (usmDbQosAclRuleSrcL4PortRangeGet(unit, aclId, rulenum, &val, &val2)  == L7_SUCCESS)
          {
            memset (buf, 0, sizeof(buf));
            if (val == val2)
            {
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_Eq,val);
            }
            else
            {
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_Range_2,val,val2);
            }
            strcat(stat, buf);
            foundValidCriterion = L7_TRUE;
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTIP) == L7_TRUE)
        {
          if (usmDbQosAclRuleDstIpMaskGet(unit, aclId, rulenum, &ipAddr, &ipMask) == L7_SUCCESS)
          {
            memset (buf, 0, sizeof(buf));
            if (usmDbInetNtoa(ipAddr,buf) == L7_SUCCESS)
            {
              strcat(stat, buf);
              strcat(stat, " ");
            }
            dstIpAddrConfigured = L7_TRUE;
          }
        }
        else if ((L7_FALSE == every) && (checkAcltype(aclId) == ACL_EXTENDED))
        {
          /* an unconfigured ipAddr means it was specified as 'any' */
          sprintf(buf, "%s ", pStrInfo_qos_AclAnyStr);
          strcat(stat, buf);
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTIP_MASK) == L7_TRUE)
        {
          memset (maskStr, 0, sizeof(maskStr));
          ipMask = ipMask^(~(L7_uint32 )0);
          if (usmDbInetNtoa(ipMask,maskStr) == L7_SUCCESS)
          {
            strcat(stat, maskStr);
            strcat(stat, " ");
          }
          dstIpMaskConfigured = L7_TRUE;
        }

        /* if either address or mask are not configured, the criteron is not complete */
        if ((L7_TRUE == dstIpAddrConfigured) && (L7_TRUE == dstIpMaskConfigured))
        {
          foundValidCriterion = L7_TRUE;
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTPORT) == L7_TRUE)
        {
          if (usmDbQosAclRuleDstL4PortGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            memset (protVal, 0, sizeof(protVal));
            switch (val)
            {
            case L7_ACL_L4PORT_DOMAIN:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Domain_1);
              break;
            case L7_ACL_L4PORT_ECHO:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Echo);
              break;
            case L7_ACL_L4PORT_FTP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Ftp);
              break;
            case L7_ACL_L4PORT_FTPDATA:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Ftpdata);
              break;
            case L7_ACL_L4PORT_HTTP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Http_1);
              break;
            case L7_ACL_L4PORT_SMTP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Smtp);
              break;
            case L7_ACL_L4PORT_SNMP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Snmp_1);
              break;
            case L7_ACL_L4PORT_TELNET:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Telnet);
              break;
            case L7_ACL_L4PORT_TFTP:
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Tftp_1);
              break;
            default:
              /* use numeric value instead of keyword */
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq,val);
              break;
            }
            strcat(stat, protVal);
            foundValidCriterion = L7_TRUE;
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTSTARTPORT) == L7_TRUE)
        {
          if (usmDbQosAclRuleDstL4PortRangeGet(unit, aclId, rulenum, &val, &val2) == L7_SUCCESS)
          {
            memset (buf, 0, sizeof(buf));
            if (val == val2)
            {
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_Eq,val);
            }
            else
            {
              sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_Range_2,val,val2);
            }
            strcat(stat, buf);
          }
          foundValidCriterion = L7_TRUE;
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_IPPREC) == L7_TRUE)
        {
          if (usmDbQosAclRuleIPPrecedenceGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Precedence,val);
            strcat(stat, buf);
            foundValidCriterion = L7_TRUE;
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_IPTOS) == L7_TRUE)
        {
          if (usmDbQosAclRuleIPTosGet(unit, aclId, rulenum, &val, &val2) == L7_SUCCESS)
          {
            osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Tos_1, (L7_uchar8)val, ~val2 & 0x000000FF);
            strcat(stat, buf);
            foundValidCriterion = L7_TRUE;
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_IPDSCP) == L7_TRUE)
        {
          if (usmDbQosAclRuleIPDscpGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            memset (dscpString, 0, sizeof(dscpString));
            cliDiffservConvertDSCPValToKeyword(val, dscpString, sizeof(dscpString));
            osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Dscp,dscpString);
            strcat(stat, buf);
            foundValidCriterion = L7_TRUE;
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_LOGGING) == L7_TRUE)
        {
          if (usmDbQosAclRuleLoggingGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            /* ignore the flag value read for logging */
            sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_AclLogsStr);
            strcat(stat, buf);
          }
        }
        
        /* Time based ACLs */ 
        if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_TIME_RANGE_NAME) == L7_TRUE)
        {
          if (usmDbQosAclRuleTimeRangeNameGet(unit, aclId, rulenum, timeRangeName) == L7_SUCCESS)
          {
            sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_TimeRange, timeRangeName);
            osapiStrncat(stat, buf, osapiStrnlen(buf, sizeof(buf)));
          }
        }

        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_ASSIGN_QUEUEID) == L7_TRUE)
        {
          if (usmDbQosAclRuleAssignQueueIdGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_AssignQueue, val);
            strcat(stat, buf);
          }
        }

        /* Mirror Interface */
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_MIRROR_INTF) == L7_TRUE)
        {
          if (usmDbQosAclRuleMirrorIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
              usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
          {
            sprintf(buf, pStrInfo_qos_Mirror_1, cliDisplayInterfaceHelp(u, s, p));
            strcat(stat, buf);
          }
        }

        /* Redirect Interface */
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_REDIRECT_INTF) == L7_TRUE)
        {
          if (usmDbQosAclRuleRedirectIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
              usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
          {
            sprintf(buf, pStrInfo_qos_Redirect_2, cliDisplayInterfaceHelp(u, s, p));
            strcat(stat, buf);
          }
        }

        if (L7_TRUE == foundValidCriterion)
        {
          EWSWRITEBUFFER(ewsContext,stat);
        }

      } /* endif found valid action */

    }
    while (usmDbQosAclRuleGetNext(unit, aclId, rulenum, &rulenum) == L7_SUCCESS);
  }

  if (needTrailingCrlf == L7_TRUE)
  {
    osapiSnprintf(buf, sizeof(buf), pStrInfo_common_CrLf);
    EWSWRITEBUFFER(ewsContext,buf);
  }
}

/*********************************************************************
* @purpose  To print the running configuration of acl Info
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliRunningConfigIpv6AclInfo(EwsContext ewsContext, L7_uint32 unit)
{
  static L7_uint32 aclId;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 rulenum;
  L7_RC_t rc;
  L7_char8 aclName[L7_CLI_MAX_STRING_LENGTH];
  L7_in6_prefix_t ip6Addr;
  L7_uint32 val, val2, u, s, p;
  L7_char8 protVal[16];
  L7_char8 dscpString[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL foundValidAction, foundValidCriterion;
  L7_BOOL every;
  L7_char8 timeRangeName[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbQosAclNamedIndexGetFirst(unit, L7_ACL_TYPE_IPV6, &aclId) == L7_SUCCESS)
  {
    do
    {
      memset (aclName, 0, sizeof(aclName));
      rc = usmDbQosAclNameGet(unit, aclId, aclName);

      sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_qos_Ipv6AccessList, aclName );
      EWSWRITEBUFFER(ewsContext,stat);

      if ( usmDbQosAclRuleGetFirst(unit, aclId, &rulenum) == L7_SUCCESS)
      {
        do
        {
          foundValidAction = L7_FALSE;
          every = L7_FALSE;
          memset (stat, 0, sizeof(stat));
          if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum, ACL_ACTION) == L7_TRUE)
          {
            if (usmDbQosAclRuleActionGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
            {
              if (val == L7_ACL_PERMIT )
              {
                strcatAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Permit_1);
                foundValidAction = L7_TRUE;
              }
              else if (val == L7_ACL_DENY )
              {
                strcatAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Deny_2);
                foundValidAction = L7_TRUE;
              }
            }
          }

          /* only process the remaining fields if a valid rule action was found */
          if (foundValidAction == L7_TRUE)
          {
            foundValidCriterion = L7_FALSE;
            /* EVERY */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_EVERY) == L7_TRUE)
            { /* if the match every is set to true */
              if (usmDbQosAclRuleEveryGet(unit, aclId, rulenum, &every) == L7_SUCCESS)
              {
                if (every == L7_TRUE)
                {
                   foundValidCriterion = L7_TRUE;
                   strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_qos_AclEveryStr);
                }
              }
            }

            /* PROTOCOL */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_PROTOCOL) == L7_TRUE)
            {
              if (usmDbQosAclRuleProtocolGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                switch (val)
                {
                case L7_ACL_PROTOCOL_ICMPV6:
                  sprintf(buf, "%s ",pStrInfo_common_Icmpv6_1);
                  break;
                case L7_ACL_PROTOCOL_IP:
                  sprintf(buf, "%s ",pStrInfo_common_Diffserv_5);
                  break;
                case L7_ACL_PROTOCOL_TCP:
                  sprintf(buf, "%s ",pStrInfo_qos_Tcp_1);
                  break;
                case L7_ACL_PROTOCOL_UDP:
                  sprintf(buf, "%s ",pStrInfo_qos_Udp_1);
                  break;
                default:
                  /* use numeric value instead of keyword */
                  sprintf(buf, "%d ",val);
                }
                strcat(stat, buf);
                foundValidCriterion = L7_TRUE;
              }
            }
            else if (L7_FALSE == every)
            {
              /* if protocol not configured and rule is not "every", we need to output a protocol
               * keyword which defaults to 'ipv6' here
               */
              sprintf(buf, "%s ", pStrInfo_common_Diffserv_5);
              strcat(stat, buf);
            }

            /* SOURCE IPv6 ADDRESS */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_SRCIPV6) == L7_TRUE)
            {
              if (usmDbQosAclRuleSrcIpv6AddrGet(unit, aclId, rulenum, &ip6Addr) == L7_SUCCESS)
              {
                memset (buf, 0, sizeof(buf));
                if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ip6Addr, buf, sizeof(buf)) != L7_NULLPTR)
                {
                  strcat(stat, buf);
                  sprintf(buf, "/%d ", ip6Addr.in6PrefixLen);
                  strcat(stat, buf);
                  foundValidCriterion = L7_TRUE;
                }
              }
            }
            else if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_EVERY) == L7_FALSE)
            {
              /* an unconfigured ipAddr means it was specified as 'any' */
              sprintf(buf, "%s ", pStrInfo_qos_AclAnyStr);
              strcat(stat, buf);
            }

            /* SOURCE PORT */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_SRCPORT) == L7_TRUE)
            {
              if (usmDbQosAclRuleSrcL4PortGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                switch (val)
                {
                case L7_ACL_L4PORT_DOMAIN:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Domain_1);
                  break;
                case L7_ACL_L4PORT_ECHO:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Echo);
                  break;
                case L7_ACL_L4PORT_FTP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Ftp);
                  break;
                case L7_ACL_L4PORT_FTPDATA:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Ftpdata);
                  break;
                case L7_ACL_L4PORT_HTTP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Http_1);
                  break;
                case L7_ACL_L4PORT_SMTP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Smtp);
                  break;
                case L7_ACL_L4PORT_SNMP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Snmp_1);
                  break;
                case L7_ACL_L4PORT_TELNET:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Telnet);
                  break;
                case L7_ACL_L4PORT_TFTP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Tftp_1);
                  break;
                default:
                  /* use numeric value instead of keyword */
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq,val);
                  break;
                }
                strcat(stat, protVal);
                foundValidCriterion = L7_TRUE;
              }
            }

            /* SOURCE PORT RANGE*/
            if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCSTARTPORT) == L7_TRUE)
            {
              if (usmDbQosAclRuleSrcL4PortRangeGet(unit, aclId, rulenum, &val, &val2)  == L7_SUCCESS)
              {
                memset (buf, 0, sizeof(buf));
                if (val == val2)
                {
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_Eq,val);
                }
                else
                {
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_Range_2,val,val2);
                }
                strcat(stat, buf);
                foundValidCriterion = L7_TRUE;
              }
            }

            /* DESTINATION IPv6 ADDRESS */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_DSTIPV6) == L7_TRUE)
            {
              if (usmDbQosAclRuleDstIpv6AddrGet(unit, aclId, rulenum, &ip6Addr) == L7_SUCCESS)
              {

                memset (buf, 0, sizeof(buf));
                if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ip6Addr, buf, sizeof(buf)) != L7_NULLPTR)
                {
                  strcat(stat, buf);
                  sprintf(buf, "/%d ", ip6Addr.in6PrefixLen);
                  strcat(stat, buf);
                  foundValidCriterion = L7_TRUE;
                }
              }
            }
            else if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_EVERY) == L7_FALSE)
            {
              /* an unconfigured ipAddr means it was specified as 'any' */
              sprintf(buf, "%s ", pStrInfo_qos_AclAnyStr);
              strcat(stat, buf);
            }

            /* DESTINATION PORT */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_DSTPORT) == L7_TRUE)
            {
              if (usmDbQosAclRuleDstL4PortGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                memset (protVal, 0, sizeof(protVal));
                switch (val)
                {
                case L7_ACL_L4PORT_DOMAIN:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Domain_1);
                  break;
                case L7_ACL_L4PORT_ECHO:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Echo);
                  break;
                case L7_ACL_L4PORT_FTP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Ftp);
                  break;
                case L7_ACL_L4PORT_FTPDATA:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Ftpdata);
                  break;
                case L7_ACL_L4PORT_HTTP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Http_1);
                  break;
                case L7_ACL_L4PORT_SMTP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_qos_Smtp);
                  break;
                case L7_ACL_L4PORT_SNMP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Snmp_1);
                  break;
                case L7_ACL_L4PORT_TELNET:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Telnet);
                  break;
                case L7_ACL_L4PORT_TFTP:
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq_1,pStrInfo_common_Tftp_1);
                  break;
                default:
                  /* use numeric value instead of keyword */
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, protVal, pStrInfo_qos_Eq,val);
                  break;
                }
                strcat(stat, protVal);
                foundValidCriterion = L7_TRUE;
              }
            }

            /* DESTINATION PORT RANGE*/
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_DSTSTARTPORT) == L7_TRUE)
            {
              if (usmDbQosAclRuleDstL4PortRangeGet(unit, aclId, rulenum, &val, &val2) == L7_SUCCESS)
              {
                memset (buf, 0, sizeof(buf));
                if (val == val2)
                {
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_Eq,val);
                }
                else
                {
                  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_Range_2,val,val2);
                }
                strcat(stat, buf);
                foundValidCriterion = L7_TRUE;
              }
            }

            /* DSCP */
            if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_IPDSCP) == L7_TRUE)
            {
              if (usmDbQosAclRuleIPDscpGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                memset (dscpString, 0, sizeof(dscpString));
                cliDiffservConvertDSCPValToKeyword(val, dscpString, sizeof(dscpString));
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Dscp,dscpString);
                strcat(stat, buf);
                foundValidCriterion = L7_TRUE;
              }
            }

            /* FLOW LABEL */
            if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum, ACL_FLOWLBLV6) == L7_TRUE)
            {
              if (usmDbQosAclRuleIpv6FlowLabelGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_AclFlowLabelsStr_1, val);
                strcat(stat, buf);
                foundValidCriterion = L7_TRUE;
              }
            }

            /* LOG */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_LOGGING) == L7_TRUE)
            {
              if (usmDbQosAclRuleLoggingGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                /* ignore the flag value read for logging */
                sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_AclLogsStr);
                strcat(stat, buf);
              }
            }
            
            /* Time based ACLs */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_TIME_RANGE_NAME) == L7_TRUE)
            {
              if (usmDbQosAclRuleTimeRangeNameGet(unit, aclId, rulenum, timeRangeName) == L7_SUCCESS)
              {
                sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_TimeRange, timeRangeName);
                osapiStrncat(stat, buf, osapiStrnlen(buf, sizeof(buf)));
              }
            }
 
            
            /* ASSIGN QUEUE */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_ASSIGN_QUEUEID) == L7_TRUE)
            {
              if (usmDbQosAclRuleAssignQueueIdGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_AssignQueue, val);
                strcat(stat, buf);
              }
            }

            /* MIRROR Interface */
            if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_MIRROR_INTF) == L7_TRUE)
            {
              if (usmDbQosAclRuleMirrorIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
                  usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
              {
                sprintf(buf, pStrInfo_qos_Mirror_1, cliDisplayInterfaceHelp(u, s, p));
                strcat(stat, buf);
              }
            }

            /* REDIRECT Interface */
            if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_REDIRECT_INTF) == L7_TRUE)
            {
              if (usmDbQosAclRuleRedirectIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
                  usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
              {
                sprintf(buf, pStrInfo_qos_Redirect_2, cliDisplayInterfaceHelp(u, s, p));
                strcat(stat, buf);
              }
            }

            if (L7_TRUE == foundValidCriterion)
            {
              EWSWRITEBUFFER(ewsContext,stat);
            }

          } /* endif found valid action */

        }
        while (usmDbQosAclRuleGetNext(unit, aclId, rulenum, &rulenum) == L7_SUCCESS);
      }

      osapiSnprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Exit);
      EWSWRITEBUFFER(ewsContext, buf);

    } while (usmDbQosAclNamedIndexGetNext(unit, L7_ACL_TYPE_IPV6, aclId, &aclId) == L7_SUCCESS);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of acl Info
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliRunningConfigMacAclInfo(EwsContext ewsContext, L7_uint32 unit)
{
  static L7_uint32 aclId;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 rulenum, rc, keyId ;
  L7_char8 macAclName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN];
  L7_uchar8 srcMacMask[L7_MAC_ADDR_LEN];
  L7_uchar8 dstMacAddr[L7_MAC_ADDR_LEN];
  L7_uchar8 dstMacMask[L7_MAC_ADDR_LEN];
  L7_char8 strDstMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strDstMacMask[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val, val2, u, s, p, i;
  L7_BOOL foundValidAction;
  L7_uchar8 timeRangeName[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbQosAclMacIndexGetFirst(unit, &aclId) == L7_SUCCESS)
  {
    do
    /* this section is executed for both the first and successive MAC ACLs */
    {
      memset (macAclName, 0, sizeof(macAclName));
      rc = usmDbQosAclMacNameGet(unit, aclId, macAclName);

      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_MacAccessListExtended_1, macAclName );
      EWSWRITEBUFFER(ewsContext,stat);

      if (usmDbQosAclMacRuleGetFirst(unit, aclId, &rulenum) == L7_SUCCESS)
      {
        do
        /* this section is executed for both the first and successive rules */
        {
          memset (stat, 0, sizeof(stat));
          foundValidAction = L7_FALSE;

          if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_ACTION) == L7_TRUE)
          {
            if (usmDbQosAclMacRuleActionGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
            {

              if (val == L7_ACL_PERMIT)
              {
                strcatAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Permit_1);
                foundValidAction = L7_TRUE;
              }
              else if (val == L7_ACL_DENY)
              {
                strcatAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Deny_2);
                foundValidAction = L7_TRUE;
              }
            }
          }

          /* only process the remaining fields if a valid rule action was found */
          if (foundValidAction == L7_TRUE)
          {
            /* SOURCE MAC ACL */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_SRCMAC) == L7_TRUE)
            {
              memset (srcMacAddr, 0, sizeof(srcMacAddr));
              memset (srcMacMask, 0, sizeof(srcMacMask));
              if (usmDbQosAclMacRuleSrcMacAddrMaskGet(unit, aclId, rulenum, srcMacAddr, srcMacMask) == L7_SUCCESS)
              {
                for (i = 0; i < L7_MAC_ADDR_LEN; i++)
                {
                  srcMacMask[i] ^= (L7_uchar8)0xFF;
                }

                osapiSnprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x ",
                              srcMacAddr[0], srcMacAddr[1], srcMacAddr[2], srcMacAddr[3], srcMacAddr[4], srcMacAddr[5]);
                strcat(stat, buf);

                if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                             L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID) == L7_TRUE)
                {
                  osapiSnprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x ",
                                srcMacMask[0], srcMacMask[1], srcMacMask[2], srcMacMask[3], srcMacMask[4], srcMacMask[5]);
                  strcat(stat, buf);
                }
              }
            }
            else
            {
              /* an unconfigured srcMacAddr means it was specified as 'any' */
              sprintf(buf, "%s ", pStrInfo_qos_AclAnyStr);
              strcat(stat, buf);
            }

            /* DESTINATION MAC ACL */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_DSTMAC) == L7_TRUE)
            {
              memset (dstMacAddr, 0, sizeof(dstMacAddr));
              memset (dstMacMask, 0, sizeof(dstMacMask));
              if (usmDbQosAclMacRuleDstMacAddrMaskGet(unit, aclId, rulenum, dstMacAddr, dstMacMask) == L7_SUCCESS)
              {
                /* BPDU */
                sprintf(strDstMacAddr,"%02X:%02X:%02X:%02X:%02X:%02X",
                        dstMacAddr[0], dstMacAddr[1], dstMacAddr[2], dstMacAddr[3], dstMacAddr[4], dstMacAddr[5]);

                sprintf(strDstMacMask,"%02X:%02X:%02X:%02X:%02X:%02X",
                        dstMacMask[0], dstMacMask[1], dstMacMask[2], dstMacMask[3], dstMacMask[4], dstMacMask[5]);

                if (strcmp(strDstMacAddr, pStrInfo_qos_0180C2000000) == 0 &&
                    strcmp(strDstMacMask, pStrInfo_qos_FfFfFf000000) == 0)
                {
                  sprintf(buf, "%s ", pStrInfo_common_MacAclBpdusStr);
                  strcat(stat, buf);
                }
                else
                {
                  for (i = 0; i < L7_MAC_ADDR_LEN; i++)
                  {
                    dstMacMask[i] ^= (L7_uchar8)0xFF;
                  }

                  osapiSnprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x ",
                                dstMacAddr[0], dstMacAddr[1], dstMacAddr[2], dstMacAddr[3], dstMacAddr[4], dstMacAddr[5]);
                  strcat(stat, buf);

                  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                               L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) == L7_TRUE)
                  {

                    osapiSnprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x ",
                                  dstMacMask[0], dstMacMask[1], dstMacMask[2], dstMacMask[3], dstMacMask[4], dstMacMask[5]);
                    strcat(stat, buf);
                  }
                }
              }
            }
            else
            {
              /* an unconfigured dstMacAddr means it was specified as 'any' */
              sprintf(buf, "%s ", pStrInfo_qos_AclAnyStr);
              strcat(stat, buf);
            }

            /* Ethertype */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_ETYPE_KEYID) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleEtypeKeyGet(unit, aclId, rulenum, &keyId, &val) == L7_SUCCESS)
              {
                memset (buf, 0, sizeof(buf));
                if (keyId == L7_QOS_ETYPE_KEYID_CUSTOM)
                {
                  sprintf(buf, "0x%4.4x ", (L7_ushort16)val);
                  strcat(stat, buf);
                }
                else if (cliDiffservConvertEtherTypeKeyIdToString(keyId, buf, sizeof(buf)) == L7_SUCCESS)
                {
                  strcat(stat, buf);
                  strcat(stat, " ");
                }
              }
            }

            /* VLAN */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_VLANID) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleVlanIdGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_VlanEq,val);
                strcat(stat, buf);
              }
            }
            else if (usmDbQosAclMacIsFieldConfigured(unit, aclId, rulenum, ACL_MAC_VLANID_START) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleVlanIdRangeGet(unit, aclId, rulenum, &val, &val2) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_VlanRange_1, val, val2);
                strcat(stat, buf);
              }
            }

            /* COS */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_COS) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleCosGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Cos_1,val);
                strcat(stat, buf);
              }
            }

            /* SECONDARY-VLAN */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_VLANID2) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleVlanId2Get(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_SecondaryVlanEq,val);
                strcat(stat, buf);
              }
            }

            /* SECONDARY-VLAN RANGE */
            else if (usmDbQosAclMacIsFieldConfigured(unit, aclId, rulenum, ACL_MAC_VLANID2_START) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleVlanId2RangeGet(unit, aclId, rulenum, &val, &val2) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_SecondaryVlanRange_1, val, val2);
                strcat(stat, buf);
              }
            }

            /* SECONDARY-COS */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_COS2) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleCos2Get(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_SecondaryCos,val);
                strcat(stat, buf);
              }
            }

            /* LOG */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_LOGGING) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleLoggingGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                /* ignore the flag value read for logging */
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_AclLogsStr);
                strcat(stat, buf);
              }
            }
            
            /* Time based ACLs */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId, rulenum, ACL_MAC_TIME_RANGE_NAME) == L7_TRUE)
            {  
              if (usmDbQosAclMacRuleTimeRangeNameGet(unit, aclId, rulenum, timeRangeName) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_TimeRange, timeRangeName);
                osapiStrncat(stat, buf, osapiStrnlen(buf, sizeof(buf)));
              }  
            }


            /* ASSIGN QUEUE */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_ASSIGN_QUEUEID) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleAssignQueueIdGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_AssignQueue_1,val);
                strcat(stat, buf);
              }
            }

            /* MIRROR */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_MIRROR_INTF) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleMirrorIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
                  usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Mirror_1, cliDisplayInterfaceHelp(u, s, p));
                strcat(stat, buf);
              }
            }

            /* REDIRECT */
            if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_REDIRECT_INTF) == L7_TRUE)
            {
              if (usmDbQosAclMacRuleRedirectIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
                  usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Redirect_2, cliDisplayInterfaceHelp(u, s, p));
                strcat(stat, buf);
              }
            }

            EWSWRITEBUFFER(ewsContext,stat);
          }
        }
        while (usmDbQosAclMacRuleGetNext(unit, aclId, rulenum, &rulenum) == L7_SUCCESS);
      }

      osapiSnprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Exit);
      EWSWRITEBUFFER(ewsContext, buf);

    } while (usmDbQosAclMacIndexGetNext(unit, aclId, &aclId) == L7_SUCCESS);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of acl Info for a particular interface
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliRunningInterfaceAclInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_uint32 aclId, sequenceNum;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 dirStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 seqNumStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 direction, i;
  L7_ACL_INTF_DIR_LIST_t listInfo;

  if (usmDbQosAclIsValidIntf(unit, interface) == L7_TRUE)
  {
    for (direction = L7_INBOUND_ACL; direction <= L7_OUTBOUND_ACL; direction++)
    {
      memset ((char *)&listInfo, 0, sizeof(listInfo));
      if (usmDbQosAclIntfDirAclListGet(unit, interface, direction, &listInfo) == L7_SUCCESS)
      {
        osapiStrncpySafe(dirStr, (direction == L7_INBOUND_ACL) ? pStrInfo_common_AclInStr : pStrInfo_common_AclOutStr, sizeof(dirStr));

        for (i = 0; i < listInfo.count; i++)
        {
          aclId = listInfo.listEntry[i].aclId;
          sequenceNum = listInfo.listEntry[i].seqNum;

          if (sequenceNum != L7_ACL_AUTO_INCR_INTF_SEQ_NUM)
          {
            sprintf(seqNumStr, "%u", sequenceNum);
          }
          else
          {
            osapiStrncpySafe(seqNumStr, " ", sizeof(seqNumStr));
          }

          memset (buf, 0,sizeof(buf));
          if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IP)
          {
            if (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) == L7_SUCCESS)
            {
              /* current ACL is named */
              memset (aclName, 0, sizeof(aclName));
              (void)usmDbQosAclNameGet(unit, aclId, aclName);
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, "ip access-group %s %s %s", aclName, dirStr, seqNumStr);
            }
            else
            {
              /* current ACl is numbered */
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_IpAccessGrp, aclId, dirStr, seqNumStr);
            }
            EWSWRITEBUFFER(ewsContext,buf);
          }
          else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_MAC)
          {
            memset (aclName, 0, sizeof(aclName));
            (void)usmDbQosAclMacNameGet(unit, aclId, aclName);
            sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_MacAccessGrp_1, aclName, dirStr, seqNumStr);
            EWSWRITEBUFFER(ewsContext,buf);
          }
          else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IPV6)
          {
            memset (aclName, 0, sizeof(aclName));
            (void)usmDbQosAclNameGet(unit, aclId, aclName);
            sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_Ipv6TrafficFilter_1, aclName, dirStr, seqNumStr);
            EWSWRITEBUFFER(ewsContext,buf);
          }
          else
          {
            /* ignore unknown ACL type */
          }
        } /* endfor */
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of acl Info for a particular vlanid
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliRunningVlanAclInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 aclId, sequenceNum;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 dirStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 seqNumStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 direction, i, vlanid;
  L7_ACL_VLAN_DIR_LIST_t listInfo;

  vlanid = 0;
  while (usmDbQosAclVlanIdGetNext(unit, vlanid, &vlanid) == L7_SUCCESS)
  {
    if (usmDbQosAclIsValidVlan(unit, vlanid) == L7_TRUE)
    {
      for (direction = L7_INBOUND_ACL; direction <= L7_OUTBOUND_ACL; direction++)
      {
        memset ((char *)&listInfo, 0, sizeof(listInfo));
        if (usmDbQosAclVlanDirAclListGet(unit, vlanid, direction, &listInfo) == L7_SUCCESS)
        {
          osapiStrncpySafe(dirStr, (direction == L7_INBOUND_ACL) ? pStrInfo_common_AclInStr : pStrInfo_common_AclOutStr, sizeof(dirStr));

          for (i = 0; i < listInfo.count; i++)
          {
            aclId = listInfo.listEntry[i].aclId;
            sequenceNum = listInfo.listEntry[i].seqNum;

            if (sequenceNum != L7_ACL_AUTO_INCR_INTF_SEQ_NUM)
            {
              sprintf(seqNumStr, "%u", sequenceNum);
            }
            else
            {
              sprintf(seqNumStr, " ");
            }

            memset (buf, 0,sizeof(buf));
            if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IP)
            {
              if (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) == L7_SUCCESS)
              {
                /* current ACL is named */
                memset (aclName, 0, sizeof(aclName));
                (void)usmDbQosAclNameGet(unit, aclId, aclName);
                sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, "ip access-group %s vlan %u %s %s", aclName, vlanid, dirStr, seqNumStr);
              }
              else
              {
                /* current ACL is numbered */
                sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_IpAccessGrpVlan, aclId, vlanid, dirStr, seqNumStr);
              }
              EWSWRITEBUFFER(ewsContext,buf);
            }
            else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_MAC)
            {
              memset (aclName, 0, sizeof(aclName));
              usmDbQosAclMacNameGet(unit, aclId, aclName);
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_MacAccessGrpVlan_1, aclName, vlanid, dirStr, seqNumStr);
              EWSWRITEBUFFER(ewsContext,buf);
            }
            else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IPV6)
            {
              memset (aclName, 0, sizeof(aclName));
              usmDbQosAclNameGet(unit, aclId, aclName);
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_Ipv6TrafficFilterVlan_1, aclName, vlanid, dirStr, seqNumStr);
              EWSWRITEBUFFER(ewsContext,buf);
            }
            else
            {
              /* ignore unknown ACL type */
            }
          } /* endfor */
        }
      }
    }
  }

  return L7_SUCCESS;
}
#endif /* end if ACL package included */
