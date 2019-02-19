/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/diffserv/cli_show_running_config_diffserv.c
 *
 * @purpose show running config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  01/07/2003
 *
 * @author  akshay
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "diffserv_exports.h"
#include "cli_web_exports.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_util_diffserv_api.h"
#endif

#include "comm_mask.h"
#include "osapi_support.h"
#include "cliapi.h"

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"
#include "cli_show_running_config.h"

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  #include "cliutil_diffserv.h"
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
/*********************************************************************
* @purpose  To print the running configuration of diffserv Info
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

L7_RC_t cliRunningConfigDiffservInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 classIndex = 0;
  L7_uint32 i;
  L7_uint32 classList[L7_DIFFSERV_CLASS_LIM];
  L7_uint32 classCount = 0;
  L7_uchar8 className[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 classNameSize = L7_CLI_MAX_STRING_LENGTH;
  L7_uchar8 classL3ProtoStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType;
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t classL3Protocol;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t ruleType;
  L7_QOS_ETYPE_KEYID_t eTypeKey;
  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_NONE;
  L7_uint32 aclNum = 0;
  L7_BOOL excludeSupported = L7_FALSE;
  L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t excludeFlag = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE;
  L7_char8 excludeFlagString[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL firstClass = L7_FALSE;
  L7_BOOL hasInstances = L7_FALSE;
  L7_uint32 localClassIndex;
  L7_uint32 ruleIndex = 0;
  L7_uint32 count = 0;
  L7_uint32 attrcount = 0;
  L7_uint32 count2 = 0;
  L7_uint32 policyInstIndex = 0;
  L7_uint32 lastPolicyInstIndex = 0;
  L7_uint32 attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL] = {0};
  L7_uint32 val, val2;
  L7_char8 strName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 valBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 val2Buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macMask[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 macAddrSize = L7_CLI_MAX_STRING_LENGTH;
  L7_uint32 macMaskSize = L7_CLI_MAX_STRING_LENGTH;
  L7_uchar8 tosBits, tosMask;
  L7_in6_addr_t ip6addr;
  L7_uint32 policyIndex = 0;
  L7_uint32 policeAttrIndex = 0;
  L7_uint32 tempPolicyIndex=0;
  L7_uint32 tempPolicyIndex_1=0;
  L7_uint32 tempPolicyIndex_2=0;
  L7_uint32 tempAttrPolicyIndex = 0;
  L7_uint32 tempPolicyInstIndex_1;
  L7_uint32 tempAttrPolicyInstIndex;
  L7_uint32 policyAttrIndex = 0;
  L7_uint32 unitNum, slot, port;

  if (usmDbDiffServGenAdminModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_QOS_DIFFSERV_ADMIN_MODE,pStrInfo_qos_Diffserv_3);
  }

/* class-map */
  if (usmDbDiffServClassOrderedListGet(unit, classList, &classCount) == L7_SUCCESS)
  {
    for (i = 0; i < classCount; i++)
    {
      classIndex = classList[i];
    memset (buf, 0, sizeof(buf));
    memset (className, 0, sizeof(className));
    classNameSize = sizeof(className);
    if ((usmDbDiffServClassNameGet(unit, classIndex, className, &classNameSize)         != L7_SUCCESS)
        || (strcmp(className, FD_QOS_DIFFSERV_DEFAULTCLASS_NAME) == 0))
    {
      continue;
    }

    if (usmDbDiffServClassTypeGet(unit,classIndex, &classType) != L7_SUCCESS)
    {
      continue;
    }

    if (usmDbDiffServClassL3ProtoGet(unit,classIndex, &classL3Protocol) != L7_SUCCESS)
    {
      continue;
    }

    memset (classL3ProtoStr, 0, sizeof(classL3ProtoStr));
    /*
     * only include the ipv4|ipv6 keyword in command if platform supports
     * ipv6 match items in diffServ classes
    */
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE)
    {
      switch (classL3Protocol)
      {
      case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4:
        sprintf(classL3ProtoStr, pStrInfo_common_Diffserv_4);
        break;
      case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6:
        sprintf(classL3ProtoStr, pStrInfo_common_Diffserv_5);
        break;
      default:
        continue;
      }
    }

    memset (buf, 0, sizeof(buf));
    memset (stat, 0, sizeof(stat));
    switch (classType)
    {
    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL:
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_ClassMapMatchAll, className, classL3ProtoStr);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY:
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_ClassMapMatchAny, className, classL3ProtoStr);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL:
      if ((usmDbDiffServClassAclTypeGet(unit, classIndex, &aclType) != L7_SUCCESS) ||
          (usmDbDiffServClassAclNumGet(unit, classIndex, &aclNum) != L7_SUCCESS))
      {
        /* skip class */
        continue;
      }

      if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP)
      {
        if (L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4 == classL3Protocol)
        {
          if ((usmDbDiffServIsAclNamedIpAcl(unit, aclNum) == L7_SUCCESS) &&
              (usmDbDiffServAclIpNameGet(unit, aclNum, strName) == L7_SUCCESS))
          {
            osapiSnprintf(stat, sizeof(stat), "%s", strName);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-12d ", aclNum);
          }
        }
        else if ((L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6 == classL3Protocol) &&
                 (usmDbDiffServAclIpNameGet(unit, aclNum, strName) == L7_SUCCESS))
        {
          osapiSnprintf(stat, sizeof(stat), "%s", strName);
        }
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_ClassMapMatchAccessGrp,className, stat);
        EWSWRITEBUFFER(ewsContext, buf);
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_WillUseCurrentAclDefinition);
      }
      else if ((aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC) &&
               (usmDbDiffServAclMacNameGet(unit, aclNum, strName) == L7_SUCCESS))
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_ClassMapMatchMacAccessGrp, 
                                className, strName, classL3ProtoStr);
        EWSWRITEBUFFER(ewsContext, buf);
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_WillUseCurrentAclDefinition);
      }
      else
      {
        /* skip class */
        continue;
      }
      break;
    default:
      /* skip class */
      continue;
    }

    EWSWRITEBUFFER(ewsContext, buf);

    /* do not display individual match rules for class type ACL, since the
     * class-map command creates the rules automatically from the access list
     */
    if (classType != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) == L7_TRUE)
      {
        excludeSupported = L7_TRUE;
      }

      localClassIndex = classIndex;
      ruleIndex = 0;

      while (usmDbDiffServClassRuleGetNext(unit, localClassIndex, ruleIndex,
                                           &localClassIndex, &ruleIndex) == L7_SUCCESS)
      {
        if (localClassIndex == classIndex)
        {
          if (usmDbDiffServClassRuleMatchEntryTypeGet(unit, localClassIndex,
                                                      ruleIndex, &ruleType) != L7_SUCCESS)
          {
            continue;
          }

          memset (excludeFlagString, 0, sizeof(excludeFlagString));
          if (excludeSupported == L7_TRUE)
          {
            if (usmDbDiffServClassRuleMatchExcludeFlagGet(unit, classIndex, ruleIndex, &excludeFlag) == L7_SUCCESS)
            {
              if (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, excludeFlagString, sizeof(excludeFlagString), pStrInfo_qos_Not_2);
              }
            }
          }

          switch (ruleType)
          {
          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
            if (usmDbDiffServClassRuleMatchCosGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchCos_1, excludeFlagString, val);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
            if (usmDbDiffServClassRuleMatchCos2Get(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchSecondaryCos_1, excludeFlagString, val);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
            if ((usmDbDiffServClassRuleMatchDstIpAddrGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchDstIpMaskGet(unit, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
            {
              memset (buf, 0, sizeof(buf));
              memset (valBuf, 0, sizeof(valBuf));
              memset (val2Buf, 0, sizeof(val2Buf));
              memset (stat, 0, sizeof(stat));
              if ((usmDbInetNtoa(val, valBuf) == L7_SUCCESS) &&
                  (usmDbInetNtoa(val2, val2Buf) == L7_SUCCESS))
              {
                osapiSnprintf(stat, sizeof(stat),  "%s %s", valBuf, val2Buf);
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchDstip, excludeFlagString, stat);
                EWSWRITEBUFFER(ewsContext, buf);
              }
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
            if ((usmDbDiffServClassRuleMatchDstIpv6AddrGet(unit, localClassIndex, ruleIndex, &ip6addr) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchDstIpv6PrefLenGet(unit, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
            {
              memset (buf, 0, sizeof(buf));
              memset (valBuf, 0, sizeof(valBuf));
              memset (stat, 0, sizeof(stat));
              if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ip6addr, valBuf, sizeof(valBuf)) != L7_NULLPTR)
              {
                sprintf(stat, "%s/%d", valBuf, val2);
                sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_MatchDstip6, excludeFlagString, stat);
                EWSWRITEBUFFER(ewsContext, buf);
              }
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
            if ((usmDbDiffServClassRuleMatchDstL4PortStartGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchDstL4PortEndGet(unit, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
            {
              memset (buf, 0, sizeof(buf));
              memset (stat, 0, sizeof(stat));
              memset (valBuf, 0, sizeof(valBuf));
              cliDiffservConvertL4PortValToString(val, valBuf, sizeof(valBuf));
              if (val == val2)
              {
                if(cliCheckIfInteger(valBuf) != L7_SUCCESS)
                {
                  /* Commented output for user info */
                  osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_MatchDstl4port, excludeFlagString, valBuf);
                  EWSWRITEBUFFER(ewsContext, stat);
                }
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_MatchDstl4port_1, excludeFlagString, val);
                EWSWRITEBUFFER(ewsContext, stat);
              }
              else
              {
                memset (val2Buf, 0, sizeof(val2Buf));
                cliDiffservConvertL4PortValToString(val2, val2Buf, sizeof(val2Buf));
                if(cliCheckIfInteger(val2Buf) != L7_SUCCESS)
                {
                  /* Commented output for user info */
                  osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_MatchDstl4portRange, excludeFlagString, valBuf, val2Buf);
                  EWSWRITEBUFFER(ewsContext, stat);
                }
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_MatchDstl4port_2, excludeFlagString, val, val2);
                EWSWRITEBUFFER(ewsContext, stat);
              }
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
            macAddrSize = sizeof(macAddr);
            memset (macAddr, 0, macAddrSize);
            macMaskSize = sizeof(macMask);
            memset (macMask, 0, macMaskSize);
            if ((usmDbDiffServClassRuleMatchDstMacAddrGet(unit, localClassIndex, ruleIndex, macAddr, &macAddrSize) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchDstMacMaskGet(unit, localClassIndex, ruleIndex, macMask, &macMaskSize) == L7_SUCCESS))
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchDstAddrMac_1,
                                      excludeFlagString,
                                      macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
                                      macMask[0], macMask[1], macMask[2], macMask[3], macMask[4], macMask[5]);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
            if (usmDbDiffServClassRuleMatchEtypeKeyGet(unit, localClassIndex, ruleIndex, &eTypeKey) == L7_SUCCESS)
            {
              switch (eTypeKey)
              {
              case L7_QOS_ETYPE_KEYID_APPLETALK:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_APPLETALK);
                break;
              case L7_QOS_ETYPE_KEYID_ARP:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_ARP);
                break;
              case L7_QOS_ETYPE_KEYID_IBMSNA:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_IBMSNA);
                break;
              case L7_QOS_ETYPE_KEYID_IPV4:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_IPV4);
                break;
              case L7_QOS_ETYPE_KEYID_IPV6:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_IPV6);
                break;
              case L7_QOS_ETYPE_KEYID_IPX:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_IPX);
                break;
              case L7_QOS_ETYPE_KEYID_MPLSMCAST:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_MPLSMCAST);
                break;
              case L7_QOS_ETYPE_KEYID_MPLSUCAST:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_MPLSUCAST);
                break;
              case L7_QOS_ETYPE_KEYID_NETBIOS:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_NETBIOS);
                break;
              case L7_QOS_ETYPE_KEYID_NOVELL:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_NOVELL);
                break;
              case L7_QOS_ETYPE_KEYID_PPPOE:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_PPPOE);
                break;
              case L7_QOS_ETYPE_KEYID_RARP:
                osapiSnprintf(stat, sizeof(stat),  L7_QOS_ETYPE_STR_RARP);
                break;
              default:
                usmDbDiffServClassRuleMatchEtypeValueGet(unit, localClassIndex, ruleIndex, &val);
                osapiSnprintf(stat, sizeof(stat),  "0x%4.4x", val);
              }
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchEthertype, excludeFlagString, stat);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchAny_1, excludeFlagString);
            EWSWRITEBUFFER(ewsContext, buf);
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
            if (usmDbDiffServClassRuleMatchIpDscpGet(unit, localClassIndex,
                                                     ruleIndex, &val) == L7_SUCCESS)
            {
              memset (buf, 0, sizeof(buf));
              memset (valBuf, 0, sizeof(valBuf));
              cliDiffservConvertDSCPValToString(val, valBuf);
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchIpDscp, excludeFlagString, valBuf);
              EWSWRITEBUFFER(ewsContext, buf);

              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchIpDscp_2, excludeFlagString, val);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
            if (usmDbDiffServClassRuleMatchIpPrecedenceGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchIpPrecedence_1, excludeFlagString, val);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
            memset (valBuf, 0, sizeof(valBuf));
            memset (val2Buf, 0, sizeof(val2Buf));
            if ((usmDbDiffServClassRuleMatchIpTosBitsGet(unit, localClassIndex, ruleIndex, &tosBits) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchIpTosMaskGet(unit, localClassIndex, ruleIndex, &tosMask) == L7_SUCCESS))
            {
              memset (buf, 0, sizeof(buf));
              osapiSnprintf(stat, sizeof(stat),  "%02X %02X", tosBits, tosMask);
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchIpTos_1, excludeFlagString, stat);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
            if (usmDbDiffServClassRuleMatchIp6FlowLabelGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_MatchIp6flowlbl, excludeFlagString, val);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
            if (usmDbDiffServClassRuleMatchProtocolNumGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS)
            {
              memset (buf, 0, sizeof(buf));
              memset (stat, 0, sizeof(stat));
              switch (val)
              {
              case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_ICMP:
                osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_Icmp_1);
                break;
              case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IGMP:
                osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_Igmp_2);
                break;
              case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IP:
                osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_IpOption);
                break;
              case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_TCP:
                osapiSnprintf(stat, sizeof(stat),  pStrInfo_qos_Tcp_1);
                break;
              case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_UDP:
                osapiSnprintf(stat, sizeof(stat),  pStrInfo_qos_Udp_1);
                break;
              default:
                osapiSnprintf(stat, sizeof(stat),  "%d", val);
              }

              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchProto_1, excludeFlagString, stat);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
            if (usmDbDiffServClassRuleMatchRefClassIndexGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS)
            {
              if (val != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE)
              {
                classNameSize = sizeof(className);
                memset (className, 0, classNameSize);
                if ((usmDbDiffServClassNameGet(unit, val,
                                               className, &classNameSize) == L7_SUCCESS)
                    && ((strcmp(className, FD_QOS_DIFFSERV_DEFAULTCLASS_NAME) != 0)||
                        EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
                {
                  osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchClassMap_1, className);
                  EWSWRITEBUFFER(ewsContext, buf);
                }
              }
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
            if ((usmDbDiffServClassRuleMatchSrcIpAddrGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchSrcIpMaskGet(unit, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
            {
              memset (buf, 0, sizeof(buf));
              memset (valBuf, 0, sizeof(valBuf));
              memset (val2Buf, 0, sizeof(val2Buf));
              memset (stat, 0, sizeof(stat));
              if ((usmDbInetNtoa(val, valBuf) == L7_SUCCESS) &&
                  (usmDbInetNtoa(val2, val2Buf) == L7_SUCCESS))
              {
                osapiSnprintf(stat, sizeof(stat),  "%s %s", valBuf, val2Buf);
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchSrcip, excludeFlagString, stat);
                EWSWRITEBUFFER(ewsContext, buf);
              }
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
            if ((usmDbDiffServClassRuleMatchSrcIpv6AddrGet(unit, localClassIndex, ruleIndex, &ip6addr) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchSrcIpv6PrefLenGet(unit, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
            {
              memset (buf, 0, sizeof(buf));
              memset (valBuf, 0, sizeof(valBuf));
              memset (stat, 0, sizeof(stat));
              if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ip6addr, valBuf, sizeof(valBuf)) != L7_NULLPTR)
              {
                sprintf(stat, "%s/%d", valBuf, val2);
                sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrInfo_qos_MatchSrcip6, excludeFlagString, stat);
                EWSWRITEBUFFER(ewsContext, buf);
              }
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
            if ((usmDbDiffServClassRuleMatchSrcL4PortStartGet(unit, localClassIndex, ruleIndex, &val) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchSrcL4PortEndGet(unit, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
            {
              memset (buf, 0, sizeof(buf));
              memset (stat, 0, sizeof(stat));
              memset (valBuf, 0, sizeof(valBuf));
              cliDiffservConvertL4PortValToString(val, valBuf, sizeof(valBuf));
              if (val == val2)
              {
                if(cliCheckIfInteger(valBuf) != L7_SUCCESS)
                {
                  /* Commented output for user info */
                  osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchSrcl4port, excludeFlagString, valBuf);
                  EWSWRITEBUFFER(ewsContext, buf);
                }
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchSrcl4port_2, excludeFlagString, val);
                EWSWRITEBUFFER(ewsContext, buf);
              }
              else
              {
                memset (val2Buf, 0, sizeof(val2Buf));
                cliDiffservConvertL4PortValToString(val2, val2Buf, sizeof(val2Buf));
                if(cliCheckIfInteger(val2Buf) != L7_SUCCESS)
                {
                  /* Commented output for user info */
                  osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchSrcl4port_1, excludeFlagString, valBuf, val2Buf);
                  EWSWRITEBUFFER(ewsContext, buf);
                }
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchSrcl4port_3, excludeFlagString, val, val2);
                EWSWRITEBUFFER(ewsContext, buf);
              }
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
            macAddrSize = sizeof(macAddr);
            memset (macAddr, 0, macAddrSize);
            macMaskSize = sizeof(macMask);
            memset (macMask, 0, macMaskSize);
            if ((usmDbDiffServClassRuleMatchSrcMacAddrGet(unit, localClassIndex, ruleIndex, macAddr, &macAddrSize) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchSrcMacMaskGet(unit, localClassIndex, ruleIndex, macMask, &macMaskSize) == L7_SUCCESS))
            {
              memset (buf, 0, sizeof(buf));
              osapiSnprintf(stat, sizeof(stat),  "%02X:%02X:%02X:%02X:%02X:%02X %02X:%02X:%02X:%02X:%02X:%02X",
                            macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
                            macMask[0], macMask[1], macMask[2], macMask[3], macMask[4], macMask[5]);
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MatchSrcAddrMac_1, excludeFlagString, stat);
              EWSWRITEBUFFER(ewsContext, buf);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
            if ((usmDbDiffServClassRuleMatchVlanIdStartGet(unit, localClassIndex,
                                                           ruleIndex, &val) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchVlanIdEndGet(unit, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
            {
              if (val == val2)
              {
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_MatchVlan_1, excludeFlagString, val);
                EWSWRITEBUFFER(ewsContext, stat);
              }
              else
              {
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_MatchVlan_2, excludeFlagString, val, val2);
                EWSWRITEBUFFER(ewsContext, stat);
              }
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
            if ((usmDbDiffServClassRuleMatchVlanId2StartGet(unit, localClassIndex,
                                                            ruleIndex, &val) == L7_SUCCESS) &&
                (usmDbDiffServClassRuleMatchVlanId2EndGet(unit, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
            {
              if (val == val2)
              {
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_MatchSecondaryVlan_1, excludeFlagString, val);
                EWSWRITEBUFFER(ewsContext, stat);
              }
              else
              {
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_MatchSecondaryVlan_2, excludeFlagString, val, val2);
                EWSWRITEBUFFER(ewsContext, stat);
              }
            }
            break;

          default:
            break;
          }
        }  /* if (localClassIndex == classIndex) */
      } /* endwhile */
    }

    EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_Exit);

    }
  }

/* policy-map */
/* policy-class-map */

  tempPolicyIndex_2 = tempPolicyIndex;
  while(usmDbDiffServPolicyGetNext(unit, tempPolicyIndex_2, &tempPolicyIndex) == L7_SUCCESS)
  {
    tempPolicyIndex_2 = tempPolicyIndex;
    policyIndex = tempPolicyIndex;
    count2 = 0;
    memset (buf, 0, sizeof(buf));
    memset (strName, 0, sizeof(strName));
    classNameSize = sizeof(strName);
    firstClass = L7_FALSE;

    if (usmDbDiffServPolicyNameGet(unit, policyIndex,
                                   strName, &classNameSize) != L7_SUCCESS)
    {
      continue;
    }

    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_PolicyMap_2, strName);

    if (usmDbDiffServPolicyTypeGet(unit,policyIndex, &policyType) != L7_SUCCESS)
    {
      continue;
    }

    switch (policyType)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
      OSAPI_STRNCAT(buf, pStrInfo_common_AclInStr);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
      OSAPI_STRNCAT(buf, pStrInfo_common_AclOutStr);
      break;
    default:
      /* skip unkown type */
      continue;
    }
    EWSWRITEBUFFER(ewsContext,buf);

    tempPolicyIndex = policyIndex;
    policyInstIndex = 0;

    do
    {
      if(policyInstIndex <= lastPolicyInstIndex)
      {
        lastPolicyInstIndex = 0;
        continue;
      }

      if (usmDbDiffServPolicyInstClassIndexGet(unit, policyIndex, policyInstIndex, &classIndex) == L7_SUCCESS)
      {
        classNameSize = sizeof(className);
        memset (className, 0, classNameSize);
        if (usmDbDiffServClassNameGet(unit, classIndex,
                                      className, &classNameSize) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_Class_3, className);
          EWSWRITEBUFFER(ewsContext,buf);
        }
      }

      tempAttrPolicyIndex = policyIndex;
      tempAttrPolicyInstIndex = policyInstIndex;
      policyAttrIndex = 0;
      memset (buf, 0, sizeof(buf));
      for(attrcount = 0; attrcount < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL; attrcount++)
      {
        attrIndex[attrcount] = 0;
      }

/* policy-class-attributes */
      while((tempAttrPolicyIndex == policyIndex) && (tempAttrPolicyInstIndex == policyInstIndex))
      {
        if(usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, tempAttrPolicyIndex,
                                                   tempAttrPolicyInstIndex,
                                                   policyAttrIndex, &entryType) == L7_SUCCESS)
        {
          if (entryType < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL)
          {
            attrIndex[entryType] = policyAttrIndex;
            hasInstances = L7_TRUE;
          }
        }
        if((usmDbDiffServPolicyAttrGetNext(unit, tempAttrPolicyIndex,
                                           tempAttrPolicyInstIndex, policyAttrIndex,
                                           &tempAttrPolicyIndex, &tempAttrPolicyInstIndex,
                                           &policyAttrIndex) != L7_SUCCESS))
        {
          break;
        }
      }

      if (hasInstances == L7_FALSE)
      {
        for(attrcount = 0; attrcount < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL; attrcount++)
        {
          attrIndex[attrcount] = 0;
        }
        EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_Exit);
        continue;
      }

      tempPolicyIndex_1 = policyIndex;
      tempPolicyInstIndex_1 = policyInstIndex;

