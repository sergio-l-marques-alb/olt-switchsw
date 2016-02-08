/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/diffserv/cliutil_diffserv.c
 *
 * @purpose assorted functions for cli diffserv
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   07/08/2002
 *
 * @author  Kathy McDowell / rakeshk
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "cliapi.h"
#include "cliutil.h"
#include "osapi_support.h"

#ifndef _L7_OS_LINUX_
  #include <vxworks_config.h>   /* for DEFAULT_MAX_CONNECTIONS */
#endif /* _L7_OS_LINUX_ */

#include "clicommands_diffserv.h"
#include "cliutil_diffserv.h"
#include "usmdb_qos_diffserv_api.h"
#include "diffserv_exports.h"
#include "cli_web_exports.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
* @purpose  used for diffserv, Converts a Layer 4 Port Keyword value
*           to the associated keyword, if one is available.
*           Otherwise, a string of the input dscpVal is returned.
*
* @param    dscpVal            value to convert
* @param    dscpString         string to return
* @param    dscpStringLenMax   Maximum number of bytes to copy
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t cliDiffservConvertL4PortValToString(L7_uint32 val, L7_char8 * dscpString, L7_int32 dscpStringLenMax)
{
  switch(val)
  {
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_DOMAIN:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, pStrInfo_qos_Domain_1);
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_ECHO:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, pStrInfo_qos_Echo);
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, pStrInfo_common_Ftp);
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTPDATA:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, pStrInfo_qos_Ftpdata);
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_HTTP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s/%s)", val, pStrInfo_common_Http_1, pStrInfo_qos_Www);
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SMTP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, pStrInfo_qos_Smtp);
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SNMP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, pStrInfo_common_Snmp_1);
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TELNET:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, pStrInfo_common_Telnet);
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TFTP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, pStrInfo_common_Tftp_1);
    break;
  default:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u", val);
    break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  used for diffserv, Returns the exclude flag string.
*
* @param    unitIndex                L7_uint32
* @param    classIndex               L7_uint32
* @param    classRuleIndex           L7_uint32
* @param    excludeFlagString        L7_uchar8* string to return
* @param    excludeFlagStringLenMax  Maximum number of bytes to copy
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

void cliDiffServClassRuleMatchExcludeFlagStringGet(L7_uint32 unitIndex,
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uchar8 * excludeFlagString,
                                                   L7_int32 excludeFlagStringLenMax)
{
  L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t excludeFlag;

  if (usmDbDiffServClassRuleMatchExcludeFlagGet(unitIndex,
                                                classIndex,
                                                classRuleIndex,
                                                &excludeFlag) != L7_SUCCESS)
  {
    osapiSnprintf (excludeFlagString, excludeFlagStringLenMax,  "%-5s", pStrInfo_common_Line);
  }

  else if (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)
  {
    osapiSnprintf (excludeFlagString, excludeFlagStringLenMax,  "%-3s", pStrInfo_common_Yes);
  }

  else if (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE)
  {
    osapiSnprintf (excludeFlagString, excludeFlagStringLenMax,  "%-3s", pStrInfo_common_No);
  }

  else
  {
    osapiSnprintf (excludeFlagString, excludeFlagStringLenMax,  "%-5s", pStrInfo_common_Line);
  }

  return;
}

