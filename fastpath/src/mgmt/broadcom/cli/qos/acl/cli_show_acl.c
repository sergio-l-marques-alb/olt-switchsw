/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/acl/cli_show_acl.c
 *
 * @purpose acl show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/11/2003
 *
 * @author  Jagdish
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
#include "usmdb_util_api.h"
#include "usmdb_qos_acl_api.h"
#include "acl_api.h"

#include "datatypes.h"
#include "default_cnfgr.h"

#include "clicommands_acl.h"
#include "clicommands_card.h"
#include "cliutil_acl.h"
#include "osapi_support.h"

/*********************************************************************
*
* @purpose  Display access list summary information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  command show ip access-list [<access-list-number>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpAccessList(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_RC_t rc = L7_SUCCESS;
  static L7_uint32 aclId;
  static L7_uint32 intfIndex;
  static L7_uint32 vlanIndex;
  static L7_ACL_DIRECTION_t direction;
  static L7_uint32 rulenum;
  L7_uint32 unit;
  L7_uint32 val;
  L7_uint32 val2;
  L7_uint32 ipAddr;
  L7_uint32 ipMask;
  L7_uint32 rules;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 aclName[L7_CLI_MAX_STRING_LENGTH];
  L7_ACL_ASSIGNED_INTF_LIST_t intfList[L7_ACL_DIRECTION_TOTAL];
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList[L7_ACL_DIRECTION_TOTAL];
  L7_ACL_DIRECTION_t dir;
  L7_uint32 x=0;
  L7_uint32 numwrites;
  L7_uint32 u;
  L7_uint32 s;
  L7_uint32 p;
  L7_BOOL currentAclInfoPrinted, currentDirectionPrinted;
  L7_char8 ipStr[CLIACL_IPSTRSIZE], maskStr[CLIACL_IPSTRSIZE];
  L7_BOOL every;
  L7_char8 protStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 currentACLs, maxACLs;
  L7_uint32 interfacesColumnEnd, vlansColumnStart, vlansColumnEnd;
  L7_BOOL interfaceColumnComplete, vlanColumnComplete, i;

  /* Time Range Variables */
  L7_uchar8 timeRangeName[L7_CLI_MAX_STRING_LENGTH]; 

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliNumFunctionArgsGet() == 0)     /* parameter check */
  {
    cliCmdScrollSet( L7_FALSE);
    if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
    {
      numwrites = 0;
      /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      rc = usmDbQosAclNumGetFirst(unit, &aclId);
      if (L7_SUCCESS != rc)
      {
        /* if we found no numbered IP ACLs, check if there are any named IP ACLs */
        rc = usmDbQosAclNamedIndexGetFirst(unit, L7_ACL_TYPE_IP, &aclId);
      }
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoAclsAreCfgured_1);
      }

      numwrites = 0;
      direction = L7_INBOUND_ACL;
      intfIndex = 0;
      vlanIndex = 0;

      cliSyntaxBottom(ewsContext);
    }

    if (usmDbQosAclMaxNumGet(unit, &maxACLs) == L7_SUCCESS)
    {
      if (usmDbQosAclCurrNumGet(unit, &currentACLs) == L7_SUCCESS)
      {
        sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, buf,pStrErr_qos_CfgAclMaxAndCurrentAcl, currentACLs, maxACLs);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                               "ACL ID/Name                      Rules  Direction  Interface(s)     VLAN(s)");
      ewsTelnetWrite(ewsContext,
                           "\r\n-------------------------------  -----  ---------  --------------   ----------");
      interfacesColumnEnd = 15;
      vlansColumnStart = 17;
      vlansColumnEnd = 28;
    }
    else
    {
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                               "ACL ID/Name                      Rules  Direction        Interface(s)");
      ewsTelnetWrite(ewsContext,
                           "\r\n-------------------------------  -----  ----------  -------------------------");
      interfacesColumnEnd = 32;
      vlansColumnStart = 0;
      vlansColumnEnd = 0;
    }
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

    currentAclInfoPrinted = L7_FALSE;
    currentDirectionPrinted = L7_FALSE;
    for (dir = L7_INBOUND_ACL; dir < L7_ACL_DIRECTION_TOTAL; dir++)
    {
      if(usmDbQosAclAssignedIntfDirListGet(unit, aclId, dir, &intfList[dir]) != L7_SUCCESS)
      {
        intfList[dir].count = 0;
      }
      if(usmDbQosAclAssignedVlanDirListGet(unit, aclId, dir, &vlanList[dir]) != L7_SUCCESS)
      {
        vlanList[dir].count = 0;
      }
    }

    while(rc == L7_SUCCESS && numwrites < (CLI_MAX_SCROLL_LINES-6))
    {
      if (currentAclInfoPrinted == L7_FALSE)
      {
        if ((usmDbQosAclNamedIndexCheckValid(unit, L7_ACL_TYPE_IP, aclId) == L7_SUCCESS) &&
            (usmDbQosAclNameGet(unit, aclId, aclName) == L7_SUCCESS))
        {
          ewsTelnetPrintf (ewsContext, "%-33s", aclName);
        }
        else
        {
         ewsTelnetPrintf (ewsContext, "%-33d", aclId);
        }

        /* count the rules in this ACL */
        rules = 0;
        rc = usmDbQosAclRuleGetFirst(unit, aclId, &rulenum);
        if(rc == L7_SUCCESS)
        {
          do
          {
            rules++;
          } while(usmDbQosAclRuleGetNext(unit, aclId, rulenum, &rulenum) == L7_SUCCESS);
        }

        ewsTelnetPrintf (ewsContext, pStrErr_qos_CfgAcls, rules);
        currentAclInfoPrinted = L7_TRUE;
      }

      if (currentDirectionPrinted == L7_FALSE)
      {
        if ((intfList[direction].count > 0) || (vlanList[direction].count > 0))
        {
          switch (direction)
          {
          case L7_INBOUND_ACL:
            sprintf(buf, "%-11s", pStrInfo_qos_Inbound_1);
            break;
          case L7_OUTBOUND_ACL:
            sprintf(buf, "%-11s", pStrInfo_qos_Outbound_1);
            break;
          default:
            /* unknown direction enumeration */
            sprintf(buf, "        ");
          }
          currentDirectionPrinted = L7_TRUE;
          ewsTelnetWrite(ewsContext, buf);
        }
      }

      memset (buf, 0,  sizeof(buf));
      memset (stat, 0,sizeof(stat));
      interfaceColumnComplete = L7_FALSE;
      vlanColumnComplete = L7_FALSE;

      while ((intfIndex < intfList[direction].count) && (interfaceColumnComplete == L7_FALSE))
      {
        if(usmDbUnitSlotPortGet(intfList[direction].intIfNum[intfIndex], &u, &s, &p) == L7_SUCCESS)
        {
          osapiStrncpySafe(stat, cliDisplayInterfaceHelp(u, s, p), sizeof(stat));
          if (intfIndex < (intfList[direction].count-1))
          {
            strcat(stat, ", ");
          }
          if ((strlen(buf) + strlen(stat)) < interfacesColumnEnd)
          {
            strcat(buf, stat);
            intfIndex++;
            if(intfIndex == intfList[direction].count)
            {
              interfaceColumnComplete = L7_TRUE;
            }
          }
          else
          {
            interfaceColumnComplete = L7_TRUE;
          }
        }
      }
      /* print spaces to pad over to start of vlan column, if necessary */
      if (vlanList[direction].count > 0)
      {
        for (i=strlen(buf); i < vlansColumnStart; i++)
        {
          strcat(buf, " ");
        }
      }

      while ((vlanIndex < vlanList[direction].count) && (vlanColumnComplete == L7_FALSE))
      {
        sprintf(stat, "%u", vlanList[direction].vlanNum[vlanIndex]);
        if (vlanIndex < (vlanList[direction].count-1))
        {
          strcat(stat, ", ");
        }
        if ((strlen(buf) + strlen(stat)) < vlansColumnEnd)
        {
          strcat(buf, stat);
          vlanIndex++;
          if(vlanIndex == vlanList[direction].count)
          {
            vlanColumnComplete = L7_TRUE;
          }
        }
        else
        {
          vlanColumnComplete = L7_TRUE;
        }
      }

      if ((interfaceColumnComplete == L7_TRUE) || (vlanColumnComplete == L7_TRUE))
      {
        ewsTelnetWrite(ewsContext,buf);
        memset (buf, 0,sizeof(buf));
      }

      rc = L7_FAILURE;
      while (rc != L7_SUCCESS)
      {
        if (intfIndex < intfList[direction].count)
        {
          /* not finished with this acl/direction; move to new line and pad to Interfaces column */
          ewsTelnetWrite(ewsContext,  "\r\n                                                   ");
          numwrites++;
          rc = L7_SUCCESS;
          break;
        }
        else if (vlanIndex < vlanList[direction].count)
        {
          /* not finished with this acl/direction; move to new line and pad to vlans column */
          ewsTelnetWrite(ewsContext,  "\r\n                                                   ");
          numwrites++;
          rc = L7_SUCCESS;
          break;
        }
        else if (direction < L7_OUTBOUND_ACL)
        {
          /* finished outputting interfaces and/or vlans for current acl/direction; move to next direction, if any */
          if (((intfList[L7_INBOUND_ACL].count > 0) || (vlanList[L7_INBOUND_ACL].count > 0)) &&
              ((intfList[L7_OUTBOUND_ACL].count > 0) || (vlanList[L7_OUTBOUND_ACL].count > 0)))
          {
            ewsTelnetWrite(ewsContext,  "\r\n                                        ");
            numwrites++;
          }
          currentDirectionPrinted = L7_FALSE;
          direction = L7_OUTBOUND_ACL;
          intfIndex = 0;
          vlanIndex = 0;
          rc = L7_SUCCESS;
          break;
        }
        else
        {
          /* done with this acl, see if there is another to work on */
          rc = usmDbQosAclNumGetNext(unit, aclId, &aclId);
          if (L7_SUCCESS != rc)
          {
            rc = usmDbQosAclNamedIndexGetNext(unit, L7_ACL_TYPE_IP, aclId, &aclId);
          }
          if (rc == L7_SUCCESS)
          {
            currentAclInfoPrinted = L7_FALSE;
            intfIndex = 0;
            vlanIndex = 0;
            currentDirectionPrinted = L7_FALSE;
            direction = L7_INBOUND_ACL;
            for (dir = L7_INBOUND_ACL; dir < L7_ACL_DIRECTION_TOTAL; dir++)
            {
              if(usmDbQosAclAssignedIntfDirListGet(unit, aclId, dir, &intfList[dir]) != L7_SUCCESS)
              {
                intfList[dir].count = 0;
              }
              if(usmDbQosAclAssignedVlanDirListGet(unit, aclId, dir, &vlanList[dir]) != L7_SUCCESS)
              {
                vlanList[dir].count = 0;
              }
            }
            ewsTelnetWrite(ewsContext,  pStrInfo_common_CrLf);
            numwrites++;
          }
          else
          {
            /* done with all acl's */
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliSyntaxReturnPrompt (ewsContext, "");
          }
        }
      }
    }

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_qos_ShowIpAccessList);
    return pStrInfo_common_Name_2;
  }

  else if(cliNumFunctionArgsGet() == 1)      /* parameter check */
  {
    cliCmdScrollSet(L7_FALSE);
    numwrites = 0;
    if(cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }

    else   /* we are entering the command for the first time */
    {
      rulenum = 0;
      if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &aclId) != L7_SUCCESS)
      {
        if ((usmDbQosAclNameToIndex(unit, (L7_uchar8 *)argv[index+1], &aclId) != L7_SUCCESS) ||
            (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) != L7_SUCCESS))
        {
          return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_AccessListDoesntExist);
        }
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, "ACL Name: %s", argv[index+1]);
      }
      else
      {
        /* check for the allowed standard and extended ACL ID ranges */
        if ((aclId < L7_MIN_ACL_ID) || (aclId > L7_MAX_ACL_ID))
        {
          return cliSyntaxReturnPrompt (ewsContext, pStrErr_qos_IncorrectValAclIdMustBeBetween1And199);
        }
  
        if (usmDbQosAclNumCheckValid(unit, aclId) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclIdDoesntExist);
        }
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_qos_CfgAclsStrIntfOr, pStrInfo_qos_AclId, aclId);
      }

      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);

      /* interfaces */
      /* inbound */
      rc = usmDbQosAclAssignedIntfDirListGet(unit, aclId, L7_INBOUND_ACL, &intfList[L7_INBOUND_ACL]);
      if (rc == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "%s %s: ", pStrInfo_qos_Inbound, pStrInfo_qos_Intf_1);
        x=0;
        while ( x < intfList[L7_INBOUND_ACL].count)
        {
          if(usmDbUnitSlotPortGet(intfList[L7_INBOUND_ACL].intIfNum[x], &u, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
            if( x < (intfList[L7_INBOUND_ACL].count-1))
            {
              strcat(stat, ", ");
            }
            ewsTelnetWrite (ewsContext, stat);
            x++;
          }
        }
        cliSyntaxBottom(ewsContext);
      }

      /* outbound */
      rc = usmDbQosAclAssignedIntfDirListGet(unit, aclId, L7_OUTBOUND_ACL, &intfList[L7_OUTBOUND_ACL]);
      if (rc == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "%s %s: ", pStrInfo_qos_Outbound, pStrInfo_qos_Intf_1);
        x=0;
        while ( x < intfList[L7_OUTBOUND_ACL].count)
        {
          if(usmDbUnitSlotPortGet(intfList[L7_OUTBOUND_ACL].intIfNum[x], &u, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
            if( x < (intfList[L7_OUTBOUND_ACL].count-1))
            {
              strcat(stat, ", ");
            }
            ewsTelnetWrite (ewsContext, stat);
            x++;
          }
        }
        cliSyntaxBottom(ewsContext);
      }

      /* vlans */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE)
      {
        /* inbound */
        rc = usmDbQosAclAssignedVlanDirListGet(unit, aclId, L7_INBOUND_ACL, &vlanList[L7_INBOUND_ACL]);
        if (rc == L7_SUCCESS)
        {
          ewsTelnetPrintf (ewsContext, "%s %s: ", pStrInfo_qos_Inbound, pStrInfo_qos_Vlan_1);
          for (x=0; x<vlanList[L7_INBOUND_ACL].count; x++)
          {
            osapiSnprintf(stat, sizeof(stat), "%u", vlanList[L7_INBOUND_ACL].vlanNum[x]);
            if( x < (vlanList[L7_INBOUND_ACL].count-1))
            {
              strcat(stat, ", ");
            }
            ewsTelnetWrite (ewsContext, stat);
          }
          cliSyntaxBottom(ewsContext);
        }

        /* outbound */
        rc = usmDbQosAclAssignedVlanDirListGet(unit, aclId, L7_OUTBOUND_ACL, &vlanList[L7_OUTBOUND_ACL]);
        if (rc == L7_SUCCESS)
        {
          ewsTelnetPrintf (ewsContext, "%s %s: ", pStrInfo_qos_Outbound, pStrInfo_qos_Vlan_1);
          for (x=0; x<vlanList[L7_OUTBOUND_ACL].count; x++)
          {
            osapiSnprintf(stat, sizeof(stat), "%u", vlanList[L7_OUTBOUND_ACL].vlanNum[x]);
            if( x < (vlanList[L7_OUTBOUND_ACL].count-1))
            {
              strcat(stat, ", ");
            }
            ewsTelnetWrite (ewsContext, stat);
          }
          cliSyntaxBottom(ewsContext);
        }
      }

      /* since this is the first time, get a new rulenum */
      rc = usmDbQosAclRuleGetFirst(unit, aclId, &rulenum);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_NoRulesHaveBeenCfguredForAcl);
      }
    }

    do
    /* this section is executed for both the first and successive times */
    {
      cliSyntaxBottom(ewsContext);
      ewsTelnetPrintf (ewsContext, "\r\n%s %d",pStrInfo_qos_RuleNum,rulenum);

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_ACTION) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_common_Action);
        memset (buf, 0, sizeof(buf));
        rc = usmDbQosAclRuleActionGet(unit, aclId, rulenum, &val);
        if (val == L7_ACL_PERMIT     /*ACL_PERMIT*/)
        {
          ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_Permit_1);
        }
        if (val == L7_ACL_DENY     /*ACL_DENY*/)
        {
          ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_Deny_2);
        }
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_EVERY) == L7_TRUE)
      {    /* if the match every is set to true */
        cliFormat(ewsContext, pStrInfo_qos_MatchAll);

        memset (buf, 0, sizeof(buf));
        rc = usmDbQosAclRuleEveryGet(unit, aclId, rulenum, &every);
        if (every == L7_TRUE)
        {
          sprintf(buf, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_True);
        }
        else
        {
          sprintf(buf, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_False);
        }
        ewsTelnetWrite(ewsContext,buf);
      }
      else if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_EVERY) == L7_FALSE)
      {    /* if the match every is set to False */
        cliFormat(ewsContext, pStrInfo_qos_MatchAll);

        memset (buf, 0, sizeof(buf));
        rc = usmDbQosAclRuleEveryGet(unit, aclId, rulenum, &every);
        if ((every == L7_FALSE) && (rulenum != L7_ACL_DEFAULT_RULE_NUM))
        {
          sprintf(buf, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_False);
        }
        else
        {
          sprintfAddBlanks (0, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_common_CfgAclsStringFmt, pStrErr_qos_MatchEveryIsNotCfgured);
        }
        ewsTelnetWrite(ewsContext,buf);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_PROTOCOL) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_common_Proto);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleProtocolGet(unit, aclId, rulenum, &val);
        switch (val)
        {
        case L7_ACL_PROTOCOL_ICMP:
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_ICMP,pStrInfo_common_Icmp_1);
          break;
        case L7_ACL_PROTOCOL_IGMP:
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_IGMP,pStrInfo_common_Igmp_2);
          break;
        case L7_ACL_PROTOCOL_IP:     /* ???? */
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_IP,pStrInfo_common_IpOption);
          break;
        case L7_ACL_PROTOCOL_TCP:
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_TCP,pStrInfo_qos_Tcp_1);
          break;
        case L7_ACL_PROTOCOL_UDP:
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_UDP,pStrInfo_qos_Udp_1);
          break;
        default:
          sprintf(protStr,"%d",val);
        }

        ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,protStr);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCIP) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_SrcIpAddr);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleSrcIpMaskGet(unit, aclId, rulenum, &ipAddr, &ipMask);
        ipMask = ipMask^(~(L7_uint32 )0);
        usmDbInetNtoa(ipAddr,ipStr);
        ipStr[15] = '\0';
        usmDbInetNtoa(ipMask,maskStr);
        maskStr[15] = '\0';
        ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,ipStr);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCIP_MASK) == L7_TRUE)
        {
          cliFormat(ewsContext, pStrInfo_qos_SrcIpMask);
          ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,maskStr);
        }
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCPORT) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_SrcL4PortKeyword);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleSrcL4PortGet(unit, aclId, rulenum, &val);
        switch (val)
        {
        case L7_ACL_L4PORT_DOMAIN:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_DOMAIN,pStrInfo_qos_Domain_1);
          break;
        case L7_ACL_L4PORT_ECHO:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_ECHO,pStrInfo_qos_Echo);
          break;
        case L7_ACL_L4PORT_FTP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_FTP,pStrInfo_common_Ftp);
          break;
        case L7_ACL_L4PORT_FTPDATA:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_FTPDATA,pStrInfo_qos_Ftpdata);
          break;
        case L7_ACL_L4PORT_HTTP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_HTTP,pStrInfo_qos_WwwHttp);
          break;
        case L7_ACL_L4PORT_SMTP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_SMTP,pStrInfo_qos_Smtp);
          break;
        case L7_ACL_L4PORT_SNMP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_SNMP,pStrInfo_common_Snmp_1);
          break;
        case L7_ACL_L4PORT_TELNET:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_TELNET,pStrInfo_common_Telnet);
          break;
        case L7_ACL_L4PORT_TFTP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_TFTP,pStrInfo_common_Tftp_1);
          break;
        default:
          sprintf(protStr, "%d", val);
          break;
        }
        ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,protStr);

      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCSTARTPORT) == L7_TRUE)
      {
        usmDbQosAclRuleSrcL4PortRangeGet(unit, aclId, rulenum, &val, &val2);
        if (val == val2)
        {
          cliFormat(ewsContext,  pStrInfo_qos_SrcL4Port);       /* Source L4 Port */
          ewsTelnetPrintf (ewsContext, "%d",val);
        }
        else
        {
          cliFormat(ewsContext, pStrInfo_qos_SrcL4StartPort);      /* Source L4 Start Port */
          ewsTelnetPrintf (ewsContext, "%d",val);
          cliFormat(ewsContext, pStrInfo_qos_SrcL4EndPort);      /* Source L4 End Port */
          ewsTelnetPrintf (ewsContext, "%d",val2);
        }
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTIP) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_common_DstIpAddr);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleDstIpMaskGet(unit, aclId, rulenum, &ipAddr, &ipMask);
        usmDbInetNtoa(ipAddr,ipStr);
        ipStr[15] = '\0';
        ipMask = ipMask^(~(L7_uint32 )0);
        usmDbInetNtoa(ipMask,maskStr);
        maskStr[15] = '\0';

        ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,ipStr);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTIP_MASK) == L7_TRUE)
        {
          cliFormat(ewsContext, pStrInfo_qos_DstIpMask);
          ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,maskStr);
        }
      }
      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTPORT) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_DstL4PortKeyword);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleDstL4PortGet(unit, aclId, rulenum, &val);
        switch (val)
        {
        case 53:
          osapiStrncpySafe(protStr, pStrInfo_qos_53, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_Domain_1, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 7:
          osapiStrncpySafe(protStr, pStrInfo_qos_7, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_Echo, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 21:
          osapiStrncpySafe(protStr, pStrInfo_qos_21, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_common_Ftp, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 20:
          osapiStrncpySafe(protStr, pStrInfo_qos_20, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_Ftpdata, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 80:
          osapiStrncpySafe(protStr, pStrInfo_qos_80, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_WwwHttp, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 25:
          osapiStrncpySafe(protStr, pStrInfo_qos_25, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_Smtp, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 161:
          osapiStrncpySafe(protStr, pStrInfo_qos_161, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_common_Snmp_1, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 23:
          osapiStrncpySafe(protStr, pStrInfo_qos_23, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_common_Telnet, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 69:
          osapiStrncpySafe(protStr, pStrInfo_qos_69, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_common_Tftp_1, sizeof(protStr));
          strcat(protStr, ")");
          break;
        default:
          sprintf(protStr, "%d", val);
          break;
        }
        ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,protStr);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTSTARTPORT) == L7_TRUE)
      {
        usmDbQosAclRuleDstL4PortRangeGet(unit, aclId, rulenum, &val, &val2);
        if (val == val2)
        {
          cliFormat(ewsContext,  pStrInfo_qos_DstL4Port);      /* Destination L4 Port */
          ewsTelnetPrintf (ewsContext, "%d",val);
        }
        else
        {
          cliFormat(ewsContext, pStrInfo_qos_DstL4StartPort);      /* Destination L4 Start Port */
          ewsTelnetPrintf (ewsContext, "%d", val);
          cliFormat(ewsContext, pStrInfo_qos_DstL4EndPort);      /* Destination L4 End Port */
          ewsTelnetPrintf (ewsContext, "%d", val2);
        }
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_IPDSCP) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_IpDscp);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleIPDscpGet(unit, aclId, rulenum, &val);
        memset (stat, 0, sizeof(stat));
        cliAclConvertDSCPValToString(val, stat);
        ewsTelnetPrintf (ewsContext, stat);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_IPPREC) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_IpPrecedence);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleIPPrecedenceGet(unit, aclId, rulenum, &val);
        ewsTelnetPrintf (ewsContext, "%d",val);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_IPTOS) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_IpTos);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleIPTosGet(unit, aclId, rulenum, &val, &val2);
        ewsTelnetPrintf (ewsContext, "%s: 0x%x  %s: 0x%x", pStrInfo_qos_TosBits, val, pStrInfo_qos_TosMask, ~val2 & 0x000000FF );

      }

      /* Log */
      if (usmDbQosAclRuleLoggingAllowed(unit, aclId, rulenum) == L7_SUCCESS)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_LOGGING) == L7_TRUE)
        {
          if (usmDbQosAclRuleLoggingGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_common_Log_1);
            ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, (val == L7_TRUE) ? pStrInfo_common_True : pStrInfo_common_False);
          }
        }
      }

      /* Assign Queue */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_ASSIGN_QUEUEID) == L7_TRUE)
        {
          if (usmDbQosAclRuleAssignQueueIdGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_AsSignQueue);
            ewsTelnetPrintf (ewsContext, "%d",val);
          }
        }
      }

      /* Mirror Interface */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_MIRROR_INTF) == L7_TRUE)
        {
          if (usmDbQosAclRuleMirrorIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
              usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_MirrorIntf);
            ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(u, s, p));
          }
        }
      }

      /* Redirect Interface */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_REDIRECT_INTF) == L7_TRUE)
        {
          if (usmDbQosAclRuleRedirectIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
              usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_RedirectIntf);
            ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(u, s, p));
          }
        }
      }
      /* Time Range Information */
     if(usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) ==L7_TRUE)
     { 
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum, ACL_TIME_RANGE_NAME) == L7_TRUE)
        {
          if (usmDbQosAclRuleTimeRangeNameGet(unit, aclId, rulenum, timeRangeName) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_TimeRangeName);
            ewsTelnetPrintf (ewsContext, "%s", timeRangeName);
          }
          if(usmDbQosAclRuleStatusGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_TimeRangeStatus);
            if( val == L7_ACL_RULE_STATUS_ACTIVE)
            {
               ewsTelnetPrintf(ewsContext, pStrInfo_qos_TimeRangeStatusActive);
            }
            else if(val == L7_ACL_RULE_STATUS_INACTIVE)
            {
               ewsTelnetPrintf(ewsContext, pStrInfo_qos_TimeRangeStatusInactive);     
            }
          }        
       }
      }
     }
    numwrites ++;
    }
    while(((rc = usmDbQosAclRuleGetNext(unit, aclId, rulenum, &rulenum)) == L7_SUCCESS) 
        && (numwrites < (CLI_MAX_SCROLL_LINES-21)));
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else
    {
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      osapiSnprintf(stat, sizeof(stat), "%s %s", pStrInfo_qos_ShowIpAccessList, (L7_uchar8 *)argv[index+1]);
      cliAlternateCommandSet(stat);
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ShowAclSummary);
  }
}