/* Assign Queue */
      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtAssignQueueIdGet(unit,tempPolicyIndex_1,
                                                        tempPolicyInstIndex_1,
                                                        attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE],
                                                        &val) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_AssignQueue_1, val);
          count2++;
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

/* Display COS info */
      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtMarkCosValGet(unit,tempPolicyIndex_1,
                                                     tempPolicyInstIndex_1,
                                                     attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL],
                                                     &val) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MarkCos_2, val);
          count2++;
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtMarkCos2ValGet(unit,tempPolicyIndex_1,
                                                      tempPolicyInstIndex_1,
                                                      attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL],
                                                      &val) == L7_SUCCESS)
        {
          if(val == L7_TRUE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_MarkCosAsInCtag_2);
            count2++;
          }
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtMarkCosAsCos2Get(unit,tempPolicyIndex_1,
                                                     tempPolicyInstIndex_1,
                                                     attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2],
                                                     &val) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MarkCosAsInCtag_2);
          count2++;
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtMarkIpDscpValGet(unit,tempPolicyIndex_1,
                                                        tempPolicyInstIndex_1,
                                                        attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL],
                                                        &val) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MarkIpDscp_2, val);
          count2++;
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet(unit,tempPolicyIndex_1,
                                                              tempPolicyInstIndex_1,
                                                              attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL],
                                                              &val) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_MarkIpPrecedence_2, val);
          count2++;
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtDropFlagGet(unit,tempPolicyIndex_1,
                                                   tempPolicyInstIndex_1,
                                                   attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP],
                                                   &val) == L7_SUCCESS)
        {
          if (val == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_Drop_2);
            count2++;
          }
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE] != 0)
      {
        policeAttrIndex = attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE];
        if (usmDbDiffServPolicyAttrStmtPoliceSimpleCrateGet(unit,tempPolicyIndex_1,
                                                            tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_PoliceSimple_3, val);
          count2++;
        }
        if (usmDbDiffServPolicyAttrStmtPoliceSimpleCburstGet(unit,tempPolicyIndex_1,
                                                             tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u ", val);
          OSAPI_STRNCAT(buf, stat);
          count2++;
        }
      }
      else if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE] != 0)
      {
        policeAttrIndex = attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE];
        if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateGet(unit,tempPolicyIndex_1,
                                                                tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_PoliceSingleRate_3, val);
          count2++;
        }

        if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstGet(unit,tempPolicyIndex_1,
                                                                 tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u ", val);
          OSAPI_STRNCAT(buf, stat);
          count2++;
        }
        if (usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstGet(unit,tempPolicyIndex_1,
                                                                 tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u ", val);
          OSAPI_STRNCAT(buf, stat);
          count2++;
        }
      }
      else if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE] != 0)
      {
        policeAttrIndex = attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE];

        if (usmDbDiffServPolicyAttrStmtPoliceTworateCrateGet(unit,tempPolicyIndex_1,
                                                             tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_PoliceTwoRate_3, val);
          count2++;
        }

        if (usmDbDiffServPolicyAttrStmtPoliceTworateCburstGet(unit,tempPolicyIndex_1,
                                                              tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u ", val);
          OSAPI_STRNCAT(buf, stat);
          count2++;
        }

        if (usmDbDiffServPolicyAttrStmtPoliceTworatePrateGet(unit,tempPolicyIndex_1,
                                                             tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u ", val);
          OSAPI_STRNCAT(buf, stat);
          count2++;
        }

        if (usmDbDiffServPolicyAttrStmtPoliceTworatePburstGet(unit,tempPolicyIndex_1,
                                                              tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u ", val);
          OSAPI_STRNCAT(buf, stat);
          count2++;
        }
      }

      if (policeAttrIndex != 0)
      {
        /* Conform Action */
        if (usmDbDiffServPolicyAttrStmtPoliceConformActGet(unit,tempPolicyIndex_1,
                                                           tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)

        {
          switch ( val )
          /* val = disable, enable, delete */
          {
          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ConformActionDrop);
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
            if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit,tempPolicyIndex_1,
                                                               tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ConformActionSetDscpTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
            if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit,tempPolicyIndex_1,
                                                               tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ConformActionSetPrecTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
            if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit,tempPolicyIndex_1,
                                                               tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ConformActionSetCosTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ConformActionSetCosAsCos2Tx);
            break;
            
          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
            if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit,tempPolicyIndex_1,
                                                               tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ConformActionSetSecondaryCosTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ConformActionTx);
            break;
          default:
            break;
          }
          count++;
        }

        /* Exceed Action */
        if ((usmDbDiffServPolicyAttrStmtPoliceExceedActGet(unit,tempPolicyIndex_1,
                                                           tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS))
        {
          switch ( val )
          /* val = disable, enable, delete */
          {
          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ExceedActionDrop);
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
            if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit,tempPolicyIndex_1,
                                                              tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ExceedActionSetCosTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ExceedActionSetCosAsCos2Tx);
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
            if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit,tempPolicyIndex_1,
                                                              tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ExceedActionSetSecondaryCosTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
            if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit,tempPolicyIndex_1,
                                                              tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ExceedActionSetDscpTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
            if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit,tempPolicyIndex_1,
                                                              tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ExceedActionSetPrecTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ExceedActionTx);
            break;
          default:
            break;
          }
          count++;
        }

        /* Non-Conform Action */
        /* Not checking to see if equal to default since this part of the command is mandatory */
        if ((usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(unit,tempPolicyIndex_1,
                                                               tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS))
        {
          switch ( val )
          /* val = disable, enable, delete */
          {
          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ViolateActionDrop);
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
            if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit,tempPolicyIndex_1,
                                                                  tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ViolateActionSetCosTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ViolateActionSetCosAsCos2Tx);
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
            if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit,tempPolicyIndex_1,
                                                                  tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ViolateActionSetSecondaryCosTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
            if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit,tempPolicyIndex_1,
                                                                  tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ViolateActionSetDscpTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
            if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit,tempPolicyIndex_1,
                                                                  tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_qos_ViolateActionSetPrecTx, val);
              OSAPI_STRNCAT(buf, stat);
            }
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_ViolateActionTx);
            break;
          default:
            break;
          }
          count++;
        }
        if (count > 0 || count2 > 0)
        {
          EWSWRITEBUFFER(ewsContext, buf);
        }
        /* conform-color parms */
        if ((usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(unit,tempPolicyIndex_1, tempPolicyInstIndex_1,
                                                                   policeAttrIndex, &val) == L7_SUCCESS) && (val != FD_QOS_DIFFSERV_POLICE_COLOR_CONFORM_INDEX))
        {
          classNameSize = sizeof(className);
          memset (className, 0, classNameSize);
          if (usmDbDiffServClassNameGet(unit, val,
                                        className, &classNameSize) == L7_SUCCESS)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_ConformColor_2, className);
          }
          if ((attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE] !=0) ||
              (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE] !=0))
          {
            if ((usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(unit, tempPolicyIndex_1,
                                                                      tempPolicyInstIndex_1, policeAttrIndex, &val) == L7_SUCCESS) &&
                (val != FD_QOS_DIFFSERV_POLICE_COLOR_EXCEED_INDEX))
            {
              classNameSize = sizeof(className);
              memset (className, 0, classNameSize);
              if(usmDbDiffServClassNameGet(unit, val, className, &classNameSize) == L7_SUCCESS)
              {
                osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_ExceedColor_1, className);
                OSAPI_STRNCAT(buf, stat);
              }
            }
          }
          EWSWRITEBUFFER(ewsContext, buf);
        }

        /* reset index value here so it does not cause extraneous output during next pass */
        policeAttrIndex = 0;
      }