/*********************************************************************
* @purpose  used for diffserv, displays the Class Rules and can
*           be called recursively for Reference Classes
*
* @param    ewsContext      contains emWeb display
* @param    classIndex      index of the class whose rules are displayed
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

void cliDisplayClassRules(EwsContext ewsContext, L7_uint32 unitIndex,
                          L7_uint32 classIndex)
{
  L7_uint32 localClassIndex = classIndex;
  L7_uint32 ruleIndex = 0;
  L7_uint32 val = 0;
  L7_uint32 val2 = 0;
  L7_in6_addr_t ip6addr;
  L7_uint32 macAddrSize = L7_CLI_MAX_STRING_LENGTH;
  L7_uint32 macMaskSize = L7_CLI_MAX_STRING_LENGTH;
  L7_uint32 classNameSize = L7_CLI_MAX_STRING_LENGTH;
  L7_uchar8 macAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macMask[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 className[L7_CLI_MAX_STRING_LENGTH];
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t ruleType;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 valBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 val2Buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tempBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 excludeFlagString[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 eTypeString[L7_CLI_MAX_STRING_LENGTH];
  L7_QOS_ETYPE_KEYID_t etypeKeyId;

  L7_BOOL excludeSupported = L7_FALSE;

  if (usmDbFeaturePresentCheck(unitIndex, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) == L7_TRUE)
  {
    excludeSupported = L7_TRUE;
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 6, 0, L7_NULLPTR, ewsContext,
                             pStrInfo_qos_MatchCriteriaValsExcld);
    ewsTelnetWrite(ewsContext,
                   "\r\n---------------------------- ------------------------------------------- --------");

  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 6, 0, L7_NULLPTR, ewsContext,
                             pStrInfo_qos_MatchCriteriaVals);
    ewsTelnetWrite(ewsContext,
                   "\r\n---------------------------- -------------------------------------------");

  }

  if (usmDbDiffServClassRuleGetNext(unitIndex, localClassIndex, ruleIndex,
                                    &localClassIndex, &ruleIndex) != L7_SUCCESS)
  {
    return;
  }

  memset (excludeFlagString, 0, sizeof(excludeFlagString));
  while (localClassIndex == classIndex)
  {
    if (usmDbDiffServClassRuleMatchEntryTypeGet(unitIndex, localClassIndex, ruleIndex, &ruleType) != L7_SUCCESS)
    {
      continue;
    }

    if (excludeSupported == L7_TRUE)
    {
      cliDiffServClassRuleMatchExcludeFlagStringGet(unitIndex,
                                                    localClassIndex,
                                                    ruleIndex,
                                                    excludeFlagString,
                                                    sizeof(excludeFlagString));
    }

    switch (ruleType)
    {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
      if (usmDbDiffServClassRuleMatchCosGet(unitIndex, localClassIndex, ruleIndex,
                                            &val) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43d %-5s", pStrInfo_qos_ClassOfService, val, excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
      if (usmDbDiffServClassRuleMatchCos2Get(unitIndex, localClassIndex, ruleIndex,
                                             &val) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43d %-5s", pStrInfo_qos_SecondaryClassOfService, val, excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
      if ((usmDbDiffServClassRuleMatchDstIpAddrGet(unitIndex, localClassIndex, ruleIndex,
                                                   &val) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchDstIpMaskGet(unitIndex, localClassIndex, ruleIndex,
                                                   &val2) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        memset (valBuf, 0, sizeof(valBuf));
        memset (val2Buf, 0, sizeof(val2Buf));
        memset (tempBuf, 0, sizeof(tempBuf));
        if ((usmDbInetNtoa(val, valBuf) == L7_SUCCESS) &&
            (usmDbInetNtoa(val2, val2Buf) == L7_SUCCESS))
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%s (%s)", valBuf, val2Buf);
          ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_common_DstIpAddr, tempBuf,
                        excludeFlagString);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
      if ((usmDbDiffServClassRuleMatchDstIpv6AddrGet(unitIndex, localClassIndex, ruleIndex, &ip6addr) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchDstIpv6PrefLenGet(unitIndex, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
      {
        memset (valBuf, 0, sizeof(valBuf));
        if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ip6addr, valBuf, sizeof(valBuf)) != L7_NULLPTR)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf), "%s/%d", valBuf, val2);
          ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_common_DstIpAddr, tempBuf,
                        excludeFlagString);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      if ((usmDbDiffServClassRuleMatchDstL4PortStartGet(unitIndex, localClassIndex, ruleIndex,
                                                        &val) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchDstL4PortEndGet(unitIndex, localClassIndex, ruleIndex,
                                                      &val2) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        memset (tempBuf, 0, sizeof(tempBuf));
        memset (valBuf, 0, sizeof(valBuf));
        cliDiffservConvertL4PortValToString(val, valBuf, sizeof(valBuf));
        if (val == val2)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf), valBuf);
        }
        else
        {
          memset (val2Buf, 0, sizeof(val2Buf));
          cliDiffservConvertL4PortValToString(val2, val2Buf, sizeof(val2Buf));
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%s - %s", valBuf, val2Buf);
        }
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_DstLayer4Port, tempBuf,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
      macAddrSize = sizeof(macAddr);
      memset (macAddr, 0, macAddrSize);
      macMaskSize = sizeof(macMask);
      memset (macMask, 0, macMaskSize);
      if ((usmDbDiffServClassRuleMatchDstMacAddrGet(unitIndex, localClassIndex, ruleIndex,
                                                    macAddr,
                                                    &macAddrSize) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchDstMacMaskGet(unitIndex, localClassIndex, ruleIndex,
                                                    macMask, &macMaskSize) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        osapiSnprintf(tempBuf, sizeof(tempBuf),  "%02X:%02X:%02X:%02X:%02X:%02X (%02X:%02X:%02X:%02X:%02X:%02X)",
                      macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
                      macMask[0], macMask[1], macMask[2], macMask[3], macMask[4], macMask[5]);
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_DstMacAddr, tempBuf,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:

      if (usmDbDiffServClassRuleMatchEtypeKeyGet(unitIndex, localClassIndex, ruleIndex,
                                                 &etypeKeyId) == L7_SUCCESS)
      {
        if (etypeKeyId == L7_QOS_ETYPE_KEYID_CUSTOM)
        {
          if (usmDbDiffServClassRuleMatchEtypeValueGet(unitIndex, localClassIndex, ruleIndex,
                                                       &val) == L7_SUCCESS)
          {
            memset (buf, 0, sizeof(buf));
            osapiSnprintf(tempBuf, sizeof(tempBuf),  "0x%4.4x", (L7_ushort16)val);
            ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_EtherType, tempBuf,
                          excludeFlagString);
          }
        }
        else
        {
          memset (buf, 0, sizeof(buf));
          memset (eTypeString, 0, sizeof(eTypeString));
          cliDiffservConvertEtherTypeKeyIdToString(etypeKeyId, eTypeString, sizeof(eTypeString));
          ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_EtherType, eTypeString,
                        excludeFlagString);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
      ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_Any, pStrInfo_common_Line,
                    excludeFlagString);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
      if (usmDbDiffServClassRuleMatchIp6FlowLabelGet(unitIndex, localClassIndex, ruleIndex,
                                                     &val) == L7_SUCCESS)
      {
        osapiSnprintf(valBuf, sizeof(valBuf), "%d", (L7_uint32)val);
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_FlowLabel, valBuf,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      if (usmDbDiffServClassRuleMatchIpDscpGet(unitIndex, localClassIndex, ruleIndex,
                                               &val) == L7_SUCCESS)
      {
        memset (buf, 0, sizeof(buf));
        memset (valBuf, 0, sizeof(valBuf));
        cliDiffservConvertDSCPValToString(val, valBuf);
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_IpDscp, valBuf,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      if (usmDbDiffServClassRuleMatchIpPrecedenceGet(unitIndex, localClassIndex, ruleIndex,
                                                     &val) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43d %-5s", pStrInfo_qos_IpPrecedence, val,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      memset (valBuf, 0, sizeof(valBuf));
      memset (val2Buf, 0, sizeof(val2Buf));
      if ((usmDbDiffServClassRuleMatchIpTosBitsGet(unitIndex, localClassIndex, ruleIndex,
                                                   valBuf) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchIpTosMaskGet(unitIndex, localClassIndex, ruleIndex,
                                                   val2Buf) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        osapiSnprintf(tempBuf, sizeof(tempBuf),  "0x%02X (0x%02X)",
                      valBuf[0], val2Buf[0]);
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_IpTos, tempBuf,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
      if (usmDbDiffServClassRuleMatchProtocolNumGet(unitIndex, localClassIndex, ruleIndex,
                                                    &val) == L7_SUCCESS)
      {
        memset (buf, 0, sizeof(buf));
        memset (tempBuf, 0, sizeof(tempBuf));
        switch (val)
        {
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_ICMP:
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%u(%s)", val, pStrInfo_qos_Icmp);
          break;
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IGMP:
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%u(%s)", val, pStrInfo_common_Igmp_1);
          break;
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IP:
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%u(%s)", val, pStrInfo_common_Ip);
          break;
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_TCP:
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%u(%s)", val, pStrInfo_qos_Tcp);
          break;
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_UDP:
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%u(%s)", val, pStrInfo_qos_Udp);
          break;
        default:
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%u", val);
        }
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_common_Proto, tempBuf,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
      if (usmDbDiffServClassRuleMatchRefClassIndexGet(unitIndex, localClassIndex, ruleIndex,
                                                      &val) == L7_SUCCESS)
      {
        if (val == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE)
        {
          /* reference class removed */
          ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s",
                        pStrInfo_qos_RefClass,
                        pStrInfo_qos_Removed);
        }
        else
        {
          classNameSize = sizeof(className);
          memset (className, 0, classNameSize);
          if (usmDbDiffServClassNameGet(unitIndex, val,
                                        className,
                                        &classNameSize) == L7_SUCCESS)
          {
            ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s",
                          pStrInfo_qos_RefClass,
                          className,
                          excludeFlagString);
          }
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      if ((usmDbDiffServClassRuleMatchSrcIpAddrGet(unitIndex, localClassIndex, ruleIndex,
                                                   &val) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchSrcIpMaskGet(unitIndex, localClassIndex, ruleIndex,
                                                   &val2) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        memset (valBuf, 0, sizeof(valBuf));
        memset (val2Buf, 0, sizeof(val2Buf));
        memset (tempBuf, 0, sizeof(tempBuf));
        if ((usmDbInetNtoa(val, valBuf) == L7_SUCCESS) &&
            (usmDbInetNtoa(val2, val2Buf) == L7_SUCCESS))
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%s (%s)", valBuf, val2Buf);
          ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_SrcIpAddr, tempBuf,
                        excludeFlagString);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
      if ((usmDbDiffServClassRuleMatchSrcIpv6AddrGet(unitIndex, localClassIndex, ruleIndex, &ip6addr) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchSrcIpv6PrefLenGet(unitIndex, localClassIndex, ruleIndex, &val2) == L7_SUCCESS))
      {
        memset (valBuf, 0, sizeof(valBuf));
        if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ip6addr, valBuf, sizeof(valBuf)) != L7_NULLPTR)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf), "%s/%d", valBuf, val2);
          ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_SrcIpAddr, tempBuf,
                        excludeFlagString);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
      if ((usmDbDiffServClassRuleMatchSrcL4PortStartGet(unitIndex, localClassIndex, ruleIndex,
                                                        &val) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchSrcL4PortEndGet(unitIndex, localClassIndex, ruleIndex,
                                                      &val2) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        memset (tempBuf, 0, sizeof(tempBuf));
        memset (valBuf, 0, sizeof(valBuf));
        cliDiffservConvertL4PortValToString(val, valBuf, sizeof(valBuf));
        if (val == val2)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf), valBuf);
        }
        else
        {
          memset (val2Buf, 0, sizeof(val2Buf));
          cliDiffservConvertL4PortValToString(val2, val2Buf, sizeof(val2Buf));
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%s - %s", valBuf, val2Buf);
        }
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_SrcLayer4Port, tempBuf,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
      macAddrSize = sizeof(macAddr);
      memset (macAddr, 0, macAddrSize);
      macMaskSize = sizeof(macMask);
      memset (macMask, 0, macMaskSize);
      if ((usmDbDiffServClassRuleMatchSrcMacAddrGet(unitIndex, localClassIndex, ruleIndex,
                                                    macAddr, &macAddrSize) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchSrcMacMaskGet(unitIndex, localClassIndex, ruleIndex,
                                                    macMask, &macMaskSize) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        osapiSnprintf(tempBuf, sizeof(tempBuf),  "%02X:%02X:%02X:%02X:%02X:%02X (%02X:%02X:%02X:%02X:%02X:%02X)",
                      macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
                      macMask[0], macMask[1], macMask[2], macMask[3], macMask[4], macMask[5]);
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_SrcMacAddr, tempBuf,
                      excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:

      if ((usmDbDiffServClassRuleMatchVlanIdStartGet(unitIndex, localClassIndex, ruleIndex,
                                                     &val) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchVlanIdEndGet(unitIndex, localClassIndex, ruleIndex,
                                                   &val2) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        memset (tempBuf, 0, sizeof(tempBuf));
        if (val == val2)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%d", val);
        }
        else
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%d - %d", val, val2);
        }
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_common_VapVlan, tempBuf, excludeFlagString);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
      if ((usmDbDiffServClassRuleMatchVlanId2StartGet(unitIndex, localClassIndex, ruleIndex,
                                                      &val) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchVlanId2EndGet(unitIndex, localClassIndex, ruleIndex,
                                                    &val2) == L7_SUCCESS))
      {
        memset (buf, 0, sizeof(buf));
        memset (tempBuf, 0, sizeof(tempBuf));
        if (val == val2)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%d", val);
        }
        else
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%d - %d", val, val2);
        }
        ewsTelnetPrintf (ewsContext, "\r\n%-28s %-43s %-5s", pStrInfo_qos_SecondaryVlan, tempBuf, excludeFlagString);
      }
      break;

    default:
      break;
    }

    if (usmDbDiffServClassRuleGetNext(unitIndex, localClassIndex, ruleIndex,
                                      &localClassIndex, &ruleIndex) != L7_SUCCESS)
    {
      return;
    }
  }

  return;
}