/*********************************************************************
*
* @purpose  Display MAC access list information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  command show mac access-lists [<name>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpv6AccessLists(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_RC_t rc = L7_SUCCESS;
  static L7_uint32 aclId;
  static L7_uint32 intfIndex;
  static L7_uint32 vlanIndex;
  static L7_ACL_DIRECTION_t direction;
  L7_uint32 argName = 1;
  L7_uint32 unit;
  static L7_uint32 rulenum;
  L7_uint32 rules;
  L7_char8 aclName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numwrites, numArgs;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList[L7_ACL_DIRECTION_TOTAL];
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList[L7_ACL_DIRECTION_TOTAL];
  L7_ACL_DIRECTION_t dir;
  L7_uint32 u, s, p;
  L7_BOOL commaFlag = L7_FALSE;
  L7_uint32 currentACLs, maxACLs;
  L7_char8 protStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val, val2;
  L7_in6_prefix_t ipv6Prefix;
  L7_BOOL every, firstLine, aclsConfigured;
  L7_BOOL printLine, currentAclInfoPrinted, currentDirectionPrinted;
  L7_uint32 interfacesColumnEnd, vlansColumnStart, vlansColumnEnd;
  L7_BOOL interfaceColumnComplete, vlanColumnComplete, i;
  L7_BOOL firstTime = L7_TRUE;

  /* Time Range Variables */
  L7_uchar8 timeRangeName[L7_CLI_MAX_STRING_LENGTH]; 

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if(unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();

  /* Summary */
  if(numArgs == 0)     /* parameter check */
  {
    cliCmdScrollSet(L7_FALSE);
    if(cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      numwrites = 0;
      /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      aclsConfigured = L7_FALSE;
      if (usmDbQosAclNamedIndexGetFirst(unit, L7_ACL_TYPE_IPV6, &aclId) == L7_SUCCESS)
      {
        if (usmDbQosAclNameGet(unit, aclId, aclName) == L7_SUCCESS)
        {
          aclsConfigured = L7_TRUE;
        }
      }

      if(aclsConfigured != L7_TRUE)
      {  /* no ACLs configured */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoAclsAreCfgured_1);
      }

      numwrites = 0;
      direction = L7_INBOUND_ACL;
      intfIndex = 0;
      vlanIndex = 0;

      cliSyntaxTop(ewsContext);
    }

    if(usmDbQosAclMaxNumGet(unit, &maxACLs) == L7_SUCCESS)
    {
      if(usmDbQosAclCurrNumGet(unit, &currentACLs) == L7_SUCCESS)
      {
        sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, buf,pStrErr_qos_CfgMacAclMaxAndCurrentMacAcl, currentACLs, maxACLs);
        ewsTelnetWrite(ewsContext, buf);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_Ipv6AclNameRulesDirectionIntfVlanS );
      ewsTelnetWrite(ewsContext,
                     "\r\n-------------------------------  -----  ---------  --------------   ----------");
      interfacesColumnEnd = 15;
      vlansColumnStart = 17;
      vlansColumnEnd = 28;

    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 4, 8, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_Ipv6AclNameRulesDirectionIntfS );
      ewsTelnetWrite(ewsContext,
                     "\r\n------------------------------- ----- --------- -------------------------");
      interfacesColumnEnd = 32;
      vlansColumnStart = 0;
      vlansColumnEnd = 0;
    }
    ewsTelnetWrite(ewsContext,
                   pStrInfo_common_CrLf);

    currentAclInfoPrinted = L7_FALSE;
    currentDirectionPrinted = L7_FALSE;
    rc = L7_SUCCESS;

    for (dir = L7_INBOUND_ACL; dir < L7_ACL_DIRECTION_TOTAL; dir++)
    {
      if(usmDbQosAclAssignedIntfDirListGet(unit, aclId, dir, &intfList[dir]) != L7_SUCCESS)
      {
        intfList[dir].count = 0;
      }
      if(usmDbQosAclAssignedVlanDirListGet(unit, aclId, dir, &vlanList[dir]) != L7_SUCCESS)
      {
        vlanList[dir].count = 0;
      }
    }

    while((rc == L7_SUCCESS) && numwrites < (CLI_MAX_SCROLL_LINES-6))
    {
      if (currentAclInfoPrinted == L7_FALSE)
      {
        rc = usmDbQosAclNameGet(unit, aclId, aclName);
        ewsTelnetPrintf (ewsContext, "%-33s", aclName);

        /* count the rules in this ACL */
        rules = 0;

        rc = usmDbQosAclRuleGetFirst(unit, aclId, &rulenum);
        if(rc == L7_SUCCESS)
        {
          do
          {
            rules++;
          } while(usmDbQosAclRuleGetNext(unit, aclId, rulenum, &rulenum) == L7_SUCCESS);
        }

        ewsTelnetPrintf (ewsContext, "%-7d", rules);
        currentAclInfoPrinted = L7_TRUE;
      }

      if (currentDirectionPrinted == L7_FALSE)
      {
        if ((intfList[direction].count > 0) || (vlanList[direction].count > 0))
        {
          switch (direction)
          {
          case L7_INBOUND_ACL:
            sprintf(buf, "%-11s", pStrInfo_qos_Inbound_1);
            break;
          case L7_OUTBOUND_ACL:
            sprintf(buf, "%-11s", pStrInfo_qos_Outbound_1);
            break;
          default:
            /* unknown direction enumeration */
            strcpy(buf, "        ");
          }
          currentDirectionPrinted = L7_TRUE;
          ewsTelnetWrite(ewsContext, buf);
        }
      }

      memset (buf, 0,  sizeof(buf));
      memset (stat, 0,sizeof(stat));
      interfaceColumnComplete = L7_FALSE;
      vlanColumnComplete = L7_FALSE;

      while ((intfIndex < intfList[direction].count) && (interfaceColumnComplete == L7_FALSE))
      {
        if(usmDbUnitSlotPortGet(intfList[direction].intIfNum[intfIndex], &u, &s, &p) == L7_SUCCESS)
        {
          osapiStrncpySafe(stat, cliDisplayInterfaceHelp(u, s, p), sizeof(stat));
          if (intfIndex < (intfList[direction].count-1))
          {
            strcat(stat, ", ");
          }
          if ((strlen(buf) + strlen(stat)) < interfacesColumnEnd)
          {
            strcat(buf, stat);
            intfIndex++;
            if(intfIndex == intfList[direction].count)
            {
              interfaceColumnComplete = L7_TRUE;
            }
          }
          else
          {
            interfaceColumnComplete = L7_TRUE;
          }
        }
      }
      /* print spaces to pad over to start of vlan column, if necessary */
      if (vlanList[direction].count > 0)
      {
        for (i=strlen(buf); i < vlansColumnStart; i++)
        {
          strcat(buf, " ");
        }
      }

      while ((vlanIndex < vlanList[direction].count) && (vlanColumnComplete == L7_FALSE))
      {
        sprintf(stat, "%u", vlanList[direction].vlanNum[vlanIndex]);
        if (vlanIndex < (vlanList[direction].count-1))
        {
          strcat(stat, ", ");
        }
        if ((strlen(buf) + strlen(stat)) < vlansColumnEnd)
        {
          strcat(buf, stat);
          vlanIndex++;
          if(vlanIndex == vlanList[direction].count)
          {
            vlanColumnComplete = L7_TRUE;
          }
        }
        else
        {
          vlanColumnComplete = L7_TRUE;
        }
      }

      if ((interfaceColumnComplete == L7_TRUE) || (vlanColumnComplete == L7_TRUE))
      {
        ewsTelnetWrite(ewsContext,buf);
        memset (buf, 0,sizeof(buf));
      }

      rc = L7_FAILURE;
      while( rc != L7_SUCCESS)
      {
        if (intfIndex < intfList[direction].count)
        {
          /* not finished with this acl/direction; move to new line and pad to Interfaces column */
          ewsTelnetWrite(ewsContext,  "\r\n                                                   ");
          numwrites++;
          rc = L7_SUCCESS;
          break;
        }
        else if (vlanIndex < vlanList[direction].count)
        {
          /* not finished with this acl/direction; move to new line and pad to vlans column */
          ewsTelnetWrite(ewsContext,  "\r\n                                                   ");
          numwrites++;
          rc = L7_SUCCESS;
          break;
        }
        else if (direction < L7_OUTBOUND_ACL)
        {
          /* finished outputting interfaces and/or vlans for current acl/direction; move to next direction, if any */
          if (((intfList[L7_INBOUND_ACL].count > 0) || (vlanList[L7_INBOUND_ACL].count > 0)) &&
              ((intfList[L7_OUTBOUND_ACL].count > 0) || (vlanList[L7_OUTBOUND_ACL].count > 0)))
          {
            ewsTelnetWrite(ewsContext,  "\r\n                                        ");
            numwrites++;
          }
          currentDirectionPrinted = L7_FALSE;
          direction = L7_OUTBOUND_ACL;
          intfIndex = 0;
          vlanIndex = 0;
          rc = L7_SUCCESS;
          break;
        }
        else
        {
          /* done with this acl, see if there is another to work on */
          rc = usmDbQosAclNamedIndexGetNext(unit, L7_ACL_TYPE_IPV6, aclId, &aclId);
          if (rc == L7_SUCCESS)
          {
            currentAclInfoPrinted = L7_FALSE;
            intfIndex = 0;
            vlanIndex = 0;
            currentDirectionPrinted = L7_FALSE;
            direction = L7_INBOUND_ACL;
            for (dir = L7_INBOUND_ACL; dir < L7_ACL_DIRECTION_TOTAL; dir++)
            {
              if(usmDbQosAclAssignedIntfDirListGet(unit, aclId, dir, &intfList[dir]) != L7_SUCCESS)
              {
                intfList[dir].count = 0;
              }
              if(usmDbQosAclAssignedVlanDirListGet(unit, aclId, dir, &vlanList[dir]) != L7_SUCCESS)
              {
                vlanList[dir].count = 0;
              }
            }
            ewsTelnetWrite(ewsContext,  pStrInfo_common_CrLf);
            numwrites++;
          }
          else
          {
            /* done with all acl's */
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliSyntaxReturnPrompt (ewsContext, "");
          }
        }
      }
    }

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_qos_ShowIpv6AccessList);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }

  /* Specific IPv6 Access-list */
  else if(numArgs == 1)
  {
    cliCmdScrollSet(L7_FALSE);
    numwrites = 0;
    if(cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      firstTime = L7_FALSE;
      /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }

    if(strlen(argv[index+argName]) > L7_ACL_NAME_LEN_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    osapiStrncpySafe(aclName, argv[index+argName], sizeof(aclName));

    if(usmDbQosAclNameStringCheck(unit, aclName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    if((usmDbQosAclNameToIndex(unit, aclName, &aclId) != L7_SUCCESS) ||
       (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IPV6, aclId) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
    }

    if(firstTime == L7_TRUE)
    {
      ewsTelnetPrintf (ewsContext, "\r\n%s %s", pStrInfo_qos_AclName, aclName);
      cliSyntaxBottom(ewsContext);

      /* retrieve interface lists for inbound and outbound directions */
      (void)usmDbQosAclAssignedIntfDirListGet(unit, aclId, L7_INBOUND_ACL, &intfList[L7_INBOUND_ACL]);
      (void)usmDbQosAclAssignedIntfDirListGet(unit, aclId, L7_OUTBOUND_ACL, &intfList[L7_OUTBOUND_ACL]);

      direction = L7_INBOUND_ACL;
      while (direction < L7_ACL_DIRECTION_TOTAL)
      {
        intfIndex = 0;
        memset (buf, 0, sizeof(buf));
        memset (stat, 0, sizeof(stat));
        if(intfList[direction].count >= 1)
        {
          if (direction == L7_INBOUND_ACL)
          {
            sprintf(buf,"\r\n%s %s: ", pStrInfo_qos_Inbound, pStrInfo_qos_Intf_1);
          }
          else
          {
            sprintf(buf,"\r\n%s %s: ", pStrInfo_qos_Outbound, pStrInfo_qos_Intf_1);
          }
          ewsTelnetWrite(ewsContext, buf);
          memset (buf, 0, sizeof(buf));
        }

        commaFlag = L7_TRUE;

        while(intfIndex < intfList[direction].count)
        {
          if(intfIndex >= (intfList[direction].count-1))
          {
            commaFlag = L7_FALSE;
          }
          if(usmDbUnitSlotPortGet(intfList[direction].intIfNum[intfIndex], &u, &s, &p) == L7_SUCCESS)
          {
            printLine = L7_FALSE;
            osapiStrncpySafe(stat, cliDisplayInterfaceHelp(u, s, p), sizeof(stat));
            if(commaFlag == L7_TRUE)
            {
              strcat(stat, ", ");
            }
            if((strlen(buf) + strlen(stat)) < 48)
            {
              strcat(buf, stat);
              intfIndex++;
              if(intfIndex == intfList[direction].count)
              {
                printLine = L7_TRUE;
              }
            }
            else
            {
              printLine = L7_TRUE;
            }

            if(printLine == L7_TRUE)
            {
              strcpy(stat, buf);
              firstLine = L7_FALSE;
              ewsTelnetWrite(ewsContext,stat);
              memset (buf, 0,sizeof(buf));
            }
          }
        }
        direction++;
      }

      /* retrieve vlan lists for inbound and outbound directions */
      (void)usmDbQosAclAssignedVlanDirListGet(unit, aclId, L7_INBOUND_ACL, &vlanList[L7_INBOUND_ACL]);
      (void)usmDbQosAclAssignedVlanDirListGet(unit, aclId, L7_OUTBOUND_ACL, &vlanList[L7_OUTBOUND_ACL]);

      direction = L7_INBOUND_ACL;
      while (direction < L7_ACL_DIRECTION_TOTAL)
      {
        intfIndex = 0;
        memset (stat, 0, sizeof(stat));
        memset (buf, 0, sizeof(buf));
        if(vlanList[direction].count >= 1)
        {
          if (direction == L7_INBOUND_ACL)
          {
            sprintf(buf,"\r\n%s %s: ", pStrInfo_qos_Inbound, pStrInfo_qos_Vlan_1);
          }
          else
          {
            sprintf(buf,"\r\n%s %s: ", pStrInfo_qos_Outbound, pStrInfo_qos_Vlan_1);
          }
          ewsTelnetWrite(ewsContext, buf);
          memset (buf, 0, sizeof(buf));
        }

        commaFlag = L7_TRUE;

        while(intfIndex < vlanList[direction].count)
        {
          if(intfIndex >= (vlanList[direction].count-1))
          {
            commaFlag = L7_FALSE;
          }
          printLine = L7_FALSE;
          sprintf(stat, "%u", vlanList[direction].vlanNum[intfIndex]);
          if(commaFlag == L7_TRUE)
          {
            strcat(stat, ", ");
          }
          if((strlen(buf) + strlen(stat)) < 48)
          {
            strcat(buf, stat);
            intfIndex++;
            if(intfIndex == vlanList[direction].count)
            {
              printLine = L7_TRUE;
            }
          }
          else
          {
            printLine = L7_TRUE;
          }

          if(printLine == L7_TRUE)
          {
            strcpy(stat, buf);
            firstLine = L7_FALSE;
            ewsTelnetWrite(ewsContext,stat);
            memset (buf, 0,sizeof(buf));
          }
        }
        direction++;
      }
      
      /* since this is the first time, get a new rulenum */
      rc = usmDbQosAclRuleGetFirst(unit, aclId, &rulenum);
      if(rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_NoRulesHaveBeenCfguredForAcl);
      }
    }

    do /* this section is executed for both the first and successive times */
    {
      cliSyntaxBottom(ewsContext);
      ewsTelnetPrintf (ewsContext, "\r\n%s %d",pStrInfo_qos_RuleNum,rulenum);

      if(usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_ACTION) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_common_Action);
        memset (buf, 0, sizeof(buf));
        rc = usmDbQosAclRuleActionGet(unit, aclId, rulenum, &val);
        if(val == L7_ACL_PERMIT /*ACL_PERMIT*/)
        {
          ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_Permit_1);
        }
        if(val == L7_ACL_DENY /*ACL_DENY*/)
        {
          ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_Deny_2);
        }
      }

      if(usmDbQosAclIsFieldConfigured(unit, aclId,rulenum, ACL_EVERY) == L7_TRUE)
      { /* if the match every is set to true */
        cliFormat(ewsContext, pStrInfo_qos_MatchEvery);
        every = L7_FALSE;
        rc = usmDbQosAclRuleEveryGet(unit, aclId, rulenum, &every);
        if(every == L7_TRUE)
        {
          sprintf(buf, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_True);
        }
        else
        {
          sprintf(buf, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_False);
        }
        ewsTelnetWrite(ewsContext,buf);
      }
      else if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_EVERY) == L7_FALSE)
      {
        cliFormat(ewsContext, pStrInfo_qos_MatchEvery);
        memset (buf, 0, sizeof(buf));
        rc = usmDbQosAclRuleEveryGet(unit, aclId, rulenum, &every);
        if ((every == L7_FALSE) && (rulenum != L7_ACL_DEFAULT_RULE_NUM))
        {
          sprintf(buf, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_False);
        }
        else
        {
          sprintfAddBlanks (0, 0, 0, 0, pStrErr_common_Error, buf, pStrErr_common_CfgAclsStringFmt, pStrErr_qos_MatchEveryIsNotCfgured);
        }
        ewsTelnetWrite(ewsContext,buf);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_PROTOCOL) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_common_Proto);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleProtocolGet(unit, aclId, rulenum, &val);
        switch (val)
        {
        case L7_ACL_PROTOCOL_ICMPV6:
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_ICMPV6,pStrInfo_common_Icmpv6_1);
          break;
        case L7_ACL_PROTOCOL_IP:   /* ???? */
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_IP, pStrInfo_common_Diffserv_5);
          break;
        case L7_ACL_PROTOCOL_TCP:
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_TCP,pStrInfo_qos_Tcp_1);
          break;
        case L7_ACL_PROTOCOL_UDP:
          sprintf(protStr, "%d(%s)",L7_ACL_PROTOCOL_UDP,pStrInfo_qos_Udp_1);
          break;
        default:
          sprintf(protStr,"%d",val);
        }

        ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,protStr);
      }

      if(usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_SRCIPV6) == L7_TRUE)
      {
        memset(&ipv6Prefix, 0, sizeof(ipv6Prefix));
        if(usmDbQosAclRuleSrcIpv6AddrGet(unit, aclId, rulenum, &ipv6Prefix) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_SrcIpAddr);

          if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ipv6Prefix.in6Addr, buf, sizeof(buf)) != L7_NULLPTR)
          {
            ewsTelnetWrite(ewsContext, buf);
          }
          ewsTelnetPrintf (ewsContext, "/%u", ipv6Prefix.in6PrefixLen);
        }
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCPORT) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_SrcL4PortKeyword);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleSrcL4PortGet(unit, aclId, rulenum, &val);
        switch (val)
        {
        case L7_ACL_L4PORT_DOMAIN:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_DOMAIN,pStrInfo_qos_Domain_1);
          break;
        case L7_ACL_L4PORT_ECHO:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_ECHO,pStrInfo_qos_Echo);
          break;
        case L7_ACL_L4PORT_FTP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_FTP,pStrInfo_common_Ftp);
          break;
        case L7_ACL_L4PORT_FTPDATA:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_FTPDATA,pStrInfo_qos_Ftpdata);
          break;
        case L7_ACL_L4PORT_HTTP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_HTTP,pStrInfo_qos_WwwHttp);
          break;
        case L7_ACL_L4PORT_SMTP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_SMTP,pStrInfo_qos_Smtp);
          break;
        case L7_ACL_L4PORT_SNMP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_SNMP,pStrInfo_common_Snmp_1);
          break;
        case L7_ACL_L4PORT_TELNET:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_TELNET,pStrInfo_common_Telnet);
          break;
        case L7_ACL_L4PORT_TFTP:
          sprintf(protStr, "%d(%s)",L7_ACL_L4PORT_TFTP,pStrInfo_common_Tftp_1);
          break;
        default:
          sprintf(protStr, "%d", val);
          break;
        }
        ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,protStr);

      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_SRCSTARTPORT) == L7_TRUE)
      {
        usmDbQosAclRuleSrcL4PortRangeGet(unit, aclId, rulenum, &val, &val2);
        if (val == val2)
        {
          cliFormat(ewsContext,  pStrInfo_qos_SrcL4Port);     /* Source L4 Port */
          ewsTelnetPrintf (ewsContext, "%d",val);
        }
        else
        {
          cliFormat(ewsContext, pStrInfo_qos_SrcL4StartPort);    /* Source L4 Start Port */
          ewsTelnetPrintf (ewsContext, "%d",val);
          cliFormat(ewsContext, pStrInfo_qos_SrcL4EndPort);    /* Source L4 End Port */
          ewsTelnetPrintf (ewsContext, "%d",val2);
        }
      }

      if(usmDbQosAclIsFieldConfigured(unit, aclId,rulenum, ACL_DSTIPV6) == L7_TRUE)
      {
        memset(&ipv6Prefix, 0, sizeof(ipv6Prefix));
        if(usmDbQosAclRuleDstIpv6AddrGet(unit, aclId, rulenum, &ipv6Prefix) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_DstIpAddress);

          if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ipv6Prefix.in6Addr, buf, sizeof(buf)) != L7_NULLPTR)
          {
            ewsTelnetWrite(ewsContext, buf);
          }
          ewsTelnetPrintf (ewsContext, "/%u", ipv6Prefix.in6PrefixLen);
        }
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTPORT) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_DstL4PortKeyword);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleDstL4PortGet(unit, aclId, rulenum, &val);
        switch (val)
        {
        case 53:
          osapiStrncpySafe(protStr, pStrInfo_qos_53, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_Domain_1, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 7:
          osapiStrncpySafe(protStr, pStrInfo_qos_7, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_Echo, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 21:
          osapiStrncpySafe(protStr, pStrInfo_qos_21, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_common_Ftp, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 20:
          osapiStrncpySafe(protStr, pStrInfo_qos_20, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_Ftpdata, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 80:
          osapiStrncpySafe(protStr, pStrInfo_qos_80, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_WwwHttp, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 25:
          osapiStrncpySafe(protStr, pStrInfo_qos_25, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_qos_Smtp, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 161:
          osapiStrncpySafe(protStr, pStrInfo_qos_161, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_common_Snmp_1, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 23:
          osapiStrncpySafe(protStr, pStrInfo_qos_23, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_common_Telnet, sizeof(protStr));
          strcat(protStr, ")");
          break;
        case 69:
          osapiStrncpySafe(protStr, pStrInfo_qos_69, sizeof(protStr));
          osapiStrncat(protStr, pStrInfo_common_Tftp_1, sizeof(protStr));
          strcat(protStr, ")");
          break;
        default:
          sprintf(protStr, "%d", val);
          break;
        }
        ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,protStr);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_DSTSTARTPORT) == L7_TRUE)
      {
        usmDbQosAclRuleDstL4PortRangeGet(unit, aclId, rulenum, &val, &val2);
        if (val == val2)
        {
          cliFormat(ewsContext,  pStrInfo_qos_DstL4Port);    /* Destination L4 Port */
          ewsTelnetPrintf (ewsContext, "%d",val);
        }
        else
        {
          cliFormat(ewsContext, pStrInfo_qos_DstL4StartPort);    /* Destination L4 Start Port */
          ewsTelnetPrintf (ewsContext, "%d", val);
          cliFormat(ewsContext, pStrInfo_qos_DstL4EndPort);    /* Destination L4 End Port */
          ewsTelnetPrintf (ewsContext, "%d", val2);
        }
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_IPDSCP) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_IpDscp);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleIPDscpGet(unit, aclId, rulenum, &val);
        memset (stat, 0, sizeof(stat));
        cliAclConvertDSCPValToString(val, stat);
        ewsTelnetPrintf (ewsContext, stat);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_IPPREC) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_IpPrecedence);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleIPPrecedenceGet(unit, aclId, rulenum, &val);
        ewsTelnetPrintf (ewsContext, "%d",val);
      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_IPTOS) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_IpTos);
        memset (buf, 0, sizeof(buf));
        usmDbQosAclRuleIPTosGet(unit, aclId, rulenum, &val, &val2);
        ewsTelnetPrintf (ewsContext, "%s: 0x%x  %s: 0x%x", pStrInfo_qos_TosBits, val, pStrInfo_qos_TosMask, ~val2 & 0x000000FF );

      }

      if (usmDbQosAclIsFieldConfigured(unit, aclId, rulenum, ACL_FLOWLBLV6) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_qos_FlowLabel);
        usmDbQosAclRuleIpv6FlowLabelGet(unit, aclId, rulenum, &val);
        ewsTelnetPrintf (ewsContext, "%d",val);
      }

      /* Log */
      if (usmDbQosAclRuleLoggingAllowed(unit, aclId, rulenum) == L7_SUCCESS)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_LOGGING) == L7_TRUE)
        {
          if (usmDbQosAclRuleLoggingGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_common_Log_1);
            ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, (val == L7_TRUE) ? pStrInfo_common_True : pStrInfo_common_False);
          }
        }
      }

      /* Assign Queue */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_ASSIGN_QUEUEID) == L7_TRUE)
        {
          if (usmDbQosAclRuleAssignQueueIdGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_AsSignQueue);
            ewsTelnetPrintf (ewsContext, "%d",val);
          }
        }
      }

      /* Mirror Interface */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_MIRROR_INTF) == L7_TRUE)
        {
          if (usmDbQosAclRuleMirrorIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
              usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_MirrorIntf);
            ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(u, s, p));
          }
        }
      }

      /* Redirect Interface */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum,ACL_REDIRECT_INTF) == L7_TRUE)
        {
          if (usmDbQosAclRuleRedirectIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
              usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_RedirectIntf);
            ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(u, s, p));
          }
        }
      }
      /* Time Range Information */
   if(usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) ==L7_TRUE)
   {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE)
      {
        if (usmDbQosAclIsFieldConfigured(unit, aclId,rulenum, ACL_TIME_RANGE_NAME) == L7_TRUE)
        {
          if (usmDbQosAclRuleTimeRangeNameGet(unit, aclId, rulenum, timeRangeName) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_TimeRangeName);
            ewsTelnetPrintf (ewsContext, "%s", timeRangeName);
          }
          if(usmDbQosAclRuleStatusGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_TimeRangeStatus);
            if( val == L7_ACL_RULE_STATUS_ACTIVE)
            {
               ewsTelnetPrintf(ewsContext, pStrInfo_qos_TimeRangeStatusActive);
            }
            else if( val == L7_ACL_RULE_STATUS_INACTIVE)
            {
               ewsTelnetPrintf(ewsContext, pStrInfo_qos_TimeRangeStatusInactive);
            }
          }
        }
      }
    }
    numwrites ++;
    } while(((rc = usmDbQosAclRuleGetNext(unit, aclId, rulenum, &rulenum)) == L7_SUCCESS) 
      && numwrites < (CLI_MAX_SCROLL_LINES-21));

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else
    {
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      osapiSnprintf(stat, sizeof(stat), "%s %s", pStrInfo_qos_ShowIpv6AccessList, aclName);

      cliAlternateCommandSet(stat);
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ShowIpv6Acl);
  }
}

