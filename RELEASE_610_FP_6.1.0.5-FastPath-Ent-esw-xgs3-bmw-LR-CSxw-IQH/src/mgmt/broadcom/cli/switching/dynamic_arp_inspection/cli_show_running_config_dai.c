
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/switching/dai/cli_show_running_config_dai.c
*
* @purpose show running config commands for the cli
*
* @component user interface
*
* @comments
*
* @create  10/01/2007
*
* @author  Kiran Kumar Kella
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "usmdb_util_api.h"
#include "comm_mask.h"
#include "cliapi.h"
#include "config_script_api.h"
#include "datatypes.h"
#ifndef _L7_OS_LINUX_
  #include <inetLib.h>    /* for converting from IP to L7_int32 */
#endif /* _L7_OS_LINUX_ */

#include "usmdb_dai_api.h"
#include "clicommands_dai.h"
#include "cli_show_running_config.h"

/*********************************************************************
 * @purpose  To print the running configuration of dynamic arp inspection
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 *
 * @notes
 *
 * @end
 **********************************************************************/
L7_RC_t cliRunningConfigDaiInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 vlanStart = 0, vlanEnd = 0, disableVlanStart = 0, disableVlanEnd = 0;
  L7_uint32 staticFlag = 0, first_time = 1;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 aclName[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL verifySMac, verifyDMac, verifyIP;
  L7_RC_t rc = L7_SUCCESS;
  L7_char8  vlanStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  *daiVlanShow = "ip arp inspection vlan %s";
  L7_char8  *daiVlanLoggingShow = "no ip arp inspection vlan %s logging";

  /* ip arp inspection vlan <vlan-list> */
  rc = usmDbDaiNextEnabledVlanRangeGet(&vlanStart, &vlanEnd);
  memset(vlanStr, 0, sizeof(vlanStr));
  while ((rc == L7_SUCCESS) && vlanStart)
  {
    if (vlanEnd > vlanStart)
    {
      if (first_time)
      {
        osapiSnprintf(buf, sizeof(buf), "%d-%d", vlanStart, vlanEnd);
        first_time = 0;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), ",%d-%d", vlanStart, vlanEnd);
      }
    }
    else
    {
      if (first_time)
      {
        osapiSnprintf(buf, sizeof(buf), "%d", vlanStart);
        first_time = 0;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), ",%d", vlanStart);
      }
    }
    osapiStrncat(vlanStr, buf, strlen(buf));
    if ((strlen(vlanStr) + strlen(daiVlanShow) - 2) >  /* -2 is for %s */
        (CLI_CONSOLE_WIDTH - 11)) /* 11 is max number of chars for a range */
    {
      osapiSnprintf(buf, sizeof(buf), daiVlanShow, vlanStr);
      EWSWRITEBUFFER (ewsContext, buf);
      memset(vlanStr, 0, sizeof(vlanStr));
      first_time = 1;
    }
    rc = usmDbDaiNextEnabledVlanRangeGet(&vlanStart, &vlanEnd);
  }
  if(vlanStr[0] != 0)
  {
    osapiSnprintf(buf, sizeof(buf), daiVlanShow, vlanStr);
    EWSWRITEBUFFER (ewsContext, buf);
    memset(vlanStr, 0, sizeof(vlanStr));
  }

  /* ip arp inspection vlan <vlan-list> logging */
  first_time = 1;
  disableVlanStart = 1;
  vlanStart = vlanEnd = 0;
  rc = usmDbDaiVlanNextLoggingEnableRangeGet(&vlanStart, &vlanEnd);
  memset(vlanStr, 0, sizeof(vlanStr));
  while ((rc == L7_SUCCESS) && vlanStart)
  {
    disableVlanEnd = vlanStart-1;
    if((FD_DAI_LOG_INVALID == L7_ENABLE) && (EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT))
    {
      if (vlanEnd > vlanStart)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nip arp inspection vlan %d-%d logging", 
                      vlanStart, vlanEnd);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nip arp inspection vlan %d logging", 
                      vlanStart);
      }
      EWSWRITEBUFFER (ewsContext, buf);
    }
    if((FD_DAI_LOG_INVALID == L7_ENABLE) ||
       ((FD_DAI_LOG_INVALID == L7_DISABLE) && (EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT)))
    {
      if(disableVlanEnd > disableVlanStart)
      {
        if (first_time)
        {
          osapiSnprintf(buf, sizeof(buf), "%d-%d", disableVlanStart, disableVlanEnd);
          first_time = 0;
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), ",%d-%d", disableVlanStart, disableVlanEnd);
        }
        osapiStrncat(vlanStr, buf, strlen(buf));
      }
      else if(disableVlanStart == disableVlanEnd)
      {
        if (first_time)
        {
          osapiSnprintf(buf, sizeof(buf), "%d", disableVlanStart);
          first_time = 0;
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), ",%d", disableVlanStart);
        }
        osapiStrncat(vlanStr, buf, strlen(buf));
      }
      if ( (vlanStr[0] != 0) &&
           ((strlen(vlanStr) + strlen(daiVlanLoggingShow) - 2) >  /* -2 is for %s */
            (CLI_CONSOLE_WIDTH - 11)) ) /* 11 is max number of chars for a range */
      {
        osapiSnprintf(buf, sizeof(buf), daiVlanLoggingShow, vlanStr);
        EWSWRITEBUFFER (ewsContext, buf);
        memset(vlanStr, 0, sizeof(vlanStr));
        first_time = 1;
      }

      disableVlanStart = vlanEnd+1;
    }
    rc = usmDbDaiVlanNextLoggingEnableRangeGet(&vlanStart, &vlanEnd);
  }

  if((FD_DAI_LOG_INVALID == L7_ENABLE) ||
     ((FD_DAI_LOG_INVALID == L7_DISABLE) && (EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT)))
  {
    if((disableVlanStart <= L7_DOT1Q_MAX_VLAN_ID) && (vlanEnd < L7_DOT1Q_MAX_VLAN_ID))
    {
      if(disableVlanStart < L7_DOT1Q_MAX_VLAN_ID)
      {
        if (first_time)
        {
          osapiSnprintf(buf, sizeof(buf), "%d-%d", disableVlanStart, L7_DOT1Q_MAX_VLAN_ID);
          first_time = 0;
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), ",%d-%d", disableVlanStart, L7_DOT1Q_MAX_VLAN_ID);
        }
        osapiStrncat(vlanStr, buf, strlen(buf));
      }
      else if(disableVlanStart == L7_DOT1Q_MAX_VLAN_ID)
      {
        if (first_time)
        {
          osapiSnprintf(buf, sizeof(buf), "%d", disableVlanStart);
          first_time = 0;
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), ",%d", disableVlanStart);
        }
        osapiStrncat(vlanStr, buf, strlen(buf));
      }
    }
  }
  if(vlanStr[0] != 0)
  {
    osapiSnprintf(buf, sizeof(buf), daiVlanLoggingShow, vlanStr);
    EWSWRITEBUFFER (ewsContext, buf);
    memset(vlanStr, 0, sizeof(vlanStr));
  }

  vlanStart = vlanEnd = 0;
  /* ip arp inspection filter <acl-name> vlan <vlan-range> [static] */

  /* We are not displaying the comma-seperated vlan-list in this command, as
   * there is possibility of num of chars in the line crossing 80 when
   * acl-name is configured with max possible length of 31 characters.
   * So, we just display a single vlan-range or single vlan-id in each line */
  rc = usmDbDaiVlanNextArpAclRangeGet(&vlanStart, &vlanEnd, aclName, &staticFlag);
  while ((rc == L7_SUCCESS) && vlanStart)
  {
    if (vlanEnd > vlanStart)
    {
      osapiSnprintf(buf, sizeof(buf), "\r\nip arp inspection filter %s vlan %d-%d%s",
                    aclName, vlanStart, vlanEnd,
                    ((staticFlag == L7_ENABLE) ? " static" : ""));
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "\r\nip arp inspection filter %s vlan %d%s",
                    aclName, vlanStart,
                    ((staticFlag == L7_ENABLE) ? " static" : ""));
    }
    EWSWRITEBUFFER (ewsContext, buf);
    rc = usmDbDaiVlanNextArpAclRangeGet(&vlanStart, &vlanEnd, aclName, &staticFlag);
  }

  /* ip arp inspection validate {[src-mac] [dst-mac] [ip]} */
  if ((usmDbDaiVerifySMacGet(&verifySMac) == L7_SUCCESS) &&
      (usmDbDaiVerifyDMacGet(&verifyDMac) == L7_SUCCESS) &&
      (usmDbDaiVerifyIPGet(&verifyIP) == L7_SUCCESS))
  {
    if(! ((verifySMac == FD_DAI_VERIFY_SOURCE_MAC) &&
           (verifyDMac == FD_DAI_VERIFY_DEST_MAC) &&
           (verifyIP == FD_DAI_VERIFY_IP)))
    {
      osapiSnprintf(buf, sizeof(buf), "\r\nip arp inspection validate%s%s%s", 
                    ((verifySMac == L7_ENABLE) ? " src-mac" : ""),
                    ((verifyDMac == L7_ENABLE) ? " dst-mac" : ""),
                    ((verifyIP == L7_ENABLE) ? " ip" : ""));
      EWSWRITEBUFFER (ewsContext, buf);
    }
    if(EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT)
    {
      osapiSnprintf(buf, sizeof(buf), "\r\nno ip arp inspection validate%s%s%s", 
                    ((verifySMac == L7_DISABLE) ? " src-mac" : ""),
                    ((verifyDMac == L7_DISABLE) ? " dst-mac" : ""),
                    ((verifyIP == L7_DISABLE) ? " ip" : ""));
      EWSWRITEBUFFER (ewsContext, buf);
    }
  }
    
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Print the running configuration of DAI for an interface
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32  intIfNum       internal interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 **********************************************************************/
L7_RC_t cliRunningConfigDaiInterfaceInfo(EwsContext ewsContext,
                                         L7_uint32 intIfNum)
{
  L7_BOOL trust;
  L7_int32 rate;
  L7_uint32 burst;
  L7_char8 cmdStr[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbDaiIntfTrustGet(intIfNum, &trust) == L7_SUCCESS)
  {
    osapiSnprintf(cmdStr, sizeof(cmdStr), "ip arp inspection trust");
    cliShowCmdTrue(ewsContext, trust, FD_DAI_TRUST, cmdStr);
  }

  if ((usmDbDaiIntfRateLimitGet(intIfNum, &rate) == L7_SUCCESS) &&
      (usmDbDaiIntfBurstIntervalGet(intIfNum, &burst) == L7_SUCCESS))
  {
    if((rate == FD_DAI_RATE_LIMIT) && (burst == FD_DAI_BURST_INTERVAL))
    {
      if(EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT)
      {
        osapiSnprintf(cmdStr, sizeof(cmdStr), "no ip arp inspection limit");
        EWSWRITEBUFFER (ewsContext, cmdStr);
      }
    }
    else if(rate == L7_DAI_RATE_LIMIT_NONE)
    {
      osapiSnprintf(cmdStr, sizeof(cmdStr), "ip arp inspection limit none");
      EWSWRITEBUFFER (ewsContext, cmdStr);
    }
    else
    {
      osapiSnprintf(cmdStr, sizeof(cmdStr), "ip arp inspection limit rate %d burst interval %d",rate, burst);
      EWSWRITEBUFFER (ewsContext, cmdStr);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Print the running configuration of ARP ACLs
 *
 * @param    EwsContext ewsContext
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 **********************************************************************/
L7_RC_t cliRunningConfigArpAclInfo(EwsContext ewsContext)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 arpAclCurrent[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macAddrCurrent[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cmdStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipAddrCurrent;
  L7_BOOL first = L7_TRUE;

  memset(arpAclCurrent, 0, sizeof(arpAclCurrent));
  memset(macAddrCurrent, 0, sizeof(macAddrCurrent));
  ipAddrCurrent = 0;
  for(;;)
  {
    if(rc != L7_SUCCESS)
    {
      if(usmDbArpAclNextGet(arpAclCurrent, arpAclCurrent) != L7_SUCCESS)
      {
        if(! first)
        {
          EWSWRITEBUFFER_ADD_BLANKS(1, 1, 0, 0, L7_NULLPTR,
                                    ewsContext, pStrInfo_common_Exit); /*exit from arp access-list <acl-name> */
        }
        break;
      }
      else
      {
        if(! first)
        {
          EWSWRITEBUFFER_ADD_BLANKS(1, 1, 0, 0, L7_NULLPTR,
                                    ewsContext, pStrInfo_common_Exit); /*exit from arp access-list <acl-name> */
        }
        else
        {
          first = L7_FALSE;
        }
      }

      memset(macAddrCurrent, 0, sizeof(macAddrCurrent));
      ipAddrCurrent = 0;
      osapiSnprintf(cmdStr, sizeof(cmdStr), "arp access-list %s",arpAclCurrent);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      usmDbInetNtoa(ipAddrCurrent, buf);
      osapiSnprintf(cmdStr, sizeof(cmdStr), "permit ip host %s mac host %s",buf, macAddrCurrent);
    }
    EWSWRITEBUFFER (ewsContext, cmdStr);
    rc = usmDbArpAclRuleInAclNextGet(arpAclCurrent, ipAddrCurrent, macAddrCurrent,
                                     &ipAddrCurrent, macAddrCurrent);
  }
  return L7_SUCCESS;
}