/*********************************************************************
* @purpose  used for diffserv, displays the Class Rules and can
*           be called recursively for Reference Classes
*
* @param    ewsContext      contains emWeb display
* @param    classIndex      index of the class whose rules are displayed
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

void cliDisplayPolicyClassAttributes(EwsContext ewsContext,
                                     L7_uint32 unitIndex,
                                     L7_uint32 policyIndex,
                                     L7_uint32 policyInstIndex)
{
  L7_uint32 attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL] = {0};
  L7_uint32 tempPolicyIndex = policyIndex;
  L7_uint32 tempPolicyInstIndex = policyInstIndex;
  L7_uint32 policyAttrIndex = 0;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;
  L7_uint32 val = 0;
  L7_uint32 policeAttrIndex = 0;
  L7_BOOL hasInstances = L7_FALSE;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 bufval[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 bufLen;
  L7_uint32 unitNum, slot, port;
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t colorMode;
  L7_uint32 colorClassIndex;

  while ((usmDbDiffServPolicyAttrGetNext(unitIndex, tempPolicyIndex,
                                         tempPolicyInstIndex, policyAttrIndex,
                                         &tempPolicyIndex, &tempPolicyInstIndex,
                                         &policyAttrIndex) == L7_SUCCESS) &&
         (tempPolicyIndex == policyIndex) &&
         (tempPolicyInstIndex == policyInstIndex))
  {
    if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unitIndex, tempPolicyIndex,
                                                tempPolicyInstIndex,
                                                policyAttrIndex,
                                                &entryType) == L7_SUCCESS)
    {
      if (entryType < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL)
      {
        attrIndex[entryType] = policyAttrIndex;
        hasInstances = L7_TRUE;
      }
    }
  }
  if (hasInstances == L7_FALSE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_BestEffortWillBeUsed_1);
    return;
  }

  tempPolicyIndex = policyIndex;
  tempPolicyInstIndex = policyInstIndex;

  /* Display COS info */
  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL] != 0)
  {
    if (usmDbDiffServPolicyAttrStmtMarkCosValGet(unitIndex,tempPolicyIndex,
                                                 tempPolicyInstIndex,
                                                 attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL],
                                                 &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_MarkCos);             /*"Mark CoS\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }

  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL] != 0)
  {
    if (usmDbDiffServPolicyAttrStmtMarkCos2ValGet(unitIndex,tempPolicyIndex,
                                                  tempPolicyInstIndex,
                                                  attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL],
                                                  &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_MarkSecondaryCos);        /*"Mark Secondary CoS\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }

  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2] != 0)
  {
    if (usmDbDiffServPolicyAttrStmtMarkCosAsCos2Get(unitIndex,tempPolicyIndex,
                                                 tempPolicyInstIndex,
                                                 attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2],
                                                 &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_MarkCosAsCos2);
      if(val == L7_TRUE)
      {
        ewsTelnetPrintf (ewsContext, "Yes");
      }
      else if (val == L7_FALSE)
      {
        ewsTelnetPrintf (ewsContext, "No");
      }
    }
  }

  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP] != 0)
  {
    /* note:  no need to get the drop flag here */
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_TrafficWillBeDropped);
    return;
  }

  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT] != 0)
  {
    if (usmDbDiffServPolicyAttrStmtRedirectIntfGet(unitIndex,tempPolicyIndex,
                                                   tempPolicyInstIndex,
                                                   attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT],
                                                   &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_Redirect);             /*"Redirect\0"*/

      usmDbUnitSlotPortGet(val, &unitNum, &slot, &port);
      ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(unitNum, slot, port));
    }
  }

  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR] != 0)
  {
    if (usmDbDiffServPolicyAttrStmtMirrorIntfGet(unitIndex,tempPolicyIndex,
                                                 tempPolicyInstIndex,
                                                 attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR],
                                                 &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_MirrorShow);             /*"Mirror\0"*/

      usmDbUnitSlotPortGet(val, &unitNum, &slot, &port);
      ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(unitNum, slot, port));
    }
  }

  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE] != 0)
  {
    if (usmDbDiffServPolicyAttrStmtAssignQueueIdGet(unitIndex,tempPolicyIndex,
                                                    tempPolicyInstIndex,
                                                    attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE],
                                                    &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_AsSignQueue);             /*"Assign Queue\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }

  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL] != 0)
  {
    if (usmDbDiffServPolicyAttrStmtMarkIpDscpValGet(unitIndex,tempPolicyIndex,
                                                    tempPolicyInstIndex,
                                                    attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL],
                                                    &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_MarkIpDscp);             /*"Mark IP DSCP\0"*/
      memset (buf, 0, sizeof(buf));
      memset (bufval, 0, sizeof(bufval));
      cliDiffservConvertDSCPValToString(val, bufval);
      ewsTelnetPrintf (ewsContext, bufval);
    }
  }
  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL] != 0)
  {
    if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet(unitIndex,tempPolicyIndex,
                                                          tempPolicyInstIndex,
                                                          attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL],
                                                          &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_MarkIpPrecedence);             /*"Mark IP Precedence\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }
  if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE] != 0)
  {
    cliFormat(ewsContext, pStrInfo_qos_PolicingStyle);               /*"Policing Style\0"*/
    ewsTelnetWrite(ewsContext, pStrInfo_qos_PoliceSimple);          /*"Police Simple\0"*/
    policeAttrIndex = attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE];

    if (usmDbDiffServPolicyAttrStmtPoliceSimpleCrateGet(unitIndex,tempPolicyIndex,
                                                        tempPolicyInstIndex,
                                                        policeAttrIndex,
                                                        &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_CommittedRate);             /*"Committed Rate (Kbps)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
    if (usmDbDiffServPolicyAttrStmtPoliceSimpleCburstGet(unitIndex,tempPolicyIndex,
                                                         tempPolicyInstIndex,
                                                         policeAttrIndex,
                                                         &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_CommittedBurstSize);             /*"Committed Burst Size (Bytes)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }
  else if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE] != 0)
  {
    cliFormat(ewsContext, pStrInfo_qos_PolicingStyle);               /*"Policing Style\0"*/
    ewsTelnetWrite(ewsContext, pStrInfo_qos_PoliceSingleRate);          /*"Police Single Rate\0"*/
    policeAttrIndex = attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE];
    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateGet(unitIndex,tempPolicyIndex,
                                                            tempPolicyInstIndex,
                                                            policeAttrIndex,
                                                            &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_CommittedRate);             /*"Committed Rate (Kbps)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstGet(unitIndex,tempPolicyIndex,
                                                             tempPolicyInstIndex,
                                                             policeAttrIndex,
                                                             &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_CommittedBurstSize);             /*"Committed Burst Size (Bytes)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstGet(unitIndex,tempPolicyIndex,
                                                             tempPolicyInstIndex,
                                                             policeAttrIndex,
                                                             &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_ExcessBurstSize);             /*"Excess Burst Size (Bytes)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }
  else if (attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE] != 0)
  {
    cliFormat(ewsContext, pStrInfo_qos_PolicingStyle);               /*"Policing Style\0"*/
    ewsTelnetWrite(ewsContext, pStrInfo_qos_PoliceTwoRate);          /*"Police Two Rate\0"*/
    policeAttrIndex = attrIndex[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE];

    if (usmDbDiffServPolicyAttrStmtPoliceTworateCrateGet(unitIndex,tempPolicyIndex,
                                                         tempPolicyInstIndex,
                                                         policeAttrIndex,
                                                         &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_CommittedRate);             /*"Committed Rate (Kbps)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
    if (usmDbDiffServPolicyAttrStmtPoliceTworateCburstGet(unitIndex,tempPolicyIndex,
                                                          tempPolicyInstIndex,
                                                          policeAttrIndex,
                                                          &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_CommittedBurstSize);             /*"Committed Burst Size (Bytes)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
    if (usmDbDiffServPolicyAttrStmtPoliceTworatePrateGet(unitIndex,tempPolicyIndex,
                                                         tempPolicyInstIndex,
                                                         policeAttrIndex,
                                                         &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_PeakRate);             /*"Peak Rate (Kbps)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
    if (usmDbDiffServPolicyAttrStmtPoliceTworatePburstGet(unitIndex,tempPolicyIndex,
                                                          tempPolicyInstIndex,
                                                          policeAttrIndex,
                                                          &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_PeakBurstSize);             /*"Peak Burst Size (Bytes)\0"*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }
  if (policeAttrIndex != 0)
  {
    /* Conform Action */
    if (usmDbDiffServPolicyAttrStmtPoliceConformActGet(unitIndex,tempPolicyIndex,
                                                       tempPolicyInstIndex,
                                                       policeAttrIndex,
                                                       &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_ConformAction);             /*"Conform Action\0"*/
      switch ( val )                                                                  /* val = disable, enable, delete */
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_Drop);      /*"Drop\0"*/
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkIpDscp);      /*"Mark IP DSCP\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unitIndex,tempPolicyIndex,
                                                           tempPolicyInstIndex,
                                                           policeAttrIndex,
                                                           &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ConformDscpVal);         /*"Conform DSCP Value\0"*/
          memset (buf, 0, sizeof(buf));
          memset (bufval, 0, sizeof(bufval));
          cliDiffservConvertDSCPValToString(val, bufval);
          ewsTelnetPrintf (ewsContext, bufval);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkIpPrecedence);      /*"Mark IP Precedence\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unitIndex,tempPolicyIndex,
                                                           tempPolicyInstIndex,
                                                           policeAttrIndex,
                                                           &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ConformIpPrecedenceVal);         /*"Conform IP Precedence Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Send);      /*"Send \0"*/
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkCos);      /*"Mark COS\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unitIndex,tempPolicyIndex,
                                                           tempPolicyInstIndex,
                                                           policeAttrIndex,
                                                           &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ConformCosVal);         /*"Conform COS Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkCosAsCos2);      /*"Mark COS as Secondary COS (COS2) "*/
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkSecondaryCos);      /*"Mark Secondary COS\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unitIndex,tempPolicyIndex,
                                                           tempPolicyInstIndex,
                                                           policeAttrIndex,
                                                           &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ConformSecondaryCosVal);         /*"Conform Secondary COS Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;

      default:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Unknown_1);        /* unknown */
      }
    }

    /* Exceed Action */
    if (usmDbDiffServPolicyAttrStmtPoliceExceedActGet(unitIndex,tempPolicyIndex,
                                                      tempPolicyInstIndex,
                                                      policeAttrIndex,
                                                      &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_ExceedAction);             /*"Exceed Action\0"*/
      switch ( val )                                                                  /* val = disable, enable, delete */
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_Drop);      /*"Drop\0"*/
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkIpDscp);      /*"Mark IP DSCP\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unitIndex,tempPolicyIndex,
                                                          tempPolicyInstIndex,
                                                          policeAttrIndex,
                                                          &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ExceedDscpVal);         /*"Exceed DSCP Value\0"*/
          memset (buf, 0, sizeof(buf));
          memset (bufval, 0, sizeof(bufval));
          cliDiffservConvertDSCPValToString(val, bufval);
          ewsTelnetPrintf (ewsContext, bufval);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkIpPrecedenceVal);      /*"Mark IP Precedence Value\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unitIndex,tempPolicyIndex,
                                                          tempPolicyInstIndex,
                                                          policeAttrIndex,
                                                          &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ExceedIpPrecedenceVal);         /*"Exceed IP Precedence Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkCos);       /*"Mark COS\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unitIndex,tempPolicyIndex,
                                                          tempPolicyInstIndex,
                                                          policeAttrIndex,
                                                          &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ExceedCosVal);         /*"Conform COS Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkCosAsCos2);      /*"Mark COS as Secondary COS (COS2)"*/
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkSecondaryCos);       /*"Mark Secondary COS\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unitIndex,tempPolicyIndex,
                                                          tempPolicyInstIndex,
                                                          policeAttrIndex,
                                                          &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_ExceedSecondaryCosVal);         /*"Conform Secondary COS Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Send);      /*"Send \0"*/
        break;
      default:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Unknown_1);        /* unknown */
      }
    }

    /* Non-Conform Action */
    if (usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(unitIndex,tempPolicyIndex,
                                                          tempPolicyInstIndex,
                                                          policeAttrIndex,
                                                          &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_qos_NonConformAction);             /*"Non-Conform Action\0"*/
      switch ( val )                                                                  /* val = disable, enable, delete */
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_Drop);      /*"Drop\0"*/
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkIpDscp);      /*"Mark IP DSCP\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unitIndex,tempPolicyIndex,
                                                              tempPolicyInstIndex,
                                                              policeAttrIndex,
                                                              &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_NonConformDscpVal);         /*"Non-Conform DSCP Value\0"*/
          memset (buf, 0, sizeof(buf));
          memset (bufval, 0, sizeof(bufval));
          cliDiffservConvertDSCPValToString(val, bufval);
          ewsTelnetPrintf (ewsContext, bufval);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkIpPrecedence);      /*"Mark IP Precedence\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unitIndex,tempPolicyIndex,
                                                              tempPolicyInstIndex,
                                                              policeAttrIndex,
                                                              &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_NonConformIpPrecedenceVal);         /*"Non-Conform IP Precedence Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkCos);      /*"Mark COS\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unitIndex,tempPolicyIndex,
                                                              tempPolicyInstIndex,
                                                              policeAttrIndex,
                                                              &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_NonConformCosVal);         /*"Conform COS Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkCosAsCos2);      /*"Mark COS as Secondary COS (COS2)"*/
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_MarkSecondaryCos);      /*"Mark Secondary COS\0"*/
        if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unitIndex,tempPolicyIndex,
                                                              tempPolicyInstIndex,
                                                              policeAttrIndex,
                                                              &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_qos_NonConformSecondaryCosVal);         /*"Conform Secondary COS Value\0"*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Send);      /*"Send \0"*/
        break;
      default:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Unknown_1);        /* unknown */
      }
    }

    /* Conform Color */
    if (usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet(unitIndex,tempPolicyIndex,
                                                             tempPolicyInstIndex,
                                                             policeAttrIndex,
                                                             &colorMode) == L7_SUCCESS)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(unitIndex,tempPolicyIndex,
                                                                tempPolicyInstIndex,
                                                                policeAttrIndex,
                                                                &colorClassIndex) == L7_SUCCESS)
      {
        if (colorClassIndex != 0)
        {
          bufLen = (L7_uint32)sizeof(buf);
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ConformColorClass);      /*"Conform Color Class\0"*/
          cliFormat(ewsContext, buf);
          if (usmDbDiffServClassNameGet(unitIndex, colorClassIndex,
                                        buf, &bufLen) == L7_SUCCESS)
          {
            ewsTelnetWrite(ewsContext, buf);
          }
          else
          {
            ewsTelnetWrite(ewsContext, pStrInfo_common_Unknown_1);        /* unknown */
          }
        }
      }
      cliFormat(ewsContext, pStrInfo_qos_ConformColorMode);             /*"Conform Color Mode\0"*/
      switch ( colorMode )                                                                  /* val = disable, enable, delete */
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeBlind);      /*"Color Blind\0"*/
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
        memset (buf, 0, sizeof(buf));
        if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeAwareCos);       /*"Color Aware COS\0"*/
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ConformColorCosVal);       /*"Conform Color COS Value\0"*/
        }
        else if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeAwareSecondaryCos);      /*"Color Aware Secondary COS\0"*/
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ConformColorSecondaryCosVal);      /*"Conform Color Secondary COS Value\0"*/
        }
        else if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeAwareIpDscp);      /*"Color Aware IP DSCP\0"*/
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ConformColorIpDscpVal);      /*"Conform Color IP DSCP Value\0"*/
        }
        else       /* (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC) */
        {
          ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeAwareIpPrecedence);      /*"Color Aware IP Precedence\0"*/
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ConformColorIpPrecedenceVal);      /*"Conform Color IP Precedence Value\0"*/
        }
        if (usmDbDiffServPolicyAttrStmtPoliceColorConformValGet(unitIndex,tempPolicyIndex,
                                                                tempPolicyInstIndex,
                                                                policeAttrIndex,
                                                                &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, buf);
          memset (buf, 0, sizeof(buf));
          if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP)
          {
            cliDiffservConvertDSCPValToString(val, buf);
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf),  "%u", val);
          }
          ewsTelnetWrite(ewsContext, buf);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
        /* should never see this for conform-color */
        /* passthru */

      default:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Unknown_1);        /* unknown */
      }
    }

    /* Exceed Color (note: not used for police-simple) */
    if (usmDbDiffServPolicyAttrStmtPoliceColorExceedModeGet(unitIndex,tempPolicyIndex,
                                                            tempPolicyInstIndex,
                                                            policeAttrIndex,
                                                            &colorMode) == L7_SUCCESS)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(unitIndex,tempPolicyIndex,
                                                               tempPolicyInstIndex,
                                                               policeAttrIndex,
                                                               &colorClassIndex) == L7_SUCCESS)
      {
        if (colorClassIndex != 0)
        {
          bufLen = (L7_uint32)sizeof(buf);
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ExceedColorClass);      /*"Exceed Color Class\0"*/
          cliFormat(ewsContext, buf);
          if (usmDbDiffServClassNameGet(unitIndex, colorClassIndex,
                                        buf, &bufLen) == L7_SUCCESS)
          {
            ewsTelnetWrite(ewsContext, buf);
          }
          else
          {
            ewsTelnetWrite(ewsContext, pStrInfo_common_Unknown_1);        /* unknown */
          }
        }
      }
      cliFormat(ewsContext, pStrInfo_qos_ExceedColorMode);             /*"Exceed Color Mode\0"*/
      switch ( colorMode )                                                                  /* val = disable, enable, delete */
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
        ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeBlind);      /*"Color Blind\0"*/
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
        memset (buf, 0, sizeof(buf));
        if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeAwareCos);       /*"Color Aware COS\0"*/
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ExceedColorCosVal);       /*"Exceed Color COS Value\0"*/
        }
        else if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeAwareSecondaryCos);      /*"Color Aware Secondary COS\0"*/
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ExceedColorSecondaryCosVal);      /*"Exceed Color Secondary COS Value\0"*/
        }
        else if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP)
        {
          ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeAwareIpDscp);      /*"Color Aware IP DSCP\0"*/
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ExceedColorIpDscpVal);      /*"Exceed Color IP DSCP Value\0"*/
        }
        else       /* (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC) */
        {
          ewsTelnetWrite(ewsContext, pStrInfo_qos_ColorModeAwareIpPrecedence);      /*"Color Aware IP Precedence\0"*/
          osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_ExceedColorIpPrecedenceVal);      /*"Exceed Color IP Precedence Value\0"*/
        }
        if (usmDbDiffServPolicyAttrStmtPoliceColorExceedValGet(unitIndex,tempPolicyIndex,
                                                               tempPolicyInstIndex,
                                                               policeAttrIndex,
                                                               &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, buf);
          memset (buf, 0, sizeof(buf));
          if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP)
          {
            cliDiffservConvertDSCPValToString(val, buf);
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf),  "%u", val);
          }
          ewsTelnetWrite(ewsContext, buf);
        }
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
        ewsTelnetWrite(ewsContext, pStrErr_qos_ColorModeAwareNotSpecified);      /*"Not Specified\0"*/
        break;

      default:
        ewsTelnetWrite(ewsContext, pStrInfo_common_Unknown_1);        /* unknown */
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  used for diffserv, displays the general Service statistics
*
* @param    ewsContext     contains emWeb display
* @param    unitIndex      index of the unit
* @param    intIfIndex     index of the internal interface whose stats are displayed
* @param    ifDirection    direction of stats to be displayed
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