/*********************************************************************
*
* @purpose  Display MAC access list information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  command show mac access-lists [<name>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowMacAccessLists(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_RC_t rc = L7_SUCCESS;
  static L7_uint32 aclId;
  static L7_uint32 intfIndex;
  static L7_uint32 vlanIndex;
  static L7_ACL_DIRECTION_t direction;
  static L7_uint32 rulenum;
  L7_uint32 argName = 1;
  L7_uint32 unit;
  L7_uint32 rules;
  L7_char8 macAclName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numwrites, numArgs;
  L7_ACL_DIRECTION_t dir;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList[L7_ACL_DIRECTION_TOTAL];
  L7_ACL_ASSIGNED_VLAN_LIST_t vlanList[L7_ACL_DIRECTION_TOTAL];
  L7_uint32 u, s, p, i;
  L7_BOOL commaFlag = L7_FALSE;
  L7_uint32 currentMacACLs, maxMacACLs;
  L7_uint32 val, startVal, endVal;
  L7_uint32 keyId;
  L7_uchar8 eTypeString[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN];
  L7_uchar8 srcMacMask[L7_MAC_ADDR_LEN];
  L7_uchar8 dstMacAddr[L7_MAC_ADDR_LEN];
  L7_uchar8 dstMacMask[L7_MAC_ADDR_LEN];
  L7_BOOL every, firstLine, aclsConfigured;
  L7_BOOL printLine, currentAclInfoPrinted, currentDirectionPrinted;
  L7_uint32 interfacesColumnEnd, vlansColumnStart, vlansColumnEnd;
  L7_BOOL interfaceColumnComplete, vlanColumnComplete;
  L7_BOOL firstTime = L7_TRUE;

  /* Time Range Variables */
  L7_uchar8 timeRangeName[L7_CLI_MAX_STRING_LENGTH]; 
  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if(unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();

  /* Summary */
  if(numArgs == 0)     /* parameter check */
  {
    cliCmdScrollSet(L7_FALSE);
    if(cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      numwrites = 0;
      /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      aclsConfigured = L7_FALSE;
      if (usmDbQosAclMacIndexGetFirst(unit, &aclId) == L7_SUCCESS)
      {
        if (usmDbQosAclMacNameGet(unit, aclId, macAclName) == L7_SUCCESS)
        {
          aclsConfigured = L7_TRUE;
        }
      }

      if(aclsConfigured != L7_TRUE)
      {  /* no ACLs configured */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_qos_NoAclsAreCfgured_1);
      }

      numwrites = 0;
      direction = L7_INBOUND_ACL;
      intfIndex = 0;
      vlanIndex = 0;

      cliSyntaxTop(ewsContext);
    }

    if(usmDbQosAclMaxNumGet(unit, &maxMacACLs) == L7_SUCCESS)
    {
      if(usmDbQosAclCurrNumGet(unit, &currentMacACLs) == L7_SUCCESS)
      {
        sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, buf,pStrErr_qos_CfgMacAclMaxAndCurrentMacAcl, currentMacACLs, maxMacACLs);
        ewsTelnetWrite(ewsContext, buf);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_MacAclNameRulesDirectionIntfVlanS );
      ewsTelnetWrite(ewsContext,
                     "\r\n-------------------------------  -----  ---------  --------------   ----------");
      interfacesColumnEnd = 15;
      vlansColumnStart = 17;
      vlansColumnEnd = 28;
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 5, 8, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_MacAclNameRulesDirectionIntfS );
      ewsTelnetWrite(ewsContext,
                     "\r\n------------------------------- ----- --------- -------------------------");
      interfacesColumnEnd = 32;
      vlansColumnStart = 0;
      vlansColumnEnd = 0;
    }
    ewsTelnetWrite(ewsContext,
                   pStrInfo_common_CrLf);

    currentAclInfoPrinted = L7_FALSE;
    currentDirectionPrinted = L7_FALSE;
    rc = L7_SUCCESS;

    for (dir = L7_INBOUND_ACL; dir < L7_ACL_DIRECTION_TOTAL; dir++)
    {
      if(usmDbQosAclMacAssignedIntfDirListGet(unit, aclId, dir, &intfList[dir]) != L7_SUCCESS)
      {
        intfList[dir].count = 0;
      }
      if(usmDbQosAclMacAssignedVlanDirListGet(unit, aclId, dir, &vlanList[dir]) != L7_SUCCESS)
      {
        vlanList[dir].count = 0;
      }
    }

    while((rc == L7_SUCCESS) && numwrites < (CLI_MAX_SCROLL_LINES-6))
    {
      if (currentAclInfoPrinted == L7_FALSE)
      {
        rc = usmDbQosAclMacNameGet(unit, aclId, macAclName);
        ewsTelnetPrintf (ewsContext, "%-33s", macAclName);

        /* count the rules in this ACL */
        rules = 0;

        rc = usmDbQosAclMacRuleGetFirst(unit, aclId, &rulenum);
        if(rc == L7_SUCCESS)
        {
          do
          {
            rules++;
          } while(usmDbQosAclMacRuleGetNext(unit, aclId, rulenum, &rulenum) == L7_SUCCESS);
        }

        ewsTelnetPrintf (ewsContext, "%-7d", rules);
        currentAclInfoPrinted = L7_TRUE;
      }

      if (currentDirectionPrinted == L7_FALSE)
      {
        if ((intfList[direction].count > 0) || (vlanList[direction].count > 0))
        {
          switch (direction)
          {
          case L7_INBOUND_ACL:
            sprintf(buf, "%-11s", pStrInfo_qos_Inbound_1);
            break;
          case L7_OUTBOUND_ACL:
            sprintf(buf, "%-11s", pStrInfo_qos_Outbound_1);
            break;
          default:
            /* unknown direction enumeration */
            sprintf(buf, "        ");
          }
          currentDirectionPrinted = L7_TRUE;
          ewsTelnetWrite(ewsContext, buf);
        }
      }

      memset (buf, 0,  sizeof(buf));
      memset (stat, 0,sizeof(stat));
      interfaceColumnComplete = L7_FALSE;
      vlanColumnComplete = L7_FALSE;

      while ((intfIndex < intfList[direction].count) && (interfaceColumnComplete == L7_FALSE))
      {
        if(usmDbUnitSlotPortGet(intfList[direction].intIfNum[intfIndex], &u, &s, &p) == L7_SUCCESS)
        {
          osapiStrncpySafe(stat, cliDisplayInterfaceHelp(u, s, p), sizeof(stat));
          if (intfIndex < (intfList[direction].count-1))
          {
            strcat(stat, ", ");
          }
          if ((strlen(buf) + strlen(stat)) < interfacesColumnEnd)
          {
            strcat(buf, stat);
            intfIndex++;
            if(intfIndex == intfList[direction].count)
            {
              interfaceColumnComplete = L7_TRUE;
            }
          }
          else
          {
            interfaceColumnComplete = L7_TRUE;
          }
        }
      }
      /* print spaces to pad over to start of vlan column, if necessary */
      if (vlanList[direction].count > 0)
      {
        for (i=strlen(buf); i < vlansColumnStart; i++)
        {
          strcat(buf, " ");
        }
      }

      while ((vlanIndex < vlanList[direction].count) && (vlanColumnComplete == L7_FALSE))
      {
        sprintf(stat, "%u", vlanList[direction].vlanNum[vlanIndex]);
        if (vlanIndex < (vlanList[direction].count-1))
        {
          strcat(stat, ", ");
        }
        if ((strlen(buf) + strlen(stat)) < vlansColumnEnd)
        {
          strcat(buf, stat);
          vlanIndex++;
          if(vlanIndex == vlanList[direction].count)
          {
            vlanColumnComplete = L7_TRUE;
          }
        }
        else
        {
          vlanColumnComplete = L7_TRUE;
        }
      }

      if ((interfaceColumnComplete == L7_TRUE) || (vlanColumnComplete == L7_TRUE))
      {
        ewsTelnetWrite(ewsContext,buf);
        memset (buf, 0,sizeof(buf));
      }

      rc = L7_FAILURE;
      while( rc != L7_SUCCESS)
      {
        if (intfIndex < intfList[direction].count)
        {
          /* not finished with this acl/direction; move to new line and pad to Interfaces column */
          ewsTelnetWrite(ewsContext,  "\r\n                                                   ");
          numwrites++;
          rc = L7_SUCCESS;
          break;
        }
        else if (vlanIndex < vlanList[direction].count)
        {
          /* not finished with this acl/direction; move to new line and pad to vlans column */
          ewsTelnetWrite(ewsContext,  "\r\n                                                   ");
          numwrites++;
          rc = L7_SUCCESS;
          break;
        }
        else if (direction < L7_OUTBOUND_ACL)
        {
          /* finished outputting interfaces and/or vlans for current acl/direction; move to next direction, if any */
          if (((intfList[L7_INBOUND_ACL].count > 0) || (vlanList[L7_INBOUND_ACL].count > 0)) &&
              ((intfList[L7_OUTBOUND_ACL].count > 0) || (vlanList[L7_OUTBOUND_ACL].count > 0)))
          {
            ewsTelnetWrite(ewsContext,  "\r\n                                        ");
            numwrites++;
          }
          currentDirectionPrinted = L7_FALSE;
          direction = L7_OUTBOUND_ACL;
          intfIndex = 0;
          vlanIndex = 0;
          rc = L7_SUCCESS;
          break;
        }
        else
        {
          /* done with this acl, see if there is another to work on */
          rc = usmDbQosAclMacIndexGetNext(unit, aclId, &aclId);
          if (rc == L7_SUCCESS)
          {
            currentAclInfoPrinted = L7_FALSE;
            intfIndex = 0;
            vlanIndex = 0;
            currentDirectionPrinted = L7_FALSE;
            direction = L7_INBOUND_ACL;
            for (dir = L7_INBOUND_ACL; dir < L7_ACL_DIRECTION_TOTAL; dir++)
            {
              if(usmDbQosAclMacAssignedIntfDirListGet(unit, aclId, dir, &intfList[dir]) != L7_SUCCESS)
              {
                intfList[dir].count = 0;
              }
              if(usmDbQosAclMacAssignedVlanDirListGet(unit, aclId, dir, &vlanList[dir]) != L7_SUCCESS)
              {
                vlanList[dir].count = 0;
              }
            }
            ewsTelnetWrite(ewsContext,  pStrInfo_common_CrLf);
            numwrites++;
          }
          else
          {
            /* done with all acl's */
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliSyntaxReturnPrompt (ewsContext, "");
          }
        }
      }
    }

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_qos_ShowMacAccessList);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }

  /* Specific MAC Access-list */
  else if(numArgs == 1)
  {
    cliCmdScrollSet(L7_FALSE);
    numwrites = 0;
    if(cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      firstTime = L7_FALSE;
      /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }

    if(strlen(argv[index+argName]) > L7_ACL_NAME_LEN_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    osapiStrncpySafe(macAclName,argv[index+argName], sizeof(macAclName));

    if(usmDbQosAclMacNameStringCheck(unit, macAclName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_qos_AclNameNameStringMayInclAlphabeticNumericDashDotOrUnderscoreCharsOnlyNameMustStartWithALetterAndSizeOfNameStringMustBeLessThanOrEqualTo31Chars);
    }

    if(usmDbQosAclMacNameToIndex(unit, macAclName, &aclId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_qos_AccessListDoesntExist);
    }

    ewsTelnetPrintf (ewsContext, "\r\n%s %s", pStrInfo_qos_AclName, macAclName);
    cliSyntaxBottom(ewsContext);

    if(firstTime == L7_TRUE)
    {   
    /* retrieve interface lists for inbound and outbound directions */
    (void)usmDbQosAclMacAssignedIntfDirListGet(unit, aclId, L7_INBOUND_ACL, &intfList[L7_INBOUND_ACL]);
    (void)usmDbQosAclMacAssignedIntfDirListGet(unit, aclId, L7_OUTBOUND_ACL, &intfList[L7_OUTBOUND_ACL]);

    direction = L7_INBOUND_ACL;
    while (direction < L7_ACL_DIRECTION_TOTAL)
    {
      intfIndex = 0;
      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      if(intfList[direction].count >= 1)
      {
        if (direction == L7_INBOUND_ACL)
        {
          sprintf(buf,"%s %s: ", pStrInfo_qos_Inbound, pStrInfo_qos_Intf_1);
        }
        else
        {
          sprintf(buf,"%s %s: ", pStrInfo_qos_Outbound, pStrInfo_qos_Intf_1);
        }
        ewsTelnetWrite(ewsContext, buf);
        memset (buf, 0, sizeof(buf));
      }

      commaFlag = L7_TRUE;

      while(intfIndex < intfList[direction].count)
      {
        if(intfIndex >= (intfList[direction].count-1))
        {
          commaFlag = L7_FALSE;
        }
        if(usmDbUnitSlotPortGet(intfList[direction].intIfNum[intfIndex], &u, &s, &p) == L7_SUCCESS)
        {
          printLine = L7_FALSE;
          osapiStrncpySafe(stat, cliDisplayInterfaceHelp(u, s, p), sizeof(stat));
          if(commaFlag == L7_TRUE)
          {
            strcat(stat, ", ");
          }
          if((strlen(buf) + strlen(stat)) < 48)
          {
            strcat(buf, stat);
            intfIndex++;
            if(intfIndex == intfList[direction].count)
            {
              printLine = L7_TRUE;
            }
          }
          else
          {
            printLine = L7_TRUE;
          }

          if(printLine == L7_TRUE)
          {
            strcpy(stat, buf);
            firstLine = L7_FALSE;
            ewsTelnetWrite(ewsContext,stat);
            memset (buf, 0,sizeof(buf));
          }
        }
      }
      cliSyntaxBottom(ewsContext);
      direction++;
    }

    /* retrieve vlan lists for inbound and outbound directions */
    (void)usmDbQosAclMacAssignedVlanDirListGet(unit, aclId, L7_INBOUND_ACL, &vlanList[L7_INBOUND_ACL]);
    (void)usmDbQosAclMacAssignedVlanDirListGet(unit, aclId, L7_OUTBOUND_ACL, &vlanList[L7_OUTBOUND_ACL]);

    direction = L7_INBOUND_ACL;
    while (direction < L7_ACL_DIRECTION_TOTAL)
    {
      intfIndex = 0;
      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      if(vlanList[direction].count >= 1)
      {
        if (direction == L7_INBOUND_ACL)
        {
          sprintf(buf,"%s %s: ", pStrInfo_qos_Inbound, pStrInfo_qos_Vlan_1);
        }
        else
        {
          sprintf(buf,"%s %s: ", pStrInfo_qos_Outbound, pStrInfo_qos_Vlan_1);
        }
        ewsTelnetWrite(ewsContext, buf);
        memset (buf, 0, sizeof(buf));
      }

      commaFlag = L7_TRUE;

      while(intfIndex < vlanList[direction].count)
      {
        if(intfIndex >= (vlanList[direction].count-1))
        {
          commaFlag = L7_FALSE;
        }
        printLine = L7_FALSE;
        sprintf(stat, "%u", vlanList[direction].vlanNum[intfIndex]);
        if(commaFlag == L7_TRUE)
        {
          strcat(stat, ", ");
        }
        if((strlen(buf) + strlen(stat)) < 48)
        {
          strcat(buf, stat);
          intfIndex++;
          if(intfIndex == vlanList[direction].count)
          {
            printLine = L7_TRUE;
          }
        }
        else
        {
          printLine = L7_TRUE;
        }

        if(printLine == L7_TRUE)
        {
          strcpy(stat, buf);
          firstLine = L7_FALSE;
          ewsTelnetWrite(ewsContext,stat);
          memset (buf, 0,sizeof(buf));
        }
      }
      cliSyntaxBottom(ewsContext);
      direction++;
    }

    /* since this is the first time, get a new rulenum */
    rc = usmDbQosAclMacRuleGetFirst(unit, aclId, &rulenum);

       if(rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_NoRulesHaveBeenCfguredForAcl);
    }
     }/* END of firstTime*/
      do /* this section is executed for both the first and successive times */
      {
        cliSyntaxBottom(ewsContext);
        ewsTelnetPrintf (ewsContext, "\r\n%s %d",pStrInfo_qos_RuleNum,rulenum);

        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_ACTION) == L7_TRUE)
        {
          cliFormat(ewsContext, pStrInfo_common_Action);
          memset (buf, 0, sizeof(buf));
          rc = usmDbQosAclMacRuleActionGet(unit, aclId, rulenum, &val);
          if(val == L7_ACL_PERMIT /*ACL_PERMIT*/)
          {
            ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_Permit_1);
          }
          if(val == L7_ACL_DENY /*ACL_DENY*/)
          {
            ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_Deny_2);
          }
        }

        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_EVERY) == L7_TRUE)
        { /* if the match every is set to true */
          cliFormat(ewsContext, pStrInfo_qos_MatchAll);

          memset (buf, 0, sizeof(buf));
          every = L7_FALSE;
          rc = usmDbQosAclMacRuleEveryGet(unit, aclId, rulenum, &every);
          if(every == L7_TRUE)
          {
            sprintf(buf, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_True);
          }
          else
          {
            sprintf(buf, pStrErr_common_CfgAclsStringFmt, pStrInfo_common_False);
          }
          ewsTelnetWrite(ewsContext,buf);
        }

        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_SRCMAC) == L7_TRUE)
        {
          memset (srcMacAddr, 0, sizeof(srcMacAddr));
          memset (srcMacMask, 0, sizeof(srcMacMask));
          if(usmDbQosAclMacRuleSrcMacAddrMaskGet(unit, aclId, rulenum, srcMacAddr, srcMacMask) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_SrcMacAddr);
            memset (stat, 0,sizeof(stat));
            osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X ",
                          srcMacAddr[0], srcMacAddr[1], srcMacAddr[2], srcMacAddr[3], srcMacAddr[4], srcMacAddr[5]);
            ewsTelnetPrintf (ewsContext, "%s ", buf);

            if(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                        L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID) == L7_TRUE)
            {
              /* invert MAC mask value for display */
              for(i = 0; i < L7_MAC_ADDR_LEN; i++)
              {
                srcMacMask[i] = ~(srcMacMask[i]);
              }
              cliFormat(ewsContext, pStrInfo_qos_SrcMacMask);
              memset (stat, 0,sizeof(stat));
              osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X ",
                            srcMacMask[0], srcMacMask[1], srcMacMask[2], srcMacMask[3], srcMacMask[4], srcMacMask[5]);
              ewsTelnetPrintf (ewsContext, "%s ", buf);
            }
          }
        }

        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_DSTMAC) == L7_TRUE)
        {

          memset (dstMacAddr, 0, sizeof(dstMacAddr));
          memset (dstMacMask, 0, sizeof(dstMacMask));
          if(usmDbQosAclMacRuleDstMacAddrMaskGet(unit, aclId, rulenum, dstMacAddr, dstMacMask) == L7_SUCCESS)
          {

            cliFormat(ewsContext, pStrInfo_qos_DstMacAddr);
            memset (stat, 0,sizeof(stat));
            osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X ",
                          dstMacAddr[0], dstMacAddr[1], dstMacAddr[2], dstMacAddr[3], dstMacAddr[4], dstMacAddr[5]);
            ewsTelnetPrintf (ewsContext, "%s ", buf);

            if(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                        L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) == L7_TRUE)
            {
              /* invert MAC mask value for display */
              for(i = 0; i < L7_MAC_ADDR_LEN; i++)
              {
                dstMacMask[i] = ~(dstMacMask[i]);
              }
              cliFormat(ewsContext, pStrInfo_qos_DstMacMask);
              memset (stat, 0,sizeof(stat));
              osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X ",
                            dstMacMask[0], dstMacMask[1], dstMacMask[2], dstMacMask[3], dstMacMask[4], dstMacMask[5]);
              ewsTelnetPrintf (ewsContext, "%s ", buf);
            }
          }
        }

        /* Ethertype */
        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_ETYPE_KEYID) == L7_TRUE)
        {
          cliFormat(ewsContext, pStrInfo_qos_EtherType);
          memset (buf, 0, sizeof(buf));
          memset (stat, 0, sizeof(stat));
          if(usmDbQosAclMacRuleEtypeKeyGet(unit, aclId, rulenum, &keyId, &val) == L7_SUCCESS)
          {
            if(keyId == L7_QOS_ETYPE_KEYID_CUSTOM)
            {
              sprintf(buf, "0x%4.4x", (L7_ushort16)val);
              ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt,buf);
            }
            else
            {
              memset (buf, 0, sizeof(buf));
              memset (eTypeString, 0, sizeof(eTypeString));
              cliDiffservConvertEtherTypeKeyIdToString(keyId, eTypeString, sizeof(eTypeString));
              ewsTelnetPrintf (ewsContext, "%-10s", eTypeString);
            }
          }
        }

        /* VLAN ID */
        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_VLANID) == L7_TRUE)
        {
          if(usmDbQosAclMacRuleVlanIdGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_common_VapVlan);
            ewsTelnetPrintf (ewsContext, "%d",val);
          }
        }

        /* VLAN Range */
        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_VLANID_START) == L7_TRUE)
        {
          if(usmDbQosAclMacRuleVlanIdRangeGet(unit, aclId, rulenum, &startVal, &endVal) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_VlanRange);
            ewsTelnetPrintf (ewsContext, "%d-%d",startVal, endVal);
          }
        }

        /* COS */
        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_COS) == L7_TRUE)
        {
          if(usmDbQosAclMacRuleCosGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_CosVal_1);
            ewsTelnetPrintf (ewsContext, "%d",val);
          }
        }

        /* Secondary VLAN ID */
        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_VLANID2) == L7_TRUE)
        {
          if(usmDbQosAclMacRuleVlanId2Get(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_SecondaryVlan);
            ewsTelnetPrintf (ewsContext, "%d",val);
          }
        }

        /* Secondary VLAN Range */
        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_VLANID2_START) == L7_TRUE)
        {
          if(usmDbQosAclMacRuleVlanId2RangeGet(unit, aclId, rulenum, &startVal, &endVal) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_SecondaryVlanRange);
            ewsTelnetPrintf (ewsContext, "%d-%d",startVal, endVal);
          }
        }

        /* Secondary COS */
        if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_COS2) == L7_TRUE)
        {
          if(usmDbQosAclMacRuleCos2Get(unit, aclId, rulenum, &val) == L7_SUCCESS)
          {
            cliFormat(ewsContext, pStrInfo_qos_SecondaryCosVal_1);
            ewsTelnetPrintf (ewsContext, "%d",val);
          }
        }

        /* Log */
        if (usmDbQosAclMacRuleLoggingAllowed(unit, aclId, rulenum) == L7_SUCCESS)
        {
          if(usmDbQosAclMacIsFieldConfigured(unit, aclId, rulenum, ACL_MAC_LOGGING) == L7_TRUE)
          {
            if(usmDbQosAclMacRuleLoggingGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
            {
              cliFormat(ewsContext, pStrInfo_common_Log_1);
              ewsTelnetPrintf (ewsContext, pStrErr_common_CfgAclsStringFmt, (val == L7_TRUE) ? pStrInfo_common_True : pStrInfo_common_False);
            }
          }
        }

        /* Assign Queue */
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
        {
          if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_ASSIGN_QUEUEID) == L7_TRUE)
          {
            if(usmDbQosAclMacRuleAssignQueueIdGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
            {
              cliFormat(ewsContext, pStrInfo_qos_AsSignQueue);
              ewsTelnetPrintf (ewsContext, "%d",val);
            }
          }
        }

        /* Mirror Interface */
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
        {
          if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_MIRROR_INTF) == L7_TRUE)
          {
            if(usmDbQosAclMacRuleMirrorIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
               usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
            {
              cliFormat(ewsContext, pStrInfo_qos_MirrorIntf);
              ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(u, s, p));
            }
          }
        }

        /* Redirect Interface */
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
        {
          if(usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum,ACL_MAC_REDIRECT_INTF) == L7_TRUE)
          {
            if(usmDbQosAclMacRuleRedirectIntfGet(unit, aclId, rulenum, &val) == L7_SUCCESS &&
               usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
            {
              cliFormat(ewsContext, pStrInfo_qos_RedirectIntf);
              ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(u, s, p));
            }
          }
        }
        /* Time Range Information */
     if(usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) ==L7_TRUE)
     {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE)
        {
          if (usmDbQosAclMacIsFieldConfigured(unit, aclId,rulenum, ACL_MAC_TIME_RANGE_NAME) == L7_TRUE)
          {
            if (usmDbQosAclMacRuleTimeRangeNameGet(unit, aclId, rulenum, timeRangeName) == L7_SUCCESS)
            {
              cliFormat(ewsContext, pStrInfo_qos_TimeRangeName);
              ewsTelnetPrintf (ewsContext, "%s", timeRangeName);
            }
            if(usmDbQosAclMacRuleStatusGet(unit, aclId, rulenum, &val) == L7_SUCCESS)
            {
              cliFormat(ewsContext, pStrInfo_qos_TimeRangeStatus);
              if( val == L7_ACL_RULE_STATUS_ACTIVE)
              {
                 ewsTelnetPrintf(ewsContext, pStrInfo_qos_TimeRangeStatusActive);
              }
              else if( val == L7_ACL_RULE_STATUS_INACTIVE)
              {
                 ewsTelnetPrintf(ewsContext, pStrInfo_qos_TimeRangeStatusInactive);
              }             
            }
            }
          }
        }
       numwrites ++;
    } while(((rc = usmDbQosAclMacRuleGetNext(unit, aclId, rulenum, &rulenum)) == L7_SUCCESS) 
              && (numwrites < (CLI_MAX_SCROLL_LINES-21)));
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else
    {
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      osapiSnprintf(stat, sizeof(stat), "%s %s", pStrInfo_qos_ShowMacAccessList, macAclName);
      cliAlternateCommandSet(stat);
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ShowMacAcl_1);
  }
}

