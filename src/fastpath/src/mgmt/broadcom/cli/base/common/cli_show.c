/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cli_show.c
*
* @purpose show commands for the cli
*
* @component user interface
*
* @comments none
*
* @create  08/09/2000
*
* @author  Forrest Samuels
* @end
*
**********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_base_web.h"
#include "strlib_ip_mcast_cli.h"
#include "strlib_common_web.h"
#include "l7_common.h"
#include "dot1q_exports.h"
#include "log_exports.h"
#include "nim_exports.h"
#include "fdb_exports.h"
#include "snmp_exports.h"
#include "sntp_exports.h"
#include "acl_exports.h"
#include "dot3ad_exports.h"
#include "trapmgr_exports.h"
#include "sim_exports.h"
#include "dtlapi.h"
#include "usmdb_1213_api.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_common.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mfdb_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_mib_vrrp_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_ospfv3_api.h"
#include "usmdb_policy_api.h"
#include "usmdb_registry_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_slotmapper.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_sntp_api.h"
#include "usmdb_status.h"
#include "usmdb_telnet_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_unitmgr_api.h"
#include "comm_mask.h"
#include "session.h"
#include "datatypes.h"
#include "ctype.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"
#include "cliutil_dot1s.h"
#include "ews.h"
#include "dot1q_api.h"
#include "osapi.h"
#include "osapi_support.h"
#include "clicommands_dot1s.h"
#include "usmdb_support.h"
#include "usmdb_dhcps_api.h"
#include "usmdb_log_api.h"
#include "log.h"
#include "clicommands_card.h"
#include "unitmgr_api.h"

#include "usmdb_garp.h"
#include "usmdb_cmd_logger_api.h"
#include "usmdb_user_mgmt_api.h"
#include "bspapi.h"
#include "usmdb_iputil_api.h"
#include "usmdb_util_api.h"
#include "strlib_qos_cli.h"
#include "snooping_exports.h"
#ifdef L7_SFLOW_PACKAGE
  #include "usmdb_sflow.h"
#endif /* sFlow Package */
#ifdef L7_STACKING_PACKAGE
#include "clicommands_stacking.h"
#endif
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  #include "clicommands_acl.h"
#endif
#ifdef L7_ROUTING_PACKAGE
  #include "clicommands_l3.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_ipv6.h"
#endif
#endif
#ifdef L7_BGP_PACKAGE
  #include "usmdb_bgp4_api.h"
  #include "clicommands_bgp_l3.h"
#endif
#ifdef L7_MCAST_PACKAGE
  #include "clicommands_mcast.h"
#endif
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
#include "clicommands_base_ipv6.h"
#include "usmdb_dhcp6c_api.h"
#endif

#include "usmdb_dns_client_api.h"

#ifdef L7_CHASSIS
#include "clicommands_chassis.h"
#endif

#define IS_BIT_SET(bit, map) ((bit) & (map))
#include "usmdb_keying_api.h"

#include "dot3ad_api.h"
#include "clicommands_debug.h"
#ifdef L7_ROUTING_PACKAGE
#include "clicommands_debug_l3.h"
#endif

#ifdef L7_AUTO_INSTALL_PACKAGE
#include "usmdb_auto_install_api.h"
#endif /* L7_AUTO_INSTALL_PACKAGE */
#ifdef L7_QOS_FLEX_PACKAGE_VOIP
#include "usmdb_qos_voip_api.h"
#endif
#ifdef L7_ISDP_PACKAGE
#include "usmdb_isdp_api.h"
#include "clicommands_isdp.h"
#endif

#ifdef L7_DOT3AH_PACKAGE
#include "usmdb_dot3ah.h" 
#endif

#ifdef L7_RLIM_PACKAGE 
#include "clicommands_loopback.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#endif /* end of L7_IPV6_PACKAGE */
#endif /* end of L7_RLIM_PACKAGE */ 

#include "sdm_exports.h"
#include "usmdb_sdm.h"

#ifdef L7_MGMT_SECURITY_PACKAGE
  void cliShowIpHttpSecure(EwsContext ewsContext);
#endif

static void cliDisplayDnsStaticEntries(EwsContext ewsContext, 
            L7_uchar8 *hostAddr, L7_uchar8 *strAddr, L7_uint32 *count);

static void cliDisplayDnsDynamicEntries(EwsContext ewsContext, 
                                 L7_uchar8 *hostAddr, L7_uchar8 *hostIPAddr, 
                                 L7_uint32    ttl, L7_uint32    elapsed, 
                                 L7_uchar8 *strrrType, L7_uint32 *count);

/*********************************************************************
 *
 * @purpose   display the forwarding database address aging timeout.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes
 *
 * @cmdsyntax
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShowFDBAgetime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 count, val;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argFdbId = 1;
  static L7_uint32 fdbId;
  L7_BOOL all = L7_TRUE;
  L7_FDB_TYPE_t fdb_type = L7_SVL;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliCmdScrollSet( L7_FALSE);
  usmDbFDBTypeOfVLGet(unit, &fdb_type);

  if (cliNumFunctionArgsGet() > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowFdbAgeTimeIvl, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {  /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (cliNumFunctionArgsGet() == 1)
    {
      if (strcmp(argv[index+argFdbId], pStrInfo_common_All) != 0)
         {
            all = L7_FALSE;
            if (fdb_type != L7_IVL)
            {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbAgeTimeSvl);
            }
            else if (( cliConvertTo32BitUnsignedInteger(argv[index+argFdbId], &fdbId) != L7_SUCCESS ) ||
                     ( fdbId < L7_DOT1Q_MIN_VLAN_ID ) ||
                     ( fdbId > L7_DOT1Q_MAX_VLAN_ID ))
            {
          return cliSyntaxReturnPromptAddBlanks (0, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
              pStrInfo_base_IncorrectInputVlanIdMustBeInRangeOfTo, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
        }
        /* get the first entry after the zero parameter */
        if (L7_SUCCESS != usmDbFdbIdGet(unit, fdbId))
        {
          return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FwdDbaseId);
        }
      }
    }
    if (all == L7_TRUE)
    {
      fdbId = 0;
      /* get the first entry after the zero parameter */
      if (L7_SUCCESS != usmDbNextVlanGet(unit, fdbId, &fdbId))
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_FwdDataBaseEmpty);
      }
    }
  }

  if (fdb_type == L7_SVL)
  {
    ewsTelnetPrintf (ewsContext, "\r\n%-20s:", pStrInfo_base_AddrAgingTimeout);
    memset (buf, 0, sizeof(buf));
    usmDbDot1dTpAgingTimeGet(unit, &val);
    return cliSyntaxReturnPrompt (ewsContext,"%-15u", val);
  }

  /* IVL Case */
  ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext,pStrInfo_base_FwddbIdAddrAgingTimeoutInSeconds);
  ewsTelnetWrite(ewsContext,"\r\n-----------------  ----------------------------------");

  for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    osapiSnprintf(buf, sizeof(buf), "\r\n%d",fdbId);
    ewsTelnetPrintf (ewsContext, "%-21.21s", buf);

    usmDbFDBAddressAgingTimeoutGet(unit, fdbId, &val);
    osapiSnprintf(buf, sizeof(buf), "%d", val);
    ewsTelnetPrintf (ewsContext, "%-10.10s ", buf);

    if ((all == L7_FALSE) || (L7_SUCCESS != usmDbNextVlanGet(unit, fdbId, &fdbId)))
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  cliSyntaxBottom(ewsContext);
  /*hard coded in command name for now, #define later */
  cliAlternateCommandSet(pStrInfo_base_ShowFwddbAgetimeAll);
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */

}

/*********************************************************************
*
* @purpose   display the forwarding database entries for Learned addresses.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show forwardingDb learned  [vlanplusmacaddr/all]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowFDBLearned(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 count, val, temp;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 mac_address[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 s;
   L7_uint32 p;
   static usmdbFdbEntry_t fdbEntry;
   L7_FDB_TYPE_t fdb_type = L7_SVL;
   L7_BOOL all = L7_TRUE;
   L7_uint32 unit;
   L7_ushort16 typeVal;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   cliCmdScrollSet( L7_FALSE);
   usmDbFDBTypeOfVLGet(unit, &fdb_type);

   if (cliNumFunctionArgsGet() > 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbLearned);
   }

   if (cliGetCharInputID() != CLI_INPUT_EMPTY)
   {
      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
      if (cliNumFunctionArgsGet() == 1)
      {
      if (strcmp(argv[index+1], pStrInfo_common_All) != 0)
         {
            all = L7_FALSE;
        OSAPI_STRNCPY_SAFE(mac_address, argv[index+1]);
            temp = strlen(mac_address);
            if (fdb_type == L7_IVL)
            {
               if (temp != 23)
               {
            return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_Learned_1);
          }
          if (cliConvertVlanMac(mac_address,mac) != L7_TRUE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbLearned);
               }
               memcpy(fdbEntry.usmdbFdbMac, mac, L7_MAC_ADDR_LEN+L7_FDB_IVL_ID_LEN);
            }
            else
            {
               if (temp != 17)
               {
            return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_Learned_1);
          }
          if (cliConvertVlanMac(mac_address,mac) != L7_TRUE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbLearned);
          }
          memcpy(fdbEntry.usmdbFdbMac, mac, L7_MAC_ADDR_LEN);
        }

        if (L7_SUCCESS != usmDbFDBEntryGet(unit, &fdbEntry))
        {
          return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_MacEntryNotFound);
        }
      }
    }

    if (all == L7_TRUE)
    {
      memset(fdbEntry.usmdbFdbMac, 0, L7_MAC_ADDR_LEN);
      /* get the first entry after the zero parameter */
      if (L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry))
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_FwdDataBaseEmpty);
      }
    }
  }
  /* printing header info.  MAC is 8 bytes long for IVL; 6 bytes for SVL*/
  if (fdb_type == L7_IVL)
  {
    ewsTelnetWriteAddBlanks (1, 0, 6, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIntfIfIdxStatus);
      ewsTelnetWrite(ewsContext,"\r\n-----------------------  ---------  ---------  ------------");
   }

   for (count = 0; count < CLI_MAX_SCROLL_LINES-6;)
   {
      typeVal =  fdbEntry.usmdbFdbEntryType;
      if (typeVal == L7_FDB_ADDR_FLAG_LEARNED)
      {
         if (fdb_type == L7_IVL)
         {
        osapiSnprintf(mac_address, sizeof(mac_address),
            "\r\n%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X ",
                    fdbEntry.usmdbFdbMac[0],
                    fdbEntry.usmdbFdbMac[1],
                    fdbEntry.usmdbFdbMac[2],
                    fdbEntry.usmdbFdbMac[3],
                    fdbEntry.usmdbFdbMac[4],
                    fdbEntry.usmdbFdbMac[5],
                    fdbEntry.usmdbFdbMac[6],
                    fdbEntry.usmdbFdbMac[7]);
         }
         else
         {
        osapiSnprintf(mac_address, sizeof(mac_address),
            "\r\n%02X:%02X:%02X:%02X:%02X:%02X ",
                    fdbEntry.usmdbFdbMac[0],
                    fdbEntry.usmdbFdbMac[1],
                    fdbEntry.usmdbFdbMac[2],
                    fdbEntry.usmdbFdbMac[3],
                    fdbEntry.usmdbFdbMac[4],
                    fdbEntry.usmdbFdbMac[5]);
         }
         count++;
      ewsTelnetPrintf (ewsContext, "%-26.26s ", mac_address);

      if (usmDbUnitSlotPortGet(fdbEntry.usmdbFdbIntIfNum, &unit, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit,s,p));
        ewsTelnetPrintf (ewsContext, "%-11.11s", buf);
      }

      if (usmDbIfIndexGet(unit, fdbEntry.usmdbFdbIntIfNum, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",val);
        ewsTelnetPrintf (ewsContext, "%-10.10s ", buf);
      }
      ewsTelnetPrintf (ewsContext, "%-13.13s ", pStrInfo_base_Learned);
    }
    if (L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry))
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
      }

      if (count == CLI_MAX_SCROLL_LINES-6)
      {
         cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
         cliSyntaxBottom(ewsContext);
      cliAlternateCommandSet(pStrInfo_base_ShowFwddbLearned);
      return pStrInfo_common_Name_2;      /* --More-- or (q)uit */
      }
   }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   display the forwarding database entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowFDBTable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 count, val;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 mac_address[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 s;
   L7_uint32 p;
   static usmdbFdbEntry_t fdbEntry;
   L7_FDB_TYPE_t fdb_type = L7_SVL;
   L7_BOOL all = L7_TRUE;
   L7_uint32 unit;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   cliCmdScrollSet( L7_FALSE);
   usmDbFDBTypeOfVLGet(unit, &fdb_type);

   if (cliNumFunctionArgsGet() > 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbTbl);
   }

   if (cliGetCharInputID() != CLI_INPUT_EMPTY)
   {                                                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
      if (cliNumFunctionArgsGet() == 1)
      {
      if (strcmp(argv[index+1], pStrInfo_common_All) != 0)
         {
            all = L7_FALSE;
            if (fdb_type == L7_IVL)
            {
          osapiStrncpySafe(mac_address, argv[index+1], min(25, (sizeof(mac_address)-1)));
               if (cliConvertVlanMac(mac_address,mac) != L7_TRUE)
               {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbTbl);
               }
               memcpy(fdbEntry.usmdbFdbMac, mac, L7_MAC_ADDR_LEN+L7_FDB_IVL_ID_LEN);
            }
            else
            {
          osapiStrncpySafe(mac_address, argv[index+1], min(19, (sizeof(mac_address)-1)));
               if (cliConvertVlanMac(mac_address,mac) != L7_TRUE)
               {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbTbl);
               }
               memcpy(fdbEntry.usmdbFdbMac, mac, L7_MAC_ADDR_LEN);
            }

            if (L7_SUCCESS != usmDbFDBEntryGet(unit, &fdbEntry))
            {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_MacEntryNotFound);
            }
         }
      }

      if (all == L7_TRUE)
      {
         memset(fdbEntry.usmdbFdbMac, 0, L7_MAC_ADDR_LEN);
         if (L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry))           /* get the first entry after the zero parameter */
         {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_FwdDataBaseEmpty);
         }
      }
   }

   /* printing header info.  MAC is 8 bytes long for IVL; 6 bytes for SVL*/
   if (fdb_type == L7_IVL)
   {
    ewsTelnetWriteAddBlanks (1, 0, 6, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIntfIfIdxStatus);
      ewsTelnetWrite(ewsContext,"\r\n-----------------------  ---------  ---------  ------------");
   }
   else
   {
    ewsTelnetWriteAddBlanks (1, 0, 3, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIntfIfIdxStatus_2);
      ewsTelnetWrite(ewsContext,"\r\n-----------------  ---------  ---------  ------------");
   }

   for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
   {
      if (fdb_type == L7_IVL)
      {
      osapiSnprintf(mac_address, sizeof(mac_address),
          "\r\n%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X ",
                 fdbEntry.usmdbFdbMac[0],
                 fdbEntry.usmdbFdbMac[1],
                 fdbEntry.usmdbFdbMac[2],
                 fdbEntry.usmdbFdbMac[3],
                 fdbEntry.usmdbFdbMac[4],
                 fdbEntry.usmdbFdbMac[5],
                 fdbEntry.usmdbFdbMac[6],
                 fdbEntry.usmdbFdbMac[7]);
      osapiSnprintf(stat,sizeof(stat),"%-26.26s ", mac_address);
      }
      else
      {
      osapiSnprintf(mac_address, sizeof(mac_address),
          "\r\n%02X:%02X:%02X:%02X:%02X:%02X ",
                 fdbEntry.usmdbFdbMac[0],
                 fdbEntry.usmdbFdbMac[1],
                 fdbEntry.usmdbFdbMac[2],
                 fdbEntry.usmdbFdbMac[3],
                 fdbEntry.usmdbFdbMac[4],
                 fdbEntry.usmdbFdbMac[5]);
      osapiSnprintf(stat,sizeof(stat),"%-20.20s ", mac_address);
      }

      ewsTelnetWrite(ewsContext, stat);

      if (usmDbUnitSlotPortGet(fdbEntry.usmdbFdbIntIfNum, &unit, &s, &p) == L7_SUCCESS)
      {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit,s,p));
      ewsTelnetPrintf (ewsContext, "%-11.11s", buf);
    }

    if (usmDbIfIndexGet(unit, fdbEntry.usmdbFdbIntIfNum, &val) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), "%d",val);
      ewsTelnetPrintf (ewsContext, "%-10.10s ", buf);
    }
    switch (fdbEntry.usmdbFdbEntryType)
    {
      case L7_FDB_ADDR_FLAG_STATIC:
        osapiSnprintf(stat, sizeof(stat), "%-13.13s ", pStrInfo_common_PimSmGrpRpMapStatic);     /* Static */
        break;
        case L7_FDB_ADDR_FLAG_LEARNED:
        case L7_FDB_ADDR_FLAG_DOT1X_STATIC:
        osapiSnprintf(stat, sizeof(stat), "%-13.13s ", pStrInfo_base_Learned);     /* Learned */
        break;
      case L7_FDB_ADDR_FLAG_SELF:
        osapiSnprintf(stat, sizeof(stat), "%-13.13s ", pStrInfo_base_Self);     /* Self */
        break;
      case L7_FDB_ADDR_FLAG_MANAGEMENT:
      case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
        osapiSnprintf(stat, sizeof(stat), "%-13.13s ", pStrInfo_base_Mgmt);     /* Management*/
        break;
      case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
        osapiSnprintf(stat, sizeof(stat), "%-13.13s ", pStrInfo_base_GmrpLearned);     /* GMRP Learned */
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), "%-13.13s ", pStrInfo_common_Other);     /* Other */
        break;
    }
    ewsTelnetWrite(ewsContext, stat);

    if (L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry))
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_base_ShowFwddbTbl);                            /*hard coded in command name for now, #define later :-) */
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */

}

/*********************************************************************
*
* @purpose  display ARP switch setting
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument checking
*
* @cmdsyntax  show arp switch
*
* @cmdhelp Display ARP switch (up to 20 entries).
*
* @cmddescript
*   This is used to check connectivity between the switch and other devices.
*   The ARP cache identifies the MAC addresses of the IP stations communicating with the switch.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowArpSwitch(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 count, s, p;
   static L7_ulong32 ipAddr;
   L7_uchar8 mac[L7_MAC_ADDR_LEN];
   L7_uchar8 nullMac[L7_MAC_ADDR_LEN];
   static L7_long32 arpIndex;
   L7_uint32 intIfNum;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowArpSwitch_1);
  }

  else
  {
    memset (nullMac, 0,sizeof(nullMac));
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {                                                                       /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
         {
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliPrompt(ewsContext);
         }
      }
      else
      {
         arpIndex = 0;
         ipAddr = 0;
      }
   }

   /* printing header info */
  ewsTelnetWriteAddBlanks (1, 0, 4, 2, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIpAddrIntf);
   ewsTelnetWrite(ewsContext,"\r\n------------------- ---------------- ------------");

   for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
   {

      if (usmDbArpInfoGetNext(unit, &arpIndex, &ipAddr, mac) != L7_SUCCESS)
      {
      return cliSyntaxReturnPrompt (ewsContext, "");
      }

      osapiSnprintf(buf, sizeof(buf), "\r\n%02X:%02X:%02X:%02X:%02X:%02X",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      ewsTelnetPrintf (ewsContext, "%-21s ",buf);

      usmDbInetNtoa(ipAddr, buf);
    ewsTelnetPrintf (ewsContext, "%-17s",buf);

      if (L7_SUCCESS == usmDbArpEntryGet(unit, mac, &intIfNum))
      {
         usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p);
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit,s,p));
      ewsTelnetPrintf (ewsContext, "%-11.11s", buf);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_base_Mgmt);
    }
  }
  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_base_ShowArpSwitch_2);
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */

}


/*********************************************************************
*
* @purpose   display the forwarding database address aging timeout.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowForwardingDbAgetime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 count, val;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 argFdbId = 1;
   static L7_uint32 fdbId;
   L7_BOOL all = L7_TRUE;
   L7_FDB_TYPE_t fdb_type = L7_SVL;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */
   L7_uint32 vlanFeature = L7_FALSE;
   L7_uint32 vlanID=1;

   cliSyntaxTop(ewsContext);

   cliCmdScrollSet( L7_FALSE);
   numArg = cliNumFunctionArgsGet();

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   usmDbFDBTypeOfVLGet(unit, &fdb_type);
   /* Check to see if VLAN Feature is supported */
   vlanFeature = usmDbFeaturePresentCheck(unit,
                                          L7_FDB_COMPONENT_ID, L7_FDB_AGETIME_PER_VLAN_FEATURE_ID);

   if (fdb_type == L7_IVL && numArg > 1 && vlanFeature == L7_TRUE)
   {
     return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowFdbAgeTimeIvl, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
   }
   else if ((vlanFeature != L7_TRUE || fdb_type != L7_IVL) && numArg > 0)
   {
     return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowFdbAgeTimeSvl);
   }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {  /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (numArg == 1)
    {
      if (strcmp(argv[index+argFdbId], pStrInfo_common_All) != 0)
      {
        all = L7_FALSE;
        if (( cliConvertTo32BitUnsignedInteger(argv[index+argFdbId], &fdbId) != L7_SUCCESS ) ||
            ( fdbId < L7_DOT1Q_MIN_VLAN_ID ) ||
            ( fdbId > L7_DOT1Q_MAX_VLAN_ID ))
        {
          return cliSyntaxReturnPromptAddBlanks (0, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                  pStrInfo_base_IncorrectInputVlanIdMustBeInRangeOfTo, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );
        }
        /* get the first entry after the zero parameter */
        if (L7_SUCCESS != usmDbFdbIdGet(unit, fdbId))
        {
          return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FwdDbaseId);
        }
      }
    }
    if (all == L7_TRUE)
    {
      fdbId = 0;
      /* get the first entry after the zero parameter */
      if (L7_SUCCESS != usmDbNextVlanGet(unit, fdbId, &fdbId))
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_FwdDataBaseEmpty);
         }
      }
   }

   if (fdb_type == L7_SVL || vlanFeature != L7_TRUE)
   {
    ewsTelnetPrintf (ewsContext, "\r\n%-20s:", pStrInfo_base_AddrAgingTimeout);
    memset (buf, 0, sizeof(buf));
      vlanID = FD_SIM_DEFAULT_MGMT_VLAN_ID;

      if (fdb_type == L7_SVL)
    {
      usmDbDot1dTpAgingTimeGet(unit, &val);
    }
    else
    {
         usmDbFDBAddressAgingTimeoutGet(unit, vlanID, &val);
    }

    return cliSyntaxReturnPrompt (ewsContext,"%-15u", val);
  }

  /* IVL Case */
  ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext,pStrInfo_base_FwddbIdAddrAgingTimeoutInSeconds);
  ewsTelnetWrite(ewsContext,"\r\n-----------------  ----------------------------------");

  for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    osapiSnprintf(buf, sizeof(buf), "\r\n%d",fdbId);
    ewsTelnetPrintf (ewsContext, "%-21.21s", buf);

    usmDbFDBAddressAgingTimeoutGet(unit, fdbId, &val);
    osapiSnprintf(buf, sizeof(buf), "%d", val);
    ewsTelnetPrintf (ewsContext, "%-10.10s ", buf);

    if ((all == L7_FALSE) || (L7_SUCCESS != usmDbNextVlanGet(unit, fdbId, &fdbId)))
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  cliSyntaxBottom(ewsContext);
  /*hard coded in command name for now, #define later */
  cliAlternateCommandSet(pStrInfo_base_ShowFwddbAgetimeAll);
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */

}

/*********************************************************************
*
* @purpose  display network configuration information such as IP address, subnet mask, gateway, etc
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument checking
*
* @cmdsyntax  show network
*
* @cmdhelp Display configuration for in-band connectivity.
*
* @cmddescript shows the system information for your switch.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowNetwork(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
   L7_RC_t rc;
   L7_uint32 val, unit;
   L7_uint32 numArg;        /* New variable Added */
   L7_BOOL isUp;
   L7_uchar8 npName[L7_CLI_MAX_STRING_LENGTH];
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_uint32 idx,acount,adminMode;
  L7_in6_addr_t ipv6Addrs[L7_RTR6_MAX_INTF_ADDRS*2];
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS*2];
  L7_uchar8 strIpv6Addr[L7_CLI_MAX_STRING_LENGTH];
#endif

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowNw_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

   cliFormat(ewsContext, "Interface Status");
   memset (stat, 0, sizeof(stat));
   osapiSnprintf(npName, L7_DTL_PORT_IF_LEN + 1, "%s%d", L7_DTL_PORT_IF, 0);
   if ((osapiInterfaceIsUp(npName, &isUp) == L7_SUCCESS) && isUp)
   {
     osapiSnprintf(stat, sizeof(stat), "%s", "Up");
   }
   else
   {
     osapiSnprintf(stat, sizeof(stat), "%s", "Down");
   }
   ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext,pStrInfo_common_ApIpAddr);
  memset (stat, 0, sizeof(stat));
   rc = usmDbAgentIpIfAddressGet(unit, &val);
   rc = usmDbInetNtoa(val, stat);
   ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext,pStrInfo_common_SubnetMask_3);
  memset (stat, 0, sizeof(stat));
   rc = usmDbAgentIpIfNetMaskGet(unit, &val);
   rc = usmDbInetNtoa(val, stat);
   ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext,pStrInfo_base_DeflGateway);
  memset (stat, 0, sizeof(stat));
   rc = usmDbAgentIpIfDefaultRouterGet(unit, &val);
   rc = usmDbInetNtoa(val, stat);
   ewsTelnetWrite(ewsContext, stat);

  /* ipv6 addresses */
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)

  adminMode = usmDbAgentIpIfIPV6AdminModeGet();
  cliFormat(ewsContext,pStrInfo_base_Ipv6AdminMode);
  ewsTelnetPrintf (ewsContext, (adminMode==L7_ENABLE ? pStrInfo_common_Enbld : pStrInfo_common_Dsbld));

  acount = L7_RTR6_MAX_INTF_ADDRS*2;    /* allow for both static and dynamic addresses */
  if (usmDbAgentIpIfIPV6AddrsGet( ipv6Prefixes, &acount) == L7_SUCCESS)
  {
    for (idx = 0; idx < acount; idx++)
    {
      cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Ipv6Addres);
      memset (stat, 0, sizeof(stat));
      osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ipv6Prefixes[idx].in6Addr, strIpv6Addr, L7_CLI_MAX_STRING_LENGTH);
      ewsTelnetPrintf (ewsContext, "%s/%d",strIpv6Addr,ipv6Prefixes[idx].in6PrefixLen);
    }
  }

  /* ipv6 default routers */
  acount = L7_RTR6_MAX_INTF_ADDRS*2;     /* allow for both static and dynamic addresses */
  if (usmDbAgentIpIfIPV6DefaultRoutersGet( ipv6Addrs, &acount) == L7_SUCCESS)
  {
    for (idx = 0; idx < acount; idx++)
    {
      cliFormat(ewsContext,pStrInfo_base_Ipv6DeflRtr);
      memset (stat, 0, sizeof(stat));
      osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ipv6Addrs[idx], strIpv6Addr, L7_CLI_MAX_STRING_LENGTH);
      ewsTelnetPrintf (ewsContext, strIpv6Addr);
    }
  }
#endif /* L7_IPV6_PACKAGE || L7_IPV6_MGMT_PACKAGE */

  cliFormat(ewsContext,pStrInfo_base_BurnedInMacAddr);
  memset (stat, 0,sizeof(stat));
   val = sizeof(mac);
   rc = usmDbSwDevCtrlBurnedInMacAddrGet(unit,mac);
  ewsTelnetPrintf (ewsContext, "%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

  if(usmDbFeaturePresentCheck(unit, L7_SIM_COMPONENT_ID, L7_SIM_ALLOW_LOCAL_MAC_USAGE_FEATURE_ID) == L7_TRUE)
  { 
    cliFormat(ewsContext,pStrInfo_common_LocallyAdministeredMacAddr);
    memset (stat, 0,sizeof(stat));
    val = sizeof(mac);
    rc = usmDbSwDevCtrlLocalAdminAddrGet(unit, mac);
    ewsTelnetPrintf (ewsContext, "%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  }

  cliFormat(ewsContext,pStrInfo_base_MacAddrType);
   rc = usmDbSwDevCtrlMacAddrTypeGet(unit,&val);
   switch (val)                                                            /* val = Invalid  ",Burned-in", "Local */
   {
   case L7_SYSMAC_BIA:
      ewsTelnetWrite(ewsContext,pStrInfo_base_BurnedIn);
      break;
   case L7_SYSMAC_LAA:
      ewsTelnetWrite(ewsContext,pStrInfo_common_WsNwLocal);
      break;
   default:
      ewsTelnetWrite(ewsContext,pStrInfo_common_Invalid_1);
   }

   /* Network Configured IPv4 Protocol Current */ 
  cliFormat(ewsContext,pStrInfo_base_NwCfgIPv4ProtoCurrent);
   rc = usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(unit,&val);
   switch (val)                                                            /* val = Invalid, "None ","BootP", "DHCP", "-----" */
   {
   case L7_SYSCONFIG_MODE_NONE:
      ewsTelnetWrite(ewsContext,pStrInfo_common_None_1);
      break;
   case L7_SYSCONFIG_MODE_BOOTP:
      ewsTelnetWrite(ewsContext,pStrInfo_base_Bootp);
      break;
   case L7_SYSCONFIG_MODE_DHCP:
      ewsTelnetWrite(ewsContext,pStrInfo_base_Dhcp);
      break;
   default:
      ewsTelnetWrite(ewsContext,pStrInfo_common_Invalid_1);
   }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
   /* Network Configured IPv6 Protocol Current */ 
   cliFormat(ewsContext,pStrInfo_base_NwCfgIPv6ProtoCurrent);  
   val = L7_SYSCONFIG_MODE_NONE;
   rc = usmDbAgentBasicConfigNetworkIPv6ConfigProtocolGet(unit,&val);
    switch (val)                                                            /* val =  "None ", "DHCP", Invalid */
   {
   case L7_SYSCONFIG_MODE_NONE:
      ewsTelnetWrite(ewsContext,pStrInfo_common_None_1);
      break;
   case L7_SYSCONFIG_MODE_DHCP:
      ewsTelnetWrite(ewsContext,pStrInfo_base_Dhcp);
      break;
   default:
      ewsTelnetWrite(ewsContext,pStrInfo_common_Invalid_1);
   }
   if (val == L7_SYSCONFIG_MODE_DHCP)
   {
     /* DHCPv6 Client DUID */
     memset (stat, 0, sizeof(stat));
     cliFormat(ewsContext,pStrInfo_base_Dhcpv6ClientDUID);  
     (void)usmDbDhcp6cDuidGet(stat);
     ewsTelnetWrite(ewsContext, stat);   
          
   }
   /* IPv6 Stateless Address AutoConfig Mode */
   val = L7_DISABLE;
   cliFormat(ewsContext,pStrInfo_base_NwCfgIPv6AutoCfgMode);
   rc = usmDbNetworkIPv6AddrAutoConfigGet(&val);
   ewsTelnetPrintf (ewsContext, (val==L7_ENABLE ? pStrInfo_common_Enbld : pStrInfo_common_Dsbld));

#endif /* defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE) */
  cliFormat(ewsContext, pStrInfo_base_MgmtVlanId);
  memset (stat, 0,sizeof(stat));
   rc = usmDbMgmtVlanIdGet(1,&val);
   if (rc == L7_SUCCESS)
   {
    osapiSnprintf(stat,sizeof(stat),"%d",val);
   }
  return cliSyntaxReturnPrompt (ewsContext,stat);
}

/*********************************************************************
*
* @purpose Displays Arp table information
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
* @cmdsyntax    show ip http
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpHttp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{

  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg = cliNumFunctionArgsGet();

   if ( numArg != 0 )
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowIpHttp_1);
  }

  cliFormat(ewsContext, pStrInfo_base_HttpModeUnsecure);
  if (usmDbSwDevCtrlWebMgmtModeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),"%-8s",strUtilEnabledDisabledGet(val ,pStrInfo_common_Line));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-8s",pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,buf);

  cliFormat(ewsContext, pStrInfo_base_JavaMode_1); 
  if (usmDbWebJavaModeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),"%-8s",strUtilEnabledDisabledGet(val ,pStrInfo_common_Line));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-8s",pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,buf);

  cliFormat(ewsContext, pStrInfo_base_MaxAllowableHttpSessions); 
  if (usmDbCliWebHttpNumSessionsGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-5d", val);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-8s",pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, buf);

  cliFormat(ewsContext, pStrInfo_base_HttpSessionHardTimeout_1); 
  if (usmDbCliWebHttpSessionHardTimeOutGet(&val) == L7_SUCCESS)
  {
    if (val > 0)
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Hours, val);
    }
    else
    {
      osapiStrncpySafe(buf, pStrInfo_common_0Infinite, sizeof(buf));
    }
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-8s",pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, buf);

  cliFormat(ewsContext, pStrInfo_base_HttpSessionSoftTimeout_1);
    if (usmDbCliWebHttpSessionSoftTimeOutGet(&val) == L7_SUCCESS)
    {
      if (val > 0)
      {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Minutes, val);
      }
      else
      {
      osapiStrncpySafe(buf, pStrInfo_common_0Infinite, sizeof(buf));
      }
    }
    else
    {
    osapiSnprintf(buf, sizeof(buf), "%-8s",pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, buf);

#ifdef L7_MGMT_SECURITY_PACKAGE
    cliShowIpHttpSecure(ewsContext);
#endif

  return cliSyntaxReturnPrompt (ewsContext, "");

 }

/*********************************************************************
*
* @purpose  display serial settings such as inactivity timeout and baudrate
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument checking
*
* @cmdsyntax  show serial
*
* @cmdhelp Display EIA-232 parameters and serial port inactivity timeout.
*
* @cmddescript
*
* @comments   L7_FAILURE ifget parm is not supported or recognized
* @comments   L7_NOT_SUPPORTED ifserial port parameter is not supported.
*
* @comments     The parameters for the usmdb f(x) specifies which serial port
* @comments     parameter to retrieve.  The are:
* @comments     SYSAPISERIALPORT_CHARSIZE   the number of bits in the character min
* @comments     SYSAPISERIALPORT_FLOWCONTROL
* @comments     SYSAPISERIALPORT_PARITYTYPE  L7_PARITY_EVEN or L7_PARITY_ODD
* @comments     SYSAPISERIALPORT_PARITY    * number of parity bits (0 or 1) L7_ENABLE or L7_DISABLE
* @comments     SYSAPISERIALPORT_STOPBITS  * number of stop bits (1 or 2).
* @comments  L7_FAILURE if value to set is in error
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowSerial(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 val;
   L7_uint32 intCharSize;
   L7_uint32 intFlowControl;
   L7_uint32 intStopBits;
   L7_uint32 intParityType;
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowSerial_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext,pStrInfo_base_SerialPortLoginTimeoutMinutes);
  memset (stat, 0, sizeof(stat));
   rc = usmDbSerialTimeOutGet(unit, &val);
  ewsTelnetPrintf (ewsContext, "%d",val);

  cliFormat(ewsContext,pStrInfo_base_BaudRateBps);
   rc = usmDbAgentBasicConfigSerialBaudRateGet(unit, &val);
   switch (val)                                                            /* val = "------", 1200","2400,4800,9600,19200,38400,57600,115200 */
   {
   case L7_BAUDRATE_1200:
      ewsTelnetWrite(ewsContext, pStrInfo_base_SerialBaudRate1200);
      break;
   case L7_BAUDRATE_2400:
      ewsTelnetWrite(ewsContext, pStrInfo_base_SerialBaudRate2400);
      break;
   case L7_BAUDRATE_4800:
      ewsTelnetWrite(ewsContext, pStrInfo_base_SerialBaudRate4800);
      break;
   case L7_BAUDRATE_9600:
      ewsTelnetWrite(ewsContext, pStrInfo_base_SerialBaudRate9600);
      break;
   case L7_BAUDRATE_19200:
      ewsTelnetWrite(ewsContext, pStrInfo_base_SerialBaudRate19200);
      break;
   case L7_BAUDRATE_38400:
      ewsTelnetWrite(ewsContext, pStrInfo_base_SerialBaudRate38400);
      break;
   case L7_BAUDRATE_57600:
      ewsTelnetWrite(ewsContext, pStrInfo_base_SerialBaudRate57600);
      break;
   case L7_BAUDRATE_115200:
      ewsTelnetWrite(ewsContext, pStrInfo_base_SerialBaudRate115200);
      break;
   default:
      ewsTelnetWrite(ewsContext, pStrInfo_common_Blank);
   }

  cliFormat(ewsContext, pStrInfo_base_CharSizeBits);
   rc = usmDbSerialPortParmGet(unit, SYSAPISERIALPORT_CHARSIZE, &intCharSize);
  ewsTelnetPrintf (ewsContext, "%d",intCharSize);

  cliFormat(ewsContext,pStrInfo_base_FlowCntrl);
   rc = usmDbSerialPortParmGet(unit, SYSAPISERIALPORT_FLOWCONTROL, &intFlowControl);
  ewsTelnetPrintf(ewsContext, "%s", strUtilEnableDisableGet(intFlowControl, pStrInfo_common_Blank));

  cliFormat(ewsContext,pStrInfo_base_StopBits);
   rc = usmDbSerialPortParmGet(unit, SYSAPISERIALPORT_STOPBITS, &intStopBits);
  ewsTelnetPrintf (ewsContext, "%d",intStopBits);

  cliFormat(ewsContext,pStrInfo_base_Parity);
   rc = usmDbSerialPortParmGet(unit, SYSAPISERIALPORT_PARITYTYPE, &intParityType);
   switch (intParityType)                                                  /* even, odd, ------ */
   {
   case L7_PARITY_EVEN:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_base_Even);
      break;
   case L7_PARITY_ODD:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_base_Odd);
      break;
   case L7_PARITY_NONE:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_None_3);
      break;
   default:
      osapiSnprintf(stat,sizeof(stat),"----");
      break;
   }
  return cliSyntaxReturnPrompt (ewsContext,stat);

}

/*********************************************************************
*
* @purpose  display service port configuration information: IP address, subnet mask, gateway
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument checking
*
* @cmdsyntax  show servicePort
*
* @cmdhelp Display configuration for service port.
*
* @cmddescript shows the information for your switch.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowServicePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 macAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_RC_t rc;
   L7_uint32 val;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */
   L7_BOOL isUp;
   L7_uchar8 spName[L7_CLI_MAX_STRING_LENGTH];
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_uint32 idx,acount,adminMode;
  L7_in6_addr_t ipv6Addrs[L7_RTR6_MAX_INTF_ADDRS*2];
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS*2];
   L7_uchar8 strIpv6Addr[L7_CLI_MAX_STRING_LENGTH];
#endif

   /* examine the arguments */
   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowServiceport_1);
  }

   cliFormat(ewsContext, "Interface Status");
   memset (stat, 0, sizeof(stat));
   osapiSnprintf(spName, L7_SERVICE_PORT_MAX_LENGTH, "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
   if ((osapiInterfaceIsUp(spName, &isUp) == L7_SUCCESS) && isUp)
   {
     osapiSnprintf(stat, sizeof(stat), "%s", "Up");
   }
   else
   {
     osapiSnprintf(stat, sizeof(stat), "%s", "Down");
   }
   ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext,pStrInfo_common_ApIpAddr);
  memset (stat, 0, sizeof(stat));
   rc = usmDbServicePortIPAddrGet(unit, &val);
   rc = usmDbInetNtoa(val, stat);
   ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext,pStrInfo_common_SubnetMask_3);
  memset (stat, 0, sizeof(stat));
   rc = usmDbServicePortNetMaskGet(unit, &val);
   rc = usmDbInetNtoa(val, stat);
   ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext,pStrInfo_base_DeflGateway);
  memset (stat, 0, sizeof(stat));
   rc = usmDbServicePortGatewayGet(unit, &val);
   rc = usmDbInetNtoa(val, stat);
   ewsTelnetWrite(ewsContext, stat);

   /* ipv6 addresses */
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)

  adminMode = usmDbServicePortIPV6AdminModeGet();
  cliFormat(ewsContext,pStrInfo_base_Ipv6AdminMode);
  ewsTelnetPrintf (ewsContext, (adminMode==L7_ENABLE ? pStrInfo_common_Enbld : pStrInfo_common_Dsbld));

  acount = L7_RTR6_MAX_INTF_ADDRS*2;     /* allow for both static and dynamic addresses */
   if (usmDbServicePortIPV6AddrsGet( ipv6Prefixes, &acount) == L7_SUCCESS)
   {
      for (idx = 0; idx < acount; idx++)
      {
      cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Ipv6Addres);
      memset (stat, 0, sizeof(stat));
         osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ipv6Prefixes[idx].in6Addr, strIpv6Addr, L7_CLI_MAX_STRING_LENGTH);
      ewsTelnetPrintf (ewsContext, "%s/%d",strIpv6Addr,ipv6Prefixes[idx].in6PrefixLen);
    }
  }

  /* ipv6 default routers */
  acount = L7_RTR6_MAX_INTF_ADDRS*2;      /* allow for both static and dynamic addresses */
   if (usmDbServicePortIPV6DefaultRoutersGet( ipv6Addrs, &acount) == L7_SUCCESS)
   {
      for (idx = 0; idx < acount; idx++)
      {
      cliFormat(ewsContext,pStrInfo_base_Ipv6DeflRtr);
      memset (stat, 0, sizeof(stat));
         osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&ipv6Addrs[idx], strIpv6Addr, L7_CLI_MAX_STRING_LENGTH);
      ewsTelnetPrintf (ewsContext, strIpv6Addr);
    }
  }
#endif /* L7_IPV6_PACKAGE || L7_IPV6_MGMT_PACKAGE */

  cliFormat(ewsContext,pStrInfo_base_ServPortCfgIPv4ProtoCurrent);
   rc = usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(unit,&val);
   switch (val)                                                            /* val = Invalid, "None ","BootP", "DHCP", "-----" */
   {
   case L7_SYSCONFIG_MODE_BOOTP:
      ewsTelnetWrite(ewsContext,pStrInfo_base_Bootp);
      break;
   case L7_SYSCONFIG_MODE_DHCP:
      ewsTelnetWrite(ewsContext,pStrInfo_base_Dhcp);
      break;
   default:
      ewsTelnetWrite(ewsContext,pStrInfo_common_None_1);
   }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
   /* Network Configured IPv6 Protocol Current */
   cliFormat(ewsContext,pStrInfo_base_ServPortCfgIPv6ProtoCurrent);
   val = L7_SYSCONFIG_MODE_NONE;
   rc =      usmDbAgentBasicConfigServPortIPv6ConfigProtocolGet(unit,&val);
    switch (val)                                                            /* val =  "None ", "DHCP", Invalid */
   {
   case L7_SYSCONFIG_MODE_NONE:
      ewsTelnetWrite(ewsContext,pStrInfo_common_None_1);
      break;
   case L7_SYSCONFIG_MODE_DHCP:
      ewsTelnetWrite(ewsContext,pStrInfo_base_Dhcp);
      break;
   default:
      ewsTelnetWrite(ewsContext,pStrInfo_common_Invalid_1);
   }
   if (val == L7_SYSCONFIG_MODE_DHCP)
   {
     /* DHCPv6 Client DUID */
     memset (stat, 0, sizeof(stat));
     cliFormat(ewsContext,pStrInfo_base_Dhcpv6ClientDUID);
     (void)usmDbDhcp6cDuidGet(stat);
     ewsTelnetWrite(ewsContext, stat);

   }
   /* IPv6 Stateless Address AutoConfig Mode */
   val = L7_DISABLE;
   cliFormat(ewsContext,pStrInfo_base_NwCfgIPv6AutoCfgMode);
   rc = usmDbServPortIPv6AddrAutoConfigGet(&val);
   ewsTelnetPrintf (ewsContext, (val==L7_ENABLE ? pStrInfo_common_Enbld : pStrInfo_common_Dsbld));

#endif /* defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE) */
   
  memset (macAddr, 0, sizeof(macAddr));
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext,pStrInfo_base_BurnedInMacAddr);
   rc = usmDbServPortMacAddressGet(unit, macAddr);
  return cliSyntaxReturnPrompt (ewsContext,"%02X:%02X:%02X:%02X:%02X:%02X", macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);

}

/*********************************************************************
*
* @purpose  display snmp community information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument checking, needs spacing work
*
* @cmdsyntax  show snmpcommunity
*
* @cmdhelp Display SNMP community entries.
*
* @cmddescript
*   The switch has an SNMP agent that complies with SNMP Version 1 (SNMPv1).
*   For more about the SNMP specification, see the appropriate SNMP RFCs. The SNMP agent
*   sends traps through TCP/IP to an external SNMP manager based on your SNMP configuration.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowSnmp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH + 1];
   L7_uchar8 ip[16];
   L7_uint32 val;
   L7_RC_t rc;
   L7_int32 i;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowSnmpCommunity);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_base_SnmpCommunityNameClientIpAddrClientIpMaskAccessModeStatus);
   ewsTelnetWrite(ewsContext,"\r\n------------------- ----------------- ----------------- ----------- -------- ");

  for (i=0; i<L7_MAX_SNMP_COMM; i++)
   {
    memset (stat, 0,sizeof(stat));
    memset (buf, 0,sizeof(buf));
      rc = usmDbSnmpCommunityNameGet(unit, i, buf);
    if (strcmp(buf, pStrInfo_common_EmptyString ) != 0)
      {
      ewsTelnetPrintf (ewsContext, "\r\n%-19s ", buf);

      memset (stat, 0,sizeof(stat));
      memset (ip, 0,sizeof(ip));
         val = 0;
         rc = usmDbSnmpCommunityIpAddrGet(unit, i, &val);
         rc = usmDbInetNtoa(val, ip);
      ewsTelnetPrintf (ewsContext, "%-17s ",ip);

      memset (stat, 0,sizeof(stat));
      memset (ip, 0,sizeof(ip));
         rc = usmDbSnmpCommunityIpMaskGet(unit, i, &val);
         rc = usmDbInetNtoa(val, ip);
      ewsTelnetPrintf (ewsContext, "%-17s ",ip);

      memset (stat, 0,sizeof(stat));
         rc = usmDbSnmpCommunityAccessLevelGet(unit, i, &val);
         switch (val)                                                          /* note invalid, read only, read/write  val =? */
         {
         case L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY:
          osapiSnprintf(stat, sizeof(stat), "%-11s ", pStrInfo_base_ReadOnly);
            break;
         case L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE:
          osapiSnprintf(stat, sizeof(stat), "%-11s ", pStrInfo_base_ReadWrite);
            break;
         default:
          osapiSnprintf(stat, sizeof(stat), "%-11s ", pStrInfo_base_ReadOnly);
         }
         ewsTelnetWrite(ewsContext,stat);

      memset (stat, 0,sizeof(stat));
         rc = usmDbSnmpCommunityStatusGet(unit, i, &val);
         switch (val)                                                          /* val = "Invalid","Enable","Disable","Delete" */
         {
         case L7_SNMP_COMMUNITY_STATUS_VALID:
          osapiSnprintf(stat,sizeof(stat),"%-8s",pStrInfo_common_Enbl_1);
            break;
         case L7_SNMP_COMMUNITY_STATUS_INVALID:
          osapiSnprintf(stat,sizeof(stat),"%-8s",pStrInfo_common_Dsbl_1);
            break;
         case L7_SNMP_COMMUNITY_STATUS_DELETE:
          osapiSnprintf(stat,sizeof(stat),"%-8s",pStrInfo_common_Del);
            break;
         case L7_SNMP_COMMUNITY_STATUS_CONFIG:
          osapiSnprintf(stat,sizeof(stat),"%-8s",pStrInfo_base_Cfg);
            break;
         default:
          osapiSnprintf(stat,sizeof(stat),"%-8s",pStrInfo_common_Invalid_1);
         }
         ewsTelnetWrite(ewsContext,stat);

      }
   }

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose  display SNMP trap receiver entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument checking
*
* @cmdsyntax  show snmptrap
*
* @cmdhelp Display SNMP trap receiver entries.
*
* @cmddescript
*   Trap messages are sent across a network to an SNMP Network Manager.
*   These messages alert the manager to events occurring within the switch
*   or on the network. Up to six simultaneous trap receivers are supported.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowSnmptrap(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH +1];
   L7_RC_t rc;
   L7_uint32 val;
   L7_char8 ip[16];
   L7_int32 i;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */
   snmpTrapVer_t trap;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_in6_addr_t ipv6_address;
  L7_char8 buf1[L7_CLI_MAX_STRING_LENGTH ];
  L7_BOOL ipv6AddrGreater = L7_FALSE;
  memset(&ipv6_address, 0, sizeof(L7_in6_addr_t));
#endif
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowSnmpTrap_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  ewsTelnetWriteAddBlanks (1, 0, 2, 2, L7_NULLPTR, ewsContext,pStrInfo_base_SnmpTrapNameIpAddrIpv6AddrSnmpVerStatus);
  ewsTelnetWrite(ewsContext,"\r\n------------------- --------------- -------------------- ------------- ---------");
#else
  ewsTelnetWriteAddBlanks (1, 0, 2, 2, L7_NULLPTR, ewsContext,pStrInfo_base_SnmpTrapNameIpAddrSnmpVerStatus);
  ewsTelnetWrite(ewsContext,"\r\n------------------- --------------- -------------- ---------");
#endif

  for (i=0; i<6; i++)
  {
    memset (stat, 0,sizeof(stat));
    memset (buf, 0,sizeof(buf));
      rc = usmDbTrapManagerCommIpGet(unit, i, buf);
    if ((strcmp(buf, pStrInfo_common_EmptyString ) != 0) && (rc == L7_SUCCESS))
      {
      ewsTelnetPrintf (ewsContext, "\r\n%-19s ", buf);

      memset (stat, 0,sizeof(stat));
      memset (ip, 0,sizeof(ip));
         val = 0;
         rc = usmDbTrapManagerIpAddrGet(unit, i, &val);
         rc = usmDbInetNtoa(val, ip);
      ewsTelnetPrintf (ewsContext, "%-15s ",ip);


#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
      memset (stat, 0,sizeof(stat));
      memset (buf1, 0,sizeof(buf1));
      rc = usmDbTrapManagerIPv6AddrGet(unit, i, &ipv6_address);
      if(rc == L7_SUCCESS)
      {
        if(memcmp(&ipv6_address, " ", sizeof(L7_in6_addr_t)) != 0)
        {
          osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, buf1, sizeof(buf1));
          if(strcmp(buf1, "::") ==0)
          {
            ewsTelnetPrintf (ewsContext, "%-20s ",pStrInfo_common_DoubleColon);
          }
          else
          {
            if ( strlen(buf1) <=  20 )
            {
              ewsTelnetPrintf (ewsContext, "%-20s ",buf1);
            }
            else
            {
              osapiStrncpySafe(stat, buf1, 21);
              ewsTelnetPrintf (ewsContext, "%-20s ",stat);
              ipv6AddrGreater = L7_TRUE;
            }
          }
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat), "%-20s ", pStrInfo_common_Space);
          ewsTelnetWrite(ewsContext,stat);
        }
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%-20s ", pStrErr_common_Err);
        ewsTelnetWrite(ewsContext,stat);
      }
#endif

      memset (stat, 0,sizeof(stat));
      memset (buf, 0,sizeof(buf));
         rc = usmDbTrapManagerVersionGet(unit, i, &trap);
         if (trap==L7_SNMP_TRAP_VER_SNMPV1)
      {
        osapiSnprintf(buf,sizeof(buf),pStrInfo_base_SnmpVer1);
      }
         else if (trap==L7_SNMP_TRAP_VER_SNMPV2C)
      {
        osapiSnprintf(buf,sizeof(buf),pStrInfo_base_SnmpVer2);
      }
      ewsTelnetPrintf (ewsContext, "%-13s ",buf);

      memset (stat, 0,sizeof(stat));
         rc = usmDbTrapManagerStatusIpGet(unit, i, &val);
         switch (val)                                                          /* val = Invalid ,Disable Enable Delete */
         {
         case L7_SNMP_TRAP_MGR_STATUS_INVALID:
          osapiSnprintf(stat,sizeof(stat), "%-8s",pStrInfo_common_Dsbl_1);
            break;
         case L7_SNMP_TRAP_MGR_STATUS_VALID:
          osapiSnprintf(stat,sizeof(stat), "%-8s",pStrInfo_common_Enbl_1);
            break;
         case L7_SNMP_TRAP_MGR_STATUS_DELETE:
          osapiSnprintf(stat,sizeof(stat), "%-8s",pStrInfo_common_Del);
            break;
         case L7_SNMP_TRAP_MGR_STATUS_CONFIG:
          osapiSnprintf(stat,sizeof(stat), "%-8s",pStrInfo_base_Cfg);
            break;
         default:
          osapiSnprintf(stat,sizeof(stat), "%-8s",pStrInfo_common_Invalid_1);
         }
         ewsTelnetWrite(ewsContext,stat);

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
         memset (stat, 0,sizeof(stat));
         if (ipv6AddrGreater == L7_TRUE)
         {
           osapiStrncpySafe(stat, &(buf1[20]),sizeof(stat));
           ewsTelnetPrintf (ewsContext, "\r\n%-35s ",pStrInfo_common_Space);
           ewsTelnetPrintf (ewsContext, "%-20s ",stat);
           ipv6AddrGreater = L7_FALSE;
         }
#endif

      }
   }

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose    Display the traps information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes    This function includes the trap flags display for all the
*           packages. Based on the selected packages, you will view
*           the corresponding status of trap flags.
*
*
* @cmdsyntax
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 val;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowTrapFlags_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext,pStrInfo_base_AuthFlag);
   rc = usmDbTrapAuthSwGet(unit, &val);
  ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));

  cliFormat(ewsContext,pStrInfo_base_LinkUpDownFlag);
   rc = usmDbTrapLinkStatusSwGet(unit, &val);
  ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));

  cliFormat(ewsContext,pStrInfo_base_MultipleUsrsFlag);
   rc = usmDbTrapMultiUsersSwGet(unit, &val);
  ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));

  cliFormat(ewsContext,pStrInfo_base_SpanTreeFlag);
   rc = usmDbTrapSpanningTreeSwGet(unit, &val);
  ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));

#ifdef L7_QOS_FLEX_PACKAGE_ACL
   commandShowAclTrapflags(ewsContext);
#endif

#ifdef L7_BGP_PACKAGE
   commandShowBGPTrapflags(ewsContext);
#endif

#ifdef L7_MCAST_PACKAGE
   commandShowDvmrpTrapflags(ewsContext);
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_OSPF_PACKAGE
   commandShowOspfTrapflags(ewsContext);
#endif
#endif

#ifdef L7_MCAST_PACKAGE
   commandShowPimTrapflags(ewsContext);
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
   commandShowIpv6OspfTrapflags(ewsContext);
#endif
#endif

#ifdef L7_WIRELESS_PACKAGE
  cliFormat(ewsContext, pStrInfo_base_WsTraps);
  if (usmDbTrapWirelessModeGet(&val) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val, pStrInfo_common_Line));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
  }
#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
   cliFormat(ewsContext, "Captive Portal Flag");
   if (usmDbTrapCaptivePortalGet(&val) == L7_SUCCESS)
   {
     ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val, pStrInfo_common_Line));
   }
   else
   {
     ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
   }
#endif

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display telnet config info such as max sessions and login timeout
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add argument checking
*
* @cmdsyntax  show telnetcon
*
* @cmdhelp Display Telnet configuration information.
*
* @cmddescript
*   Show remote connection settings such as login timeout, the max number of remote
*   sessions, and if new telnet sessions are allowed or not.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowTelnetCon(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 val;
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowTelnet_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext,pStrInfo_base_TelnetLoginTimeoutMinutes);
  memset (stat, 0,sizeof(stat));
   rc = usmDbAgentTelnetTimeoutGet(unit, &val);
  ewsTelnetPrintf (ewsContext, "%d",val);

  cliFormat(ewsContext,pStrInfo_base_MaxNumOfTelnetSessions);
  memset (stat, 0,sizeof(stat));
   rc = usmDbAgentTelnetNumSessionsGet(unit, &val);
  ewsTelnetPrintf (ewsContext, "%d",val);

  cliFormat(ewsContext,pStrInfo_base_AllowNewTelnetSessions);
  memset (stat, 0,sizeof(stat));
   rc = usmDbAgentTelnetNewSessionsGet(unit, &val);
   switch (val)                                                            /* val = yes no "---" */
   {
   case L7_YES:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_Yes);
      break;
   case L7_NO:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_No);
      break;
   default:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_No);
   }
   ewsTelnetWrite(ewsContext,stat);

  cliFormat(ewsContext,pStrInfo_base_TelnetAdminMode);
  memset (stat, 0,sizeof(stat));
   rc = usmDbAgentTelnetAdminModeGet(unit, &val);
  osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
  return cliSyntaxReturnPrompt (ewsContext,stat);
}

/*********************************************************************
*
* @purpose  display Generic Attribute Registration Protocol parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show garp
*
* @cmdhelp  Display Generic Attribute Registration Protocol parameters.
*
* @cmddescript  Show Generic Attributes Registration Protocol (GARP) configuration parameters.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowGarp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 mode;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowGarp_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext, pStrInfo_base_GmrpAdminMode);
   if (usmDbGarpGmrpEnableGet(unit, &mode) == L7_SUCCESS)
   {
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode,pStrInfo_common_NotApplicable));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
  }

  cliFormat(ewsContext, pStrInfo_base_GvrpAdminMode);
   if (usmDbGarpGvrpEnableGet(unit, &mode) == L7_SUCCESS)
   {
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode,pStrInfo_common_NotApplicable));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose Displays the IGMP Snooping information
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
* @notes     Here we  display all IGMP Snooping information.
*
* @cmdsyntax    show igmpsnooping [<slot/port>|<vlan-id>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIGMPSnooping(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_uint32 val;
   static L7_uint32 intIfNum;
   L7_uint32 mode = L7_DISABLE;
   L7_uint32 vlanMode = L7_DISABLE;
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_BOOL hasInterfaces = L7_FALSE;
   L7_BOOL hasVlans = L7_FALSE;
   L7_uint32 unit;
   L7_uint32 u, s, p;
   static  L7_uint32 nextvid;
   L7_uint32 slot, port, interface;
   L7_uint32 argSlotPort=1;
   L7_uint32 numArg;        /* New variable Added */
   L7_RC_t rc = L7_FAILURE;
   L7_uint32 argVlan;
   L7_uint32 count = 0;
   static L7_BOOL interfaceCompleted = L7_FALSE;
   static L7_BOOL vlanModeEnabled = L7_FALSE;
   static L7_BOOL interfaceEnabled = L7_FALSE;
   L7_BOOL fastleavemode;
   L7_ushort16 groupmembershipinterval;
   L7_ushort16 maxresponsetime;
   L7_ushort16 mcrtrexpirytime;
  static L7_uchar8 family = L7_AF_INET;
  static L7_BOOL firstTime = L7_TRUE;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();

  numArg = cliNumFunctionArgsGet();
  if (firstTime == L7_TRUE)
  {
    if (strcmp(argv[1], pStrInfo_common_Igmpsnooping) == 0)
    {
      family = L7_AF_INET;
    }
    else if (strcmp(argv[1], pStrInfo_common_Mldsnooping) == 0)
    {
      family = L7_AF_INET6;
    }
    else
    {
      family = L7_AF_INET;
    }
    firstTime = L7_FALSE;
  }

  if (numArg > 1)
  {
    ewsTelnetPrintf (ewsContext, CLISYNTAX_SHOWIGMPSNOOPING(family), cliSyntaxInterfaceHelp());
    cliSyntaxBottom(ewsContext);
    firstTime = L7_TRUE;
      return cliPrompt(ewsContext);
   }

   if (numArg == 1)
   {
      if (cliConvertTo32BitUnsignedInteger(argv[index + 1],&argVlan) == L7_SUCCESS)
      {
         /* Vlan ID */
      cliFormat(ewsContext, pStrInfo_base_VlanId_4);
      ewsTelnetPrintf (ewsContext, "%d", argVlan);

         /*  Vlan Admin Mode */
      if (family == L7_AF_INET)
      {
        cliFormat(ewsContext, pStrInfo_base_IgmpSnoopingAdminMode);
      }
      else
      {
        cliFormat(ewsContext, pStrInfo_base_MldSnoopingAdminMode);
      }

      if (usmDbSnoopVlanModeGet(unit, argVlan, &mode, family) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext,strUtilEnabledDisabledGet(mode,pStrInfo_common_Dsbld));
         }

         /* Vlan Fast Leave Mode */
      cliFormat(ewsContext, pStrInfo_base_IgmpFastLeaveAdminMode);
      if (usmDbSnoopVlanFastLeaveModeGet(unit, argVlan, &fastleavemode,family) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,strUtilEnabledDisabledGet(fastleavemode,pStrInfo_common_Dsbld));
         }

         /* Vlan Group Membership Interval */
      cliFormat(ewsContext, pStrInfo_common_GrpMbrshipIntvl);
      if (usmDbSnoopVlanGroupMembershipIntervalGet(unit, argVlan, &groupmembershipinterval, family) == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "%d",groupmembershipinterval);
         }

         /* Vlan Maximum Response Time */
      cliFormat(ewsContext, pStrInfo_common_MaxRespTime);
      if (usmDbSnoopVlanMaximumResponseTimeGet(unit, argVlan, &maxresponsetime, family) == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "%d",maxresponsetime);
         }

         /* Vlan Multicast Router Expiry Time */
      cliFormat(ewsContext, pStrInfo_common_McastRtrExpiryTime);
      if (usmDbSnoopVlanMcastRtrExpiryTimeGet(unit, argVlan, &mcrtrexpirytime, family) == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "%d",mcrtrexpirytime);
      }
    }
    else
    {
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
      {
        firstTime = L7_TRUE;
        return cliPrompt(ewsContext);
         }

         /* code for the interface to be valid for snooping */
      if (usmDbIntIfNumTypeCheckValid(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, interface) != L7_SUCCESS ||
          usmDbSnoopIsValidIntf(unit, interface) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

         /* Interface Admin Mode */
      cliFormat(ewsContext, CLIIGMP_SNOOPING_ADMINMODE(family));
      if (usmDbSnoopIntfModeGet(unit, interface, &mode, family) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode,pStrInfo_common_NotApplicable));
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }

         /* Interface Fast Leave Mode */
      cliFormat(ewsContext, pStrInfo_base_IgmpFastLeaveAdminMode);
      if (usmDbSnoopIntfFastLeaveAdminModeGet(unit, interface, &fastleavemode, family) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(fastleavemode,pStrInfo_common_NotApplicable));
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
         }

         /* Group Membership Interval */
      cliFormat(ewsContext, pStrInfo_common_GrpMbrshipIntvl);
      if (usmDbSnoopIntfGroupMembershipIntervalGet(unit, interface, &val, family) == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "%d", val);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }

         /* Query Response */
      cliFormat(ewsContext, pStrInfo_common_MaxRespTime);
      if (usmDbSnoopIntfResponseTimeGet(unit, interface, &val, family) == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "%d", val);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }

         /* Multicast Router Present */
      cliFormat(ewsContext, pStrInfo_common_McastRtrExpiryTime);
      if (usmDbSnoopIntfMcastRtrExpiryTimeGet(unit, interface, &val, family) == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "%d", val);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }
      }
   }
   else if (numArg == 0)
   {
      if (cliGetCharInputID() == CLI_INPUT_EMPTY)
      {
         vlanModeEnabled = L7_FALSE;
         interfaceEnabled = L7_FALSE;
         interfaceCompleted = L7_FALSE;
         /* Admin Mode */
      cliFormat(ewsContext, pStrInfo_common_IgmpProxyAdminMode);
      if (usmDbSnoopAdminModeGet(unit, &mode, family) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode,pStrInfo_common_NotApplicable));
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }
         /* Control Frame Count */
      cliFormat(ewsContext, pStrInfo_common_McastCntrlFrameCount);
      if (usmDbSnoopControlFramesProcessed(unit, &val, family) == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "%d", val);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
      }

      if (family == L7_AF_INET)
      {
        /* Router alert check */
        memset(stat, L7_NULL, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_ipmcast_IgmpRouterAlertCheck);
        if (usmDbSnoopRouterAlertMandatoryGet (&val, L7_AF_INET) == L7_SUCCESS)
        {
          osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);
      }
         intIfNum = 0;
         nextvid = 1;
      }
      else
      {
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        firstTime = L7_TRUE;
            return cliPrompt(ewsContext);
         }
      }

      if (interfaceCompleted == L7_FALSE)
      {
         /* Enabled Interfaces */
      cliFormat(ewsContext, CLIINTERFACES_ENABLED_FOR_IGMP_SNOOPING(family));
         while (usmDbValidIntIfNumNext(intIfNum, &intIfNum) == L7_SUCCESS)
         {
        if (usmDbSnoopIntfModeGet(unit, intIfNum, &val, family) == L7_SUCCESS)
            {
               if (val == L7_ENABLE)
               {
                  interfaceEnabled = L7_TRUE;
            memset (stat, 0, sizeof(stat));
                  if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
                  {
                     if (hasInterfaces == L7_FALSE)
              {
                osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
              }
              else
              {
                osapiSnprintf(stat, sizeof(stat), "\r\n                                                %s", cliDisplayInterfaceHelp(u, s, p));
              }
                     ewsTelnetWrite(ewsContext, stat);
                     hasInterfaces = L7_TRUE;
                  }
                  count++;
                  if (count == 15)
                  {
                     cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
                     cliSyntaxBottom(ewsContext);
              if (family == L7_AF_INET)
              {
                cliAlternateCommandSet(pStrInfo_base_ShowIgmpsnooping);
              }
              else
              {
                cliAlternateCommandSet(pStrInfo_base_ShowMldsnooping);
              }
              return pStrInfo_common_Name_2;          /* --More-- or (q)uit */
                  }
               }
            }
         }
         if (interfaceEnabled == L7_FALSE)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_None_1);
      }
      }
      interfaceCompleted = L7_TRUE;

    if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID, L7_IGMP_SNOOPING_FEATURE_PER_VLAN) == L7_TRUE)
      {
         /* VLANS FOR IGMP SNOOPING */
      if (family == L7_AF_INET)
      {
        cliFormat(ewsContext, pStrInfo_base_VlansEnbldForIgmpSnooping);
      }
      else
      {
        cliFormat(ewsContext, pStrInfo_base_VlansEnbldForMldSnooping);
      }
        
         nextvid = L7_NULL;
         while (usmDbSnoopVlanNextGet(nextvid, &nextvid) == L7_SUCCESS)
         {
        if (usmDbSnoopVlanModeGet(unit, nextvid, &vlanMode, family) == L7_SUCCESS)
            {
               if (vlanMode == L7_ENABLE)
               {
                  vlanModeEnabled = L7_TRUE;
            memset (stat, 0, sizeof(stat));
                  if (hasVlans == L7_FALSE)
            {
              osapiSnprintf(stat, sizeof(stat), "%d",nextvid);
            }
            else
            {
              osapiSnprintf(stat, sizeof(stat), "\r\n                                              %d", nextvid);
            }
                  ewsTelnetWrite(ewsContext, stat);
                  hasVlans = L7_TRUE;
                  count++;
                  if (count == 15)
                  {
                     nextvid++;
                     cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
                     cliSyntaxBottom(ewsContext);
              if (family == L7_AF_INET)
              {
                cliAlternateCommandSet(pStrInfo_base_ShowIgmpsnooping);
              }
              else
              {
                cliAlternateCommandSet(pStrInfo_base_ShowMldsnooping);
              }
              return pStrInfo_common_Name_2;          /* --More-- or (q)uit */
                  }
               }
            }
         }

         if (vlanModeEnabled == L7_FALSE)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_None_1);
      }
      }
   }
   vlanModeEnabled = L7_FALSE;
   interfaceEnabled = L7_FALSE;
   interfaceCompleted = L7_FALSE;

   cliSyntaxBottom(ewsContext);
  firstTime = L7_TRUE;
   return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose Displays the IGMP Snooping Multicast Router detected information for a specified interface
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
* @notes     Here we  display all Multicast Router detected  information.
*
* @cmdsyntax    show igmpsnooping mrouter interface <slot/port>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIGMPSnoopingMrouterInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_int32 u = 1;
   L7_int32 s;
   L7_int32 p;
   L7_uint32 unit;
   L7_uint32 slot, port;
   L7_uint32 argInterface=1;
   L7_uint32 argSlotPort=2;
   L7_uint32 interface;
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 numArg;        /* New variable Added */
   L7_RC_t rc;
   L7_uint32 vlanId = 0;
   L7_uint32 operVlanMode = L7_DISABLE;
   L7_uchar8 cfgVlanMode = L7_DISABLE;
   L7_uint32 operIntfMode = L7_DISABLE;
   L7_uint32 cfgIntfMode = L7_DISABLE;
   L7_uint32 mcastRtrEnabled = L7_FALSE;
  L7_uchar8 family = L7_AF_INET;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmpsnooping) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mldsnooping) == 0)
  {
    family = L7_AF_INET6;
  }

   if (numArg != 2)
   {
    memset (buf, 0, sizeof(buf));
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if (strcmp(argv[index+argInterface], pStrInfo_common_Ipv6DhcpRelayIntf_1) == 0)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    cliFormat(ewsContext, pStrInfo_common_SlotPortWithoutUnit_1);
    if (usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(u, s, p));
    }

    /* Add code for the interface to be valid for snooping */

    cliFormat(ewsContext, pStrInfo_base_McastRtrAttached);
    (void)usmDbSnoopIntfMrouterStatusGet(unit, interface, &cfgIntfMode, family);
      if (cfgIntfMode == L7_ENABLE)
    {
      osapiSnprintf(stat, sizeof(stat), "%s ", pStrInfo_common_Enbl_1);
    }
      else
      {
         mcastRtrEnabled = L7_FALSE;
         vlanId = L7_NULL;
         while (usmDbSnoopVlanNextGet(vlanId, &vlanId) == L7_SUCCESS)
         {
            operIntfMode = L7_DISABLE;
        (void)usmDbSnoopIntfMrouterGet(unit, interface, vlanId, &operIntfMode, family);
            if (operIntfMode == L7_ENABLE)
            {
               mcastRtrEnabled = L7_TRUE;
               break;
            }
         }
      osapiSnprintf(stat,sizeof(stat),"%s ",strUtilEnableDisableGet(mcastRtrEnabled,pStrInfo_common_Dsbl_1));
    }
    ewsTelnetWrite(ewsContext,stat);
  }
  else if (strcmp(argv[index+argInterface],pStrInfo_common_MacAclVlan_1) == 0)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    cliFormat(ewsContext, pStrInfo_common_SlotPortWithoutUnit_1);
    if (usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(u, s, p));
    }

    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_VlanId_1);
      ewsTelnetWrite(ewsContext,"\r\n--------");

      vlanId = L7_NULL;
      while (usmDbSnoopVlanNextGet(vlanId, &vlanId) == L7_SUCCESS)
      {
         cfgVlanMode = L7_DISABLE;
         operVlanMode = L7_DISABLE;

      (void) usmDbsnoopIntfApiVlanStaticMcastRtrGet(unit, interface, vlanId, &cfgVlanMode, family);
      (void) usmDbSnoopIntfMrouterGet(unit, interface, vlanId, &operVlanMode, family);
      osapiSnprintf(buf, sizeof(buf), CLISYNTAX_SHOWIGMPSNOOPING_MROUTERINTERFACE(family),
          cliSyntaxInterfaceHelp());
         if ((cfgVlanMode == L7_ENABLE) || (operVlanMode == L7_ENABLE))
         {
        ewsTelnetPrintf (ewsContext, "\r\n%-6d", vlanId);
         }
      }
   } /* END OF VLAN */
   else
   {
    return cliSyntaxReturnPrompt (ewsContext,CLISYNTAX_SHOWIGMPSNOOPING_MROUTERINTERFACE(family),
        cliSyntaxInterfaceHelp());
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display port-channel (LAG) information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add argument checking
*
* @cmdsyntax  show port-channel <slot/port | all>
*
* @cmdhelp Display information for a specific or all port-channels (LAGs).
*
* @cmddescript
*   Show an overview of lags including name, status, and member ports.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowPortChannel(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_RC_t lrc;
   L7_uint32 val;
   L7_uint32 count = L7_MAX_MEMBERS_PER_LAG;
   L7_uint32 currentIntIfNum;
   L7_uint32 currentExtIfNum;
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 adminMode[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 portChannelName[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 portChannelSlotAndPort[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 portChannelSlotAndPortTemp[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 portChannelMembers[L7_MAX_MEMBERS_PER_LAG];
   L7_int32 i;

   L7_uint32 logicalSlot;
   L7_uint32 logicalPort;
   L7_uchar8 strLogicalSlotPort[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unitNum, slot, port;
   L7_uint32 argSlotPort=1;
   L7_BOOL all = L7_FALSE;
   L7_uint32 unit;
   L7_BOOL boolVal;
   L7_uint32 numArg;        /* New variable Added */
   L7_uchar8 portChannelType[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 phyCapability;
   L7_BOOL sfpLinkUp;
   L7_uchar8 lacpState;
   L7_uint32 loadBalance;

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLagDetailed, cliSyntaxLogInterfaceHelp());
  }

  if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    all = L7_TRUE;
    lrc = usmDbDot3adIfIndexGetFirst(unit, &currentExtIfNum);
    if (lrc == L7_SUCCESS)
    {
      lrc = usmDbIntIfNumFromExtIfNum(currentExtIfNum, &currentIntIfNum);
    }
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit,
              &logicalSlot, &logicalPort)) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidSlotPort_1);
        return cliPrompt(ewsContext);
      }

         /* Get interface and check its validity */
         lrc = usmDbIntIfNumFromUSPGet(unit, logicalSlot, logicalPort, &currentIntIfNum);
         if (lrc != L7_SUCCESS)
         {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidSlotPort_1);
        return cliPrompt(ewsContext); 
         }
      }
      else
      {
         /* NOTE: No need to check the value of `unit` as
          *       ID of a standalone switch is always `U_IDX` (=> 1).
          */
         unit = cliGetUnitId();
         lrc = cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &logicalSlot, &logicalPort, &currentIntIfNum);
         if (lrc != L7_SUCCESS)
      {
            return cliPrompt(ewsContext);
      }
      }
   }

   if (!all  &&  usmDbDot3adIsConfigured(unit, currentIntIfNum) == L7_FALSE)
   {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_InvalidLag, cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
  }

  if(all)
  {
  ewsTelnetWrite(ewsContext,"\r\n                                                                         ");
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LogChannelAdmMbrDevicePortPort);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_IntfNameLinkModeTypePortsTimeoutSpeedActive);
  ewsTelnetWrite(ewsContext,"\r\n------ --------------- ------ ---- ---- ------ ------------- --------- -------");

   while (lrc == L7_SUCCESS)
   {
      if (usmDbDot3adIsConfigured(unit, currentIntIfNum) == L7_TRUE)         /* only print those port-channels that have been created */
      {
      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
         if (usmDbUnitSlotPortGet(currentIntIfNum, &unit, &logicalSlot, &logicalPort) == L7_SUCCESS)
         {
        osapiSnprintf(strLogicalSlotPort,sizeof(strLogicalSlotPort), cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
        ewsTelnetPrintf (ewsContext, "%-7.6s",strLogicalSlotPort);
      }

      memset (portChannelName, 0,sizeof(portChannelName));
      rc = usmDbDot3adNameGet(unit, currentIntIfNum, buf);
      ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

      memset (stat, 0,sizeof(stat));
      rc = usmDbIfOperStatusGet(unit, currentIntIfNum, &val);
      switch (val)                                                           /* val = up or down */
      {
        case L7_DOWN:
          osapiSnprintf(stat,sizeof(stat),"%-6s ", pStrInfo_common_Down_1);
            break;
         case L7_UP:
            /* check to see if the link up came from the SFP module */
            rc = usmDbIntfPhyCapabilityGet(currentIntIfNum, &phyCapability);
            if (phyCapability & L7_PHY_CAP_PORTSPEED_SFP)
            {
               if (usmDbStatusSFPLinkUpGet(unit, currentIntIfNum, &sfpLinkUp) == L7_SUCCESS)
               {
                  if (sfpLinkUp == L7_TRUE)
                  {
                osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_base_UpSfp);
                  }
                  else
                  {
                osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Up);
                  }
               }
            }
            else
            {
            osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Up);
            }
            break;
         default:
          osapiSnprintf(stat,sizeof(stat),"%-6s ", pStrInfo_common_Down_1);
            break;
         }
         ewsTelnetWrite(ewsContext,stat);

      memset (adminMode, 0,sizeof(adminMode));
         rc = usmDbDot3adAdminModeGet(unit, currentIntIfNum, &val);
         switch (val)                                                        /* val = L7_ENABLE, L7_DISABLE, ------  */
         {
         case L7_ENABLE:
          osapiSnprintf(adminMode,sizeof(adminMode),"%-5.4s",pStrInfo_base_Enbl);
            break;
         case L7_DISABLE:
          osapiSnprintf(adminMode,sizeof(adminMode),"%-5.4s",pStrInfo_base_Dsbl);
            break;
         default:
          osapiSnprintf(adminMode,sizeof(adminMode),"%-5.4s","----");
         }
         ewsTelnetWrite(ewsContext,adminMode);

      rc = usmDbDot3adIsStaticLag(unit, currentIntIfNum, &boolVal);

      memset (portChannelType, 0,sizeof(portChannelType));
         if (boolVal == L7_TRUE)
      {
        osapiSnprintf(portChannelType, sizeof(portChannelType), "%-5.4s", pStrInfo_base_Static);
      }
      else
      {
        osapiSnprintf(portChannelType, sizeof(portChannelType), "%-5.4s", pStrInfo_base_Dyn);
      }

      ewsTelnetWrite(ewsContext,portChannelType);

      count = L7_MAX_MEMBERS_PER_LAG;
      rc = usmDbDot3adMemberListGet(unit, currentIntIfNum, &count, portChannelMembers);      /* note portChannelMembers[8] */

      if (count == 0)
      {
        ewsTelnetWrite(ewsContext,pStrInfo_common_EmptyString);
      }
      else
      {
        /* need a loop that prints out every MbrPort that belongs to this port-channel */
        for (i = 0; i < count;)
        {
          if (usmDbUnitSlotPortGet(portChannelMembers[i], &unitNum, &slot, &port) == L7_SUCCESS)
          {
            memset (portChannelSlotAndPort, 0, sizeof(portChannelSlotAndPort));
            osapiSnprintf(portChannelSlotAndPortTemp,sizeof(portChannelSlotAndPortTemp), cliDisplayInterfaceHelp(unitNum, slot, port));
            ewsTelnetPrintf (ewsContext, "%-7.6s", portChannelSlotAndPortTemp);
               }

               if (usmDbDot3adAggPortActorAdminStateGet(unit, portChannelMembers[i], (L7_uchar8 *) &lacpState) == L7_SUCCESS)
               {
                  if (((L7_uchar8) (lacpState & DOT3AD_STATE_LACP_TIMEOUT)))
            {
              osapiStrncpySafe(buf, pStrInfo_base_Short_1, sizeof(buf));
            }
            else
            {
              osapiStrncpySafe(buf, pStrInfo_base_Long, sizeof(buf));
            }
            osapiSnprintf(stat, sizeof(stat), "%s/%s", pStrInfo_base_Actor, buf);
            ewsTelnetPrintf (ewsContext, "%-14.13s", stat);
          }

          memset (stat, 0,sizeof(stat));
               rc = usmDbIfAutoNegAdminStatusGet(unit, portChannelMembers[i], &val);
               if (val == L7_ENABLE)
               {
                  /* we are in auto-negotiate */
                  osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_common_Auto);
               }
               else  /* print out the correct speed */
               {
                  memset (stat, 0, sizeof(stat));
                  usmDbIfSpeedGet(unit, portChannelMembers[i], &val);
                  switch (val)
                  {
                    case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_common_Auto);
                      break;
                    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed100Half);
                      break;
                    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed100Full);
                      break;
                    case L7_PORTCTRL_PORTSPEED_HALF_10T:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed10Half);
                      break;
                    case L7_PORTCTRL_PORTSPEED_FULL_10T:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed10Full);
                      break;
                    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed100FxFull);
                      break;
                    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed1000Full);
                      break;
                    /* PTin added: Speed 2.5G */
                    case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed2500Full);
                      break;
                    /* PTin end */
                    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed10GigFull);
                      break;
                    /* PTin added: Speed 40G */
                    case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed40GigFull);
                      break;
                    /* PTin added: Speed 100G */
                    case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
                      osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed100GigFull);
                      break;
                    /* PTin end */
                    default:
                      osapiSnprintf (stat,sizeof(stat),"%-10.9s", pStrInfo_common_EmptyString);
                      break;
                  }
               }
               ewsTelnetWrite(ewsContext,stat);

          memset (stat, 0,sizeof(stat));
               rc = usmDbDot3adIsActiveMember(unit, portChannelMembers[i]);
               if (rc == L7_SUCCESS)
          {
            osapiSnprintf(stat,sizeof(stat),"%-7.6s", pStrInfo_common_True2);
          }
          else
          {
            osapiSnprintf(stat,sizeof(stat),"%-7.6s", pStrInfo_common_False2);
          }

          ewsTelnetWrite(ewsContext, stat);

               if (usmDbDot3adAggPortPartnerAdminStateGet(unit, portChannelMembers[i], (L7_uchar8 *) &lacpState) == L7_SUCCESS)
               {
                  if (((L7_uchar8) (lacpState & DOT3AD_STATE_LACP_TIMEOUT)))
            {
              osapiStrncpySafe(buf, pStrInfo_base_Short_1, sizeof(buf));
            }
            else
            {
              osapiStrncpySafe(buf, pStrInfo_base_Long, sizeof(buf));
            }
            ewsTelnetPrintf (ewsContext, "\r\n%47s",pStrInfo_common_EmptyString);
            osapiSnprintf(stat, sizeof(stat), "%s/%s", pStrInfo_base_Partner, buf);
            ewsTelnetPrintf (ewsContext, "%-14.13s", stat);
               }

               i++;
               if (i < count)
               {
                   ewsTelnetPrintf (ewsContext, "\r\n%-40s", pStrInfo_common_EmptyString);
               }
            }
         }

        if (usmDbDot3adIfIndexGetNext(unit, currentExtIfNum, &currentExtIfNum) == L7_SUCCESS)
         {
          lrc = usmDbIntIfNumFromExtIfNum(currentExtIfNum, &currentIntIfNum);
         }
         else
         {
          lrc = L7_FAILURE;
      }
    }
      else
      {
            if (usmDbDot3adIfIndexGetNext(unit, currentExtIfNum, &currentExtIfNum) == L7_SUCCESS)
            {
               lrc = usmDbIntIfNumFromExtIfNum(currentExtIfNum, &currentIntIfNum);
            }
            else
        {
               lrc = L7_FAILURE;
         }
      }
   } /* end of while  */
  }
  else
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    cliFormat(ewsContext, pStrInfo_base_LocalIntf_1);
    if (usmDbDot3adIsConfigured(unit, currentIntIfNum) == L7_TRUE)         /* only print those port-channels that have been created */
    {
      if (usmDbUnitSlotPortGet(currentIntIfNum, &unit, &logicalSlot, &logicalPort) == L7_SUCCESS)
      {
        osapiSnprintf(strLogicalSlotPort,sizeof(strLogicalSlotPort), cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
        ewsTelnetPrintf (ewsContext, "%s",strLogicalSlotPort);
      }
    }
    
    cliFormat(ewsContext, pStrInfo_base_PortChannelName);
    memset (portChannelName, 0,sizeof(portChannelName));
    rc = usmDbDot3adNameGet(unit, currentIntIfNum, buf);
    ewsTelnetPrintf (ewsContext, "%s", buf);

    cliFormat(ewsContext, pStrInfo_common_LinkState);
    memset (stat, 0,sizeof(stat));
    rc = usmDbIfOperStatusGet(unit, currentIntIfNum, &val);
    switch (val)                                                           /* val = up or down */
    {
    case L7_DOWN:
      osapiSnprintf(stat,sizeof(stat),"%s", pStrInfo_common_Down_1);
      break;
    case L7_UP:
      /* check to see if the link up came from the SFP module */
      rc = usmDbIntfPhyCapabilityGet(currentIntIfNum, &phyCapability);
      if (phyCapability & L7_PHY_CAP_PORTSPEED_SFP)
      {
        if (usmDbStatusSFPLinkUpGet(unit, currentIntIfNum, &sfpLinkUp) == L7_SUCCESS)
        {
          if (sfpLinkUp == L7_TRUE)
          {
            osapiSnprintf(stat,sizeof(stat),"%s",pStrInfo_base_UpSfp);
          }
          else
          {
            osapiSnprintf(stat,sizeof(stat),"%s",pStrInfo_common_Up);
          }
        }
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat),"%s",pStrInfo_common_Up);
      }
      break;
    default:
      osapiSnprintf(stat,sizeof(stat),"%s", pStrInfo_common_Down_1);
      break;
    }
    ewsTelnetWrite(ewsContext,stat);

    cliFormat(ewsContext, pStrInfo_base_AdminMode_1);
    memset (adminMode, 0,sizeof(adminMode));
    rc = usmDbDot3adAdminModeGet(unit, currentIntIfNum, &val);
    switch (val)                                                        /* val = L7_ENABLE, L7_DISABLE, ------  */
    {
    case L7_ENABLE:
      osapiSnprintf(adminMode,sizeof(adminMode),"%s",pStrInfo_base_Enabled);
      break;
    case L7_DISABLE:
      osapiSnprintf(adminMode,sizeof(adminMode),"%s",pStrInfo_base_Disabled);
      break;
    default:
      osapiSnprintf(adminMode,sizeof(adminMode),"%s",pStrInfo_base_Disabled);
    }
    ewsTelnetWrite(ewsContext,adminMode);

    cliFormat(ewsContext, pStrInfo_base_Type_2);
    rc = usmDbDot3adIsStaticLag(unit, currentIntIfNum, &boolVal);
    memset (portChannelType, 0,sizeof(portChannelType));
    if (boolVal == L7_TRUE)
    {
      osapiSnprintf(portChannelType, sizeof(portChannelType), "%s", pStrInfo_base_Static_1);
    }
    else
    {
      osapiSnprintf(portChannelType, sizeof(portChannelType), "%s", pStrInfo_base_Dynamic);
    }
    ewsTelnetWrite(ewsContext,portChannelType);

    cliFormat(ewsContext, pStrInfo_base_LoadBalanceOption);
    memset (stat, 0,sizeof(stat));
    if((rc = usmDbDot3adLagHashModeGet(unit, currentIntIfNum, &loadBalance)) == L7_SUCCESS)
    {
      switch(loadBalance)
      {
      case L7_DOT3AD_HASHING_MODE_SA_VLAN:
       if (usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, 
                                    L7_DOT3AD_HASHMODE_SA_FEATURE_ID) == L7_TRUE)
       {
          osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeSA);
       }
       else
       {
          osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeSAEthIntf);
       }
        break;
      case L7_DOT3AD_HASHING_MODE_DA_VLAN:
       if (usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID,
                                    L7_DOT3AD_HASHMODE_DA_FEATURE_ID) == L7_TRUE)
       {
         osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeDA);
       }
       else
       {
         osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeDAEthIntf);
       }
        break;
      case L7_DOT3AD_HASHING_MODE_SDA_VLAN:
       if (usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID,
                                    L7_DOT3AD_HASHMODE_SADA_FEATURE_ID) == L7_TRUE)
       {
         osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeSADA);
       }
       else
       {
         osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeSADAEthIntf);
       }
        break;
      case L7_DOT3AD_HASHING_MODE_SIP_SPORT:
        osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeSrcIP);
        break;
      case L7_DOT3AD_HASHING_MODE_DIP_DPORT:
        osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeDestIP);
        break;
      case L7_DOT3AD_HASHING_MODE_SDIP_DPORT:
        osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeSrcDestIP);
        break;
      
      case L7_DOT3AD_HASHING_MODE_ENHANCED:
        osapiSnprintf(stat, sizeof(stat), "%d\r\n(%s)", loadBalance, pStrInfo_base_HashModeEnhanced);
        break;
      
      default:
        ewsTelnetWrite(ewsContext,pStrInfo_common_EmptyString);
        break;
      }
      ewsTelnetWrite(ewsContext, stat);
    }

    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MbrDevicePortPort);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_PortsTimeoutSpeedActive);
    ewsTelnetWrite(ewsContext, pStrInfo_base_ShowPortChannelDivideLine);

    count = L7_MAX_MEMBERS_PER_LAG;
    rc = usmDbDot3adMemberListGet(unit, currentIntIfNum, &count, portChannelMembers);      /* note portChannelMembers[8] */
    if (count == 0)
    {
      ewsTelnetWrite(ewsContext,pStrInfo_common_EmptyString);
    }
    else
    {
      /* need a loop that prints out every MbrPort that belongs to this port-channel */
      for (i = 0; i < count; i++)
      {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        if (usmDbUnitSlotPortGet(portChannelMembers[i], &unitNum, &slot, &port) == L7_SUCCESS)
        {
          memset (portChannelSlotAndPort, 0, sizeof(portChannelSlotAndPort));
          osapiSnprintf(portChannelSlotAndPortTemp,sizeof(portChannelSlotAndPortTemp), cliDisplayInterfaceHelp(unitNum, slot, port));
          ewsTelnetPrintf (ewsContext, "%-7.6s", portChannelSlotAndPortTemp);
        }

        if (usmDbDot3adAggPortActorAdminStateGet(unit, portChannelMembers[i], (L7_uchar8 *) &lacpState) == L7_SUCCESS)
        {
          if (((L7_uchar8) (lacpState & DOT3AD_STATE_LACP_TIMEOUT)))
          {
            osapiStrncpySafe(buf, pStrInfo_base_Short_1, sizeof(buf));
          }
          else
          {
            osapiStrncpySafe(buf, pStrInfo_base_Long, sizeof(buf));
          }
          osapiSnprintf(stat, sizeof(stat), "%s/%s", pStrInfo_base_Actor, buf);
          ewsTelnetPrintf (ewsContext, "%-14.13s", stat);
        }

        memset (stat, 0,sizeof(stat));
        rc = usmDbIfAutoNegAdminStatusGet(unit, portChannelMembers[i], &val);
        if (val == L7_ENABLE)
        {
          /* we are in auto-negotiate */
          osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_common_Auto);
        }
        else  /* print out the correct speed */
        {
          memset (stat, 0, sizeof(stat));
          usmDbIfSpeedGet(unit, portChannelMembers[i], &val);
          switch (val)
          {
          case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_common_Auto);
            break;
          case L7_PORTCTRL_PORTSPEED_HALF_100TX:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed100Half);
            break;
          case L7_PORTCTRL_PORTSPEED_FULL_100TX:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed100Full);
            break;
          case L7_PORTCTRL_PORTSPEED_HALF_10T:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed10Half);
            break;
          case L7_PORTCTRL_PORTSPEED_FULL_10T:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed10Full);
            break;
          case L7_PORTCTRL_PORTSPEED_FULL_100FX:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed100FxFull);
            break;
          case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed1000Full);
            break;
          /* PTin added: Speed 2.5G */
          case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed2500Full);
            break;
          /* PTin end */
          case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed10GigFull);
            break;
          /* PTin added: Speed 40G */
          case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed40GigFull);
            break;
          /* PTin added: Speed 100G */
          case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
            osapiSnprintf(stat,sizeof(stat),"%-10.9s", pStrInfo_base_Speed100GigFull);
            break;
          /* PTin end */
          default:
            osapiSnprintf (stat,sizeof(stat),"%-10.9s", pStrInfo_common_EmptyString);
            break;
          }
        }
        ewsTelnetWrite(ewsContext,stat);

        memset (stat, 0,sizeof(stat));
        rc = usmDbDot3adIsActiveMember(unit, portChannelMembers[i]);
        if (rc == L7_SUCCESS)
        {
          osapiSnprintf(stat,sizeof(stat),"%-7.6s", pStrInfo_common_True2);
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat),"%-7.6s", pStrInfo_common_False2);
        }
        ewsTelnetWrite(ewsContext, stat);

        if (usmDbDot3adAggPortPartnerAdminStateGet(unit, portChannelMembers[i], (L7_uchar8 *) &lacpState) == L7_SUCCESS)
        {
          if (((L7_uchar8) (lacpState & DOT3AD_STATE_LACP_TIMEOUT)))
          {
            osapiStrncpySafe(buf, pStrInfo_base_Short_1, sizeof(buf));
          }
          else
          {
            osapiStrncpySafe(buf, pStrInfo_base_Long, sizeof(buf));
          }
          ewsTelnetPrintf (ewsContext, "\r\n%7s",pStrInfo_common_EmptyString);
          osapiSnprintf(stat, sizeof(stat), "%s/%s", pStrInfo_base_Partner, buf);
          ewsTelnetPrintf (ewsContext, "%-14.13s", stat);
        }
      }
    }

    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");

}
/*********************************************************************
*
* @purpose  display port-channel (LAG) summary information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add argument checking
*
* @cmdsyntax  show port-channel brief
*
* @cmdhelp Display summary information for a specific or all port-channels (LAGs).
*
* @cmddescript
*   Show an overview of lags including name, status, member ports, and active ports.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowPortChannelSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_RC_t lrc;
   L7_uint32 val;
   L7_uint32 count = L7_MAX_MEMBERS_PER_LAG;
   L7_uint32 currentIntIfNum;
   L7_uint32 currentExtIfNum;
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 portChannelMembers[L7_MAX_MEMBERS_PER_LAG];
   L7_int32 i;
   L7_uchar8 portChannelType[L7_MAX_MEMBERS_PER_LAG];
   L7_BOOL boolVal;
   L7_uint32 logicalSlot;
   L7_uint32 logicalPort;
   L7_uchar8 strLogicalSlotPort[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unitNum, slot, port;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */
   L7_char8  interfaceListMbr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  interfaceListActive[L7_CLI_MAX_STRING_LENGTH];
   L7_BOOL first, commaFlagActive, commaFlagMbr;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLagSummary_1);
   }

   ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

   lrc = usmDbDot3adIfIndexGetFirst(unit, &currentExtIfNum);
   if (lrc == L7_SUCCESS)
   {
     lrc = usmDbIntIfNumFromExtIfNum(currentExtIfNum, &currentIntIfNum);
   }

  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LogicalIntfLagNameLinkStateTypeMbrPortsActivePorts);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LogicalIntfLagNameLinkStateTypeMbrPortsActivePorts_1);
  ewsTelnetWrite(ewsContext,"\r\n--------- ----------------- ---------- -------- ------- --------- ------------");

  while (lrc == L7_SUCCESS)
  {
    if (usmDbDot3adIsConfigured(unit, currentIntIfNum) == L7_TRUE)         /* only print those port-channels that have been created */
    {
      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
      rc = usmDbUnitSlotPortGet(currentIntIfNum, &unit, &logicalSlot, &logicalPort);
      if (rc == L7_SUCCESS)
      {
        osapiSnprintf(strLogicalSlotPort,sizeof(strLogicalSlotPort), cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
        ewsTelnetPrintf (ewsContext, "%-10.9s",strLogicalSlotPort);
      }

      memset (buf, 0,sizeof(buf));
      rc = usmDbDot3adNameGet(unit, currentIntIfNum, buf);
      if (rc == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "%-18.17s", buf);
      }
      memset (stat, 0,sizeof(stat));
      rc = usmDbIfOperStatusGet(unit, currentIntIfNum, &val);
      if (rc == L7_SUCCESS)
      {
        memset (stat, 0,sizeof(stat));
        switch (val)                                                            /* val = up or down */
        {
          case L7_DOWN:
            osapiSnprintf(stat,sizeof(stat),"%-11.10s",pStrInfo_common_Down_1);
            break;
          case L7_UP:
            osapiSnprintf(stat,sizeof(stat),"%-11.10s",pStrInfo_common_Up);
            break;
          default:
            osapiSnprintf(stat,sizeof(stat),"%-11.10s",pStrInfo_common_Down_1);
        }
        ewsTelnetWrite(ewsContext,stat);
      }

      if(usmDbDot3adLinkTrapGet(unit, currentIntIfNum, &val) == L7_SUCCESS)
      {
         osapiSnprintf(stat,sizeof(stat),"%-9.8s", strUtilEnabledDisabledGet(val ,pStrInfo_common_Dsbld));
         ewsTelnetWrite(ewsContext,stat);
      }

      rc = usmDbDot3adIsStaticLag(unit, currentIntIfNum, &boolVal);
      memset (portChannelType, 0,sizeof(portChannelType));
      if (boolVal == L7_TRUE)
      {
        osapiSnprintf(portChannelType, sizeof(portChannelType), "%-8.7s", pStrInfo_common_PimSmGrpRpMapStatic);
      }
      else
      {
        osapiSnprintf(portChannelType, sizeof(portChannelType), "%-8.7s", pStrInfo_common_Dyn_1);
      }

      ewsTelnetWrite(ewsContext,portChannelType);

      count = L7_MAX_MEMBERS_PER_LAG;
      rc = usmDbDot3adMemberListGet(unit, currentIntIfNum, &count, portChannelMembers);      /* note portChannelMembers[8] */

      if (count == 0)
      {
        ewsTelnetWrite(ewsContext,pStrInfo_common_EmptyString);
      }
      else
      {

        memset (interfaceListMbr, 0,sizeof(interfaceListMbr));
        memset (interfaceListActive, 0,sizeof(interfaceListActive));
        
	/* need a loop that prints out every MbrPort that belongs to this port-channel */
        commaFlagMbr=L7_FALSE;
        commaFlagActive=L7_FALSE;
        for (i = 0; i < count;)
        {
          if (usmDbUnitSlotPortGet(portChannelMembers[i], &unitNum, &slot, &port) == L7_SUCCESS)
          {

            osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(unitNum, slot, port));

            if (commaFlagMbr == L7_TRUE)
            {
              OSAPI_STRNCAT(interfaceListMbr, ",");
            }
            commaFlagMbr = L7_TRUE;
            OSAPI_STRNCAT(interfaceListMbr, stat);

            rc = usmDbDot3adIsActiveMember(unit, portChannelMembers[i]);
            if (rc == L7_SUCCESS)
            {
              osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(unitNum, slot, port));
              if (commaFlagActive == L7_TRUE)
              {
                OSAPI_STRNCAT(interfaceListActive, ",");
              }
              commaFlagActive = L7_TRUE;
              OSAPI_STRNCAT(interfaceListActive, stat);
            }
          }
          i++;
        }

        first = L7_TRUE;
        osapiSnprintf(buf, sizeof(buf), cliCombineStringOutput(interfaceListMbr, 9, interfaceListActive, 12));
        do
        {
          if (first == L7_TRUE)
          {
            first = L7_FALSE;
            osapiSnprintf(stat, sizeof(stat), " %-21s", buf);
          }
          else
          {
            ewsTelnetPrintf (ewsContext, "\r\n%-56s", pStrInfo_common_EmptyString);
            osapiSnprintf(stat, sizeof(stat), "%-21s", buf);
          }
          ewsTelnetWrite(ewsContext,stat);
          osapiSnprintf(buf, sizeof(buf), cliCombineStringOutput(interfaceListMbr, 9, interfaceListActive, 12));
        }while (strcmp(buf, interfaceListMbr) != 0);
      }

      if (usmDbDot3adIfIndexGetNext(unit, currentExtIfNum, &currentExtIfNum) == L7_SUCCESS)
      {
        lrc = usmDbIntIfNumFromExtIfNum(currentExtIfNum, &currentIntIfNum);
      }
      else
      {
        lrc = L7_FAILURE;
      }
    }
    else
    {
      if (usmDbDot3adIfIndexGetNext(unit, currentExtIfNum, &currentExtIfNum) == L7_SUCCESS)
      {
        lrc = usmDbIntIfNumFromExtIfNum(currentExtIfNum, &currentIntIfNum);
      }
      else
      {
        lrc = L7_FAILURE;
      }
    }
  } /* end of while  */

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
 *
 * @purpose  display port-channel system priority
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes add argument checking
 *
 * @cmdsyntax  show port-channel system priority
 *
 * @cmdhelp Display port-channel system priority.
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShowPortChannelSysPriority(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 numArg, unit, intf, priorityValue;

  unit = intf = 0;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_ShowLagSysPri_2);
  }

  if(usmDbDot3adAggActorSystemPriorityGet(unit, intf,&priorityValue) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_ShowLagSysPri_2);
  }

  cliFormat(ewsContext, pStrInfo_base_ShowLagSysPri_1);
  return cliSyntaxReturnPrompt (ewsContext,"%u", priorityValue);
}

/*********************************************************************
*
* @purpose Displays Multicast Forwarding Database GMRP entry information
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
* @notes     Here we only display the visible interfaces.
*
* @cmdsyntax    show mac-address-table gmrp
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowMacAddressTableGmrp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_RC_t rc;
   static usmdbMfdbEntry_t entry;
   static usmdbMfdbUserInfo_t compInfo;
   L7_uint32 userId;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 mac[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 listFor[256];
   L7_uint32 listFilt[256];
   L7_char8   interfaceListFor[256];
   L7_char8   interfaceListFilt[256];
   L7_BOOL commaFlag = L7_FALSE;
   L7_BOOL first = L7_TRUE;
   L7_BOOL emptyFwd = L7_TRUE;
   L7_BOOL emptyFlt = L7_TRUE;
   L7_uint32 u, s, p, i, count, numFwd, numFlt;
   L7_uint32 unit, vlanId;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   cliCmdScrollSet( L7_FALSE);
   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowMfdbGmrp_1);
  }

  userId = L7_MFDB_PROTOCOL_GMRP;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
    /* Heading for the mfdb gmrp table */
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrTypeDescIntfs);
    ewsTelnetWrite(ewsContext,"\r\n-------  -----------------------  -------  ----------------  -------------------------\r\n");
  }
  else
  {
    memset(&entry, 0, sizeof(usmdbMfdbEntry_t));
    memset(&compInfo, 0, sizeof(usmdbMfdbUserInfo_t));
    if (usmDbMfdbComponentEntryGetNext(unit, entry.usmdbMfdbVidMac, userId, &compInfo) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CurrentlyNoGmrpEntriesInTbl);
    }

    /* Heading for the mfdb gmrp table */
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrTypeDescIntfs);
    ewsTelnetWrite(ewsContext,"\r\n-------  -----------------------  -------  ----------------  -------------------------\r\n");
  }

  for (count=0; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    usmDbEntryVidMacSeparate(entry.usmdbMfdbVidMac, &vlanId, mac);
    osapiSnprintf(stat,sizeof(stat),"%-9d", vlanId);
    ewsTelnetWrite(ewsContext, stat);
    osapiSnprintf(buf, sizeof(buf),
        "%02X:%02X:%02X:%02X:%02X:%02X ",
        entry.usmdbMfdbVidMac[2],
        entry.usmdbMfdbVidMac[3],
        entry.usmdbMfdbVidMac[4],
        entry.usmdbMfdbVidMac[5],
        entry.usmdbMfdbVidMac[6],
        entry.usmdbMfdbVidMac[7]);
    ewsTelnetPrintf (ewsContext, "%-25s", buf);

    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    switch (compInfo.usmdbMfdbType)
    {
      case L7_MFDB_TYPE_STATIC:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_PimSmGrpRpMapStatic);
        break;
      case L7_MFDB_TYPE_DYNAMIC:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dyn_1);
        break;
      default:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Unknown_1);
        break;
    }
    ewsTelnetPrintf (ewsContext, "%-9s", buf);

    memset (buf, 0,sizeof(buf));
    ewsTelnetPrintf (ewsContext, "%-18s", compInfo.usmdbMfdbDescr);

    memset(interfaceListFor, 0, sizeof(interfaceListFor));
    memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
    memset(listFor, 0, sizeof(listFor));
    memset(listFilt, 0, sizeof(listFilt));
    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    rc = usmDbConvertMaskToList(&compInfo.usmdbMfdbFwdMask, listFor, &numFwd);
    rc = usmDbConvertMaskToList(&compInfo.usmdbMfdbFltMask, listFilt, &numFlt);

    /* put forwarding list in a buffer with commas to print to screen*/
    commaFlag = L7_FALSE;
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, interfaceListFor, pStrInfo_common_Fwd_2);

    for (i=1; i <= numFwd; i++)
    {
      if (usmDbUnitSlotPortGet(listFor[i], &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
        if (commaFlag == L7_TRUE)
        {
          OSAPI_STRNCAT(interfaceListFor, ",");
        }
        commaFlag = L7_TRUE;
        OSAPI_STRNCAT(interfaceListFor, stat);
      }
    }
    if (commaFlag == L7_FALSE)
    {
      emptyFwd = L7_TRUE;
    }
    else
    {emptyFwd = L7_FALSE;}

    /* put filtering list in a buffer with commas to print to screen*/
    commaFlag = L7_FALSE;
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, interfaceListFilt, pStrInfo_common_Flt);

    for (i=1; i <= numFlt; i++)
    {
      if (usmDbUnitSlotPortGet(listFilt[i], &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
        if (commaFlag == L7_TRUE)
        {
          OSAPI_STRNCAT(interfaceListFilt, ",");
        }
        commaFlag = L7_TRUE;
        OSAPI_STRNCAT(interfaceListFilt, stat);
      }
    }
    if (commaFlag == L7_FALSE)
    {
      emptyFlt = L7_TRUE;
    }
    else
    {emptyFlt = L7_FALSE;}

    if (emptyFwd == L7_TRUE)
    {
      emptyFwd = L7_FALSE;

      memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
      emptyFlt = L7_TRUE;
    }

    /* Print the lists to the CLI */
    if (emptyFwd == L7_FALSE)
    {
      memset (stat, 0,sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListFor, 25));
      do
      {
        if (first == L7_TRUE)
        {
          first = L7_FALSE;
          osapiSnprintf(stat, sizeof(stat), "%-25s", buf);
        }
        else
        {
          ewsTelnetPrintf (ewsContext, "\r\n%-52s", " ");
          osapiSnprintf(stat, sizeof(stat), "%-25s", buf);
        }

        ewsTelnetWrite(ewsContext,stat);

        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListFor, 25));
      }
      while (strcmp(buf, interfaceListFor) != 0);
      first = L7_TRUE;
    }

    if (emptyFlt == L7_FALSE)
    {
      memset (stat, 0,sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListFilt, 25));
      do
      {
        ewsTelnetPrintf (ewsContext, "\r\n%-52s", " ");
        ewsTelnetPrintf (ewsContext, "%-25s", buf);

        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListFilt, 25));
      }
      while (strcmp(buf, interfaceListFilt) != 0);
    }

    if (usmDbMfdbComponentEntryGetNext(unit, entry.usmdbMfdbVidMac, userId, &compInfo) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    cliSyntaxBottom(ewsContext);

  } /* end for*/

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_base_ShowMacAddrTblGmrpCmd);
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */

}

/*********************************************************************
*
* @purpose Displays Multicast Forwarding Database IGMP Snooping entry information
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
* @notes     Here we only display the visible interfaces.
*
* @cmdsyntax    show mac-address-table igmpsnooping
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowMacAddressTableIgmpSnooping(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_RC_t rc;
   static usmdbMfdbEntry_t entry;
   static usmdbMfdbUserInfo_t compInfo;
   static L7_uint32 userId = L7_MFDB_PROTOCOL_IGMP;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 listFor[256];
   L7_uint32 listFilt[256];
   L7_char8   interfaceListFor[256];
   L7_char8   interfaceListFilt[256];
   L7_BOOL commaFlag = L7_FALSE;
   L7_BOOL first = L7_TRUE;
   L7_BOOL emptyFwd = L7_TRUE;
   L7_BOOL emptyFlt = L7_TRUE;
   L7_uint32 u, s, p, i, count, numFwd, numFlt;
   L7_uint32 unit, vlanId;
   L7_uint32 numArg;        /* New variable Added */
  static L7_uchar8 family = L7_AF_INET;
  static L7_BOOL firstTime = L7_TRUE;

   cliSyntaxTop(ewsContext);

   cliCmdScrollSet( L7_FALSE);
   numArg = cliNumFunctionArgsGet();

  if (firstTime == L7_TRUE)
  {
    if (strcmp(argv[2], pStrInfo_common_Igmpsnooping) == 0)
    {
      family = L7_AF_INET;
      userId = L7_MFDB_PROTOCOL_IGMP;
    }
    else
    {
      family = L7_AF_INET6;
      /* MLD Snooping MFDB User ID */
      userId = L7_MFDB_PROTOCOL_MLD;
    }
    firstTime = L7_FALSE;
  }

  if (numArg != 0)
  {
    ewsTelnetWrite( ewsContext, CLISYNTAX_SHOWMFDBIGMP(family));
    cliSyntaxBottom(ewsContext);
    firstTime = L7_TRUE;
    return cliPrompt(ewsContext);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    firstTime = L7_TRUE;
    return cliPrompt(ewsContext);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      firstTime = L7_TRUE;
      return cliPrompt(ewsContext);
    }
    /* Heading for the mfdb igmp snooping table */
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrTypeDescIntfs);
    ewsTelnetWrite(ewsContext,"\r\n-------  -----------------------  -------  ----------------  -------------------------\r\n");
  }
  else
  {
    memset(&entry, 0, sizeof(usmdbMfdbEntry_t));
    memset(&compInfo, 0, sizeof(usmdbMfdbUserInfo_t));
    if (usmDbMfdbComponentEntryGetNext(unit, entry.usmdbMfdbVidMac, userId, &compInfo) != L7_SUCCESS)
    {
      if (family == L7_AF_INET)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CurrentlyNoIgmpSnoopingEntriesInTbl);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CurrentlyNoMldSnoopingEntriesInTbl);
      }
      cliSyntaxBottom(ewsContext);
      firstTime = L7_TRUE;
      return cliPrompt(ewsContext);
    }

    /* Heading for the mfdb igmp snooping table */
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrTypeDescIntfs);
    ewsTelnetWrite(ewsContext,"\r\n-------  -----------------------  -------  ----------------  -------------------------\r\n");
  }

   count = 0;
   while (count < (CLI_MAX_SCROLL_LINES-6))
   {
    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    usmDbEntryVidMacSeparate(entry.usmdbMfdbVidMac, &vlanId, mac);
    osapiSnprintf(stat,sizeof(stat),"%-9d", vlanId);
    ewsTelnetWrite(ewsContext, stat);
    osapiSnprintf(buf, sizeof(buf),
        "%02X:%02X:%02X:%02X:%02X:%02X ",
              entry.usmdbMfdbVidMac[2],
              entry.usmdbMfdbVidMac[3],
              entry.usmdbMfdbVidMac[4],
              entry.usmdbMfdbVidMac[5],
              entry.usmdbMfdbVidMac[6],
              entry.usmdbMfdbVidMac[7]);
    ewsTelnetPrintf (ewsContext, "%-25s", buf);

    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    switch (compInfo.usmdbMfdbType)
    {
      case L7_MFDB_TYPE_STATIC:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_PimSmGrpRpMapStatic);
        break;
      case L7_MFDB_TYPE_DYNAMIC:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dyn_1);
        break;
      default:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Unknown_1);
        break;
    }
    ewsTelnetPrintf (ewsContext, "%-9s", buf);

    memset (buf, 0,sizeof(buf));
    ewsTelnetPrintf (ewsContext, "%-18s", compInfo.usmdbMfdbDescr);

    memset(interfaceListFor, 0, sizeof(interfaceListFor));
    memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
    memset(listFor, 0, sizeof(listFor));
    memset(listFilt, 0, sizeof(listFilt));
    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    rc = usmDbConvertMaskToList(&compInfo.usmdbMfdbFwdMask, listFor, &numFwd);
    rc = usmDbConvertMaskToList(&compInfo.usmdbMfdbFltMask, listFilt, &numFlt);

    /* put forwarding list in a buffer with commas to print to screen*/
    commaFlag = L7_FALSE;
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, interfaceListFor, pStrInfo_common_Fwd_2);

    for (i=1; i <= numFwd; i++)
    {
      if (usmDbUnitSlotPortGet(listFor[i], &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
        if (commaFlag == L7_TRUE)
        {
          OSAPI_STRNCAT(interfaceListFor, ",");
        }
        commaFlag = L7_TRUE;
        OSAPI_STRNCAT(interfaceListFor, stat);
      }
    }
    if (commaFlag == L7_FALSE)
    {
      emptyFwd = L7_TRUE;
    }
    else
    {emptyFwd = L7_FALSE;}

    /* put filtering list in a buffer with commas to print to screen*/
    commaFlag = L7_FALSE;
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, interfaceListFilt, pStrInfo_common_Flt);

    for (i=1; i <= numFlt; i++)
    {
      if (usmDbUnitSlotPortGet(listFilt[i], &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
        if (commaFlag == L7_TRUE)
        {
          OSAPI_STRNCAT(interfaceListFilt, ",");
        }
        commaFlag = L7_TRUE;
        OSAPI_STRNCAT(interfaceListFilt, stat);
      }
    }
    if (commaFlag == L7_FALSE)
    {
      emptyFlt = L7_TRUE;
    }
    else
    {emptyFlt = L7_FALSE;}

    if (emptyFwd == L7_TRUE)
    {
      emptyFwd = L7_FALSE;

      memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
      emptyFlt = L7_TRUE;
    }

    /* Print the lists to the CLI */
    if (emptyFwd == L7_FALSE)
    {
      memset (stat, 0,sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListFor, 25));
      do
      {
        if (first == L7_TRUE)
        {
          count++;
          first = L7_FALSE;
          osapiSnprintf(stat, sizeof(stat), "%-25s", buf);
        }
        else
        {
          count++;
          ewsTelnetPrintf (ewsContext, "\r\n%-52s", " ");
          osapiSnprintf(stat, sizeof(stat), "%-25s", buf);
        }

        ewsTelnetWrite(ewsContext,stat);

        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListFor, 25));
      }
      while (strcmp(buf, interfaceListFor) != 0);
      first = L7_TRUE;
    }

    if (emptyFlt == L7_FALSE)
    {
      memset (stat, 0,sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListFilt, 25));
      do
      {
        count++;
        ewsTelnetPrintf (ewsContext, "\r\n%-52s", " ");
        ewsTelnetPrintf (ewsContext, "%-25s", buf);

        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListFilt, 25));
      }
      while (strcmp(buf, interfaceListFilt) != 0);
    }

    if (usmDbMfdbComponentEntryGetNext(unit, entry.usmdbMfdbVidMac, userId, &compInfo) != L7_SUCCESS)
    {
      cliSyntaxBottom(ewsContext);
      firstTime = L7_TRUE;
      return cliPrompt(ewsContext);
    }
    cliSyntaxBottom(ewsContext);
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliAlternateCommandSet(CLI_SHOW_MACADDRTABLE_IGMP_CMD(family));                             /*hard coded in command name for now, #define later :-) */
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */

}

/*********************************************************************
*
* @purpose Displays Multicast Forwarding Database Table information
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
* @notes     Here we only display the visible interfaces.
*
* @cmdsyntax  show mac-address-table multicast [<macaddr> <vlanid>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
 const char *commandShowMacAddressTableMulticast(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
 {
    L7_RC_t rc = L7_SUCCESS;
    L7_uint32 argMacAddr = 1;
    L7_uint32 argVlanId = 2;
    L7_uint32 vlanId;
    L7_char8 vidMac[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 temp[L7_CLI_MAX_STRING_LENGTH];
    L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 listFor[256];
    L7_uint32 listFor2[256];
    L7_uint32 listFilt[256];
    static L7_uint32 nextProt;
    L7_uint32 result;
    static usmdbMfdbUserInfo_t compInfo;
    static usmdbMfdbEntry_t entry;
    L7_INTF_MASK_t forwardingMask;
    L7_char8   interfaceListFor[256];
    L7_char8   interfaceListFor2[256];
    L7_char8   interfaceListFilt[256];
    L7_char8 interfaceListSrc[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 interfaceListDest[L7_CLI_MAX_STRING_LENGTH];
    L7_BOOL emptyFwd = L7_TRUE;
    L7_BOOL emptyFlt = L7_TRUE;
    L7_uint32 u, s, p, i, numFwd, numFlt, numFwd2;
    L7_uint32 lineCount=0;
    L7_uint32 unit;
    L7_uint32 numArg;        /* New variable Added */
    L7_BOOL fwdFiltBoth = L7_FALSE, fwdFiltBothFlag = L7_FALSE;
    L7_uint32 maxDisplaylength, displayItems, j, k;
    L7_BOOL flag=L7_FALSE;
 
    cliSyntaxTop(ewsContext);
 
    cliCmdScrollSet( L7_FALSE);
    numArg = cliNumFunctionArgsGet();
 
    if (numArg > 2)
    {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowMfdbTbl_1);
    }
 
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
 
 
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {                                                                     /* if our question has been answered */
     if(L7_TRUE == cliIsPromptRespQuit())
       {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliPrompt(ewsContext);
       }
 
       /* Heading for the mfdb table */
     ewsTelnetWriteAddBlanks (1, 0, 68, 0, L7_NULLPTR, ewsContext,pStrInfo_base_Fwd_1);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrSrcTypeDescIntfIntf);
     ewsTelnetWrite(ewsContext,"\r\n------- ----------------- ------- ------- --------------- --------- ---------\r\n");
    }
    else
    {
       if (numArg == 2)
       {
          if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
          {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidMacAddrList);
       }
       OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);
 
       memset (mac, 0, sizeof(mac));
       if (cliConvertMac(buf, mac) != L7_TRUE)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr );
       }
 
       sscanf(argv[index+argVlanId], "%d", &vlanId);
       if (usmDbVlanIDGet(unit, vlanId) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
       }
 
       /* combine mac and vlanid to get an 8-byte vidMac address */
       memset (vidMac, 0, sizeof(vidMac));
       (void)usmDbEntryVidMacCombine(vlanId, mac, vidMac);
 
       memset(&entry, 0, sizeof(usmdbMfdbEntry_t));
          /*memset(&compInfo, 0x00, sizeof(usmdbMfdbUserInfo_t));*/
          memcpy(entry.usmdbMfdbVidMac, vidMac, L7_MFDB_KEY_SIZE);
 
          if (usmDbMfdbEntryInfoGet(unit, &entry) == L7_SUCCESS)
          {
             /* Heading for the mfdb table */
         ewsTelnetWriteAddBlanks (1, 0, 68, 0, L7_NULLPTR, ewsContext,pStrInfo_base_Fwd_1);
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrSrcTypeDescIntfIntf);
         ewsTelnetWrite(ewsContext,"\r\n------- ----------------- ------- ------- --------------- --------- ---------\r\n");
 
             for (i = 0; i < L7_MFDB_MAX_USERS; i++)
             {
                if (entry.usmdbMfdbUser[i].usmdbMfdbCompId != L7_NULL)
                {
             memset (buf, 0,sizeof(buf));
             memset (stat, 0,sizeof(stat));
             usmDbEntryVidMacSeparate(entry.usmdbMfdbVidMac, &vlanId, mac);
             osapiSnprintf(stat,sizeof(stat),"%-8d", vlanId);
             ewsTelnetWrite(ewsContext, stat);
             osapiSnprintf(buf, sizeof(buf),
                 "%02X:%02X:%02X:%02X:%02X:%02X ",
                           entry.usmdbMfdbVidMac[2],
                           entry.usmdbMfdbVidMac[3],
                           entry.usmdbMfdbVidMac[4],
                           entry.usmdbMfdbVidMac[5],
                           entry.usmdbMfdbVidMac[6],
                           entry.usmdbMfdbVidMac[7]);
             ewsTelnetPrintf (ewsContext, "%-18s", buf);
 
             memset (buf, 0,sizeof(buf));
             memset (stat, 0,sizeof(stat));
                   switch (entry.usmdbMfdbUser[i].usmdbMfdbCompId)
                   {
               case L7_MFDB_PROTOCOL_STATIC:
                 osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Filter);
                      break;
               case L7_MFDB_PROTOCOL_GMRP:
                 osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Gmrp);
                 break;
               case L7_MFDB_PROTOCOL_IGMP:
                 osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Igmp_1);
                 break;
               case L7_MFDB_PROTOCOL_MLD:
                 osapiSnprintf(buf, sizeof(buf), pStrInfo_base_Mld);
                 break;
               default:
                 osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Unknown_1);
                 break;
             }
             ewsTelnetPrintf (ewsContext, "%-8s", buf);
 
             memset (buf, 0,sizeof(buf));
             memset (stat, 0,sizeof(stat));
                   switch (entry.usmdbMfdbUser[i].usmdbMfdbType)
                   {
                   case L7_MFDB_TYPE_STATIC:
                 osapiSnprintf(buf, sizeof(buf), pStrInfo_common_PimSmGrpRpMapStatic);
                      break;
                   case L7_MFDB_TYPE_DYNAMIC:
                 osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dyn_1);
                      break;
                   default:
                 osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Unknown_1);
                      break;
                   }
             ewsTelnetPrintf (ewsContext, "%-8s", buf);
 
             memset (buf, 0,sizeof(buf));
             ewsTelnetPrintf (ewsContext, "%-16s", entry.usmdbMfdbUser[i].usmdbMfdbDescr);
 
             memset(interfaceListFor, 0, sizeof(interfaceListFor));
             memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
             memset(listFor, 0, sizeof(listFor));
             memset(listFilt, 0, sizeof(listFilt));
             memset (buf, 0,sizeof(buf));
             memset (stat, 0,sizeof(stat));
             rc = usmDbConvertMaskToList(&entry.usmdbMfdbUser[i].usmdbMfdbFwdMask, listFor, &numFwd);
             rc = usmDbConvertMaskToList(&entry.usmdbMfdbUser[i].usmdbMfdbFltMask, listFilt, &numFlt);
 
             /* put forwarding list in a buffer with commas to print to screen*/
             OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 0, L7_NULLPTR, interfaceListFor, pStrInfo_common_Fwd_2);
 
             if (numFwd <= 0 )
             {
               emptyFwd = L7_TRUE;
             }
             else
             {
              emptyFwd = L7_FALSE;
             }
 
             /* put filtering list in a buffer with commas to print to screen*/
             OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, interfaceListFilt, pStrInfo_common_Flt);
 
             if (numFlt <= 0)
             {
               emptyFlt = L7_TRUE;
               memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
             }
             else
             {
               emptyFlt = L7_FALSE;
             }
 
             if (emptyFwd == L7_TRUE)
             {
               emptyFwd = L7_FALSE;
 
               memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
               emptyFlt = L7_TRUE;
             }
             else
             {
               if (emptyFlt == L7_FALSE)
               {
                 /*OSAPI_STRNCAT(interfaceListFor, pStrInfo_common_CrLf);
                 OSAPI_STRNCAT(interfaceListFor, interfaceListFilt);*/
               }
             }
 
             /* Forwarding Interfaces*/
             memset(interfaceListFor2, 0, sizeof(interfaceListFor2));
             memset(listFor2, 0, sizeof(listFor2));
             rc = usmDbMfdbEntryFwdIntfGetNext(unit, entry.usmdbMfdbVidMac, L7_MATCH_EXACT, &forwardingMask);
             rc = usmDbConvertMaskToList(&forwardingMask, listFor2, &numFwd2);
 
             OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 0, L7_NULLPTR, interfaceListFor2, pStrInfo_common_Fwd_2);
             if (numFwd2 <= 0)
             {
               emptyFwd = L7_TRUE;
               memset(interfaceListFor2, 0, sizeof(interfaceListFor2));
             }
             else
             {
               emptyFwd = L7_FALSE;
             }
 
             if((numFwd > 0) && (numFlt == 0))
               ewsTelnetPrintf (ewsContext, "%-10s", interfaceListFor);
             else if((numFlt >0) && (numFwd ==0))
               ewsTelnetPrintf (ewsContext, "%-10s  ", interfaceListFilt);
             else if ((numFwd > 0 ) && (numFlt > 0))
             {
               ewsTelnetPrintf (ewsContext, "%-10s", interfaceListFor);
               fwdFiltBoth = L7_TRUE;
               fwdFiltBothFlag = L7_TRUE;
             }
             else
               ewsTelnetPrintf (ewsContext, "%-10s",pStrInfo_common_Space);
 
            if(numFwd2 >0)
              ewsTelnetPrintf (ewsContext, "%s", interfaceListFor2);
            else
               ewsTelnetPrintf (ewsContext, "%s",pStrInfo_common_Space);
 
            maxDisplaylength = 9;
            displayItems = maxDisplaylength/7;
            i=1;
            j = 1;
 
            osapiSnprintf(temp, sizeof(temp),"\r\n%-58s",pStrInfo_common_Space);
            while((numFwd > 0) || (numFwd2 > 0) || (numFlt > 0))
            {
              bzero(interfaceListSrc, sizeof(interfaceListSrc));
              bzero(interfaceListDest, sizeof(interfaceListDest));
              flag=L7_FALSE;
              for(k = 1; k <= displayItems; k++)
              {
               if(fwdFiltBoth == L7_TRUE)
                {
                  if(listFor[j] != 0)
                  {
                    rc = usmDbUnitSlotPortGet(listFor[j], &u, &s, &p);
                    if ( rc == L7_SUCCESS )
                    {
                      osapiSnprintf(stat, sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
                    }
                    flag=L7_TRUE;
                    numFwd--;
                    OSAPI_STRNCAT(interfaceListSrc, stat);
                    if (listFor[j+1] != 0)
                    {
                      OSAPI_STRNCAT(interfaceListSrc, ",");
                    }
                  }
                  else if(listFilt[j]!= 0)
                  {
                    if(fwdFiltBothFlag == L7_TRUE)
                    {
                      osapiStrncpySafe(interfaceListSrc, interfaceListFilt, sizeof(interfaceListSrc));
                      fwdFiltBothFlag = L7_FALSE;
                    }
                    else
                    {
                      rc = usmDbUnitSlotPortGet(listFilt[j], &u, &s, &p);
                      if ( rc == L7_SUCCESS )
                      {
                        osapiSnprintf(stat, sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
                      }
                      flag=L7_TRUE;
                      numFlt--;
                      OSAPI_STRNCAT(interfaceListSrc, stat);
                      if (listFilt[j+1] != 0)
                      {
                        OSAPI_STRNCAT(interfaceListSrc, ",");
                      }
                    }
                  }
                }
                else if (listFor[i] != 0)
                {
                  rc = usmDbUnitSlotPortGet(listFor[i], &u, &s, &p);
                  if ( rc == L7_SUCCESS )
                  {
                    osapiSnprintf(stat, sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
                  }
                  flag=L7_TRUE;
                  numFwd--;
                  OSAPI_STRNCAT(interfaceListSrc, stat);
                  if (listFor[i+1] != 0)
                  {
                    OSAPI_STRNCAT(interfaceListSrc, ",");
                  }
                }
                else if (listFilt[i] != 0 )
                {
                  rc = usmDbUnitSlotPortGet(listFilt[i], &u, &s, &p);
                  if ( rc == L7_SUCCESS )
                  {
                    osapiSnprintf(stat, sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
                  }
                  flag=L7_TRUE;
                  numFlt--;
                  OSAPI_STRNCAT(interfaceListSrc, stat);
                  if (listFilt[i+1] != 0)
                  {
                    OSAPI_STRNCAT(interfaceListSrc, ",");
                  }
                }
 
                if(listFor2[i] != 0)
                {
                  memset (stat, 0,sizeof(stat));
                  rc = usmDbUnitSlotPortGet(listFor2[i], &u, &s, &p);
                  if ( rc == L7_SUCCESS )
                  {
                    osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(u, s, p));
                  }
                  flag=L7_TRUE;
                  numFwd2--;
                  OSAPI_STRNCAT(interfaceListDest, stat);
                  if (listFor2[i+1]!= 0)
                  {
                    OSAPI_STRNCAT(interfaceListDest, ",");
                  }
                }
 
                i++;
                j++;
                if((numFwd2 ==0) && (numFlt ==0) && (numFwd ==0))
                  break;
              }
 
              while(strlen(interfaceListSrc) < ( maxDisplaylength+1))
                OSAPI_STRNCAT(interfaceListSrc, " ");
 
              OSAPI_STRNCAT(interfaceListSrc, interfaceListDest);
              if(flag==L7_TRUE)
                ewsTelnetWrite(ewsContext,temp);
              ewsTelnetWrite(ewsContext,interfaceListSrc);
              lineCount++;
 
            }
            cliSyntaxBottom(ewsContext);
                }
             }
          }
          else
          {
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_EntryWasNotFound);
             cliSyntaxBottom(ewsContext);
          }
 
          return cliPrompt(ewsContext);
       }
 
       else if (numArg != 0)
       {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowMfdbTbl_1);
     }
 
     memset(&entry, 0, sizeof(usmdbMfdbEntry_t));
     memset(&compInfo, 0, sizeof(usmdbMfdbUserInfo_t));
       nextProt = 0;
 
       if (usmDbMfdbEntryGetNext(unit, entry.usmdbMfdbVidMac, &nextProt, &compInfo) != L7_SUCCESS)
       {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CurrentlyNoEntriesInTbl);
     }
     /* Heading for the mfdb table */
     ewsTelnetWriteAddBlanks (1, 0, 68, 0, L7_NULLPTR, ewsContext,pStrInfo_base_Fwd_1);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrSrcTypeDescIntfIntf);
     ewsTelnetWrite(ewsContext,"\r\n------- ----------------- ------- ------- --------------- --------- ---------\r\n");
   }
   while (rc == L7_SUCCESS)
   {
     memset(&forwardingMask, 0, sizeof(L7_INTF_MASK_t));
     rc = usmDbMfdbEntryFwdIntfGetNext(unit, entry.usmdbMfdbVidMac, L7_MATCH_EXACT, &forwardingMask);
     memset(listFor, 0, sizeof(listFor));
     memset(listFilt, 0, sizeof(listFilt));
       L7_INTF_NONZEROMASK(forwardingMask, result);
       if (result == 1)
       {
          rc = usmDbConvertMaskToList(&compInfo.usmdbMfdbFwdMask, listFor, &numFwd);
          rc = usmDbConvertMaskToList(&compInfo.usmdbMfdbFltMask, listFilt, &numFlt);
       }
       else
       {
          numFwd = 0;
          numFlt = 0;
       }
 
       /* find out how many lines this entry is going to need */
 
       if (lineCount >= CLI_MAX_SCROLL_LINES-6)
       {
          lineCount = 0;
          break;
       }
 
     memset (buf, 0,sizeof(buf));
     memset (stat, 0,sizeof(stat));
     usmDbEntryVidMacSeparate(entry.usmdbMfdbVidMac, &vlanId, mac);
     osapiSnprintf(stat,sizeof(stat),"%-8d", vlanId);
     ewsTelnetWrite(ewsContext, stat);
 
     osapiSnprintf(buf, sizeof(buf),
         "%02X:%02X:%02X:%02X:%02X:%02X ",
               entry.usmdbMfdbVidMac[2],
               entry.usmdbMfdbVidMac[3],
               entry.usmdbMfdbVidMac[4],
               entry.usmdbMfdbVidMac[5],
               entry.usmdbMfdbVidMac[6],
               entry.usmdbMfdbVidMac[7]);
     ewsTelnetPrintf (ewsContext, "%-18s", buf);
 
     memset (buf, 0,sizeof(buf));
     memset (stat, 0,sizeof(stat));
       switch (compInfo.usmdbMfdbCompId)
       {
       case L7_MFDB_PROTOCOL_STATIC:
         osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Filter);
          break;
       case L7_MFDB_PROTOCOL_GMRP:
         osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Gmrp);
         break;
       case L7_MFDB_PROTOCOL_IGMP:
         osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Igmp_1);
         break;
       case L7_MFDB_PROTOCOL_MLD:
         osapiSnprintf(buf, sizeof(buf), pStrInfo_base_Mld);
         break;
       default:
         osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Unknown_1);
         break;
     }
     ewsTelnetPrintf (ewsContext, "%-8s", buf);
 
     memset (buf, 0,sizeof(buf));
     memset (stat, 0,sizeof(stat));
     switch (compInfo.usmdbMfdbType)
     {
       case L7_MFDB_TYPE_STATIC:
         osapiSnprintf(buf, sizeof(buf), pStrInfo_common_PimSmGrpRpMapStatic);
         break;
       case L7_MFDB_TYPE_DYNAMIC:
         osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dyn_1);
         break;
       default:
         osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Unknown_1);
         break;
     }
     ewsTelnetPrintf (ewsContext, "%-8s", buf);
 
     memset (buf, 0,sizeof(buf));
     ewsTelnetPrintf (ewsContext, "%-16s", compInfo.usmdbMfdbDescr);
 
     memset(interfaceListFor, 0, sizeof(interfaceListFor));
     memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
     memset (buf, 0,sizeof(buf));
     memset (stat, 0,sizeof(stat));
     /* put forwarding list in a buffer with commas to print to screen*/
     OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 0, L7_NULLPTR, interfaceListFor, pStrInfo_common_Fwd_2);
 
     if (numFwd <= 0)
     {
       emptyFwd = L7_TRUE;
     }
     else
     {emptyFwd = L7_FALSE;}
 
     /* put filtering list in a buffer with commas to print to screen*/
     OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, interfaceListFilt, pStrInfo_common_Flt);
 
     if (numFlt <= 0)
     {
       emptyFlt = L7_TRUE;
       memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
     }
     else
     {emptyFlt = L7_FALSE;}
 
     if (emptyFwd == L7_TRUE)
     {
       emptyFwd = L7_FALSE;
 
       memset(interfaceListFilt, 0, sizeof(interfaceListFilt));
       emptyFlt = L7_TRUE;
     }
     else
     {
       if (emptyFlt == L7_FALSE)
       {
         /*OSAPI_STRNCAT(interfaceListFor, pStrInfo_common_CrLf);
         OSAPI_STRNCAT(interfaceListFor, interfaceListFilt);*/
       }
     }
 
     /* Forwarding Interfaces*/
     memset(listFor2, 0, sizeof(listFor2));
     memset(interfaceListFor2, 0, sizeof(interfaceListFor2));
     memset (stat, 0,sizeof(stat));
     rc = usmDbConvertMaskToList(&forwardingMask, listFor2, &numFwd2);
 
     OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 0, L7_NULLPTR, interfaceListFor2, pStrInfo_common_Fwd_2);
 
     if (numFwd2 <= 0)
     {
       emptyFwd = L7_TRUE;
     }
     else
     {emptyFwd = L7_FALSE;}
 
             if((numFwd > 0) && (numFlt == 0))
               ewsTelnetPrintf (ewsContext, "%-10s", interfaceListFor);
             else if((numFlt >0) && (numFwd ==0))
               ewsTelnetPrintf (ewsContext, "%-10s", interfaceListFilt);
             else if ((numFwd > 0 ) && (numFlt > 0))
             {
               ewsTelnetPrintf (ewsContext, "%-10s", interfaceListFor);
               fwdFiltBoth = L7_TRUE;
               fwdFiltBothFlag = L7_TRUE;
             }
             else
               ewsTelnetPrintf (ewsContext, "%-10s",pStrInfo_common_Space);
 
            if(numFwd2 >0)
              ewsTelnetPrintf (ewsContext, "%s", interfaceListFor2);
            else
               ewsTelnetPrintf (ewsContext, "%s",pStrInfo_common_Space);
 
 
            maxDisplaylength = 9;
            displayItems = maxDisplaylength/7;
            i=1;
            j = 1;
       
            osapiSnprintf(temp, sizeof(temp),"\r\n%-58s",pStrInfo_common_Space);
            while((numFwd > 0) || (numFwd2 > 0) || (numFlt > 0))
            {
              bzero(interfaceListSrc, sizeof(interfaceListSrc));
              bzero(interfaceListDest, sizeof(interfaceListDest));
              flag=L7_FALSE;
              for(k = 1; k <= displayItems; k++)
              {
                              
 
                if(fwdFiltBoth == L7_TRUE)
                {
                  if(listFor[j] != 0)
                  {
                    rc = usmDbUnitSlotPortGet(listFor[j], &u, &s, &p);
                    if ( rc == L7_SUCCESS )
                    {
                      osapiSnprintf(stat, sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
                    }
                    flag=L7_TRUE;
                    numFwd--;
                    OSAPI_STRNCAT(interfaceListSrc, stat);
                    if (listFor[j+1] != 0)
                    {
                      OSAPI_STRNCAT(interfaceListSrc, ",");
                    }
                  }
                  else if(listFilt[j] != 0)
                  {
                    if(fwdFiltBothFlag == L7_TRUE)
                    {
                      osapiStrncpySafe(interfaceListSrc, interfaceListFilt, sizeof(interfaceListSrc));
                      fwdFiltBothFlag = L7_FALSE;
                    }
                    else
                    {
                      rc = usmDbUnitSlotPortGet(listFilt[j], &u, &s, &p);
                      if ( rc == L7_SUCCESS )
                      { 
                        osapiSnprintf(stat, sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
                      }
                      flag=L7_TRUE; 
                      numFlt--;
                      OSAPI_STRNCAT(interfaceListSrc, stat);
                      if (listFilt[j+1] != 0)
                      {
                        OSAPI_STRNCAT(interfaceListSrc, ",");
                      }
                    }
                  }
                }
                else if (listFor[i] != 0)
                {
                  rc = usmDbUnitSlotPortGet(listFor[i], &u, &s, &p);
                  if ( rc == L7_SUCCESS )
                  {
                    osapiSnprintf(stat, sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
                  }
                  flag=L7_TRUE;
                  numFwd--;
                  OSAPI_STRNCAT(interfaceListSrc, stat);
                  if (listFor[i+1] != 0)
                  {
                    OSAPI_STRNCAT(interfaceListSrc, ",");
                  }
                }
                else if (listFilt[i] != 0)
                {
                  rc = usmDbUnitSlotPortGet(listFilt[i], &u, &s, &p);
                  if ( rc == L7_SUCCESS )
                  {
                    osapiSnprintf(stat, sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
                  }
                  rc = usmDbUnitSlotPortGet(listFor2[i], &u, &s, &p);
                  if ( rc == L7_SUCCESS )
                  {
                    osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(u, s, p));
                  }
                  flag=L7_TRUE;
                  numFlt--;
                  OSAPI_STRNCAT(interfaceListSrc, stat);
                  if (listFilt[i+1] != 0)
                  {
                    OSAPI_STRNCAT(interfaceListSrc, ",");
                  }
                }
 
                if(listFor2[i] != 0)
                {
                  memset (stat, 0,sizeof(stat));
                  if (usmDbUnitSlotPortGet(listFor2[i], &u, &s, &p) == L7_SUCCESS)
                  { 
                    osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(u, s, p));
                  } 
                  numFwd2--;
                  flag=L7_TRUE;
                  OSAPI_STRNCAT(interfaceListDest, stat);
                  if (listFor2[i+1]!= 0)
                  {
                    OSAPI_STRNCAT(interfaceListDest, ",");
                  }
                }
 
                i++;
                j++;
                if((numFwd2 ==0) && (numFlt ==0) && (numFwd ==0))
                  break;
              }
 
              while(strlen(interfaceListSrc) < (maxDisplaylength+1))
                OSAPI_STRNCAT(interfaceListSrc, " ");
              
              OSAPI_STRNCAT(interfaceListSrc, interfaceListDest);
              if(flag==L7_TRUE)
                ewsTelnetWrite(ewsContext,temp);
              ewsTelnetWrite(ewsContext,interfaceListSrc);
              lineCount++;
 
            }
 
 
     cliSyntaxBottom(ewsContext);
 
     memset(&compInfo, 0, sizeof(usmdbMfdbUserInfo_t));
     if (usmDbMfdbEntryGetNext(unit, entry.usmdbMfdbVidMac, &nextProt, &compInfo) != L7_SUCCESS)
     {
       return cliPrompt(ewsContext);
     }
   }
 
   cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
   cliAlternateCommandSet(pStrInfo_base_ShowMacAddrTblMcastCmd);
   return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
 }

/*********************************************************************
*
* @purpose Displays Multicast Forwarding Database Statistics
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
* @notes     Here we only display the visible interfaces.
*
* @cmdsyntax    show mac-address-table stats
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowMacAddressTableStats(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_uint32 val;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowMfdbStats_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext, pStrInfo_common_MaxMfdbTblEntries);
   if (usmDbMfdbMaxTableEntriesGet(unit, &val) == L7_SUCCESS)
   {
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  cliFormat(ewsContext, pStrInfo_common_MostMfdbEntriesSinceLastReset);
   if (usmDbMfdbMostEntriesGet(unit, &val) == L7_SUCCESS)
   {
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  cliFormat(ewsContext, pStrInfo_common_CurrentEntries);
   if (usmDbMfdbCurrEntriesGet(unit, &val) == L7_SUCCESS)
   {
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display port mirroring information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes

* @cmdsyntax  show monitor session {session-id}
*
* @cmdhelp Display port monitoring info.
*
* @cmddescript
*   Show current port monitoring information.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowMonitor(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 argSessionNum = 1;
   L7_INTF_MASK_t  srcIfMask;
   L7_uint32 sessionNum, mode, destIntf;
   /* L7_uint32 oprMode; */
   L7_uint32 listSrcPorts[L7_MAX_INTERFACE_COUNT], numPorts;
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 typeString[L7_CLI_MAX_STRING_LENGTH];
   L7_int32 u;
   L7_int32 s;
   L7_int32 p;
   L7_uint32 i;
   L7_BOOL  first,flagType;
   L7_MIRROR_DIRECTION_t type;

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();
   type = L7_MIRROR_BIDIRECTIONAL;
   flagType = L7_FALSE;

   if (numArg != 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowMonitor_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

   if (cliConvertTo32BitUnsignedInteger(argv[index + argSessionNum],&sessionNum) != L7_SUCCESS)
   {
      /* Error is displayed by parser */
      return cliPrompt(ewsContext);
   }

   if (usmDbSwPortMonitorModeGet(unit, sessionNum, &mode) == L7_NOT_EXIST)
   {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgMirroringInvalidSessionId, 1, L7_MIRRORING_MAX_SESSIONS);
  }
  if((cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
          L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID)) == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_SessionIdAdminModeProbePortMirroredPortType);
      ewsTelnetWrite(ewsContext,"\r\n----------   ----------   ----------   -------------   -----");
   }
   else
   {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_SessionIdAdminModeProbePortMirroredPort);
      ewsTelnetWrite(ewsContext,"\r\n----------   ----------   ----------   -------------");
   }

  osapiSnprintf(buf, sizeof(buf), "\r\n%d",sessionNum);
  ewsTelnetPrintf (ewsContext, "%-13s  ", buf);
  ewsTelnetPrintf(ewsContext,"%-13s",strUtilEnableDisableGet(mode,pStrInfo_common_Dsbl_1));

   usmDbSwPortMonitorDestPortGet(unit, sessionNum, &destIntf);
   if (destIntf != 0)
   {
      rc = usmDbUnitSlotPortGet(destIntf, &u, &s, &p);
      if (rc == L7_SUCCESS)
      {
      osapiSnprintf(stat,sizeof(stat),"%-13s",cliDisplayInterfaceHelp(u, s, p));
      }
   }
   else
   {
    osapiSnprintf (stat,sizeof(stat),"%-13s", pStrInfo_common_EmptyString);
   }
   ewsTelnetWrite(ewsContext,stat);

   usmDbSwPortMonitorSourcePortsGet(unit, sessionNum, &srcIfMask);
   memset(listSrcPorts, 0, sizeof(listSrcPorts));
   rc = usmDbConvertMaskToList(&srcIfMask, listSrcPorts, &numPorts);

   first = L7_TRUE;
   for (i=1; i <= numPorts; i++)
   {
       if((cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
              L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID)) == L7_TRUE)
       {
          rc = usmDbMirrorSourcePortDirectionGet(sessionNum,listSrcPorts[i], &type);
          if(rc == L7_SUCCESS)
          {
             if(type == L7_MIRROR_INGRESS)
        {
          osapiSnprintf(typeString,sizeof(typeString),"%-3s%-5s","",pStrInfo_base_IngressDirection);
        }
             else if(type == L7_MIRROR_EGRESS)
        {
          osapiSnprintf(typeString,sizeof(typeString),"%-3s%-5s","",pStrInfo_common_EgressDirection);
        }
        else
        {
          osapiSnprintf(typeString,sizeof(typeString),"%-3s%-5s","",pStrInfo_base_BothDirections);
        }
             flagType = L7_TRUE;
          }
       }

      if (usmDbUnitSlotPortGet(listSrcPorts[i], &u, &s, &p) == L7_SUCCESS)
      {
      osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
         if (first == L7_TRUE)
         {
            first = L7_FALSE;
        ewsTelnetPrintf (ewsContext, "%-13s",stat);
	    if(flagType == L7_TRUE)
	    {
	       ewsTelnetWrite(ewsContext,typeString);
	       flagType = L7_FALSE;
	    }
         }
         else
         {
        ewsTelnetPrintf (ewsContext, "\r\n%-39s"," ");
        ewsTelnetPrintf (ewsContext, "%-13s",stat);
	    if(flagType == L7_TRUE)
	    {
	       ewsTelnetWrite(ewsContext,typeString);
	       flagType = L7_FALSE;
	    }
         }
      }
   }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display port information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument checking, only through interface 16 currently
*
* @cmdsyntax in stacking env:  show port {<unit/slot/port> | all}
* @cmdsyntax in non-stacking env:  show port {<slot/port> | all}
*
* @cmdhelp Display port mode and settings; display port status.
*
* @cmddescript
*   This command will show current port settings and status.
*   The switch is shipped from the factory with default port settings that
*   allow it to automatically determine the port type and speed.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_int32 u;
   L7_int32 s;
   L7_int32 p;
   L7_int32 slot;
   L7_int32 port;
   L7_uint32 nextInterface;
   static L7_uint32 interface;
   L7_uint32 val;
   L7_uint32 val2;
   L7_uint32 val3;
   L7_uint32 argSlotPort=1;
   L7_BOOL all = L7_FALSE;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */
   L7_INTF_STATES_t state;
   L7_uint32 count;
   L7_INTF_TYPES_t sysIntfType;
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 channelDesc[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 lacpState;
   L7_BOOL intfStatus;

  memset (buf, 0, sizeof(buf));
  memset (channelDesc, 0, sizeof(channelDesc));
  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowPort_1, cliSyntaxInterfaceHelp());
  }

  if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
  {
    all = L7_TRUE;
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
  }
  else
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
       ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliPrompt(ewsContext);
    }
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
     {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliPrompt(ewsContext);
      }

      rc = usmDbIntfTypeGet(interface, &sysIntfType);
      if (rc != L7_SUCCESS || sysIntfType == L7_CPU_INTF ||
          sysIntfType == L7_LOOPBACK_INTF || sysIntfType == L7_TUNNEL_INTF)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
   }

   if (cliGetCharInputID() != CLI_INPUT_EMPTY)
   {
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         interface = 0;
         ewsCliDepth(ewsContext, cliPrevDepthGet(),argv);
         return cliPrompt(ewsContext);
      }
   }

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 15, 2, L7_NULLPTR, ewsContext,pStrInfo_base_AdminPhyPhyLinkLinkLacpFlow);
    ewsTelnetWriteAddBlanks (1, 0, 1, 2, L7_NULLPTR, ewsContext,pStrInfo_base_IntfTypeModeModeStatusStatusTrapModeMode);
    ewsTelnetWrite(ewsContext,"\r\n------ ------ --------- ---------- ---------- ------ ------- ------ -------");
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 15, 0, L7_NULLPTR, ewsContext,pStrInfo_base_AdminPhyPhyLinkLinkLacpActor);
    ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_IntfTypeModeModeStatusStatusTrapModeTimeout);
    ewsTelnetWrite(ewsContext,"\r\n------ ------ --------- ---------- ---------- ------ ------- ------ --------");
  }


  for (count=0; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    if (interface == 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
      }

      if (usmDbIntfTypeGet(interface, &sysIntfType) == L7_SUCCESS &&
          sysIntfType != L7_CPU_INTF &&
          sysIntfType != L7_LOOPBACK_INTF &&
          sysIntfType != L7_TUNNEL_INTF)
      {
         rc = usmDbUnitSlotPortGet(interface, &u, &s, &p);
         if (rc == L7_SUCCESS)
         {
            rc = usmDbIfSpecialPortTypeGet(unit, interface, &val);
            switch (val)                                                        /* " ","Port-Channel Mbr","Mon","N/A" */
            {
            case L7_PORT_NORMAL:
            osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_EmptyString);
               break;
            case L7_TRUNK_MEMBER:
            osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_LagMbr);
               break;
            case L7_MIRRORED_PORT:
            osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_MirrorShow);
               break;
            case L7_PROBE_PORT:
            osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_Probe);
               break;
            case L7_L2TUNNEL_PORT:
            osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_WsL2Tunnel);
               break;
            default:
            osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_EmptyString);
            }
            ewsTelnetWrite(ewsContext,stat);
         }

         rc = usmDbIfAdminStateGet(u, interface, &val);
         switch (val)                                                           /* val = disable, enable, delete */
         {
            case L7_DISABLE:
              osapiSnprintf(stat, sizeof(stat),"%-9s ",pStrInfo_common_Dsbl_1);
              break;
            case L7_ENABLE:
              osapiSnprintf(stat, sizeof(stat),"%-9s ",pStrInfo_common_Enbl_1);
              break;
            case L7_DIAG_DISABLE:
              osapiSnprintf(stat, sizeof(stat),"%-9s ", pStrInfo_base_DiagDsbl);
              break;
            default:
              osapiSnprintf(stat, sizeof(stat),"%-9s ",pStrInfo_common_Enbl_1);
         }
         ewsTelnetWrite(ewsContext,stat);

         memset (stat, 0,sizeof(stat));
         rc = usmDbIfAutoNegAdminStatusGet(u, interface, &val);
         if (val == L7_ENABLE)
         {
            /* we are in auto-negotiate */
            osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_Auto);
         }
         else
         {
            /* print out the correct speed */
            rc = usmDbIfSpeedGet(u, interface, &val);
            switch (val)
            {                                                                       /* it is missing entry number 6 for value 100FX Half.*/
              /* "---","Auto","100 Half, 100 Full, 10 Half,"10 Full","100FX Half","100FX Full","1000 Full" */
              case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_Auto);
                break;
              case L7_PORTCTRL_PORTSPEED_HALF_100TX:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100Half);
                break;
              case L7_PORTCTRL_PORTSPEED_FULL_100TX:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100Full);
                break;
              case L7_PORTCTRL_PORTSPEED_HALF_10T:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10Half);
                break;
              case L7_PORTCTRL_PORTSPEED_FULL_10T:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10Full);
                break;
              case L7_PORTCTRL_PORTSPEED_FULL_100FX:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100FxFull);
                break;
              case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed1000Full);
                break;
              /* PTin added: Speed 2.5G */
              case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed2500Full);
                break;
              /* PTin end */
              case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10GigFull);
                break;
              /* PTin added: Speed 40G */
              case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed40GigFull);
                break;
              /* PTin added: Speed 100G */
              case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100GigFull);
                break;
              /* PTin end */
              default:
                osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_EmptyString);
                break;
            }
         }
         ewsTelnetWrite(ewsContext,stat);

         memset (stat, 0,sizeof(stat));
         rc = usmDbIfHighSpeedGet(u, interface, &val);
         rc = usmDbIfOperStatusGet(u, interface, &val2);
         rc = usmDbIntfStatusGet(interface, &val3);
         if ((val3!=L7_INTF_ATTACHED) || (val2==L7_DOWN))
         {
             osapiSnprintf (stat,sizeof(stat),"%-10s ", pStrInfo_common_EmptyString);     /*for a port that is down blanks should be shown here.*/
         }
         else
         {
             switch (val)
             {                                                                         /* it is missing entry number 6 for value 100FX Half.*/
                  /* "---","Auto","100 Half, 100 Full, 10 Half,"10 Full","100FX Half","100FX Full","1000 Full" */
                  case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_Auto);
                    break;
                  case L7_PORTCTRL_PORTSPEED_HALF_100TX:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100Half);
                    break;
                  case L7_PORTCTRL_PORTSPEED_FULL_100TX:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100Full);
                    break;
                  case L7_PORTCTRL_PORTSPEED_HALF_10T:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10Half);
                    break;
                  case L7_PORTCTRL_PORTSPEED_FULL_10T:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10Full);
                    break;
                  case L7_PORTCTRL_PORTSPEED_FULL_100FX:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100FxFull);
                    break;
                  case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed1000Full);
                    break;
                  /* PTin added: Speed 2.5G */
                  case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed2500Full);
                    break;
                  /* PTin end */
                  case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10GigFull);
                    break;
                 /* PTin added: Speed 40G */
                 case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
                   osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed40GigFull);
                   break;
                 /* PTin added: Speed 100G */
                 case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
                   osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100GigFull);
                   break;
                 /* PTin end */
                  default:
                    osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_EmptyString);
                    break;
             }
         }
         ewsTelnetWrite(ewsContext,stat);

         memset (stat, 0,sizeof(stat));
         rc = usmDbIntfStatusGet(interface, &val);
         if (rc != L7_SUCCESS)
         {
             osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrErr_common_Err);
         }
         else
         {
            state = (L7_INTF_STATES_t)val;
            if (state == L7_INTF_ATTACHED)
            {
               rc = usmDbIfOperStatusGet(u, interface, &val);
               switch (val)                                                        /* val = up or down */
               {
               case L7_DOWN:
              osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Down_1);
                  break;
               case L7_UP:
              osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Up);
                  break;
               default:
              osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Down_1);
               }
            }
            else
            {
              osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Detach);
            }
         }
          ewsTelnetWrite(ewsContext,stat);

          memset (stat, 0,sizeof(stat));
          rc = usmDbIfLinkUpDownTrapEnableGet(u, interface, &val);
          ewsTelnetPrintf (ewsContext, "%-7s ",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));

          /* LACP Mode */
          memset (stat, 0, sizeof(stat));
          rc = usmDbDot3adAggPortLacpModeGet(u, interface, &val);
         if (rc == L7_SUCCESS)
         {
              osapiSnprintf(stat,sizeof(stat),"%-6s ",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));
         }
         else
         {
            /* The usmDbDot3adAggPortLacpModeGet returns failure for lag interfaces
             * so check if this interface is a lag interface, if so print N/A
            */
            if (usmDbDot3adIsConfigured(u, interface) == L7_TRUE)
            {
                osapiSnprintf(stat,sizeof(stat), "%-6s ", pStrInfo_common_NotApplicable);
            }
            else
            {
               /* The call failed so printf the default */
                osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Enbl_1);
            }
         }
         ewsTelnetWrite(ewsContext,stat);

         if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
         {
            /* Flow Mode */
             memset (stat, 0, sizeof(stat));
            rc = usmDbIfFlowCtrlModeGet(u, interface, &val);
            ewsTelnetPrintf (ewsContext, "%s",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));
         }
		 else
		 {
         rc = usmDbDot3adAggPortActorAdminStateGet(unit, interface, (L7_uchar8 *) &lacpState);
         if (rc == L7_SUCCESS)
         {

            if ((L7_uchar8) (lacpState & DOT3AD_STATE_LACP_TIMEOUT))
            {
                osapiStrncpySafe(buf, pStrInfo_base_Short_1, sizeof(buf));
            }
            else
            {
              osapiStrncpySafe(buf, pStrInfo_base_Long, sizeof(buf));
            }

            if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
            {
              ewsTelnetPrintf (ewsContext, "\r\n%-59s", pStrInfo_common_EmptyString);
              ewsTelnetPrintf (ewsContext, "%7s/%-5s", pStrInfo_base_Actor, buf);
            }
            else
            {
              ewsTelnetPrintf (ewsContext, "%s", buf);
            }

         }
		 else
         {
            /* The usmDbDot3adAggPortActorAdminStateGet returns failure for LAG interfaces
             * so check if this interface is a LAG interface, if so print N/A
            */
             ewsTelnetPrintf (ewsContext, "%s ", pStrInfo_common_NotApplicable);
         }
      }
      }


      if (!all)
      {
         interface = 0;
      }
      else
      {
          intfStatus = L7_FALSE;
          while(usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
          {
              interface = nextInterface;
              if (usmDbIntfTypeGet(interface, &sysIntfType) == L7_SUCCESS &&
                      sysIntfType != L7_CPU_INTF &&
                      sysIntfType != L7_LOOPBACK_INTF &&
                      sysIntfType != L7_WIRELESS_INTF &&
                      sysIntfType != L7_TUNNEL_INTF &&
                      sysIntfType != L7_VLAN_PORT_INTF)     /* PTin added: virtual ports */
                {
                    intfStatus = L7_TRUE;
                    break;
                }
                else
                {
                    continue;
                }
          }

          if (intfStatus == L7_FALSE)
          {
                interface = 0;
          }
      }
   }

   if (interface != 0)
   {
      cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
      cliSyntaxBottom(ewsContext);
      cliAlternateCommandSet(pStrInfo_base_ShowPortAll);
      return pStrInfo_common_Name_2;
   }
   else
   {
    return cliSyntaxReturnPrompt (ewsContext, "");
   }

}

/*********************************************************************
*
* @purpose  display switch options such as broadcast storm recovery mode and flow control
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument checking
*
* @cmdsyntax  show storm control
*
* @cmdhelp Display parameters that apply to the switch.
*
* @cmddescript
*   the switch allows you to set a time after which the address will timeout,
*   and to enable/disable broadcast storm recovery and 802.3x flow control.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowStormControl(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_char8 buf1[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 buf2[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 buf3[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 buf4[L7_CLI_MAX_STRING_LENGTH];
   L7_int32 u;
   L7_int32 s;
   L7_int32 p;
   L7_int32 slot;
   L7_int32 port;
   L7_uint32 nextInterface;
   static L7_uint32 interface;
   L7_uint32 val;
   L7_int32 threshold;
   L7_uint32 argSlotPort=1;
   L7_int32 retVal;
   L7_BOOL all = L7_FALSE;
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uint32 count;
   L7_RATE_UNIT_t rate_unit;

   cliSyntaxTop(ewsContext);
   cliCmdScrollSet( L7_FALSE);

   numArg = cliNumFunctionArgsGet();

   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   if (numArg == 0)
   {
      if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
      {
      cliFormat(ewsContext,pStrInfo_base_StormCntrlBcastMode);
         rc=usmDbSwDevCtrlBcastStormModeGet(unit, &val);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
      }

      cliFormat(ewsContext,pStrInfo_base_StormCntrlBcastLvl);
         rc=usmDbSwDevCtrlBcastStormThresholdGet(unit, &val, &rate_unit);
         if (rc == L7_SUCCESS)
         {
             if (rate_unit == L7_RATE_UNIT_PERCENT) 
             {
               ewsTelnetPrintf (ewsContext, "%d percent",val);
             }
             else if (rate_unit == L7_RATE_UNIT_PPS) 
             {
               ewsTelnetPrintf (ewsContext, "%d pps",val);
             }
             else
             {
               ewsTelnetPrintf (ewsContext, "%d",val);
             }
         }
      }

      if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
      {
      cliFormat(ewsContext,pStrInfo_base_StormCntrlMcastMode);
         rc=usmDbSwDevCtrlMcastStormModeGet(unit, &val);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
      }

      cliFormat(ewsContext,pStrInfo_base_StormCntrlMcastLvl);
         rc=usmDbSwDevCtrlMcastStormThresholdGet(unit, &val, &rate_unit);
         if (rc == L7_SUCCESS)
         {
             if (rate_unit == L7_RATE_UNIT_PERCENT) 
             {
               ewsTelnetPrintf (ewsContext, "%d percent",val);
             }
             else if (rate_unit == L7_RATE_UNIT_PPS) 
             {
               ewsTelnetPrintf (ewsContext, "%d pps",val);
             }
             else
             {
               ewsTelnetPrintf (ewsContext, "%d",val);
             }
         }
      }

      if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
      {
      cliFormat(ewsContext,pStrInfo_base_StormCntrlUcastMode);
         rc=usmDbSwDevCtrlUcastStormModeGet(unit, &val);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
      }

      cliFormat(ewsContext,pStrInfo_base_StormCntrlUcastLvl);
         rc=usmDbSwDevCtrlUcastStormThresholdGet(unit, &val, &rate_unit);
         if (rc == L7_SUCCESS)
         {
             if (rate_unit == L7_RATE_UNIT_PERCENT) 
             {
               ewsTelnetPrintf (ewsContext, "%d percent",val);
             }
             else if (rate_unit == L7_RATE_UNIT_PPS) 
             {
               ewsTelnetPrintf (ewsContext, "%d pps",val);
             }
             else
             {
               ewsTelnetPrintf (ewsContext, "%d",val);
             }
         }
      }

      if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_FALSE)
      {
      cliFormat(ewsContext,pStrInfo_base_802Dot3XFlowCntrlMode);
      rc = usmDbSwDevCtrlFlowControlModeGet(unit, &val);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "%s ",strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
      }
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
   }

   else if (numArg > 1 || ( (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_FALSE &&
                             usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_FALSE &&
                             usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_FALSE &&
                             usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) != L7_TRUE)
                            && (numArg != 0) ))
   {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowStormCfg, cliSyntaxInterfaceHelp());
  }
  else if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
  {
    all = L7_TRUE;
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
  }
  else
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) != L7_SUCCESS)
    {
      if (retVal == -2)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,pStrErr_common_InvalidLag, cliDisplayInterfaceHelp(unit, slot, port));
      }
      else if (retVal == -1)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      interface = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(),argv);
      return cliPrompt(ewsContext);
    }
  }

  memset(buf1,0,sizeof(buf1));
  memset(buf2,0,sizeof(buf2));
  memset(buf3,0,sizeof(buf3));

  osapiSnprintf(buf1, sizeof(buf1),"\r\n      ");
  osapiSnprintfAddBlanks (1, 0, 1, 1, L7_NULLPTR, buf2, sizeof(buf2),pStrInfo_common_Intf_3);
  osapiSnprintf(buf3, sizeof(buf3),"\r\n------");

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatPrintfAddBlanks (0, 0, 2, 2, L7_NULLPTR, buf1, sizeof(buf1), pStrInfo_base_BcastBcast);
    osapiStrncatPrintfAddBlanks (0, 0, 2, 2, L7_NULLPTR, buf2, sizeof(buf2), pStrInfo_base_ModeLvl);
    osapiSnprintf(buf4, sizeof(buf4), "%s%s",buf4," ------- --------");
    OSAPI_STRNCAT(buf3, buf4);
  }

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatPrintfAddBlanks (0, 0, 2, 2, L7_NULLPTR, buf1, sizeof(buf1), pStrInfo_base_McastMcast);
    osapiStrncatPrintfAddBlanks (0, 0, 2, 2, L7_NULLPTR, buf2, sizeof(buf2), pStrInfo_base_ModeLvl);
    osapiSnprintf(buf4, sizeof(buf4), "%s%s",buf4," ------- --------");
    OSAPI_STRNCAT(buf3, buf4);
  }

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatPrintfAddBlanks (0, 0, 2, 2, L7_NULLPTR, buf1, sizeof(buf1), pStrInfo_base_UcastUcast);
    osapiStrncatPrintfAddBlanks (0, 0, 2, 2, L7_NULLPTR, buf2, sizeof(buf2), pStrInfo_base_ModeLvl);
    osapiSnprintf(buf4, sizeof(buf4), "%s%s",buf4," ------- --------");
    OSAPI_STRNCAT(buf3, buf4);
  }

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatPrintfAddBlanks (0, 0, 2, 1, L7_NULLPTR, buf1, sizeof(buf1), pStrInfo_base_Flow);
    osapiStrncatPrintfAddBlanks (0, 0, 2, 1, L7_NULLPTR, buf2, sizeof(buf2), pStrInfo_common_Mode_1);
    osapiSnprintf(buf4, sizeof(buf4), "%s%s",buf4," ------");
    OSAPI_STRNCAT(buf3, buf4);
   }

   ewsTelnetWrite(ewsContext,buf1);
   ewsTelnetWrite(ewsContext,buf2);
   ewsTelnetWrite(ewsContext,buf3);

   for (count=0; count < CLI_MAX_SCROLL_LINES-6; count++)
   {
      if (interface == 0)
      {
      return cliSyntaxReturnPrompt (ewsContext, "");
      }
      if ((usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS)
          && (usmDbPolicyIsValidIntf(unit, interface)))
      {
         rc = usmDbUnitSlotPortGet(interface, &u, &s, &p);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "\r\n%-6s ", cliDisplayInterfaceHelp(u, s, p));
      }

         if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
         {
            rc = usmDbSwDevCtrlBcastStormModeIntfGet(interface, &val);
            if (rc == L7_SUCCESS)
            {
          ewsTelnetPrintf (ewsContext, "%-7s ",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));
            }

            rc = usmDbSwDevCtrlBcastStormThresholdIntfGet(interface, &threshold, &rate_unit);
            if (rc == L7_SUCCESS)
            {
               if (rate_unit == L7_RATE_UNIT_PERCENT) 
               {
                  ewsTelnetPrintf (ewsContext, "%7d%% ",threshold);
               }
               else if (rate_unit == L7_RATE_UNIT_PPS) 
               {
                  ewsTelnetPrintf (ewsContext, "%8d ",threshold);
               }
            }
         }

         if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
         {
            rc = usmDbSwDevCtrlMcastStormModeIntfGet(interface, &val);
            if (rc == L7_SUCCESS)
            {
          ewsTelnetPrintf (ewsContext, "%-7s ",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));
            }

            rc = usmDbSwDevCtrlMcastStormThresholdIntfGet(interface, &threshold, &rate_unit);
            if (rc == L7_SUCCESS)
            {
               if (rate_unit == L7_RATE_UNIT_PERCENT) 
               {
                  ewsTelnetPrintf (ewsContext, "%7d%% ",threshold);
               }
               else if (rate_unit == L7_RATE_UNIT_PPS) 
               {
                  ewsTelnetPrintf (ewsContext, "%8d ",threshold);
               }
            }
         }

         if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
         {
            rc = usmDbSwDevCtrlUcastStormModeIntfGet(interface, &val);
            if (rc == L7_SUCCESS)
            {
          ewsTelnetPrintf (ewsContext, "%-7s ",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));
            }

            rc = usmDbSwDevCtrlUcastStormThresholdIntfGet(interface, &threshold, &rate_unit);
            if (rc == L7_SUCCESS)
            {
               if (rate_unit == L7_RATE_UNIT_PERCENT) 
               {
                  ewsTelnetPrintf (ewsContext, "%7d%% ",threshold);
               }
               else if (rate_unit == L7_RATE_UNIT_PPS) 
               {
                  ewsTelnetPrintf (ewsContext, "%8d ",threshold);
               }
            }
         }

         if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
         {
            rc = usmDbIfFlowCtrlModeGet(unit, interface, &val);
            if (rc == L7_SUCCESS)
            {
          ewsTelnetPrintf (ewsContext, "%-7s",strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
            }
         }
      }
      if (!all)
      {
         interface = 0;
      }
      else
      {
         /* Find next visible interface.
         */
         do
         {
            if ((usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS) && (usmDbVisibleInterfaceCheck(unit, nextInterface, &retVal) == L7_SUCCESS))
            {
               interface = nextInterface;
               break;   
            }
            else
            {
               interface = 0;
            }
         } while (interface != 0);
      }
   }

   if (interface != 0)
   {
      cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
      cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_base_ShowStormCntrlAll);
    return pStrInfo_common_Name_2;
   }
   else
   {
    return cliSyntaxReturnPrompt (ewsContext, "");
   }
}

/*********************************************************************
*
* @purpose  display Generic Attribute Registration Protocol Interface parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show gmrp configuration { <slot/port> | all }
*
* @cmdhelp  Display Generic Attribute Registration Protocol Interface parameters.
*
* @cmddescript  Show Generic Attributes Registration Protocol (GARP) interface
*               configuration parameters.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowGmrpConfiguration(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_BOOL all = L7_TRUE;
   static L7_uint32 interface = 0;
   static L7_uint32 intfValid;
   L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 i;
   L7_uint32 count;
   L7_uint32 nextInterface = 0;
   L7_uint32 intSlot;
   L7_uint32 intPort;
   L7_uint32 val;
   L7_uint32 portGmrpMode = L7_DISABLE;
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strJoinTime[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strLeaveTime[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strPortGmrpMode[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strLeaveAllTime[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 argSlotPort=1;
   L7_uint32 unit = 1;
   L7_uint32 numArg;        /* New variable Added */
   L7_RC_t rc = L7_FAILURE;

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   cliCmdScrollSet( L7_FALSE);
   if (cliGetCharInputID() != CLI_INPUT_EMPTY)
   {                                                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
      /* Error Checking for Number of Arguments */
      if (numArg != 1)
      {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowGarpIntf, cliSyntaxInterfaceHelp());
    }

    if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
    {
      intfValid = L7_FALSE;
      /* get switch ID based on presence/absence of STACKING package */
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }

      all = L7_TRUE;
      intfValid = L7_TRUE;
      if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
      {
        /* No valid interfaces! */
        interface = 0;
        intfValid = L7_FALSE;
      }
    }
    else
    {
      all = L7_FALSE;
      intfValid = L7_FALSE;
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort)) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
      if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

      if (usmDbGarpIsValidIntf(unit, interface) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      intfValid = L7_TRUE;
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 16, 4, L7_NULLPTR, ewsContext,pStrInfo_base_JoinLeaveLeaveallPort);
  ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext,pStrInfo_base_IntfTimerTimerTimerGmrpMode);
  ewsTelnetWriteAddBlanks (1, 0, 13, 13, L7_NULLPTR, ewsContext,pStrInfo_base_CentisecsCentisecsCentisecs);
  ewsTelnetWrite(ewsContext,"\r\n-----------  -----------  -----------  -----------  -----------\r\n");

  count = 0;
  while ((intfValid == L7_TRUE) && (count < CLI_MAX_SCROLL_LINES-8))
  {
    if (usmDbGarpIsValidIntf(unit, interface) == L7_TRUE)
    {
      /* First pass through, interface has already been verified */
      if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
        osapiSnprintf(strSlotPort,sizeof(strSlotPort),"%-13.11s",buf);
      }

      if (usmDbGarpJoinTimeGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),"%d",val);
        osapiSnprintf(strJoinTime,sizeof(strJoinTime),"%-13.7s",buf);
      }

      if (usmDbGarpLeaveTimeGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),"%d",val);
        osapiSnprintf(strLeaveTime,sizeof(strLeaveTime),"%-13.7s",buf);
      }

      if (usmDbGarpLeaveAllTimeGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),"%d",val);
        osapiSnprintf(strLeaveAllTime,sizeof(strLeaveAllTime),"%-13.10s",buf);
      }

      memset (strPortGmrpMode, 0,sizeof(strPortGmrpMode));
      if (usmDbGarpGmrpPortEnableGet(unit, interface, &portGmrpMode) == L7_SUCCESS)
      {
        osapiSnprintf(strPortGmrpMode,sizeof(strPortGmrpMode),"%-11.11s",strUtilEnabledDisabledGet(portGmrpMode ,pStrInfo_common_Line));
      }
      else
      {
        osapiSnprintf(strPortGmrpMode,sizeof(strPortGmrpMode),"%-11.11s",pStrInfo_common_Line);
      }

      ewsTelnetWrite(ewsContext,strSlotPort);
      ewsTelnetWrite(ewsContext,strJoinTime);
      ewsTelnetWrite(ewsContext,strLeaveTime);
      ewsTelnetWrite(ewsContext,strLeaveAllTime);
      ewsTelnetWrite(ewsContext,strPortGmrpMode);
      cliSyntaxNewLine(ewsContext);

      count++;

    }   /* end if for interface type check */
    if (!all)
    {
      interface = 0;
      intfValid = L7_FALSE;
    }
    else
    {
      if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
      {
        interface = nextInterface;
        intfValid = L7_TRUE;
      }
      else
      {
        /* No more interfaces */
        interface = 0;
        intfValid = L7_FALSE;
      }
    }   /* end else handling for all */
  }  /* end of while loop */

  if (intfValid == L7_FALSE)
  {
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  cliSyntaxBottom(ewsContext);

  osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
  for (i=1; i<argc; i++)
  {
    OSAPI_STRNCAT(cmdBuf, " ");
    OSAPI_STRNCAT(cmdBuf, argv[i]);
  }
  cliAlternateCommandSet(cmdBuf);

  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose  display Generic Attribute Registration Protocol Interface parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show gvrp configuration {<slot/port> | all}
*
* @cmdhelp  Display Generic Attribute Registration Protocol Interface parameters.
*
* @cmddescript  Show Generic Attributes Registration Protocol (GARP) interface
*               configuration parameters.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowGvrpConfiguration(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_BOOL all = L7_TRUE;
   static L7_uint32 interface = 0;
   static L7_uint32 intfValid;
   L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 i;
   L7_uint32 count;
   L7_uint32 nextInterface = 0;
   L7_uint32 intSlot;
   L7_uint32 intPort;
   L7_uint32 val;
   L7_uint32 portGvrpMode = L7_DISABLE;
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strJoinTime[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strLeaveTime[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strLeaveAllTime[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strPortGmrpMode[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strPortGvrpMode[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 argSlotPort=1;
   L7_uint32 unit = 1;
   L7_uint32 numArg;        /* New variable Added */
   L7_RC_t rc = L7_FAILURE;

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   cliCmdScrollSet( L7_FALSE);
   if (cliGetCharInputID() != CLI_INPUT_EMPTY)
   {                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
      /* Error Checking for Number of Arguments */
      if (numArg != 1)
      {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowGvrpIntf, cliSyntaxInterfaceHelp());
    }

    if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
    {
      intfValid = L7_FALSE;
      /* get switch ID based on presence/absence of STACKING package */
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }

      all = L7_TRUE;
      intfValid = L7_TRUE;
      if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
      {
        /* No valid interfaces! */
        interface = 0;
        intfValid = L7_FALSE;
      }
    }
    else
    {
      all = L7_FALSE;
      intfValid = L7_FALSE;
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort)) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
      if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

      if (usmDbGarpIsValidIntf(unit, interface) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      intfValid = L7_TRUE;
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 16, 4, L7_NULLPTR, ewsContext,pStrInfo_base_JoinLeaveLeaveallPort);
  ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext,pStrInfo_base_IntfTimerTimerTimerGvrpMode);
  ewsTelnetWriteAddBlanks (1, 0, 13, 13, L7_NULLPTR, ewsContext,pStrInfo_base_CentisecsCentisecsCentisecs);
  ewsTelnetWrite(ewsContext,"\r\n-----------  -----------  -----------  -----------  -----------\r\n");

  count = 0;
  while ((intfValid == L7_TRUE) && (count < CLI_MAX_SCROLL_LINES-8))
  {
    if (usmDbGarpIsValidIntf(unit, interface) == L7_TRUE)
    {

      /* First pass through, interface has already been verified */
      if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
        osapiSnprintf(strSlotPort,sizeof(strSlotPort),"%-13.11s",buf);
      }

      if (usmDbGarpJoinTimeGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),"%d",val);
        osapiSnprintf(strJoinTime,sizeof(strJoinTime),"%-13.7s",buf);
      }

      if (usmDbGarpLeaveTimeGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),"%d",val);
        osapiSnprintf(strLeaveTime,sizeof(strLeaveTime),"%-13.7s",buf);
      }

      if (usmDbGarpLeaveAllTimeGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf,sizeof(buf),"%d",val);
        osapiSnprintf(strLeaveAllTime,sizeof(strLeaveAllTime),"%-13.10s",buf);
      }

      memset (strPortGmrpMode, 0,sizeof(strPortGmrpMode));
      memset (strPortGvrpMode, 0,sizeof(strPortGvrpMode));
      if (usmDbQportsEnableGVRPGet(unit, interface, &portGvrpMode) == L7_SUCCESS)
      {
        osapiSnprintf(strPortGvrpMode,sizeof(strPortGvrpMode),"%-11.11s",strUtilEnabledDisabledGet(portGvrpMode ,pStrInfo_common_Line));
      }
      else
      {
        osapiSnprintf(strPortGvrpMode,sizeof(strPortGvrpMode),"%-11.11s",pStrInfo_common_Line);
      }

      ewsTelnetWrite(ewsContext,strSlotPort);
      ewsTelnetWrite(ewsContext,strJoinTime);
      ewsTelnetWrite(ewsContext,strLeaveTime);
      ewsTelnetWrite(ewsContext,strLeaveAllTime);
      ewsTelnetWrite(ewsContext,strPortGvrpMode);
      cliSyntaxNewLine(ewsContext);

    }   /* end if for interface type check */
    if (!all)
    {
      interface = 0;
      intfValid = L7_FALSE;
    }
    else
    {
      if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
      {
        interface = nextInterface;
        intfValid = L7_TRUE;
      }
      else
      {
        /* No more interfaces */
        interface = 0;
        intfValid = L7_FALSE;
      }
    }   /* end else handling for all */
    count++;
  }  /* end of while loop */

  if (intfValid == L7_FALSE)
  {
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  cliSyntaxBottom(ewsContext);

  osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
  for (i=1; i<argc; i++)
  {
    OSAPI_STRNCAT(cmdBuf, " ");
    OSAPI_STRNCAT(cmdBuf, argv[i]);
  }
  cliAlternateCommandSet(cmdBuf);

  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*************Switching System Info Commands added - Start **********/

/*********************************************************************
*
* @purpose  display switch inventory information
*
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
* @cmdsyntax:  show hardware
*
* @cmdhelp Display vital product data.
*
* @cmddescript shows the Read-Only inventory information available for your switch.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowHardware(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
#ifndef FEAT_METRO_CPE_V1_0
   L7_uchar8 dummy[L7_CLI_MAX_STRING_LENGTH];
   L7_BOOL addpkgs = L7_FALSE;
#endif
   L7_RC_t rc;
   L7_uint32 unit = cliGetUnitId();
   L7_uint32 numArg = cliNumFunctionArgsGet();        /* New variable Added */
   cliSyntaxTop(ewsContext);
   cliCmdScrollSet(L7_FALSE);
   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowInventory);
   }

   if (cliIsStackingSupported() == L7_TRUE)
   {
     rc = usmDbUnitMgrMgrNumberGet(&unit);
     if (rc != L7_SUCCESS)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_MgmtUnitId);
     }
   }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
#ifndef L7_CHASSIS
     ewsTelnetPrintf (ewsContext, "\r\n%s: %u", pStrInfo_common_Switch, unit);
     cliSyntaxBottom(ewsContext);
#endif
     cliFormat(ewsContext, pStrInfo_common_SysDesc_1);
     memset (buf, 0, sizeof(buf));
     rc = usmDb1213SysDescrGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormatStringDisplay(ewsContext, buf);
     }
     else
     {
       ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
     }


#ifndef FEAT_METRO_CPE_V1_0
     cliFormat(ewsContext, pStrInfo_base_MachineType);
     memset (buf, 0, sizeof(buf));
     rc = usmDbMachineTypeGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormatStringDisplay(ewsContext, buf);
     }
     else
     {
       ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
     }
#endif

     cliFormat(ewsContext, pStrInfo_base_MachineModel);
     memset (buf, 0, sizeof(buf));
     rc = usmDbMachineModelGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

     cliFormat(ewsContext, pStrInfo_common_ApSerialNum);
     memset (buf, 0, sizeof(buf));
     rc = usmDbSwDevInfoSerialNumGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

#ifndef FEAT_METRO_CPE_V1_0
     cliFormat(ewsContext, pStrInfo_base_FruNum);
     memset (buf, 0, sizeof(buf));
     rc = usmDbFRUNumGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

     cliFormat(ewsContext, pStrInfo_common_ApPartNum);
     memset (buf, 0, sizeof(buf));
     rc = usmDbPartNumGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);
#endif

     cliFormat(ewsContext, pStrInfo_base_MaintenanceLvl);
     memset (buf, 0, sizeof(buf));
     rc = usmDbMaintLevelGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

     cliFormat(ewsContext, pStrInfo_base_Manufacturer);
     memset (buf, 0, sizeof(buf));
     rc = usmDbManufacturerGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

     cliFormat(ewsContext, pStrInfo_base_BurnedInMacAddr);
     memset (stat, 0,sizeof(stat));
     if (cliIsStackingSupported() == L7_TRUE)
     {
       rc = usmDbUnitMgrUnitIdKeyGet(unit, (L7_enetMacAddr_t *)mac);
     }
     else
     {
       rc = usmDbSwDevCtrlBurnedInMacAddrGet(unit, mac);
     }
     ewsTelnetPrintf (ewsContext, "%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

     cliFormat(ewsContext, pStrInfo_common_ApSoftwareVer);
     cliUtilsCodeVersionGet(unit, buf, sizeof(buf));
     ewsTelnetWrite(ewsContext, buf);

#ifdef FEAT_METRO_CPE_V1_0
     cliFormat(ewsContext, pStrInfo_common_ApHardwareVer);
     memset (buf, 0, sizeof(buf));
     rc = usmDbHwVersionGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);
#else
     memset (buf, 0, sizeof(buf));
     rc = usmDbOperSysGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormat(ewsContext, pStrInfo_common_OperatingSys);
       ewsTelnetWrite(ewsContext, buf);
     }

     memset (buf, 0, sizeof(buf));
     rc = usmDbNPDGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormat(ewsContext, pStrInfo_base_NwProcessingDevice);
       ewsTelnetWrite(ewsContext, buf);
     }
     cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

     cliSyntaxBottom(ewsContext);
     cliAlternateCommandSet(pStrInfo_base_ShowHardwareCmd);

     return pStrInfo_common_Name_2;   /* --More-- or (q)uit */

#endif

  }

#ifndef FEAT_METRO_CPE_V1_0
  cliFormat(ewsContext, pStrInfo_base_AdditionalPackages_1);

   osapiSnprintf(dummy,sizeof(dummy),"\r\n%s", "                                                ");

   if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
   {
      addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s",L7_FASTPATH_FLEX_BGP_PACKAGE_STR,dummy);
   }

   if ((usmDbComponentPresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_TRUE) ||
       (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID) == L7_TRUE) ||
       (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID) == L7_TRUE))
   {
      addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_QOS_PACKAGE_STR,dummy);
   }

   if(usmDbComponentPresentCheck(unit, L7_FLEX_MCAST_MAP_COMPONENT_ID) == L7_TRUE)
   {
      addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_MCAST_PACKAGE_STR,dummy);
   }

  if(usmDbComponentPresentCheck(unit, L7_FLEX_IPV6_MAP_COMPONENT_ID) == L7_TRUE)
  {
      addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_IPV6_PACKAGE_STR,dummy);
  }

  if(usmDbComponentPresentCheck(unit, L7_FLEX_IPV6_MGMT_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_IPV6_MGMT_PACKAGE_STR, dummy);
  }

  if(usmDbComponentPresentCheck(unit, L7_FLEX_WIRELESS_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_WIRELESS_PACKAGE_STR,dummy);
  }

  if(usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_STACKING_PACKAGE_STR,dummy);
  }

  if((usmDbComponentPresentCheck(unit, L7_FLEX_METRO_DOT1AD_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_DOT3AH_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_DOT1AG_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_TR069_COMPONENT_ID)  == L7_TRUE))
  {
    addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_METRO_PACKAGE_STR,dummy);
  }

  if(addpkgs == L7_FALSE)
  {
    ewsTelnetPrintf (ewsContext, pStrInfo_common_None_1);
  }
#endif

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To display the details of switch inventory information and
*           switch description information
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
* @cmdsyntax show version
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVersion (EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
#ifndef FEAT_METRO_CPE_V1_0
   L7_uchar8 dummy[L7_CLI_MAX_STRING_LENGTH];
   L7_BOOL addpkgs = L7_FALSE;
#endif
   L7_RC_t rc;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowInventory);
   }

   if (cliIsStackingSupported() == L7_TRUE)
   {
     rc = usmDbUnitMgrMgrNumberGet(&unit);
     if (rc != L7_SUCCESS)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_MgmtUnitId);
     }
   }
   else
   {
     unit = cliGetUnitId();
   }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
     ewsTelnetPrintf (ewsContext, "\r\n%s: %u", pStrInfo_common_Switch, unit);
     cliSyntaxBottom(ewsContext);

     cliFormat(ewsContext, pStrInfo_common_SysDesc_1);
     memset (buf, 0, sizeof(buf));
     rc = usmDb1213SysDescrGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormatStringDisplay(ewsContext, buf);
     }
     else
     {
       ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
     }
 

#ifndef FEAT_METRO_CPE_V1_0
     cliFormat(ewsContext, pStrInfo_base_MachineType);
     memset (buf, 0, sizeof(buf));
     rc = usmDbMachineTypeGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormatStringDisplay(ewsContext, buf);
     }
     else
     {
       ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
     }

#endif

     cliFormat(ewsContext, pStrInfo_base_MachineModel);
     memset (buf, 0, sizeof(buf));
     rc = usmDbMachineModelGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

     cliFormat(ewsContext, pStrInfo_common_ApSerialNum);
     memset (buf, 0, sizeof(buf));
     rc = usmDbSwDevInfoSerialNumGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

#ifndef FEAT_METRO_CPE_V1_0
     cliFormat(ewsContext, pStrInfo_base_FruNum);
     memset (buf, 0, sizeof(buf));
     rc = usmDbFRUNumGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

     cliFormat(ewsContext, pStrInfo_common_ApPartNum);
     memset (buf, 0, sizeof(buf));
     rc = usmDbPartNumGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);
#endif
     cliFormat(ewsContext, pStrInfo_base_MaintenanceLvl);
     memset (buf, 0, sizeof(buf));
     rc = usmDbMaintLevelGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

     cliFormat(ewsContext, pStrInfo_base_Manufacturer);
     memset (buf, 0, sizeof(buf));
     rc = usmDbManufacturerGet(unit, buf);
     ewsTelnetWrite(ewsContext, buf);

     cliFormat(ewsContext, pStrInfo_base_BurnedInMacAddr);
     memset (stat, 0,sizeof(stat));
     if (cliIsStackingSupported() == L7_TRUE)
     {
       rc = usmDbUnitMgrUnitIdKeyGet(unit, (L7_enetMacAddr_t *)mac);
     }
     else
     {
       rc = usmDbSwDevCtrlBurnedInMacAddrGet(unit, mac);
     }
     ewsTelnetPrintf (ewsContext, "%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

     cliFormat(ewsContext, pStrInfo_common_ApSoftwareVer);
     cliUtilsCodeVersionGet(unit, buf, sizeof(buf));
     ewsTelnetWrite(ewsContext, buf);

#ifndef FEAT_METRO_CPE_V1_0
     memset (buf, 0, sizeof(buf));
     rc = usmDbOperSysGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormat(ewsContext, pStrInfo_common_OperatingSys);
       ewsTelnetWrite(ewsContext, buf);
     }

     memset (buf, 0, sizeof(buf));
     rc = usmDbNPDGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormat(ewsContext, pStrInfo_base_NwProcessingDevice);
       ewsTelnetWrite(ewsContext, buf);
     }
     cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

     cliSyntaxBottom(ewsContext);
     cliAlternateCommandSet(pStrInfo_base_ShowVersionCmd);

     return pStrInfo_common_Name_2;   /* --More-- or (q)uit */

#endif
  }

#ifndef FEAT_METRO_CPE_V1_0
  cliFormat(ewsContext, pStrInfo_base_AdditionalPackages_1);

  osapiSnprintf(dummy,sizeof(dummy), "\r\n%s", "                                                ");

   if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
   {
      addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_BGP_PACKAGE_STR,dummy);
   }

   if ((usmDbComponentPresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_TRUE) ||
       (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID) == L7_TRUE) ||
       (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID) == L7_TRUE))
   {
      addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s",  L7_FASTPATH_FLEX_QOS_PACKAGE_STR,dummy);
   }

   if (usmDbComponentPresentCheck(unit, L7_FLEX_MCAST_MAP_COMPONENT_ID) == L7_TRUE)
   {
      addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_MCAST_PACKAGE_STR,dummy);
   }

   if (usmDbComponentPresentCheck(unit, L7_FLEX_IPV6_MAP_COMPONENT_ID) == L7_TRUE)
   {
     addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_IPV6_PACKAGE_STR, dummy);
  }

  if (usmDbComponentPresentCheck(unit, L7_FLEX_IPV6_MGMT_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_IPV6_MGMT_PACKAGE_STR, dummy);
  }

  if(usmDbComponentPresentCheck(unit, L7_FLEX_WIRELESS_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_WIRELESS_PACKAGE_STR,dummy);
  }

  if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_STACKING_PACKAGE_STR, dummy);
  }

  if((usmDbComponentPresentCheck(unit, L7_FLEX_METRO_DOT1AD_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_DOT3AH_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_DOT1AG_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_TR069_COMPONENT_ID)  == L7_TRUE))
  {
    addpkgs = L7_TRUE;
    ewsTelnetPrintf (ewsContext, "%s%s", L7_FASTPATH_FLEX_METRO_PACKAGE_STR,dummy);
  }

  if (addpkgs == L7_FALSE)
  {
    ewsTelnetPrintf (ewsContext, pStrInfo_common_None_1);
  }
#endif

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose Displays Forwarding Database Count
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
* @notes     Here we only display the visible interfaces.
*
* @cmdsyntax    show mac-addr-table count
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowMacAddrTableCount(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 val;
  L7_uint32 unit;
  usmdbFdbEntry_t fdbEntry;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 countDynamic = 0, countStatic = 0;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowMfdbStats_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  memset(&fdbEntry, 0, sizeof(usmdbFdbEntry_t));
  while( L7_SUCCESS == usmDbFDBEntryNextGet(unit, &fdbEntry))
  {
    switch(fdbEntry.usmdbFdbEntryType)
    {
      case L7_FDB_ADDR_FLAG_STATIC:
        countStatic++;
        break;
      case L7_FDB_ADDR_FLAG_LEARNED:
      case L7_FDB_ADDR_FLAG_DOT1X_STATIC:
        countDynamic++;
        break;
      default:
        break;
    }
  }

  cliFormat(ewsContext, pStrInfo_base_DynAddrCount);
  ewsTelnetPrintf (ewsContext, "%u", countDynamic);

  cliFormat(ewsContext, pStrInfo_base_StaticAddrCount);
  ewsTelnetPrintf (ewsContext, "%u", countStatic);

  cliFormat(ewsContext, pStrInfo_base_MaxMacAddrTblInUse);
  ewsTelnetPrintf (ewsContext, "%u", (countStatic + countDynamic));

  cliFormat(ewsContext, pStrInfo_base_MaxMacAddrTblAvailable);
  if(usmDbFdbMaxTableEntriesGet(unit, &val) == L7_SUCCESS)
  {
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   display the Event Log entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax   show eventlog [unit]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowEventLog(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 count, stringLength;
   static L7_uint32 ptr;
   static L7_char8 string[L7_CLI_MAX_STRING_LENGTH*2]; /* protect ourselves from strings greater than what the MAX is */
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
#ifdef L7_CHASSIS
  L7_uint32 logicalSlot, intSlot;
  L7_BOOL slotFull;
#endif

   cliSyntaxTop(ewsContext);

   cliCmdScrollSet( L7_FALSE);
   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowEventLog_1);
   }

   if (numArg == 0)
   {
      /* get switch ID based on presence/absence of STACKING package */
      unit = cliGetUnitId();
      if (unit == 0)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
  }
  else if (numArg == 1)
  {
#ifndef L7_CHASSIS
    sscanf(argv[index+1], "%d", &unit);
#else
    sscanf(argv[index+1], "%u", &logicalSlot);

    /* In chassis user is unaware of internal unit numbers . User allways
     * configure or refer using slot
     * here we are coverting user entered slot into internal unit
     */

    if (logicalSlot < L7_CHASSIS_MIN_SLOT_NUMBER || logicalSlot > L7_CHASSIS_MAX_SLOT_NUMBER)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_INVALID_UNIT);
    }

    if (usmDbSlotMapUSIntUnitSlotGet(logicalSlot, &unit, &intSlot) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_INVALID_UNIT);
    }

    if ((usmDbSlotIsFullGet(unit, intSlot, &slotFull) == L7_SUCCESS) &&
        (slotFull != L7_TRUE))
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIUNIT_X_DOES_NOT_EXIST, logicalSlot);
    }
#endif
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
      if (numArg != 0) /* for remote units, get the log */
      {
         if (usmDbUnitMgrEventLogGet(unit) != L7_SUCCESS)
         {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_EventLogUnitEmpty);
         }
      }

      /* If first time into function, set ptr to zero to get first entry. */
    memset (string, 0, L7_CLI_MAX_STRING_LENGTH*2);
      ptr = L7_NULL;
      ptr = usmDbLogEventLogEntryNextGet(unit, ptr, string);
      if (ptr == L7_NULL)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_EventLogEmpty_1);
    }
  }

  ewsTelnetWriteAddBlanks (0, 1, 75, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_Time);
  ewsTelnetWriteAddBlanks (0, 1, 7, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_FileLineTaskidCodeDHMS);

  for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    if (ptr == L7_NULL)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    /* without the \r, the lines are not output correctly on the telnet screen. */
    stringLength = strlen(string);
    if(stringLength >= 2)
    {
      if (stringLength > 1) /* To address klocwork ArrayIndexOutOfBounds */
      {
      if (string[stringLength-2] != '\r')
      {
         string[stringLength-1] = '\r';
         string[stringLength]   = '\n';
         string[stringLength+1]   = L7_NULL;
      }
      }
    }
    ewsTelnetWrite(ewsContext, string);
    memset (string, 0, L7_CLI_MAX_STRING_LENGTH*2);
      ptr = usmDbLogEventLogEntryNextGet(unit, ptr, string);
   }

   cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

   if (numArg == 1)
   {
    osapiSnprintf(buf, sizeof(buf), pStrInfo_base_ShowEventLog_2, unit);
      cliAlternateCommandSet(buf);
   }
   else
  {
    cliAlternateCommandSet(pStrInfo_base_ShowEventLogCmd);
  }

  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */

}

/*********************************************************************
*
* @purpose   display the forwarding database entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show mac-addr-table {<macaddr> <vlanid> | all}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowMacAddrTable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 count=0, val;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 mac_address[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 s;
   L7_uint32 p;
   static usmdbFdbEntry_t fdbEntry;
   L7_FDB_TYPE_t fdb_type = L7_SVL;
   L7_uint32 unit, vlanId;
   L7_uint32 numArg;        /* New variable Added */
   L7_char8 vidMac[L7_CLI_MAX_STRING_LENGTH];

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }
   cliCmdScrollSet( L7_FALSE);
   numArg = cliNumFunctionArgsGet();

   usmDbFDBTypeOfVLGet(unit, &fdb_type);

   if (numArg > 2)
   {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbTbl);
   }

   if (cliGetCharInputID() != CLI_INPUT_EMPTY)
   {                                                                     /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
      if (numArg == 2)
      {
         if (strlen(argv[index+1]) >= sizeof(buf))
         {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidMacAddrList);
      }
      OSAPI_STRNCPY_SAFE(buf, argv[index + 1]);
      memset (mac, 0, sizeof(mac));
      if (cliConvertMac(buf, mac) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
      }

      sscanf(argv[index+2], "%d", &vlanId);
      if (usmDbVlanIDGet(unit, vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
      }

      /* combine mac and vlanid to get an 8-byte vidMac address */
      memset (vidMac, 0, sizeof(vidMac));
      (void)usmDbEntryVidMacCombine(vlanId, mac, vidMac);

      memset(&fdbEntry, 0, sizeof(usmdbFdbEntry_t));
      memcpy(fdbEntry.usmdbFdbMac, vidMac, L7_FDB_KEY_SIZE);

      if (L7_SUCCESS != usmDbFDBEntryGet(unit, &fdbEntry))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_MacEntryNotFound);
      }

      /* print this entry info. MAC is 8 bytes long for IVL; 6 bytes for SVL */
      memset (mac_address, 0, sizeof(mac_address));
      memset (stat, 0, sizeof(stat));
      if (fdb_type == L7_IVL)
      {
         ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIntfIfIdxStatus_1);
         ewsTelnetWrite(ewsContext,"\r\n-------  ------------------  ---------  -------  ------------");
         ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
         usmDbEntryVidMacSeparate(fdbEntry.usmdbFdbMac, &vlanId, mac);
         osapiSnprintf(stat,sizeof(stat),"%-9d", vlanId);
         ewsTelnetWrite(ewsContext, stat);
         osapiSnprintf(mac_address, sizeof(mac_address),
          "%02X:%02X:%02X:%02X:%02X:%02X",
                  fdbEntry.usmdbFdbMac[2], fdbEntry.usmdbFdbMac[3],
                  fdbEntry.usmdbFdbMac[4], fdbEntry.usmdbFdbMac[5],
                  fdbEntry.usmdbFdbMac[6], fdbEntry.usmdbFdbMac[7]);
         osapiSnprintf(stat,sizeof(stat),"%-20s", mac_address);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 3, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIntfIfIdxStatus_3);
        ewsTelnetWrite(ewsContext,"\r\n-----------------  ---------  -------  ------------");
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        osapiSnprintf(mac_address, sizeof(mac_address),
          "%02X:%02X:%02X:%02X:%02X:%02X",
                  fdbEntry.usmdbFdbMac[0], fdbEntry.usmdbFdbMac[1],
                  fdbEntry.usmdbFdbMac[2], fdbEntry.usmdbFdbMac[3],
                  fdbEntry.usmdbFdbMac[4], fdbEntry.usmdbFdbMac[5]);
        osapiSnprintf(stat,sizeof(stat),"%-19s ", mac_address);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      if (usmDbUnitSlotPortGet(fdbEntry.usmdbFdbIntIfNum, &unit, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit,s,p));
        ewsTelnetPrintf (ewsContext, "%-11s", buf);
      }

      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      if (usmDbIfIndexGet(unit, fdbEntry.usmdbFdbIntIfNum, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",val);
        ewsTelnetPrintf (ewsContext, "%-9s", buf);
      }

      memset (stat, 0, sizeof(stat));
      switch (fdbEntry.usmdbFdbEntryType)
      {
        case L7_FDB_ADDR_FLAG_STATIC:
          osapiSnprintf(stat, sizeof(stat), "%-17s", pStrInfo_common_PimSmGrpRpMapStatic);      /* Static */
          break;
        case L7_FDB_ADDR_FLAG_LEARNED:
        case L7_FDB_ADDR_FLAG_DOT1X_STATIC:
          osapiSnprintf(stat, sizeof(stat), "%-17s", pStrInfo_base_Learned);      /* Learned */
          break;
        case L7_FDB_ADDR_FLAG_SELF:
          osapiSnprintf(stat, sizeof(stat), "%-17s", pStrInfo_base_Self);      /* Self */
          break;
        case L7_FDB_ADDR_FLAG_MANAGEMENT:
        case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
          osapiSnprintf(stat, sizeof(stat), "%-17s", pStrInfo_base_Mgmt);      /* Management*/
          break;
        case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
          osapiSnprintf(stat, sizeof(stat), "%-17s", pStrInfo_base_GmrpLearned);      /* GMRP Learned */
          break;
        default:
          osapiSnprintf(stat, sizeof(stat), "%-17s", pStrInfo_common_Other);      /* Other */
          break;
      }
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }
    else if (numArg == 1)
    {
      if (strcmp(argv[index+1], pStrInfo_common_All) != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowFdbTbl);
      }
    }

    memset(&fdbEntry, 0, sizeof(usmdbFdbEntry_t));
    if (L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry))             /* get the first entry after the zero parameter */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacAddrTblShow);
    }
  }

  /* printing header info.  MAC is 8 bytes long for IVL; 6 bytes for SVL*/
  if (fdb_type == L7_IVL)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIntfIfIdxStatus_1);
    ewsTelnetWrite(ewsContext,"\r\n-------  ------------------  ---------  -------  ------------");
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 3, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIntfIfIdxStatus_3);
      ewsTelnetWrite(ewsContext,"\r\n-----------------  ---------  -------  ------------");
   }

   for (count = 0; count < CLI_MAX_SCROLL_LINES-3; count++)
   {
     if (fdb_type == L7_IVL)
     {
       ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
       usmDbEntryVidMacSeparate(fdbEntry.usmdbFdbMac, &vlanId, mac);
       osapiSnprintf(stat,sizeof(stat),"%-9d", vlanId);
       ewsTelnetWrite(ewsContext, stat);
       osapiSnprintf(mac_address, sizeof(mac_address),
          "%02X:%02X:%02X:%02X:%02X:%02X",
                 fdbEntry.usmdbFdbMac[2],
                 fdbEntry.usmdbFdbMac[3],
                 fdbEntry.usmdbFdbMac[4],
                 fdbEntry.usmdbFdbMac[5],
                 fdbEntry.usmdbFdbMac[6],
                 fdbEntry.usmdbFdbMac[7]);
       osapiSnprintf(stat,sizeof(stat),"%-20s", mac_address);
     }
     else
     {
       ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
       osapiSnprintf(mac_address, sizeof(mac_address),
         "%02X:%02X:%02X:%02X:%02X:%02X",
                 fdbEntry.usmdbFdbMac[0],
                 fdbEntry.usmdbFdbMac[1],
                 fdbEntry.usmdbFdbMac[2],
                 fdbEntry.usmdbFdbMac[3],
                 fdbEntry.usmdbFdbMac[4],
                 fdbEntry.usmdbFdbMac[5]);
       osapiSnprintf(stat,sizeof(stat),"%-19s", mac_address);
     }

     ewsTelnetWrite(ewsContext, stat);

     if (usmDbUnitSlotPortGet(fdbEntry.usmdbFdbIntIfNum, &unit, &s, &p) == L7_SUCCESS)
     {
       osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit,s,p));
       ewsTelnetPrintf (ewsContext, "%-11s", buf);
     }

     if (usmDbIfIndexGet(unit, fdbEntry.usmdbFdbIntIfNum, &val) == L7_SUCCESS)
     {
       osapiSnprintf(buf, sizeof(buf), "%d",val);
       ewsTelnetPrintf (ewsContext, "%-9s", buf);
     }

     switch (fdbEntry.usmdbFdbEntryType)
     {
       case L7_FDB_ADDR_FLAG_STATIC:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_PimSmGrpRpMapStatic);     /* Static */
          break;
       case L7_FDB_ADDR_FLAG_LEARNED:
       case L7_FDB_ADDR_FLAG_DOT1X_STATIC:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Learned);     /* Learned */
          break;
       case L7_FDB_ADDR_FLAG_SELF:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Self);     /* Self */
          break;
       case L7_FDB_ADDR_FLAG_MANAGEMENT:
       case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Mgmt);     /* Management*/
          break;
       case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_GmrpLearned);     /* GMRP Learned */
          break;
       default:
         osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_Other);     /* Other */
         break;
     }
     ewsTelnetWrite(ewsContext, stat);

     if (L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry))
     {
       ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
       return cliSyntaxReturnPrompt (ewsContext, "");
     }
   }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliAlternateCommandSet(pStrInfo_base_ShowMacAddrTblCmd);
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose  To display the forwarding database enteries on per interface basis
*
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
* @cmdsyntax show mac-addr-table interface u/s/p
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowMacAddrTableInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static usmdbFdbEntry_t fdbEntry;
  L7_FDB_TYPE_t fdb_type = L7_SVL;
  L7_uint32 unit, slot, port, interface;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 mac_address[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 vlanId;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uint32 numArg, count;
  L7_RC_t rc;
  L7_BOOL flag = L7_FALSE;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if(unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if( numArg > 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowMacIntf);
  }

    if((rc = cliValidSpecificUSPCheck(argv[index+1], &unit, &slot, &port)) != L7_SUCCESS)
    {
    return cliPrompt(ewsContext);
  }
    if(usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
    return cliPrompt(ewsContext);
  }

  usmDbFDBTypeOfVLGet(unit, &fdb_type);

  if( cliGetCharInputID() != CLI_INPUT_EMPTY )
  {                                                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {

    memset(&fdbEntry, 0, sizeof(usmdbFdbEntry_t));
    if( L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry) )           /* get the first entry after the zero parameter */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacAddrTblShow);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 3, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrVlanIdStatus);
  ewsTelnetWrite(ewsContext,"\r\n-----------------  ---------  ------------");

  for( count = 0; count < CLI_MAX_SCROLL_LINES-6;)
  {
    if(fdbEntry.usmdbFdbIntIfNum == interface)
    {
      usmDbEntryVidMacSeparate(fdbEntry.usmdbFdbMac, &vlanId, mac);
      osapiSnprintf(mac_address,sizeof(mac_address),"\r\n%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      ewsTelnetPrintf (ewsContext, "%-23s ", mac_address);

      memset (stat, 0, sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), "%d",vlanId);
      ewsTelnetPrintf (ewsContext, "%-9s", buf);

      memset (stat, 0, sizeof(stat));
      switch(fdbEntry.usmdbFdbEntryType)
      {
        case L7_FDB_ADDR_FLAG_STATIC:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_PimSmGrpRpMapStatic);   /* Static */
          break;
        case L7_FDB_ADDR_FLAG_LEARNED:
        case L7_FDB_ADDR_FLAG_DOT1X_STATIC:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Learned);   /* Learned */
          break;
        case L7_FDB_ADDR_FLAG_SELF:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Self);   /* Self */
          break;
        case L7_FDB_ADDR_FLAG_MANAGEMENT:
        case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Mgmt);   /* Management*/
          break;
        case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_GmrpLearned);   /* GMRP Learned */
          break;
        default:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_Other);   /* Other */
          break;
      }
      ewsTelnetWrite(ewsContext, stat);
      count++;
      flag = L7_TRUE;
    }
    if( L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry) )
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  if(flag == L7_FALSE)
  {
    osapiSnprintfAddBlanks (0, 0, 1, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_MacAddrIntfTblShow);
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_MacIntfCmd, argv[index+1]);
  cliAlternateCommandSet(buf);
  return pStrInfo_common_Name_2;

}

/*********************************************************************
*
* @purpose  To display the forwarding database enteries on per Vlan ID basis
*
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
* @cmdsyntax show mac-addr-table vlan <vlanId>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowMacAddrTableVlanId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static usmdbFdbEntry_t fdbEntry;
  L7_FDB_TYPE_t fdb_type = L7_SVL;
  L7_uint32 unit, slot, port;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 mac_address[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 vlanId,vid;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uint32 numArg, count;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if(cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArg = cliNumFunctionArgsGet();

  if(numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_MacAddrTblVlanId);
  }

  if(numArg == 1)
  {
    if(cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    rc = usmDbVlanIDGet(unit, vlanId);

    if(rc == L7_FAILURE)
    {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_VlanLookupFailure);
    }
    else if(rc == L7_NOT_EXISTS)
    {
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_VlanDoesntExist);
    }
  }

  usmDbFDBTypeOfVLGet(unit, &fdb_type);
  if( cliGetCharInputID() != CLI_INPUT_EMPTY )
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    memset(&fdbEntry, 0, sizeof(usmdbFdbEntry_t));
    if( L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry) )           /* get the first entry after the zero parameter */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacAddrTblShow);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 3, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrIntfStatus);
  ewsTelnetWrite(ewsContext,"\r\n-----------------  ---------  ------------");

  for( count = 0; count < CLI_MAX_SCROLL_LINES-6; )
  {
    usmDbEntryVidMacSeparate(fdbEntry.usmdbFdbMac, &vid, mac);
    if(vid == vlanId)
    {
      osapiSnprintf(mac_address,sizeof(mac_address),"\r\n%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      ewsTelnetPrintf (ewsContext, "%-20s ", mac_address);

      memset (stat, 0, sizeof(stat));
      if(usmDbUnitSlotPortGet(fdbEntry.usmdbFdbIntIfNum, &unit, &slot, &port) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit,slot,port));
        ewsTelnetPrintf (ewsContext, "%-11s", buf);
      }

      memset (stat, 0, sizeof(stat));
      switch(fdbEntry.usmdbFdbEntryType)
      {
        case L7_FDB_ADDR_FLAG_STATIC:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_PimSmGrpRpMapStatic);   /* Static */
          break;
        case L7_FDB_ADDR_FLAG_LEARNED:
        case L7_FDB_ADDR_FLAG_DOT1X_STATIC:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Learned);   /* Learned */
          break;
        case L7_FDB_ADDR_FLAG_SELF:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Self);   /* Self */
          break;
        case L7_FDB_ADDR_FLAG_MANAGEMENT:
        case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_Mgmt);   /* Management*/
          break;
        case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_base_GmrpLearned);   /* GMRP Learned */
          break;
        default:
          osapiSnprintf(stat, sizeof(stat), "%-13.3s", pStrInfo_common_Other);   /* Other */
          break;
      }
      ewsTelnetWrite(ewsContext, stat);
      count++;
    }
    if( L7_SUCCESS != usmDbFDBEntryNextGet(unit, &fdbEntry) )
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_MacVlanIdCmd, argv[index+1]);
  cliAlternateCommandSet(buf);
  return pStrInfo_common_Name_2;   /* --More-- or (q)uit */

}
/*********************************************************************
 * @purpose  used for diffserv, Converts a log severity keyword string to the
 *           associated value, if one is available.  If the string holds
 *           an integer value, the integer value is returned.
 *
 * @param    dscpVal         value to return
 * @param    dscpString      string to parse
 * @param    bufSiz          size of output buffer
 *
 * @returns rc
 *
 * @notes none
 *
 * @end
 *********************************************************************/

L7_RC_t cliLogConvertSeverityLevelToString(L7_LOG_SEVERITY_t severityLevel, L7_char8 * severityLevelString, L7_uint32 bufSiz)
{
  L7_char8 * pStr;

  switch (severityLevel)
  {
    case L7_LOG_SEVERITY_EMERGENCY:
      pStr = pStrInfo_base_LogSeverityEmergency;
      break;

    case L7_LOG_SEVERITY_ALERT:
      pStr = pStrInfo_base_LogSeverityAlert;
      break;

    case L7_LOG_SEVERITY_CRITICAL:
      pStr = pStrInfo_common_LogSeverityCritical;
      break;

    case L7_LOG_SEVERITY_ERROR:
      pStr = pStrInfo_common_Error_5;
      break;

    case L7_LOG_SEVERITY_WARNING:
      pStr = pStrWarn_base_LogSeverityWarning;
      break;

    case L7_LOG_SEVERITY_NOTICE:
      pStr = pStrInfo_base_LogSeverityNotice;
      break;

    case L7_LOG_SEVERITY_INFO:
      pStr = pStrInfo_base_LogSeverityInfo;
      break;

    case L7_LOG_SEVERITY_DEBUG:
      pStr = pStrInfo_common_LogSeverityDebug;
      break;

    default:
      pStr = L7_NULLPTR;
      break;

  }  /* endswitch */

  if (pStr != L7_NULLPTR)
  {
    (void)osapiStrncpySafe(severityLevelString, pStr, bufSiz);
  }                                                              /* ignore ret val here */

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  used for diffserv, Converts a log severity keyword string to the
 *           associated value, if one is available.  If the string holds
 *           an integer value, the integer value is returned.
 *
 * @param    dscpVal         value to return
 * @param    dscpString      string to parse
 * @param    bufSiz          size of output buffer
 *
 * @returns rc
 *
 * @notes none
 *
 * @end
 *********************************************************************/

L7_RC_t cliLogConvertFacilityToString(L7_uint32 facility, L7_char8 * facilityString, L7_uint32 bufSiz)
{
  L7_char8 * pStr;

  switch (facility)
  {
    case L7_LOG_FACILITY_KERNEL:
      pStr = pStrInfo_base_LogFacilityKernel;
      break;

    case L7_LOG_FACILITY_USER:
      pStr = pStrInfo_common_LogFacilityUsr;
      break;

    case L7_LOG_FACILITY_MAIL:
      pStr = pStrInfo_base_LogFacilityMail;
      break;

    case L7_LOG_FACILITY_SYSTEM:
      pStr = pStrInfo_base_BootSys;
      break;

    case L7_LOG_FACILITY_SECURITY:
      pStr = pStrInfo_common_LogFacilitySecurity;
      break;

    case L7_LOG_FACILITY_SYSLOG:
      pStr = pStrInfo_base_LogFacilitySyslog;
      break;

    case L7_LOG_FACILITY_LPR:
      pStr = pStrInfo_base_LogFacilityLpr;
      break;

    case L7_LOG_FACILITY_NNTP:
      pStr = pStrInfo_base_LogFacilityNntp;
      break;

    case L7_LOG_FACILITY_UUCP:
      pStr = pStrInfo_base_LogFacilityUucp;
      break;

    case L7_LOG_FACILITY_CRON:
      pStr = pStrInfo_base_LogFacilityCron;
      break;

    case L7_LOG_FACILITY_AUTH:
      pStr = pStrInfo_common_LogFacilityAuth;
      break;

    case L7_LOG_FACILITY_FTP:
      pStr = pStrInfo_common_Ftp;
      break;

    case L7_LOG_FACILITY_NTP:
      pStr = pStrInfo_base_LogFacilityNtp;
      break;

    case L7_LOG_FACILITY_AUDIT:
      pStr = pStrInfo_base_LogFacilityAudit;
      break;

    case L7_LOG_FACILITY_ALERT:
      pStr = pStrInfo_base_LogSeverityAlert;
      break;

    case L7_LOG_FACILITY_CLOCK:
      pStr = pStrInfo_base_LogFacilityClock;
      break;

    case L7_LOG_FACILITY_LOCAL0:
      pStr = pStrInfo_base_LogFacilityLocal;
      break;

    case L7_LOG_FACILITY_LOCAL1:
      pStr = pStrInfo_base_LogFacilityLocal_1;
      break;

    case L7_LOG_FACILITY_LOCAL2:
      pStr = pStrInfo_base_LogFacilityLocal2;
      break;

    case L7_LOG_FACILITY_LOCAL3:
      pStr = pStrInfo_base_LogFacilityLocal3;
      break;

    case L7_LOG_FACILITY_LOCAL4:
      pStr = pStrInfo_base_LogFacilityLocal_2;
      break;

    case L7_LOG_FACILITY_LOCAL5:
      pStr = pStrInfo_base_LogFacilityLocal_3;
      break;

    case L7_LOG_FACILITY_LOCAL6:
      pStr = pStrInfo_base_LogFacilityLocal_4;
      break;

    case L7_LOG_FACILITY_LOCAL7:
      pStr = pStrInfo_base_LogFacilityLocal_4;
      break;

    case L7_LOG_FACILITY_ALL:
      pStr = pStrInfo_common_All;
      break;

    default:
      pStr = L7_NULLPTR;
      break;

  }  /* endswitch */

  if (pStr != L7_NULLPTR)
  {
    (void)osapiStrncpySafe(facilityString, pStr, bufSiz);
  }                                                         /* ignore ret val here */

   return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  display the traplog
*
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
* @cmdsyntax  show logging traplogs
*
* @cmdhelp Display trap records.
*
* @cmddescript
*   The switch maintains a Trap Log; it contains a maximum of 256 entries
*   that wrap. Trap Log information is not retained across a switch reset.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoggingTraplogs(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   usmDbTrapLogEntry_t trapLog;
   L7_uint32 val;
   static L7_uint32 count = 0;
   static L7_uint32 startingEntryIndex = 0;
   static L7_uint32 entryIndex = 0;
   static L7_uint32 numEntries = 0;
   L7_uint32 numWrites = 0;
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 buf[280];
   L7_BOOL first = L7_TRUE;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   cliCmdScrollSet( L7_FALSE);
   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLoggingTrapLog_1);
   }
   else
   {
      if (cliGetCharInputID() != CLI_INPUT_EMPTY)
      {                                                                     /* if our question has been answered */
         numWrites = 0;

      if(L7_TRUE == cliIsPromptRespQuit())
         {
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliPrompt(ewsContext);
         }
      }
      else
      {
         count = 0;
         entryIndex = usmDbTrapLogEntryLastReceivedGet();
         startingEntryIndex = entryIndex;

         if (usmDbInfoTrapsSinceLastResetSwGet(unit, &numEntries) != L7_SUCCESS)
      {
        numEntries = 0;
      }

      /* Determine # of traps to be displayed. */
      cliFormat(ewsContext, pStrInfo_base_NumOfTrapsSinceLastReset);     /*Number of Traps Since Last Reset*/
      ewsTelnetPrintf (ewsContext, "%u", numEntries);
      numWrites++;

      /* Display the Trap Log Capacity. */
      cliFormat(ewsContext, pStrInfo_base_NumTrapLogCapacity);     /*Trap Log Capacity*/
      ewsTelnetPrintf (ewsContext, "%u", TRAPMGR_TABLE_SIZE);
      numWrites++;

      cliFormat(ewsContext, pStrInfo_base_NumOfTrapsSinceLogLastViewed);    /*Number of Traps Since Log Last Displayed*/
      rc = usmDbTrapNumUnDisplayedTrapEntriesGet(unit, &val);
      ewsTelnetPrintf (ewsContext, "%u", val);
         numWrites++;

         if (numEntries > TRAPMGR_TABLE_SIZE)
      {
            numEntries = TRAPMGR_TABLE_SIZE;
      }
   }
  }

   /* printout header information */
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LogSysUpTimeTrap);
  ewsTelnetWrite(ewsContext,"\r\n--- ------------------------ -------------------------------------------------");
  numWrites +=3;

  if (numEntries == 0)
  {
    ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  while (numWrites < CLI_MAX_SCROLL_LINES-6)
  {
    rc = usmDbTrapLogEntryGet(unit, USMDB_USER_DISPLAY, entryIndex, &trapLog);

    if (rc != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    if (strcmp(trapLog.timestamp, pStrInfo_common_EmptyString) != 0)
    {
      ewsTelnetPrintf (ewsContext, "\r\n%3d ", count);

      ewsTelnetPrintf (ewsContext, "%-24s ", trapLog.timestamp);

      memset (stat, 0,sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(trapLog.message, 48));
      do
      {
        if (first == L7_TRUE)
        {
          first = L7_FALSE;
          osapiSnprintf(stat, sizeof(stat), "%-48s", buf);
        }
        else
        {
          ewsTelnetPrintf (ewsContext, "\r\n%-29s", " ");
          osapiSnprintf(stat, sizeof(stat), "%-48s", buf);
        }

        ewsTelnetWrite(ewsContext,stat);
        numWrites++;

        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(trapLog.message, 48));
      }
      while (strcmp(buf, trapLog.message) != 0);

      first = L7_TRUE;

    }
    count++;

    if (entryIndex > 0)
    {
      entryIndex--;
    }
    else
    {
      if (count < numEntries)
      {
        entryIndex = TRAPMGR_TABLE_SIZE-1;
      }
      }

      if (((count == startingEntryIndex + 1) && (numEntries < TRAPMGR_TABLE_SIZE)) || (count == TRAPMGR_TABLE_SIZE))
      {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_base_ShowLoggingTrapLogsCmd);
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose   Display the Message Log  entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax in stacking env: show logging  [unit]
* @cmdsyntax in non-stacking env: show logging
*
* @cmdhelp
*
* @cmddescript
    ewsTelnetWrite( ewsContext,"\r\n");
  ewsTelnetWrite( ewsContext,"\r\n");
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLogging(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_RC_t rc;
   L7_ADMIN_MODE_t status;
   L7_ushort16 port;
   L7_uint32 count;
   L7_uint32 severity;
   L7_uint32 cliAdminMode;
   L7_char8 valString[L7_CLI_MAX_STRING_LENGTH];

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLogging_1);
   }

   if (numArg == 0)
   {
      unit = L7_ALL_UNITS;
   }
   else if (numArg == 1)
   {
      sscanf(argv[index+1], "%d", &unit);
   }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowLogging_1);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {

      /* Port */
      rc = usmDbLogLocalPortGet (unit, &port);

      if (rc != L7_FAILURE)
      {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowLoggingClientLocalPort);
      osapiSnprintf(buf, sizeof(buf), "%d", port);
      OSAPI_STRNCAT(stat, buf);
         ewsTelnetWrite( ewsContext, stat);

      }

      /* CLI command Logging Admin Mode */
      rc = usmDbCmdLoggerAdminModeGet(&cliAdminMode);

      if (rc != L7_FAILURE)
      {
      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowCmdLogging);
         /* eventually set to default */
         if (cliAdminMode == L7_ENABLE)
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Enbld_1);
      }
      else
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Dsbld2);
      }
      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }

    /*      Console Admin Mode */
    rc = usmDbLogConsoleAdminStatusGet (unit, &status);

    if (rc != L7_FAILURE)
    {
      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowConsoleLogging);
      /* eventually set to default */
      if (status ==   L7_ADMIN_MODE_ENABLE)
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Enbld_1);
      }
      else
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Dsbld2);
      }

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }

    /* Severity Mode */
    rc = usmDbLogConsoleSeverityFilterGet (unit, &severity);
    if(rc != L7_FAILURE)
    {
      memset (stat, 0,sizeof(stat));
      memset (valString, 0,sizeof(valString));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowConsoleLoggingSeverityFilter);
      rc = cliLogConvertSeverityLevelToString(severity, valString, sizeof(valString));
      if(rc != L7_FAILURE)
      {
        OSAPI_STRNCAT(stat, valString);
        ewsTelnetWrite( ewsContext, stat);
      }
    }

    /*      Buffered Admin Mode */
    rc = usmDbLogInMemoryAdminStatusGet (unit, &status);

    if (rc != L7_FAILURE)
    {
      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowBufedLogging);
      /* eventually set to default */
      if (status ==   L7_ADMIN_MODE_ENABLE)
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Enbld_1);
      }
      else
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Dsbld2);
      }

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
    /* Component */
    rc = usmDbLogInMemoryComponentFilterGet (unit, &component);
    if (rc != L7_SUCCESS)
    {

      memset (stat, 0, sizeof(stat));
      memset (valString, 0,sizeof(valString));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowBufedLoggingCompFilter);
      rc = usmDbComponentMnemonicGet(component, valString);

      if (rc != L7_SUCCESS)
      {

        if (strcmp (valString, pStrInfo_common_EmptyString) == 0)
        {
          OSAPI_STRNCAT(stat, pStrInfo_common_ApProfileRadioScanFrequencyAll);
        }
        else
        {
          OSAPI_STRNCAT(stat, valString);
        }

            ewsTelnetWrite( ewsContext, stat);
         }
      }
#endif

      if (usmDbFeaturePresentCheck(unit, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) == L7_TRUE)
      {
          /*      Persistent Admin Mode */
         rc = usmDbLogPersistentAdminStatusGet (unit, &status);
         if (rc != L7_FAILURE)
         {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowPersistentLogging);
        /* eventually set to default */
        if (status ==   L7_ADMIN_MODE_ENABLE)
        {
          OSAPI_STRNCAT(buf, pStrInfo_common_Enbld_1);
        }
        else
        {
          OSAPI_STRNCAT(buf, pStrInfo_common_Dsbld2);
        }

        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }
    }
    /* Admin Mode */
    rc = usmDbLogSyslogAdminStatusGet (unit, &status);

    if (rc != L7_FAILURE)
    {
      memset (stat, 0,sizeof(stat));
      memset (buf, 0,sizeof(buf));
      OSAPI_STRNCAT_ADD_BLANKS (2, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowSyslogLogging);
      /* eventually set to default */
      if (status ==   L7_ADMIN_MODE_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld_1);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld2);
      }

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }

    /* Messages Received */
    rc = usmDbLogMessagesReceivedGet (unit, &count);
    if (rc != L7_FAILURE)
    {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (2, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowLogMsgsRcvd);

      osapiSnprintf(buf, sizeof(buf), "%d", count);

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }

    /* Messages Dropped */
    rc = usmDbLogMessagesDroppedGet (unit, &count);
    if (rc != L7_FAILURE)
    {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowLogMsgsDropped);

      osapiSnprintf(buf, sizeof(buf), "%d", count);

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }

    /* Messages Relayed */
    rc = usmDbLogSyslogMessagesRelayedGet (unit, &count);
    if (rc != L7_FAILURE)
    {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowLogMsgsRelayed);

      osapiSnprintf(buf, sizeof(buf), "%d", count);

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }

  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}
/*********************************************************************
*
* @purpose   Display the Message Log  entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax in stacking env: show logging  [unit]
* @cmdsyntax in non-stacking env: show logging
*
* @cmdhelp
*
* @cmddescript
    ewsTelnetWrite( ewsContext,"\r\n");
  ewsTelnetWrite( ewsContext,"\r\n");
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoggingEmailAlertStats(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 numArg;
   L7_RC_t rc = L7_SUCCESS;	 
   emailAlertingStatsData_t emailAlertStatistics;  
	L7_uint32 operationalStatus	; 
   L7_timespec timeSpec;	 
	L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];	
	 

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLogging_1);
   }   

	rc= usmDbLogEmailAlertOperationalStatusGet(USMDB_UNIT_CURRENT,&operationalStatus);

	if(rc != L7_FAILURE)
	{
	  memset (buf, 0,sizeof(buf));
 	  cliFormat(ewsContext, pStrInfo_base_EmailAlertOperationalStatus);							

      if (operationalStatus==   L7_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld_1);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld2);
      }
      ewsTelnetWrite( ewsContext, buf);
      ewsTelnetWrite( ewsContext, "\n");			
	}

    rc=usmDbLogEmailAlertStatsGet(USMDB_UNIT_CURRENT, &emailAlertStatistics);

	if(rc != L7_FAILURE)
	{

	      ewsTelnetWrite( ewsContext, "\nEmail Alert Statistics:\n");

	      memset (buf, 0,sizeof(buf));
   	      cliFormat(ewsContext, pStrInfo_base_EmailAlertNoOfEmailFailures);							
	      osapiSnprintf(buf, sizeof(buf), "%d",emailAlertStatistics.noEmailFailures);
	      ewsTelnetWrite( ewsContext, buf);

	      memset (buf, 0,sizeof(buf));
   	      cliFormat(ewsContext, pStrInfo_base_EmailAlertNoOfEmailsSent);											
	      osapiSnprintf(buf, sizeof(buf), "%d",emailAlertStatistics.noEmailsSent);
	      ewsTelnetWrite( ewsContext, buf);

	      memset (buf, 0,sizeof(buf));
   	      cliFormat(ewsContext, pStrInfo_base_EmailAlertTimeSinceLastEmail);															
         osapiConvertRawUpTime(emailAlertStatistics.timeSinceLastMail,(L7_timespec *)&timeSpec);
         osapiSnprintf(buf, sizeof(buf), "%2.2d days %2.2d hours %2.2d mins %2.2d  secs", timeSpec.days,timeSpec.hours,
                                                 timeSpec.minutes,timeSpec.seconds);

	      ewsTelnetWrite( ewsContext, buf);
				
	}

  return cliSyntaxReturnPrompt (ewsContext, "");
   
}

/*********************************************************************
*
* @purpose   Display the Message Log  entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax in stacking env: show logging  [unit]
* @cmdsyntax in non-stacking env: show logging
*
* @cmdhelp
*
* @cmddescript
    ewsTelnetWrite( ewsContext,"\r\n");
  ewsTelnetWrite( ewsContext,"\r\n");
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoggingEmailAlertConfig(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 numArg;
   L7_RC_t rc = L7_SUCCESS;
   L7_uint32 status;	 
	L7_char8 fromAddress[L7_CLI_MAX_STRING_LENGTH];	 
	L7_uint32 urgentSeverityLevel;
	L7_uint32 nonUrgentSeverityLevel;	
	L7_uint32 trapSeverityLevel;	
	L7_uint32 logDuration;	
	L7_uint32  msgType;		
	L7_char8 toAddress[L7_CLI_MAX_STRING_LENGTH];	 
	L7_char8 subject[L7_CLI_MAX_STRING_LENGTH];
	L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];	
	L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 valString[L7_CLI_MAX_STRING_LENGTH];

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) )
   {
	    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLogging_1);
   }   

    rc = usmDbLogEmailAlertStatusGet(USMDB_UNIT_CURRENT, &status);

    if (rc != L7_FAILURE)
    {
      memset (buf, 0,sizeof(buf));
	   cliFormat(ewsContext, pStrInfo_base_EmailAlertLogging);			

      /* eventually set to default */
      if (status ==   L7_ADMIN_MODE_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld_1);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld2);
      }

      ewsTelnetWrite( ewsContext, buf);
	 
    }

    rc = usmDbLogEmailAlertFromAddrGet(USMDB_UNIT_CURRENT, fromAddress);

    if (rc != L7_FAILURE)
    {
      memset (buf, 0,sizeof(buf));
	   cliFormat(ewsContext, pStrInfo_base_EmailAlertFromAddress );						
      /* eventually set to default */
      osapiSnprintf(buf, sizeof(buf), fromAddress);

      ewsTelnetWrite( ewsContext, buf);
    }

    rc = usmDbLogEmailAlertUrgentSeverityGet(USMDB_UNIT_CURRENT, &urgentSeverityLevel);

    if (rc != L7_FAILURE)
    {
      memset (buf, 0,sizeof(buf));
	   cliFormat(ewsContext, pStrInfo_base_EmailAlertUrgentSeverityLevel );									
      /* eventually set to default */
	  if(urgentSeverityLevel == -1)
	  {
        osapiSnprintf(buf, sizeof(buf), "%s", "none");
	  }
	  else
	  {
         memset (valString, 0,sizeof(valString));							
	      rc = cliLogConvertSeverityLevelToString(urgentSeverityLevel, valString, sizeof(valString));			
	      if(rc != L7_FAILURE)
	      {
	        OSAPI_STRNCAT(buf, valString);
	      }
	  }

      ewsTelnetWrite( ewsContext, buf);
    }

    rc = usmDbLogEmailAlertNonUrgentSeverityGet(USMDB_UNIT_CURRENT, &nonUrgentSeverityLevel);
    if (rc != L7_FAILURE)
    {
      memset (buf, 0,sizeof(buf));
      memset (valString, 0,sizeof(valString));			
	   cliFormat(ewsContext, pStrInfo_base_EmailAlertNonUrgentSeverityLevel );												

      rc = cliLogConvertSeverityLevelToString(nonUrgentSeverityLevel, valString, sizeof(valString));			
      if(rc != L7_FAILURE)
      {
        OSAPI_STRNCAT(buf, valString);
        ewsTelnetWrite( ewsContext, buf);
      }

    }

    rc = usmDbLogEmailAlertTrapSeverityGet(USMDB_UNIT_CURRENT, &trapSeverityLevel);
    if (rc != L7_FAILURE)
    {
      memset (buf, 0,sizeof(buf));
      memset (valString, 0,sizeof(valString));						
	  cliFormat(ewsContext, pStrInfo_base_EmailAlertTrapSeverityLevel);															

      rc = cliLogConvertSeverityLevelToString(trapSeverityLevel, valString, sizeof(valString));			
      if(rc != L7_FAILURE)
      {
        OSAPI_STRNCAT(buf, valString);
        ewsTelnetWrite( ewsContext, buf);
      }
    }

    rc = usmDbLogEmailAlertLogTimeGet(USMDB_UNIT_CURRENT, &logDuration);

    if (rc != L7_FAILURE)
    {
      memset (buf, 0,sizeof(buf));
	  cliFormat(ewsContext, pStrInfo_base_EmailAlertNotificationPeriod);															
      /* eventually set to default */
      osapiSnprintf(buf, sizeof(buf), "%d min", logDuration);

      ewsTelnetWrite( ewsContext, buf);
    }

   memset (stat, 0,sizeof(stat));
   OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_EmailAlertToAddress);
   ewsTelnetWrite( ewsContext, stat);		 


	for(msgType=1; msgType<=2;msgType++)
	{
		 L7_uint32  messageType;
		 L7_uint32 i=1;			
 		 messageType =msgType;			
		 if(usmDbLogEmailAlertToAddrGet(USMDB_UNIT_CURRENT , msgType, toAddress) == L7_FAILURE)
		 {
			continue;

		 }

	     memset (buf, 0,sizeof(buf));
	     memset (stat, 0,sizeof(stat));
	     OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_EmailAlertMsgType);
	     cliFormat(ewsContext, stat);
		 if(msgType == L7_LOG_EMAIL_ALERT_URGENT)
		 {
		   osapiSnprintf(buf, sizeof(buf),"%s\n", "urgent");
		 }
		 else
		 {
		   osapiSnprintf(buf, sizeof(buf),"%s\n", "non-urgent");
		 }
		 ewsTelnetWrite( ewsContext, buf);

	     memset (stat, 0,sizeof(stat));
  	     cliFormat(ewsContext, "Address1 ");																		 			 
 	     osapiSnprintf(buf, sizeof(buf), toAddress);
	     OSAPI_STRNCAT(stat, buf);
	     ewsTelnetWrite( ewsContext, stat);		 

			 

		  while(usmDbLogEmailAlertNextToAddrGet(USMDB_UNIT_CURRENT , &msgType, toAddress) != L7_FAILURE)
		  {

			  if(messageType != msgType)
			  {
					msgType= messageType ;
					break;
			  }
			  i++;	
		     memset (stat, 0,sizeof(stat));
		     memset (buf, 0,sizeof(buf));	
		     osapiSnprintf(stat, sizeof(stat), "Address%d", i);				 
	  	     cliFormat(ewsContext, stat);																		 			 				 
		  	 osapiSnprintf(buf, sizeof(buf), toAddress);
		     ewsTelnetWrite( ewsContext, buf);

		  }
	}

	memset (stat, 0,sizeof(stat));
	OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_EmailAlertSubject);
	ewsTelnetWrite( ewsContext, stat);		 

	for(msgType=1; msgType <=2;msgType++)
	{

		 if(usmDbLogEmailAlertSubjectGet(USMDB_UNIT_CURRENT , msgType, subject) == L7_FAILURE)
		 {
			continue;
		 }

	     memset (stat, 0,sizeof(stat));
	     memset (buf, 0,sizeof(buf));
	     OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_EmailAlertMsgType);
		  if(msgType == L7_LOG_EMAIL_ALERT_URGENT)
		  {
			 osapiSnprintf(buf, sizeof(buf),"%s", "urgent, ");
		  }
		  else
		  {
			 osapiSnprintf(buf, sizeof(buf),"%s", "non-urgent, ");
		  }
	     OSAPI_STRNCAT(stat, buf);
	     OSAPI_STRNCAT(stat, "subject is");			 
  	     cliFormat(ewsContext, stat);																		 			 			 
	     memset (buf, 0,sizeof(buf));			 
	  	 osapiSnprintf(buf, sizeof(buf), subject);			 
	     ewsTelnetWrite( ewsContext, buf);
	}
	
    return cliSyntaxReturnPrompt (ewsContext, "");
   
}

/*********************************************************************
*
* @purpose   Display the Mail Server Config.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax in stacking env: show logging  [unit]
* @cmdsyntax in non-stacking env: show logging
*
* @cmdhelp
*
* @cmddescript
    ewsTelnetWrite( ewsContext,"\r\n");
  ewsTelnetWrite( ewsContext,"\r\n");
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoggingMailServerConfig(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 numArg;
   L7_RC_t rc = L7_FAILURE;
   L7_RC_t rc1= L7_FAILURE;	 
	L7_uint32 securityProtocol;	
	L7_char8 username[L7_EMAIL_ALERT_USERNAME_STRING_SIZE];	
	L7_char8 passwd[L7_EMAIL_ALERT_PASSWD_STRING_SIZE];	
	L7_uint32   port;	
	L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];	
	L7_uint32   noOfMailServers=0;	
	L7_inet_addr_t   mailServerAddr;
   L7_char8 mailServerAddrStr[L7_EMAIL_ALERT_SMTP_MAX_ADDRESS_LEN];	
	L7_uint32 ipAddr = L7_NULL;	
	L7_IP_ADDRESS_TYPE_t   type = L7_IP_ADDRESS_TYPE_UNKNOWN;	
	

	
   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 2) )
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_Showmailserverconfig_1);
   }   

	OSAPI_STRNCPY_SAFE(mailServerAddrStr, argv[index + 1]);

	if(osapiStrCaseCmp(mailServerAddrStr, "all",strlen("all")) != 0)
	{
		rc = usmDbIPHostAddressValidate(mailServerAddrStr,&ipAddr,&type);
		if(rc == L7_ERROR)
		{
		  /* Invalid Ip Address */
		  return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
				  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
		}
		else if( rc == L7_FAILURE)
		{
		  /* It can be either a invalid host name or can be a IPV6 address */
		  /* Convert the IP Address to Inet Address */
		  if(usmDbIPv6HostAddressValidate(mailServerAddrStr, &mailServerAddr,&type) != L7_SUCCESS)
		  {
		    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
			    ewsContext, pStrErr_common_MailServerInValIp);
		  }
		  if( (type != L7_IP_ADDRESS_TYPE_IPV6)&&(type != L7_IP_ADDRESS_TYPE_DNS))
		  {
		      /* Invalid Host Name*/
		      cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
				    ewsContext, pStrErr_base_InvalidHostName);
		  }
		}
			

		rc1 = usmDbLogEmailAlertMailServerAddressGet(USMDB_UNIT_CURRENT, type, mailServerAddrStr);

		if(rc1 == L7_SUCCESS)
		{
			memset (buf, 0,sizeof(buf));
		    cliFormat(ewsContext, pStrInfo_base_EmailAlertHost);															
			/* eventually set to default */
			osapiSnprintf(buf, sizeof(buf), "%s ",mailServerAddrStr);
			ewsTelnetWrite( ewsContext, buf);

		    rc = usmDbLogEmailAlertMailServerPortGet(USMDB_UNIT_CURRENT, mailServerAddrStr,&port);

		    if (rc != L7_FAILURE)
		    {
		      memset (buf, 0,sizeof(buf));
 		      cliFormat(ewsContext, pStrInfo_base_EmailAlertPort);																				
		      /* eventually set to default */
		  	  osapiSnprintf(buf, sizeof(buf), "%d ",port);
				 
		      ewsTelnetWrite( ewsContext, buf);
		    }

		    rc = usmDbLogEmailAlertMailServerSecurityGet(USMDB_UNIT_CURRENT, mailServerAddrStr,&securityProtocol);

		    if (rc != L7_FAILURE)
		    {
		      memset (buf, 0,sizeof(buf));
 		      cliFormat(ewsContext, pStrInfo_base_EmailAlertSecurityProtocol);																									
		      /* eventually set to default */
			   if(securityProtocol == L7_LOG_EMAIL_ALERT_TLSV1)
			   {
		      		osapiSnprintf(buf, sizeof(buf), "tlsv1");
			   }
				else
				{
		      		osapiSnprintf(buf, sizeof(buf), "none");
				}
				 
		      ewsTelnetWrite( ewsContext, buf);
		    }

		    rc = usmDbLogEmailAlertMailServerUsernameGet(USMDB_UNIT_CURRENT,mailServerAddrStr, username);

		    if (rc != L7_FAILURE)
		    {
		      memset (buf, 0,sizeof(buf));
 		      cliFormat(ewsContext, pStrInfo_base_EmailAlertUsername);																														
		      /* eventually set to default */
		  	  osapiSnprintf(buf, sizeof(buf), username);
				 
		      ewsTelnetWrite( ewsContext, buf);
		    }

		    rc = usmDbLogEmailAlertMailServerPasswdGet(USMDB_UNIT_CURRENT,mailServerAddrStr, passwd);

		    if (rc != L7_FAILURE)
		    {
		      memset (buf, 0,sizeof(buf));
 		      cliFormat(ewsContext, pStrInfo_base_EmailAlertPassword );																																			
		      /* eventually set to default */
		  	  osapiSnprintf(buf, sizeof(buf), passwd);
				 
		      ewsTelnetWrite( ewsContext, buf);
		    }

			return cliSyntaxReturnPrompt (ewsContext, "");
		}
		else
		{
			return cliSyntaxReturnPrompt (ewsContext, "This Mail Server Not Configured");
		}
		
	}

	rc1 = usmDbLogEmailAlertFirstMailServerAddressGet(USMDB_UNIT_CURRENT, &type, mailServerAddrStr);

	while(rc1 == L7_SUCCESS)
	{
		noOfMailServers++;
		rc1 = usmDbLogEmailAlertNextMailServerAddressGet(USMDB_UNIT_CURRENT, &type,mailServerAddrStr);
	}

	memset (buf, 0,sizeof(buf));
	OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_EmailAlertMailServersConfig );
	ewsTelnetWrite( ewsContext, buf);		 
	ewsTelnetWrite( ewsContext, "\n");
	
    memset (buf, 0,sizeof(buf));
    cliFormat(ewsContext, pStrInfo_base_EmailAlertNoOfMailServers);																				
    osapiSnprintf(buf, sizeof(buf), "%d\n ",noOfMailServers);
    ewsTelnetWrite( ewsContext, buf);
	
	rc1 = usmDbLogEmailAlertFirstMailServerAddressGet(USMDB_UNIT_CURRENT, &type, mailServerAddrStr);

	while (rc1 == L7_SUCCESS)
	{
		memset (buf, 0,sizeof(buf));
	    cliFormat(ewsContext, pStrInfo_base_EmailAlertHost);															
		/* eventually set to default */
		osapiSnprintf(buf, sizeof(buf), "%s ",mailServerAddrStr);
		ewsTelnetWrite( ewsContext, buf);

	    rc = usmDbLogEmailAlertMailServerPortGet(USMDB_UNIT_CURRENT, mailServerAddrStr,&port);

	    if (rc != L7_FAILURE)
	    {
	      memset (buf, 0,sizeof(buf));
		      cliFormat(ewsContext, pStrInfo_base_EmailAlertPort);																				
	      /* eventually set to default */
	  	  osapiSnprintf(buf, sizeof(buf), "%d ",port);
			 
	      ewsTelnetWrite( ewsContext, buf);
	    }

	    rc = usmDbLogEmailAlertMailServerSecurityGet(USMDB_UNIT_CURRENT, mailServerAddrStr,&securityProtocol);

	    if (rc != L7_FAILURE)
	    {
	      memset (buf, 0,sizeof(buf));
		      cliFormat(ewsContext, pStrInfo_base_EmailAlertSecurityProtocol);																									
	      /* eventually set to default */
		   if(securityProtocol == L7_LOG_EMAIL_ALERT_TLSV1)
		   {
	      		osapiSnprintf(buf, sizeof(buf), "tlsv1");
		   }
			else
			{
	      		osapiSnprintf(buf, sizeof(buf), "none");
			}
			 
	      ewsTelnetWrite( ewsContext, buf);
	    }

	    rc = usmDbLogEmailAlertMailServerUsernameGet(USMDB_UNIT_CURRENT,mailServerAddrStr, username);

	    if (rc != L7_FAILURE)
	    {
	      memset (buf, 0,sizeof(buf));
		      cliFormat(ewsContext, pStrInfo_base_EmailAlertUsername);																														
	      /* eventually set to default */
	  	  osapiSnprintf(buf, sizeof(buf), username);
			 
	      ewsTelnetWrite( ewsContext, buf);
	    }

	    rc = usmDbLogEmailAlertMailServerPasswdGet(USMDB_UNIT_CURRENT,mailServerAddrStr, passwd);

	    if (rc != L7_FAILURE)
	    {
	      memset (buf, 0,sizeof(buf));
		      cliFormat(ewsContext, pStrInfo_base_EmailAlertPassword );																																			
	      /* eventually set to default */
	  	  osapiSnprintf(buf, sizeof(buf), passwd);
			 
	      ewsTelnetWrite( ewsContext, buf);
	    }

        ewsTelnetWrite( ewsContext, "\n");	

		rc1 = usmDbLogEmailAlertNextMailServerAddressGet(USMDB_UNIT_CURRENT, &type,mailServerAddrStr);
			
	}

    return cliSyntaxReturnPrompt (ewsContext, "");

}


/*********************************************************************
*
* @purpose   Display the Message Log entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax in stacking env: show logging buffered [unit]
* @cmdsyntax in non-stacking env: show logging buffered
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoggingBuffered(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32  count, stringLength;
   static L7_uint32 logIndex, bufIndex;
   /*static L7_uint32 logIndex, bufIndex;*/
   static L7_char8 string[L7_CLI_MAX_STRING_LENGTH*3]; /* protect ourselves from strings greater than what the MAX is */
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_RC_t rc = L7_SUCCESS;
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_ADMIN_MODE_t status;

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
   L7_COMPONENT_IDS_t component;
#endif
   L7_LOG_FULL_BEHAVIOR_t behavior;
   L7_uint32 logCount;

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLoggingBufed_1);
   }

   if (numArg == 0)
   {
      unit = L7_ALL_UNITS;
   }
   else if (numArg == 1)
   {
      sscanf(argv[index+1], "%d", &unit);
   }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowLoggingBufed_1);
  }

  count = 0;

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    /* Admin Mode */
    rc = usmDbLogInMemoryAdminStatusGet (unit, &status);

    if (rc != L7_FAILURE)
    {
      memset (stat, 0,sizeof(stat));
      memset (buf, 0,sizeof(buf));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_BufedInMemoryLogging);
      /* eventually set to default */
      if (status ==   L7_ADMIN_MODE_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld_1);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld2);
      }

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }
#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
    /* Component */
    rc = usmDbLogInMemoryComponentFilterGet (unit, &component);
    if (rc != L7_FAILURE)
    {

      memset (stat, 0, sizeof(stat));
      memset (buf, 0,sizeof(buf));
      memset (valString, 0,sizeof(valString));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_BufedLoggingCompFilter);
      rc = usmDbComponentMnemonicGet(component, valString);
      if (rc != L7_SUCCESS)
      {

        if (strcmp (valString, pStrInfo_common_EmptyString) == 0)
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_ApProfileRadioScanFrequencyAll);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), valString);
        }

        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }
    }
#endif
    /* Wrapping */
    rc = usmDbLogInMemoryBehaviorGet (unit, &behavior);
    if (rc != L7_FAILURE)
    {
      memset (stat, 0, sizeof(stat));
      memset (buf, 0,sizeof(buf));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_BufedLoggingWrappingBehavior);

      if (behavior ==  L7_LOG_WRAP_BEHAVIOR)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_ApProfileRadioStatusOn);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_OffLogWillStopWhenFull);
      }

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
    }

    /* Count */
    rc = usmDbLogInMemoryLogCountGet (unit, &logCount);

    if (rc != L7_FAILURE)
    {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_BufedLogCount);
      osapiSnprintf(buf, sizeof(buf), "%d", logCount);

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);
      cliSyntaxBottom(ewsContext);
    }
    else
    {
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    }

    /* If first time into function, set pointer to null to get first entry. */
    memset(string, 0, L7_CLI_MAX_STRING_LENGTH*2);
    /* ptr = L7_NULLPTR; */

    logIndex = 0;
    rc = usmDbLogMsgInMemoryGetNext(unit, logIndex, string, &bufIndex);
    if (rc == L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      if (string[0] != '\0')
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_BufedLogEmpty);
      }
    }

    count = 7;  /*to take care of above output to console*/
   }

   for (; count < CLI_MAX_SCROLL_LINES-6; count++)
   {
      logIndex = bufIndex;
      if (rc != L7_SUCCESS)
      {
         /*output string one last time to get oldest entry in message log*/
         stringLength = strlen(string);

         if (stringLength >= 2 &&
             string[stringLength-2] != '\r' && string[stringLength-1] == '\n')
         {
            string[stringLength-1] = '\r';
            string[stringLength]   = '\n';
            string[stringLength+1] = '\0';
         }                         /* don't cut off the last char if there is no \n */
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
      /* without the \r, the lines are not output correctly on the telnet screen. */
      stringLength = strlen(string);

      if (stringLength >= 2)
      {
         if (string[stringLength-2] == '\n' && string[stringLength-1] == '\n')
         {
            string[stringLength-2] = '\r';
            string[stringLength] = '\0';
         }                          /* don't cut off the last char if there is no \n */
         else if (string[stringLength-2] != '\r' && string[stringLength-1] == '\n')
         {
            string[stringLength-1] = '\r';
            string[stringLength]   = '\n';
            string[stringLength+1]   = '\0';
         }                          /* don't cut off the last char if there is no \n */
         else if (string[stringLength-2] != '\r' && string[stringLength-1] != '\n')
         {
            string[stringLength] = '\r';
            string[stringLength+1]   = '\n';
            string[stringLength+2]   = '\0';
         }
      }

      if (stringLength >= MAX_STRING_LEN)
      {
         count++;
      }
      ewsTelnetWrite(ewsContext, string);
      memset(string, 0, L7_CLI_MAX_STRING_LENGTH*2);
      rc = usmDbLogMsgInMemoryGetNext(unit, logIndex, string, &bufIndex);
   }

   cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

   if (numArg == 1)
   {
    osapiSnprintf(buf, sizeof(buf), pStrInfo_base_ShowLoggingBufed_2, unit);
      cliAlternateCommandSet(buf);
   }
   else
   {
    cliAlternateCommandSet(pStrInfo_base_ShowLoggingBufedCmd);
   }

  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose   Display the Message Log entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax in stacking env: show logging persistent [unit]
* @cmdsyntax in non-stacking env: show logging persistent
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoggingPersistent(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 count, stringLength;
   static L7_uint32 logIndex, bufIndex;
   static L7_char8 string[L7_LOG_PERSISTENT_MESSAGE_LENGTH]; /* protect ourselves from strings greater than what the MAX is */
   L7_uint32 unit;
   L7_uint32 numArg;
   static L7_RC_t rc;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_ADMIN_MODE_t status;
   L7_uint32 logCount;   /* validity check (arguments, Access */

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
   {
      cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLoggingPersistent_1);
   }

   if (numArg == 0)
   {
      unit = L7_ALL_UNITS;
   }
   else if (numArg == 1)
   {
      sscanf(argv[index+1], "%d", &unit);
   }
   else
   {
      cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowLoggingPersistent_1);
  }

  count = 0;

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    cliSyntaxTop(ewsContext);
    /* Admin Mode */
    rc = usmDbLogPersistentAdminStatusGet (unit, &status);

    if (rc != L7_FAILURE)
    {
      memset (stat, 0,sizeof(stat));
      memset (buf, 0,sizeof(buf));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowPersistentLogging);
      /* eventually set to default */
      if (status ==   L7_ADMIN_MODE_ENABLE)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld_1);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld2);
      }

      OSAPI_STRNCAT(stat, buf);
         ewsTelnetWrite( ewsContext, stat);
      }

      /* Log Count */
      rc = usmDbLogPersistentLogCountGet (unit, &logCount);

      if (rc != L7_FAILURE)
      {

      memset (stat, 0, sizeof(stat));
      memset (buf, 0,sizeof(buf));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_PersistentLogCount);
      osapiSnprintf(buf, sizeof(buf), "%d", logCount);

      OSAPI_STRNCAT(stat, buf);
         ewsTelnetWrite( ewsContext, stat);
      }

      /* If first time into function, set pointer to null to get first entry. */
      memset(string, 0, L7_LOG_PERSISTENT_MESSAGE_LENGTH);
      /* ptr = L7_NULLPTR; */

      logIndex = 0;
      rc = usmDbLogMsgPersistentGetNext(unit, logIndex, string, &bufIndex);
      if (rc == L7_SUCCESS)
      {

         cliSyntaxTop(ewsContext);
         if (string[0] != '\0')
         {
            stringLength = strlen(string);

            if (stringLength >= 2)
            {

               if (string[stringLength-2] != '\r' && string[stringLength-1] == '\n')
               {
                  string[stringLength-1] = '\r';
                  string[stringLength]   = '\n';
                  string[stringLength+1]   = '\0';
               }                          /* don't cut off the last char if there is no \n */
               else if (string[stringLength-2] != '\r' && string[stringLength-1] != '\n')
               {
                  string[stringLength] = '\r';
                  string[stringLength+1]   = '\n';
                  string[stringLength+2]   = '\0';
               }
            }
            /* ewsTelnetWrite(ewsContext, string); */
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         }
         else
         {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LogPersistentEmpty);
         }
      }

      count = 7; /*to take care of above outputto console*/
   }

   cliSyntaxTop(ewsContext);
   for (; count < CLI_MAX_SCROLL_LINES-6; count++)
   {

      logIndex = bufIndex;
      if (rc != L7_SUCCESS)
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
      /* without the \r, the lines are not output correctly on the telnet screen. */
      stringLength = strlen(string);
      if (stringLength >= 2)
      {

         if (string[stringLength-2] != '\r' && string[stringLength-1] == '\n')
         {
            string[stringLength-1] = '\r';
            string[stringLength]   = '\n';
            string[stringLength+1]   = '\0';
         }                          /* don't cut off the last char if there is no \n */
         else if (string[stringLength-2] != '\r' && string[stringLength-1] != '\n')
         {
            string[stringLength] = '\r';
            string[stringLength+1]   = '\n';
            string[stringLength+2]   = '\0';
         }

      }
      ewsTelnetWrite(ewsContext, string);
      rc = usmDbLogMsgPersistentGetNext(unit, logIndex, string, &bufIndex);
   }

   cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

   if (numArg == 1)
   {
    osapiSnprintf(buf, sizeof(buf), pStrInfo_base_ShowLoggingPersistent_2, unit);
      cliAlternateCommandSet(buf);
   }
   else
  {
    cliAlternateCommandSet(pStrInfo_base_ShowLoggingPersistentCmd);
  }

  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose   Display the Message Log entries.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax in stacking env: show logging host [unit]
* @cmdsyntax in non-stacking env: show logging host
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoggingHosts(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 count = 0;
   static L7_uint32 logIndex, bufIndex;
   static L7_char8 string[L7_CLI_MAX_STRING_LENGTH*2]; /* protect ourselves from strings greater than what the MAX is */
   L7_ROW_STATUS_t status;
   L7_uint32 unit = 0;
   L7_uint32 numArg;
   L7_RC_t rc = L7_SUCCESS;
   L7_char8 buffer[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 ipAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 valString[L7_CLI_MAX_STRING_LENGTH];
   L7_ushort16 port;
   L7_LOG_SEVERITY_t severity;
   L7_uint32 val;
   L7_BOOL allUnits;
   L7_char8 rowString[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 splitLabel[L7_CLI_MAX_STRING_LENGTH];
   L7_BOOL firstRow = L7_TRUE;
   L7_uchar8 *hostAddrTraverse = L7_NULLPTR;

   cliSyntaxTop(ewsContext);
   cliCmdScrollSet( L7_FALSE);

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLoggingHosts_1);
   }
   else if ((numArg != 1) && (cliIsStackingSupported()) == L7_TRUE)
   {
#ifndef L7_CHASSIS
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowLoggingHostsStacking);
#else
    /* Initialise the maganer's unit number */
    if (L7_SUCCESS != unitMgrNumberGet(&unit))
    {
      unit = 0;
    }
#endif
  }
  else if (numArg == 1)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &val) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_INVALID_UNIT);
    }

#ifdef L7_CHASSIS
    if (val < L7_CHASSIS_MIN_SLOT_NUMBER || val > L7_CHASSIS_MAX_SLOT_NUMBER)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_INVALID_UNIT);
    }
#endif     

    /* check validity of input unit */
    if ((rc = cliValidUnitCheck(argv[index+1], &unit, &allUnits)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWrite(ewsContext, CLIERROR_INVALID_UNIT);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetPrintf (ewsContext, CLIUNIT_X_DOES_NOT_EXIST, (L7_int32)val);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }
      
        return cliSyntaxReturnPrompt (ewsContext, "");
   }
  }
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {

    /* If first time into function, set pointer to null to get first entry. */
    memset(string, 0, L7_CLI_MAX_STRING_LENGTH*2);
    /* ptr = L7_NULLPTR; */

    logIndex = 0;
    rc = usmDbLogHostTableRowStatusGetNext(unit, logIndex, &status, &bufIndex);
    if (rc == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_IdxIpAddrSeverityPortStatus);
      ewsTelnetWrite(ewsContext,"\r\n----- ------------------------  ----------  ------ -------------");
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LoggingHostListEmpty);
      }
   }

   for (count = 0; ((CLI_MAX_SCROLL_LINES-6) - count) > CLI_MAX_LINES_IN_PAGE;)
   {

      logIndex = bufIndex;
      if (rc == L7_SUCCESS)
      {
         memset (ipAddr, 0,sizeof(ipAddr));
         rc = usmDbLogHostTableIpAddressGet(unit, logIndex, ipAddr);
         if (rc == L7_SUCCESS)
         {
           hostAddrTraverse = ipAddr;
         }
         else
         {
           return cliSyntaxReturnPrompt (ewsContext, "");
         }
         
         while (*hostAddrTraverse  != L7_EOS)
         {
           if (firstRow == L7_TRUE)
           {
             cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
             osapiSnprintf(rowString, sizeof(rowString), "\r\n%-7u%-26s",
                           logIndex, splitLabel);
             memset (buffer, 0, sizeof(buffer));
             rc = usmDbLogHostTableSeverityGet(unit, logIndex, &severity);
             if (rc == L7_SUCCESS)
             {
               memset (valString, 0,sizeof(valString));
               rc = cliLogConvertSeverityLevelToString(severity, 
                    valString, sizeof(valString));

               if (rc == L7_SUCCESS)
               {
                 osapiSnprintf (buffer, sizeof(buffer), "%-12s",valString);
                 OSAPI_STRNCAT(rowString, buffer);
               }
             }
             else
             {
               return cliSyntaxReturnPrompt (ewsContext, "");
             }

             memset (buffer, 0, sizeof(buffer));
             rc = usmDbLogHostTablePortGet(unit, logIndex, &port);
             if (rc == L7_SUCCESS)
             {
               osapiSnprintf (buffer, sizeof(buffer), "%-6d", port);
               OSAPI_STRNCAT(rowString, buffer);
             }
             else
             {
               return cliSyntaxReturnPrompt (ewsContext, "");
             }
             memset (buffer, 0, sizeof(buffer));
             /* Status */
             switch (status)
             {
             case L7_ROW_STATUS_INVALID:
              osapiSnprintf(buffer,sizeof(buffer),"%-13s", 
                            pStrInfo_common_Invalid_1);
                break;
             case L7_ROW_STATUS_ACTIVE:
              osapiSnprintf(buffer,sizeof(buffer),"%-13s", 
                            pStrInfo_common_WsActive);
                break;
             case L7_ROW_STATUS_NOT_IN_SERVICE:
              osapiSnprintf(buffer,sizeof(buffer),"%-13s", 
                            pStrErr_base_NotInService);
                break;
             case     L7_ROW_STATUS_NOT_READY:
              osapiSnprintf(buffer,sizeof(buffer),"%-13s", 
                            pStrErr_base_NotReady);
                break;
             case L7_ROW_STATUS_CREATE_AND_GO:
              osapiSnprintf(buffer,sizeof(buffer),"%-13s", 
                            pStrInfo_base_CreateAndGo);
                break;
             case L7_ROW_STATUS_CREATE_AND_WAIT:
              osapiSnprintf(buffer,sizeof(buffer),"%-13s", 
                            pStrInfo_base_Wait);
                break;
             case L7_ROW_STATUS_DESTROY:
              osapiSnprintf(buffer,sizeof(buffer),"%-13s", 
                            pStrInfo_base_Destroy);
                break;
             }
             OSAPI_STRNCAT(rowString, buffer);
             ewsTelnetWrite( ewsContext, rowString);
             firstRow = L7_FALSE;
             count++;
           }
           else /* end of if (firstRow == L7_TRUE)*/
           {
              /* Displaying remaining hostname in second Row */
              memset (rowString, 0, sizeof(rowString));
              memset (splitLabel, 0, sizeof(splitLabel));
              /* Get the split hostname which fits in available space 
               * (24 characters) in a row 
               */
              osapiSnprintf(rowString, sizeof(rowString), "%s", "\r\n       ");
              cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
              OSAPI_STRNCAT(rowString, splitLabel);
              ewsTelnetWrite( ewsContext, rowString);
              count++;
           }
         } /* end of while */
      }
      firstRow = L7_TRUE;
      rc = usmDbLogHostTableRowStatusGetNext(unit, logIndex, &status, 
                                             &bufIndex);
      if (logIndex == bufIndex)
      {
        break;
      }
  }  /* end of for loop */

   if ((((CLI_MAX_SCROLL_LINES-6) - count) < 10) && (logIndex != bufIndex))
   {

      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

      if (numArg == 1)
      {
      osapiSnprintf(buffer,sizeof(buffer),pStrInfo_base_ShowLoggingHosts_2, unit);
         cliAlternateCommandSet(buffer);
      }
      else
      {
      cliAlternateCommandSet(pStrInfo_base_ShowLoggingHostsCmd);
      }
    return pStrInfo_common_Name_2;     /* --More-- or (q)uit */
   }

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose  display switch description information
*
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
* @cmdsyntax  show sysinfo
*
* @cmdhelp Display information about the system including system up time.
*
* @cmddescript shows the system information for your switch.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowSysinfo(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];              /* Multiple uses and size requirements. */
   L7_RC_t rc;
   L7_uint32 numwrites;
   usmDbTimeSpec_t  ts;
   static L7_uint32 idx;
#ifndef FEAT_METRO_CPE_V1_0
   L7_char8 descr[L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE];
   L7_char8 name[L7_SNMP_SUPPORTED_MIB_NAME_SIZE];
#endif
   L7_char8 stat[256];            /* Multiple uses including displaying time. */
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */
   L7_uint32 val = 0;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   numArg = cliNumFunctionArgsGet();

   cliCmdScrollSet( L7_FALSE);
   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowSysinfo_1);
   }
   else
   {
      if (cliGetCharInputID() != CLI_INPUT_EMPTY)
      {                                                                     /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
         {
            idx = 0;
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliPrompt(ewsContext);
         }
         numwrites = 0;
      }
      else
      {
         numwrites = 0;

      cliFormat(ewsContext, pStrInfo_common_SysDesc_1);
      memset (buf, 0, sizeof(buf));
     rc = usmDb1213SysDescrGet(unit, buf);
     if (rc == L7_SUCCESS)
     {
       cliFormatStringDisplay(ewsContext, buf);
     }
     else
     {
       ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
     }

      cliFormat(ewsContext, pStrInfo_common_SysName);
         rc = usmDb1213SysNameGet(unit, stat);
         ewsTelnetWrite(ewsContext, stat);

      cliFormat(ewsContext, pStrInfo_base_SysLocation);
         rc= usmDb1213SysLocationGet(unit, stat);
         ewsTelnetWrite(ewsContext, stat);

      cliFormat(ewsContext, pStrInfo_base_SysContact);
         rc = usmDb1213SysContactGet(unit, stat);
         ewsTelnetWrite(ewsContext, stat);

      cliFormat(ewsContext, pStrInfo_base_SysObjectId);
      memset (buf, 0, sizeof(buf));
         rc = usmDb1213SysObjectIDGet(unit, buf);
         ewsTelnetWrite(ewsContext, buf);

      cliFormat(ewsContext, pStrInfo_common_ApSysUpTime);
      memset (stat, 0, sizeof(stat));
         usmDb1213SysUpTimeGet(unit, &ts);
      ewsTelnetPrintf (ewsContext, pStrInfo_common_DaysHrsMinsSecs, ts.days, ts.hours, ts.minutes, ts.seconds);

      cliFormat(ewsContext, pStrInfo_base_SysSntpSyncTime);
      memset(stat, 0, sizeof(stat) );
         rc = usmDbSntpLastUpdateTimeGet (unit, &val);

         if (rc != L7_SUCCESS || val == 0)
         {
        osapiSnprintf(stat, sizeof(stat), pStrErr_base_SysSntpNotSync);
         }
         else
         {
        osapiStrncpySafe(buf, (const char *)usmDbConvertTimeToDateString((L7_uint32)osapiUTCTimeNow()), min(20, (sizeof(buf)-1)));
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, buf,pStrInfo_base_Utc);
        osapiSnprintf(stat,sizeof(stat),buf);
      }
      ewsTelnetWrite( ewsContext, stat);

      cliSyntaxBottom(ewsContext);
         numwrites += 12;

         idx = 1;
      }
   }

#ifndef FEAT_METRO_CPE_V1_0
   ewsTelnetPrintf (ewsContext, "\r\n%s:\r\n", pStrInfo_base_MibsSupported);
   cliSyntaxBottom(ewsContext);
   if (usmDbSnmpSupportedMibTableEntryGet(unit, idx) == L7_SUCCESS)
   {
      do
      {
      memset (buf, 0,sizeof(buf));
      memset (name, 0,sizeof(name));
      memset (descr, 0,sizeof(descr));
         rc = usmDbSnmpSupportedMibTableMibNameGet(unit, idx, name);
         rc = usmDbSnmpSupportedMibTableMibDescriptionGet(unit, idx, descr);
         if (rc == L7_SUCCESS)
         {
        osapiSnprintf(buf, sizeof(buf), cliCombineStringOutput(name, 33, descr, 45));
            do
            {
          ewsTelnetPrintf (ewsContext, "%s\r\n", buf);
          numwrites++;

          osapiSnprintf(buf, sizeof(buf), cliCombineStringOutput(name, 33, descr, 45));
            }
            while (strcmp(buf, name) != 0);
         }
         idx++;
         rc = usmDbSnmpSupportedMibTableEntryNextGet(unit, &idx);
         if (rc != L7_SUCCESS)
         {
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliPrompt(ewsContext);
         }
      }
      while (numwrites < CLI_MAX_SCROLL_LINES-3);
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_base_ShowSysinfoCmd);
    return pStrInfo_common_Name_2;     /* --More-- or (q)uit */
   }
#endif

   ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
   return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  display summary statistics for a specific port or for the entire switch
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes need to add argument and range checking
*
* @cmdsyntax  show interface [ <slot/port> | switchport]
*
* @cmdhelp Display summary statistics for a port or for the switch.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowInterfaceSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 val, high, low;
   L7_RC_t rc;
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_int32 interface = 0;
   L7_int32 logicalSlot;
   L7_int32 logicalPort;
   usmDbTimeSpec_t ts;
   L7_uint32 argSlotPort=1;
   L7_uint32 argMode=1;
   L7_uint32 itype;
   L7_uint32 unit;
   L7_char8 strToken[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowStats_1, cliSyntaxInterfaceHelp());
   }

   if(strlen(argv[index+1]) > L7_CLI_MAX_STRING_LENGTH)
   {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  OSAPI_STRNCPY_SAFE(strToken,argv[index+ argMode]);
  cliConvertToLowerCase(strToken);

  /* convert command line token to lower case */
  if (strlen(argv[index+1]) <= L7_CLI_MAX_STRING_LENGTH)
  {
    OSAPI_STRNCPY_SAFE(strToken, argv[index+1]);
    cliConvertToLowerCase(strToken);
  }

  if (strcmp (strToken,pStrInfo_base_Switchport) == 0)
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrErr_common_PktsRcvdWithout);           /* Packets Received Without Error*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_RX_BCAST_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_BcastPktsRcvd);            /* Broadcast Packets Received*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_MAC_ERROR_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrErr_common_PktsRcvdWith);            /* Packets Received With Error*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrErr_common_PktsTxedWithoutErrs);          /* Packets Transmitted Without Error*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_TX_BCAST_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_base_BcastPktsTxed);         /*  Broadcast Packets Transmitted*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_ERROR_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrErr_common_TxPktErrs);         /* Transmit Packet Errors */
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbActiveAddrEntriesGet(unit, &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_base_AddrEntriesCurrentlyInUse);          /*  Address Entries Currently In Use*/
      ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbDot1qNumVlansGet(unit, &val) == L7_SUCCESS)
      {
      cliFormat(ewsContext, pStrInfo_base_VlanEntriesCurrentlyInUse);           /*   VLAN Entries Currently In Use*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    cliFormat(ewsContext, pStrInfo_common_TimeSinceCountersLastClred);
    memset (stat, 0, sizeof(stat));
    rc = usmDbTimeSinceLastSwitchStatsResetGet(unit, &ts);
    ewsTelnetPrintf (ewsContext, pStrErr_common_ShowTime, ts.days, ts.hours, ts.minutes, ts.seconds);
  }
  else
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &logicalSlot, &logicalPort)) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
         if (usmDbIntIfNumFromUSPGet(unit, logicalSlot, logicalPort, &interface) != L7_SUCCESS)
         {
      return cliPrompt(ewsContext);
    }

    if (usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    if (itype == L7_LOGICAL_VLAN_INTF ||
        itype == L7_CPU_INTF ||
        itype == L7_LOOPBACK_INTF ||
        itype == L7_TUNNEL_INTF)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else
    {
      /* PACKETS RECEIVED SUCCESSFULLY */
      if (usmDbStatGet64(unit, L7_CTR_RX_TOTAL_FRAMES, interface, &high, &low) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        cliFormat(ewsContext, pStrErr_common_PktsRcvdWithout);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_ERROR_FRAMES, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_common_PktsRcvdWith);       /*Packets Received With Error*/
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_RX_BCAST_FRAMES, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_common_BcastPktsRcvd);      /*Broadcast Packets Received*/
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_FRAMES, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_common_PktsTxedWithoutErrs);     /*Packets Transmitted Without Error*/
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_ERROR_FRAMES, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrErr_common_TxPktErrs);      /*Transmit Packet Errors*/
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_COLLISION_FRAMES, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_common_CollisionFrames);      /*Collision Frames*/
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbTimeSinceLastStatsResetGet(unit, interface, &ts) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_common_TimeSinceCountersLastClred);
        ewsTelnetPrintf (ewsContext, pStrErr_common_ShowTime, ts.days, ts.hours, ts.minutes, ts.seconds);
      }
    }
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display detailed statistics for a specific port or for the entire switch
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show interface ethernet [ <slot/port> | switchport ]
*
* @cmdhelp Display detailed statistics for a port or for the switch.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowInterfacesEthernet(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 val, high, low;
   L7_RC_t rc;
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 swtCmd[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 buf[1024];
   L7_int32 interface = 0;
   L7_int32 slot;
   L7_int32 port;
   usmDbTimeSpec_t ts;
   L7_uint32 argSlotPort=1;
   L7_uint32 itype;
   L7_uint32 unit;
   L7_char8 strToken[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowStatsPort, cliSyntaxInterfaceHelp());
  }

  /* convert command line token to lower case */
  if (strlen(argv[index+1]) <= L7_CLI_MAX_STRING_LENGTH)
  {
    OSAPI_STRNCPY_SAFE(strToken, argv[index+1]);
    cliConvertToLowerCase(strToken);
  }

  if (strcmp (strToken,pStrInfo_base_Switchport) == 0)
  {

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {                                                                       /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
         {
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliPrompt(ewsContext);
         }

         /* Address Entries*/

         if (usmDbMostAddrEntriesGet(unit, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_MostAddrEntriesEverUsed);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbActiveAddrEntriesGet(unit, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_base_AddrEntriesCurrentlyInUse);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         cliSyntaxTop(ewsContext);
         /*Vlan Entires*/

         if (usmDbDot1qMaxSupportedVlansGet(unit, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_MaxVlanEntries);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbMostVlanEntriesGet(unit, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_MostVlanEntriesEverUsed);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStaticVlanEntriesGet(unit, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_StaticVlanEntries);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbDynamicVlanEntriesGet(unit, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_DynVlanEntries);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbDot1qVlanNumDeletesGet(unit, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_VlanDels);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      cliFormat(ewsContext, pStrInfo_common_TimeSinceCountersLastClred);
      memset (stat, 0, sizeof(stat));
      rc = usmDbTimeSinceLastSwitchStatsResetGet(unit, &ts);
      ewsTelnetPrintf (ewsContext, pStrErr_common_ShowTime, ts.days, ts.hours, ts.minutes, ts.seconds);
         cliSyntaxBottom(ewsContext);
      }

      else
      {                                                                     /* if our question has been answered */
         /*Receive*/

         if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_BYTES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_TotalPktsRcvdOctets);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrErr_common_PktsRcvdWithout);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_RX_UCAST_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_UcastPktsRcvd);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_RX_MCAST_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_common_McastPktsRcvd);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_RX_BCAST_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_common_BcastPktsRcvd);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_MAC_ERROR_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_base_ReceivePktsDiscarded);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         cliSyntaxTop(ewsContext);
         /*Transmit*/

         if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_BYTES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_OctetsTxed);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrErr_common_PktsTxedWithoutErrs);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_UCAST_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_UcastPktsTxed);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_MCAST_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_McastPktsTxed);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_TX_BCAST_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_base_BcastPktsTxed);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_ERROR_FRAMES, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_base_TxPktsDiscarded);
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

      cliSyntaxBottom(ewsContext);
      osapiSnprintf(swtCmd, sizeof(swtCmd), "%s %s", pStrInfo_base_ShowIntfEtherNetCmd, argv[index+1]);

         cliAlternateCommandSet(swtCmd);
      return pStrInfo_common_Name_2;
      }

   }
   else
   {

    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if (usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    if (itype == L7_LOGICAL_VLAN_INTF ||
        itype == L7_CPU_INTF ||
        itype == L7_LOOPBACK_INTF ||
        itype == L7_TUNNEL_INTF)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else
      {
         /* PACKETS RECEIVED (OCTETS) */
         if (usmDbStatGet64(unit, L7_CTR_RX_TOTAL_BYTES, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_TotalPktsRcvdOctets);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_64, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRcvdSize64);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_65_127, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRcvdRange65And127);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_128_255, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRcvdRange128And255);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_256_511, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRcvdRange256And511);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_512_1023, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRcvdRange512And1023);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_1024_1518, interface, &high, &low)== L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRcvdRange1024And1518);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_1519_1530, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRcvdRange1519And1522);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_GOOD_OVERSIZE, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRcvdGreater1518);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
      }

         /* PACKETS RECEIVED AND TRANSMITTED (counted together) */

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_64, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxSize64);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_65_127, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange65And127);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_128_255, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange128And255);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_256_511, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange256And511);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_512_1023, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange512And1023);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_1024_1518, interface, &high, &low)== L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange1024And1518);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_1519_1522, interface, &high, &low)== L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange1519And1522);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
      }

         /* Hardware supports 1519to2047, not 1523to2047 */
         if (usmDbStatGet64(unit, L7_CTR_TX_RX_1523_2047, interface, &high, &low)== L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange1519And2047);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_2048_4095, interface, &high, &low)== L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange2048And4095);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_RX_4096_9216, interface, &high, &low)== L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsRxAndTxRange4096And9216);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
      }

      /* PACKETS RECEIVED SUCCESSFULLY */
      if (usmDbStatGet64(unit, L7_CTR_RX_TOTAL_FRAMES, interface, &high, &low) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        cliFormat(ewsContext, pStrErr_base_TotalPktsRcvdWithoutErrs);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_RX_UCAST_FRAMES, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_UcastPktsRcvd);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet(unit, L7_CTR_RX_MCAST_FRAMES, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_common_McastPktsRcvd);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
      }

      if (usmDbStatGet(unit, L7_CTR_RX_BCAST_FRAMES, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_common_BcastPktsRcvd);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
      }

         /* PACKETS RECEIVED WITH MAC ERRORS */
         if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_MAC_ERROR_FRAMES, interface, &val) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        cliFormat(ewsContext, pStrErr_base_TotalPktsRcvdWithMacErrs);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_RX_ERROR_OVERSIZE, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_JabbersRcvd);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
      }

         if (usmDbStatGet(unit, L7_CTR_RX_ERROR_UNDERSIZE, interface, &val) == L7_SUCCESS)
         {
           cliFormat(ewsContext, pStrInfo_base_FragmentsUnderSizeRcvd);
           ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_RX_GOOD_UNDERSIZE, interface, &val) == L7_SUCCESS)
         {
           cliFormat(ewsContext, pStrInfo_base_UnderSizeRcvd);
           ewsTelnetPrintf (ewsContext, "%-15u", val);
         }
         if (usmDbStatGet(unit, L7_CTR_RX_ALIGN_ERRORS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_base_AlignmentErrs);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_RX_FCS_ERRORS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_base_FcsErrs);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_RX_OVERRUNS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_base_Overruns);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         /* RECEIVED PACKETS NOT FORWARDED */
         if (usmDbStatGet64(unit, L7_CTR_DOT1Q_RX_TOTAL_NOT_FORWARDED_FRAMES, interface, &high, &low) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        cliFormat(ewsContext, pStrErr_base_TotalRcvdPktsNotForwarded);
        ewsTelnetPrintf (ewsContext, "%-15s", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_LOCAL_TRAFFIC_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_LocalTrafficFrames);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_RX_PAUSE_FRAMES, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_802Dot3XPauseFramesRcvd);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_UNACCEPTABLE_FRAME_TYPE_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_UnAcceptableFrameType);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_INGRESS_FILTER_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_VlanMbrshipMismatch);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_INGRESS_VLAN_VIABLE_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_VlanViableDiscards);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_MULTICAST_TREE_VIABLE_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_McastTreeViableDiscards);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_RESERVED_ADDRESS_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_ReservedAddrDiscards);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }
         /* counter is not supported in H/W. xWeb uses the usmdb call in a different way, so can not disable
            in application */
#if 0
         if (usmDbStatGet(unit, L7_CTR_DOT1Q_BCAST_STORM_RECOVERY_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_BcastStormRecovery);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }
#endif

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_CFI_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_CfiDiscards);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_UPSTREAM_THRESHOLD_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_UpstreamThresh);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
      }

         /* PACKETS TRANSMITTED (OCTETS) */
         if (usmDbStatGet64(unit, L7_CTR_TX_TOTAL_BYTES, interface, &high, &low) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        cliFormat(ewsContext, pStrInfo_base_TotalPktsTxedOctets);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_64, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsTxedSize64);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_65_127, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsTxedRange65And127);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_128_255, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsTxedRange128And255);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_256_511, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsTxedRange256And511);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_512_1023, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsTxedRange512And1023);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_1024_1518, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsTxedRange1024And1518);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_1519_1530, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PktsTxedRange1519And1522);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet(unit, L7_CTR_TX_OVERSIZED, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_TxOversized);
        ewsTelnetPrintf (ewsContext, "%-12u", val);
         }


         /* Check for Jumbo Frames Feature Support */
         if (usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID, L7_NIM_JUMBOFRAMES_FEATURE_ID) == L7_TRUE)
         {
            if (usmDbDot1dTpPortMaxInfoGet(unit, interface, &val) == L7_SUCCESS)
            {
          cliFormat(ewsContext, pStrInfo_base_MaxInfo);
          ewsTelnetPrintf (ewsContext, "%-15u", val);
        }
      }

         /* PACKETS TRANSMITTED SUCCESSFULLY */
         if (usmDbStatGet64(unit, L7_CTR_TX_TOTAL_FRAMES, interface, &high, &low) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        cliFormat(ewsContext, pStrInfo_base_TotalPktsTxedSuccessfully);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet64(unit, L7_CTR_TX_UCAST_FRAMES, interface, &high, &low) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_UcastPktsTxed);
        ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
         }

         if (usmDbStatGet(unit, L7_CTR_TX_MCAST_FRAMES, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_McastPktsTxed);
        ewsTelnetPrintf (ewsContext, "%-12u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_BCAST_FRAMES, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_BcastPktsTxed);
        ewsTelnetPrintf (ewsContext, "%-10u", val);
         }

         /* TRANSMIT ERRORS */
         if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_ERROR_FRAMES, interface, &val) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        cliFormat(ewsContext, pStrErr_common_ApStatTotalTxErrs);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_FCS_ERRORS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_base_FcsErrs);
        ewsTelnetPrintf (ewsContext, "%-12u", val);
         }


         if (usmDbStatGet(unit, L7_CTR_TX_UNDERRUN_ERRORS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_base_UnderRunErrs);
        ewsTelnetPrintf (ewsContext, "%-10u", val);
      }

         /* TRANSMIT DISCARDS */
         if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_COLLISION_FRAMES, interface, &val) == L7_SUCCESS)
         {
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        cliFormat(ewsContext, pStrInfo_base_TotalTxPktsDiscarded);
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_ONE_COLLISION, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_SingleCollisionFrames);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_MULTIPLE_COLLISION, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_MultipleCollisionFrames);
        ewsTelnetPrintf (ewsContext, "%-12u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_EXCESSIVE_COLLISION, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_ExcessiveCollisionFrames);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_PORT_MEMBERSHIP_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_PortMbrshipDiscards);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_EGRESS_VLAN_VIABLE_DISCARDS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_VlanViableDiscards);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
      }

         /* PROTOCOL STATISTICS */

      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
         /*ewsTelnetWrite(ewsContext, "Protocol Statistics");     */

         if (usmDbStatGet(unit, L7_CTR_RX_BPDU_COUNT, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_BpdusRcvd_1);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_BPDU_COUNT, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_BpdusTxed);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_DOT1Q_TX_PAUSE_FRAMES, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_802Dot3XPauseFramesTxed);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_RX_GVRP_PDU_COUNT, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_GvrpPdusRcvd_1);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_GVRP_PDU_COUNT, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_GvrpPdusTxed);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_GVRP_FAILED_REGISTRATIONS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_base_GvrpFailedRegistrations);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
      }

         if (usmDbStatGet(unit, L7_CTR_RX_GMRP_PDU_COUNT, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_GmrpPdusRcvd);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_TX_GMRP_PDU_COUNT, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_base_GmrpPdusTxed);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
         }

         if (usmDbStatGet(unit, L7_CTR_GMRP_FAILED_REGISTRATIONS, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrErr_base_GmrpFailedRegistrations);
        ewsTelnetPrintf (ewsContext, "%-15u", val);
      }

            /* Spanning Tree Statistics */
            cliSyntaxNewLine(ewsContext);
            cliSpanTreeDisplayPortStats( ewsContext, interface );

         /* Dot1x Statistics */
         cliSyntaxNewLine(ewsContext);
         if (usmDbDot1xPortEapolFramesTxGet(unit, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_common_EapolFramesTxed);       /* EAPOL Frames Transmitted */
        ewsTelnetPrintf (ewsContext, "%u", val);
         }

         if (usmDbDot1xPortEapolStartFramesRxGet(unit, interface, &val) == L7_SUCCESS)
         {
        cliFormat(ewsContext, pStrInfo_common_EapolStartFramesRcvd);       /* EAPOL Start Frames Received*/
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbTimeSinceLastStatsResetGet(unit, interface, &ts) == L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        cliFormat(ewsContext, pStrInfo_common_TimeSinceCountersLastClred);
        ewsTelnetPrintf (ewsContext, pStrErr_common_ShowTime, ts.days, ts.hours, ts.minutes, ts.seconds);
         }

         cliSyntaxBottom(ewsContext);
      }
   }

   return cliPrompt(ewsContext);
}

/*************Switching System Info Commands added - End   **********/
/*********************************************************************
*
* @purpose  To display SNTP info
*
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
* @cmdsyntax show sntp
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowSntp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit, numArg, val;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_RC_t rc;
   L7_char8 bufString[L7_CLI_MAX_STRING_LENGTH];
   L7_SNTP_PACKET_STATUS_t status;

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowSntp_1);
   }

   if (numArg == 0)
   {
      unit = L7_ALL_UNITS;
   }
   else if (numArg == 1)
   {
      sscanf(argv[index+1], "%d", &unit);
   }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowSntp_1);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {

      rc = usmDbSntpLastUpdateTimeGet (unit, &val);

      if (rc != L7_FAILURE)
      {

      memset (buf, 0,sizeof(buf));
      memset (bufString, 0,sizeof(bufString));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 18, L7_NULLPTR, bufString, pStrInfo_base_LastUpdateTime);
      osapiStrncpySafe(buf, usmDbConvertTimeToDateString(val), min(21, (sizeof(buf)-1)));

      osapiSnprintf(stat,sizeof(stat), buf);
      OSAPI_STRNCAT(bufString, stat);

      ewsTelnetWrite( ewsContext, bufString);

    }

      rc = usmDbSntpLastAttemptTimeGet (unit, &val);
      if (rc != L7_FAILURE)
      {

      memset (bufString, 0,sizeof(bufString));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 9, L7_NULLPTR, bufString, pStrInfo_base_LastUcastAttemptTime);

      osapiStrncpySafe(buf, usmDbConvertTimeToDateString(val), min(21, (sizeof(buf)-1)));

      osapiSnprintf(stat,sizeof(stat), buf);
      OSAPI_STRNCAT(bufString, stat);

         ewsTelnetWrite( ewsContext, bufString);

      }

      rc = usmDbSntpLastAttemptStatusGet (unit, &status);
      if (rc != L7_FAILURE)
      {
      ewsTelnetWriteAddBlanks (1, 0, 0, 15, L7_NULLPTR, ewsContext, pStrInfo_base_LastAttemptStatus);
      ewsTelnetWrite( ewsContext, strUtilSntpStatusGet(status, L7_NULLPTR));
      }

      rc = usmDbSntpBroadcastCountGet (unit, &val);
      if (rc != L7_FAILURE)
      {

      memset (buf, 0,sizeof(buf));
      memset (bufString, 0,sizeof(bufString));
      OSAPI_STRNCAT_ADD_BLANKS (2, 0, 0, 19, L7_NULLPTR, bufString, pStrInfo_base_BcastCount);
      osapiSnprintf(buf, sizeof(buf), "%d", val);

      OSAPI_STRNCAT(bufString, buf);
      ewsTelnetWrite( ewsContext, bufString);

    }

  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To display SNTP server table
*
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
* @cmdsyntax show sntp server
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowSntpServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   static L7_uint32 logIndex, bufIndex;
   static L7_char8 string[L7_CLI_MAX_STRING_LENGTH*2]; /* protect ourselves from strings greater than what the MAX is */
   L7_uint32 unit = 1, val, i, lineCount;
   static L7_uint32 maxServEntries;
   L7_uint32 numArg;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_RC_t rc;
   L7_char8 ipAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_SNTP_SERVER_STATUS_t serverStatus;
   L7_SNTP_ADDRESS_TYPE_t addressType;
   L7_SNTP_PACKET_STATUS_t packetStatus;
   L7_ushort16 priority, version, port;
   L7_char8 valString[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 tempLogIndex;

   cliCmdScrollSet( L7_FALSE);

   if (cliGetCharInputID() != CLI_INPUT_EMPTY)
   {                                                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
      lineCount = 0;
   }
   else
   {
      cliSyntaxTop(ewsContext);
      numArg = cliNumFunctionArgsGet();

      if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowSntpSrvr_1);
      }

      if (numArg == 0)
      {
         unit = L7_ALL_UNITS;
      }
      else if (numArg == 1)
      {
         sscanf(argv[index+1], "%d", &unit);
      }
      else
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowSntpSrvr_1);
      }

      if (usmDbSntpServerAddressTypeGet (unit, &val) == L7_SUCCESS)
      {
         if (val != L7_SNTP_ADDRESS_UNKNOWN)
         {
            rc = usmDbSntpServerIpAddressGet (unit, valString);
            if (rc == L7_SUCCESS)
            {
          memset (buf, 0,sizeof(buf));
          memset (stat, 0,sizeof(valString));
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 15, L7_NULLPTR, stat, pStrInfo_base_SrvrHostAddr_1);
          osapiSnprintf(buf, sizeof(buf), valString);
              OSAPI_STRNCAT(stat, buf);
              ewsTelnetWrite( ewsContext, stat);
            }
         }
         else
         {
            /* If address type is unknown, print server ip address as blank */
        memset (stat, 0, L7_CLI_MAX_STRING_LENGTH);
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 15, L7_NULLPTR, stat, pStrInfo_base_SrvrHostAddr_1);
            ewsTelnetWrite( ewsContext, stat);
         }
      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 23, L7_NULLPTR, stat, pStrInfo_base_SrvrType);

         if (val == L7_SNTP_ADDRESS_UNKNOWN)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Unknown_2);
      }
         else if (val == L7_SNTP_ADDRESS_IPV4)
      {
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Ipv4_2);
      }
         else if (val == L7_SNTP_ADDRESS_DNS)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_Dns_1);
      }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
         else if (val == L7_SNTP_ADDRESS_IPV6)
         {
           osapiSnprintf(buf, sizeof(buf), pStrInfo_base_IPv6);
         }
#endif
          OSAPI_STRNCAT(stat, buf);
          ewsTelnetWrite( ewsContext, stat);
      }

      rc = usmDbSntpServerStratumGet (unit, &val);
      if (rc != L7_FAILURE)
      {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 20, L7_NULLPTR, stat, pStrInfo_base_SrvrStratum);
      osapiSnprintf(buf, sizeof(buf), "%d", val);

      OSAPI_STRNCAT(stat, buf);
         ewsTelnetWrite( ewsContext, stat);
      }
      rc = usmDbSntpServerRefIdGet (unit, valString);
      if (rc != L7_FAILURE)
      {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 15, L7_NULLPTR, stat, pStrInfo_base_SrvrRefId);
      osapiSnprintf(buf, sizeof(buf), valString);

      OSAPI_STRNCAT(stat, buf);
         ewsTelnetWrite( ewsContext, stat);
      }

      rc = usmDbSntpServerModeGet (unit, &val);
      if (rc != L7_FAILURE)
      {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 23, L7_NULLPTR, stat, pStrInfo_base_SrvrMode);

         if (val == L7_SNTP_SERVER_MODE_RESERVED)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_Reserved);
      }
         else if (val == L7_SNTP_SERVER_MODE_SYMMETRIC_ACTIVE)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SymmetricActive);
      }
         else if (val == L7_SNTP_SERVER_MODE_SYMMETRIC_PASSIVE)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_SymmetricPassive);
      }
         else if (val == L7_SNTP_SERVER_MODE_CLIENT)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Client);
      }
         else if (val == L7_SNTP_SERVER_MODE_SERVER)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Srvr);
      }
         else if (val == L7_SNTP_SERVER_MODE_BROADCAST)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Bcast);
      }
         else if (val == L7_SNTP_SERVER_MODE_RESERVED_CONTROL)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_ReservedCntrl);
      }
         else if (val == L7_SNTP_SERVER_MODE_RESERVED_PRIVATE)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_ReservedPrivate);
      }

      OSAPI_STRNCAT(stat, buf);
         ewsTelnetWrite( ewsContext, stat);

      }

      rc = usmDbSntpServerMaxEntriesGet (unit, &maxServEntries);
      if (rc != L7_FAILURE)
      {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 12, L7_NULLPTR, stat, pStrInfo_base_SrvrMaxEntries);
      osapiSnprintf(buf, sizeof(buf), "%d", maxServEntries);

      OSAPI_STRNCAT(stat, buf);
         ewsTelnetWrite( ewsContext, stat);

      }

      rc = usmDbSntpServerCurrentEntriesGet (unit, &val);
      if (rc != L7_FAILURE)
      {

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 12, L7_NULLPTR, stat, pStrInfo_base_SrvrCurrentEntries);
      osapiSnprintf(buf, sizeof(buf), "%d", val);

      OSAPI_STRNCAT(stat, buf);
      ewsTelnetWrite( ewsContext, stat);

    }

    /* If first time into function, set pointer to null to get first entry. */
    memset (string, 0, sizeof(string));
      /* ptr = L7_NULLPTR; */

      logIndex = 0;
      rc = L7_NOT_EXIST;
      for (i = 0; i <= maxServEntries; logIndex++, i++)
      {
         rc = usmDbSntpServerTableRowStatusGetNext(unit, logIndex, &serverStatus, &bufIndex);
         if (rc == L7_SUCCESS)
      {
        break;
      }
      logIndex = bufIndex;
    }

    if (rc == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SntpSrvrs);
         ewsTelnetWrite(ewsContext,"\r\n------------");

      }
      else
      {

         cliSyntaxBottom(ewsContext);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NoSntpSrvrsExist);
      }
      lineCount = 10;
   }

   while ((lineCount < 20))
   {
      if (logIndex >= maxServEntries)
      {
      return cliSyntaxReturnPrompt (ewsContext, "");
      }
      rc = usmDbSntpServerTableRowStatusGetNext(unit, logIndex, &serverStatus, &bufIndex);
      if (rc != L7_SUCCESS)
      {
         logIndex++;
         continue;
      }
      else if (logIndex != bufIndex)
      {
         logIndex = bufIndex;
         lineCount = lineCount + 10;

         rc = usmDbSntpServerTableIpAddressGet(unit, logIndex, ipAddr);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetWriteAddBlanks (2, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_HostAddr_1);
        ewsTelnetPrintf (ewsContext, ipAddr);
      }
      else
      {
        return cliSyntaxReturnPrompt (ewsContext, "");
         }

         rc = usmDbSntpServerTableAddressTypeGet(unit, logIndex, &addressType);
         if (rc == L7_SUCCESS)
         {

        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_AddrType_1);
        memset (buf, 0,L7_CLI_MAX_STRING_LENGTH);

            if (addressType == L7_SNTP_ADDRESS_UNKNOWN)
        {
          osapiSnprintf(buf,sizeof(buf),pStrInfo_common_Unknown_2 );
        }
          else if (addressType == L7_SNTP_ADDRESS_IPV4)
        {
            osapiSnprintf(buf,sizeof(buf),pStrInfo_common_Ipv4_2);
        }
          else if (addressType == L7_SNTP_ADDRESS_DNS)
        {
          osapiSnprintf(buf,sizeof(buf),pStrInfo_base_Dns);
        }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
          else if (addressType == L7_SNTP_ADDRESS_IPV6)
          {
            osapiSnprintf(buf,sizeof(buf),pStrInfo_base_IPv6);
          }
#endif
            ewsTelnetWrite( ewsContext, buf);
         }
         else
         {
        return cliSyntaxReturnPrompt (ewsContext, "");
         }

         rc = usmDbSntpServerTablePriorityGet(unit, logIndex, &priority);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_Pri_2);
        ewsTelnetPrintf (ewsContext, "%d",priority);
         }

         rc = usmDbSntpServerTableVersionGet(unit, logIndex, &version);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_Ver_1);
        ewsTelnetPrintf (ewsContext, "%d",version);
         }

         rc = usmDbSntpServerTablePortGet(unit, logIndex, &port);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_Port_3);
        ewsTelnetPrintf (ewsContext, "%d",port);
         }

         rc = usmDbSntpServerStatsLastUpdateTimeGet(unit, logIndex, &val);
         if ((rc == L7_SUCCESS) && (val != 0))
         {

        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_LastUpdateTime);
        memset (stat, 0,sizeof(stat));
        osapiStrncpySafe(buf, usmDbConvertTimeToDateString(val), min(21, (sizeof(buf)-1)));
        ewsTelnetPrintf (ewsContext, buf);
         }

         rc = usmDbSntpServerStatsLastAttemptTimeGet(unit, logIndex, &val);
         if (rc == L7_SUCCESS)
         {

        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_LastAttemptTime);
        memset (stat, 0,sizeof(stat));
        osapiStrncpySafe(buf, usmDbConvertTimeToDateString(val), min(21, (sizeof(buf)-1)));
        ewsTelnetPrintf (ewsContext, buf);
         }

         rc = usmDbSntpServerStatsLastUpdateStatusGet(unit, logIndex, &packetStatus);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_LastUpdateStatus);
        ewsTelnetWrite( ewsContext, strUtilSntpStatusGet(packetStatus, L7_NULLPTR));
         }

         rc = usmDbSntpServerStatsUnicastServerNumRequestsGet(unit, logIndex, &val);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_TotalUcastReqs);
        ewsTelnetPrintf (ewsContext, "%d",val);
         }

         rc = usmDbSntpServerStatsUnicastServerNumFailedRequestsGet(unit, logIndex, &val);
         if (rc == L7_SUCCESS)
         {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_FailedUcastReqs);
        ewsTelnetPrintf (ewsContext, "%d",val);
      }
    }
    else

    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    tempLogIndex = logIndex;
    do
    {
      if (tempLogIndex >= maxServEntries)
      {
         return cliSyntaxReturnPrompt (ewsContext, "");
      }
      rc = usmDbSntpServerTableRowStatusGetNext(unit, tempLogIndex, &serverStatus, &bufIndex);
      if (rc != L7_SUCCESS)
      {
        tempLogIndex++;
        continue;
      }
    }while (rc != L7_SUCCESS); 

  }

  if (logIndex >= maxServEntries)
  {
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

    cliSyntaxBottom(ewsContext);

    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
    for (i=1; i<argc; i++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[i]);
    }
    cliAlternateCommandSet(cmdBuf);

    return pStrInfo_common_Name_2;     /* --More-- or (q)uit */
  }

}

/*********************************************************************
*
* @purpose  To display SNTP client info
*
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
* @cmdsyntax show sntp client
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowSntpClient(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit, val;
   L7_uint32 numArg;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 bufString[L7_CLI_MAX_STRING_LENGTH];
   L7_RC_t rc;
   L7_ushort16  port;
   L7_SNTP_SUPPORTED_MODE_t  supportedMode;
   L7_SNTP_CLIENT_MODE_t  mode;

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if ((numArg != 0) && (cliIsStackingSupported() != L7_TRUE))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowSntpClient_1);
   }

   if (numArg == 0)
   {
      unit = L7_ALL_UNITS;
   }
   else if (numArg == 1)
   {
      sscanf(argv[index+1], "%d", &unit);
   }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowSntpSrvr_1);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliPrompt(ewsContext);
      }
   }
   else
   {
      rc = usmDbSntpClientSupportedModeGet (unit, &supportedMode);

    memset (buf, 0,sizeof(buf));
    memset (bufString, 0,sizeof(bufString));
    OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 14, L7_NULLPTR, bufString, pStrInfo_base_ClientSupportedModes);
      if (rc == L7_SUCCESS)
      {
         if (L7_SNTP_SUPPORTED_UNICAST == supportedMode)
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, bufString, pStrInfo_common_Ucast_1);
      }
         if (L7_SNTP_SUPPORTED_BROADCAST == supportedMode)
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, bufString, pStrInfo_common_Bcast_1);
      }
         if (L7_SNTP_SUPPORTED_UNICAST_AND_BROADCAST == supportedMode)
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, bufString, pStrInfo_common_Ucast_and_Bcast_1);
      }

    }
    else
    {
      OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, bufString, pStrInfo_common_Dsbld2);
      }
      ewsTelnetWrite( ewsContext, bufString);

      rc = usmDbSntpClientVersionGet (unit, &val);
      if (rc == L7_SUCCESS)
      {
      memset (buf, 0,sizeof(buf));
      memset (bufString, 0,sizeof(bufString));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 24, L7_NULLPTR, bufString, pStrInfo_base_SntpVer);
      osapiSnprintf(buf, sizeof(buf), "%d", val);

      OSAPI_STRNCAT(bufString, buf);
         ewsTelnetWrite( ewsContext, bufString);

      }

      rc = usmDbSntpClientPortGet (unit, &port);
      if (rc == L7_SUCCESS)
      {
      memset (buf, 0,sizeof(buf));
      memset (bufString, 0,sizeof(bufString));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 32, L7_NULLPTR, bufString, pStrInfo_base_Port_3);
      osapiSnprintf(buf, sizeof(buf), "%d", port);

      OSAPI_STRNCAT(bufString, buf);
         ewsTelnetWrite( ewsContext, bufString);
      }

      rc = usmDbSntpClientModeGet (unit, &mode);
      if (rc == L7_SUCCESS)
      {

      memset (buf, 0,sizeof(buf));
      memset (bufString, 0,sizeof(bufString));
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 25, L7_NULLPTR, bufString, pStrInfo_base_ClientMode_1);
         if (mode == L7_SNTP_CLIENT_DISABLED)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld2);
      }
         else if (mode == L7_SNTP_CLIENT_UNICAST)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Ucast_1);
      }
      else if (mode == L7_SNTP_CLIENT_BROADCAST)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Bcast_1);
      }

      OSAPI_STRNCAT(bufString, buf);
         ewsTelnetWrite( ewsContext, bufString);

      }

      if (mode != L7_SNTP_CLIENT_DISABLED)
      {

      memset (buf, 0,sizeof(buf));
      memset (bufString, 0,sizeof(bufString));
         if (mode == L7_SNTP_CLIENT_UNICAST)
         {
            rc = usmDbSntpClientUnicastPollIntervalGet (unit, &val);
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 15, L7_NULLPTR, bufString, pStrInfo_base_UcastPollIntvl_1);
         }
         else if (mode == L7_SNTP_CLIENT_BROADCAST)
         {
            rc = usmDbSntpClientBroadcastPollIntervalGet (unit, &val);
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 13, L7_NULLPTR, bufString, pStrInfo_base_BcastPollIntvl_1);
         }

         if (rc == L7_SUCCESS)
         {
        osapiSnprintf(buf, sizeof(buf), "%d", val);
        OSAPI_STRNCAT(bufString, buf);
            ewsTelnetWrite( ewsContext, bufString);
         }
         if (mode == L7_SNTP_CLIENT_UNICAST)
         {

            rc = usmDbSntpClientUnicastPollTimeoutGet (unit, &val);
            if (rc != L7_FAILURE)
            {
          memset (buf, 0,sizeof(buf));
          memset (bufString, 0,sizeof(bufString));
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 14, L7_NULLPTR, bufString, pStrInfo_base_PollTimeoutSeconds);
          osapiSnprintf(buf, sizeof(buf), "%d", val);

          OSAPI_STRNCAT(bufString, buf);
          ewsTelnetWrite( ewsContext, bufString);
        }

            rc = usmDbSntpClientUnicastPollRetryGet (unit, &val);
            if (rc != L7_FAILURE)
            {
          memset (buf, 0,sizeof(buf));
          memset (bufString, 0,sizeof(bufString));
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 26, L7_NULLPTR, bufString, pStrInfo_base_PollRetry_1);
          osapiSnprintf(buf, sizeof(buf), "%d", val);

          OSAPI_STRNCAT(bufString, buf);
          ewsTelnetWrite( ewsContext, bufString);
        }
      }
    }

  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To display status of various keyable functions
*
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
* @cmdsyntax show key-features
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowKeyFeatures(EwsContext ewsContext,
                                       L7_uint32 argc,
    const L7_char8 * * argv,
                                       L7_uint32 index)
{
   L7_RC_t   rc;
   L7_uint32 unit, numArg;
   L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 componentName[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 intComponentId = 0;
   L7_uint32 intNextComponentId = 0;
   L7_uint32 mode = 0;
   L7_uint32 keyable = 0;

   cliSyntaxTop(ewsContext);

   unit = cliGetUnitId();
   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowKeyFeat);
  }

  memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_FuncStatus);
   ewsTelnetWrite( ewsContext, "\r\n---------        -------");

   rc = usmDbFeatureGetFirst(unit, &intComponentId);

   while (rc == L7_SUCCESS)
   {
      usmDbFeatureKeyGet(unit, intComponentId, &mode, &keyable);

      if (keyable == L7_TRUE)
      {
      memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
      memset (componentName, 0, sizeof(componentName));
         usmDbComponentNameGet(intComponentId, componentName);
      ewsTelnetPrintf (ewsContext, "\r\n%-17s", componentName);

      ewsTelnetPrintf (ewsContext, strUtilEnableDisableGet(mode,pStrInfo_common_Dsbl_1));
      }
      rc = usmDbFeatureGetNext(unit, intComponentId, &intNextComponentId);
      intComponentId = intNextComponentId;
   }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

#ifdef L7_OUTBOUND_TELNET_PACKAGE
/*********************************************************************
*
* @purpose     Display Outbound Telnet config info such as max sessions
*              and login timeout and whether new outbound telnet
*              sessions are allowed or not
*
*
* @param       EwsContext ewsContext
* @param       L7_uint32 argc
* @param       const L7_char8 **argv
* @param       L7_uint32 index
*
* @returntype  const L7_char8  *
* @returns     cliPrompt(ewsContext)
*
* @notes       Add argument checking
*
* @cmdsyntax   show telnet
*
* @cmdhelp     Display Outbound Telnet configuration information.
*
* @cmddescript
*   Show Outbound Telnet settings such as login timeout, the max number
*   of sessions, and if new Outbound Telnet sessions are allowed or not.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowOutboundTelnet(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 val;
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowOutboundTelnet_1);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext,pStrInfo_base_OutboundTelnetLoginTimeoutMinutes);
  memset (stat, 0,sizeof(stat));
   rc = usmDbTelnetTimeoutGet(unit, &val);
   if (val == 0)
   {
    ewsTelnetPrintf (ewsContext, pStrInfo_base_OutboundTelnetInfiniteTimeout);
  }
  else
  {
    ewsTelnetPrintf (ewsContext, "%d",val);
  }

  cliFormat(ewsContext,pStrInfo_base_MaxNumOfOutboundTelnetSessions);
  memset (stat, 0,sizeof(stat));
   rc = usmDbTelnetMaxSessionsGet(unit, &val);
  ewsTelnetPrintf (ewsContext, "%d",val);

  cliFormat(ewsContext,pStrInfo_base_AllowNewOutboundTelnetSessions);
  memset (stat, 0,sizeof(stat));
   rc = usmDbTelnetAdminModeGet(unit, &val);
   switch (val)                                      /* val = yes no "---" */
   {
   case L7_ENABLE:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_Yes);
      break;
   case L7_DISABLE:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_No);
      break;
   default:
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_No);
   }
  return cliSyntaxReturnPrompt (ewsContext,stat);
}
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
/*********************************************************************
 *
 * @purpose     Displays the information description, ifIndex, Bit Offset, Physical Address for an interface.
 *
 * @param       EwsContext ewsContext
 * @param       L7_uint32 argc
 * @param       const L7_char8 **argv
 * @param       L7_uint32 index
 *
 * @returntype  const L7_char8  *
 * @returns     cliPrompt(ewsContext)
 *
 * @notes
 *
 * @cmdsyntax   stacking environment: show port description {<unit/slot/port>|loopback <loopback-id>|tunnel <tunnel-id>} 
 * @cmdsyntax   non-stacking environment:  show port description {<slot/port>|loopback <loopback-id>|tunnel <tunnel-id>}
 *
 * @cmddescript
 * @end
 *
***********************************************************************/
const L7_char8 *commandShowPortDescription(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 maskOffset;
   L7_uint32 numArg;
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unit, slot, port;
   L7_uint32 interface;
   L7_uint32 argSlotPort=1;
#ifdef L7_RLIM_PACKAGE
   L7_uint32 argId=2;
#endif /* end of L7_RLIM_PACKAGE */
   L7_int32  id = L7_NULL;
   L7_BOOL   isTunnel=L7_FALSE;
   L7_BOOL   isLoopback=L7_FALSE;
   L7_int32 retVal;
   L7_int32 val;
   L7_uchar8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];

   cliSyntaxTop(ewsContext);
   cliCmdScrollSet( L7_FALSE);

   numArg = cliNumFunctionArgsGet();

   if (numArg != 1)
   {
#ifdef L7_RLIM_PACKAGE
     if ((osapiStrncmp(argv[index+argSlotPort], pStrInfo_common_LoopBack, sizeof(pStrInfo_common_LoopBack))==0) && (numArg==2))
     {
       isLoopback = L7_TRUE;
       if(cliValidateLoopbackId(ewsContext, (L7_char8 *)argv[index+argId],
                                &id) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidLoopBackId);
       }
       if(usmDbRlimLoopbackIntIfNumGet(id, &interface) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_IntfNotExisting);
       }
     }
#ifdef L7_IPV6_PACKAGE
     else if ((osapiStrncmp(argv[index+argSlotPort], pStrInfo_common_Tunnel_1, sizeof(pStrInfo_common_Tunnel_1))==0) && (numArg==2))
     {
       isTunnel = L7_TRUE;
       if(cliValidateTunnelId(ewsContext, (L7_char8 *)argv[index+argId],
                              &id) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidTunnel);
       }
      if(usmDbRlimTunnelIntIfNumGet(id, &interface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_IntfNotExisting);
      }
     }
#endif /* end of L7_IPV6_PACKAGE */
     else
     {
       return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowPortDescwithloopbacktunnel, cliSyntaxInterfaceHelp());
     }
#else
     return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowPortDesc, cliSyntaxInterfaceHelp());
#endif /* end of L7_RLIM_PACKAGE */
   }

   if((isTunnel == L7_FALSE) && (isLoopback == L7_FALSE))
   { 
     if((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
     {
       return cliPrompt(ewsContext);
     }
     if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
     {
        return cliPrompt(ewsContext);
     }
     if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) != L7_SUCCESS)
     {
       if (retVal == -2)
       {
         return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_InvalidLag, cliDisplayInterfaceHelp(unit, slot, port));
       }
       else if (retVal == -1)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
       }
     }
   }

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWrite(ewsContext, pStrInfo_common_Intf);
   ewsTelnetWrite(ewsContext, ".......");

  if (isLoopback == L7_TRUE)
  {
    osapiSnprintf(stat, sizeof(stat), "Loopback %d", id);
  }
  else if (isTunnel == L7_TRUE)
  {
    osapiSnprintf(stat, sizeof(stat), "Tunnel %d", id);
  }
  else
  { 
    if (cliIsStackingSupported() == L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat), "%u/%u/%u", unit, slot, port);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%u/%u", slot, port);
    }
  }

  ewsTelnetWrite(ewsContext, stat);

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWrite(ewsContext, pStrInfo_common_IfIdx);
   ewsTelnetWrite(ewsContext, "......... ");
   if (usmDbIfIndexGet(unit, interface, &val) == L7_SUCCESS)
   {
    ewsTelnetPrintf (ewsContext, "%d", val);
  }

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWrite(ewsContext, pStrInfo_common_Desc_1);
   ewsTelnetWrite(ewsContext, "..... ");
   if (usmDbIfAliasGet(unit, interface, stat) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_Space);
  }

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWrite(ewsContext, pStrInfo_common_MacAddr_2);
   ewsTelnetWrite(ewsContext, "..... ");
   if (usmDbIfPhysAddressGet(unit, interface, strMacAddr) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),"%02X:%02X:%02X:%02X:%02X:%02X",
        strMacAddr[0],strMacAddr[1],
              strMacAddr[2],strMacAddr[3],
              strMacAddr[4],strMacAddr[5]);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }

  ewsTelnetWrite(ewsContext, stat);

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWrite(ewsContext, pStrInfo_base_BitOffsetVal);
   ewsTelnetWrite(ewsContext, ".. ");
   if (usmDbConfigIdMaskOffsetGet(interface, &maskOffset) == L7_SUCCESS)
   {
    ewsTelnetPrintf (ewsContext, "%d", maskOffset);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose  displays the information needed for trouble-shooting.
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
* @cmdsyntax show tech-support
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowTechSupport(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_techSupportCurrentData_t *techSupportCurrData = L7_NULL;
  localOptions_t currentOption;
  L7_uint32 interface;

  L7_uint32 numWrites = 0;
  usmDbTrapLogEntry_t trapLog;
  L7_uint32 first=0;
  static L7_uint32 trapLogCount = 0;
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
#ifndef FEAT_METRO_CPE_V1_0
  L7_uchar8 dummy[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL addpkgs = L7_FALSE;
#endif
  L7_RC_t rc = L7_FALSE;
  L7_uint32 unit, len;
  L7_uint32 numArg;
  L7_uint32 nextInterface;
#ifndef FEAT_METRO_CPE_V1_0
  L7_char8 descr[L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE];
  L7_char8 name[L7_SNMP_SUPPORTED_MIB_NAME_SIZE];
#endif
  L7_LOG_FULL_BEHAVIOR_t behavior;
  L7_uint32 virtualIfCount = 0;
  L7_BOOL txFlag,rxFlag;
  L7_uchar8 family = L7_AF_INET;
#ifdef LVL7_DEBUG_BREAKIN
  L7_uint32 current_handle;
#endif

#ifdef L7_ROUTING_PACKAGE
  L7_uint32 nextvid = L7_DOT1Q_DEFAULT_VLAN, intIfNum;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  L7_uint32 i=0;
  L7_uchar8 str[L7_CLI_MAX_STRING_LENGTH];
#endif /*L7_QOS_FLEX_PACKAGE_ACL*/
#endif /*L7_ROUTING_PACKAGE*/

  L7_uint32 logCount =0;

  L7_uint32 count=0, stringLength=0;
  static L7_uchar8 string[L7_CLI_MAX_STRING_LENGTH*3]; /* protect ourselves from strings greater than what the MAX is */
  static L7_uint32 logIndex =0;
  L7_char8 valString[L7_CLI_MAX_STRING_LENGTH];
  L7_ADMIN_MODE_t status;
  L7_LOG_SEVERITY_t severity;
  L7_ushort16 port;
  L7_uint32 cliAdminMode;

  L7_int32 u;
  L7_int32 s;
  L7_int32 p;
  L7_uint32 val;
  L7_uint32 val2;
  L7_uint32 val3;
  L7_int32 retVal;
  L7_INTF_STATES_t state;

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  L7_uint32 SIPTraceFlag, H323TraceFlag, SCCPTraceFlag, MGCPTraceFlag;
#endif

  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);
  numArg = cliNumFunctionArgsGet();

  ewsContext->unbufferedWrite = L7_TRUE;

  if(numArg != 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowInventory);
    cliSyntaxBottom(ewsContext);
    ewsContext->unbufferedWrite = L7_FALSE;
    return cliPrompt(ewsContext);
  }

  if(cliIsStackingSupported() == L7_TRUE)
  {
    rc = usmDbUnitMgrMgrNumberGet(&unit);

    if(rc != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_MgmtUnitId);
      cliSyntaxBottom(ewsContext);
      ewsContext->unbufferedWrite = L7_FALSE;
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
  }
  
  if( cliGetCharInputID() == CLI_INPUT_EMPTY )
  {
    techSupportCurrData = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, sizeof(L7_techSupportCurrentData_t));
    if (L7_NULL == techSupportCurrData)
    {
      ewsTelnetWrite(ewsContext, "\r\nUnable to allocate storage for techSupportCurrData structure.");
      cliSyntaxBottom(ewsContext);
      ewsContext->unbufferedWrite = L7_FALSE;
      return cliPrompt(ewsContext);
    }
    ewsContext->techSupportData = techSupportCurrData;
    ewsContext->configScriptData = L7_NULL;
    techSupportCurrData->currentOption = SHOW_VERSION;
  }
  else
  {
    techSupportCurrData = ewsContext->techSupportData;
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      osapiFree(L7_CLI_WEB_COMPONENT_ID, techSupportCurrData);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      ewsContext->unbufferedWrite = L7_FALSE;
      return cliPrompt(ewsContext);
    }
  }
  currentOption = techSupportCurrData->currentOption;

  if(currentOption == SHOW_VERSION)
  {
    usmDbCodeVersion_t ver;
    L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];

    memset (buf, 0, sizeof(buf));
    ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ShowVerFmt);
    cliSyntaxBottom(ewsContext);

    ewsTelnetPrintf (ewsContext, "\r\n%s: %u", pStrInfo_common_Switch, unit);
    cliSyntaxBottom(ewsContext);

    cliFormat(ewsContext, pStrInfo_common_SysDesc_1);
    memset (buf, 0, sizeof(buf));
    rc = usmDb1213SysDescrGet(unit, buf);
    if (rc == L7_SUCCESS)
    {
      cliFormatStringDisplay(ewsContext, buf);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }

#ifndef FEAT_METRO_CPE_V1_0
    cliFormat(ewsContext, pStrInfo_base_MachineType);
    memset (buf, 0, sizeof(buf));
    rc = usmDbMachineTypeGet(unit, buf);
    if (rc == L7_SUCCESS)
    {
      cliFormatStringDisplay(ewsContext, buf);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }

#endif

    cliFormat(ewsContext, pStrInfo_base_MachineModel);
    memset (buf, 0, sizeof(buf));
    usmDbMachineModelGet(unit, buf);
    ewsTelnetWrite(ewsContext, buf);

    cliFormat(ewsContext, pStrInfo_common_ApSerialNum);
    memset (buf, 0, sizeof(buf));
    usmDbSwDevInfoSerialNumGet(unit, buf);
    ewsTelnetWrite(ewsContext, buf);

#ifndef FEAT_METRO_CPE_V1_0
    cliFormat(ewsContext, pStrInfo_base_FruNum);
    memset (buf, 0, sizeof(buf));
    usmDbFRUNumGet(unit, buf);
    ewsTelnetWrite(ewsContext, buf);

    cliFormat(ewsContext, pStrInfo_common_ApPartNum);
    memset (buf, 0, sizeof(buf));
    usmDbPartNumGet(unit, buf);
    ewsTelnetWrite(ewsContext, buf);
#endif
    cliFormat(ewsContext, pStrInfo_base_MaintenanceLvl);
    memset (buf, 0, sizeof(buf));
    usmDbMaintLevelGet(unit, buf);
    ewsTelnetWrite(ewsContext, buf);

    cliFormat(ewsContext, pStrInfo_base_Manufacturer);
    memset (buf, 0, sizeof(buf));
    usmDbManufacturerGet(unit, buf);
    ewsTelnetWrite(ewsContext, buf);

    cliFormat(ewsContext, pStrInfo_base_BurnedInMacAddr);
    memset (stat, 0,sizeof(stat));
    if(cliIsStackingSupported() == L7_TRUE)
    {
      usmDbUnitMgrUnitIdKeyGet(unit, (L7_enetMacAddr_t *)mac);
    }
    else
    {
      usmDbSwDevCtrlBurnedInMacAddrGet(unit, mac);
    }
    ewsTelnetPrintf (ewsContext, "%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

    cliFormat(ewsContext, pStrInfo_common_ApSoftwareVer);
    memset (buf, 0, sizeof(buf));
    if(cliIsStackingSupported() == L7_TRUE)
    {
      if(usmDbUnitMgrDetectCodeVerRunningGet(unit, &ver) == L7_SUCCESS)
      {
        if(isalpha(ver.rel))
        {
          osapiSnprintf(buf, sizeof(buf), "%c.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "%d.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
        }
      }
    }
    else
    {
      usmDbSwVersionGet(unit, buf);
    }
    ewsTelnetWrite(ewsContext, buf);

#ifndef FEAT_METRO_CPE_V1_0
    memset (buf, 0, sizeof(buf));
    rc = usmDbOperSysGet(unit, buf);
    if(rc == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_OperatingSys);
      ewsTelnetWrite(ewsContext, buf);
    }

    memset (buf, 0, sizeof(buf));
    rc = usmDbNPDGet(unit, buf);
    if(rc == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_base_NwProcessingDevice);
      ewsTelnetWrite(ewsContext, buf);
    }
#endif

#ifndef FEAT_METRO_CPE_V1_0
    cliFormat(ewsContext, pStrInfo_base_AdditionalPackages_1);
    memset (buf, 0, sizeof(buf));
    memset (dummy, 0, sizeof(dummy));
    if(usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
    {
        addpkgs = L7_TRUE;
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_FastPathBgp4);
        osapiSnprintf(dummy, sizeof(dummy), "\r\n%s", "                                                ");
        ewsTelnetWrite(ewsContext, buf);
    }

    if ((usmDbComponentPresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_TRUE) ||
          (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID) == L7_TRUE) ||
          (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID) == L7_TRUE))
    {
        addpkgs = L7_TRUE;
        osapiSnprintf(buf, sizeof(buf), "%s%s", dummy, pStrInfo_base_FastPathQos);
        osapiSnprintf(dummy, sizeof(dummy), "\r\n%s", "                                                ");
        ewsTelnetWrite(ewsContext, buf);
    }

    if(usmDbComponentPresentCheck(unit, L7_FLEX_MCAST_MAP_COMPONENT_ID) == L7_TRUE)
    {
        addpkgs = L7_TRUE;
        osapiSnprintf(buf, sizeof(buf), "%s%s", dummy, pStrInfo_base_FastPathMcast);
        osapiSnprintf(dummy, sizeof(dummy), "\r\n%s", "                                                ");
        ewsTelnetWrite(ewsContext, buf);
    }

    if(usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
    {
        addpkgs = L7_TRUE;
        osapiSnprintf(buf, sizeof(buf), "%s%s", dummy, pStrInfo_base_FastPathStacking);
        osapiSnprintf(dummy, sizeof(dummy), "\r\n%s", "                                                ");
        ewsTelnetWrite(ewsContext, buf);
    }

    if((usmDbComponentPresentCheck(unit, L7_FLEX_METRO_DOT1AD_COMPONENT_ID) == L7_TRUE) ||
         (usmDbComponentPresentCheck(unit, L7_DOT3AH_COMPONENT_ID) == L7_TRUE) ||
         (usmDbComponentPresentCheck(unit, L7_DOT1AG_COMPONENT_ID) == L7_TRUE) ||
         (usmDbComponentPresentCheck(unit, L7_TR069_COMPONENT_ID)  == L7_TRUE))
    {
        addpkgs = L7_TRUE;
        osapiSnprintf(buf, sizeof(buf), "%s%s", dummy, pStrInfo_base_FastPathMetro);
        osapiSnprintf(dummy, sizeof(dummy), "\r\n%s", "                                                ");
        ewsTelnetWrite(ewsContext, buf);
    }

#ifdef L7_ROUTING_PACKAGE
    addpkgs = L7_TRUE;
    osapiSnprintf(buf, sizeof(buf), "%s%s", dummy, pStrInfo_base_FastPathRouting);
    osapiSnprintf(dummy, sizeof(dummy), "\r\n%s", "                                                ");
    ewsTelnetWrite(ewsContext, buf);
#else
    if(addpkgs == L7_FALSE)
    {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_None_1);
    }
#endif
#endif

    techSupportCurrData->flag = L7_TRUE;
    techSupportCurrData->currentOption = SHOW_SYSINFO;
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
    ewsContext->unbufferedWrite = L7_FALSE;
    return pStrInfo_common_Name_2;   
  }

  if(currentOption == SHOW_SYSINFO)
  {

    usmDbTimeSpec_t  ts;
    L7_uint32 speedCount = 0, speedCount1G = 0, speedCount10G = 0;
    L7_uint32 speedCount2p5G = 0;  /* PTin added: Speed 2.5G */
    L7_uint32 speedCount40G  = 0;  /* PTin added: Speed 40G */
    L7_uint32 speedCount100G = 0;  /* PTin added: Speed 100G */
    L7_uint32 numwrites=0;
    L7_uint32 intfSpeed;
    L7_uint32 *idx = &techSupportCurrData->entryIndex; 
    if(techSupportCurrData->flag == L7_TRUE)
    {
      memset (buf, 0, sizeof(buf));
      ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ShowSysinfoFmt);
      cliSyntaxBottom(ewsContext);

      cliFormat(ewsContext, pStrInfo_base_SysLocation);
      usmDb1213SysLocationGet(unit, stat);
      ewsTelnetWrite(ewsContext, stat);

      cliFormat(ewsContext, pStrInfo_base_SysContact);
      usmDb1213SysContactGet(unit, stat);
      ewsTelnetWrite(ewsContext, stat);

      cliFormat(ewsContext, pStrInfo_base_SysObjectId);
      memset (buf, 0, sizeof(buf));
      usmDb1213SysObjectIDGet(unit, buf);
      ewsTelnetWrite(ewsContext, buf);

      cliFormat(ewsContext, pStrInfo_common_ApSysUpTime);
      memset (stat, 0, sizeof(stat));
      usmDb1213SysUpTimeGet(unit, &ts);
      ewsTelnetPrintf (ewsContext, pStrInfo_common_DaysHrsMinsSecs, ts.days, ts.hours, ts.minutes, ts.seconds);

      if(usmDbValidIntIfNumFirstGet(&nextInterface) == L7_SUCCESS)
      {
        do
        {
          interface = nextInterface;
          usmDbIfSpeedGet(unit,interface,&intfSpeed);
          switch( intfSpeed )
          {
          case L7_PORTCTRL_PORTSPEED_HALF_100TX:
          case L7_PORTCTRL_PORTSPEED_FULL_100TX:
          case L7_PORTCTRL_PORTSPEED_FULL_100FX:
          case L7_PORTCTRL_PORTSPEED_HALF_10T:
          case L7_PORTCTRL_PORTSPEED_FULL_10T:
            speedCount++;
            break;

          case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
            speedCount1G++;
            break;

          /* PTin added: Speed 2.5G */
          case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
            speedCount2p5G++;
            break;
          /* PTin end */

          case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
            speedCount10G++;
            break;

          /* PTin added: Speed 40G */
          case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
            speedCount40G++;
            break;

          /* PTin added: Speed 100G */
          case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
            speedCount100G++;
            break;
          /* PTin end */

          default:
            break;
          }
        } while( (usmDbValidIntIfNumNext(interface, &nextInterface)) == L7_SUCCESS);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext,  pStrErr_base_ShowVerIntf);
      }

#ifdef L7_ROUTING_PACKAGE
      if(usmDbVlanIDGet(unit, nextvid) == L7_SUCCESS)
      {
        do
        {
          if(usmDbIpVlanRtrVlanIdToIntIfNum(unit, nextvid, &intIfNum) == L7_SUCCESS)
          {
            virtualIfCount++;
          }
        } while(usmDbIpVlanRtrVlanIdGetNext(unit, &nextvid) == L7_SUCCESS);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,  pStrInfo_base_ShowVerVlanId);
      }
#endif

      cliFormat(ewsContext, pStrInfo_base_PortSpeed10By100);
      ewsTelnetPrintf (ewsContext, "%d", speedCount);

      cliFormat(ewsContext, pStrInfo_base_Portspeed);
      ewsTelnetPrintf (ewsContext, "%d", speedCount1G);

      /* PTin added: Speed 2.5G */
      cliFormat(ewsContext, pStrInfo_base_PortSpeed2500);
      ewsTelnetPrintf (ewsContext, "%d", speedCount2p5G);
      /* PTin end */

      cliFormat(ewsContext, pStrInfo_base_PortSpeed10Gig);
      ewsTelnetPrintf (ewsContext, "%d", speedCount10G);

      /* PTin added: Speed 40G */
      cliFormat(ewsContext, pStrInfo_base_PortSpeed40Gig);
      ewsTelnetPrintf (ewsContext, "%d", speedCount40G);

      /* PTin added: Speed 100G */
      cliFormat(ewsContext, pStrInfo_base_PortSpeed100Gig);
      ewsTelnetPrintf (ewsContext, "%d", speedCount100G);
      /* PTin end */

      cliFormat(ewsContext, pStrInfo_base_ShowTechSupportVirtualEtherNet);
      ewsTelnetPrintf (ewsContext, "%d", virtualIfCount);
#ifndef FEAT_METRO_CPE_V1_0
      cliSyntaxBottom(ewsContext);
      ewsTelnetPrintf (ewsContext, "\r\n%s:\r\n", pStrInfo_base_MibsSupported);
      cliSyntaxBottom(ewsContext);
      *idx = 1;
#endif

      techSupportCurrData->flag = L7_FALSE;
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
      ewsContext->unbufferedWrite = L7_FALSE;
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */

    }

#ifndef FEAT_METRO_CPE_V1_0
    if(usmDbSnmpSupportedMibTableEntryGet(unit, *idx) == L7_SUCCESS)
    {
      do
      {
        memset (buf, 0,sizeof(buf));
        memset (name, 0,sizeof(name));
        memset (descr, 0,sizeof(descr));
        usmDbSnmpSupportedMibTableMibNameGet(unit, *idx, name);
        rc = usmDbSnmpSupportedMibTableMibDescriptionGet(unit, *idx, descr);
        if(rc == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf), cliCombineStringOutput(name, 33, descr, 45));
          do
          {
            ewsTelnetPrintf (ewsContext, "%s\r\n", buf);
            numwrites++;

            osapiSnprintf(buf, sizeof(buf), cliCombineStringOutput(name, 33, descr, 45));
          } while(strcmp(buf, name) != 0);
        }
        *idx = *idx + 1;
        rc = usmDbSnmpSupportedMibTableEntryNextGet(unit, idx);
        if(rc != L7_SUCCESS)
        {
          techSupportCurrData->currentOption = SHOW_SWITCH;
          *idx = 0;
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
          ewsContext->unbufferedWrite = L7_FALSE;
          return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
        }
      } while(numwrites < CLI_MAX_SCROLL_LINES - 3);

      if((rc == L7_SUCCESS) && (numwrites > CLI_MAX_SCROLL_LINES - 3) )
      {
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
        ewsContext->unbufferedWrite = L7_FALSE;
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }
    }
    else
    {
      techSupportCurrData->currentOption = SHOW_SWITCH;
    }
#endif

#ifdef FEAT_METRO_CPE_V1_0
      techSupportCurrData->currentOption = SHOW_SWITCH;
      *idx = 0;
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
      ewsContext->unbufferedWrite = L7_FALSE;
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
#endif
  }

  /* Show Switch */
  if(currentOption == SHOW_SWITCH)
  {
    if ((rc = usmDbUnitMgrStackMemberGetFirst(&unit)) != L7_SUCCESS)
    {
      ewsContext->unbufferedWrite = L7_FALSE;
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitNoneExist);
    }

    cliSyntaxBottom(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 8, 2, L7_NULLPTR, ewsContext,pStrInfo_common_MgmtPreconfigPluggedInSwitchCode);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_SwitchStatusModelIdModelIdStatusVer);
    ewsTelnetWrite(ewsContext,"\r\n------ ------------ ------------- ------------- --------------------- --------\r\n");

    while (rc == L7_SUCCESS)
    {
      memset (buf, 0, (L7_int32)sizeof(buf));
      osapiSnprintf(stat, sizeof(stat), "%u", unit);
      ewsTelnetPrintf (ewsContext, "%-7s", stat);

      memset (buf, 0, (L7_int32)sizeof(buf));
      if (usmDbUnitMgrIsMgmtUnit(unit, &val) == L7_SUCCESS)
      {
        switch (val)
        {
        case L7_USMDB_UNITMGR_MGMT_ENABLED:
          /* unit is currently the manager */
            osapiSnprintf(buf, sizeof(buf), "%-13s", pStrInfo_common_MgmtUnit_2);
          break;

        case L7_USMDB_UNITMGR_MGMT_DISABLED:
          /* unit is currently not the manager */
            osapiSnprintf(buf, sizeof(buf), "%-13s", pStrInfo_common_StackMbr);
          break;

        case L7_USMDB_UNITMGR_MGMT_UNASSIGNED:
            osapiSnprintf(buf, sizeof(buf), "%-13s", pStrInfo_common_UnAssigned);
          break;

        default:
            osapiSnprintf (buf, sizeof(buf), "%-13s", pStrInfo_common_EmptyString);
          break;
        }
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "%-13s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, buf);

      memset (buf, 0, (L7_int32)sizeof(buf));
      memset (stat, 0, (L7_int32)sizeof(stat));
      if (usmDbUnitMgrModelIdentiferPreCfgGet(unit, (L7_uchar8 *)buf) == L7_SUCCESS)
      {
        len = strlen(buf);
        if (len > 13)
        {
          osapiStrncpySafe(&buf[10], pStrInfo_base_DotDotDot, min((sizeof(pStrInfo_base_DotDotDot)+1), (sizeof(buf)-10)));
        }
        osapiSnprintf(stat, sizeof(stat), "%-13s ", buf);
        }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-14s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (buf, 0, (L7_int32)sizeof(buf));
      memset (stat, 0, (L7_int32)sizeof(stat));
      if (usmDbUnitMgrModelIdentifierGet(unit, (L7_uchar8 *)buf) == L7_SUCCESS)
      {
        len = strlen(buf);
        if (len > 13)
        {
          osapiStrncpySafe(&buf[10], pStrInfo_base_DotDotDot, min((sizeof(pStrInfo_base_DotDotDot)+1), (sizeof(buf)-10)));
        }
        osapiSnprintf(stat, sizeof(stat), "%-13s ", buf);
        }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-14s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (buf, 0, (L7_int32)sizeof(buf));
      if (usmDbUnitMgrUnitStatusGet(unit, &val) == L7_SUCCESS)
      {
        switch (val)
        {
        case L7_USMDB_UNITMGR_UNIT_OK:
            osapiSnprintf(buf, sizeof(buf), "%-22s", pStrInfo_common_Ok);
          break;

        case L7_USMDB_UNITMGR_UNIT_UNSUPPORTED:
            osapiSnprintf(buf, sizeof(buf), "%-22s", pStrInfo_common_UnSupported);
          break;

        case L7_USMDB_UNITMGR_CODE_MISMATCH:
            osapiSnprintf(buf, sizeof(buf), "%-22s", pStrInfo_common_CodeVerMismatch);
          break;

        case L7_USMDB_UNITMGR_CFG_MISMATCH:
            osapiSnprintf(buf, sizeof(buf), "%-22s", pStrInfo_common_CfgMismatch_1);
          break;

        case L7_USMDB_UNITMGR_UNIT_NOT_PRESENT:
            osapiSnprintf(buf, sizeof(buf), "%-22s", pStrErr_common_NotPresent);
          break;

        case L7_USMDB_UNITMGR_CODE_UPDATE:
            osapiSnprintf(buf, sizeof(buf), "%-22s", pStrInfo_common_CodeUpdate);
          break;

        default:
            osapiSnprintf (buf, sizeof(buf), "%-22s", pStrInfo_common_EmptyString);
          break;
        }
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "%-22s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, buf);

      cliUtilsCodeVersionGet(unit, buf, sizeof(buf));
      ewsTelnetPrintf (ewsContext, "%-8s", buf);

      rc = usmDbUnitMgrStackMemberGetNext(unit, &unit);
      cliSyntaxBottom(ewsContext);
    }

    techSupportCurrData->currentOption = SHOW_PORT_ALL;
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
    ewsContext->unbufferedWrite = L7_FALSE;
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */

  }

  /* show Port all */
  if(currentOption == SHOW_PORT_ALL)
  {
    if(techSupportCurrData->flag == L7_FALSE)
    {
      cliClearCharInput();
      memset (buf, 0, sizeof(buf));
      techSupportCurrData->entryIndex = 0;
      ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ShowPortAllFmt);
      cliSyntaxBottom(ewsContext);
      rc = usmDbValidIntIfNumFirstGet(&interface);
      techSupportCurrData->flag = L7_TRUE;
    }

    interface = techSupportCurrData->entryIndex;
    if(usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 15, 2, L7_NULLPTR, ewsContext,pStrInfo_base_AdminPhyPhyLinkLinkLacpFlow);
      ewsTelnetWriteAddBlanks (1, 0, 1, 2, L7_NULLPTR, ewsContext,pStrInfo_base_IntfTypeModeModeStatusStatusTrapModeMode);
        ewsTelnetWrite(ewsContext,"\r\n------ ------ --------- ---------- ---------- ------ ------- ------ -------");
      }
      else
      {
      ewsTelnetWriteAddBlanks (1, 0, 15, 2, L7_NULLPTR, ewsContext,pStrInfo_base_AdminPhyPhyLinkLinkLacp);
      ewsTelnetWriteAddBlanks (1, 0, 1, 2, L7_NULLPTR, ewsContext,pStrInfo_base_IntfTypeModeModeStatusStatusTrapMode);
        ewsTelnetWrite(ewsContext,"\r\n------ ------ --------- ---------- ---------- ------ ------- ------");

      }

    for(count = 0; count < CLI_MAX_SCROLL_LINES - 6; count++)
    {
      if(usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS)
      {
        rc = usmDbUnitSlotPortGet(interface, &u, &s, &p);
        if(rc == L7_SUCCESS)
        {
          usmDbIfSpecialPortTypeGet(unit, interface, &val);
          switch(val)                                                        /* " ","Port-Channel Mbr","Mon","N/A" */
          {
          case L7_PORT_NORMAL:
              osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_EmptyString);
            break;
          case L7_TRUNK_MEMBER:
              osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_LagMbr);
            break;
          case L7_MIRRORED_PORT:
              osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_MirrorShow);
            break;
          case L7_PROBE_PORT:
              osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_Probe);
            break;
          case L7_L2TUNNEL_PORT:
              osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_WsL2Tunnel);
            break;
          default:
              osapiSnprintf(stat, sizeof(stat), "\r\n%-6s %-6s ", cliDisplayInterfaceHelp(u, s, p), pStrInfo_common_EmptyString);
          }
          ewsTelnetWrite(ewsContext,stat);
        }

        rc = usmDbIfAdminStateGet(u, interface, &val);
        switch(val)                                                        /* val = disable, enable, delete */
        {
          case L7_DISABLE:
            osapiSnprintf(stat,sizeof(stat),"%-9s ",pStrInfo_common_Dsbl_1);
            break;
          case L7_ENABLE:
            osapiSnprintf(stat,sizeof(stat),"%-9s ",pStrInfo_common_Enbl_1);
            break;
          case L7_DIAG_DISABLE:
            osapiSnprintf(stat,sizeof(stat), "%-9s ", pStrInfo_base_DiagDsbl);
            break;
          default:
            osapiSnprintf(stat,sizeof(stat),"%-9s ",pStrInfo_common_Enbl_1);
        }
        ewsTelnetWrite(ewsContext,stat);

        rc = usmDbIfAutoNegAdminStatusGet(u, interface, &val);
        if(val == L7_ENABLE)
        {
          /* we are in auto-negotiate */
          osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_Auto);
        }
        else
        {
          /* print out the correct speed */
          rc = usmDbIfSpeedGet(u, interface, &val);
          switch(val)
          {                                          /* it is missing entry number 6 for value 100FX Half.*/
          /* "---","Auto","100 Half, 100 Full, 10 Half,"10 Full","100FX Half","100FX Full","1000 Full" */
          case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_Auto);
            break;
          case L7_PORTCTRL_PORTSPEED_HALF_100TX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100Half);
            break;
          case L7_PORTCTRL_PORTSPEED_FULL_100TX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100Full);
            break;
          case L7_PORTCTRL_PORTSPEED_HALF_10T:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10Half);
            break;
          case L7_PORTCTRL_PORTSPEED_FULL_10T:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10Full);
            break;
          case L7_PORTCTRL_PORTSPEED_FULL_100FX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100FxFull);
            break;
          case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed1000Full);
            break;
          /* PTin added: Speed 2.5G */
          case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed2500Full);
            break;
          /* PTin end */
          case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10GigFull);
            break;
          /* PTin added: Speed 40G */
          case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed40GigFull);
            break;
          /* PTin added: Speed 100G */
          case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100GigFull);
            break;
          /* PTin end */
          default:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_EmptyString);
            break;
          }
        }
        ewsTelnetWrite(ewsContext,stat);
        memset (stat, 0,sizeof(stat));
        rc = usmDbIfHighSpeedGet(u, interface, &val);
        rc = usmDbIfOperStatusGet(u, interface, &val2);
        rc = usmDbIntfStatusGet(interface, &val3);
        if((val3!=L7_INTF_ATTACHED) || (val2==L7_DOWN))
        {
          osapiSnprintf (stat,sizeof(stat),"%-10s ", pStrInfo_common_EmptyString);  /*for a port that is down blanks should be shown here.*/
        }
        else
        {
          switch(val)
          {   /* it is missing entry number 6 for value 100FX Half.*/
            /* "---","Auto","100 Half, 100 Full, 10 Half,"10 Full","100FX Half","100FX Full","1000 Full" */
            case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_Auto);
              break;
            case L7_PORTCTRL_PORTSPEED_HALF_100TX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100Half);
              break;
            case L7_PORTCTRL_PORTSPEED_FULL_100TX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100Full);
              break;
            case L7_PORTCTRL_PORTSPEED_HALF_10T:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10Half);
              break;
            case L7_PORTCTRL_PORTSPEED_FULL_10T:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10Full);
              break;
            case L7_PORTCTRL_PORTSPEED_FULL_100FX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100FxFull);
              break;
            case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed1000Full);
              break;
            /* PTin added: Speed 2.5G */
            case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed2500Full);
              break;
            /* PTin end */
            case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed10GigFull);
              break;
            /* PTin added: Speed 40G */
            case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed40GigFull);
              break;
            /* PTin added: Speed 100G */
            case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_base_Speed100GigFull);
              break;
            /* PTin end */
            default:
              osapiSnprintf(stat,sizeof(stat),"%-10s ", pStrInfo_common_EmptyString);
              break;
          }
        }
        ewsTelnetWrite(ewsContext,stat);
#if 0
        memset (stat, 0,sizeof(stat));
        rc = usmDbIfOperStatusGet(u, interface, &val);
        switch(val)                                                        /* val = up or down */
        {
          case L7_DOWN:
            osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Down_1);
            break;
          case L7_UP:
            osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Up);
            break;
          default:
            osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Down_1);
        }
        ewsTelnetWrite(ewsContext,stat);
#endif
        memset (stat, 0,sizeof(stat));
        rc = usmDbIntfStatusGet(interface, &val);
        if(rc != L7_SUCCESS)
        {
          osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrErr_common_Err);
        }
        else
        {
          state = (L7_INTF_STATES_t)val;
          if(state == L7_INTF_ATTACHED)
          {
            rc = usmDbIfOperStatusGet(u, interface, &val);
            switch(val)                                                        /* val = up or down */
            {
            case L7_DOWN:
                osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Down_1);
              break;
            case L7_UP:
                osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Up);
              break;
            default:
                osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Down_1);
            }
          }
          else
          {
            osapiSnprintf(stat,sizeof(stat),"%-6s ",pStrInfo_common_Detach);
          }
        }
        ewsTelnetWrite(ewsContext,stat);
        memset (stat, 0,sizeof(stat));
        rc = usmDbIfLinkUpDownTrapEnableGet(u, interface, &val);
        ewsTelnetPrintf (ewsContext, "%-7s ",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));

        /* LACP Mode */
        memset (stat, 0, sizeof(stat));
        rc = usmDbDot3adAggPortLacpModeGet(u, interface, &val);
        ewsTelnetPrintf (ewsContext, "%-7s ",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));

        if(usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
        {
          /* Flow Mode */
          memset (stat, 0, sizeof(stat));
          rc = usmDbIfFlowCtrlModeGet(u, interface, &val);
          ewsTelnetPrintf (ewsContext, "%-7s",strUtilEnableDisableGet(val,pStrInfo_common_Enbl_1));
        }
      }

      do
      {
        if(usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
        {
          interface = nextInterface;
        }
        else
        {
          interface = 0;
          techSupportCurrData->flag = L7_TRUE;
#ifdef L7_ISDP_PACKAGE
          techSupportCurrData->currentOption = SHOW_ISDP_NEIGHBORS;
#else
          techSupportCurrData->currentOption = SHOW_LOGGING;
#endif
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
          ewsContext->unbufferedWrite = L7_FALSE;
          return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
        }
      } while((interface != 0)
             && (usmDbVisibleInterfaceCheck(unit, interface, &retVal) != L7_SUCCESS));
    }

    if(interface != 0)
    {
      techSupportCurrData->entryIndex = interface;
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliSyntaxBottom(ewsContext);
      cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
      ewsContext->unbufferedWrite = L7_FALSE;
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }

  }

#ifdef L7_ISDP_PACKAGE
  /* show isdp neighbors */
  if(currentOption == SHOW_ISDP_NEIGHBORS)
  {
    L7_char8 deviceId[L7_ISDP_DEVICE_ID_LEN];

    memcpy(deviceId, techSupportCurrData->lastDeviceId, L7_ISDP_DEVICE_ID_LEN);

    if(techSupportCurrData->flag == L7_TRUE)
    {
      cliClearCharInput();
      memset (buf, 0, sizeof(buf));
      memset(deviceId, 0x00, sizeof(deviceId));
      ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ShowIsdpNeighborsFmt);
      cliSyntaxBottom(ewsContext);
      cliSyntaxBottom(ewsContext);

      ewsTelnetWrite(ewsContext, pStrInfo_base_IsdpCpbCodes);
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  
      ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnNeighborTable,
                      pStrInfo_base_PrnDeviceID, pStrInfo_base_PrnIntf,
                      pStrInfo_base_PrnHoldtime, pStrInfo_base_PrnCap,
                      pStrInfo_base_PrnPlatform, pStrInfo_base_PrnPortId);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  
      ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnNeighborTable,
                      pStrInfo_base_StrRow, pStrInfo_base_StrRow, pStrInfo_base_StrRow,
                      pStrInfo_base_StrRow, pStrInfo_base_StrRow, pStrInfo_base_StrRow);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  
      techSupportCurrData->flag = L7_FALSE;
      rc = usmdbIsdpNeighborGetNext(interface, deviceId, &interface, deviceId);
    }
    count = 0;

    while((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
    {
      cliIsdpShowNeighborWrite(ewsContext, interface, deviceId);
      rc = usmdbIsdpNeighborGetNext(interface, deviceId, &interface, deviceId);
      count++;
    }
    if (rc != L7_SUCCESS)
    {
      memset(techSupportCurrData->lastDeviceId, 0x0, L7_ISDP_DEVICE_ID_LEN);
      interface = 0;
      techSupportCurrData->flag = L7_TRUE;
      techSupportCurrData->currentOption = SHOW_LOGGING;
    }
    else
    {
      memcpy(techSupportCurrData->lastDeviceId, deviceId, L7_ISDP_DEVICE_ID_LEN);
    }
    techSupportCurrData->entryIndex = interface;
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
    ewsContext->unbufferedWrite = L7_FALSE;
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }
#endif

  /****Show logging **/
  if(currentOption == SHOW_LOGGING)
  {
    if(techSupportCurrData->flag == L7_TRUE)
    {
      techSupportCurrData->flag = L7_FALSE;
      cliClearCharInput();
      memset (buf, 0, sizeof(buf));
      ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ShowLoggingFmt);
      cliSyntaxBottom(ewsContext);

      if(usmDbUnitMgrEventLogGet(unit) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_UnitDoesntExist);
        techSupportCurrData->currentOption = SHOW_LOGGING_BUFFERED;
        techSupportCurrData->entryIndex = 0;
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
        ewsContext->unbufferedWrite = L7_FALSE;
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }
      /* Port */
      rc = usmDbLogLocalPortGet (unit, &port);
      if(rc != L7_FAILURE)
      {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowLoggingClientLocalPort);
        osapiSnprintf(buf, sizeof(buf), "%d", port);
        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }
      /* CLI command Logging Admin Mode */
      rc = usmDbCmdLoggerAdminModeGet(&cliAdminMode);
      if(rc != L7_FAILURE)
      {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowCmdLogging);
        /* eventually set to default */
        if(cliAdminMode == L7_ENABLE)
        {
          OSAPI_STRNCAT(buf, pStrInfo_common_Enbld_1);
        }
        else
        {
          OSAPI_STRNCAT(buf, pStrInfo_common_Dsbld2);
        }
        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }
      /*      Console Admin Mode */
      rc = usmDbLogConsoleAdminStatusGet (unit, &status);
      if(rc != L7_FAILURE)
      {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowConsoleLogging);
        /* eventually set to default */
        if(status ==   L7_ADMIN_MODE_ENABLE)
        {
          OSAPI_STRNCAT(buf, pStrInfo_common_Enbld_1);
        }
        else
        {
          OSAPI_STRNCAT(buf, pStrInfo_common_Dsbld2);
        }
        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }
      /* Severity Mode */
      rc = usmDbLogConsoleSeverityFilterGet (unit, &severity);
      if(rc != L7_FAILURE)
      {
        memset (stat, 0,sizeof(stat));
        memset (valString, 0,sizeof(valString));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowConsoleLoggingSeverityFilter);
        rc = cliLogConvertSeverityLevelToString(severity, valString, sizeof(valString));
        if(rc != L7_FAILURE)
        {
          OSAPI_STRNCAT(stat, valString);
          ewsTelnetWrite( ewsContext, stat);
        }
      }

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
      /* Component */
      rc = usmDbLogConsoleComponentFilterGet (unit, &component);
      if(rc != L7_SUCCESS)
      {
        memset (stat, 0, sizeof(stat));
        memset (valString, 0,sizeof(valString));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowConsoleLoggingCompFilter);
        rc = usmDbComponentMnemonicGet(component, valString);
        if(rc != L7_SUCCESS)
        {
          if(strcmp (valString, "") == 0)
          {
            OSAPI_STRNCAT(stat, pStrInfo_common_ApProfileRadioScanFrequencyAll);
          }
          else
          {
            OSAPI_STRNCAT(stat, valString);
          }
          ewsTelnetWrite( ewsContext, stat);
        }
      }
#endif
      /* Buffered Admin Mode */
      rc = usmDbLogInMemoryAdminStatusGet (unit, &status);
      if(rc != L7_FAILURE)
      {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowBufedLogging);
        /* eventually set to default */
        if(status ==   L7_ADMIN_MODE_ENABLE)
        {
          OSAPI_STRNCAT(buf, pStrInfo_common_Enbld_1);
        }
        else
        {
          OSAPI_STRNCAT(buf, pStrInfo_common_Dsbld2);
        }
        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
      /* Component */
      rc = usmDbLogInMemoryComponentFilterGet (unit, &component);
      if(rc != L7_SUCCESS)
      {
        memset (stat, 0, sizeof(stat));
        memset (valString, 0,sizeof(valString));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowBufedLoggingCompFilter);
        rc = usmDbComponentMnemonicGet(component, valString);
        if(rc != L7_SUCCESS)
        {
          if(strcmp (valString, "") == 0)
          {
            OSAPI_STRNCAT(stat, pStrInfo_common_ApProfileRadioScanFrequencyAll);
          }
          else
          {
            OSAPI_STRNCAT(stat, valString);
          }
          ewsTelnetWrite( ewsContext, stat);
        }
      }
#endif

      if(usmDbFeaturePresentCheck(unit, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) == L7_TRUE)
      { /*      Persistent Admin Mode */
        rc = usmDbLogPersistentAdminStatusGet (unit, &status);
        if(rc != L7_FAILURE)
        {
          memset (buf, 0,sizeof(buf));
          memset (stat, 0,sizeof(stat));
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowPersistentLogging);
          /* eventually set to default */
          if(status ==   L7_ADMIN_MODE_ENABLE)
          {
            OSAPI_STRNCAT(buf, pStrInfo_common_Enbld_1);
          }
          else
          {
            OSAPI_STRNCAT(buf, pStrInfo_common_Dsbld2);
          }
          OSAPI_STRNCAT(stat, buf);
          ewsTelnetWrite( ewsContext, stat);
        }
        /* Severity Mode */
        rc = usmDbLogPersistentSeverityFilterGet (unit, &severity);
        if(rc != L7_FAILURE)
        {
          memset (stat, 0,sizeof(stat));
          memset (valString, 0,sizeof(valString));
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowPersistentSeverityFilter);
          rc = cliLogConvertSeverityLevelToString(severity, valString, sizeof(valString));
          if(rc != L7_FAILURE)
          {
            OSAPI_STRNCAT(stat, valString);
            ewsTelnetWrite( ewsContext, stat);
          }
        }
      }
      /* Admin Mode */
      rc = usmDbLogSyslogAdminStatusGet (unit, &status);
      if(rc != L7_FAILURE)
      {
        memset (stat, 0,sizeof(stat));
        memset (buf, 0,sizeof(buf));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowSyslogLogging);
        /* eventually set to default */
        if(status ==   L7_ADMIN_MODE_ENABLE)
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld_1);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld2);
        }
        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
      /* Severity Mode */
      rc = usmDbLogSyslogDefaultFacilityGet (unit, &facility);
      if(rc != L7_FAILURE)
      {
        memset (stat, 0,sizeof(stat));
        memset (buf, 0,sizeof(buf));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowSyslogLoggingFacility);
        rc = cliLogConvertFacilityToString(facility, valString, sizeof(valString));
        if(rc != L7_FAILURE)
        {
          if(strcmp (valString, "") == 0)
          {
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Unknown_1);
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf), valString);
          }
          OSAPI_STRNCAT(stat, buf);
          ewsTelnetWrite( ewsContext, stat);
        }
      }
#endif
      /* Messages Received */
      rc = usmDbLogMessagesReceivedGet (unit, &count);
      if(rc != L7_FAILURE)
      {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowLogMsgsRcvd);
        osapiSnprintf(buf, sizeof(buf), "%d", count);
        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }

      /* Messages Dropped */
      rc = usmDbLogMessagesDroppedGet (unit, &count);
      if(rc != L7_FAILURE)
      {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowLogMsgsDropped);
        osapiSnprintf(buf, sizeof(buf), "%d", count);
        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }
      /* Messages Relayed */
      rc = usmDbLogSyslogMessagesRelayedGet (unit, &count);
      if(rc != L7_FAILURE)
      {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_ShowLogMsgsRelayed);
        osapiSnprintf(buf, sizeof(buf), "%d", count);
        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }

      ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_EventLog);
      ewsTelnetWrite( ewsContext, "\r\n ---------");

      usmDbUnitMgrEventLogGet(unit);
      /* If first time into function, set techSupportCurrData->entryIndex to zero to get first entry. */
      memset (string, 0, L7_CLI_MAX_STRING_LENGTH*2);
      techSupportCurrData->entryIndex = L7_NULL;
      techSupportCurrData->entryIndex = usmDbLogEventLogEntryNextGet(unit, techSupportCurrData->entryIndex, string);
      techSupportCurrData->entryIndex = L7_NULL;
      techSupportCurrData->entryIndex = usmDbLogEventLogEntryNextGet(unit, techSupportCurrData->entryIndex, string);
      if(techSupportCurrData->entryIndex == L7_NULL)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_EventLogEmpty);
        techSupportCurrData->currentOption = SHOW_LOGGING_BUFFERED;
        techSupportCurrData->entryIndex = 0;
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
        ewsContext->unbufferedWrite = L7_FALSE;
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }

      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliSyntaxBottom(ewsContext);
      cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
      ewsContext->unbufferedWrite = L7_FALSE;
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */

    }
    ewsTelnetWriteAddBlanks (0, 1, 75, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_Time);
    ewsTelnetWriteAddBlanks (0, 1, 7, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_FileLineTaskidCodeDHMS);
    cliSyntaxBottom(ewsContext);

    for(count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
    {
      if(techSupportCurrData->entryIndex == L7_NULL)
      {
        techSupportCurrData->flag = L7_TRUE;
        techSupportCurrData->currentOption = SHOW_LOGGING_BUFFERED;
        techSupportCurrData->entryIndex = 0;
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
        ewsContext->unbufferedWrite = L7_FALSE;
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }

      /* without the \r, the lines are not output correctly on the telnet screen. */
      stringLength = strlen(string);
      if(stringLength >= 2)
      {
        if(string[stringLength-2] != '\r')
        {
          string[stringLength-1] = '\r';
          string[stringLength]   = '\n';
          string[stringLength+1]   = L7_NULL;
        }
      }
      ewsTelnetWrite(ewsContext, string);
      memset (string, 0, L7_CLI_MAX_STRING_LENGTH*2);
      techSupportCurrData->entryIndex = usmDbLogEventLogEntryNextGet(unit, techSupportCurrData->entryIndex, string);
    }

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
    ewsContext->unbufferedWrite = L7_FALSE;
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */

  }

  if(currentOption == SHOW_LOGGING_BUFFERED)
  {
    count = 0;
    if(techSupportCurrData->flag == L7_TRUE)
    {
      techSupportCurrData->flag = L7_FALSE;
      techSupportCurrData->entryIndex = 0;
      rc = usmDbLogInMemoryAdminStatusGet (unit, &status);
      if (rc != L7_FAILURE)
      {
        memset (stat, 0,sizeof(stat));
        memset (buf, 0,sizeof(buf));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_BufedInMemoryLogging);
        /* eventually set to default */
        if (status ==   L7_ADMIN_MODE_ENABLE)
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld_1);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld2);
        }

        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }
#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
      /* Component */
      rc = usmDbLogInMemoryComponentFilterGet (unit, &component);
      if (rc != L7_FAILURE)
      {
        memset (stat, 0, sizeof(stat));
        memset (buf, 0,sizeof(buf));
        memset (valString, 0,sizeof(valString));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_BufedLoggingCompFilter);
        rc = usmDbComponentMnemonicGet(component, valString);
        if (rc != L7_SUCCESS)
        {
          if (strcmp (valString, pStrInfo_common_EmptyString) == 0)
          {
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_ApProfileRadioScanFrequencyAll);
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf), valString);
          }

          OSAPI_STRNCAT(stat, buf);
          ewsTelnetWrite( ewsContext, stat);
        }
      }
#endif
      /* Wrapping */
      rc = usmDbLogInMemoryBehaviorGet (unit, &behavior);
      if (rc != L7_FAILURE)
      {
        memset (stat, 0, sizeof(stat));
        memset (buf, 0,sizeof(buf));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_BufedLoggingWrappingBehavior);
        if (behavior ==  L7_LOG_WRAP_BEHAVIOR)
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_ApProfileRadioStatusOn);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_base_OffLogWillStopWhenFull);
        }

        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
      }

      /* Count */
      rc = usmDbLogInMemoryLogCountGet (unit, &logCount);
      if (rc != L7_FAILURE)
      {
        memset (buf, 0,sizeof(buf));
        memset (stat, 0,sizeof(stat));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_base_BufedLogCount);
        osapiSnprintf(buf, sizeof(buf), "%d", logCount);

        OSAPI_STRNCAT(stat, buf);
        ewsTelnetWrite( ewsContext, stat);
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      }

      /* If first time into function, set pointer to null to get first entry. */
      memset(string, 0, L7_CLI_MAX_STRING_LENGTH*2);
      /* ptr = L7_NULLPTR; */
      logIndex = 0;
      rc = usmDbLogMsgInMemoryGetNext(unit, logIndex, string, &techSupportCurrData->entryIndex);
      if (rc == L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
        if (string[0] != '\0')
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        }
        else
        {
          ewsContext->unbufferedWrite = L7_FALSE;
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_BufedLogEmpty);
        }
      }
      count = 7;/*to take care of above output to console*/
    }

    for (; count < CLI_MAX_SCROLL_LINES-6; count++)
    {
      logIndex = techSupportCurrData->entryIndex;
      if (rc != L7_SUCCESS)
      {
        /*output string one last time to get oldest entry in message log*/
        stringLength = strlen(string);
        if (stringLength >= 2)
        {
          if (string[stringLength-2] != '\r' && string[stringLength-1] == '\n')
          {
            string[stringLength-1] = '\r';
            string[stringLength]   = '\n';
            string[stringLength+1] = '\0';
          }
        }
        techSupportCurrData->currentOption = SHOW_TRAPLOG;
        techSupportCurrData->entryIndex = 0;
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
        ewsContext->unbufferedWrite = L7_FALSE;
        return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
      }

      /* without the \r, the lines are not output correctly on the telnet screen. */
      stringLength = strlen(string);
      if(stringLength >= 2)
      {
        if (string[stringLength-2] == '\n' && string[stringLength-1] == '\n')
        {
          string[stringLength-2] = '\r';
          string[stringLength] = '\0';
        }                          /* don't cut off the last char if there is no \n */
        else if (string[stringLength-2] != '\r' && string[stringLength-1] == '\n')
        {
          string[stringLength-1] = '\r';
          string[stringLength]   = '\n';
          string[stringLength+1]   = '\0';
        }                          /* don't cut off the last char if there is no \n */
        else if (string[stringLength-2] != '\r' && string[stringLength-1] != '\n')
        {
          string[stringLength] = '\r';
          string[stringLength+1]   = '\n';
          string[stringLength+2]   = '\0';
        }
      }

      if (stringLength >= MAX_STRING_LEN)
      {
        count++;
      }
      ewsTelnetWrite(ewsContext, string);
      memset(string, 0, L7_CLI_MAX_STRING_LENGTH*2);
      rc = usmDbLogMsgInMemoryGetNext(unit, logIndex, string, &techSupportCurrData->entryIndex);

    }
    if(rc != L7_SUCCESS)
    {
      techSupportCurrData->currentOption = SHOW_TRAPLOG;
      techSupportCurrData->entryIndex = 0;
    }
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
    ewsContext->unbufferedWrite = L7_FALSE;
    return pStrInfo_common_Name_2;  /* --More-- or (q)uit */


  }

  if(currentOption == SHOW_TRAPLOG)
  {
    if(techSupportCurrData->flag == L7_FALSE)
    {
      trapLogCount = 0;
      techSupportCurrData->entryIndex = usmDbTrapLogEntryLastReceivedGet();
      techSupportCurrData->startingEntryIndex = techSupportCurrData->entryIndex;

      if(usmDbInfoTrapsSinceLastResetSwGet(unit, &techSupportCurrData->numEntries) != L7_SUCCESS)
      {
        techSupportCurrData->numEntries = 0;
      }

      /* Determine # of traps to be displayed. */
      cliFormat(ewsContext, pStrInfo_base_NumOfTrapsSinceLastReset);  /*Number of Traps Since Last Reset*/
      ewsTelnetPrintf (ewsContext, "%u", techSupportCurrData->numEntries);
      numWrites++;

      /* Display the Trap Log Capacity. */
      cliFormat(ewsContext, pStrInfo_base_NumTrapLogCapacity);  /*Trap Log Capacity*/
      ewsTelnetPrintf (ewsContext, "%u", TRAPMGR_TABLE_SIZE);
      numWrites++;

      cliFormat(ewsContext, pStrInfo_base_NumOfTrapsSinceLogLastViewed); /*Number of Traps Since Log Last Displayed*/
      rc = usmDbTrapNumUnDisplayedTrapEntriesGet(unit, &val);
      ewsTelnetPrintf (ewsContext, "%u", val);
      numWrites++;

      if(techSupportCurrData->numEntries > TRAPMGR_TABLE_SIZE)
      {
        techSupportCurrData->numEntries = TRAPMGR_TABLE_SIZE;
      }
      techSupportCurrData->flag = L7_TRUE;
    }

    /* printout header information */
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LogSysUpTimeTrap);
    ewsTelnetWrite(ewsContext,"\r\n--- ------------------------ -------------------------------------------------");
    numWrites +=3;

    if (techSupportCurrData->numEntries == 0)
    {
      techSupportCurrData->flag = L7_TRUE;
      techSupportCurrData->currentOption = SHOW_RUNNING_CONFIG;
      techSupportCurrData->entryIndex = 0;
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliSyntaxBottom(ewsContext);
      cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
      ewsContext->unbufferedWrite = L7_FALSE;
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }

    while (numWrites < CLI_MAX_SCROLL_LINES-6)
    {
      rc = usmDbTrapLogEntryGet(unit, USMDB_USER_DISPLAY, techSupportCurrData->entryIndex, &trapLog);
      if (rc != L7_SUCCESS)
      {
        techSupportCurrData->flag = L7_TRUE;
        techSupportCurrData->currentOption = SHOW_RUNNING_CONFIG;
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
        ewsContext->unbufferedWrite = L7_FALSE;
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }

      if (strcmp(trapLog.timestamp, "") != 0)
      {
        ewsTelnetPrintf (ewsContext, "\r\n%3d ",trapLogCount);

        ewsTelnetPrintf (ewsContext, "%-24s ",trapLog.timestamp);

        memset (stat, 0,sizeof(stat));
        osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(trapLog.message, 48));
        do
        {
          if (first == L7_TRUE)
          {
            first = L7_FALSE;
            osapiSnprintf(stat, sizeof(stat), "%-48s", buf);
          }
          else
          {
            ewsTelnetPrintf (ewsContext, "\r\n%-29s", " ");
            osapiSnprintf(stat, sizeof(stat), "%-48s", buf);
          }

          ewsTelnetWrite(ewsContext,stat);
          numWrites++;

          memset (buf, 0,sizeof(buf));
          memset (stat, 0,sizeof(stat));
          osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(trapLog.message, 48));
        }
        while (strcmp(buf, trapLog.message) != 0);

        first = L7_TRUE;

      }
      trapLogCount++;

      if (techSupportCurrData->entryIndex > 0)
      {
        techSupportCurrData->entryIndex--;
      }
      else
      {
        if (trapLogCount < techSupportCurrData->numEntries)
        {
          techSupportCurrData->entryIndex = TRAPMGR_TABLE_SIZE-1;
        }
      }

      if (((trapLogCount == techSupportCurrData->startingEntryIndex + 1) && (techSupportCurrData->numEntries < TRAPMGR_TABLE_SIZE))
          || (trapLogCount == TRAPMGR_TABLE_SIZE))
      {
        techSupportCurrData->currentOption = SHOW_RUNNING_CONFIG;
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
        ewsContext->unbufferedWrite = L7_FALSE;
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }
   }

   cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
   cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
    ewsContext->unbufferedWrite = L7_FALSE;
    return pStrInfo_common_Name_2;  /* --More-- or (q)uit */

  }
  if(currentOption == SHOW_RUNNING_CONFIG)
  {
    if (techSupportCurrData->flag == L7_TRUE)
    {
      cliClearCharInput();
      memset (buf, 0, sizeof(buf));
      ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ShowRunCfgFmt);
      cliSyntaxBottom(ewsContext);
      techSupportCurrData->flag = L7_FALSE;

      cliShowRunningConfig(ewsContext, argv, numArg, L7_TRUE);
      ewsContext->unbufferedWrite = L7_TRUE;
      cliSyntaxTop(ewsContext);
      numArg = cliNumFunctionArgsGet();

      ewsContext->unbufferedWrite = L7_FALSE;
   }
#ifdef LVL7_DEBUG_BREAKIN
   techSupportCurrData->currentOption = SHOW_CONSOLE_BREAKIN;
#else
   techSupportCurrData->currentOption = SHOW_DEBUGGING;
#endif

   cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
   cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
   ewsContext->unbufferedWrite = L7_FALSE;
   techSupportCurrData->flag = L7_TRUE;
   return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }
#ifdef LVL7_DEBUG_BREAKIN
  if(currentOption == SHOW_CONSOLE_BREAKIN)
  {
    techSupportCurrData->currentOption = SHOW_DEBUGGING;

    current_handle = cliCurrentHandleGet ();
    cliFormat(ewsContext, pStrInfo_common_ConsoleBreakInStatus);
    rc =  usmDbConsoleBreakinModeGet(current_handle);
    if (rc == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
    }
    else
    {
    ewsTelnetWrite(ewsContext, pStrInfo_common_Dsbl_1);
    }

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
    ewsContext->unbufferedWrite = L7_FALSE;
    return pStrInfo_common_Name_2;  /*----More--- or (q)uit*/
  }
#endif
  if (currentOption == SHOW_DEBUGGING)
  {
    cliClearCharInput();
    ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ShowDebugging);
    cliSyntaxBottom(ewsContext);

    currentOption = SHOW_VERSION;
    if (usmDbDot3adPacketDebugTraceFlagGet() == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugLacpPktEnbld);
    }

    usmDbDot1sPacketDebugTraceFlagGet(&txFlag,&rxFlag);
    if(txFlag)
    {
      if(rxFlag)
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugSpanTreePktEnbld);
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugSpanTreePktTxEnbld);
     }
    }
    else if(rxFlag)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugSpanTreePktRxEnbld);
    }
    if (usmDbPingPacketDebugTraceFlagGet() == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugPingPktEnbld);
    }
    usmDbDot1xPacketDebugTraceFlagGet(&txFlag,&rxFlag);

    if(txFlag)
    {
     if(rxFlag)
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot1xPktEnbld);
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot1xPktTxEnbld);
     }
    }
    else if(rxFlag)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot1xPktRxEnbld);
    }

#ifdef L7_DOT3AH_PACKAGE
    usmDbDot3ahPacketDebugTraceFlagGet(&txFlag,&rxFlag);

    if(txFlag)
    {
     if(rxFlag)
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot3ahPktEnbld);
     }
     else
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot3ahPktTxEnbld);
     }
    }
    else if(rxFlag)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot3ahPktRxEnbld);
    }
#endif

   #ifdef L7_BGP_PACKAGE
   if (usmDbBgpPacketDebugTraceFlagGet() == L7_TRUE)
   {
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DebugBgpPktEnbld);
   }
   #endif /*L7_PACKAGE_BGP*/

   #ifdef L7_ROUTING_PACKAGE
   #ifdef L7_OSPF_PACKAGE
   if (usmDbOspfPacketDebugTraceFlagGet() == L7_TRUE)
   {
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DebugOspfPktEnbld);
   }
   #endif

#ifdef L7_IPV6_PACKAGE
   if (usmDbOspfv3PacketDebugTraceFlagGet() == L7_TRUE)
   {
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DebugOspfV3PktEnbld);
   }
#endif

   #ifdef L7_RIP_PACKAGE
   if (usmDbRipMapPacketDebugTraceFlagGet() == L7_TRUE)
   {
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DebugRipPktEnbld);
   }
   #endif

   #endif /*L7_PACKAGE_ROUTING*/

   usmDbSnoopPacketDebugTraceFlagGet(&txFlag,&rxFlag,family);

   if(txFlag)
   {
     if(rxFlag)
     {
       ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_ENABLED(family));
     }
     else
     {
       ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_TX_ENABLED(family));
     }
   }
   else if(rxFlag)
   {
     ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_RX_ENABLED(family));
   }

   usmDbSnoopPacketDebugTraceFlagGet(&txFlag,&rxFlag,L7_AF_INET6);

   if(txFlag)
   {
     if(rxFlag)
     {
       ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_ENABLED(L7_AF_INET6));
     }
     else
     {
       ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_TX_ENABLED(L7_AF_INET6));
     }
   }
   else if(rxFlag)
   {
     ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_RX_ENABLED(L7_AF_INET6));
   }
#ifdef L7_SFLOW_PACKAGE
  /* sFlow Transmit debugging info */
  if (usmDbComponentPresentCheck(unit, L7_SFLOW_COMPONENT_ID))
  {
    usmDbsFlowPacketDebugTraceFlagGet(&txFlag);

    if (txFlag)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugsFlowPacketEnabled);
    } 
  }
#endif
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_VRRP_PACKAGE
   if (usmDbIpVrrpPacketDebugTraceFlagGet() == L7_TRUE)
   {
     ewsTelnetWriteAddBlanks(1,0,0,0, L7_NULLPTR, ewsContext, pStrErr_base_DebugVrrpPktEnbld);
   }
#endif

   if (usmDbArpPacketDebugTraceFlagGet() == L7_TRUE)
   {
     ewsTelnetWriteAddBlanks(1,0,0,0, L7_NULLPTR ,ewsContext, PStrErr_base_DebugArpPktEnbld);
   }

#ifdef L7_QOS_FLEX_PACKAGE_ACL
   for(i = L7_MIN_ACL_ID; i<=L7_MAX_ACL_ID; i++)
   {
     if (usmDbIpPacketDebugTraceFlagGet(i,&rxFlag)== L7_SUCCESS)
     {
       if(rxFlag == L7_TRUE)
       {
         memset(str,0x00,sizeof(str));
         sprintfAddBlanks(1,0,0,0,L7_NULLPTR,str,pStrErr_base_DebugAclPktEnbld,i);
         ewsTelnetWrite(ewsContext, str);
       }
     }
   }
#endif /*L7_QOS_FLEX_PACKAGE_ACL*/
#endif /*L7_ROUTING_PACKAGE*/

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  if (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_VOIP_COMPONENT_ID) == L7_TRUE)
  {
    if (usmDbQosVoIPDebugTraceAllFlagGet(&SIPTraceFlag, &H323TraceFlag, 
                                         &SCCPTraceFlag, &MGCPTraceFlag) == L7_SUCCESS)
    {
      if (SIPTraceFlag == L7_ENABLE)
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoSIP, pStrInfo_base_TracingEnbld);
      }
      else
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoSIP, pStrInfo_base_TracingDsbld);
      }

      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);

      if (H323TraceFlag == L7_ENABLE)
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoH323, pStrInfo_base_TracingEnbld);
      }
      else
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoH323, pStrInfo_base_TracingDsbld);
      }

      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
      
      if (SCCPTraceFlag == L7_ENABLE)
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoSCCP, pStrInfo_base_TracingEnbld);
      }
      else
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoSCCP, pStrInfo_base_TracingDsbld);
      }

      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
    }
  }
#endif
  
   cliSyntaxBottom(ewsContext);
   ewsContext->unbufferedWrite = L7_FALSE;
   return cliPrompt(ewsContext);
  }
  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_base_ShowTechSupportCmd);
  ewsContext->unbufferedWrite = L7_FALSE;
  return pStrInfo_common_Name_2; /*----More--- or (q)uit*/
}

/*********************************************************************
 *
 * @purpose  Display the terminal length for show running-config
 *
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
 * @cmdsyntax  show terminal length
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShowTerminalLength (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 length;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowTerminalLen_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext,pStrInfo_base_TerminalLen);
  if (usmDbTerminalLineGet(unit, &length) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_GetTerminalLen);
  }
  return cliSyntaxReturnPrompt (ewsContext,"%d", length);
}

/*********************************************************************
*
* @purpose  display the default domain name, list of name server hosts,
*           the static and the cached list of host names and addresses.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show hosts <hostname>
*
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowHosts(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 **argv,
                                 L7_uint32 index)
{
   L7_uint32 unit = 0;
   static L7_char8 hostname[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
   L7_char8 qname[DNS_DOMAIN_NAME_SIZE_MAX];
   L7_char8 userHostname[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH*10];
   L7_char8 domain[DNS_DOMAIN_NAME_SIZE_MAX + 1];
   L7_char8 strAddr[DNS_DOMAIN_NAME_SIZE_MAX];
   L7_uint32 hostnameArg = 1;
   L7_uint32 numArg;
   L7_inet_addr_t serverAddr;
   static L7_uint32 ipAddress = 0;
   static L7_inet_addr_t inetAddr;
   L7_uint32 hostNum, serverNum;
   L7_RC_t rc = L7_FAILURE;
   L7_RC_t retCode = L7_FAILURE;
   L7_BOOL enable;
   static L7_uchar8    rrName[DNS_DOMAIN_NAME_SIZE_MAX];
   static dnsRRType_t  rrType;
   static L7_uint32    rrIndex = 0;
   static dnsRRClass_t rrClass;
   L7_uint32    ttl = 0;
   L7_uint32    elapsed = 0;
   L7_uint32    retries = L7_NULL, ipAddr = L7_NULL;
   L7_ushort16  timeout = L7_NULL;
   L7_uchar8 strrrType[20];
   L7_uchar8 prettyName[DNS_DOMAIN_NAME_SIZE_MAX];
   L7_char8 sysName[L7_SYS_SIZE];
   L7_BOOL   primary = L7_FALSE;
   L7_uchar8 data[DNS_DOMAIN_NAME_SIZE_MAX];
   L7_BOOL   tableEmpty = L7_TRUE;
   L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
   L7_BOOL firstRow = L7_TRUE;
   L7_uint32 lineCount = 7, cmdIndex = 0;
   L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
   static L7_BOOL firstDisplay = L7_TRUE;
   static L7_BOOL firstDcDisplay = L7_TRUE;
   static L7_BOOL firstTableDisplay = L7_TRUE;
   L7_BOOL entriesComplete = L7_TRUE;

   /*************Set Flag for Script Failed******/
   ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
   cliSyntaxTop(ewsContext);
   cliCmdScrollSet( L7_FALSE);
   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
             pStrErr_common_ErrCouldNot, ewsContext, 
             pStrErr_common_UnitId_1);
   }
   numArg = cliNumFunctionArgsGet();

   if (cliGetCharInputID() != CLI_INPUT_EMPTY)
   { 
     /* if our question has been answered */
     if(L7_TRUE == cliIsPromptRespQuit())
     {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        inetAddressReset(&inetAddr);
        return cliPrompt(ewsContext);
     }
     lineCount = 0; /* resetting the Line count in More case*/
   }
   else
   {
     firstDisplay = L7_TRUE;
     firstDcDisplay = L7_TRUE;
     firstTableDisplay = L7_TRUE;
     inetAddressReset(&inetAddr);
   }

   if ((numArg == 1) && (firstDisplay == L7_TRUE) && (firstDcDisplay == L7_TRUE))
   {
      bzero(hostname, sizeof(hostname));
      bzero(userHostname, sizeof(userHostname));
      osapiStrncpySafe(userHostname, argv[index + hostnameArg], 
                       sizeof(userHostname));
      rc = usmDbIPHostAddressValidate(userHostname, &ipAddr, &addrType);
      if ((rc != L7_SUCCESS) || (addrType == L7_IP_ADDRESS_TYPE_IPV4))
      {
         /* Invalid HostName */
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                ewsContext, pStrErr_base_InvalidHostName_space);
      }
   }

   if ((numArg != 1) && (firstDisplay == L7_TRUE) && (firstDcDisplay == L7_TRUE)) 
   {
     bzero(buf, sizeof(buf));
     cliFormat(ewsContext, pStrInfo_base_DnsHostName);  /* Host Name */
     ewsTelnetWrite(ewsContext, buf);
     bzero(sysName, sizeof(sysName));
     bzero(buf, sizeof(buf));
     if ( usmDb1213SysNameGet(unit, sysName) == L7_SUCCESS )
     {
        sprintf(buf, "%s", sysName);
        ewsTelnetWrite(ewsContext, buf);
     }

     bzero(buf, sizeof(buf));
     bzero(domain, sizeof(domain));
     cliFormat(ewsContext, pStrInfo_base_Domain);  /* Default Domain */
     rc = usmDbDNSClientDefaultDomainGet(domain);
     if(rc == L7_SUCCESS && (strcmp (domain, "\0") == 0))
     {
        osapiStrncpySafe(buf, pStrInfo_base_DomainNameNotConfig, sizeof(buf));
        ewsTelnetWrite(ewsContext, buf);
     }
     else if (rc == L7_SUCCESS)
     {
        sprintf(buf, "%s", domain);
        ewsTelnetWrite(ewsContext, buf);
     }
     else
     {
        osapiStrncpySafe(buf, pStrInfo_base_DomainNameNotConfig, sizeof(buf));
        ewsTelnetWrite(ewsContext, buf);
     }

     bzero(buf, sizeof(buf));
     bzero(domain, sizeof(domain));
     cliFormat(ewsContext, pStrInfo_base_DomainList); /* Default Domain List */
     rc = usmDbDNSClientDomainNameListGetNext(domain);
     if (rc == L7_SUCCESS && (strcmp (domain, "\0") == 0))
     {
        osapiStrncpySafe(buf, pStrInfo_base_DomainListNotConfig, sizeof(buf));
        ewsTelnetWrite(ewsContext, buf);
     }
     else if (rc == L7_SUCCESS)
     {
        sprintf(buf, "%s", domain);
        ewsTelnetWrite(ewsContext, buf);
        while(usmDbDNSClientDomainNameListGetNext(domain) == L7_SUCCESS)
        {
           sprintf(buf, ", %s", domain);
           ewsTelnetWrite(ewsContext, buf);
        }
     }
     else
     {
        osapiStrncpySafe(buf, pStrInfo_base_DomainListNotConfig, sizeof(buf));
        ewsTelnetWrite(ewsContext, buf);
     }

     bzero(buf, sizeof(buf));
     bzero(domain, sizeof(domain));
     cliFormat(ewsContext, pStrInfo_base_DomainLookup); /* Domain Name Lookup */
     rc = usmDbDNSClientAdminModeGet(&enable);
     if (rc == L7_SUCCESS)
     {
        if ( enable == L7_TRUE )
        {
           osapiStrncpySafe(buf, pStrInfo_base_Enabled, sizeof(buf));
           ewsTelnetWrite(ewsContext, buf);
        }
        else
        {
           osapiStrncpySafe(buf, pStrInfo_base_Disabled, sizeof(buf));
           ewsTelnetWrite(ewsContext, buf);
        }
     }
     bzero(buf, sizeof(buf));
     cliFormat(ewsContext, pStrInfo_base_NoOfRetries); /* Retries */
     rc = usmDbDNSClientRetransmitsGet(&retries);
     if (rc == L7_SUCCESS)
     {
        sprintf(buf, "%d", retries);
        ewsTelnetWrite(ewsContext, buf);
     }
     bzero(buf, sizeof(buf));
     cliFormat(ewsContext, pStrInfo_base_RetryTimeout); /* Timeout */
     rc = usmDbDNSClientQueryTimeoutGet(&timeout);
     if (rc == L7_SUCCESS)
     {
        sprintf(buf, "%d", timeout);
        ewsTelnetWrite(ewsContext, buf);
     }
     bzero(buf, sizeof(buf));
     cliFormat(ewsContext, pStrInfo_base_NameServer); /* Name Servers */
     inetAddressReset(&serverAddr);
     rc = usmDbDNSClientNameServerUnorderedListEntryGetNext(&serverAddr);
     if (rc == L7_SUCCESS)
     {
        bzero(strAddr, sizeof(strAddr));
        if(inetAddrHtop(&serverAddr, strAddr) == L7_SUCCESS)
        {
           sprintf(buf, "%s", strAddr);
           ewsTelnetWrite(ewsContext, buf);
        }
     }
     else if(rc == L7_FAILURE)
     {
        osapiStrncpySafe(buf, pStrInfo_base_NameServersNotConfig, sizeof(buf));
        ewsTelnetWrite(ewsContext, buf);
     }
     /**** FOR GETTING SECOND AND NEXT SERVER ENTRIES **********/
     if ( rc == L7_SUCCESS)
     {
        for (serverNum = 1; serverNum < L7_DNS_NAME_SERVER_ENTRIES; serverNum++)
        {
           rc = usmDbDNSClientNameServerUnorderedListEntryGetNext(&serverAddr);
           if (rc == L7_SUCCESS)
           {
             bzero(strAddr, sizeof(strAddr));
              if(inetAddrHtop(&serverAddr, strAddr) == L7_SUCCESS)
              {
                 sprintf(buf, ", %s", strAddr);
                 ewsTelnetWrite(ewsContext, buf);
              }
           }
           else
           {
              break;
           }
        }
        sprintf(buf, "\n");
        ewsTelnetWrite(ewsContext, buf);
     }
   }
   if ((firstDisplay == L7_TRUE) && (firstDcDisplay == L7_TRUE)) 
   {
     osapiStrncpySafe(buf, pStrInfo_base_ConfigHostNameIPMapping, sizeof(buf));
     ewsTelnetWrite(ewsContext, buf);
     osapiStrncpySafe(buf, pStrInfo_base_HostAddress, sizeof(buf));
     ewsTelnetWrite(ewsContext, buf);
     osapiStrncpySafe(buf, pStrInfo_base_ShowHostsLine, sizeof(buf));
     ewsTelnetWrite(ewsContext, buf);
   }

   if (numArg == 1)
   {
      bzero(hostname, sizeof(hostname));
      bzero(userHostname, sizeof(userHostname));
      osapiStrncpySafe(userHostname, argv[index + hostnameArg], 
                       sizeof(userHostname));
      rc = usmDbIPHostAddressValidate(userHostname, &ipAddr, &addrType);
      if (rc == L7_SUCCESS && addrType == L7_IP_ADDRESS_TYPE_DNS)
      {
         osapiStrncpySafe(hostname, "", sizeof(hostname));
         for (hostNum=0; hostNum<L7_DNS_STATIC_HOST_ENTRIES; hostNum++)
         {
            retCode = usmDbDNSClientStaticHostEntryNextGet(hostname, &inetAddr);
            if (retCode == L7_SUCCESS)
            {
               if (strcmp(hostname, userHostname) == 0)
               {
                  tableEmpty = L7_FALSE;
                  if(inetAddrHtop(&inetAddr, strAddr) == L7_SUCCESS)
                  {
                     cliDisplayDnsStaticEntries(ewsContext, 
                                      hostname, strAddr, &lineCount);
                     break;
                  }
               }
            }
         }
      }
      if ((addrType != L7_IP_ADDRESS_TYPE_IPV4) && 
          (tableEmpty == L7_TRUE) && (rc == L7_SUCCESS))
      {
         osapiStrncpySafe(buf, pStrInfo_base_HostNameNotConfig, sizeof(buf));
         ewsTelnetWrite(ewsContext, buf);
      }
   }
   else if (numArg == 0)
   {
      if (firstTableDisplay == L7_TRUE)
      {
        if (firstDisplay == L7_TRUE)
        {
          bzero(hostname, sizeof(hostname));
          osapiStrncpySafe(hostname, "", sizeof(hostname));
          ipAddress = 0;
        }
        for (hostNum=0; hostNum<L7_DNS_STATIC_HOST_ENTRIES; hostNum++)
        {
           rc = usmDbDNSClientStaticHostEntryNextGet(hostname, &inetAddr);
           if (rc == L7_SUCCESS)
           {
              if(inetAddrHtop(&inetAddr, strAddr) == L7_SUCCESS)
              {
                 cliDisplayDnsStaticEntries(ewsContext, 
                                            hostname, strAddr, &lineCount);
              }
           }
           else if ((rc == L7_FAILURE) && hostNum == 0)
           {
              if (firstDisplay == L7_TRUE)
              {
                osapiStrncpySafe(buf, pStrInfo_base_HostNameNotConfig, sizeof(buf));
                ewsTelnetWrite(ewsContext, buf);
              }
              firstTableDisplay = L7_FALSE;
              break;
           }
           else if(rc == L7_FAILURE)
           {
              firstTableDisplay = L7_FALSE;
              break;
           }
           if (((CLI_MAX_SCROLL_LINES-6) - lineCount) < CLI_MAX_LINES_IN_PAGE)
           {
             firstDisplay = L7_FALSE;
             break;
           }

        }
        if ((rc == L7_SUCCESS) && ((CLI_MAX_SCROLL_LINES-6) - lineCount) < 
             CLI_MAX_LINES_IN_PAGE)
        {
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          cliSyntaxBottom(ewsContext);
          osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
          
          for (cmdIndex=1; cmdIndex<argc; cmdIndex++)
          {
            OSAPI_STRNCAT(cmdBuf, " ");
            OSAPI_STRNCAT(cmdBuf, argv[cmdIndex]);
          }
          cliAlternateCommandSet(cmdBuf);

          return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
        }
      }
   }

   if (firstDcDisplay == L7_TRUE) 
   {
     bzero(buf, sizeof(buf));
     osapiStrncpySafe(buf, pStrInfo_base_HostTotalElapsedTypeAddress, sizeof(buf));
     ewsTelnetWrite(ewsContext, buf);
     bzero(buf, sizeof(buf));
     osapiStrncpySafe(buf, pStrInfo_base_ShowHostsLine_2, sizeof(buf));
     ewsTelnetWrite(ewsContext, buf);
   }


   bzero(strrrType, sizeof(strrrType));
   if (firstDcDisplay == L7_TRUE)
   {
     bzero(rrName, sizeof(rrName));
   }
   tableEmpty = L7_TRUE;
   firstRow = L7_TRUE;
   if (numArg == 1)
   {
     bzero(userHostname, sizeof(userHostname));
     osapiStrncpySafe(userHostname, argv[index + hostnameArg], sizeof(userHostname));
     rc = usmDbIPHostAddressValidate(userHostname, &ipAddr, &addrType);
     if (rc == L7_SUCCESS && addrType == L7_IP_ADDRESS_TYPE_DNS)
     {
       while(usmDbDNSClientCacheRRNextGet(rrName, &rrType, &rrIndex, &rrClass, qname) == L7_SUCCESS)
       {
         usmDbDNSClientCacheRRDataGet(rrName, rrType, rrIndex, data, &primary);
         if ( rrType == DNS_RR_TYPE_ADDRESS)
         {
            osapiStrncpySafe(strrrType, pStrInfo_base_IP, sizeof(strrrType));
            inetAddrHtop((L7_inet_addr_t *)data, strAddr);
         }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
         else if (rrType == DNS_RR_TYPE_IPV6_ADDRESS)
         {
            osapiStrncpySafe(strrrType, pStrInfo_base_IPv6, sizeof(strrrType));
            inetAddrHtop((L7_inet_addr_t *)data, strAddr);
         }
#endif
         else if (rrType == DNS_RR_TYPE_CNAME)
         {
            osapiStrncpySafe(strrrType, pStrInfo_base_Canonical, sizeof(strrrType));
            usmDbDNSClientDisplayNameGet( data, hostname);
            osapiStrncpySafe(strAddr, hostname, sizeof(strAddr));
            strAddr[sizeof(strAddr) - 1] = 0;
         }
         usmDbDNSClientDisplayNameGet(qname, hostname);
         if (strcmp(hostname, userHostname) == 0)
         {
            tableEmpty = L7_FALSE;
            memset(hostname, 0, sizeof(hostname));
            usmDbDNSClientCacheRRPrettyNameGet( rrName, rrType, 
                                                rrIndex, prettyName);
            usmDbDNSClientDisplayNameGet( prettyName, hostname);
            usmDbDNSClientCacheRRTTLGet(rrName, rrType, rrIndex, &ttl);
            usmDbDNSClientCacheRRTTLElapsedGet( rrName, rrType, rrIndex, 
                                                &elapsed);
            cliDisplayDnsDynamicEntries(ewsContext, hostname, strAddr, 
                                        ttl, elapsed, strrrType, &lineCount);
         }
       }
     }
     if ((tableEmpty == L7_TRUE) && (addrType != L7_IP_ADDRESS_TYPE_IPV4) && (rc == L7_SUCCESS))
     {
        osapiStrncpySafe(buf, pStrInfo_base_HostNameNotMapped, sizeof(buf));
        ewsTelnetWrite(ewsContext, buf);
     }
   }
   else if (numArg == 0)
   {
     while(usmDbDNSClientCacheRRNextGet(rrName, &rrType, &rrIndex, &rrClass, qname) == L7_SUCCESS)
     {
         tableEmpty = L7_FALSE;
         usmDbDNSClientCacheRRDataGet(rrName, rrType, rrIndex, data, &primary);
         if ( rrType == DNS_RR_TYPE_ADDRESS)
         {
            osapiStrncpySafe(strrrType, pStrInfo_base_IP, sizeof(strrrType));
            inetAddrHtop((L7_inet_addr_t *)data, strAddr);
         }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
         else if (rrType == DNS_RR_TYPE_IPV6_ADDRESS)
         {
            osapiStrncpySafe(strrrType, pStrInfo_base_IPv6, sizeof(strrrType));
            inetAddrHtop((L7_inet_addr_t *)data, strAddr);
         }
#endif
         else if (rrType == DNS_RR_TYPE_CNAME)
         {
            osapiStrncpySafe(strrrType, pStrInfo_base_Canonical, sizeof(strrrType));
            usmDbDNSClientDisplayNameGet( data, hostname);
            osapiStrncpySafe(strAddr, hostname, sizeof(strAddr));
         }
         usmDbDNSClientCacheRRPrettyNameGet( rrName, rrType, rrIndex, prettyName);
         usmDbDNSClientDisplayNameGet( prettyName, hostname);


         usmDbDNSClientCacheRRTTLGet(rrName, rrType, rrIndex, &ttl);
         usmDbDNSClientCacheRRTTLElapsedGet( rrName, rrType, rrIndex, &elapsed);
         cliDisplayDnsDynamicEntries(ewsContext, hostname, strAddr, 
                                     ttl, elapsed, strrrType, &lineCount);
         if (((CLI_MAX_SCROLL_LINES-6) - lineCount) < CLI_MAX_LINES_IN_PAGE)
         {
           firstDcDisplay = L7_FALSE;
           entriesComplete = L7_FALSE;
           break;
         }

     }
     if ((entriesComplete == L7_FALSE) && 
         (((CLI_MAX_SCROLL_LINES-6) - lineCount) < CLI_MAX_LINES_IN_PAGE))
     {
       cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
       cliSyntaxBottom(ewsContext);
       osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
       
       for (cmdIndex=1; cmdIndex<argc; cmdIndex++)
       {
         OSAPI_STRNCAT(cmdBuf, " ");
         OSAPI_STRNCAT(cmdBuf, argv[cmdIndex]);
       }
       cliAlternateCommandSet(cmdBuf);

       return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
     }
     if(tableEmpty == L7_TRUE && firstDcDisplay == L7_TRUE)
     {
        osapiStrncpySafe(buf, pStrInfo_base_HostNameNotMapped, sizeof(buf));
        ewsTelnetWrite(ewsContext, buf);
     }
   }
   cliSyntaxBottom(ewsContext);
   return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  display the static entries in table format.
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uchar8  hostAddr     @b((input))  Host Address
* @param    L7_uint32  strAddr    @b((input))  IP Address 
* @param    L7_uint32  count        @b((input))  line count
*
* @returntype None
* @return void
*
* @note
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
void cliDisplayDnsStaticEntries(EwsContext ewsContext, L7_uchar8 *hostAddr, 
                                L7_uchar8 *strAddr, L7_uint32 *count) 
{
   L7_uchar8 *hostAddrTraverse = hostAddr;
   L7_BOOL   firstRow = L7_TRUE;
   L7_char8  splitLabel[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

   while (*hostAddrTraverse  != L7_EOS)
   {
     if (firstRow == L7_TRUE)
     {
       memset (splitLabel, 0, sizeof(splitLabel));
       cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
       sprintf(buf, "\r\n%-36s %-40s ", 
                    splitLabel, strAddr);
       ewsTelnetWrite(ewsContext, buf);
       firstRow = L7_FALSE;
       *count = *count + 1;
     } /* end of if (firstRow == L7_TRUE)*/
     else
     {
       /* Displaying remaining hostname in second Row */
       memset (buf, 0, sizeof(buf));
       memset (splitLabel, 0, sizeof(splitLabel));
       /* Get the split hostname which fits in available space 
        * (24 characters) in a row 
        */

       cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
       osapiSnprintf(buf, sizeof(buf), "\r\n%-36s", 
                                       splitLabel);
       ewsTelnetWrite(ewsContext, buf);
       *count = *count + 1;
     }
   }
   return;
}
/*********************************************************************
*
* @purpose  display the Dynamic entries in table format.
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uchar8  hostAddr     @b((input))  Host Address
* @param    L7_uchar8  hostIPAddr   @b((input)) Host IP Address 
* @param    L7_uint32  ttl          @b((input)) TTL
* @param    L7_uint32  elapsed      @b((input)) elapsed
* @param    L7_uchar8  strrrType    @b((input)) RR Type
* @param    L7_uint32  count        @b((input)) line count
*
* @returntype None
* @return void
*
* @note
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
void cliDisplayDnsDynamicEntries(EwsContext ewsContext, L7_uchar8 *hostAddr, 
                                 L7_uchar8 *hostIPAddr, L7_uint32    ttl, 
                                 L7_uint32  elapsed, L7_uchar8 *strrrType, 
                                 L7_uint32 *count)
{
    L7_uchar8 *hostAddrTraverse = hostAddr;
    L7_uchar8 *hostIPAddrTraverse = hostIPAddr;
    L7_BOOL   firstRow = L7_TRUE;
    L7_char8  splitLabel[L7_CLI_MAX_STRING_LENGTH];
    L7_char8  splitLabelIPAddr[L7_CLI_MAX_STRING_LENGTH];
    L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];


    while ((*hostAddrTraverse  != L7_EOS) || 
           (*hostIPAddrTraverse != L7_EOS))
    {
       if (firstRow == L7_TRUE)
       {
          memset (splitLabel, 0, sizeof(splitLabel));
          memset (splitLabelIPAddr, 0, sizeof(splitLabelIPAddr));
          cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
          cliSplitLongNameGet(&hostIPAddrTraverse, splitLabelIPAddr);
          sprintf(buf, "\r\n%-25s %-8d %-8d %-10s %-24s", splitLabel, 
                  ttl, elapsed, strrrType, splitLabelIPAddr);
          ewsTelnetWrite(ewsContext, buf);
          firstRow = L7_FALSE;
          *count = *count + 1;
       }       /* end of if (firstRow == L7_TRUE) */
       else
       {
          /* Displaying remaining hostname in second Row */
          memset (buf, 0, sizeof(buf));
          memset (splitLabel, 0, sizeof(splitLabel));
          memset (splitLabelIPAddr, 0, sizeof(splitLabelIPAddr));
         /* Get the split hostname which fits in available space 
          * (24 characters) in a row. This will also handle
          * when two column host names with two rows.
          */

          if ((*hostAddrTraverse != L7_EOS) && 
              (*hostIPAddrTraverse != L7_EOS))
          {
            cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
            cliSplitLongNameGet(&hostIPAddrTraverse, splitLabelIPAddr);
            osapiSnprintf(buf, sizeof(buf), "\r\n%-25s %-28s %-24s", 
                          splitLabel, pStrInfo_common_EmptyString,
                          splitLabelIPAddr);
          }
          if ((*hostAddrTraverse == L7_EOS) &&
              (*hostIPAddrTraverse != L7_EOS))
          {
            cliSplitLongNameGet(&hostIPAddrTraverse, splitLabelIPAddr);
            osapiSnprintf(buf, sizeof(buf), "\r\n%-25s %-28s %-24s", 
                          pStrInfo_common_EmptyString, 
                          pStrInfo_common_EmptyString,
                          splitLabelIPAddr);
          }
          if ((*hostAddrTraverse != L7_EOS) && 
              (*hostIPAddrTraverse == L7_EOS))
          {
            cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
            osapiSnprintf(buf, sizeof(buf), "\r\n%-25s %-28s %-24s", 
                          splitLabel, 
                          pStrInfo_common_EmptyString,
                          pStrInfo_common_EmptyString);
          }
          ewsTelnetWrite(ewsContext, buf);
          *count = *count + 1;
       }
    }
    return;
}

/*********************************************************************
*
* @purpose  Display the status information about the last detected
*           IP address conflict in the switch
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show ip address-conflict
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpAddrConflict(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 **argv,
                                          L7_uint32 index)
{
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 mac[L7_MAC_ADDR_LEN];
   L7_BOOL   detectStatus = L7_FALSE;
   L7_RC_t   rc;
   L7_uint32 ipAddr, unit, detectTime;
   L7_uint32 numArg;
   L7_timespec tsUptime;

   cliSyntaxTop(ewsContext);
   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_base_ShowIpAddrConflict);
   }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   /* detection status */
   cliFormat(ewsContext, pStrInfo_common_AddrConflictStatus);
   memset (stat, 0, sizeof(stat));
   rc = usmDbIPAddrConflictDetectStatusGet(&detectStatus);
   if(rc == L7_SUCCESS)
   {
     if(L7_TRUE == detectStatus)
     {
       osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_ConflictDetected);
     }
     else
     {
       osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_NoConflictDetected);
     }
   }
   else
   {
     osapiSnprintf(stat, sizeof(stat), "%s", pStrErr_common_Err);
   }
   ewsTelnetWrite(ewsContext, stat);

   if(detectStatus == L7_TRUE)
   {
     /* last reported conflicting ip address */
     cliFormat(ewsContext, pStrInfo_common_LastConflictIP);
     memset (stat, 0, sizeof(stat));
     if(! ((usmDbIPAddrConflictLastDetectIPGet(&ipAddr) == L7_SUCCESS) &&
           (usmDbInetNtoa(ipAddr, stat) == L7_SUCCESS)) )
     {
       osapiSnprintf(stat, sizeof(stat), "%s", pStrErr_common_Err);
     }
     ewsTelnetWrite(ewsContext, stat);
  
     /* last reported conflicting mac address */
     cliFormat(ewsContext, pStrInfo_common_LastConflictMac);
     memset (stat, 0, sizeof(stat));
     if (usmDbIPAddrConflictLastDetectMACGet(mac) == L7_SUCCESS)
     {
       osapiSnprintf(stat, sizeof(stat), "%02X:%02X:%02X:%02X:%02X:%02X",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
     }
     else
     {
       osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
     }
     ewsTelnetWrite(ewsContext, stat);
  
     /* last reported conflict time */
     cliFormat(ewsContext, pStrInfo_common_LastConflictTime);
     memset (stat, 0, sizeof(stat));
     if(usmDbIPAddrConflictLastDetectTimeGet(&detectTime) == L7_SUCCESS)
     {
       osapiConvertRawUpTime(detectTime, &tsUptime);
       osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DaysHrsMinsSecs,
                     tsUptime.days, tsUptime.hours, tsUptime.minutes, tsUptime.seconds);
     }
     else
     {
       osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
     }
     ewsTelnetWrite(ewsContext, stat);
   }
   return cliSyntaxReturnPrompt (ewsContext,"");
}

#ifdef LVL7_DEBUG_BREAKIN
/*********************************************************************
*
* @purpose  show the console break-in status
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  show console break-in
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandSupportShowConsoleBreakin(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_int32  current_handle;
  L7_BOOL rc;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if(numArg !=0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_base_ShowConsoleBreakin);
  }

  unit = cliGetUnitId();
  if(unit == L7_NULL)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext, pStrErr_common_UnitId_1);
  }

  current_handle = cliCurrentHandleGet ();
  cliFormat(ewsContext, pStrInfo_common_ConsoleBreakInStatus);
  rc =  usmDbConsoleBreakinModeGet(current_handle);
  if (rc == L7_TRUE)
  {
    ewsTelnetWrite(ewsContext, "Enable");
  }
  else
  {
    ewsTelnetWrite(ewsContext, "Disable");
  }
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}
#endif


/*********************************************************************
*
* @purpose  display CPU Utilization
*
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
* @cmdsyntax  show process cpu
*
* @cmdhelp Display information about the cpu utilization
*
* @cmddescript Display information about the cpu utilization
*
* @end
*
*********************************************************************/
#define L7_BUFFSIZE  16*1024
const L7_char8 *commandShowProcessCpu(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
   L7_uint32  unit = L7_NULL;
   L7_char8   buffer[L7_BUFFSIZE];
   L7_char8   printBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8   *cpuUtil = "CPU Utilization:";
   L7_uint32  cpuUtilLen = osapiStrnlen(cpuUtil, L7_CLI_MAX_STRING_LENGTH);

   cliSyntaxTop(ewsContext);

   memset(buffer, 0x00, sizeof(buffer));

   cliCmdScrollSet( L7_FALSE);

   unit = cliGetUnitId();
   if (unit == L7_NULL)
   {
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0,pStrErr_common_ErrCouldNot,
                                           ewsContext,pStrErr_common_UnitId_1);
   }
   if(usmdbCpuUtilizationStringGet(unit, buffer, L7_BUFFSIZE) != L7_SUCCESS)
   {
       return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                                pStrErr_common_Cpu_Utilization);
   }
   else
   {
       L7_char8 *start;
       L7_char8 *end;

       start = buffer;
       end   = buffer;
       buffer[L7_BUFFSIZE-1] = 0; /* For safety */

       do
       {
          while ( ( isprint((int)*end ) ) ||
                   ( ESC == *end  ) )
          {
             end++;
          }
          *end = '\0';

          if (osapiStrncmp(start, cpuUtil, cpuUtilLen) == L7_NULL)
          {
            osapiSnprintf(printBuf, L7_CLI_MAX_STRING_LENGTH,"\r\n %s\r\n\r\n", start);
            ewsTelnetWrite(ewsContext, printBuf);
          }
          else
          {
            osapiSnprintf(printBuf, L7_CLI_MAX_STRING_LENGTH,"%s\r\n", start);
            ewsTelnetWrite(ewsContext, printBuf);
          }

          end++;               /* Skip over zero */
          while ( ( iscntrl((int)*end ) ) &&
                   ( *end != ESC ) &&
                   ( *end != '\0' )    &&
                   ( end - buffer < L7_BUFFSIZE  ) )
          {
             end++;
          }
          start = end;
       } while ( ( start - buffer < L7_BUFFSIZE ) && ( start[1] != '\0' ) );
   }
   ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
   return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose  display CPU Utilization
*
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
* @cmdsyntax  show process cpu
*
* @cmdhelp Display information about the cpu utilization
*
* @cmddescript Display information about the cpu utilization
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowProcessCpuThreshold(EwsContext ewsContext, 
                                               L7_uint32 argc, 
                                               const L7_char8 **argv, 
                                               L7_uint32 index)
{
  L7_char8   buffer[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32  unit = L7_NULL;
  L7_uint32  risingThr = 0, risingInterval = 0;
  L7_uint32  fallingThr = 0, fallingInterval = 0;
  L7_uint32  memoryLowWatermark = 0;

   cliSyntaxTop(ewsContext);

   memset(buffer, 0x00, sizeof(buffer));

   cliCmdScrollSet( L7_FALSE);

   unit = cliGetUnitId();
   if (unit == L7_NULL)
   {
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0,pStrErr_common_ErrCouldNot,
                                           ewsContext,pStrErr_common_UnitId_1);
   }

   (void) usmdbCpuUtilMonitorParamGet(unit, 
                                      SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM,
                                      &risingThr);
   (void) usmdbCpuUtilMonitorParamGet(unit, 
                                      SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM,
                                      &risingInterval);
   (void) usmdbCpuUtilMonitorParamGet(unit, 
                                      SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM,
                                      &fallingThr);
   (void) usmdbCpuUtilMonitorParamGet(unit, 
                                      SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM,
                                      &fallingInterval);
   (void) usmdbCpuFreeMemoryThresholdGet(unit, &memoryLowWatermark);

   
   osapiSnprintf(buffer, sizeof(buffer), pStrInfo_base_CpuUtilShow);
   ewsTelnetWrite(ewsContext, buffer);

   cliFormat(ewsContext, pStrInfo_base_CpuUtilRisingThreshold);
   osapiSnprintf(buffer, sizeof(buffer), "%d %%", risingThr);
   ewsTelnetWrite(ewsContext, buffer);

   cliFormat(ewsContext, pStrInfo_base_CpuUtilRisingInterval);
   osapiSnprintf(buffer, sizeof(buffer), "%d secs", risingInterval);
   ewsTelnetWrite(ewsContext, buffer);

   cliFormat(ewsContext, pStrInfo_base_CpuUtilFallingThreshold);
   osapiSnprintf(buffer, sizeof(buffer), "%d %%", fallingThr);
   ewsTelnetWrite(ewsContext, buffer);

   cliFormat(ewsContext, pStrInfo_base_CpuUtilFallingInterval);
   osapiSnprintf(buffer, sizeof(buffer), "%d secs", fallingInterval);
   ewsTelnetWrite(ewsContext, buffer);

   cliSyntaxBottom(ewsContext);

   cliFormat(ewsContext, pStrInfo_base_CpuMemoryLwm);
   osapiSnprintf(buffer, sizeof(buffer), "%d KB", memoryLowWatermark);
   ewsTelnetWrite(ewsContext, buffer);
   cliSyntaxBottom(ewsContext);

   ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
   return cliPrompt(ewsContext);
}

#ifdef L7_AUTO_INSTALL_PACKAGE
/******************************************************************************
*
* @purpose  Displays autoinstall state.
*
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
* @cmdsyntax  show autoinstall
*
* @cmdhelp Displays autoinstall state.
*
* @cmddescript Displays autoinstall state.
*
* @end
*
******************************************************************************/
const L7_char8 * commandShowAutoinstall(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_char8 status[L7_CLI_MAX_STRING_LENGTH]; 
  L7_BOOL mode = L7_FALSE, autoSave = L7_FALSE;
  L7_uint32 retryCnt = L7_NULL;

  memset(status, 0x00, sizeof(status));

  cliSyntaxTop(ewsContext);

  if (usmdbAutoInstallStartStopGet(&mode) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_base_AutoinstallModePrn);
    memset (status, 0, sizeof(status));
    osapiSnprintf(status, sizeof(status), "%s", (mode == L7_TRUE) ? "Started": "Stopped");
    ewsTelnetWrite(ewsContext, status);
  }

  if (usmdbAutoInstallAutoSaveGet(&autoSave) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_base_AutoinstallAutoSavePrn);
    memset (status, 0, sizeof(status));
    osapiSnprintf(status, sizeof(status), "%s", (autoSave == L7_TRUE) ? "Enabled": "Disabled");
    ewsTelnetWrite(ewsContext, status);
  }
  if (usmdbAutoInstallFileDownLoadRetryCountGet(&retryCnt) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_base_AutoinstallRetryCountPrn);
    memset (status, 0, sizeof(status));
    osapiSnprintf(status, sizeof(status), "%d", retryCnt);
    ewsTelnetWrite(ewsContext, status);
  }

  if (mode == L7_TRUE)
  {
    memset (status, 0, sizeof(status));
    if (usmdbAutoInstallCurrentStatusGet(status) == L7_SUCCESS)
    {
      if (strlen(status) > 0)
      {
        cliFormat(ewsContext, pStrInfo_base_AutoinstallStatePrn);
        ewsTelnetWrite(ewsContext, status);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                            pStrErr_base_AutoInstallState);
    }
  }
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliPrompt(ewsContext);
}
#endif /* L7_AUTO_INSTALL_PACKAGE */

/*********************************************************************
*
* @purpose Show parameters from a specific SDM template.
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
* @cmdsyntax    N/A
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
static void cliShowSdmTemplateParameters(EwsContext ewsContext, sdmTemplateId_t templateId)
{
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, "ARP Entries");
  osapiSnprintf(buf, sizeof(buf), "%u", usmDbSdmTemplateMaxArpEntriesGet(templateId));
  ewsTelnetWrite(ewsContext, buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, "IPv4 Unicast Routes");
  osapiSnprintf(buf, sizeof(buf), "%u", usmDbSdmTemplateMaxIpv4RoutesGet(templateId));
  ewsTelnetWrite(ewsContext, buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, "IPv6 NDP Entries");
  osapiSnprintf(buf, sizeof(buf), "%u", usmDbSdmTemplateMaxNdpEntriesGet(templateId));
  ewsTelnetWrite(ewsContext, buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, "IPv6 Unicast Routes");
  osapiSnprintf(buf, sizeof(buf), "%u", usmDbSdmTemplateMaxIpv6RoutesGet(templateId));
  ewsTelnetWrite(ewsContext, buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, "ECMP Next Hops");
  osapiSnprintf(buf, sizeof(buf), "%u", usmDbSdmTemplateMaxEcmpNextHopsGet(templateId));
  ewsTelnetWrite(ewsContext, buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, "IPv4 Multicast Routes");
  osapiSnprintf(buf, sizeof(buf), "%u", usmDbSdmTemplateMaxIpv4McastRoutesGet(templateId));
  ewsTelnetWrite(ewsContext, buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, "IPv6 Multicast Routes");
  osapiSnprintf(buf, sizeof(buf), "%u", usmDbSdmTemplateMaxIpv6McastRoutesGet(templateId));
  ewsTelnetWrite(ewsContext, buf);
}

/*********************************************************************
*
* @purpose Displays SDM template configuration.
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
* @cmdsyntax    show sdm prefer  [ dual-ipv4-and-ipv6 default | ipv4-routing { default | data-center } ]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowSdmPrefer(EwsContext ewsContext, L7_uint32 argc,
                                 const L7_char8 **argv, L7_uint32 index)
{
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg = 0;    
  L7_uint32 sdmGroupPos = 1;     /* index of keyword that selects template group */
  L7_uint32 sdmTemplatePos = 2;  /* index of keyword that specifies template within group */
  sdmTemplateId_t templateId = SDM_TEMPLATE_NONE;
  sdmTemplateId_t curTemplateId = SDM_TEMPLATE_NONE;
  sdmTemplateId_t nextTemplateId = SDM_TEMPLATE_NONE;
  L7_uchar8 *syntax = "Use 'show sdm prefer {dual-ipv4-and-ipv6 default | ipv4-routing {default | data-center}}'.";

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((numArg != 0) && (numArg != 2))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, syntax);
  }

  if (numArg == 0)
  {
    cliSyntaxTop(ewsContext);
    /* Identify the current template. */
    curTemplateId = usmDbSdmActiveTemplateGet();

    memset(buf, 0, L7_CLI_MAX_STRING_LENGTH);
    snprintf(buf, L7_CLI_MAX_STRING_LENGTH,
             "The current template is the %s template.",
             usmDbSdmTemplateNameGet(curTemplateId));
    ewsTelnetWrite(ewsContext, buf);
    cliSyntaxBottom(ewsContext);

    /* Now show the parameters in the template */
    cliShowSdmTemplateParameters(ewsContext, curTemplateId);
    cliSyntaxBottom(ewsContext);

    /* If next template is set, identify it. */
    nextTemplateId = usmDbSdmNextActiveTemplateGet();
    if ((nextTemplateId != SDM_TEMPLATE_NONE) && 
        (nextTemplateId != curTemplateId) &&
        usmDbSdmTemplateSupported(nextTemplateId))
    {
      cliSyntaxTop(ewsContext);
      memset(buf, 0, L7_CLI_MAX_STRING_LENGTH);
      snprintf(buf, L7_CLI_MAX_STRING_LENGTH,
               "On the next reload, the template will be the %s template.",
               usmDbSdmTemplateNameGet(nextTemplateId));
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  {
    /* show parameters for a specific template */
    if (strcmp(argv[index + sdmGroupPos], "dual-ipv4-and-ipv6") == 0)
    {
      if (strcmp(argv[index + sdmTemplatePos], "default") == 0)
      {
        templateId = SDM_TEMPLATE_DUAL_DEFAULT;
      }
      else
      {
        osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), syntax);
        ewsTelnetWrite(ewsContext, buf);
        return cliPrompt(ewsContext);
      }
    }
    else if (strcmp(argv[index + sdmGroupPos], "ipv4-routing") == 0)
    {
      if (strcmp(argv[index + sdmTemplatePos], "default") == 0)
      {
        templateId = SDM_TEMPLATE_V4_DEFAULT;
      }
      else if (strcmp(argv[index + sdmTemplatePos], "data-center") == 0)
      {
        templateId = SDM_TEMPLATE_V4_DATA_CENTER;
      }
      else
      {
        osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), syntax);
        ewsTelnetWrite(ewsContext, buf);
        return cliPrompt(ewsContext);
      }
    }
    cliShowSdmTemplateParameters(ewsContext, templateId);
    cliSyntaxBottom(ewsContext);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}