void cliDiffservDisplayServiceStats(EwsContext ewsContext,
                                    L7_uint32 unitIndex,
                                    L7_uint32 intIfIndex,
                                    L7_uint32 ifDirection)
{
  L7_uint32 packetsHi = 0;
  L7_uint32 packetsLo = 0;
  L7_uint32 octetsHi = 0;
  L7_uint32 octetsLo = 0;
  L7_char8 packetsbuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 octetsbuf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL offeredOctetsSupported = L7_FALSE;
  L7_BOOL offeredPacketsSupported = L7_FALSE;
  L7_BOOL discardedOctetsSupported = L7_FALSE;
  L7_BOOL discardedPacketsSupported = L7_FALSE;
  L7_BOOL sentOctetsSupported = L7_FALSE;
  L7_BOOL sentPacketsSupported = L7_FALSE;

  /* offered octets */
  if (usmDbDiffServServicePerfOfferedOctetsGet(unitIndex, intIfIndex, ifDirection,
                                               &octetsHi, &octetsLo) == L7_SUCCESS)
  {
    offeredOctetsSupported = L7_TRUE;
    strUtil64toa (octetsHi, octetsLo, octetsbuf, sizeof (octetsbuf));
  }

  if (usmDbDiffServServicePerfOfferedPacketsGet(unitIndex, intIfIndex, ifDirection,
                                                &packetsHi, &packetsLo) == L7_SUCCESS)
  {
    offeredPacketsSupported = L7_TRUE;
    strUtil64toa (packetsHi, packetsLo, packetsbuf, sizeof (packetsbuf));
  }

  if (offeredOctetsSupported == L7_TRUE && offeredPacketsSupported == L7_TRUE )
  {
    /* Both octets and packets supported */
    cliFormat(ewsContext, pStrInfo_qos_IntfOfferedOctetsPkts);
    ewsTelnetPrintf (ewsContext, "%s/%s", octetsbuf, packetsbuf);
  }
  else
  {
    if (offeredOctetsSupported == L7_TRUE)
    {
      cliFormat(ewsContext, pStrInfo_qos_IntfOfferedOctets);
      ewsTelnetPrintf (ewsContext, octetsbuf);
    }
    else if (offeredPacketsSupported == L7_TRUE)
    {
      cliFormat(ewsContext, pStrInfo_qos_IntfOfferedPkts);
      ewsTelnetPrintf (ewsContext, packetsbuf);
    }
  }

  /* discarded octets/packets */
  if (usmDbDiffServServicePerfDiscardedOctetsGet(unitIndex, intIfIndex, ifDirection,
                                                 &octetsHi, &octetsLo) == L7_SUCCESS)
  {
    discardedOctetsSupported = L7_TRUE;
    strUtil64toa (octetsHi, octetsLo, octetsbuf, sizeof (octetsbuf));
  }

  if (usmDbDiffServServicePerfDiscardedPacketsGet(unitIndex, intIfIndex, ifDirection,
                                                  &packetsHi, &packetsLo) == L7_SUCCESS)
  {
    discardedPacketsSupported = L7_TRUE;
    strUtil64toa (packetsHi, packetsLo, packetsbuf, sizeof (packetsbuf));
  }

  if (discardedOctetsSupported == L7_TRUE && discardedPacketsSupported == L7_TRUE )
  {
    /* Both octets and packets supported */
    cliFormat(ewsContext, pStrInfo_qos_IntfDiscardedOctetsPkts);
    ewsTelnetPrintf (ewsContext, "%s/%s", octetsbuf, packetsbuf);
  }
  else
  {
    if (discardedOctetsSupported == L7_TRUE)
    {
      cliFormat(ewsContext, pStrInfo_qos_IntfDiscardedOctets);
      ewsTelnetPrintf (ewsContext, octetsbuf);
    }
    else if (discardedPacketsSupported == L7_TRUE)
    {
      cliFormat(ewsContext, pStrInfo_qos_IntfDiscardedPkts);
      ewsTelnetPrintf (ewsContext, packetsbuf);
    }
  }

  /* sent octets/packets */
  if (usmDbDiffServServicePerfSentOctetsGet(unitIndex, intIfIndex, ifDirection,
                                            &octetsHi, &octetsLo) == L7_SUCCESS)
  {
    sentOctetsSupported = L7_TRUE;
    strUtil64toa (octetsHi, octetsLo, octetsbuf, sizeof (octetsbuf));
  }

  if (usmDbDiffServServicePerfSentPacketsGet(unitIndex, intIfIndex, ifDirection,
                                             &packetsHi, &packetsLo) == L7_SUCCESS)
  {
    sentPacketsSupported = L7_TRUE;
    strUtil64toa (packetsHi, packetsLo, packetsbuf, sizeof (packetsbuf));
  }

  if (sentOctetsSupported == L7_TRUE && sentPacketsSupported == L7_TRUE )
  {
    /* Both octets and packets supported */
    cliFormat(ewsContext, pStrInfo_qos_IntfSentOctetsPkts);
    ewsTelnetPrintf (ewsContext, "%s/%s", octetsbuf, packetsbuf);
  }
  else
  {
    if (sentOctetsSupported == L7_TRUE)
    {
      cliFormat(ewsContext, pStrInfo_qos_IntfSentOctets);
      ewsTelnetPrintf (ewsContext, octetsbuf);
    }
    else if (sentPacketsSupported == L7_TRUE)
    {
      cliFormat(ewsContext, pStrInfo_qos_IntfSentPkts);
      ewsTelnetPrintf (ewsContext, packetsbuf);
    }
  }
  return;
}