/*********************************************************************
*
* @purpose  Display access list information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  command show  access-lists <interface> <in | out>
*
* @cmdhelp
*
* @cmddescript
* Using L7_RC_t usmDbQosAclIntfDirAclListGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 direction,
*                                      L7_ACL_INTF_DIR_LIST_t *listInfo)
* {
*   return aclIntfDirAclListGet(intIfNum, direction, listInfo);
* }
*
*
* @end
*
*********************************************************************/
const char *commandShowAccessListInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)

{

  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 argDirection = 2;
  L7_RC_t rc;
  L7_uint32 aclId;
  L7_uint32 unit;
  static L7_uint32 interface;
  L7_char8 dirStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 seqNumStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 numArgs;
  L7_uint32 u, s, p, i, direction;
  L7_int32 slot, port;
  L7_int32 retVal;
  L7_uint32 sequenceNum;
  L7_ACL_INTF_DIR_LIST_t listInfo;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();

  if (numArgs != 2 || strlen(argv[index+argSlotPort]) >= sizeof(strSlotPort))      /* parameter check */
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) != L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_ShowAclIntfIn_1);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_ShowAclIntf_1);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  osapiStrncpySafe(strSlotPort, argv[index+argSlotPort], sizeof(strSlotPort));
  cliConvertToLowerCase(strSlotPort);

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  if (interface != 0)
  {
    if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS &&
        usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS &&
        usmDbQosAclIsValidIntf(unit, interface) == L7_TRUE)
    {

      if (strlen(argv[index+argDirection]) > 3)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_InvalidDirection);
      }

      osapiStrncpySafe(dirStr,argv[index+argDirection], sizeof(dirStr));

      if (strcmp(dirStr, pStrInfo_common_AclInStr) == 0)
      {
        direction = L7_INBOUND_ACL;
      }
      else
      {
        direction = L7_OUTBOUND_ACL;
      }

      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_qos_AclTypeAclIdSeqNum );
      ewsTelnetWrite(ewsContext,
                     "\r\n-------- ------------------------------- ---------------");

      memset ((char *)&listInfo, 0, sizeof(listInfo));
      if (usmDbQosAclIntfDirAclListGet(unit, interface, direction, &listInfo) == L7_SUCCESS)
      {

        for (i = 0; i < listInfo.count; i++)
        {
          L7_BOOL isNamedIpAcl;

          aclId = listInfo.listEntry[i].aclId;
          isNamedIpAcl = L7_FALSE;

          if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IP)
          {
            if (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) == L7_SUCCESS)
            {
              isNamedIpAcl = L7_TRUE;
              (void)usmDbQosAclNameGet(unit, aclId, aclName);
            }
            ewsTelnetPrintf (ewsContext, "\r\n%-9.5s", pStrInfo_common_Ip);
          }
          else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_MAC)
          {
            (void)usmDbQosAclMacNameGet(unit, aclId, aclName);
            ewsTelnetPrintf (ewsContext, "\r\n%-9.5s", pStrInfo_qos_Mac_1);
          }
          else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IPV6)
          {
            (void)usmDbQosAclNameGet(unit, aclId, aclName);
            ewsTelnetPrintf (ewsContext, "\r\n%-9.5s", pStrInfo_common_Ipv6_3);
          }

          if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IP)
          {
            if (L7_FALSE == isNamedIpAcl)
            {
              ewsTelnetPrintf (ewsContext, "%-32d", aclId);
            }
            else
            {
              ewsTelnetPrintf (ewsContext, "%-32s", aclName);
            }
          }
          else if ((listInfo.listEntry[i].aclType == L7_ACL_TYPE_MAC) ||
                   (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IPV6))
          {
            ewsTelnetPrintf (ewsContext, "%-32s", aclName);
          }

          sequenceNum = listInfo.listEntry[i].seqNum;
          if (sequenceNum != L7_ACL_AUTO_INCR_INTF_SEQ_NUM)
          {
            sprintf(seqNumStr, "%u", sequenceNum);
          }
          else
          {
            sprintf(seqNumStr, " ");
          }
          ewsTelnetWrite(ewsContext, seqNumStr);

        }     /* endfor */
      }
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose  Display access list information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  command show  access-lists <interface> <in | out>
*
* @cmdhelp
*
* @cmddescript
* Using L7_RC_t usmDbQosAclIntfDirAclListGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 direction,
*                                      L7_ACL_INTF_DIR_LIST_t *listInfo)
* {
*   return aclIntfDirAclListGet(intIfNum, direction, listInfo);
* }
*
*
* @end
*
*********************************************************************/
const char *commandShowAccessListVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)