/* Redirect */
      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtRedirectIntfGet(unit,tempPolicyIndex_1,
                                                       tempPolicyInstIndex_1,
                                                       attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT],
                                                       &val) == L7_SUCCESS)
        {
          if (usmDbUnitSlotPortGet(val, &unitNum, &slot, &port) == L7_SUCCESS)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_Redirect_2, cliDisplayInterfaceHelp(unitNum, slot, port));
          }
          count2++;
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

/* Mirror */
      if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR] != 0)
      {
        if (usmDbDiffServPolicyAttrStmtMirrorIntfGet(unit,tempPolicyIndex_1,
                                                     tempPolicyInstIndex_1,
                                                     attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR],
                                                     &val) == L7_SUCCESS)
        {
          if (usmDbUnitSlotPortGet(val, &unitNum, &slot, &port) == L7_SUCCESS)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_Mirror_1, cliDisplayInterfaceHelp(unitNum, slot, port));
          }
          count2++;
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }

      lastPolicyInstIndex = policyInstIndex;
      EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_Exit);

    } while ((usmDbDiffServPolicyInstGetNext(unit, tempPolicyIndex, policyInstIndex, &tempPolicyIndex, &policyInstIndex) == L7_SUCCESS) && (tempPolicyIndex == policyIndex));
    EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_Exit);
  }
  /* while (usmDbDiffServPolicyGetNext(...)) */
  /* EWSWRITEBUFFER(ewsContext, "\r\nexit \r\n");*/

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the diffserv service policy Info
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
* @param    L7_unint32 interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningPhysicalInterfaceDiffservInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_uint32 intIfNum = 0;
  L7_uint32 ifDirection = 0;
  L7_char8 stringPtr[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 policyLen;
  L7_uint32 policyIndex;

  while (usmDbDiffServServiceGetNext(unit, intIfNum, ifDirection,
                                     &intIfNum, &ifDirection) == L7_SUCCESS)
  {
    if (interface != intIfNum)
    {
      continue;
    }
    policyLen = L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1;
    memset ( stringPtr, 0, sizeof( stringPtr ));
    if ( usmDbDiffServServicePolicyIndexGet(unit, intIfNum,ifDirection, &policyIndex) ==L7_SUCCESS
        && usmDbDiffServPolicyNameGet(unit, policyIndex, stringPtr, &policyLen) == L7_SUCCESS)
    {
      memset (buf, 0, sizeof(buf));
      if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_ServicePolicyIn_1, stringPtr);
        EWSWRITEBUFFER(ewsContext, buf);
      }
      else if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_qos_ServicePolicyOut_1, stringPtr);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }

  }  /* while (usmDbDiffServServiceGetNext(...)) */
  return L7_SUCCESS;
}

#endif /* end if diffserv package included */