/*********************************************************************
* @purpose  builds a valid class types string for display
*
* @param    validTypes  buffer to hold string
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/

void cliDiffservBuildValidTypes(L7_char8 * validTypes,
                                L7_int32 validTypesLenMax,
                                L7_BOOL * allSupported,
                                L7_BOOL * anySupported,
                                L7_BOOL * aclSupported )
{
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  *aclSupported = L7_FALSE;
  *allSupported = L7_FALSE;
  *anySupported = L7_FALSE;

  memset (validTypes, 0, validTypesLenMax);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_TYPE_ALL_FEATURE_ID) == L7_TRUE)
  {
    *allSupported = L7_TRUE;
    osapiStrncpySafe(validTypes, pStrInfo_common_All, validTypesLenMax);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_TYPE_ANY_FEATURE_ID) == L7_TRUE)
  {
    *anySupported = L7_TRUE;
    if (strlen(validTypes) != 0)
    {
      osapiStrncat(validTypes, "/", validTypesLenMax - strlen(validTypes) - 1);
    }
    osapiStrncat( validTypes, pStrInfo_qos_AclAnyStr, validTypesLenMax - strlen(validTypes) - 1);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_TYPE_ACL_FEATURE_ID) == L7_TRUE)
  {
    *aclSupported = L7_TRUE;
    if (strlen(validTypes) != 0)
    {
      osapiStrncat(validTypes, "/", validTypesLenMax - strlen(validTypes) - 1);
    }
    osapiStrncat(validTypes, pStrInfo_qos_AclAclid, validTypesLenMax - strlen(validTypes) - 1);
  }
  return;
}

/*********************************************************************
* @purpose  builds a valid string of possible interfaces for display
*
* @param    direction         (input) indicates in or out
* @param    validTypes        (output) buffer to hold string
* @param    validTypesLenMax  (input) size of buffer to hold string
* @param    slotportsupported indicates if slot/port supported in this direction
* @param    allsupported      bool indicating if all is allowed in this direction
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/

void cliDiffservBuildValidIntfTypes(L7_uint32 direction,
                                    L7_char8 * validTypes,
                                    L7_int32 validTypesLenMax,
                                    L7_BOOL * slotPortSupported,
                                    L7_BOOL * allSupported )
{
  L7_uint32 unit;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  *allSupported = L7_FALSE;
  *slotPortSupported = L7_FALSE;

  memset (validTypes, 0, validTypesLenMax);
  if (direction == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN )
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_TRUE)
    {
      *slotPortSupported = L7_TRUE;
      /* osapiStrncpySafe( validTypes, "slot/port", validTypesLenMax); */
      osapiSnprintf(buf, sizeof(buf), cliSyntaxInterfaceHelp());
      osapiStrncpySafe(validTypes, buf, validTypesLenMax);
    }
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE)
    {
      *allSupported = L7_TRUE;
      if (strlen(validTypes) != 0)
      {
        osapiStrncat(validTypes, "/", validTypesLenMax - strlen(validTypes) - 1);
      }
      osapiStrncat(validTypes, pStrInfo_common_All, validTypesLenMax - strlen(validTypes) - 1);
    }
  }
  else if (direction == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT )
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE)
    {
      *slotPortSupported = L7_TRUE;
      /* osapiStrncpySafe( validTypes, "slot/port", validTypesLenMax); */
      osapiSnprintf(buf, sizeof(buf), cliSyntaxInterfaceHelp());
      osapiStrncpySafe(validTypes, buf, validTypesLenMax);
    }
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE)
    {
      *allSupported = L7_TRUE;
      if (strlen(validTypes) != 0)
      {
        osapiStrncat(validTypes, "/", validTypesLenMax - strlen(validTypes) - 1);
      }
      osapiStrncat(validTypes, pStrInfo_common_All, validTypesLenMax - strlen(validTypes) - 1);
    }
  }

  /* If an expected direction is not provided, an empty string is returned */
  return;
}