{
  L7_uint32 argVlanId = 1;
  L7_uint32 argDirection = 2;
  L7_uint32 aclId;
  L7_uint32 vlanId;
  L7_uint32 unit;
  L7_char8 dirStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 seqNumStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 numArgs;
  L7_uint32 i, direction;
  L7_uint32 sequenceNum;
  L7_ACL_VLAN_DIR_LIST_t listInfo;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();

  if (numArgs != 2)      /* parameter check */
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID) != L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_ShowAclVlanIn_1);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_qos_ShowAclVlan_1);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get the vlan id specified by user */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId], &vlanId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidVlanId_1);
  }

  if (vlanId != 0)
  {
    if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_TRUE)
    {
      osapiStrncpySafe(dirStr,argv[index+argDirection], sizeof(dirStr));

      if (strcmp(dirStr, pStrInfo_common_AclInStr) == 0)
      {
        direction = L7_INBOUND_ACL;
      }
      else if (strcmp(dirStr, pStrInfo_common_AclOutStr) == 0)
      {
        direction = L7_OUTBOUND_ACL;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_InvalidDirection);
      }

      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_AclTypeAclIdSeqNum );
      ewsTelnetWrite(ewsContext,
                     "\r\n-------- ------------------------------- ---------------");

      memset ((char *)&listInfo, 0, sizeof(listInfo));
      if (usmDbQosAclVlanDirAclListGet(unit, vlanId, direction, &listInfo) == L7_SUCCESS)
      {
        for (i = 0; i < listInfo.count; i++)
        {
          L7_BOOL isNamedIpAcl;

          aclId = listInfo.listEntry[i].aclId;
          isNamedIpAcl = L7_FALSE;

          if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IP)
          {
            if (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) == L7_SUCCESS)
            {
              isNamedIpAcl = L7_TRUE;
              (void)usmDbQosAclNameGet(unit, aclId, aclName);
            }
            ewsTelnetPrintf (ewsContext, "\r\n%-9.5s", pStrInfo_common_Ip);
          }
          else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_MAC)
          {
            usmDbQosAclMacNameGet(unit, aclId, aclName);
            ewsTelnetPrintf (ewsContext, "\r\n%-9.5s", pStrInfo_qos_Mac_1);
          }
          else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IPV6)
          {
            usmDbQosAclNameGet(unit, aclId, aclName);
            ewsTelnetPrintf (ewsContext, "\r\n%-9.5s", pStrInfo_common_Ipv6_3);
          }

          if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IP)
          {
            if (L7_FALSE == isNamedIpAcl)
            {
              ewsTelnetPrintf (ewsContext, "%-32d", aclId);
            }
            else
            {
              ewsTelnetPrintf (ewsContext, "%-32s", aclName);
            }
          }
          else if ((listInfo.listEntry[i].aclType == L7_ACL_TYPE_MAC) ||
                   (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IPV6))
          {
            ewsTelnetPrintf (ewsContext, "%-32s", aclName);
          }

          sequenceNum = listInfo.listEntry[i].seqNum;
          if (sequenceNum != L7_ACL_AUTO_INCR_INTF_SEQ_NUM)
          {
            sprintf(seqNumStr, "%u", sequenceNum);
          }
          else
          {
            sprintf(seqNumStr, " ");
          }
          ewsTelnetWrite(ewsContext, seqNumStr);

        }     /* endfor */
      }
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");

}
/*********************************************************************
*
* @purpose  Displays trap flag info for QOS ACL
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax     show trapflags
*
* @cmdhelp
*
* @cmddescript This commands takes no options
*
* @end
*
*********************************************************************/
void commandShowAclTrapflags(EwsContext ewsContext)
{
  L7_uint32 val;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return;
  }

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    if (usmDbQosAclTrapFlagGet(unit, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_AclTraps);
      ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
    }
  }
  return;
}