/*********************************************************************
* @purpose  builds a valid string of possible service directions for display
*
* @param    dirOptions        (output) buffer to hold string
* @param    dirOptionsLenMax  (input) maximum length of buffer to hold string
* @param    leftEnclosure     type of bracket on left of options
* @param    rightEnclosure    type of bracket on right of options
* @param    inSupported       indicates if in direction supported
* @param    outSupported      indicates if out direction is supported
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/

void cliDiffservBuildValidDirs(L7_char8 * dirOption,
                               L7_int32 dirOptionLenMax,
                               L7_char8 * leftEnclosure,
                               L7_char8 * rightEnclosure,
                               L7_BOOL * inSupported,
                               L7_BOOL * outSupported )
{
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  *inSupported = L7_FALSE;
  *outSupported = L7_FALSE;

  osapiSnprintf( dirOption, dirOptionLenMax,  leftEnclosure);

  /* set up service interface directional values based on feature support */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE )
  {
    *inSupported = L7_TRUE;
    osapiStrncat(dirOption, pStrInfo_common_AclInStr, dirOptionLenMax - strlen(dirOption) - 1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE )
  {
    *outSupported = L7_TRUE;
    if (*inSupported == L7_TRUE)
    {
      osapiStrncat(dirOption, pStrInfo_qos_Out_1, dirOptionLenMax - strlen(dirOption) - 1);
    }
    else
    {
      osapiStrncat(dirOption, pStrInfo_common_AclOutStr, dirOptionLenMax - strlen(dirOption) - 1);
    }
  }
  osapiStrncat(dirOption, rightEnclosure, dirOptionLenMax - strlen(dirOption) - 1);
  return;
}

void cliConstructPoliceParmsSyntax(L7_char8 * buf, L7_int32 bufLenMax)
{

  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrErr_qos_PoliceDrop, bufLenMax - strlen(buf) - 1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrErr_qos_PoliceSetCosTx, bufLenMax - strlen(buf) - 1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrErr_qos_SetCosAsCos2Tx, bufLenMax - strlen(buf) - 1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, pStrErr_qos_PoliceSetDscpTx, bufLenMax - strlen(buf) - 1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrErr_qos_PoliceSetPrecTx, bufLenMax - strlen(buf) - 1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrErr_qos_PoliceSetSecCosTx, bufLenMax - strlen(buf) - 1);
  }

  osapiStrncat(buf, pStrErr_qos_PoliceTx, bufLenMax - strlen(buf) - 1);
  return;
}

void cliConstructAndPrintPoliceSimpleSyntax(EwsContext ewsContext, L7_char8 * buf, L7_int32 bufLenMax)
{
  memset (buf, 0,sizeof(buf));
  osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_qos_PoliceSimple_1, bufLenMax - strlen(buf) - 1);
  cliConstructPoliceParmsSyntax(buf, bufLenMax);
  osapiStrncatAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_qos_PoliceViolate, bufLenMax - strlen(buf) - 1);
  cliConstructPoliceParmsSyntax(buf, bufLenMax);
  osapiStrncat(buf, pStrErr_qos_Close, bufLenMax - strlen(buf) - 1);
  ewsTelnetWrite( ewsContext, buf);
  cliSyntaxBottom(ewsContext);
  return;
}

void cliConstructAndPrintPoliceSingleRateSyntax(EwsContext ewsContext, L7_char8 * buf, L7_int32 bufLenMax)
{
  memset (buf, 0,sizeof(buf));
  osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_qos_PoliceSingleRate_1, bufLenMax - strlen(buf) - 1);
  cliConstructPoliceParmsSyntax(buf, bufLenMax);
  osapiStrncatAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_qos_PoliceExceed, bufLenMax - strlen(buf) - 1);
  cliConstructPoliceParmsSyntax(buf, bufLenMax);
  osapiStrncatAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_qos_PoliceViolate, bufLenMax - strlen(buf) - 1);
  cliConstructPoliceParmsSyntax(buf, bufLenMax);
  osapiStrncat(buf, pStrErr_qos_Close, bufLenMax - strlen(buf) - 1);
  ewsTelnetWrite( ewsContext, buf);
  cliSyntaxBottom(ewsContext);
  return;
}

void cliConstructAndPrintPoliceTwoRateSyntax(EwsContext ewsContext, L7_char8 * buf, L7_int32 bufLenMax)
{
  memset (buf, 0,sizeof(buf));
  osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_qos_PoliceTwoRate_1, bufLenMax - strlen(buf) - 1);
  cliConstructPoliceParmsSyntax(buf, bufLenMax);
  osapiStrncatAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_qos_PoliceExceed, bufLenMax - strlen(buf) - 1);
  cliConstructPoliceParmsSyntax(buf, bufLenMax);
  osapiStrncatAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_qos_PoliceViolate, bufLenMax - strlen(buf) - 1);
  cliConstructPoliceParmsSyntax(buf, bufLenMax);
  osapiStrncat(buf, pStrErr_qos_Close, bufLenMax - strlen(buf) - 1);
  ewsTelnetWrite( ewsContext, buf);
  cliSyntaxBottom(ewsContext);
  return;
}
