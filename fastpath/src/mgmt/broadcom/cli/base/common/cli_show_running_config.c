/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config.c
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
#define SNMP_FACTORY_DEFAULT FACTORY_DEFAULT_DEFINE_CLI
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_stacking_cli.h"
#include "commdefs.h"

#include "l7_common.h"
#include "acl_exports.h"
#include "cos_exports.h"
#include "diffserv_exports.h"
#include "doscontrol_exports.h"
#include "dot1q_exports.h"
#include "dot1x_exports.h"
#include "fdb_exports.h"
#include "nim_exports.h"
#include "log_exports.h"
#include "radius_exports.h"
#include "snmp_exports.h"
#include "sntp_exports.h"
#include "transfer_exports.h"
#include "user_manager_exports.h"
#include "usmdb_1213_api.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_common.h"
#include "usmdb_dot1d_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_log_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mib_diffserv_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_pml_api.h"
#include "usmdb_policy_api.h"
#include "usmdb_port_user.h"
#include "usmdb_protected_port_api.h"
#include "usmdb_radius_api.h"
#include "usmdb_registry_api.h"
#include "usmdb_rlim_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_sntp_api.h"
#include "usmdb_telnet_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_unitmgr_api.h"
#include "dot3ad_exports.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_acl_api.h"
#include "usmdb_qos_cos_api.h"
#endif

#ifdef L7_WIRELESS_PACKAGE
#include "usmdb_wdm_api.h"
#endif

#include "comm_mask.h"
#include "cli_show_running_config.h"
#include "config_script_api.h"
#include "osapi_support.h"
#include "usmdb_dhcp_client.h"
#include "dot1ad_l2tp_api.h"
#include "usmdb_filter_api.h"

#ifdef L7_BGP_PACKAGE
#include "cliutil_bgp.h"
#include "usmdb_bgp4_api.h"
#include "usmdb_mib_bgp4_api.h"
#include "l3_bgp_defaultconfig.h"
#include "l3_bgp_default_cnfgr.h"
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
#include "acl_api.h"
#endif

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_garp.h"
#include "usmdb_cmd_logger_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dos_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"
#include "clicommands_card.h"

#ifdef L7_ROUTING_PACKAGE
#include "usmdb_rtrdisc_api.h"
#include "l3_mcast_defaultconfig.h"
#endif

#include "cliutil_dot1s.h"
#include "dot1q_api.h"
#include "cliutil_dot1x.h"


#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
#include "cliutil_diffserv.h"
#endif

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
#include "usmdb_qos_voip_api.h"
#endif

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
#include "usmdb_ip6_api.h"
#include "osapi_support.h"
#include "ipv6_default_cnfgr.h"
#include "ipv6_defaultconfig.h"
#include "clicommands_base_ipv6.h"
#include "clicommands.h"
#endif
#include "usmdb_dhcp6c_api.h"

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
#include "clicommands_ipv6.h"
#ifdef L7_MCAST_PACKAGE
#include "clicommands_mcast6.h"
#endif
#endif
#endif

#ifdef L7_WIRELESS_PACKAGE
#include "wireless_defaultconfig.h"
#endif

#include "usmdb_dns_client_api.h"
#include "usmdb_dhcps_api.h"

#ifdef L7_DHCP_FILTER_PACKAGE
#include "usmdb_dhcp_filtering.h"
#include "clicommands_dhcp_filtering.h"
#endif

#include "clicommands_lldp.h"
#ifdef L7_SFLOW_PACKAGE
#include "clicommands_sflow.h"
#endif

#ifdef L7_STATIC_FILTERING_PACKAGE
#include "clicommands_filter.h"
#endif

#include "clicommands_lacp.h"
#include "usmdb_lldp_api.h"

#include "usmdb_tacacs_api.h"
#include "clicommands_tacacs.h"
#include "clicommands_usermgr.h"
#include "bspapi.h"
#include "dot3ad_api.h"
#include "pw_scramble_api.h"
#include "snooping_exports.h"
#include "usmdb_user_mgmt_api.h"
#include "usmdb_filter_api.h"

#ifdef L7_TR069_PACKAGE
#include "clicommands_tr069.h"
#endif

#ifdef L7_AUTO_INSTALL_PACKAGE
  #include "usmdb_auto_install_api.h"
#endif 

#include "dot1x_auth_serv_exports.h"
#include "clicommands_time.h"
#include "user_mgr_api.h"

L7_char8 currentInterfaceString[L7_CLI_MAX_STRING_LENGTH];

/* This flag indicates given command is show running config all or not. When you give 
   the show running or show running all, first it stores all the running configuraton 
   in buffer.We have restricted that buffer size for normal show running or save 
   configuration(i.e show running config) is 2MB. For show running config all the 
   buffer size is 5MB. When show running config all case, we will set this flag 
   as 1, Based on this value buffer size will be expanded up to 5MB. For normal show 
   running config case we will set this flag as 0, Based on this value buffer size will
   be expanded up to 2MB. */    

L7_uint32 showRunningConfigAllFlag = 0;
extern void *cliRunCfgSema;

/*********************************************************************
 * @purpose  To print the running configuration of system info
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

L7_RC_t cliRunningConfigSysInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[CONFIG_SCRIPT_MAX_COMMAND_SIZE+1];
  L7_BOOL addpkgs = L7_FALSE;
  L7_BOOL cama = L7_FALSE;
#ifndef L7_CHASSIS
  L7_uchar8 dummy[L7_CLI_MAX_STRING_LENGTH];
#endif
  usmDbTimeSpec_t ts;
  L7_uint32 val = 0;
  L7_RC_t rc;

  if ( usmDb1213SysDescrGet(unit, stat)  == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SysDesc, stat);
    EWSWRITEBUFFER(ewsContext, buf);
  }

  if ( usmDbSwVersionGet(unit, stat)  == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SysSoftwareVer, stat);
    EWSWRITEBUFFER(ewsContext, buf);
  }
  /*   osapiSnprintf(buf, sizeof(buf), "\r\n!Image Filename          \"%s\"", fileName);
       osapiSnprintf(buf, sizeof(buf), "\r\n!Image Timestamp         \"%s\"", timeStamp);*/

  /* System up time */
  usmDb1213SysUpTimeGet(unit, &ts);
  osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DaysHrsMinsSecs, ts.days, ts.hours, ts.minutes, ts.seconds);
  osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SysUpTime, stat);
  EWSWRITEBUFFER(ewsContext, buf);

  osapiSnprintfAddBlanks (1, 0, 0, 5, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_AdditionalPackages, stat);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    osapiSnprintf(stat,sizeof(stat),L7_FASTPATH_FLEX_BGP_PACKAGE_STR);
    strcat(buf,stat);
    cama = L7_TRUE;
  }

  if ((usmDbComponentPresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_TRUE) ||
      (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID) == L7_TRUE) ||
      (usmDbComponentPresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID) == L7_TRUE))
  {
    addpkgs = L7_TRUE;
    if(cama == L7_TRUE)
    {
      osapiSnprintf(stat,sizeof(stat),",%s", L7_FASTPATH_FLEX_QOS_PACKAGE_STR);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), L7_FASTPATH_FLEX_QOS_PACKAGE_STR);
    }
    strcat(buf,stat);
    cama = L7_TRUE;
  }

  if (usmDbComponentPresentCheck(unit, L7_FLEX_MCAST_MAP_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    if(cama == L7_TRUE)
    {
      osapiSnprintf(stat,sizeof(stat),",%s", L7_FASTPATH_FLEX_MCAST_PACKAGE_STR);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), L7_FASTPATH_FLEX_MCAST_PACKAGE_STR);
    }
    strcat(buf,stat);
    cama = L7_TRUE;
  }
  if (usmDbComponentPresentCheck(unit, L7_FLEX_IPV6_MAP_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    if(cama == L7_TRUE)
    {
      osapiSnprintf(stat,sizeof(stat),",%s", L7_FASTPATH_FLEX_IPV6_PACKAGE_STR);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), L7_FASTPATH_FLEX_IPV6_PACKAGE_STR);
    }
    strcat(buf,stat);
    cama = L7_TRUE;
  }

  if (usmDbComponentPresentCheck(unit, L7_FLEX_WIRELESS_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    if(cama == L7_TRUE)
    {
      osapiSnprintf(stat,sizeof(stat),",%s",L7_FASTPATH_FLEX_WIRELESS_PACKAGE_STR);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat),L7_FASTPATH_FLEX_WIRELESS_PACKAGE_STR);
    }
    strcat(buf, stat);
    cama = L7_TRUE;
  }

  if((usmDbComponentPresentCheck(unit, L7_FLEX_METRO_DOT1AD_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_DOT3AH_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_DOT1AG_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(unit, L7_TR069_COMPONENT_ID)  == L7_TRUE))
  {
    addpkgs = L7_TRUE;
    if(cama == L7_TRUE)
    {
      osapiSnprintf(stat,sizeof(stat),",%s",L7_FASTPATH_FLEX_METRO_PACKAGE_STR);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat),L7_FASTPATH_FLEX_METRO_PACKAGE_STR);
    }
    strcat(buf, stat);
    cama = L7_TRUE;
  }

#ifndef L7_CHASSIS
  if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    if(cama == L7_TRUE)
    {
      osapiSnprintf(stat,sizeof(stat),",%s",L7_FASTPATH_FLEX_STACKING_PACKAGE_STR,dummy);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat),L7_FASTPATH_FLEX_STACKING_PACKAGE_STR,dummy);
    }
    strcat(buf, stat);
  }
#endif
  if (addpkgs == L7_FALSE)
  {
    osapiSnprintf(stat,sizeof(stat), pStrInfo_common_None_1);
    strcat(buf, stat);
  }
  EWSWRITEBUFFER(ewsContext, buf);
  /*
   * Get System Name , Location and contact Default values set to ""
   * in simBuildDefaultConfigData. It should set to
   * Factory default.
   */

  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_CurrentSntpSyncronizedTime);

  rc = usmDbSntpLastUpdateTimeGet (unit, &val);

  if (rc != L7_SUCCESS || val == 0)
  {
    osapiSnprintf(buf, sizeof(buf), pStrErr_base_SysSntpNotSync);
    strcat(stat, buf);
  }
  else
  {
    osapiStrncpySafe(buf, (const char *)usmDbConvertTimeToDateString((L7_uint32)osapiUTCTimeNow()), min(20, (sizeof(buf)-1)));
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, buf,pStrInfo_base_Utc);
    strcat(stat, buf);
  }
  EWSWRITEBUFFER(ewsContext, stat);
  /*Still we need add two more fields image name and imgae time stamp of current image */

  EWSWRITEBUFFER(ewsContext, "\r\n!\r\n");

  return L7_SUCCESS;
}

L7_RC_t cliRunningConfigSysDesc(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 stat[L7_SYS_SIZE+1];

  memset(stat, 0, sizeof(stat));
  if (usmDb1213SysNameGet(unit, stat) == L7_SUCCESS)
  {
    cliShowCmdQuotedStrcmp(ewsContext, stat, FD_SNMP_SVR_DEFAULT_SYSNAME, pStrInfo_base_SnmpSrvrSysname);
  }

  memset(stat, 0, sizeof(stat));
  if (usmDb1213SysLocationGet(unit, stat) == L7_SUCCESS)
  {
    cliShowCmdQuotedStrcmp(ewsContext, stat, FD_SNMP_SVR_DEFAULT_SYSLOC, pStrInfo_base_SnmpSrvrLocation);
  }

  memset(stat, 0, sizeof(stat));
  if (usmDb1213SysContactGet(unit, stat) == L7_SUCCESS)
  {
    cliShowCmdQuotedStrcmp(ewsContext, stat, FD_SNMP_SVR_DEFAULT_CONTACT, pStrInfo_base_SnmpSrvrContact);
  }

  EWSWRITEBUFFER(ewsContext, "\r\n!\r\n");

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of snmp info
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

L7_RC_t cliRunningConfigSnmpInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_ushort16 count, communityIndex;
  L7_uint32 val, val1, val2, status;
  L7_BOOL displayDefComm = L7_FALSE;
  L7_char8 snmpCommunity[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[CONFIG_SCRIPT_MAX_COMMAND_SIZE+1];
  L7_char8 stat[CONFIG_SCRIPT_MAX_COMMAND_SIZE+1];
  L7_RC_t rc, rc1, rc2;
 
  /*  Get SNMP config info */
  for ( count=0; count<L7_MAX_SNMP_COMM; count++ )
  {
    if ((usmDbSnmpCommunityNameGet(unit, count, snmpCommunity) == L7_SUCCESS ) && 
        (strcmp(snmpCommunity, "") == 0))
    {
      continue; 
    }
    
    rc = usmDbSnmpCommunityStatusGet(unit, count, &status);
    if((rc== L7_SUCCESS) && (status != L7_SNMP_COMMUNITY_STATUS_DELETE))
    {
      rc = usmDbSnmpCommunityIpAddrGet(unit, count, &val);
      rc1 = usmDbSnmpCommunityIpMaskGet(unit, count, &val1);
      rc2 = usmDbSnmpCommunityAccessLevelGet(unit, count, &val2);
      displayDefComm  = L7_FALSE;
      for(communityIndex = 0; communityIndex < L7_MAX_SNMP_COMM; communityIndex++ )
      {
        if(strcmp(snmpCommunity, FD_snmpComm[communityIndex].agentCommunityName ) == 0)
        {
          displayDefComm  = L7_TRUE;
          break;
        }
      }

      /* Diaply non-default community name */
      if((displayDefComm != L7_TRUE) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SnmpSrvrCommunity, snmpCommunity);
        EWSWRITEBUFFER(ewsContext, buf);
      }

       /* IP Address/Mask is checked against 0 as the same is defined in FD_snmpComm.  
       * Any changes for this field in FD_snmpComm have to be taken care of in running 
       * config and config command.
       */ 

      if( (rc == L7_SUCCESS) && 
          (((val != 0) && ((displayDefComm != L7_TRUE) || (val != FD_snmpComm[communityIndex].agentCommunityClientIpAddr))) || 
           (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))) 
      {
        memset(stat, 0x00, sizeof(stat));
        if (usmDbInetNtoa(val, stat) == L7_SUCCESS)
        {
          osapiSnprintf (buf, sizeof (buf), "\r\n%s %s %s", pStrInfo_base_SnmpSrvrCommunityIpaddr, stat, snmpCommunity);
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
      
      if( (rc1 == L7_SUCCESS) && 
          (((val1 != 0) && ((displayDefComm != L7_TRUE) || (val1 != FD_snmpComm[communityIndex].agentCommunityClientIpMask))) || 
           (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
      {
        memset(stat, 0x00, sizeof(stat));
        if (usmDbInetNtoa(val1, stat) == L7_SUCCESS)
        {
          osapiSnprintf (buf, sizeof (buf), "\r\n%s %s %s", pStrInfo_base_SnmpSrvrCommunityIpmask, stat, snmpCommunity);
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }

      /* Access Level is checked against L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY as the 
       * same is defined in FD_snmpComm. Any changes for this field in FD_snmpComm have  
       * to be taken care of in running config and config command.
       */
      if( (rc2 == L7_SUCCESS) && 
          (((displayDefComm != L7_TRUE) && (val2 != L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY)) ||
           ((displayDefComm == L7_TRUE) && (val2 != FD_snmpComm[communityIndex].agentCommunityAccessLevel)) ||
           (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))

      {
        switch(val2)
        {
           case L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY:
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SnmpSrvrCommunityRo, snmpCommunity);
             EWSWRITEBUFFER(ewsContext, buf);
             break;
           case L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE:
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SnmpSrvrCommunityRw, snmpCommunity);
             EWSWRITEBUFFER(ewsContext, buf);
             break;
           default:
             /* Do nothing */
             break;
        }
      }
    }

    if ((rc==L7_SUCCESS) && (status == L7_SNMP_COMMUNITY_STATUS_INVALID)) 
    {
      /* val = "Invalid","Valid","Config","Delete" */
      /* don't check for L7_SNMP_COMMUNITY_STATUS_VALID, since that's what
       *   new communities default to.
       */
       osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_NoSnmpSrvrCommunityMode,
                               snmpCommunity);
       EWSWRITEBUFFER(ewsContext, buf);
    }
  } /* End of snmp get info for*/

  for(count = 0; count < L7_MAX_SNMP_COMM; count++)
  {
    if(strcmp(FD_snmpComm[count].agentCommunityName, "") != 0)
    {
      displayDefComm = L7_TRUE;
      for(communityIndex = 0; communityIndex < L7_MAX_SNMP_COMM; communityIndex++)
      {
        if ((usmDbSnmpCommunityNameGet(unit, communityIndex, snmpCommunity) == L7_SUCCESS ) &&
           (strcmp(snmpCommunity, FD_snmpComm[count].agentCommunityName) == 0))
        {
          displayDefComm = L7_FALSE;
          break;
        }
      }
      if(displayDefComm == L7_TRUE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_NoSnmpSrvrCommunity, FD_snmpComm[count].agentCommunityName);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
    else
    {
      break;  /* Assuming default values are populated from first index in FD_snmpComm */
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of snmp trap info
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

L7_RC_t cliRunningConfigSnmpTrapInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_ushort16 count;
  L7_uint32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat2[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 snmpCommunity[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH +1];
  snmpTrapVer_t trap;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_in6_addr_t ipv6_address;
  L7_char8 stat1[L7_CLI_MAX_STRING_LENGTH];
  memset(&ipv6_address, 0, sizeof(L7_in6_addr_t));
#endif
  memset (buf, 0, sizeof(buf));
  memset (stat, 0, sizeof(stat));
  memset (stat2, 0, sizeof(stat2));

  for ( count=0; count<L7_MAX_SNMP_COMM; count++ )
  {
    if (usmDbTrapManagerCommIpGet(unit, count, snmpCommunity) == L7_SUCCESS )
    {
      if ((strcmp(snmpCommunity, FD_snmpTrapMgr[count].agentTrapMgrCommunityName ) != 0)||
          (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        if (strcmp(snmpCommunity,pStrInfo_common_EmptyString)!=0)
        {
          osapiSnprintf (buf, sizeof(buf), pStrInfo_base_Snmptrap_1, snmpCommunity);

          /* This may have default ip address */
          if (usmDbTrapManagerIpAddrGet(unit, count, &val) == L7_SUCCESS)
          {
            if(val != 0)
            {
              memset (stat, 0,sizeof(stat));
              if (usmDbInetNtoa(val, stat) == L7_SUCCESS)
              {
                OSAPI_STRNCAT(buf," ");
                OSAPI_STRNCAT(buf,pStrInfo_base_Ipaddr_2);
                OSAPI_STRNCAT(buf," ");
                OSAPI_STRNCAT(buf,stat);
              }
            }
          }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
          if(usmDbTrapManagerIPv6AddrGet(unit, count, &ipv6_address) == L7_SUCCESS)
          {
            memset (stat1, 0,sizeof(stat1));
            osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, stat1, sizeof(stat1));
            if(strcmp(stat1, "::") != 0)
            {
              strcat(buf," ");
              strcat(buf,pStrInfo_base_Ip6addr_1);
              strcat(buf," ");
              strcat(buf, stat1);
              OSAPI_STRNCPY_SAFE(stat, stat1);
            }
          }
#endif

          if(usmDbTrapManagerVersionGet(unit, count, &trap) == L7_SUCCESS)
          {
            if((trap != FD_SNMP_DEFAULT_TRAP_VERSION) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
            {
              if(trap==L7_SNMP_TRAP_VER_SNMPV1)
              {
                osapiSnprintf(stat2, sizeof(stat2), pStrInfo_base_SnmpversionSnmpv1);
              }
              else
              {
                osapiSnprintf(stat2, sizeof(stat2), pStrInfo_base_SnmpversionSnmpv2);
              }
              OSAPI_STRNCAT(buf," ");
              OSAPI_STRNCAT(buf,stat2);
            }
          }

          EWSWRITEBUFFER(ewsContext, buf);
          memset (buf, 0, sizeof(buf));
          if (usmDbTrapManagerStatusIpGet(unit, count, &val) == L7_SUCCESS )
          {

            if ((( val !=  FD_snmpTrapMgr[count].agentTrapMgrStatus )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) && (val != L7_ENABLE))
            {
              switch ( val )   /* val = "Invalid","Enable","Disable","Delete" */
              {
                case L7_SNMP_TRAP_MGR_STATUS_INVALID:
                  OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, buf,pStrInfo_base_NoSnmptrapMode);
                  OSAPI_STRNCAT(buf," ");
                  OSAPI_STRNCAT(buf,snmpCommunity);
                  OSAPI_STRNCAT(buf," ");
                  OSAPI_STRNCAT(buf,stat);
                  break;
                case L7_SNMP_TRAP_MGR_STATUS_DELETE:
                  OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, buf,pStrInfo_base_NoSnmptrap);
                  OSAPI_STRNCAT(buf," ");
                  OSAPI_STRNCAT(buf,snmpCommunity);
                  OSAPI_STRNCAT(buf," ");
                  OSAPI_STRNCAT(buf,stat);
                  break;
                default:
                  /* Do nothing */
                  break;
              }
            }
          }
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    }
  } /* End of snmp get info for*/

  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of forwardingdb info
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

L7_RC_t cliRunningConfigForwardingDbInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_ushort16 count;
  L7_uint32 val;
  L7_uint32 temp = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 vlanID = FD_SIM_DEFAULT_MGMT_VLAN_ID;
  L7_uint32 vlanFeature = L7_FALSE;
  L7_FDB_TYPE_t fdb_type = L7_SVL;

  /* Get the type of supported VL */
  usmDbFDBTypeOfVLGet(unit, &fdb_type);

  /* Check to see if VLAN Feature is supported */
  vlanFeature = usmDbFeaturePresentCheck(unit, L7_FDB_COMPONENT_ID, L7_FDB_AGETIME_PER_VLAN_FEATURE_ID);

  if (fdb_type == L7_SVL || vlanFeature != L7_TRUE)
  {
    if (fdb_type == L7_SVL)
    {
      usmDbDot1dTpAgingTimeGet(unit, &val);
    }
    else
    {
      usmDbFDBAddressAgingTimeoutGet(unit, vlanID, &val);
    }

    cliShowCmdInt(ewsContext,val,FD_FDB_DEFAULT_AGING_TIMEOUT,pStrInfo_base_BridgeAgingTime);
  }
  /* if vlan type is IVL and VLAN Feature is supported */
  else
  {
    for (count = 0; count < L7_MAX_FILTERING_DATABASES; count++)
    {
      if (usmDbFDBAddressAgingTimeoutGet(unit, count, &val) == L7_SUCCESS)
      {
        if ((val != FD_FDB_DEFAULT_AGING_TIMEOUT)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
        {
          if (temp == 0)
          {
            temp = val;
          }
          else
          {
            if (temp != val)
            {
              /* if aging timeout is forwarding database specific */
              temp = 0;
              for (count = 0; count < L7_MAX_FILTERING_DATABASES; count++)
              {
                if (usmDbFDBAddressAgingTimeoutGet(unit, count, &val) == L7_SUCCESS)
                {
                  if ((val != FD_FDB_DEFAULT_AGING_TIMEOUT)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
                  {
                    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_BridgeAgingTime_1, val, count);
                    EWSWRITEBUFFER(ewsContext, stat);
                  }
                }
              } /* end of inner for loop */

              break; /* come out from the outer for loop */
            }
          } /* end of inner else */
        }
      }
    } /* end of outer for loop */

    /* if aging timeout is same for all forwarding databases */
    if (temp != 0)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_BridgeAgingTimeAll, temp);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  } /* end of outer else */

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of garp info
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

L7_RC_t cliRunningConfigGarpInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if(usmDbGarpGmrpEnableGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, val,FD_GMRP_GARP_DEFAULT_SWITCH_STATUS, pStrInfo_base_SetGmrpAdminmode);
  }

  if(usmDbGarpGvrpEnableGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, val, FD_GMRP_GARP_DEFAULT_SWITCH_STATUS, pStrInfo_base_SetGvrpAdminmode);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of igmp snooping info
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

L7_RC_t cliRunningConfigIgmpSnoopingInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  L7_uchar8 ipAddr[L7_IP6_ADDR_LEN], nullIp[L7_IP6_ADDR_LEN];

  memset (nullIp, 0, L7_IP6_ADDR_LEN);
  if (usmDbFeaturePresentCheck (unit, L7_SNOOPING_COMPONENT_ID, L7_IGMP_SNOOPING_FEATURE_SUPPORTED)
      == L7_TRUE)
  {

    if (usmDbSnoopAdminModeGet (unit, &val, L7_AF_INET) == L7_SUCCESS)
    {
      cliShowCmdEnable (ewsContext, val, FD_IGMP_SNOOPING_ADMIN_MODE, pStrInfo_base_SetIgmp);
    }

    if (usmDbSnoopRouterAlertMandatoryGet(&val, L7_AF_INET) == L7_SUCCESS )
    {
      cliShowCmdEnable(ewsContext,val,FD_IGMP_SNOOPING_ROUTER_ALERT_CHECK, pStrInfo_base_SetIgmpRouterAlert);
    }

    if (usmDbSnoopQuerierAdminModeGet (&val, L7_AF_INET) == L7_SUCCESS)
    {
      cliShowCmdEnable (ewsContext, val, FD_IGMP_SNOOPING_QUERIER_ADMIN_MODE, pStrInfo_base_SetIgmpQuerier);
    }
    /* Querier Address */
    memset (ipAddr, 0, L7_IP6_ADDR_LEN);
    if (usmDbSnoopQuerierAddressGet (ipAddr, L7_AF_INET) == L7_SUCCESS)
    {
      cliShowCmdIpAddr (ewsContext, L7_AF_INET, ipAddr, L7_NULLPTR, pStrInfo_base_SetIgmpQuerierAddr_1);
    }
    /* Querier Version */
    if (usmDbSnoopQuerierVersionGet (&val, L7_AF_INET) == L7_SUCCESS)
    {
      cliShowCmdInt (ewsContext, val, FD_IGMP_SNOOPING_QUERIER_VERSION, pStrInfo_base_SetIgmpQuerierVer);
    }

    /* Query Interval */
    if (usmDbSnoopQuerierQueryIntervalGet (&val, L7_AF_INET) == L7_SUCCESS)
    {
      cliShowCmdInt (ewsContext, val, FD_IGMP_SNOOPING_QUERIER_QUERY_INTERVAL, pStrInfo_base_SetIgmpQuerierQueryIntvl);
    }

    /* Querier Expiry time */
    if (usmDbSnoopQuerierExpiryIntervalGet (&val, L7_AF_INET) == L7_SUCCESS)
    {
      cliShowCmdInt (ewsContext, val, FD_IGMP_SNOOPING_QUERIER_EXPIRY_INTERVAL, pStrInfo_base_SetIgmpQueryTimerExpiry);
    }
  }

  if (usmDbFeaturePresentCheck (unit, L7_SNOOPING_COMPONENT_ID, L7_MLD_SNOOPING_FEATURE_SUPPORTED)
      == L7_TRUE)
  {
    if (usmDbSnoopAdminModeGet (unit, &val, L7_AF_INET6) == L7_SUCCESS)
    {
      cliShowCmdEnable (ewsContext, val, FD_MLD_SNOOPING_ADMIN_MODE, pStrInfo_base_SetMld);
    }
    /* Querier */
    if (usmDbSnoopQuerierAdminModeGet (&val, L7_AF_INET6) == L7_SUCCESS)
    {
      cliShowCmdEnable (ewsContext, val, FD_MLD_SNOOPING_QUERIER_ADMIN_MODE, pStrInfo_base_SetMldQuerier);
    }

    /* Querier Address */
    memset (ipAddr, 0, L7_IP6_ADDR_LEN);
    if (usmDbSnoopQuerierAddressGet (ipAddr, L7_AF_INET6) == L7_SUCCESS)
    {
      cliShowCmdIpAddr (ewsContext, L7_AF_INET6, ipAddr, L7_NULLPTR, pStrInfo_base_SetMldQuerierAddr_1);
    }

    /* Querier Version */
    if (usmDbSnoopQuerierVersionGet (&val, L7_AF_INET6) == L7_SUCCESS)
    {
      cliShowCmdInt (ewsContext, val, FD_MLD_SNOOPING_QUERIER_VERSION, pStrInfo_base_SetMldQuerierVer);
    }

    /* Query Interval */
    if (usmDbSnoopQuerierQueryIntervalGet (&val, L7_AF_INET6) == L7_SUCCESS)
    {
      cliShowCmdInt (ewsContext, val, FD_MLD_SNOOPING_QUERIER_QUERY_INTERVAL, pStrInfo_base_SetMldQuerierIntvl);
    }

    /* Querier Expiry time */
    if (usmDbSnoopQuerierExpiryIntervalGet (&val, L7_AF_INET6) == L7_SUCCESS)
    {
      cliShowCmdInt (ewsContext, val, FD_MLD_SNOOPING_QUERIER_EXPIRY_INTERVAL, pStrInfo_base_SetMldQuerierTimerExpiry);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of trapflag Info
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

L7_RC_t cliRunningConfigTrapflagInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if (usmDbComponentPresentCheck(unit, L7_SNMP_COMPONENT_ID) == L7_TRUE)
  {
    if (usmDbTrapAuthSwGet(unit, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val,FD_TRAP_AUTH, pStrInfo_base_SnmpSrvrEnblTraps);
    }

    if (usmDbTrapLinkStatusSwGet(unit, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val,FD_TRAP_LINK_STATUS, pStrInfo_base_SnmpSrvrEnblTrapsLinkmode);
    }

    if (usmDbTrapMultiUsersSwGet(unit, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val,FD_TRAP_MULTI_USERS, pStrInfo_base_SnmpSrvrEnblTrapsMultiusers);
    }

    if (usmDbTrapSpanningTreeSwGet(unit, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val,FD_TRAP_SPAN_TREE, pStrInfo_base_SnmpSrvrEnblTrapsStpmode);
    }

#ifdef L7_WIRELESS_PACKAGE
    if (usmDbTrapWirelessModeGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val, FD_TRAP_WIRELESS, pStrInfo_base_SnmpSrvrEnblTrapsWirelessMode);
    }
#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
    if (usmDbTrapCaptivePortalGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val, FD_TRAP_CAPTIVE_PORTAL, pStrInfo_base_SnmpSrvrEnblTrapsCaptivePortalMode);
    }
#endif

  }
  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of users along with snmpv3 settings Info
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
L7_RC_t cliRunningConfigUsersSnmpv3Info(EwsContext ewsContext, L7_uint32 unit)
{
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH +1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH * 2]={0};
  L7_char8 stat2[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 password[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 encryptKey[L7_ENCRYPTION_KEY_SIZE]; 
  L7_char8 encrypted[L7_ENCRYPTION_KEY_SIZE*2 + 1]; 
  L7_int32 tempInt = 0;
  L7_uint32 snmpUserSupported = L7_FALSE;
  L7_uint32 snmpAuthEncryptSupported = L7_FALSE;
  L7_uint32 accessLevel, auth, encrypt;
  L7_BOOL   enable;
  L7_char8 adminStrEncrypted[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8 adminDefPasswd[L7_PASSWORD_SIZE] = FD_USER_MGR_DEFAULT_PASSWORD;
  L7_char8 guestStrEncrypted[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8 guestDefPasswd[L7_PASSWORD_SIZE] = FD_USER_MGR_DEFAULT_PASSWORD;

  if (usmDbFeaturePresentCheck(unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_SUPPORTED) == L7_TRUE)
  {
    snmpUserSupported = L7_TRUE;
  }
  if ((usmDbFeaturePresentCheck(unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_AUTHENTICATION_FEATURE_ID) ==  L7_TRUE) || (usmDbFeaturePresentCheck(unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID) == L7_TRUE))
  {
    snmpAuthEncryptSupported = L7_TRUE;
  }

  memset(adminStrEncrypted, 0, L7_ENCRYPTED_PASSWORD_SIZE);
  pwEncrypt(adminDefPasswd, adminStrEncrypted, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);
  memset(guestStrEncrypted, 0, L7_ENCRYPTED_PASSWORD_SIZE);
  pwEncrypt(guestDefPasswd, guestStrEncrypted, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);

  tempInt = 0;
  for (; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    bzero(buf, sizeof(buf));
    bzero(password, sizeof(password));
    rc = cliWebLoginUserNameGet(tempInt, buf);
    if (strcmp(buf, FD_CLI_WEB_DEFAULT_LOGIN_NAME) != 0)
    {
      if (cliWebLoginUserPasswordEncryptedGet(tempInt, password) == L7_SUCCESS)
      {
        memset(stat, 0x00, sizeof(stat));    
        memset(stat2, 0x00, sizeof(stat2));    
        if (!(((strcmp(buf,pStrInfo_base_LoginUsrId) == 0) && (userMgrNoCaseCompare(password, adminStrEncrypted) == L7_TRUE)) ||
              ((strcmp(buf,pStrInfo_base_Guest) == 0) && (userMgrNoCaseCompare(password, guestStrEncrypted) == L7_TRUE))))
        {
          if(osapiStrnlen(password, sizeof(password)) != 0)
          {
            osapiSnprintf(stat, sizeof(stat), "\r\nusername \"%s\" password %s", buf, password);
          }    
          else
          {
            osapiSnprintf(stat, sizeof(stat), "\r\nusername \"%s\" nopassword %s", buf, password);
          }

          if (cliWebLoginUserAccessModeGet(tempInt, &accessLevel) == L7_SUCCESS)
          {
            osapiSnprintf(stat2, sizeof(stat2), " level %d", accessLevel);
            osapiStrncat(stat, stat2, sizeof(stat)-strlen(stat));
          }
     
          if(osapiStrnlen(password, sizeof(password)) != 0)
          {
            osapiStrncat(stat, " encrypted", sizeof(stat)-osapiStrnlen(stat,sizeof(stat)));
          }
          if(usmDbUserMgrPasswordStrengthCheckConfigGet(&enable) == L7_SUCCESS)
          {
            if(enable == L7_ENABLE)
            {
              memset(stat2, 0x00, sizeof(stat2));
              osapiSnprintf(stat2, sizeof(stat2), " override-complexity-check");
              osapiStrncat(stat, stat2, sizeof(stat)-strlen(stat));
            }  
          }
          EWSWRITEBUFFER(ewsContext,stat);
        }
      }
      memset(stat, 0x00, sizeof(stat));
      if (snmpUserSupported == L7_TRUE)
      {
        if ((cliWebLoginUserSnmpv3AccessModeGet(tempInt, &accessLevel) == L7_SUCCESS) ||
            (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
        {
          if((strcmp(buf,pStrInfo_base_LoginUsrId) == 0) && (accessLevel != L7_AGENT_COMM_ACCESS_LEVEL_READ_WRITE))
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                    pStrInfo_base_UsrsSnmpv3AccessmodeReadonly, buf);
          else if((strcmp(buf,pStrInfo_base_LoginUsrId) != 0) && (accessLevel != L7_AGENT_COMM_ACCESS_LEVEL_READ_ONLY))
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                    pStrInfo_base_UsrsSnmpv3AccessmodeReadwrite, buf);
          EWSWRITEBUFFER(ewsContext,stat);
        }
      }

      if (snmpAuthEncryptSupported == L7_TRUE)
      {
        if ((cliWebLoginUserAuthenticationGet( tempInt, &auth ) == L7_SUCCESS) &&
            ((auth != FD_SNMP_DEFAULT_USER_AUTH)||(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
        {
          /* authentication type */
          switch (auth)
          {
            case L7_SNMP_USER_AUTH_PROTO_HMACMD5:
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                     pStrInfo_base_UsrsSnmpv3AuthMd5, buf);    /* MD5 */
              break;
            case L7_SNMP_USER_AUTH_PROTO_HMACSHA:
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                     pStrInfo_base_UsrsSnmpv3AuthSha, buf);    /* SHA */
              break;
            case L7_SNMP_USER_AUTH_PROTO_NONE:
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                     pStrInfo_base_UsrsSnmpv3AuthNone, buf);   /* None */
              break;
            default:
              break;
          }
          EWSWRITEBUFFER(ewsContext,stat);
        }

        if ((cliWebLoginUserEncryptionGet( tempInt, &encrypt ) == L7_SUCCESS) &&
            ((encrypt != FD_SNMP_DEFAULT_PRIV_PROTO)||(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
        {
          /* encryption type */
          switch (encrypt)
          {
            case L7_SNMP_USER_PRIV_PROTO_DES:
            memset(encryptKey, 0x00, sizeof(encryptKey));
            memset(encrypted, 0x00, sizeof(encrypted));              
            cliWebLoginUserEncryptionKeyGet(tempInt, encryptKey); 
            if(pwEncrypt( encryptKey, encrypted, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_UsrsSnmpv3EncryptionDes,
                                      buf, encrypted);
              EWSWRITEBUFFER(ewsContext,stat);
            }
            break;
            case L7_SNMP_USER_PRIV_PROTO_NONE:
            sprintf(stat,"\r\nusername snmpv3 encryption \"%s\" none",buf);  /* None */
              EWSWRITEBUFFER(ewsContext,stat);
              break;

            default:
              break;
          }
        }
      }
    } /* end if user name is not blank */
  } /* end of for loop */
  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of telnet Info
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
L7_RC_t cliRunningConfigTelnetInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if (usmDbAgentTelnetNewSessionsGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdTrue(ewsContext,val,FD_CLI_WEB_TELNET_NEW_SESSIONS,pStrInfo_base_TransportInputTelnet);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of telnet Info
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
L7_RC_t cliRunningTelnetInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if (usmDbAgentTelnetTimeoutGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT,pStrInfo_base_TelnetconTimeout);
  }

  if (usmDbAgentTelnetNumSessionsGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_CLI_WEB_DEFAULT_NUM_SESSIONS,pStrInfo_base_TelnetconMaxsessions);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of telnet Info
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
L7_RC_t cliRunningTelnetEnableInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  if (usmDbAgentTelnetAdminModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable (ewsContext, val,FD_CLI_WEB_TELNET_DEFAULT_ADMIN_MODE, pStrInfo_base_IpTelnetSrvrEnbl_1);

  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of monitor session Info
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

L7_RC_t cliRunningConfigMirroringInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_uint32 sessionIndex, sessionCount;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unitNum = 0, slot = 0, port = 0;
  L7_uint32 destIntf;
  L7_INTF_MASK_t srcIntfMask;
  L7_uint32 listSrcPorts[256], numPorts;
  L7_uint32 i;
  L7_MIRROR_DIRECTION_t type;
  L7_uchar8 typeString[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  sessionCount = usmDbSwPortMonitorSessionCount(unit);
  for(sessionIndex = 1; sessionIndex <= sessionCount; sessionIndex++)
  {
    if (usmDbSwPortMonitorDestPortGet(unit, sessionIndex, &destIntf) == L7_SUCCESS)
    {
      if (usmDbUnitSlotPortGet(destIntf, &unitNum, &slot, &port) == L7_SUCCESS)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_MonitorSessionDstIntf,
            sessionIndex, cliDisplayInterfaceHelp(unitNum, slot, port));
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }

    if (usmDbSwPortMonitorSourcePortsGet(unit, sessionIndex, &srcIntfMask) == L7_SUCCESS)
    {
      usmDbConvertMaskToList(&srcIntfMask, listSrcPorts, &numPorts);
      for (i=1; i <= numPorts; i++)
      {
        if (usmDbUnitSlotPortGet(listSrcPorts[i], &unitNum, &slot, &port) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_MonitorSessionSrcIntf, sessionIndex,
              cliDisplayInterfaceHelp(unitNum, slot, port));
          if((cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
                  L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID)) == L7_TRUE)
          {
            rc = usmDbMirrorSourcePortDirectionGet(sessionIndex,listSrcPorts[i], &type);
            if(rc == L7_SUCCESS)
            {
              if(type == L7_MIRROR_INGRESS)
              {
                osapiSnprintf(typeString, sizeof(typeString), " %s", pStrInfo_base_IngressDirection);
              }
              else if(type == L7_MIRROR_EGRESS)
              {
                osapiSnprintf(typeString, sizeof(typeString), " %s", pStrInfo_common_EgressDirection);
              }
              else if(type == L7_MIRROR_BIDIRECTIONAL)
              {
                osapiSnprintf(typeString, sizeof(typeString), " %s", "");
              }
              OSAPI_STRNCAT(stat,typeString);
            }
          }
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }

    /* Factory default for mode is L7_DISABLE */

    if(usmDbSwPortMonitorModeGet(unit, sessionIndex, &val) == L7_SUCCESS && 
       val != FD_MIRRORING_MODE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_base_MonitorSessionMode, sessionIndex);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  To print the running configuration of Cpu Monitor info
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
L7_RC_t cliRunningConfigCpuMonitorInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_uint32 risingThr = 0, risingPeriod = 0, fallingThr = 0, fallingPeriod = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  memset(stat, 0, sizeof(stat));
  if ((usmdbCpuFreeMemoryThresholdGet(unit, &val) == L7_SUCCESS) &&
      (val != 0))
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_base_CpuFreeMemoryThreshold, val);
    EWSWRITEBUFFER(ewsContext, stat);
  }


  memset(stat, 0, sizeof(stat));
  if ((usmdbCpuUtilMonitorParamGet(unit, 
                                   SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM,
                                   &risingPeriod) == L7_SUCCESS) &&
      (risingPeriod != 0))
  {

    (void) usmdbCpuUtilMonitorParamGet(unit, 
                                       SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM,
                                       &risingThr);
    (void) usmdbCpuUtilMonitorParamGet(unit, 
                                       SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM,
                                       &fallingThr);
    (void) usmdbCpuUtilMonitorParamGet(unit, 
                                       SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM,
                                       &fallingPeriod);

    osapiSnprintf(stat, sizeof(stat), pStrInfo_base_ProcessCpuThresholdTypeTotal, 
                  risingThr, risingPeriod, fallingThr, fallingPeriod);
    EWSWRITEBUFFER(ewsContext, stat);
  }

  
  return L7_SUCCESS;
}

#ifdef L7_AUTO_INSTALL_PACKAGE
/*********************************************************************
 * @purpose  To display the running configuration of Auto Install Info
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
L7_RC_t cliRunningConfigAutoInstallInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_BOOL optMode;
  L7_uint32 retryCnt = 0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* boot autoinstall {start |stop} */
  if (usmdbAutoInstallStartStopGet(&optMode) == L7_SUCCESS)
  {
    if (optMode != FD_AUTO_INSTALL_START_STOP_DEFAULT)
    {
      osapiSnprintf(buf, sizeof(buf), "%s %s %s", pStrInfo_base_Boot, pStrInfo_base_AutoInstall,
                    (optMode == L7_TRUE) ? pStrInfo_base_AutoInstallStart : pStrInfo_base_AutoInstallStop);
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, buf);
    }
  }
  /* boot autoinstall auto-save */
  if (usmdbAutoInstallAutoSaveGet(&optMode) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%s %s %s", pStrInfo_base_Boot, pStrInfo_base_AutoInstall,
                    pStrInfo_base_AutoInstallAutoSave);
    cliShowCmdTrue(ewsContext, optMode, FD_AUTO_INSTALL_AUTO_SAVE_DEFAULT, buf);
  }
  /* boot autoinstall retry-count <cnt>*/
  if (usmdbAutoInstallFileDownLoadRetryCountGet(&retryCnt) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%s %s %s", pStrInfo_base_Boot, pStrInfo_base_AutoInstall,
                    pStrInfo_base_AutoInstallRetryCount);
    cliShowCmdInt(ewsContext, retryCnt, FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_DEFAULT, buf);
  }
  return L7_SUCCESS;
}
#endif
/*********************************************************************
 * @purpose  To print the running configuration of network Info
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

L7_RC_t cliRunningConfigDhcpClientInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 optMode;
  L7_uchar8 remoteIdStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* dhcp client vendor-id-option */
  if (usmdbDhcpVendorClassOptionAdminModeGet(&optMode) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, optMode, FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE, 
                     pStrInfo_base_DhcpClientVendorIdShow);
  }
  /* dhcp client vendor-id-option-string <string> */
  if (usmdbDhcpVendorClassOptionStringGet(remoteIdStr) == L7_SUCCESS)
  {
    if (osapiStrncmp(remoteIdStr, FD_DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING, sizeof(remoteIdStr)) != L7_NULL)
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_base_DhcpClientVendorIdStrShow,
                    remoteIdStr); 
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, buf);
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of network Info
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

L7_RC_t cliRunningConfigNetworkInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uint32 val;

  /*
   * System IP , NetMask and Getway Default values set to 0
   * in simBuildDefaultConfigData. It should set to
   * Factory default.
   */
  if (usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(unit,&val) == L7_SUCCESS)
  {
    if (( val != FD_SIM_NETWORK_CONFIG_MODE)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      switch ( val )
      {
        case L7_SYSCONFIG_MODE_NONE:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NwProtoNone_1);
          break;
        case L7_SYSCONFIG_MODE_BOOTP:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NwProtoBootp);
          break;
        case L7_SYSCONFIG_MODE_DHCP:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NwProtoDhcp_1);
          break;
        default:
          break;
      }
    }
  }

  if (L7_SYSCONFIG_MODE_NONE == val)
  {
    if ((usmDbAgentConfiguredIpIfAddressGet(&val) == L7_SUCCESS) &&
        (val != FD_SIM_DEFAULT_NETWORK_IP_ADDR))
    {
      memset (buf, 0, sizeof(buf));
      if (usmDbInetNtoa(val, buf) == L7_SUCCESS)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NwParms_1, buf);
        if (usmDbAgentConfiguredIpIfNetMaskGet(&val) == L7_SUCCESS)
        {
          memset (buf, 0, sizeof(buf));
          if (usmDbInetNtoa(val, buf) == L7_SUCCESS)
          {
            OSAPI_STRNCAT(stat, buf);
            OSAPI_STRNCAT(stat, " ");
          }
        }
        if (usmDbAgentConfiguredIpIfDefaultRouterGet(&val) == L7_SUCCESS)
        {
          memset (buf, 0, sizeof(buf));
          if (usmDbInetNtoa(val, buf) == L7_SUCCESS)
          {
            OSAPI_STRNCAT(stat, buf);
          }
        }
        EWSWRITEBUFFER(ewsContext, stat);
      }

    }
  }

  memset (stat, 0,sizeof(stat));
  val = sizeof(mac);
  /*
   * mac  Default values set to ""
   * in simBuildDefaultConfigData. It should set to
   * Factory default.
   */
  if ((usmDbSwDevCtrlLocalAdminAddrGet(unit, mac) == L7_SUCCESS) &&
      ((strcmp(mac,FD_SIM_DEFAULT_MAC_ADR) != 0 )))
  {
    osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[0],mac[1],mac[2],mac[3],mac[4 ],mac[5]);
    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NwMacAddr_1, buf);
    EWSWRITEBUFFER(ewsContext,stat);
  }

  if (usmDbSwDevCtrlMacAddrTypeGet(unit,&val) == L7_SUCCESS)
  {
    if ((val != FD_SIM_MACTYPE )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      switch ( val )
      {
        case L7_SYSMAC_BIA:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_NwMacTypeBurnedin);
          break;

        case L7_SYSMAC_LAA:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_NwMacTypeLocal);
          break;

        default:
          /* Do nothing */
          break;
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of HTTP Server Info
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

L7_RC_t cliRunningConfigHttpServerInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if(usmDbWebJavaModeGet(0,&val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, val,FD_CLI_WEB_JAVA_MODE, pStrInfo_base_IpHttpJava);
  }

  if(usmDbSwDevCtrlWebMgmtModeGet(0, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, val,FD_CLI_WEB_MODE, pStrInfo_base_IpHttpSrvr);
  }

  if (usmDbCliWebHttpNumSessionsGet(&val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext, val,FD_HTTP_DEFAULT_MAX_CONNECTIONS, pStrInfo_base_IpHttpSessionMaxsessions);
  }

  if (usmDbCliWebHttpSessionHardTimeOutGet(&val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext, val,FD_HTTP_SESSION_HARD_TIMEOUT_DEFAULT, pStrInfo_base_IpHttpSessionHardTimeout);
  }

  if (usmDbCliWebHttpSessionSoftTimeOutGet(&val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext, val,FD_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT, pStrInfo_base_IpHttpSessionSoftTimeout);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of Authentication Info
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
L7_RC_t cliRunningConfigAuthenticationInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 strAuthListName[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8 strAuthListNamePrev[L7_MAX_APL_NAME_SIZE + 1];
  L7_uint32 i = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH +1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_USER_MGR_AUTH_METHOD_t method;
  L7_BOOL flag = L7_FALSE;
  L7_RC_t rc;

  rc = usmDbAPLListGetFirst(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, strAuthListName);
  do
  {
    memset (stat, 0,sizeof(stat));
    memset (buf, 0,sizeof(buf));
    for (i = 0; i <= 2; i++)
    {
      if ((usmDbAPLAuthMethodGet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, strAuthListName, i, &method) == L7_SUCCESS)&&
          (strcmp(strAuthListName, FD_APL_DEFAULT_LIST_NAME) != 0))

      {
        switch (method)
        {
          case L7_AUTH_METHOD_LOCAL:
            OSAPI_STRNCAT(buf, " ");
            OSAPI_STRNCAT(buf, pStrInfo_common_LocalAuth);
            flag = L7_TRUE;
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_AuthLogin_1,strAuthListName, buf);
            break;
          case L7_AUTH_METHOD_RADIUS:
            if (flag == L7_TRUE)
            {
              OSAPI_STRNCAT(buf, " ");
            }
            OSAPI_STRNCAT(buf, pStrInfo_common_RadiusAuth);
            flag = L7_TRUE;
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_AuthLogin_1,strAuthListName, buf);
            break;
          case L7_AUTH_METHOD_TACACS:
            if (flag == L7_TRUE)
            {
              OSAPI_STRNCAT(buf, " ");
            }
            OSAPI_STRNCAT(buf, pStrInfo_base_TacacsAuth);
            flag = L7_TRUE;
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_AuthLogin_1,strAuthListName, buf);
            break;
          case L7_AUTH_METHOD_REJECT:
            if (flag == L7_TRUE)
            {
              OSAPI_STRNCAT(buf, " ");
            }
            OSAPI_STRNCAT(buf, pStrInfo_base_RejectAuth);
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_AuthLogin_1,strAuthListName, buf);
            break;
          default:
            break;
        }
      }
    }
    EWSWRITEBUFFER(ewsContext, stat);
    OSAPI_STRNCPY_SAFE(strAuthListNamePrev, strAuthListName);
    memset (strAuthListName, 0, sizeof(strAuthListName));
  } while (usmDbAPLListGetNext(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, strAuthListNamePrev, strAuthListName) == L7_SUCCESS);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of Authentication Info
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliRunningConfigAuthentication(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8  strAuthListName[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8  strAuthMethodList[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i = 0;
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_USER_MGR_AUTH_METHOD_t method;
  L7_ACCESS_LEVEL_t authMode;
  L7_ACCESS_LINE_t accessLine;
  L7_RC_t usmDbReturnCode;

  for (accessLine = ACCESS_LINE_CTS; accessLine >= ACCESS_LINE_HTTPS; accessLine--)
  {
    if (accessLine == ACCESS_LINE_DOT1X)
    {
      continue;
    }

    for (authMode = ACCESS_LEVEL_LOGIN; authMode <=ACCESS_LEVEL_ENABLE; authMode++)
    {
      if ((usmDbReturnCode = usmDbAPLListGetFirst(accessLine,
                                                             authMode,
                                                             strAuthListName))==
          L7_SUCCESS)
      {
        if (authMode == ACCESS_LEVEL_ENABLE && accessLine != ACCESS_LINE_CTS)
        {
          continue;
        }
        do
        {
          /* Initialising the stat buffer to zero */
          bzero(stat,sizeof(stat));
          switch (accessLine)
          {
            case ACCESS_LINE_CTS:
              if (authMode == ACCESS_LEVEL_LOGIN)
                osapiSnprintf(stat, sizeof(stat), "\r\naaa authentication login \"%s\"",strAuthListName);
              else
                osapiSnprintf(stat, sizeof(stat), "\r\naaa authentication enable \"%s\"",strAuthListName);
              break;
            case ACCESS_LINE_HTTPS:
              osapiSnprintf(stat, sizeof(stat), "\r\nip https authentication");
              break;
            case ACCESS_LINE_HTTP:
              osapiSnprintf(stat, sizeof(stat), "\r\nip http authentication");
              break;
            default:
              break;

          }

          bzero(strAuthMethodList,sizeof(strAuthMethodList));
          /* for loop for every methods possible */
          for (i=0; i<L7_MAX_AUTH_METHODS ; i++)
          {
            /* compile the list of methods
             */
            if ((usmDbReturnCode= usmDbAPLAuthMethodGet(accessLine,
                                                                       authMode,
                                                                       strAuthListName,
                                                                       i,
                                                                       &method))==
                L7_SUCCESS)
            {
              switch (method)
              {
                case L7_AUTH_METHOD_LOCAL:
                  osapiStrncat(strAuthMethodList, " ", sizeof(strAuthMethodList));
                  osapiStrncat(strAuthMethodList, pStrInfo_base_LocalAuth, sizeof(strAuthMethodList));
                  break;
                case L7_AUTH_METHOD_RADIUS:
                  osapiStrncat(strAuthMethodList, " ", sizeof(strAuthMethodList));
                  osapiStrncat(strAuthMethodList, pStrInfo_base_RadiusAuth, sizeof(strAuthMethodList));
                  break;
                case L7_AUTH_METHOD_ENABLE:
                  osapiStrncat(strAuthMethodList, " ", sizeof(strAuthMethodList));
                  osapiStrncat(strAuthMethodList, pStrInfo_base_EnableAuth, sizeof(strAuthMethodList));
                  break;
                case L7_AUTH_METHOD_TACACS:
                  osapiStrncat(strAuthMethodList, " ", sizeof(strAuthMethodList));
                  osapiStrncat(strAuthMethodList, pStrInfo_base_TacacsAuth, sizeof(strAuthMethodList));
                  break;
                case L7_AUTH_METHOD_NONE:
                  osapiStrncat(strAuthMethodList, " ", sizeof(strAuthMethodList));
                  osapiStrncat(strAuthMethodList, pStrInfo_base_NoneAuth, sizeof(strAuthMethodList));
                  break;
                case L7_AUTH_METHOD_LINE:
                  osapiStrncat(strAuthMethodList, " ", sizeof(strAuthMethodList));
                  osapiStrncat(strAuthMethodList, pStrInfo_base_LineAuth, sizeof(strAuthMethodList));
                  break;
                default:
                  break;
              }
            } /*End of if ((usmDbReturnCode = usmDbAPLAuthMethodGet(unit,*/
          } /* End of for (i=0; */
          osapiStrncat(stat, strAuthMethodList, sizeof(stat));
          if ((osapiStrncmp(stat, "\r\nip http authentication local", strlen(stat)) == L7_SUCCESS) ||
              (osapiStrncmp(stat, "\r\nip https authentication local", strlen(stat)) == L7_SUCCESS) ||
              (osapiStrncmp(stat, "\r\naaa authentication login \"default\" none", strlen(stat)) == L7_SUCCESS) ||
              (osapiStrncmp(stat, "\r\naaa authentication login \"defaultList\" local", strlen(stat)) == L7_SUCCESS) ||
              (osapiStrncmp(stat, "\r\naaa authentication login \"networkList\" local", strlen(stat)) == L7_SUCCESS) ||
              (osapiStrncmp(stat, "\r\naaa authentication enable \"default\" none", strlen(stat)) == L7_SUCCESS) ||
              (osapiStrncmp(stat, "\r\naaa authentication enable \"enableList\" none", strlen(stat)) == L7_SUCCESS))

          {
            continue;
          }
          else
          {
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
        while (usmDbAPLListGetNext(accessLine,
                                              authMode,
                                              strAuthListName,
                                              strAuthListName)==
               L7_SUCCESS);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of console/telnet/ssh lines
*
* @param    EwsContext ewsContext
* @param    L7_ACCESS_LINE_t accessLine
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliRunningConfigLineInfo(EwsContext       ewsContext,
                                 L7_ACCESS_LINE_t accessLine)
{
  L7_char8  strAuthListName[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  strPassword[L7_CLI_MAX_STRING_LENGTH];
  L7_ACCESS_LEVEL_t authMode;

  /* login/enable authentication */
  for (authMode=ACCESS_LEVEL_LOGIN; authMode<=ACCESS_LEVEL_ENABLE; authMode++)
  {
    if (usmDbAPLLineGet(accessLine, authMode,strAuthListName) == L7_SUCCESS)
    {
      if (authMode == ACCESS_LEVEL_ENABLE && strcmp(strAuthListName, L7_ENABLE_APL_NAME)!=0)
      {
        osapiSnprintf(stat, sizeof(stat), "\r\nenable authentication %s", strAuthListName);
        EWSWRITEBUFFER (ewsContext, stat);
      }
      if ((authMode == ACCESS_LEVEL_LOGIN &&
           strcmp(strAuthListName, L7_DEFAULT_APL_NAME)!=0 &&
           accessLine == ACCESS_LINE_CONSOLE) ||
          (authMode == ACCESS_LEVEL_LOGIN &&
           strcmp(strAuthListName, L7_DEFAULT_NETWORK_APL_NAME)!=0 &&
           accessLine != ACCESS_LINE_CONSOLE))
      {
        osapiSnprintf(stat, sizeof(stat), "\r\nlogin authentication %s", strAuthListName);
        EWSWRITEBUFFER (ewsContext, stat);
      }
    }
  }

  /* password */
  bzero(strPassword, sizeof(strPassword));
  if (usmDbLinePasswordGet(accessLine, strPassword) == L7_SUCCESS)
  {
    if (strlen(strPassword) != 0 || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      osapiSnprintf(stat, sizeof(stat), "\r\npassword %s encrypted", strPassword);
      EWSWRITEBUFFER (ewsContext, stat);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of enable command
*
* @param    EwsContext ewsContext
* @param    L7_ACCESS_LINE_t accessLine
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliRunningConfigEnable(EwsContext ewsContext)
{
  L7_char8  strPassword[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  /* login/enable authentication */

  /* password */
  bzero(strPassword, sizeof(strPassword));
  if (usmDbEnablePasswordGet(FD_USER_MGR_ADMIN_ACCESS_LEVEL, strPassword) == L7_SUCCESS)
  {
    if (strlen(strPassword) != 0 || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      osapiSnprintf(stat, sizeof(stat), "\r\nenable password %s encrypted", strPassword);
      EWSWRITEBUFFER (ewsContext, stat);
    }
  }
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  To print the running configuration of Users Login Info
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
L7_RC_t cliRunningConfigUsersLoginInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 strAuthListName[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8 strUserName[L7_LOGIN_SIZE];
  L7_uint32 userCount;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  for (userCount = 0; userCount < L7_MAX_LOGINS; userCount++ )
  {
    memset (strUserName, 0, sizeof(strUserName));
    rc = cliWebLoginUserNameGet(userCount, strUserName);

    /* skip user if empty.  The name list will contain blanks if a user is deleted.  */
    if (strcmp(strUserName, FD_CLI_WEB_DEFAULT_LOGIN_NAME) == 0)
    {
      continue;
    }
    memset (strAuthListName, 0, sizeof(strAuthListName));
    if ((usmDbAPLUserGet(unit, strUserName, L7_USER_MGR_COMPONENT_ID, strAuthListName) == L7_SUCCESS) &&
        (strcmp(strUserName, pStrInfo_base_LoginUsrId) != 0) &&
        (strcmp(strUserName, pStrInfo_base_Guest) != 0) &&
        (strcmp(strUserName, pStrInfo_common_PassDefl) != 0) &&
        (strcmp(strAuthListName, FD_APL_DEFAULT_LIST_NAME) != 0))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_UsrsLogin, strUserName, strAuthListName);
      EWSWRITEBUFFER(ewsContext,stat);
    }
  }

  /* Get the settings for the default user */
  memset (stat, 0,sizeof(stat));
  if (( usmDbAPLNonConfiguredUserGet(unit, L7_USER_MGR_COMPONENT_ID, strAuthListName) == L7_SUCCESS) && ((strcmp(strAuthListName, FD_APL_DEFAULT_LIST_NAME) != 0)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_UsrsDefllogin, strAuthListName);
    EWSWRITEBUFFER(ewsContext,stat);
  }

  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of serial settings Info
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

L7_RC_t cliRunningConfigSerialInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  memset (stat, 0, sizeof(stat));
  if (usmDbSerialTimeOutGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val,FD_SIM_DEFAULT_SERIAL_PORT_TIMEOUT,pStrInfo_base_SerialTimeout);
  }

  if (usmDbAgentBasicConfigSerialBaudRateGet(unit, &val) == L7_SUCCESS )
  {
    if (( val != FD_SIM_DEFAULT_SYSTEM_BAUD_RATE )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      switch ( val )
      {
        case L7_BAUDRATE_1200:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialBaudrate1200);
          break;
        case L7_BAUDRATE_2400:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialBaudrate2400);
          break;
        case L7_BAUDRATE_4800:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialBaudrate4800);
          break;
        case L7_BAUDRATE_9600:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialBaudrate9600);
          break;
        case L7_BAUDRATE_19200:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialBaudrate19200);
          break;
        case L7_BAUDRATE_38400:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialBaudrate38400);
          break;
        case L7_BAUDRATE_57600:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialBaudrate57600);
          break;
        case L7_BAUDRATE_115200:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialBaudrate115200);
          break;
        default:
          break;
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of service port Info
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

L7_RC_t cliRunningConfigServicePortInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  memset (stat, 0, sizeof(stat));
  memset (buf, 0, sizeof(buf));

  if (usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(unit,&val) == L7_SUCCESS )
  {
    if (( val != FD_SIM_SERVPORT_CONFIG_MODE)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      switch ( val )
      {
        case L7_SYSCONFIG_MODE_BOOTP:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_ServiceportProtoBootp);
          break;

        case L7_SYSCONFIG_MODE_DHCP:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_ServiceportProtoDhcp);
          break;

        case L7_SYSCONFIG_MODE_NONE:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_ServiceportProtoNone);
          break;

        default:
          break;
      }
    }
  }

  if (L7_SYSCONFIG_MODE_NONE == val)
  {
    if ((usmDbServicePortConfiguredIpAddrGet(&val) == L7_SUCCESS )&&
        ((val != FD_SIM_DEFAULT_SERVPORT_IP_ADDR)||(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      if (usmDbInetNtoa(val, buf) == L7_SUCCESS )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_ServiceportIp,buf);

        memset (buf, 0, sizeof(buf));
        if (usmDbServicePortConfiguredNetMaskGet(&val) == L7_SUCCESS )
        {
          if (usmDbInetNtoa(val, buf) == L7_SUCCESS )
          {
            OSAPI_STRNCAT(stat, buf);
            OSAPI_STRNCAT(stat, " ");
          }
        }

        memset (buf, 0, sizeof(buf));
        if (usmDbServicePortConfiguredGatewayGet(&val) == L7_SUCCESS )
        {
          if (usmDbInetNtoa(val, buf) == L7_SUCCESS )
          {
            OSAPI_STRNCAT(stat, buf);
          }
        }
        EWSWRITEBUFFER(ewsContext, stat);
      }

    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of radius Info
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
L7_RC_t cliRunningConfigRadiusInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH+L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8 buf2[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 hostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 strSecret[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strEncrypted[ L7_ENCRYPTED_PASSWORD_SIZE ];
  L7_BOOL boolVal;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_IP_ADDRESS_TYPE_t authTypeList[L7_RADIUS_MAX_AUTH_SERVERS] = {L7_IP_ADDRESS_TYPE_UNKNOWN};
  L7_char8 authHostList[L7_RADIUS_MAX_AUTH_SERVERS][L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t acctTypeList[L7_RADIUS_MAX_ACCT_SERVERS] = {L7_IP_ADDRESS_TYPE_UNKNOWN};
  L7_char8 acctHostList[L7_RADIUS_MAX_ACCT_SERVERS][L7_DNS_HOST_NAME_LEN_MAX];

  L7_uint32 count;
  L7_int32 i;
  L7_RC_t rc = L7_FAILURE;

  memset(authHostList, 0x00, sizeof(L7_DNS_HOST_NAME_LEN_MAX) * L7_RADIUS_MAX_AUTH_SERVERS);
  memset(acctHostList, 0x00, sizeof(L7_DNS_HOST_NAME_LEN_MAX) * L7_RADIUS_MAX_ACCT_SERVERS);


  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];

  if ( usmDbRadiusAccountingModeGet(unit, &val ) == L7_SUCCESS )
  {
    cliShowCmdEnable (ewsContext, val, FD_RADIUS_ACCT_ADMIN_MODE, pStrInfo_base_RadiusAcctMode);

  }

  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);

  memset(buf, 0, sizeof(buf));
  /* Max Number of Retransmits */
  if (usmDbRadiusServerRetransGet(strIpAddr, &val, &paramStatus) == L7_SUCCESS)
  {
    cliShowCmdInt (ewsContext, val, FD_RADIUS_MAX_RETRANS, pStrInfo_base_RadiusSrvrReTx_1);
  }

  memset(buf, 0, sizeof(buf));
  /* Timeout Duration */
  if ( usmDbRadiusServerTimeOutGet(strIpAddr, &val, &paramStatus) == L7_SUCCESS )
  {
    cliShowCmdInt (ewsContext, val, FD_RADIUS_TIMEOUT_DURATION, pStrInfo_base_RadiusSrvrTimeout_1);
  }

  if( (rc = usmDbRadiusServerFirstIPHostNameGet(unit, hostAddr, &type)) == L7_SUCCESS )
  {
    count = 0;
    do
    {
      authTypeList[count] = type;
      osapiStrncpy(authHostList[count], hostAddr, sizeof(hostAddr));
      count++;
    } while (usmDbRadiusServerNextIPHostNameGet(unit, hostAddr,
                     hostAddr, &type) == L7_SUCCESS );
  }

  if ( rc == L7_SUCCESS )
  {
    for(i = count-1; i >= 0; i--)
    {
      memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
      type = authTypeList[i];
      osapiStrncpy(hostAddr, authHostList[i], sizeof(authHostList[i]));
      if( usmDbRadiusServerHostNameGet(unit, hostAddr, type, name) == L7_SUCCESS )
      {
        /* IP Address+ Server Name + Def. Port */
        if (usmDbRadiusHostNameServerPortNumGet(unit, hostAddr, 
                     type, &val) == L7_SUCCESS)
        {
          if ((val == FD_RADIUS_AUTH_PORT)|| EWSSHOWALL(ewsContext) 
                     == L7_SHOW_DEFAULT)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), 
                     pStrInfo_base_RadiusSrvrHostAuth_3, hostAddr, name);
            EWSWRITEBUFFER(ewsContext, buf);
          }
        }

        /* IP Address + Server Name + User configured Port */
        if (usmDbRadiusHostNameServerPortNumGet(unit, hostAddr, 
                     type, &val) == L7_SUCCESS)
        {
          if ((val != FD_RADIUS_AUTH_PORT)|| EWSSHOWALL(ewsContext) 
                     == L7_SHOW_DEFAULT)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), 
                     pStrInfo_base_RadiusSrvrHostAuth_4, hostAddr, name, val);
            EWSWRITEBUFFER(ewsContext,buf);
          }
        }
      }

      /* Secret Configured */
      if (( usmDbRadiusHostNameServerSharedSecretConfigured( hostAddr, 
                                                            &boolVal ) == L7_SUCCESS ) &&
        ((boolVal != FD_RADIUS_SERVER_DEFAULT_SECRET_CONFIG)|| 
                     (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
      {
        memset(strEncrypted, 0, sizeof(strEncrypted));
        if (boolVal == L7_TRUE && 
                     usmDbRadiusHostNameServerSharedSecretGet(hostAddr, strSecret, &paramStatus) == L7_SUCCESS && 
                     pwEncrypt(strSecret, strEncrypted, 
                     L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), 
                     pStrInfo_base_RadiusSrvrKeyAuth_1, hostAddr, strEncrypted);
          EWSWRITEBUFFER( ewsContext, buf);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), 
                     pStrInfo_base_NoRadiusSrvrKeyAuth, hostAddr);
          EWSWRITEBUFFER( ewsContext, buf);
        }
      }

      /* Type */
      if (usmDbRadiusHostNameServerEntryTypeGet(unit, hostAddr, 
                     type, &val) == L7_SUCCESS )
      {
        cliShowCmdStr(ewsContext,val,FD_RADIUS_SERVER_ENTRY_TYPE,
                     pStrInfo_base_RadiusSrvrPrimary, hostAddr);
      }

      /* Message Authenticator */
      if ( usmDbRadiusHostNameServerIncMsgAuthModeGet( unit, 
                     hostAddr, type, &val ) == L7_SUCCESS )
      {
        cliShowCmdStr(ewsContext,val,FD_RADIUS_INC_MSG_AUTH_MODE,
                     pStrInfo_base_RadiusSrvrMsgauth, hostAddr);
      }
    }
  } /* radius auth servers */

  memset(buf2, 0, sizeof(buf2));
  if ( usmDbRadiusAttribute4Get(unit,&boolVal,&val ) == L7_SUCCESS )
  {
    if ((usmDbInetNtoa(val,buf2) == L7_SUCCESS) && (val != L7_NULL))
    {
      if (( boolVal == L7_TRUE) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR,  buf, 
                          pStrInfo_base_RadiusSrvrAttr4_1, buf2 );
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
    else
    {
      if (( boolVal == L7_TRUE) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        sprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR,  buf, 
                          pStrInfo_base_RadiusSrvrAttr4);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
  }

  if( (rc = usmDbRadiusAcctServerFirstIPHostNameGet(unit, hostAddr, &type)) == L7_SUCCESS )
  {
    count = 0;
    do
    {
      acctTypeList[count] = type;
      osapiStrncpy(acctHostList[count], hostAddr, sizeof(hostAddr));
      count++;
    } while (usmDbRadiusAcctServerNextIPHostNameGet(unit, hostAddr,
                     hostAddr, &type) == L7_SUCCESS );
  }

  if ( rc == L7_SUCCESS)
  {
    for(i = count-1; i >= 0; i--)
    {
      memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
      type = acctTypeList[i];
      osapiStrncpy(hostAddr, acctHostList[i], L7_DNS_HOST_NAME_LEN_MAX);
      if(usmDbRadiusServerAcctHostNameGet(unit, 
                     hostAddr,type,name) == L7_SUCCESS )
      {
        /* IP Address+ Server Name + Def. Port */    
        if (usmDbRadiusAccountingHostNameServerPortNumGet(unit, hostAddr, 
                         type, &val) == L7_SUCCESS)
        {
          if ((val == FD_RADIUS_ACCT_PORT)|| 
                         EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
          {
              osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), 
                         pStrInfo_base_RadiusSrvrHostAcct_2, hostAddr,name);
              EWSWRITEBUFFER(ewsContext, buf);
           }
        }
        
        /* IP Address + Server Name + User configured Port */
        if (usmDbRadiusAccountingHostNameServerPortNumGet(unit, hostAddr, 
                         type, &val) == L7_SUCCESS)
        {
          if ((val != FD_RADIUS_ACCT_PORT)|| 
                         EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), 
                         pStrInfo_base_RadiusSrvrHostAcct_3, hostAddr, name, val);
            EWSWRITEBUFFER(ewsContext,buf);
          }
        }
      }

      /* Secret Configured */
      if (( usmDbRadiusAccountingHostNameServerSharedSecretConfigured( unit, 
                     hostAddr, type, &boolVal ) == L7_SUCCESS ) &&
          ( (boolVal != FD_RADIUS_SERVER_DEFAULT_SECRET_CONFIG)|| 
            (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
          )
      {
        memset(strEncrypted, 0, sizeof(strEncrypted));
        if (boolVal == L7_TRUE &&
                     usmDbRadiusAccountingHostNameServerSharedSecretGet( unit, 
                     hostAddr, type, strSecret ) == L7_SUCCESS &&
                     pwEncrypt(strSecret, strEncrypted, 
                     L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG) == L7_SUCCESS
             )
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), 
                     pStrInfo_base_RadiusSrvrKeyAcct_1, hostAddr, strEncrypted);
          EWSWRITEBUFFER( ewsContext, buf);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), 
                     pStrInfo_base_NoRadiusSrvrKeyAcct, hostAddr);
          EWSWRITEBUFFER( ewsContext, buf);
        }
      }    
    }
  } /* Radius Accounting Servers */
  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of dot1x info
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
L7_RC_t cliRunningConfigDot1xInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL mode;
  L7_USER_MGR_AUTH_METHOD_t method;
  L7_BOOL bFoundMethod = L7_FALSE;
  L7_uint32 i;

  /* Administrative Mode */
  memset ( buf, 0, sizeof(buf) );
  if (usmDbDot1xAdminModeGet(unit, &mode) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, mode,FD_DOT1X_ADMIN_MODE,  pStrInfo_base_Dot1xSysAuthCntrl);
  }
    
  if(usmDbDot1xMonitorModeGet(unit, &mode) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, mode,FD_DOT1X_MONITOR_MODE,  pStrInfo_base_Dot1xSysAuthCntrlMonitor);
  }

  memset(buf,0,sizeof(buf));
  for (i = 0; i < 3; i++)
  {
    method=L7_AUTH_METHOD_UNDEFINED;
    if (usmDbAPLAuthMethodGet(ACCESS_LINE_DOT1X, 0, L7_NULL, i, &method) == L7_SUCCESS)
    {
      if (i==0) /* first time */
      {
        osapiStrncpySafe(buf, "\r\naaa authentication dot1x default ", sizeof(buf));
      }
      if (method == L7_AUTH_METHOD_NONE)
      {
        osapiStrncat(buf, "none ", sizeof(buf)-strlen(buf));
        bFoundMethod = L7_TRUE;

      }
      if (method == L7_AUTH_METHOD_LOCAL)
      {
        osapiStrncat(buf, "local ", sizeof(buf)-strlen(buf));
        bFoundMethod = L7_TRUE;

      }
      else if (method == L7_AUTH_METHOD_RADIUS)
      {
        osapiStrncat(buf, "radius ", sizeof(buf)-strlen(buf));
        bFoundMethod = L7_TRUE;
      }
      else if (method == L7_AUTH_METHOD_IAS)
      {
        osapiStrncat(buf, "ias ", sizeof(buf)-strlen(buf));
        bFoundMethod = L7_TRUE;
      }
    }
  }

  if (bFoundMethod == L7_TRUE)
  {
    EWSWRITEBUFFER(ewsContext, buf);
  }

  /* Get the Vlan assignment configuration */
  if(usmDbDot1xVlanAssignmentModeGet(unit,&mode) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, mode,FD_DOT1X_VLAN_ASSIGN_MODE,pStrInfo_base_AuthorizationNwRadius);
  }

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1X_COMPONENT_ID, L7_DOT1X_DYNAMIC_VLANASSIGN_FEATURE_ID) == L7_TRUE)
  {

   /* Get the Dynamic Vlan Creation configuration Mode*/
   if(usmDbDot1xDynamicVlanCreationModeGet(unit,&mode) == L7_SUCCESS)
   {
     cliShowCmdEnable(ewsContext, mode, FD_DOT1X_DYNAMIC_VLAN_CREATION_MODE, pStrInfo_base_Dot1xDynamicVlanEnable);
   }
  }

  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of Storm Control Features
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

L7_RC_t cliRunningStormControlInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val,threshold;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RATE_UNIT_t rate_unit;

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbSwDevCtrlBcastStormModeGet(unit, &val) == L7_SUCCESS &&
        usmDbSwDevCtrlBcastStormThresholdGet(unit, &threshold, L7_NULLPTR /* PTin added: stormcontrol */, &rate_unit) == L7_SUCCESS )
    {
      if (val != FD_POLICY_DEFAULT_BCAST_STORM_MODE) /* If not the default value. */
      {
        if (threshold != FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD ||
             rate_unit != FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT)
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastLvl_2,threshold);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS)
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastRate_2,threshold);
          }
          EWSWRITEBUFFER(ewsContext,stat);
          if (val == L7_DISABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlBcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
        else if (val == L7_ENABLE)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcast_2);
          EWSWRITEBUFFER(ewsContext,stat);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlBcast);
          EWSWRITEBUFFER(ewsContext,stat);
        }
      }
      else if (threshold != FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD ||
                rate_unit != FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT)
      {
        if (rate_unit == L7_RATE_UNIT_PERCENT)
        {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastLvl_2,threshold);
        }
        else if (rate_unit == L7_RATE_UNIT_PPS) 
        {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastRate_2,threshold);
        }
        EWSWRITEBUFFER(ewsContext,stat);
        if (val == L7_DISABLE)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlBcast);
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
      else if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        if (val == L7_DISABLE)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastLvl_2,FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastRate_2,FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
          }
          EWSWRITEBUFFER(ewsContext, stat);
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlBcast);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        else
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastLvl_2,FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastRate_2,FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
          }
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbSwDevCtrlMcastStormModeGet(unit, &val) == L7_SUCCESS &&
        usmDbSwDevCtrlMcastStormThresholdGet(unit, &threshold, L7_NULLPTR /* PTin added: stormcontrol */, &rate_unit) == L7_SUCCESS )
    {
      if (val != FD_POLICY_DEFAULT_MCAST_STORM_MODE)  /* If not the default value. */
      {
        if (threshold != FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD ||
             rate_unit != FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastLvl_2,threshold);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastRate_2,threshold);
          }
          EWSWRITEBUFFER(ewsContext,stat);
          if (val == L7_DISABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlMcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
        else if (val == L7_ENABLE)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcast_2);
          EWSWRITEBUFFER(ewsContext,stat);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlMcast);
          EWSWRITEBUFFER(ewsContext,stat);
        }
      }
      else if (threshold != FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD ||
                rate_unit != FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT)
      {
        if (rate_unit == L7_RATE_UNIT_PERCENT)
        {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastLvl_2,threshold);
        }
        else if (rate_unit == L7_RATE_UNIT_PPS) 
        {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastRate_2,threshold);
        }
        EWSWRITEBUFFER(ewsContext,stat);
        if (val == L7_DISABLE)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlMcast);
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
      else if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        if (val == L7_DISABLE)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastLvl_2,FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastRate_2,FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
          }
          EWSWRITEBUFFER(ewsContext, stat);
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlMcast);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        else
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastLvl_2,FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastRate_2,FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
          }
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbSwDevCtrlUcastStormModeGet(unit, &val) == L7_SUCCESS &&
        usmDbSwDevCtrlUcastStormThresholdGet(unit, &threshold, L7_NULLPTR /* PTin added: stormcontrol */, &rate_unit) == L7_SUCCESS )
    {
      if (val != FD_POLICY_DEFAULT_UCAST_STORM_MODE)  /* If not the default value. */
      {
        if (threshold != FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD ||
             rate_unit != FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastLvl_2,threshold);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastRate_2,threshold);
          }
          EWSWRITEBUFFER(ewsContext,stat);
          if (val == L7_DISABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlUcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
        else if (val == L7_ENABLE)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcast_2);
          EWSWRITEBUFFER(ewsContext,stat);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlUcast);
          EWSWRITEBUFFER(ewsContext,stat);
        }
      }
      else if (threshold != FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD ||
                rate_unit != FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT)
      {
        if (rate_unit == L7_RATE_UNIT_PERCENT) 
        {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastLvl_2,threshold);
        }
        else if (rate_unit == L7_RATE_UNIT_PPS) 
        {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastRate_2,threshold);
        }
        EWSWRITEBUFFER(ewsContext,stat);
        if (val == L7_DISABLE)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlUcast);
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
      else if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        if (val == L7_DISABLE)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastLvl_2,FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastRate_2,FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
          }
          EWSWRITEBUFFER(ewsContext, stat);
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlUcast);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        else
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastLvl_2,FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastRate_2,FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
          }
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_FALSE)
  {
    if (usmDbSwDevCtrlFlowControlModeGet(unit, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val,FD_POLICY_DEFAULT_FLOW_CONTROL_MODE,pStrInfo_base_StormCntrlFlowcontrol);
    }

  }
  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of Physical Interface
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 * @param    L7_uint32 interface
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/

L7_RC_t cliRunningPhysicalInterfaceInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_uint32 portFactoryDefaultSpeed;
  L7_BOOL boolVal;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 suppUsrName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat1[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buffer[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 pvid;
  L7_uint32 val,val1, nextvid;
  L7_uint32 mstid;
  L7_uint32 groupID, nextGroupID;
  L7_BOOL first = L7_TRUE;
  L7_BOOL val_lacp, timeoutBit, timeoutBitCurrent;
  L7_RC_t rc,rc2;
  L7_int32 retVal;
  L7_int32 u=1;
  L7_int32 s, p;
  L7_int32 threshold;
  L7_VLAN_MASK_t vlanStaticMcastRtr;
  L7_uint32 sysIntfType;
  L7_uchar8 lacpState;
  L7_uchar8 family = L7_AF_INET;
  static L7_uint32 autoVids[4500], autoCount = 0;
  static L7_uint32 incldueVids[4500], includeCount = 0;
  static L7_uint32 excludeVids[4500], excludeCount = 0;
  static L7_uint32 taggedVids[4500], tagCount = 0;
  static L7_uint32 notaggedVids[4500], notagCount = 0;
  L7_RATE_UNIT_t rate_unit;
  L7_uchar8 paeCap;
  L7_uint32 tmpVal;
  L7_int32 hashMode;

  L7_ushort16 defPrio;

  autoCount = 0;
  includeCount = 0;
  excludeCount = 0;
  tagCount = 0;
  notagCount = 0;
#ifdef L7_ROUTING_PACKAGE
  L7_int32 bandwidth;
#endif

#if defined L7_DOT1AD_PACKAGE
  DOT1AD_INTFERFACE_TYPE_t  intfType;
#endif

  groupID = 0;
  nextGroupID = 0;
  first = L7_TRUE;

  memset(stat, 0, sizeof(stat));
  memset(buffer, 0, sizeof(buffer));
  rc2 = usmDbIntfTypeGet(interface, &sysIntfType);

  /*Commands supported only for physical interfaces*/

  if (sysIntfType == L7_PHYSICAL_INTF)
  {
    if (usmDbDot3adIsMember(unit, interface) != L7_SUCCESS)
    {
      if ((usmDbIfAutoNegAdminStatusGet(unit, interface, &val) == L7_SUCCESS) &&
          (usmDbIntfParmCanSet(interface, L7_INTF_PARM_AUTONEG) == L7_TRUE))
      {
        cliShowCmdEnable (ewsContext, val, FD_NIM_AUTONEG_ENABLE, pStrInfo_base_AutoNegotiate);
      }
    }

    if (usmDbIfTypeGet(unit, interface, &val) == L7_SUCCESS)
    {
      switch (val)
      {
        case L7_IANA_FAST_ETHERNET:
          portFactoryDefaultSpeed = FD_NIM_FAST_ENET_SPEED;
          break;

        case L7_IANA_FAST_ETHERNET_FX:
          portFactoryDefaultSpeed = FD_NIM_FAST_ENET_FX_SPEED;
          break;

        case L7_IANA_GIGABIT_ETHERNET:
          portFactoryDefaultSpeed = FD_NIM_GIG_ENET_SPEED;
          break;

        /* PTin added: Speed 2.5G */
        case L7_IANA_2G5_ETHERNET:
          portFactoryDefaultSpeed = FD_NIM_2G5_ENET_SPEED;
          break;
        /* PTin end */

        case L7_IANA_10G_ETHERNET:
          portFactoryDefaultSpeed = FD_NIM_10G_ENET_SPEED;
          break;

        /* PTin added: Speed 40G */
        case L7_IANA_40G_ETHERNET:
          portFactoryDefaultSpeed = FD_NIM_40G_ENET_SPEED;
          break;

        /* PTin added: Speed 100G */
        case L7_IANA_100G_ETHERNET:
          portFactoryDefaultSpeed = FD_NIM_100G_ENET_SPEED;
          break;
        /* PTin end */

        default:
          portFactoryDefaultSpeed = FD_NIM_OTHER_PORT_TYPE_SPEED;
          break;
      }

      if (usmDbIfAutoNegAdminStatusGet(unit, interface, &val1) == L7_SUCCESS)
      {
        if (val1 != FD_NIM_AUTONEG_ENABLE) /* If not the default value. */
        {
          /* print out the correct speed */
          if ((usmDbIfSpeedGet(unit, interface, &val) == L7_SUCCESS) &&
              ((val != portFactoryDefaultSpeed)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
          {
            switch ( val )
            {
              /* PTin added: Speed 100G */
              case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed100gFullDuplex);
                break;
              /* PTin added: Speed 40G  */
              case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed40gFullDuplex);
                break;
              /* PTin end */
              case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed10gFullDuplex);
                break;
              /* PTin added: Speed 2.5G *** Note: 1000SX was also added! */
              case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed2500FullDuplex);
                break;
              case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed1000FullDuplex);
                break;
              /* PTin end */
              case L7_PORTCTRL_PORTSPEED_HALF_100TX:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed100HalfDuplex);
                break;
              case L7_PORTCTRL_PORTSPEED_FULL_100TX:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed100FullDuplex);
                break;
              case L7_PORTCTRL_PORTSPEED_HALF_10T:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed10HalfDuplex);
                break;
              case L7_PORTCTRL_PORTSPEED_FULL_10T:
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Speed10FullDuplex);
                break;
              default:
                /* Do nothing */
                break;
            }
            EWSWRITEBUFFER(ewsContext,stat);
          }
        }
      }
    }

    /* End of if portType */

    /* Storm Control configuration */

    if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbSwDevCtrlBcastStormModeIntfGet(interface, &val) == L7_SUCCESS &&
          usmDbSwDevCtrlBcastStormThresholdIntfGet(interface, &threshold, L7_NULLPTR /* PTin added: stormcontrol */, &rate_unit) == L7_SUCCESS )
      {
        if (val != FD_POLICY_DEFAULT_BCAST_STORM_MODE)  /* If not the default value. */
        {
          if (threshold != FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD ||
               rate_unit != FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT)
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastLvl_2,threshold);
               EWSWRITEBUFFER(ewsContext,stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastRate_2,threshold);
               EWSWRITEBUFFER(ewsContext,stat);
            }
            if (val == L7_DISABLE)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlBcast);
              EWSWRITEBUFFER(ewsContext, stat);
            }
          }
          else if (val == L7_ENABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcast_2);
            EWSWRITEBUFFER(ewsContext,stat);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlBcast);
            EWSWRITEBUFFER(ewsContext,stat);
          }
        }
        else if (threshold != FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD ||
                  rate_unit != FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastLvl_2,threshold);
             EWSWRITEBUFFER(ewsContext,stat);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastRate_2,threshold);
             EWSWRITEBUFFER(ewsContext,stat);
          }
          if (val == L7_DISABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlBcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
        else if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          if (val == L7_DISABLE)
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastLvl_2,FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastRate_2,FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlBcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
          else
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastLvl_2,FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlBcastRate_2,FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
          }
        }
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbSwDevCtrlMcastStormModeIntfGet(interface, &val) == L7_SUCCESS &&
          usmDbSwDevCtrlMcastStormThresholdIntfGet(interface, &threshold, L7_NULLPTR /* PTin added: stormcontrol */, &rate_unit) == L7_SUCCESS )
      {
        if (val != FD_POLICY_DEFAULT_MCAST_STORM_MODE)  /* If not the default value. */
        {
          if (threshold != FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD ||
               rate_unit != FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT)
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastLvl_2,threshold);
               EWSWRITEBUFFER(ewsContext,stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastRate_2,threshold);
               EWSWRITEBUFFER(ewsContext,stat);
            }
            if (val == L7_DISABLE)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlMcast);
              EWSWRITEBUFFER(ewsContext, stat);
            }
          }
          else if (val == L7_ENABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcast_2);
            EWSWRITEBUFFER(ewsContext,stat);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlMcast);
            EWSWRITEBUFFER(ewsContext,stat);
          }
        }
        else if (threshold != FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD ||
                  rate_unit != FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastLvl_2,threshold);
             EWSWRITEBUFFER(ewsContext,stat);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastRate_2,threshold);
             EWSWRITEBUFFER(ewsContext,stat);
          }
          if (val == L7_DISABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlMcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
        else if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          if (val == L7_DISABLE)
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastLvl_2,FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastRate_2,FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlMcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
          else
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastLvl_2,FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlMcastRate_2,FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
          }
        }
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbSwDevCtrlUcastStormModeIntfGet(interface, &val) == L7_SUCCESS &&
          usmDbSwDevCtrlUcastStormThresholdIntfGet(interface, &threshold, L7_NULLPTR /* PTin added: stormcontrol */, &rate_unit) == L7_SUCCESS )
      {
        if (val != FD_POLICY_DEFAULT_UCAST_STORM_MODE)  /* If not the default value. */
        {
          if (threshold != FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD ||
               rate_unit != FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT)
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastLvl_2,threshold);
               EWSWRITEBUFFER(ewsContext,stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastRate_2,threshold);
               EWSWRITEBUFFER(ewsContext,stat);
            }
            if (val == L7_DISABLE)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlUcast);
              EWSWRITEBUFFER(ewsContext, stat);
            }
          }
          else if (val == L7_ENABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcast_2);
            EWSWRITEBUFFER(ewsContext,stat);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlUcast);
            EWSWRITEBUFFER(ewsContext,stat);
          }
        }
        else if (threshold != FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD ||
                  rate_unit != FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT)
        {
          if (rate_unit == L7_RATE_UNIT_PERCENT) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastLvl_2,threshold);
             EWSWRITEBUFFER(ewsContext,stat);
          }
          else if (rate_unit == L7_RATE_UNIT_PPS) 
          {
             osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastRate_2,threshold);
             EWSWRITEBUFFER(ewsContext,stat);
          }
          if (val == L7_DISABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlUcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
        else if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          if (val == L7_DISABLE)
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastLvl_2, FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastRate_2, FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoStormCntrlUcast);
            EWSWRITEBUFFER(ewsContext, stat);
          }
          else
          {
            if (rate_unit == L7_RATE_UNIT_PERCENT) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastLvl_2, FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
            else if (rate_unit == L7_RATE_UNIT_PPS) 
            {
               osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_StormCntrlUcastRate_2, FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD);
               EWSWRITEBUFFER(ewsContext, stat);
            }
          }
        }
      }
    }

    /* LACP Mode */
    if (usmDbDot3adAggPortLacpModeGet(unit, interface, &val) == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        val_lacp = L7_TRUE;
      }                           /* since val can be L7_ENABLE OR L7_DISABLE */
      /*new variable created for comparison to default value */
      else
      {
        val_lacp = L7_FALSE;
      }
      /*new variable used for comparison*/
      cliShowCmdTrue (ewsContext, val_lacp, FD_DOT3AD_DEFAULT_LACP_ENABLED, pStrInfo_base_PortLacpmode);
    }
    lacpState = 0;
    if (usmDbDot3adAggPortActorAdminStateGet(unit, interface, (L7_uchar8 *) &lacpState) == L7_SUCCESS)
    {
      /* Does the factory default have this time out bit set ?*/
      timeoutBit = (FD_DOT3AD_ACTOR_ADMIN_PORT_STATE & DOT3AD_STATE_LACP_TIMEOUT) ? L7_TRUE : L7_FALSE;
      timeoutBitCurrent = (lacpState & DOT3AD_STATE_LACP_TIMEOUT) ? L7_TRUE : L7_FALSE;
      if ((timeoutBit != timeoutBitCurrent) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        if (((L7_uchar8) (lacpState & DOT3AD_STATE_LACP_TIMEOUT)))
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_PortLacptimeoutActorShort);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_PortLacptimeoutActorLong);
        }

        EWSWRITEBUFFER(ewsContext,stat);
      }
    }

    lacpState = 0;
    if (usmDbDot3adAggPortPartnerAdminStateGet(unit, interface, (L7_uchar8 *) &lacpState) == L7_SUCCESS)
    {
      /* Does the factory default have this time out bit set ?*/
      timeoutBit = (FD_DOT3AD_PARTNER_ADMIN_PORT_STATE & DOT3AD_STATE_LACP_TIMEOUT) ? L7_TRUE : L7_FALSE;
      timeoutBitCurrent = (lacpState & DOT3AD_STATE_LACP_TIMEOUT) ? L7_TRUE : L7_FALSE;
      if ((timeoutBit != timeoutBitCurrent) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        if (((L7_uchar8) (lacpState & DOT3AD_STATE_LACP_TIMEOUT)))
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_PortLacptimeoutPartnerShort);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_PortLacptimeoutPartnerLong);
        }

        EWSWRITEBUFFER(ewsContext,stat);
      }
    }

    /* Control Mode */
    if (( usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS) && (usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS) && (cliDot1xInterfaceValidateWithMessage(ewsContext, unit, interface, L7_FALSE ) == L7_SUCCESS ))
    {
      if ( usmDbDot1xPortControlModeGet(unit, interface, &val) == L7_SUCCESS )
      {
        if ((val != FD_DOT1X_PORT_MODE) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          switch (val)
          {
            case L7_DOT1X_PORT_AUTO:
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Dot1xPortCntrlAuto_1);
              EWSWRITEBUFFER(ewsContext,stat);
              break;
            case L7_DOT1X_PORT_FORCE_AUTHORIZED:
              osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Dot1xPortCntrlForceAuthorized);
              EWSWRITEBUFFER(ewsContext,stat);
              break;
            case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
              osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Dot1xPortCntrlForceUnauthorized);
              EWSWRITEBUFFER(ewsContext,stat);
              break;
            case L7_DOT1X_PORT_AUTO_MAC_BASED:
              osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Dot1xPortCntrlMacBased);
              EWSWRITEBUFFER(ewsContext,stat);
              break;

            default:
              break;

          }
        }
      }
    }
    /* Reauthentication Enabled */
    if (usmDbDot1xPortReAuthEnabledGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdTrue(ewsContext,val,FD_DOT1X_PORT_REAUTH_ENABLED,pStrInfo_base_Dot1xReAuth_2);
    }

    /* Quiet Period */
    if (usmDbDot1xPortQuietPeriodGet(unit, interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_QUIET_PERIOD,pStrInfo_base_Dot1xTimeoutQuietPeriod);
    }
    /* Reauthentication Period */
    if ( usmDbDot1xPortReAuthPeriodGet(unit, interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_REAUTH_PERIOD,pStrInfo_base_Dot1xTimeoutReauthPeriod);
    }
    /* Supplicant Timeout */
    if ( usmDbDot1xPortSuppTimeoutGet(unit, interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_SUPP_TIMEOUT,pStrInfo_base_Dot1xTimeoutSuppTimeout);
    }

    /* Transmit Period */
    if ( usmDbDot1xPortTxPeriodGet(unit, interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_TX_PERIOD,pStrInfo_base_Dot1xTimeoutTxPeriod);
    }
    /* Guest Vlan Id*/
    if ( usmDbDot1xAdvancedGuestPortsCfgGet(unit, interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_GUEST_VLAN,pStrInfo_base_Dot1xGuestVlan_1);
    }
    /*Guest Vlan Period*/
    if(usmDbDot1xAdvancedPortGuestVlanPeriodGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_GUEST_PERIOD,pStrInfo_base_Dot1xGuestVlanPeriod);
    }

    /* Server Timeout */
    if(usmDbDot1xPortServerTimeoutGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_SERVER_TIMEOUT,pStrInfo_base_Dot1xTimeoutSrvrTimeout);
    }

    /* Maximum Requested */
    if ( usmDbDot1xPortMaxReqGet(unit, interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_MAX_REQ,pStrInfo_base_Dot1xMaxReq_1);
    }
    /* Max-user */
    if((usmDbDot1xPortMaxUsersGet(unit, interface, &val) == L7_SUCCESS ) && (val != FD_DOT1X_PORT_MAX_USERS))
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_MAX_USERS,pStrInfo_base_Dot1xMaxUsers);
    }
    /* Unauthenticated vlan */
    if((usmDbDot1xPortUnauthenticatedVlanGet(unit, interface, &val) == L7_SUCCESS ) 
      && (val != FD_DOT1X_PORT_UNAUTHENTICATED_VLAN))
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_PORT_UNAUTHENTICATED_VLAN,pStrInfo_base_Dot1xUnatuhenticatedVlan);
    }
    /* Voice Vlan */
    cliRunningConfigInterfaceVoiceVlanInfo(ewsContext,interface);

    /* Control Mode */
    if (( usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS) && (usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS) && (cliDot1xInterfaceValidateWithMessage(ewsContext, unit, interface, L7_FALSE ) == L7_SUCCESS ))
    {
      paeCap = 0x00;
      if (usmDbDot1xPortPaeCapabilitiesGet(unit,interface, &paeCap) == L7_SUCCESS)
      {
        if (paeCap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
        {
#if defined L7_DOT1AD_PACKAGE
          {
            dot1adInterfaceTypeGet(interface, &intfType);
            if(intfType == DOT1AD_INTFERFACE_TYPE_NNI && interface != FD_DOT1X_WAN_PORT)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                                 pStrInfo_base_Dot1xPaeCapability);
                EWSWRITEBUFFER(ewsContext,stat);
            }
          }
#else
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                                 pStrInfo_base_Dot1xPaeCapability);
                EWSWRITEBUFFER(ewsContext,stat);
#endif
        }
        else if (paeCap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
        {
#if defined L7_DOT1AD_PACKAGE
          {
            dot1adInterfaceTypeGet(interface, &intfType);
            if(intfType == DOT1AD_INTFERFACE_TYPE_UNI && interface == FD_DOT1X_WAN_PORT)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                                 pStrInfo_base_Dot1xAuthPaeCapability);
                EWSWRITEBUFFER(ewsContext,stat);
            }
          }
#endif
        }
      }
      if ( usmDbDot1xSupplicantPortControlModeGet(interface, &val) == L7_SUCCESS )
      {
        if ((val != FD_DOT1X_SUPPLICANT_PORT_MODE) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          switch (val)
          {
            case L7_DOT1X_PORT_AUTO:
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                     pStrInfo_base_Dot1xSupplicantPortCntrlAuto);
              EWSWRITEBUFFER(ewsContext,stat);
              break;
            case L7_DOT1X_PORT_FORCE_AUTHORIZED:
              osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),
                                   pStrInfo_base_Dot1xSupplicantPortCntrlForceAuthorized);
              EWSWRITEBUFFER(ewsContext,stat);
              break;
            case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
              osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),
                                  pStrInfo_base_Dot1xSupplicantPortCntrlForceUnAuthorized);
              EWSWRITEBUFFER(ewsContext,stat);
              break;
            default:
              break;

          }
        }
      }
    }
    /* User Name */
    if ( usmDbDot1xSupplicantPortUserNameGet (interface, suppUsrName) == L7_SUCCESS)
    {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),
                                  pStrInfo_base_SupplicantUsrName,suppUsrName);
              EWSWRITEBUFFER(ewsContext,stat);
    }


    /* Maximum Start */
    if (usmDbDot1xSupplicantPortMaxStartGet(interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_SUPPLICANT_PORT_MAX_START,pStrInfo_base_Dot1xMaxStart);
    }
    /* Start Period */
    if (usmDbDot1xSupplicantPortStartPeriodGet(interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_SUPPLICANT_PORT_START_PERIOD,pStrInfo_base_Dot1xTimeoutStartPeriod);
    }
    /* Held Period */
    if (usmDbDot1xSupplicantPortHeldPeriodGet(interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_SUPPLICANT_PORT_HELD_PERIOD,pStrInfo_base_Dot1xTimeoutHeldPeriod);
    }
    /* Auth Period */
    if (usmDbDot1xSupplicantPortAuthPeriodGet(interface, &val) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1X_SUPPLICANT_PORT_AUTH_PERIOD,pStrInfo_base_Dot1xTimeoutAuthPeriod);
    }
   
  } /*end of commands supported only for physical intfs*/

  /*check if diffserv lag is supported or the interface is a phy. intf*/

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  if (usmDbQosDiffServIsValidIntf(unit, interface) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
          L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE)
    {
      if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              L7_DIFFSERV_SERVICE_SUPPORTS_LAG_INTF_FEATURE_ID) == L7_TRUE) ||
          (rc2 == L7_SUCCESS && sysIntfType == L7_PHYSICAL_INTF) )
      {
        cliRunningPhysicalInterfaceDiffservInfo(ewsContext, unit, interface);
      }

    }
  }

#endif

  /*check if lag is supported for CoS or the intf is a phy. intf*/

#ifdef L7_QOS_FLEX_PACKAGE_COS
  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID,
          L7_COS_LAG_INTF_SUPPORT_FEATURE_ID) == L7_TRUE) ||
      (rc2 == L7_SUCCESS && sysIntfType == L7_PHYSICAL_INTF))
  {
    if (usmDbQosCosMapIntfIsValid(unit, interface) == L7_TRUE)
    {
      cliRunningConfigInterfaceClassofService(ewsContext, unit, interface);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID,
          L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbQosCosQueueIntfIsValid(unit, interface) == L7_TRUE)
      {
        cliRunningConfigInterfaceRandomDetect(ewsContext, unit, interface);
        cliRunningConfigInterfaceTailDrop(ewsContext, unit, interface);
        cliRunningConfigInterfaceCosQueue(ewsContext, unit, interface);
      }
    }
  }
#endif

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  if ((usmDbComponentPresentCheck(unit, L7_FLEX_QOS_VOIP_COMPONENT_ID) == L7_TRUE) ||
      (rc2 == L7_SUCCESS && sysIntfType == L7_PHYSICAL_INTF))
  {
    if (usmdbQoSVoIPIsValidIntf(interface) == L7_TRUE)
    {
      cliRunningConfigInterfaceAutoVoIPInfo(ewsContext, interface);
    }
  }
#endif

  if ( usmDbIfMgmtAdminStateGet(interface, &val) == L7_SUCCESS)
  {
    if (( val != FD_NIM_ADMIN_STATE) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      switch(val)
      {

        case L7_ENABLE:
        case L7_DIAG_DISABLE:                     /* diag disable is a run time state not a configured state*/
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoShutdown);
          EWSWRITEBUFFER(ewsContext, stat);
          break;

        case L7_DISABLE:
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_Shutdown_2);
          EWSWRITEBUFFER(ewsContext, stat);
          break;

        default:
          break;
      }

    }
  }

#ifdef L7_ROUTING_PACKAGE
  if ( usmDbIfBWGet(interface, &bandwidth) == L7_SUCCESS)
  {
    if (bandwidth != FD_IP_DEFAULT_BW)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_Bandwidth, bandwidth);
      EWSWRITEBUFFER(ewsContext, stat);
    }
    else if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_NoBandwidth);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }
#endif


  if( (rc = usmDbIfAliasGet(unit, interface, stat1)) == L7_SUCCESS)
  {
    if((strcmp(stat1, FD_NIM_IF_ALIAS) != 0 ) || EWSSHOWALL(ewsContext)
        == L7_SHOW_DEFAULT)
    {
      if(strcmp(stat1, FD_NIM_IF_ALIAS) != 0)
      {
        OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, buffer, pStrInfo_base_Desc_2);
        OSAPI_STRNCAT(buffer,"'");
        OSAPI_STRNCAT(buffer,stat1);
        OSAPI_STRNCAT(buffer,"'");
        EWSWRITEBUFFER(ewsContext, buffer);
      }
      else
      {
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_NoDesc);
      }
    }
  }

  if( sysIntfType == L7_LAG_INTF)
  {
    if (usmDbDot3adAdminModeGet(unit, interface, &val) ==  L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val,FD_DOT3AD_ADMIN_MODE,pStrInfo_base_Adminmode);
    }
    if(usmDbDot3adIsStaticLag(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdTrue(ewsContext,val,FD_DOT3AD_STATIC_MODE,pStrInfo_base_LagStatic);
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, 
                                 L7_DOT3AD_HASHMODE_INTF_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbDot3adLagHashModeGet(unit, interface, &hashMode) ==  L7_SUCCESS)
      {
        if ( hashMode != FD_DOT3AD_HASH_MODE )
        {
          OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, buffer, pStrInfo_base_LagLoadBalance);
          osapiSnprintf(stat, sizeof(stat), "%d", hashMode);
          OSAPI_STRNCAT(buffer, stat);
          EWSWRITEBUFFER(ewsContext,buffer);
        }
      }
    }
  }
  else
  {
    if (usmDbIfLinkUpDownTrapEnableGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext, val, FD_TRAP_LINK_STATUS, pStrInfo_base_SnmpTrapLinkStatus_1);
    }
  }

  if (usmDbGarpIsValidIntf(unit, interface) == L7_TRUE)
  {
    /* garp config info */
    if (usmDbGarpJoinTimeGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,(DEFAULT_GARPCONFIGJOINTIME/10),pStrInfo_base_SetGarpTimerJoin);
    }

    if (usmDbGarpLeaveTimeGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,(DEFAULT_GARPCONFIGLEAVETIME/10),pStrInfo_base_SetGarpTimerLeave);
    }

    if (usmDbGarpLeaveAllTimeGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,(DEAFULT_GARPCONFIGLEAVEALLTIME/10),pStrInfo_base_SetGarpTimerLeaveall);
    }

  }
  /* igmpsnooping  config info */
  if (usmDbComponentPresentCheck (unit, L7_SNOOPING_COMPONENT_ID) == L7_TRUE &&
      usmDbSnoopIsValidIntf (unit, interface) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck (unit, L7_SNOOPING_COMPONENT_ID,
          L7_IGMP_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
    {
      memset(&vlanStaticMcastRtr, 0, sizeof(vlanStaticMcastRtr));

      if (usmDbSnoopIntfModeGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdEnable (ewsContext, val, FD_IGMP_SNOOPING_INTF_MODE, pStrInfo_base_SetIgmp);
      }

      if (usmDbSnoopIntfGroupMembershipIntervalGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdInt (ewsContext, val, FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL,
            pStrInfo_common_SetIgmpGrpMbrshipIntvl);
      }
      /* Query Response */
      if (usmDbSnoopIntfResponseTimeGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdInt (ewsContext, val, FD_IGMP_SNOOPING_MAX_RESPONSE_TIME,
            pStrInfo_switching_SetIgmpMaxresponse);
      }

      /* Multicast Router Present */
      if (usmDbSnoopIntfMcastRtrExpiryTimeGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdInt (ewsContext, val, FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME,
            pStrInfo_common_SetIgmpMcrtrexpiretime);
      }
      if (usmDbSnoopIntfFastLeaveAdminModeGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdEnable (ewsContext, val, FD_IGMP_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE,
            pStrInfo_base_SetIgmpFastLeave_1);
      }

      if (usmDbSnoopIntfMrouterStatusGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdEnable (ewsContext, val, L7_DISABLE, pStrInfo_base_SetIgmpMrouterIntf);

      }
      if (usmDbSnoopIntfApiVlanStaticMcastRtrMaskGet (unit, interface, &vlanStaticMcastRtr, family) == L7_SUCCESS)
      {
        for (pvid = 1; pvid <= L7_DOT1Q_MAX_VLAN_ID; pvid++)
        {
          if (L7_VLAN_ISMASKBITSET(vlanStaticMcastRtr, pvid))
          {
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_base_SetIgmpMrouter, pvid);
              EWSWRITEBUFFER(ewsContext, stat);
          }
        }
      }

    }
    if (usmDbFeaturePresentCheck (unit, L7_SNOOPING_COMPONENT_ID,
          L7_MLD_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
    {
      memset(&vlanStaticMcastRtr, 0, sizeof(vlanStaticMcastRtr));
      /* MLD Snooping */
      family = L7_AF_INET6;
      if (usmDbSnoopIntfModeGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdEnable (ewsContext, val, FD_MLD_SNOOPING_INTF_MODE, pStrInfo_base_SetMld);
      }
      if (usmDbSnoopIntfGroupMembershipIntervalGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdInt (ewsContext, val, FD_MLD_SNOOPING_GROUP_MEMBERSHIP_INTERVAL, pStrInfo_common_SetMldGrpMbrshipIntvl);
      }
      /* Query Response */
      if (usmDbSnoopIntfResponseTimeGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdInt (ewsContext, val, FD_MLD_SNOOPING_MAX_RESPONSE_TIME, pStrInfo_base_SetMldMaxresponse);
      }

      /* Multicast Router Present */
      if (usmDbSnoopIntfMcastRtrExpiryTimeGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdInt (ewsContext, val, FD_MLD_SNOOPING_MCAST_RTR_EXPIRY_TIME, pStrInfo_base_SetMldMcrtrexpiretime_1);
      }

      if (usmDbSnoopIntfFastLeaveAdminModeGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdEnable (ewsContext, val, FD_MLD_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE, pStrInfo_base_SetMldFastLeave);
      }
      if (usmDbSnoopIntfMrouterStatusGet (unit, interface, &val, family) == L7_SUCCESS)
      {
        cliShowCmdEnable (ewsContext, val, L7_DISABLE, pStrInfo_base_SetMldMrouterIntf);
      }
      if (usmDbSnoopIntfApiVlanStaticMcastRtrMaskGet (unit, interface, &vlanStaticMcastRtr, family) == L7_SUCCESS)
      {
        for (pvid = 1; pvid <= L7_DOT1Q_MAX_VLAN_ID; pvid++)
        {
          if (L7_VLAN_ISMASKBITSET(vlanStaticMcastRtr, pvid))
          {
              sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_base_SetMldMrouter, pvid);
              EWSWRITEBUFFER(ewsContext, stat);
          }
        }
      }
    }   /* End of mld snooping feature present check */

  }                             /* end igmpsnooping  config info */

#ifdef L7_STATIC_FILTERING_PACKAGE
  if (usmDbFilterIsValidIntf (unit, interface) == L7_TRUE)
  {
    /* MacFilter Info */
    cliRunningConfigMacfilterPhyIntfInfo(ewsContext, unit, interface);
  }
#endif

  if (usmDbGarpIsValidIntf(unit, interface) == L7_TRUE)
  {
    /* Factory default set in dot1qBuildDefaultQportsData to L7_DISABLE */
    /* it should be set to some FD value which can be enable/disbale */

    if (usmDbGarpGmrpPortEnableGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_GMRP_GARP_DEFAULT_PORT_STATUS,pStrInfo_base_SetGmrpIntfmode);
    }
  }

  if (usmDbQportsEnableGVRPGet(unit, interface, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_GVRP_DEFAULT_INTERFACE_MODE,pStrInfo_base_SetGvrpIntfmode);
  }

  /* end garp config info */

#ifdef L7_PBVLAN_PACKAGE
  /* protocol vlan group*/
  cliRunningPbVlanPhysicalInterfaceInfo(ewsContext,unit,interface);
#endif
  /* spanningtree port config info */
  if ((usmDbDot1sIsValidIntf(unit, interface) == L7_TRUE))
  {
    if ( usmDbDot1sCistPortEdgeGet(unit, interface, &boolVal) == L7_SUCCESS)
    {
      cliShowCmdTrue(ewsContext,boolVal,FD_DOT1S_DEFAULT_ADMIN_EDGE,pStrInfo_base_SpanTreeEdgeport);
    }

    if(usmDbDot1sIntfBpduFilterGet(unit, interface, &boolVal) == L7_SUCCESS )
    {
      cliShowCmdTrue(ewsContext, boolVal, FD_DOT1S_BPDUFILTER, pStrInfo_base_SpanTreeBpdufilter);
    }

    tmpVal = FD_DOT1S_BPDUFLOOD;
    if(usmDbDot1sIntfBpduFloodGet(unit, interface, &tmpVal) == L7_SUCCESS )
    {
      if (tmpVal != FD_DOT1S_BPDUFLOOD)
      {
        cliShowCmdTrue(ewsContext, tmpVal, !tmpVal, pStrInfo_base_SpanTreeBpduflood);
      }
    }

    if ( usmDbDot1sPortAutoEdgeGet(unit, interface, &boolVal) == L7_SUCCESS )
    {
      cliShowCmdTrue(ewsContext,boolVal,FD_DOT1S_AUTO_EDGE,pStrInfo_base_SpanTreeAutoEdge);
    }

    if ( usmDbDot1sPortRestrictedRoleGet(unit, interface, &boolVal) == L7_SUCCESS)
    {
      cliShowCmdTrue(ewsContext,boolVal,FD_DOT1S_ROOTGUARD,pStrInfo_base_SpanTreeRootguard_1);
    }

    if ( usmDbDot1sPortLoopGuardGet(unit, interface, &boolVal) == L7_SUCCESS)
    {
      cliShowCmdTrue(ewsContext,boolVal,FD_DOT1S_LOOPGUARD,pStrInfo_base_SpanTreeLoopguard);
    }

    if ( usmDbDot1sPortRestrictedTcnGet(unit, interface, &boolVal) == L7_SUCCESS)
    {
      cliShowCmdTrue(ewsContext,boolVal,FD_DOT1S_TCNGUARD,pStrInfo_base_SpanTreeTcnguard);
    }

    /* spanning-tree hello time on this interface */
    if (usmDbDot1sCistPortAdminHelloTimeGet(unit, interface, &val) == L7_SUCCESS)
    {
      /*val==0 indicates that hello-time is not configured on that interface*/
      if (val != 0)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SpanTreeHelloTime_2, val );
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }

    if(usmDbDot1sPortStateGet(unit, interface, &boolVal) == L7_SUCCESS)
    {
      cliShowCmdTrue(ewsContext,boolVal,FD_DOT1S_PORT_MODE,pStrInfo_base_SpanTreePortMode);
    }

    rc=usmDbDot1sInstanceFirstGet(unit, &mstid) ;
    do
    {
      if(usmDbDot1sMstiPortPathCostModeGet(unit, mstid, interface, &boolVal) == L7_SUCCESS)
      {
        if(boolVal != L7_TRUE || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          if ( usmDbDot1sMstiPortPathCostGet(unit, mstid, interface, &val) == L7_SUCCESS )
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SpanTreeMstCost, mstid);
            cliShowCmdInt(ewsContext, val, FD_DOT1S_PORT_PATH_COST, stat);
          }
        }
      }

      if (usmDbDot1sCistExtPortPathCostModeGet(unit, interface) != L7_TRUE)
      {
        if((usmDbDot1sCistPortExternalPathCostGet(unit, interface, &val) == L7_SUCCESS) && mstid == 0)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SpanTreeMstExternalCost, mstid);
          cliShowCmdInt(ewsContext, val, FD_DOT1S_PORT_PATH_COST, stat);
        }
      }

      /* Port Priority */
      if (usmDbDot1sMstiPortPriorityGet(unit, mstid, interface, &val) == L7_SUCCESS )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SpanTreeMstPortPri, mstid);
        defPrio = usmDbDot1sDefaultPortPriorityGet(unit,interface);
        cliShowCmdInt(ewsContext, val, defPrio, stat);
      }
    } while(usmDbDot1sInstanceNextGet(unit, mstid, &mstid) == L7_SUCCESS);  /* end of while */
  }   /* end of if dot1s valid intf */
#ifdef L7_DHCP_SNOOPING_PACKAGE
  cliRunningConfigDhcpSnoopingInterfaceInfo(ewsContext, interface);
#ifdef L7_IPSG_PACKAGE
  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) == L7_TRUE)
  {
    cliRunningConfigIpsgInterfaceInfo(ewsContext, interface);
  }
#endif
#endif

#ifdef L7_DAI_PACKAGE
  cliRunningConfigDaiInterfaceInfo(ewsContext, interface);
#endif

  /*
   * This should not have a separate check for a vlan interface,
   * but I'm waiting for input on consistency of interface type
   * checking for dot1d - eberge
   */
  if ((usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) == L7_TRUE) &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF) &&
      (usmDbDot1dIsValidIntf(unit, interface) == L7_TRUE))
  {
    cliRunningConfigInterfaceClassofServiceDot1p(ewsContext, unit, interface);
  }

  /* mtu config info */
  if ((usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID,
          L7_NIM_JUMBOFRAMES_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID,
                                L7_NIM_JUMBOFRAMES_PER_INTERFACE_FEATURE_ID) == L7_TRUE) &&
      (usmDbIntfParmCanSet(interface, L7_INTF_PARM_MTU) == L7_TRUE))
  {
    if (usmDbIfConfigMaxFrameSizeGet(interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt (ewsContext, val, FD_NIM_DEFAULT_MAX_FRAME_SIZE, pStrInfo_common_Mtu);
    }
  }

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
   if (usmDbComponentPresentCheck(L7_NULL, L7_FLEX_METRO_DOT1AD_COMPONENT_ID))
  {
    /* dot1ad interface running config */
    cliRunningConfigdot1adInfo(ewsContext, unit, interface);
    /* dot1ad service subscription config */
    cliRunningConfigdot1adServiceSubscInfo(ewsContext, interface);
  }
#endif
#endif

  if (usmDbDot1qIsValidIntf(unit, interface) == L7_TRUE)
  {
    if ( usmDbQportsPVIDGet(unit, interface, &pvid) == L7_SUCCESS )
    {
      cliShowCmdInt(ewsContext,pvid,FD_DOT1Q_DEFAULT_VLAN,pStrInfo_base_VlanPvid_1);
    }
    if ( usmDbQportsAcceptFrameTypeGet(unit, interface, &val) == L7_SUCCESS )
    {
      if(( val != L7_DOT1Q_ADMIT_ALL )||EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        switch (val)
        {
          case L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED:
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_VlanAcceptframeVlanonly);
            EWSWRITEBUFFER(ewsContext, stat);
            break;
          case L7_DOT1Q_ADMIT_ALL:
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_VlanAcceptframeAll);
            EWSWRITEBUFFER(ewsContext, stat);
            break;
          case L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED:
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_VlanAcceptframeAdmitUntaggedonly);
            EWSWRITEBUFFER(ewsContext, stat);
            break;
          default:
            /* Do nothing */
            break;
        }

      }
    }
    memset(stat, 0, sizeof(stat));
    /* enableIngressFiltering should set to factory default value. As of **
     ** now function dot1qBuildDefaultQportsData is setting to L7_DISABLE */
    if (usmDbFeaturePresentCheck(unit, L7_DOT1Q_COMPONENT_ID, L7_DOT1Q_INGRESSFILTER_FEATURE_ID) != L7_TRUE)
    {
      if (usmDbQportsEnableIngressFilteringGet(unit, interface, &val) == L7_SUCCESS )
      {
        cliShowCmdEnable(ewsContext,val,FD_DOT1Q_DEFAULT_INGRESS_FILTER_PORT,pStrInfo_base_VlanIngressfilter);
      }
    }

    /* loop over all vlan ids.
     * if intf is part of default vlan: mode include => skip
     *                                  mode exclude/auto => display
     * if intf is part of created vlan: mode auto => skip
     *                                  mode include/exclude => display
     */
    rc = usmDbNextVlanGet(unit, 0, &nextvid);
    while (rc == L7_SUCCESS)
    {
      memset(stat, 0, sizeof(stat));
      if(rc2 == L7_SUCCESS)
      {
        if ((sysIntfType == L7_PHYSICAL_INTF) || (sysIntfType == L7_LAG_INTF))
        {
          if (usmDbVlanMemberSetGet(unit, nextvid, interface, &val) == L7_SUCCESS)
          {
            if (nextvid == L7_DOT1Q_DEFAULT_VLAN)
            {
#if defined(L7_DOT1AD_PACKAGE) && defined(FEAT_METRO_CPE_V1_0)
             dot1adInterfaceTypeGet(interface, &intfType);
#endif
              switch (val)
              {
                case L7_DOT1Q_FORBIDDEN:
#if defined(L7_DOT1AD_PACKAGE) && defined(FEAT_METRO_CPE_V1_0)
                if ( (intfType == DOT1AD_INTFERFACE_TYPE_UNI) ||
                     (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)||
                     (intfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
                   )
                break;
#endif

                excludeVids[excludeCount] = nextvid;
                excludeCount++;
                break;
#if defined(L7_DOT1AD_PACKAGE) && defined(FEAT_METRO_CPE_V1_0)
                case L7_DOT1Q_FIXED:
                if (intfType == DOT1AD_INTFERFACE_TYPE_NNI ||
                    intfType == DOT1AD_INTFERFACE_TYPE_SWITCHPORT)
                break;
                incldueVids[includeCount] = nextvid;
                includeCount++;
                break;
#endif


                case L7_DOT1Q_NORMAL_REGISTRATION:
                  autoVids[autoCount] = nextvid;
                  autoCount++;
                  break;

                default:
                  break;
              }

            }
            else
            {
              switch (val)
              {
                case L7_DOT1Q_FIXED:
                  incldueVids[includeCount] = nextvid;
                  includeCount++;
                  break;

                case L7_DOT1Q_FORBIDDEN:
                  excludeVids[excludeCount] = nextvid;
                  excludeCount++;
                  break;

                default:
                  break;
              }

            }
          }
      /* Tagging for this interface and vlan */
      if(usmDbVlanTaggedSetGet(unit, nextvid, interface, &val) == L7_SUCCESS)
          {  
             if(val != FD_DOT1Q_DEFAULT_TAGGING)
             { 
               taggedVids[tagCount] = nextvid;
               tagCount++;
             }
             else if (EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT)
             {
               notaggedVids[notagCount] = nextvid;
               notagCount++;
             }
          } 
      
        }
      }

      rc = usmDbNextVlanGet(unit, nextvid, &nextvid);
    }
    if(autoCount > 0)
       displayRangeFormat(ewsContext, autoVids, autoCount, pStrInfo_base_VlanParticipationAuto_1);

    if(excludeCount > 0)
       displayRangeFormat(ewsContext, excludeVids, excludeCount, pStrInfo_base_VlanParticipationExcl_1);

    if(includeCount > 0)
       displayRangeFormat(ewsContext, incldueVids, includeCount, pStrInfo_base_VlanParticipationIncl_1);
 
    if(tagCount > 0)
       displayRangeFormat(ewsContext, taggedVids, tagCount, pStrInfo_base_VlanTagging_2);

    if(notagCount > 0)
       displayRangeFormat(ewsContext, notaggedVids, notagCount, pStrInfo_base_VlanTagging_3);

  }

  if (usmDbDot1dIsValidIntf(unit, interface) == L7_TRUE &&
      usmDbDot1qIsValidIntf(unit, interface) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID,
        L7_DOT1P_FEATURE_SUPPORTED) == L7_TRUE)
  {
    /* Need to check with default value*/
    if (usmDbDot1dPortDefaultUserPriorityGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_DOT1P_DEFAULT_USER_PRIORITY,pStrInfo_base_VlanPri);
    }
  }

  if (usmDbComponentPresentCheck(unit, L7_PORT_MACLOCK_COMPONENT_ID) == L7_TRUE &&
      usmDbPmlIsValidIntf(unit, interface) == L7_TRUE)
  {
    cliRunningConfigPmlInterfaceInfo(ewsContext, unit, interface);
  }
#ifdef L7_SFLOW_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_SFLOW_COMPONENT_ID))
  {
    /* sflow interface running config */
    cliRunningConfigsFlowInterfaceInfo(ewsContext, unit, interface);
  }
#endif
#ifdef L7_DOT3AH_PACKAGE
   if (usmDbComponentPresentCheck(L7_NULL, L7_DOT3AH_COMPONENT_ID))
  {
    /* dot3ah interface running config */
    cliRunningConfigdot3ahInfo(ewsContext, unit, interface);
  }
#endif
#ifdef L7_DOT1AG_PACKAGE
   /* dot1ag interface running config */
   if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
   {
     cliRunningConfigDot1agInterfaceInfo(ewsContext, unit, interface);
   }

#endif


  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of prompt
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

L7_RC_t cliRunningConfigPrompt(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 prompt[L7_PROMPT_SIZE];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  memset (prompt, 0, sizeof(prompt));
  memset (buf, 0, sizeof(buf));
  if (usmDbCommandPromptGet(unit,prompt) == L7_SUCCESS )
  {
    if (( strcmp(prompt, FD_CLI_WEB_COMMAND_PROMPT) != 0)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SetPrompt_1, prompt);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of slots
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

L7_RC_t cliRunningConfigSlotInfo(EwsContext ewsContext)
{
  L7_uint32 unit, numSlots, val, i, unitType;
  L7_uint32 slots[L7_MAX_SLOTS_PER_UNIT];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL bSlotExists;
  L7_uint32 cx;

  unit = 0;
  bSlotExists = L7_FALSE;

  if (usmDbUnitMgrStackMemberGetFirst(&unit) == L7_SUCCESS
      && usmDbUnitMgrUnitTypeGet(unit, &unitType) == L7_SUCCESS
      && usmDbUnitDbEntrySlotsGet(unitType, &numSlots, slots) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  while (rc == L7_SUCCESS)
  {
    for (i=0; i<numSlots; i++)
    {
      bSlotExists = L7_FALSE;
      if (usmDbCardConfiguredCardTypeGet(unit, slots[i], &val) != L7_NOT_EXIST)
      {
        if (usmDbCardIndexFromIDGet(val, &cx) == L7_SUCCESS)
        {
#ifndef L7_CHASSIS
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_Slot_3, cliDisplaySlotHelp(unit,slots[i]), cx);
          EWSWRITEBUFFER(ewsContext, buf);
#endif
          bSlotExists = L7_TRUE;
        }
      }
      else if (usmDbCardInsertedCardTypeGet(unit, slots[i], &val) != L7_NOT_EXIST)
      {
        if (usmDbCardIndexFromIDGet(val, &cx) == L7_SUCCESS)
        {
#ifndef L7_CHASSIS
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_Slot_3, cliDisplaySlotHelp(unit,slots[i]), cx);
          EWSWRITEBUFFER(ewsContext, buf);
#endif
          bSlotExists = L7_TRUE;
        }
      }

      if(bSlotExists == L7_TRUE)
      {
        if (usmDbSlotPowerModeGet(unit, slots[i], &val) == L7_SUCCESS)
        {
          if (val == L7_ENABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SetSlotPower_1, cliDisplaySlotHelp(unit,slots[i]));
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_NoSetSlotPower, cliDisplaySlotHelp(unit,slots[i]));
          }

          EWSWRITEBUFFER(ewsContext, buf);
        }

        if (usmDbSlotAdminModeGet(unit, slots[i], &val) == L7_SUCCESS)
        {
          if (val == L7_ENABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_NoSetSlotDsbl_1, cliDisplaySlotHelp(unit,slots[i]));
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SetSlotDsbl_1, cliDisplaySlotHelp(unit,slots[i]));
          }

          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    }

    if (usmDbUnitMgrStackMemberGetNext(unit, &unit) == L7_SUCCESS
        && usmDbUnitMgrUnitTypeGet(unit, &unitType) == L7_SUCCESS
        && usmDbUnitDbEntrySlotsGet(unitType, &numSlots, slots) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  EWSWRITEBUFFER(ewsContext,pStrInfo_common_CrLf);

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of SNTP
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
L7_RC_t cliRunningSNTPInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val, logIndex, bufIndex;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 valString[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 temp[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_ushort16 val16, val1, val2, val3;
  L7_BOOL show = L7_FALSE;
  L7_SNTP_SERVER_STATUS_t status;
  L7_SNTP_PACKET_STATUS_t packetStatus;

  if ( usmDbSntpClientModeGet(unit, &val) == L7_SUCCESS )
  {
    switch ( val )
    {
      case L7_SNTP_CLIENT_UNICAST:
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_SntpClientModeUcast);
        break;

      case L7_SNTP_CLIENT_BROADCAST:
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_SntpClientModeBcast);
        break;

      default:
        break;
    }
  }
  if (usmDbSntpClientUnicastPollIntervalGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val,L7_SNTP_DEFAULT_POLL_INTERVAL,pStrInfo_base_SntpUcastClientPollIntvl);
  }

  if ( usmDbSntpClientUnicastPollRetryGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val,L7_SNTP_DEFAULT_POLL_RETRY,pStrInfo_base_SntpUcastClientPollRetry);
  }
  if ( usmDbSntpClientUnicastPollTimeoutGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val,L7_SNTP_DEFAULT_POLL_TIMEOUT,pStrInfo_base_SntpUcastClientPollTimeout);
  }
  if ( usmDbSntpClientBroadcastPollIntervalGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val,L7_SNTP_DEFAULT_POLL_INTERVAL,pStrInfo_base_SntpBcastClientPollIntvl);
  }
  if ( usmDbSntpClientPortGet(unit, &val16) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val16,L7_SNTP_DEFAULT_CLIENT_PORT,pStrInfo_base_SntpClientPort_1);
  }

  logIndex = 0;

  while (usmDbSntpServerTableRowStatusGetNext(unit, logIndex, &status, &bufIndex)
      == L7_SUCCESS)
  {
    if (usmDbSntpServerStatsLastUpdateStatusGet(unit, bufIndex, &packetStatus) == L7_SUCCESS)
    {
      memset (stat, 0, sizeof(stat));
      osapiSnprintfAddBlanks (1, 0, 0, 1, "! sntp server status is ", stat, sizeof(stat), strUtilSntpStatusGet(packetStatus, L7_NULLPTR));
      EWSWRITEBUFFER(ewsContext, stat);
    }

    memset (valString, 0, sizeof(valString));
    memset (temp, 0, sizeof(temp));
    rc = usmDbSntpServerTableIpAddressGet(unit, bufIndex, temp);

    if (rc == L7_SUCCESS)
    {
      osapiSnprintf(valString , sizeof(valString), "\"%s\"", temp);
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SntpSrvr_2, valString);

      show = L7_FALSE;
      val1 = FD_SNTP_DEFAULT_SERVER_PRIORITY;
      val2 = FD_SNTP_DEFAULT_VERSION;
      val3 = FD_SNTP_DEFAULT_SERVER_PORT;
      if (usmDbSntpServerTablePriorityGet(unit, bufIndex, &val1) == L7_SUCCESS &&
          usmDbSntpServerTableVersionGet(unit, bufIndex, &val2) == L7_SUCCESS &&
          usmDbSntpServerTablePortGet(unit, bufIndex, &val3) == L7_SUCCESS)
      {
        /* Only display command arguments unless non-default or needed for later arguments */

        /* priority */
        if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) || val1 != FD_SNTP_DEFAULT_SERVER_PRIORITY || val2 != FD_SNTP_DEFAULT_VERSION || val3 != FD_SNTP_DEFAULT_SERVER_PORT)
        {
          osapiSnprintf(buf, sizeof(buf), "%d ", val1);
          OSAPI_STRNCAT(stat, buf);
        }

        /* version */
        if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) || val2 != FD_SNTP_DEFAULT_VERSION || val3 != FD_SNTP_DEFAULT_SERVER_PORT)
        {
          osapiSnprintf(buf, sizeof(buf), "%d ", val2);
          OSAPI_STRNCAT(stat, buf);
        }

        /* port */
        if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) || val3 != FD_SNTP_DEFAULT_SERVER_PORT)
        {
          osapiSnprintf(buf, sizeof(buf), "%d ", val3);
          OSAPI_STRNCAT(stat, buf);
        }
      }
    }     
    OSAPI_STRNCAT(stat,pStrInfo_common_CrLf);
    EWSWRITEBUFFER(ewsContext, stat);

    logIndex = bufIndex;

  }  /* while */

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of Logging
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

L7_RC_t cliRunningSyslogInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_IP_ADDRESS_TYPE_t addrType;
  L7_char8 *addrTypeStr;
  L7_uint32 val, sev, adminVal, logIndex, bufIndex;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH+L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strVal[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 val16;
  L7_ROW_STATUS_t status;
  L7_LOG_FULL_BEHAVIOR_t valBehavior;
  L7_LOG_EMAIL_ALERT_MSGTYPE_t msgType;
  L7_char8 address[L7_CLI_MAX_STRING_LENGTH];	
  L7_char8 subject[L7_CLI_MAX_STRING_LENGTH];		
  L7_IP_ADDRESS_TYPE_t type;
  L7_char8  mailServerAddr[L7_EMAIL_ALERT_SERVER_ADDRESS_SIZE];	

  /* In-Memory/Buffered*/

  if ( usmDbLogInMemoryAdminStatusGet(unit, &adminVal) == L7_SUCCESS )
  {
    cliShowCmdEnable (ewsContext, adminVal, FD_LOG_DEFAULT_IN_MEMORY_ADMIN_STATUS,pStrInfo_base_LoggingBufed_1);
  }
  if ( ( usmDbLogInMemoryBehaviorGet(unit, &valBehavior) == L7_SUCCESS ) &&
      (valBehavior != FD_LOG_DEFAULT_IN_MEMORY_BEHAVIOR || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT ) )
  {
    switch ( valBehavior )
    {
      case L7_LOG_STOP_ON_FULL_BEHAVIOR:
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoLoggingBufedWrap);
        break;
      case L7_LOG_WRAP_BEHAVIOR:
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingBufedWrap);
        break;
    }
  }

  /* Console */
  if (usmDbLogConsoleAdminStatusGet(unit, &val) == L7_SUCCESS )      
  {
    memset(stat, 0, sizeof(stat));
    switch ( val )
    {
      case L7_ADMIN_MODE_ENABLE:

        if (( usmDbLogConsoleSeverityFilterGet (unit, &sev) == L7_SUCCESS ) &&
            sev != FD_LOG_DEFAULT_CONSOLE_SEVERITY_FILTER)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingConsole_2, sev);
        }
        else if(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT )
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingConsole_1);
        }
        EWSWRITEBUFFER(ewsContext, stat);
        break;
      case L7_ADMIN_MODE_DISABLE:
        if( (L7_ADMIN_MODE_DISABLE != FD_LOG_DEFAULT_CONSOLE_ADMIN_STATUS) ||
            (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
        {
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoLoggingConsole);
        }
        break;
      default:
        break;
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) == L7_TRUE)
  {
    /* Persistent*/
    memset(stat, 0, sizeof(stat));
    if (usmDbLogPersistentAdminStatusGet(unit, &adminVal) == L7_SUCCESS &&
        (adminVal != FD_LOG_DEFAULT_PERSISTENT_ADMIN_STATUS || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT ) )
    {
      switch (adminVal)
      {
        case L7_ADMIN_MODE_ENABLE:
          if (( usmDbLogPersistentSeverityFilterGet (unit, &sev) == L7_SUCCESS ) &&
              sev != L7_LOG_SEVERITY_ALERT)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingPersistent_2, sev);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingPersistent_1);
          }
          EWSWRITEBUFFER(ewsContext, stat);
          break;
        case L7_ADMIN_MODE_DISABLE:
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoLoggingPersistent);
          break;
        default:
          break;
      }
    }
  }
  /* Host */
  logIndex = 0;

  while ( usmDbLogHostTableRowStatusGetNext(unit, logIndex, &status, &bufIndex)== L7_SUCCESS )
  {
    logIndex++;

    memset(stat, 0, sizeof(stat));
    if(status ==  L7_ROW_STATUS_ACTIVE)
    {
      if(usmDbLogHostTableIpAddressGet(unit,logIndex, strVal) == L7_SUCCESS)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingHost_1, strVal);

        if(usmDbLogHostTableAddressTypeGet(unit, logIndex, &addrType) == L7_SUCCESS )
        {
          if(addrType == L7_IP_ADDRESS_TYPE_DNS)
          {
            addrTypeStr = "dns";
          }
          else
          {
            addrTypeStr = "ipv4";
          }
          osapiSnprintfcat(stat, sizeof(stat), " %s", addrTypeStr);
        } 

        if(usmDbLogHostTablePortGet(unit, logIndex, &val16) == L7_SUCCESS)
        {
          if(usmDbLogHostTableSeverityGet(unit, logIndex, &val) == L7_SUCCESS )
          {
            cliLogConvertSeverityLevelToString(val, strVal, sizeof(strVal));

            if (val != FD_LOG_DEFAULT_SYSLOG_SEVERITY_FILTER)     /*default value of severity as set in function 'logCfgDefaultsSet' in log_cfg.c*/
            {
              osapiSnprintfcat(stat, sizeof(stat), " %u %s", val16, strVal);
            }
            else if (val16 != FD_LOG_DEFAULT_SERVER_PORT)      /*default value of port as set in function 'logHostAdd' in log_api.c*/
            {
              osapiSnprintfcat(stat, sizeof(stat), " %u", val16);
            }
          }
        }
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }  /* end while */

  /* Syslog */

  if ( usmDbLogSyslogAdminStatusGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdEnable (ewsContext, val, FD_LOG_DEFAULT_SYSLOG_ADMIN_STATUS,pStrInfo_base_LoggingSyslog_1);
  }
  if ((usmDbLogSyslogAdminStatusGet(unit, &val) == L7_SUCCESS ) &&
      (val != FD_LOG_DEFAULT_SYSLOG_ADMIN_STATUS || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT ))
  {
    /* Port */
    if (usmDbLogLocalPortGet(unit, &val16) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext, val16, FD_LOG_DEFAULT_SERVER_PORT, pStrInfo_base_LoggingSyslogPort);
    }
  }

  /* Email Alert */

  if ( usmDbLogEmailAlertStatusGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdEnable (ewsContext, val, FD_EMAIL_ALERT_DEFAULT_STATUS,pStrInfo_base_LoggingEmail_1);
  }

  if ( usmDbLogEmailAlertLogTimeGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext, val, FD_LOG_EMAILALERT_DEFAULT_DURATION,pStrInfo_base_LoggingEmail_logtime_1);
  }

  if ( usmDbLogEmailAlertNonUrgentSeverityGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext, val, FD_EMAIL_ALERT_DEFAULT_NON_URGENT_SEVERITY,pStrInfo_base_LoggingEmail_1);
  }
	
  if ( usmDbLogEmailAlertUrgentSeverityGet(unit, &val) == L7_SUCCESS )
  {
	if(((L7_int32)val) == -1)
	{
	    memset(stat, 0, sizeof(stat));
 		 memcpy(strVal,"none",sizeof("none"));			
	    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingEmail_urgentseverity_2, strVal);
	    EWSWRITEBUFFER(ewsContext, stat);				
	}
	else 
	{
    	cliShowCmdInt(ewsContext, val, FD_EMAIL_ALERT_DEFAULT_URGENT_SEVERITY,pStrInfo_base_LoggingEmail_urgentseverity_1);
	}
  }

  if ( usmDbLogEmailAlertTrapSeverityGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext, val, FD_EMAIL_ALERT_DEFAULT_TRAP_SEVERITY,pStrInfo_base_LoggingEmail_trapseverity_1);
  }

  if ( usmDbLogEmailAlertFromAddrGet(unit, strVal) == L7_SUCCESS )
  {
	if(osapiStrncmp(strVal, FD_EMAIL_ALERT_DEFAULT_FROM_ADDRESS, sizeof(FD_EMAIL_ALERT_DEFAULT_FROM_ADDRESS)) != 0)
	{
	    memset(stat, 0, sizeof(stat));
	    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingEmail_fromaddress_1, strVal);
	    EWSWRITEBUFFER(ewsContext, stat);				
	}
  }
	
	msgType=0;
	memset(address,0, L7_CLI_MAX_STRING_LENGTH);

  while (usmDbLogEmailAlertNextToAddrGet(unit, &msgType, address)== L7_SUCCESS )
  {
	    memset(stat, 0, sizeof(stat));
	    memset(strVal, 0, L7_CLI_MAX_STRING_LENGTH);			

		if(msgType	== L7_LOG_EMAIL_ALERT_URGENT)
		{
			memcpy(strVal,"urgent",sizeof("urgent"));
		}
		else
		{
			memcpy(strVal,"non-urgent",sizeof("non-urgent"));
		}
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingEmail_msgtype_1, strVal);
        osapiSnprintfcat(stat, sizeof(stat), "to-addr %s", address);
        EWSWRITEBUFFER(ewsContext, stat);				
  }

	msgType=0;
	memset(subject,0, L7_CLI_MAX_STRING_LENGTH);

  while (usmDbLogEmailAlertNextSubjectGet(unit, &msgType, subject)== L7_SUCCESS )
  {
       	        memset(stat, 0, sizeof(stat));
		memset(strVal, 0, L7_CLI_MAX_STRING_LENGTH);			

		if(msgType	== L7_LOG_EMAIL_ALERT_URGENT)
		{
			memcpy(strVal,"urgent",sizeof("urgent"));
			if(osapiStrncmp(subject, "Urgent Log Messages", sizeof("Urgent Log Messages")) == 0)
			{
				continue;
			}
		}
		else
		{
			memcpy(strVal,"non-urgent",sizeof("non-urgent"));
			if(osapiStrncmp(subject, "Non Urgent Log Messages", sizeof("Non Urgent Log Messages")) == 0)
			{
				continue;
			}
		}
			 
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingEmail_msgtype_1, strVal);
        osapiSnprintfcat(stat, sizeof(stat), "subject \"%s\"", subject);
        EWSWRITEBUFFER(ewsContext, stat);				
  }

	type = L7_AF_INET;
	memset(mailServerAddr,0,L7_EMAIL_ALERT_SERVER_ADDRESS_SIZE);

  while ( usmDbLogEmailAlertNextMailServerAddressGet(unit, &type, mailServerAddr)== L7_SUCCESS )
  {
      memset(stat, 0, sizeof(stat));
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingEmail_mailserver_1, mailServerAddr);
      EWSWRITEBUFFER(ewsContext, stat);				

	 if(usmDbLogEmailAlertMailServerPortGet(unit, mailServerAddr, &val) == L7_SUCCESS)
	 {
	    cliShowCmdInt(ewsContext, val, FD_EMAIL_ALERT_SMTP_DEFAULT_PORT,pStrInfo_base_LoggingEmail_mailserver_port_1);
	 }

	 if(usmDbLogEmailAlertMailServerSecurityGet(unit, mailServerAddr, &val) == L7_SUCCESS)
	 {

	 	if(val == L7_LOG_EMAIL_ALERT_TLSV1 )
	 	{
			memcpy(strVal,"tlsv1",sizeof("tlsv1"));
	 	}
		else
		{
			memcpy(strVal,"none",sizeof("none"));
		}
	    memset(stat, 0, sizeof(stat));
	    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingEmail_mailserver_security_1, strVal);
	    EWSWRITEBUFFER(ewsContext, stat);				
	 }

	 if(usmDbLogEmailAlertMailServerUsernameGet(unit, mailServerAddr, strVal) == L7_SUCCESS)
	 {
	    memset(stat, 0, sizeof(stat));
	    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingEmail_mailserver_username_1, strVal);
	    EWSWRITEBUFFER(ewsContext, stat);				
	 }
	 
	 if(usmDbLogEmailAlertMailServerPasswdGet(unit, mailServerAddr, strVal) == L7_SUCCESS)
	 {
	    memset(stat, 0, sizeof(stat));
	    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LoggingEmail_mailserver_passwd_1, strVal);
	    EWSWRITEBUFFER(ewsContext, stat);				
	 }

    EWSWRITEBUFFER(ewsContext, "exit");				
	 
  }
	
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  running config information
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
 * @cmdsyntax  show running-config
 *
 * @cmdhelp Display running configuration
 *
 * @cmddescript
 *   This command will show running configuration information.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShowRunningConfig(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index){
  L7_uint32 numArg;

  ewsContext->unbufferedWrite = L7_TRUE;
  numArg = cliNumFunctionArgsGet();

  ewsContext->unbufferedWrite = L7_FALSE;

  return cliShowRunningConfig(ewsContext, argv, numArg, L7_TRUE);

}

#ifdef L7_OUTBOUND_TELNET_PACKAGE
/*********************************************************************
 * @purpose  To print the running configuration of Outbound Telnet Info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t cliRunningConfigOutboundTelnetInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if(usmDbTelnetAdminModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, val,FD_TELNET_DEFAULT_ADMIN_MODE,pStrInfo_base_TransportOutputTelnet);
  }
  if(usmDbTelnetTimeoutGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext, val,FD_TELNET_DEFAULT_TIMEOUT,pStrInfo_base_SessionTimeout_1);
  }
  if(usmDbTelnetMaxSessionsGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext, val,FD_TELNET_DEFAULT_MAX_SESSIONS,pStrInfo_base_SessionLimit);
  }
  return L7_SUCCESS;
}
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
/*********************************************************************
 * @purpose  To print the running configuration of Cli Command Logger
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
L7_RC_t cliRunningCliCmdLoggerInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 mode;

  if ( usmDbCmdLoggerAdminModeGet(&mode) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext, mode,FD_CMD_LOGGER_ADMIN_MODE,pStrInfo_base_LoggingCliCmd);
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of Denial of Service
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

L7_RC_t cliRunningDoSInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_int32 size;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
  {
    if (usmdbDoSFirstFragModeGet(&val) == L7_SUCCESS &&
        usmdbDoSFirstFragSizeGet(&size) == L7_SUCCESS)
    {
      memset (buf, 0,sizeof(buf));
      if (size != FD_DOS_DEFAULT_MINTCPHDR_SIZE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DosCntrlFirstfrag_1,size);
        EWSWRITEBUFFER(ewsContext, buf);
      }
      else
      {
        cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_FIRSTFRAG_MODE,pStrInfo_base_DosCntrlFirstfrag);
      }
    }
  }

  if ((usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV4_FEATURE_ID) != L7_TRUE))
  {
    if (usmdbDoSICMPModeGet(&val) == L7_SUCCESS &&
        usmdbDoSICMPSizeGet(&size) == L7_SUCCESS)
    {
      memset (buf, 0,sizeof(buf));
      if (size != FD_DOS_DEFAULT_MAXICMP_SIZE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DosCntrlIcmp_1,size);
        EWSWRITEBUFFER(ewsContext, buf);
      }
      else
      {
        cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_ICMP_MODE,pStrInfo_base_DosCntrlIcmp);
      }
    }
  }
  else 
  {
     if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV4_FEATURE_ID) == L7_TRUE)
     {
       if (usmdbDoSICMPModeGet(&val) == L7_SUCCESS &&
           usmdbDoSICMPv4SizeGet(&size) == L7_SUCCESS)
       {
         memset (buf, 0,sizeof(buf));
         if (size != FD_DOS_DEFAULT_MAXICMP_SIZE)
         {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DosCntrlIcmpv4_1,size);
           EWSWRITEBUFFER(ewsContext, buf);
         }
         else
         {
           cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_ICMP_MODE,pStrInfo_base_DosCntrlIcmpv4);
         }
       }
     }
    
     if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV6_FEATURE_ID) == L7_TRUE)
     {
       if (usmdbDoSICMPModeGet(&val) == L7_SUCCESS &&
           usmdbDoSICMPv6SizeGet(&size) == L7_SUCCESS)
       {
         memset (buf, 0,sizeof(buf));
         if (size != FD_DOS_DEFAULT_MAXICMP_SIZE)
         {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DosCntrlIcmpv6_1,size);
           EWSWRITEBUFFER(ewsContext, buf);
         }
         else
         {
           cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_ICMP_MODE,pStrInfo_base_DosCntrlIcmpv6);
         }
       }
     }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPFRAG_FEATURE_ID) == L7_TRUE)
  {
    if (usmdbDoSICMPFragModeGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_ICMPFRAG_MODE,pStrInfo_base_DosCntrlIcmpFrag);
    }
  }

  if ((usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_L4PORT_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPPORT_FEATURE_ID) != L7_TRUE))
  {
    if (usmdbDoSL4PortModeGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_L4PORT_MODE,pStrInfo_base_DosCntrlL4port);
    }
  }
  else
  {
    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPPORT_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPPortModeGet(&val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_TCPPORT_MODE,pStrInfo_base_DosCntrlTcpPort);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_UDPPORT_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSUDPPortModeGet(&val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_UDPPORT_MODE,pStrInfo_base_DosCntrlUdpPort);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SIPDIP_FEATURE_ID) == L7_TRUE)
  {
    if (usmdbDoSSIPDIPModeGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_SIPDIP_MODE,pStrInfo_base_DosCntrlSipdip);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SMACDMAC_FEATURE_ID) == L7_TRUE)
  {
    if (usmdbDoSSMACDMACModeGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_SMACDMAC_MODE,pStrInfo_base_DosCntrlSmacdmac);
    }
  }

  if ((usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAG_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) != L7_TRUE))
  {
    if (usmdbDoSTCPFlagModeGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_TCPFLAG_MODE,pStrInfo_base_DosCntrlTcpflag);
    }
  }
  else
  {
    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPFinUrgPshModeGet(&val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_TCPFINURGPSH_MODE,pStrInfo_base_DosCntrlTcpFinUrgPsh);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPFlagSeqModeGet(&val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_TCPFLAGSEQ_MODE,pStrInfo_base_DosCntrlTcpflagseq);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPSYN_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPSynModeGet(&val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_TCPSYN_MODE,pStrInfo_base_DosCntrlTcpSyn);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID) == L7_TRUE)
    {
      if (usmdbDoSTCPSynFinModeGet(&val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_TCPSYNFIN_MODE,pStrInfo_base_DosCntrlTcpSynFin);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFRAG_FEATURE_ID) == L7_TRUE)
  {
    if (usmdbDoSTCPFragModeGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_TCPFRAG_MODE,pStrInfo_base_DosCntrlTcpfrag);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPOFFSET_FEATURE_ID) == L7_TRUE)
  {
    if (usmdbDoSTCPOffsetModeGet(&val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_DOS_DEFAULT_TCPOFFSET_MODE,pStrInfo_base_DosCntrlTcpOffset);
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To capture running configuration
 *
 * @param    EwsContext ewsContext
 * @param    const L7_char8 **argv
 * @param    L7_uint32 numArg
 * @param    boolean chkFile
 *
 * @returns  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @notes
 *
 @end
 *********************************************************************/

static const L7_char8 *cliShowRunningConfigGenerate(EwsContext ewsContext, 
                                                    const L7_char8 * * argv, 
                                                    L7_uint32 numArg, 
                                                    boolean chkFile)
{
  L7_int32 argOne=2;
  L7_char8 cmd[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMode1[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 scrFileName[L7_MAX_FILENAME];
  L7_RC_t rc;
  L7_uint32 val;
  L7_uint32 unit;
  L7_char8 strCurrentCommand[128];
  L7_ConfigScript_t * configScriptData = L7_NULL;
  L7_BOOL saveToFile = L7_FALSE;
  L7_uint32 termLines;

  ewsContext->showRunningDefaultFlag = L7_HIDE_DEFAULT;
  ewsContext->showRunningOutputFlag = L7_WRITE_TERMINAL;
  ewsContext->showRunningSaveFileFlag = L7_NO_SAVE;
  ewsContext->unbufferedWrite = L7_TRUE;
  showRunningConfigAllFlag = 0;

  memset (strMode1, 0,sizeof(strMode1));
  memset (scrFileName, 0,sizeof(scrFileName));
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    ewsContext->unbufferedWrite = L7_FALSE;
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  numArg = cliNumFunctionArgsGet();

  /*No matter wheather it is all command or normal command for alternate command */
  OSAPI_STRNCPY_SAFE(cmd,pStrErr_base_ShowRunningCfgScript_1);

  if ( numArg > 1 )
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowRunningCfg_1);
    ewsContext->unbufferedWrite = L7_FALSE;
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  osapiStrncpySafe(cmd,pStrErr_base_ShowRunningCfgScript_1, sizeof(cmd));
  if ( numArg == 1 )
  {
    if (strlen(argv[argOne]) >= sizeof(strMode1))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowRunningCfg_1);
      ewsContext->unbufferedWrite = L7_FALSE;
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    OSAPI_STRNCPY_SAFE(strMode1, argv[argOne]);
    cliConvertToLowerCase(strMode1);

    if (strcmp(strMode1, pStrInfo_common_All) == 0)
    {
      ewsContext->showRunningDefaultFlag = L7_SHOW_DEFAULT;
      osapiStrncpySafe(cmd,pStrInfo_base_ShowRunningCfgAll_1, sizeof(cmd));

      showRunningConfigAllFlag = 1;

    }
    else /*its script file name*/
    {
      ewsContext->showRunningOutputFlag = L7_WRITE_CONFIG_SCRIPT;
      OSAPI_STRNCPY_SAFE(scrFileName, argv[argOne]);
      saveToFile = L7_TRUE;
    }
  }

  if (ewsContext->showRunningOutputFlag == L7_WRITE_CONFIG_SCRIPT)
  {
    if (chkFile &&
        (checkConfigScriptFilename(scrFileName) != L7_SUCCESS ||
         checkConfigScriptFiletype(scrFileName) != L7_SUCCESS ||
         checkConfigScriptCountLimit(scrFileName) != L7_SUCCESS))
    {
      ewsTelnetWrite( ewsContext, getLastError());
      ewsContext->unbufferedWrite = L7_FALSE;
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    if (chkFile && checkConfigScriptPresent(scrFileName) == L7_SUCCESS)
    {
      if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
      {
        if ( tolower(cliGetCharInput()) != 'y' )
        {
          ewsTelnetWrite(ewsContext, pStrInfo_base_FileCreationCancelled);
          ewsContext->unbufferedWrite = L7_FALSE;
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
      else
      {
        cliSetCharInputID(1, ewsContext, argv);
        osapiSnprintf(strCurrentCommand, sizeof(strCurrentCommand), "%s \"%s\"", pStrErr_base_ShowRunningCfgScript_1, scrFileName);
        cliAlternateCommandSet(strCurrentCommand);
        return pStrInfo_base_CfgScriptAlreadyExistsDoYouWantToOverwriteAndContinueYN;
      }
    }

    configScriptData = osapiMalloc( L7_CONFIG_SCRIPT_COMPONENT_ID, sizeof( L7_ConfigScript_t ) );

    if ( L7_NULL == configScriptData )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_UnableToAllocateStorageForScriptDataStructure);
      cliSyntaxBottom(ewsContext);
      ewsContext->unbufferedWrite = L7_FALSE;
      ewsContext->showRunningOutputFlag = L7_WRITE_TERMINAL;
      return cliPrompt(ewsContext);
    }

    ewsContext->configScriptData =  configScriptData;

    /*Initialise*/
    initialiseScriptBuffer(scrFileName, configScriptData);

    if(getConfigScriptsSize(scrFileName, &val) == L7_SUCCESS)
    {
      configScriptData->size += val;
      val = 0;
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_base_GetSizeOfExistingScripts);
      releaseScriptBuffer(ewsContext->configScriptData);
      osapiFree( L7_CONFIG_SCRIPT_COMPONENT_ID, configScriptData );
      cliSyntaxBottom(ewsContext);
      ewsContext->unbufferedWrite = L7_FALSE;
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if ( CLI_INPUT_EMPTY == cliGetCharInputID() )
    {
      configScriptData = osapiMalloc( L7_CONFIG_SCRIPT_COMPONENT_ID, sizeof( L7_ConfigScript_t ) );

      if ( L7_NULL == configScriptData )
      {
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_UnableToAllocateStorageForScriptDataStructure);
        cliSyntaxBottom(ewsContext);
        ewsContext->unbufferedWrite = L7_FALSE;
        ewsContext->showRunningOutputFlag = L7_WRITE_TERMINAL;
        showRunningConfigAllFlag = 0;
        return cliPrompt(ewsContext);
      }

      ewsContext->runningConfig.showRunCounter = 0;  /* first time it should start from the beginning */
      /* set all modes to false value */
      ewsContext->configScriptData = configScriptData;
      initialiseScriptBuffer("", configScriptData);
    }
  }

  /*
   * Force showRunningOutputFlag to WRITE.  This will cause all the output to
   * be stored in configScriptData instead of written to the terminal
   * immediately.  At the end of this function, if the saveToFile flag is
   * FALSE, then the output will be written to the terminal (using "More" as
   * appropriate).
   */
   if ( (L7_TRUE != saveToFile) && (CLI_INPUT_EMPTY != cliGetCharInputID()) )
   {
     termLines =  cliGetCharInputID();
   }
   else
   {
     usmDbTerminalLineGet(unit, &termLines);
   }
 
  /* Pagination is not enabled,Then we have to display the data directly 
     on to screen instead of buffers. So that we can save the time */
  if((saveToFile == L7_FALSE) && (termLines == 0))
  {
    ewsContext->showRunningOutputFlag = L7_WRITE_TERMINAL;
    /* We are directly displaying output to the terminal.So we 
       dont require any buffers */   
    releaseScriptBuffer(configScriptData);
    osapiFree( L7_CONFIG_SCRIPT_COMPONENT_ID, configScriptData );
  }
  else
  {
    ewsContext->showRunningOutputFlag = L7_WRITE_CONFIG_SCRIPT;
    ewsContext->showRunningSaveFileFlag = (saveToFile == L7_TRUE)?L7_SAVE:L7_NO_SAVE;
  }

  if ( ( L7_TRUE         == saveToFile ) ||
      ( CLI_INPUT_EMPTY == cliGetCharInputID() ) )
  {
    rc = cliShowRunningConfigExecute(ewsContext);
  }

  if ( L7_TRUE == saveToFile )
  {
    if (ewsContext->showRunningOutputFlag == L7_WRITE_CONFIG_SCRIPT)
    {
      if (writeConfigScriptData(configScriptData) != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, getLastError());
        releaseScriptBuffer(configScriptData);
        osapiFree( L7_CONFIG_SCRIPT_COMPONENT_ID, configScriptData );
        ewsContext->unbufferedWrite = L7_FALSE;
        return cliPrompt(ewsContext);
      }

      releaseScriptBuffer(configScriptData);
      osapiFree( L7_CONFIG_SCRIPT_COMPONENT_ID, configScriptData );
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CfgScriptCreatedSuccessfully);
    }
  }
  else
  {
   /* We have to go inside when we rquired paginatiion mechanism */ 
   if (ewsContext->showRunningOutputFlag == L7_WRITE_CONFIG_SCRIPT)
   {
    L7_ConfigScript_t * currentScript = ewsContext->configScriptData;
    L7_uchar8 * scriptCommand;
    L7_uint32 count = 0;
    L7_char8 strFormatedCommand[CONFIG_SCRIPT_MAX_COMMAND_SIZE];
    L7_BOOL printCmd = L7_TRUE;
    L7_BOOL cmdWireless = L7_FALSE;
    L7_BOOL cmdCP = L7_FALSE;

    if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
    {                                                                     /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        releaseScriptBuffer(currentScript) ;
        osapiFree( L7_CONFIG_SCRIPT_COMPONENT_ID, currentScript );
        ewsContext->unbufferedWrite = L7_FALSE;
        ewsContext->showRunningOutputFlag = L7_WRITE_TERMINAL;
        showRunningConfigAllFlag = 0;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
    /* Get the number of lines for page. if it is 0, pagination not required */
    cliSyntaxBottom( ewsContext );

    while(getConfigScriptCommand((ewsContext->runningConfig.showRunCounter) + 1, currentScript,  &scriptCommand) == L7_SUCCESS)
    {
      osapiSnprintf(strFormatedCommand, sizeof(strFormatedCommand), scriptCommand);

      scriptCommand = trimLeft( scriptCommand );
      if ( 0 == strlen( scriptCommand ) )
      {
       (ewsContext->runningConfig.showRunCounter)++;
        continue;
      }

      if ( count > 0 )
      {
        cliSyntaxBottom( ewsContext );
      }

     #ifdef L7_WIRELESS_PACKAGE
      cmdWireless = cliRunningConfigWirelessIsCmdAPPwd(strFormatedCommand);
     #endif

     #ifdef L7_CAPTIVE_PORTAL_PACKAGE
      cmdCP = cliRunningConfigCPIsCmdUserPwd(strFormatedCommand);
     #endif

      if ((cmdWireless == L7_TRUE) || (cmdCP == L7_TRUE))
      {
        if (count == (termLines-1))
        {
          (ewsContext->runningConfig.showRunCounter)--;
          printCmd = L7_FALSE;
        }
        else
        {
          count++;
        }
      }

      if (printCmd == L7_TRUE)
      {
        ewsTelnetWrite(ewsContext, strFormatedCommand);
      }

      (ewsContext->runningConfig.showRunCounter)++;

      count++;
      if(termLines != 0)
      {
        if(count == termLines)
        {
          cliSetCharInputID(termLines, ewsContext, argv);
          cliAlternateCommandSet(cmd);
          cliSyntaxBottom(ewsContext);
          return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
        }
      }
    }
    releaseScriptBuffer( currentScript );
    osapiFree( L7_CONFIG_SCRIPT_COMPONENT_ID, currentScript );
   } 
  }

  ewsContext->showRunningOutputFlag = L7_WRITE_TERMINAL;

  cliSyntaxBottom(ewsContext);
  ewsContext->unbufferedWrite = L7_FALSE;
  showRunningConfigAllFlag = 0;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  To capture running configuration
 *
 * @param    EwsContext ewsContext
 * @param    const L7_char8 **argv
 * @param    L7_uint32 numArg
 * @param    boolean chkFile
 *
 * @returns  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @notes
 *
 @end
 *********************************************************************/

const L7_char8 *cliShowRunningConfig(EwsContext ewsContext, 
                                     const L7_char8 * * argv, 
                                     L7_uint32 numArg, boolean chkFile)
{

  const L7_char8 *prompt;

  if (osapiSemaTake(cliRunCfgSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CLI_WEB_COMPONENT_ID,
           "Failed to take cliRunCfgSemaphore %x\n",
            cliRunCfgSema);
  }

  prompt = cliShowRunningConfigGenerate(ewsContext, argv, numArg, chkFile);

  if (osapiSemaGive(cliRunCfgSema) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CLI_WEB_COMPONENT_ID,
            "Failed to give cliRunCfgSema %x\n",
            cliRunCfgSema);
  }

  return prompt;
}

/*********************************************************************
 * @purpose  Execute running configuration
 *
 * @param    EwsContext ewsContext
 * @param    const L7_char8 **argv
 * @param    L7_uint32 numArg
 * @param    boolean chkFile
 *
 * @returns  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @notes
 *
 *  @end
 *********************************************************************/
L7_RC_t cliShowRunningConfigExecute(EwsContext ewsContext)
{
  L7_uint32 interface = 1, nextInterface;
  L7_int32 u=1;
  L7_int32 s, p;
  L7_uchar8 strUserName[L7_LOGIN_SIZE];
  L7_uchar8 strUserNamePrev[L7_LOGIN_SIZE];
  L7_RC_t rc, rc1;
  L7_uint32 val;
  L7_uint32 sysIntfType;

  L7_int32 retVal;
  L7_uint32 intf, itype;
  L7_uint32 i, unitNum;
  L7_uint32 usrIndex;
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
  L7_uint32 lbid;
  L7_uint32 ptunnelId=0;
#endif
#endif

  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_INTF_MASK_t protectedPortMask;
#ifdef L7_WIRELESS_PACKAGE
  L7_uchar8 val8 = 0;
  L7_char8 data[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 name[L7_MAX_FILENAME+1];
  L7_IP_ADDR_t addr = 0;
  L7_char8 path[L7_MAX_FILEPATH+1];
  L7_uint32 imageID = 0;
#endif /* L7_WIRELESS_PACKAGE */

  L7_uint32 unit =cliGetUnitId();

  EWSWRITEBUFFER_MAIN(ewsContext, pStrInfo_common_CrLf);
  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CurrentCfg);
  EWSWRITEBUFFER_MAIN(ewsContext, "\r\n!");
  EWSWRITEBUFFER_MAIN(ewsContext, pStrInfo_common_CrLf);


  /* System description information */
  cliRunningConfigSysInfo(ewsContext, unit);

  cliRunningConfigPrompt(ewsContext, unit);
  cliRunningConfigGarpInfo(ewsContext, unit);
  cliRunningTelnetInfo(ewsContext, unit);

  cliRunningConfigEnable(ewsContext);

  /**ServicePort Showrunning configuration*****/
  if ( usmDbComponentPresentCheck(unit, L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE )
  {
    cliRunningConfigServicePortInfo(ewsContext, unit);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    cliRunningConfigServicePortIPV6Info(ewsContext, unit);
#endif
  }

  cliRunningConfigNetworkInfo(ewsContext, unit);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  cliRunningConfigNetworkIPV6Info(ewsContext, unit);
#endif
#ifdef L7_AUTO_INSTALL_PACKAGE
  cliRunningConfigAutoInstallInfo(ewsContext, unit);
#endif

#ifdef L7_STACKING_PACKAGE
if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_STACKING_COMPONENT_ID,
                             L7_STACKING_SFS_FEATURE_ID) == L7_TRUE)
{
  cliRunningConfigAutoCopySw(ewsContext);
}
#endif

#ifdef L7_PBVLAN_PACKAGE
  /*Protocol Config information should be displayed before Vlan info. Hence this is not included with the configure group displayed after vlan info*/
  cliRunningConfigProtocolInfo(ewsContext, unit);
#endif
  /* non interface specific vlan */
  cliRunningConfigVlanInfo(ewsContext, unit);

  if (usmDbFeaturePresentCheck(unit, L7_SIM_COMPONENT_ID, L7_SIM_NETWORK_PORT_FEATURE_ID))
  {
    /* Network Mgmt VLAN */
    if (usmDbMgmtVlanIdGet(unit,&val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_SIM_DEFAULT_MGMT_VLAN_ID,pStrInfo_base_NwMgmtVlan_1);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
  cliRunningConfigHttpServerInfo(ewsContext, unit);
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_WEB_FEATURE_SUPPORTED) == L7_TRUE)
  {
  cliRunningConfigHttpSecureServerInfo(ewsContext, unit);
  }
  cliRunningConfigSshInfo (ewsContext, unit);
#endif /* L7_MGMT_SECURITY_PACKAGE */
  cliRunningTelnetEnableInfo(ewsContext, unit);
  EWSWRITEBUFFER_MAIN(ewsContext,pStrInfo_common_CrLf);

#ifdef L7_WIRELESS_PACKAGE
  while(usmDbWdmAPImageTypeIDNextGet(imageID, &imageID) == L7_SUCCESS)
  {
    if (usmDbWdmAPCodeDownloadFileNameGet(imageID,name) == L7_SUCCESS &&
        usmDbWdmAPCodeDownloadServerGet(&addr) == L7_SUCCESS &&
        usmDbWdmAPCodeDownloadFilePathGet(imageID,path) == L7_SUCCESS)
    {
      if ((EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) ||
          (addr != 0 && strlen(name) != 0))
      {
        if (usmDbInetNtoa(addr, data) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, L7_CLI_MAX_STRING_LENGTH,
              pStrInfo_base_WsApDownloadTftp, imageID,data, path, name);
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    }
  }/* End while */
  memset(buf,0,sizeof(buf));
  if (usmDbWdmAPCodeDownloadGroupSizeGet(&val8) == L7_SUCCESS)
  {
    if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT ||
        val8 != FD_WDM_AP_CODE_DOWNLOAD_GROUP_SIZE)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, L7_CLI_MAX_STRING_LENGTH, pStrInfo_base_GrpSize_2, val8);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

#endif /* L7_WIRELESS_PACKAGE */

  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_Cfgure_1);

  
  /*Displays DHCP options configured on the DHCP client.
   These options are used while getting IP address for the device.*/
  cliRunningConfigDhcpClientInfo(ewsContext, unit);

  if ( usmDbComponentPresentCheck(unit, L7_SNTP_COMPONENT_ID) == L7_TRUE )
  {
    cliRunningSNTPInfo(ewsContext, unit);
  }

  /* summer time */
  cliRunningConfigSummerTime (ewsContext, unit);

  /* timezone*/
  cliRunningConfigClockTimeZone (ewsContext, unit);

#ifdef L7_TIMERANGES_PACKAGE
  cliRunningConfigTimeRangeInfo(ewsContext, unit);
#endif

#ifdef L7_STACKING_PACKAGE
#ifndef L7_CHASSIS
  /*stacking related*/
  cliRunningConfigStackingInfo(ewsContext);
#endif
#endif /* L7_STACKING_PACKAGE */

  /* Denial of Service */
  if (usmDbComponentPresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID) == L7_TRUE)
  {
    cliRunningDoSInfo(ewsContext,unit);
  }

  cliRunningConfigDnsClientInfo(ewsContext, unit);

  if ( usmDbComponentPresentCheck(unit, L7_LOG_COMPONENT_ID) == L7_TRUE )
  {
    cliRunningSyslogInfo(ewsContext, unit);
  }

  /* CLI Logging info */
  cliRunningCliCmdLoggerInfo(ewsContext, unit);

#ifdef L7_STACKING_PACKAGE
  /*slot related*/
#ifndef L7_CHASSIS
  cliRunningConfigSlotInfo(ewsContext);
#else
  cliRunningConfigChassisSlotInfo(ewsContext);
#endif
#endif /* L7_STACKING_PACKAGE */

  /* mtu config info */
  if ((usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID,
          L7_NIM_JUMBOFRAMES_FEATURE_ID) == L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_NIM_COMPONENT_ID,
                                L7_NIM_JUMBOFRAMES_PER_INTERFACE_FEATURE_ID) == L7_FALSE))
  {
    /* get configured mtu value of any interface
     * as in global-config-mode this value is configured for all interfaces
     */
    rc = usmDbValidIntIfNumFirstGet(&intf);
    while (rc == L7_SUCCESS)
    {
      if (usmDbIntfParmCanSet(intf, L7_INTF_PARM_MTU) == L7_TRUE &&
          usmDbIfConfigMaxFrameSizeGet(intf, &val) == L7_SUCCESS)
      {
        cliShowCmdInt (ewsContext, val, FD_NIM_DEFAULT_MAX_FRAME_SIZE, pStrInfo_common_Mtu);
      }
      rc = usmDbValidIntIfNumNext(intf, &intf);
    }
  }

  cliRunningConfigSnmpTrapInfo(ewsContext, unit);

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
     cliRunningConfigIpv6Hosts(ewsContext, unit);
#endif /* L7_IPV6_PACKAGE or L7_IPV6_MGMT_PACKAGE*/

#ifdef L7_IPV6_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_FLEX_OSPFV3_MAP_COMPONENT_ID) == L7_TRUE)
  {
    cliRunningConfigOspfv3Info(ewsContext, unit);
  }
  cliRunningConfigIPv6Global(ewsContext, unit);
  cliRunningConfigIPv6DhcpGlobal(ewsContext, unit);
  cliRunningConfigIpv6RouteInfo(ewsContext, unit);
#ifdef L7_MCAST_PACKAGE
  cliRunningConfigIpv6McastInfo(ewsContext, unit);
#endif
#endif

#ifdef L7_ROUTING_PACKAGE
  cliRunningConfigIntfIpRoute(ewsContext, unit);
  cliRunningConfigIpRoute(ewsContext, unit);
  cliRunningConfigArpInfo(ewsContext, unit);
  cliRunningConfigBootpDhcpRelayInfo(ewsContext, unit);
  cliRunningConfigUdpRelayInfo(ewsContext, unit);
  cliRunningConfigInternalVlan(ewsContext, unit);
#endif /* end if routing package included */

  cliRunningConfigPwdMgmtInfo(ewsContext, unit);
  cliRunningConfigUsersSnmpv3Info(ewsContext, unit);
  cliRunningConfigAuthentication(ewsContext, unit);
  cliRunningConfigUsersLoginInfo(ewsContext, unit);
  cliRunningConfigDot1xInfo(ewsContext, unit);
  cliRunningConfigVoiceVlanInfo(ewsContext,unit);
#ifdef L7_POE_PACKAGE
  cliRunningConfigGlobalPoeInfo(ewsContext, unit);
#endif
  /* Get the login user name.
   * For all interfaces, if this user has access to an interface, continue,
   * else, display the `no` form of the command for this interface.
   */
  rc = usmDbValidIntIfNumFirstGet(&intf);
  while (rc == L7_SUCCESS)
  {
    memset (strUserName, 0, sizeof(strUserName));
    rc = usmDbPortUserGetFirst(unit, intf, &usrIndex, strUserName, sizeof(strUserName));
    i = 0;
    while (i < L7_MAX_LOGINS)
    {
      memset (buf, 0, sizeof(buf));
      (void)cliWebLoginUserNameGet(i, buf);
      /* skip index if blank, indicating user at that index is deleted */
      if (strcmp(buf, FD_CLI_WEB_DEFAULT_LOGIN_NAME) == 0)
      {
        i++;
        continue;
      }

      if (strcmp(buf, strUserName) != 0)
      {
        if((usmDbUnitSlotPortGet(intf, &unitNum, &s, &p) == L7_SUCCESS) && unitNum != FD_NIM_DEFAULT_UNIT_NUM)
        {
          if ((usmDbIntfTypeGet(intf, &itype) == L7_SUCCESS) &&
              (itype == L7_PHYSICAL_INTF))
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoDot1xUsr, (L7_char8 *)buf, cliDisplayInterfaceHelp(unitNum,s,p)) ;
            EWSWRITEBUFFER_MAIN(ewsContext,stat);
          }
        }
      }
      else
      {
        OSAPI_STRNCPY_SAFE(strUserNamePrev, strUserName);
        memset (strUserName, 0, sizeof(strUserName));
        rc = usmDbPortUserGetNext(unit, intf, &usrIndex, strUserName, sizeof(strUserName));
      }
      i++;
    }
    rc = usmDbValidIntIfNumNext(intf, &intf);
  }
#ifdef L7_SFLOW_PACKAGE
  if (usmDbComponentPresentCheck(L7_NULL, L7_SFLOW_COMPONENT_ID))
  {
    cliRunningConfigSflowInfo(ewsContext, unit);
  }
#endif
  cliRunningConfigRadiusInfo(ewsContext, unit);
  

  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LineConsoleconfig);
  cliRunningConfigSerialInfo(ewsContext, unit);
  cliRunningConfigLineInfo(ewsContext, ACCESS_LINE_CONSOLE);
  cliRunningConfigTelnetInfo(ewsContext, unit);
 #ifdef L7_OUTBOUND_TELNET_PACKAGE
  cliRunningConfigOutboundTelnetInfo(ewsContext, unit);
 #endif /* L7_OUTBOUND_TELNET_PACKAGE */
  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Exit); /*exit from lineconfig*/

  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LineTelnetconfig);
  cliRunningConfigLineInfo(ewsContext, ACCESS_LINE_TELNET);
  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Exit); /*exit from lineconfig*/

  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LineSshconfig);
  cliRunningConfigLineInfo(ewsContext, ACCESS_LINE_SSH);
  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Exit); /*exit from lineconfig*/

  cliRunningConfigIntAuthServ(ewsContext, unit);

  cliRunningStormControlInfo(ewsContext, unit);
  cliRunningSpanningTreeInfo(ewsContext, unit);
  cliRunningConfigDot3Info(ewsContext, unit);
  

  if (usmDbFeaturePresentCheck( unit, L7_SNMP_COMPONENT_ID,
        L7_SNMP_USER_SUPPORTED) == L7_TRUE )
  {
    cliRunningConfigSysDesc(ewsContext, unit);
    cliRunningConfigSnmpInfo(ewsContext, unit);
  }

  if ( usmDbFeaturePresentCheck(unit, L7_FDB_COMPONENT_ID,
        L7_FDB_FEATURE_ID ) == L7_TRUE  )
  {
    cliRunningConfigForwardingDbInfo(ewsContext, unit);
  }

  if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
        L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
  {
    for (i = 0; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
    {
      memset (buf, 0, sizeof(buf));
      if(( usmdbProtectedPortGroupNameGet(unit,i,buf) ==  L7_SUCCESS ))
      {
        if((strcmp(buf, FD_PROTECTED_PORT_GROUP_NAME) != 0)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
        {
          if(strcmp(buf, FD_PROTECTED_PORT_GROUP_NAME) != 0)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SwitchportProtectedName, i,buf);
            EWSWRITEBUFFER(ewsContext, stat);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoSwitchportProtected_1,i);
            EWSWRITEBUFFER(ewsContext, stat);
          }

        }
      }
    }
  }
  else
  {
    memset (buf, 0, sizeof(buf));
    if(( usmdbProtectedPortGroupNameGet(unit,0,buf) ==  L7_SUCCESS ))
    {
      if((strcmp(buf, FD_PROTECTED_PORT_GROUP_NAME) != 0)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        if(strcmp(buf, FD_PROTECTED_PORT_GROUP_NAME) != 0)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SwitchportProtectedName_1,buf);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoSwitchportProtected);
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }
  }

#ifdef L7_QOS_PACKAGE

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_FEATURE_SUPPORTED) == L7_TRUE)
  {
    cliRunningConfigAclInfo(ewsContext, unit);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
  {
    cliRunningConfigIpv6AclInfo(ewsContext, unit);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_MAC_FEATURE_ID) == L7_TRUE )
  {
    cliRunningConfigMacAclInfo(ewsContext, unit);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE )
  {
    cliRunningVlanAclInfo(ewsContext, unit);
  }
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
        L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE)
  {
    cliRunningConfigDiffservInfo(ewsContext, unit);
  }
#endif

#endif

#ifdef L7_CHASSIS
  cliRunningConfigChassisInfo(ewsContext);
#endif
  cliRunningConfigTrapflagInfo(ewsContext, unit);

#ifdef L7_STATIC_FILTERING_PACKAGE
  cliRunningConfigMacfilterInfo(ewsContext, unit);
#endif

  if (usmDbComponentPresentCheck(unit, L7_SNOOPING_COMPONENT_ID) == L7_TRUE)
  {
    cliRunningConfigIgmpSnoopingInfo(ewsContext, unit);
  }
#ifdef L7_DHCP_SNOOPING_PACKAGE
  cliRunningConfigDhcpSnoopingInfo(ewsContext, unit);
#ifdef L7_IPSG_PACKAGE
  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) == L7_TRUE)
  {
    cliRunningConfigIpsgInfo(ewsContext, unit);
  }
#endif
#endif

#ifdef L7_DAI_PACKAGE  
  cliRunningConfigDaiInfo(ewsContext, unit);
  cliRunningConfigArpAclInfo(ewsContext);
#endif

#ifdef L7_TR069_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_TR069_COMPONENT_ID) == L7_TRUE)
  {
   cliRunningConfigTr069(ewsContext); 
  }
#endif

#ifdef L7_IP_MCAST_PACKAGE

#ifdef L7_MCAST_PACKAGE
  cliRunningConfigMiscInfo(ewsContext, unit);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_MCAST_MAP_COMPONENT_ID) == L7_TRUE)
  {
    cliRunningConfigMcastInfo(ewsContext, unit);
  }

#endif
#endif


  /* non interface specific classofservice dot1p-mapping */
  cliRunningConfigClassofserviceInfo(ewsContext, unit);

#ifdef L7_QOS_FLEX_PACKAGE_COS
  cliRunningConfigGlobalClassofService(ewsContext, unit);

  cliRunningConfigGlobalRandomDetect(ewsContext, unit);
  cliRunningConfigGlobalTailDrop(ewsContext, unit);
  cliRunningConfigGlobalCosQueue(ewsContext, unit);
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
  cliRunningGlobalConfigIscsiInfo(ewsContext);
#endif

  if (usmDbComponentPresentCheck(unit, L7_PORT_MACLOCK_COMPONENT_ID) == L7_TRUE)
  {
    cliRunningConfigPmlGlobalInfo(ewsContext, unit);
  }

#ifdef L7_WIRELESS_PACKAGE
  pStringBufAddBlanks(1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Ws_3);
  EWS_WRITE_MODE_START(ewsContext, buf);

  /* mutual-authentication-mode */
  if (usmDbWdmNetworkMutualAuthModeGet (&val) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext, val, FD_WDM_DEFAULT_WS_NETWORK_MUTUAL_AUTHENTICATION_MODE, "mutual-auth-mode");
  }

  cliRunningConfigWirelessInfo(ewsContext);
  pStringBufAddBlanks(1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Exit);
  EWS_WRITE_MODE_EXIT(ewsContext, buf);
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  cliRunningConfigCPInfo(ewsContext);
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
   if (usmDbComponentPresentCheck(L7_NULL, L7_FLEX_METRO_DOT1AD_COMPONENT_ID))
  {
    cliRunningConfigDot1adMode(ewsContext);
    /* dot1ad service config */
    cliRunningConfigdot1adService(ewsContext);
    cliRunningConfigdot1adL2ptInfo(ewsContext);
  }
#endif
#ifdef L7_DOT1AG_PACKAGE
   /* dot1ag global mode running-config */
   if (usmDbComponentPresentCheck(L7_NULL, L7_DOT1AG_COMPONENT_ID) == L7_TRUE)
   {
     cliRunningConfigDot1agGlobalInfo(ewsContext);
   }
#endif
#endif

#ifdef L7_DVLAN_PACKAGE
  cliRunningConfigGlobalDVlanInfo(ewsContext, unit);
#endif
  /* show loopback interfaces before all other interfaces */
  if (usmDbValidIntIfNumFirstGet(&interface) == L7_SUCCESS)
  {
    while (interface != 0)
    {
      if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS)
      {
        if(((usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS) && (cliSlotPortCpuCheck(s, p) == L7_SUCCESS)))
        {
          /*enter into interface mode*/
          EWSWRITEBUFFER_MAIN(ewsContext, pStrInfo_common_CrLf);

          rc1 = usmDbIntfTypeGet(interface, &sysIntfType);

          /* Might need to check for other interface type as well */
          if (rc1 == L7_SUCCESS)
          {
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
            if(usmDbRlimLoopbackIdGet(interface, &lbid)==L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_IntfLoopback, lbid);
              EWSWRITEBUFFER_MAIN(ewsContext, stat);
            }
            else if(usmDbRlimTunnelIdGet(interface, &ptunnelId) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_IntfTunnel, ptunnelId);
              EWSWRITEBUFFER_MAIN(ewsContext, stat);
            }
            else
#endif
#endif
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Intf_4, cliDisplayInterfaceHelp(u,s,p));
              EWS_WRITE_MODE_START(ewsContext, stat);
            }
            cliRunningPhysicalInterfaceInfo(ewsContext, unit, interface);
          }

          if(usmdbProtectedPortIntfIsValid(unit,interface) == L7_SUCCESS)
          {
            if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                  L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
            {
              /* get protected port list */
              for (i = 0; i < L7_PROTECTED_PORT_MAX_GROUPS; i++)
              {
                memset(&protectedPortMask, i, sizeof(protectedPortMask));
                if(usmdbProtectedPortGroupIntfMaskGet(unit, i, &protectedPortMask) == L7_SUCCESS)
                {
                  if(L7_INTF_ISMASKBITSET(protectedPortMask, interface))
                  {
                    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_ProtectedPort_2, i);
                    EWSWRITEBUFFER_MAIN(ewsContext, stat);
                  }
                  else if (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
                  {
                    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoSwitchportProtected_1,i);
                    EWSWRITEBUFFER_MAIN(ewsContext, stat);
                  }
                }
              }
            }
            else
            {
              memset(&protectedPortMask, 0, sizeof(protectedPortMask));
              if(usmdbProtectedPortGroupIntfMaskGet(unit, 0, &protectedPortMask) == L7_SUCCESS)
              {
                if( (L7_INTF_ISMASKBITSET(protectedPortMask, interface)) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
                {
                  if(L7_INTF_ISMASKBITSET(protectedPortMask, interface))
                  {
                    EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_SwitchportProtected);
                  }
                  else
                  {
                    EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_NoSwitchportProtected);
                  }
                }
              }
            }
          }
 
#ifdef L7_DVLAN_PACKAGE
          /* DVlan specific config */
          if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE &&
              usmDbDot1qIsValidIntf(unit, interface) == L7_TRUE)
          {
            cliRunningConfigDVlanInfo(ewsContext, unit, interface);
          }
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
          if (usmDbQosAclIsValidIntf(unit, interface) == L7_TRUE)
          {
            cliRunningInterfaceAclInfo(ewsContext, unit, interface);
          }
#endif

#ifdef L7_ROUTING_PACKAGE
          cliRunningVisibleRoutingInterfaceInfo(ewsContext, unit, interface);
#endif

          cliRunningInterfaceConfigLACPInfo(ewsContext, unit, interface);
          
          cliRunningInterfaceConfigLLDPInfo(ewsContext, interface);
#ifdef L7_ISDP_PACKAGE
          cliRunningInterfaceConfigIsdpInfo(ewsContext, interface);
#endif          

#ifdef L7_LLPF_PACKAGE
          cliRunningInterfaceConfigLlpfInfo(ewsContext, interface);
#endif          
#ifdef L7_POE_PACKAGE
          cliRunningInterfacePoeInfo(ewsContext, unit, interface);
#endif

#ifdef L7_PFC_PACKAGE
          cliRunningConfigInterfacePfcInfo(ewsContext,interface);
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
          if((usmDbRlimLoopbackIdGet(interface, &lbid)==L7_SUCCESS) ||
              (usmDbRlimTunnelIdGet(interface, &ptunnelId) == L7_SUCCESS) )
          {
            EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_Exit);
          }
          else
#endif
#endif
          {
            osapiSnprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, buf, sizeof (buf), pStrInfo_common_Exit);
            EWS_WRITE_MODE_EXIT(ewsContext, buf);
          }
        }
      }
      if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
      {
        interface = nextInterface;
      }
      else
      {
        interface = 0;
      }
    }
  }
  

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RIP_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_RIP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    cliRunningConfigRIPInfo(ewsContext, unit);
  }
#endif
#ifdef L7_OSPF_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_OSPF_MAP_COMPONENT_ID) == L7_TRUE)
  {
    cliRunningConfigOspfInfo(ewsContext, unit);
  }
#endif
#ifdef L7_VRRP_PACKAGE
  cliRunningConfigVrrpInfo(ewsContext, unit);
#endif
#endif

#ifdef L7_BGP_PACKAGE
  if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    cliRunningConfigBgpInfo(ewsContext, unit);
  }
#endif

#ifdef L7_DHCPS_PACKAGE
  cliRunningConfigDhcps(ewsContext, unit);
#endif

  cliRunningGlobalConfigTACACSInfo(ewsContext, unit);
  cliRunningConfigLLDPInfo(ewsContext);
#ifdef L7_ISDP_PACKAGE  
  cliRunningConfigIsdpInfo(ewsContext);
#endif /* L7_ISDP_PACKAGE */ 
  cliRunningConfigMirroringInfo(ewsContext, unit);
  cliRunningConfigCpuMonitorInfo(ewsContext, unit);

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  /* 
   * Output encoded data at the end of the configuration
   * if writing to script or showing all.
   */
  if ( ( ewsContext->showRunningSaveFileFlag == L7_SAVE ) ||
       ( L7_SHOW_DEFAULT == EWSSHOWALL(ewsContext) ) )
  {
    cliRunningEncodedConfigExecute(ewsContext); 
  }
#endif


  EWSWRITEBUFFER_MAIN_ADD_BLANKS (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Exit); /*exit from config mode*/

  cliRunningConfigPaginationInfo(ewsContext, unit);
  /*cliRunningConfigDebuggingInfo(ewsContext);*/
  EWSWRITEBUFFER_MAIN(ewsContext,pStrInfo_common_CrLf);

  return L7_SUCCESS;

}

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
/*********************************************************************
 * @purpose  Execute running configuration for encoded binary images
 *           and Unicode. Please note that this encoded section is
 *           added to the END of the running and config scripts.
 *
 * @param    EwsContext ewsContext
 *
 * @returns  const L7_char8 *
 * @returns  cliPrompt(ewsContext)
 *
 * @notes
 *
 * @end
 *********************************************************************/
const L7_uchar8 *cliRunningEncodedConfigExecute(EwsContext ewsContext)
{
  L7_ConfigScript_t *oldScriptData;
  L7_ConfigScript_t  cpScriptData;
  L7_uchar8         *command;
  L7_BOOL            commandsFound;
  L7_uint32          idx;

  oldScriptData = ewsContext->configScriptData;
  ewsContext->configScriptData = &cpScriptData;
  initialiseScriptBuffer("", &cpScriptData);

  cliRunningEncodedConfig(ewsContext);

  ewsContext->configScriptData = oldScriptData;

  if ( ( L7_NULL != cpScriptData.cfgData ) &&
       ( 0 < cpScriptData.cfgData->noOfCmds ) )
  {
    /*
     * Loop through the generated commands and ensure they are not all blank.
     */
    commandsFound = L7_FALSE;
    idx = 1;
    while ( L7_SUCCESS == getConfigScriptCommand( idx++, &cpScriptData, &command ) )
    {
      command = trimLeft( command );
      if ( 0 < strlen( command ) )
      {
        commandsFound = L7_TRUE;
        break;
      }
    }

    if ( L7_TRUE == commandsFound )
    {
      /*
       * Loop through the generated commands and output them.
       */
      idx = 1;
      while ( L7_SUCCESS == getConfigScriptCommand( idx++, &cpScriptData, &command ) )
      {
        command = trimLeft( command );
        if ( 0 == strlen( command ) )
        {
          continue;
        }
        EWSWRITEBUFFER_MAIN(ewsContext, command);
      }
    }
  }

  releaseScriptBuffer( &cpScriptData );

  return cliPrompt(ewsContext);
}
#endif

/*********************************************************************
 * @purpose  To print the running configuration of Pagination for show running-config
 *
 * @param    EwsContext  ewsContext    @b((input))
 * @param    L7_uint32   unit          @b((input))   unit number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t cliRunningConfigPaginationInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 termLines;

  memset (buf, 0,sizeof(buf));
  if (usmDbTerminalLineGet(unit, &termLines) == L7_SUCCESS)
  {
    if ((termLines != FD_CLI_LINES_FOR_PAGINATION) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      if (termLines != FD_CLI_LINES_FOR_PAGINATION)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_TerminalLen_1, termLines);
        EWSWRITEBUFFER(ewsContext, buf);
      }
      else
      {
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoTerminalLen);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of DNS Client
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 *  @end
 *********************************************************************/
L7_RC_t cliRunningConfigDnsClientInfo(EwsContext ewsContext, 
    L7_uint32 unitIndex)
{
  L7_char8  hostname[L7_DNS_DOMAIN_NAME_SIZE_MAX + 1];
  L7_char8  domain[L7_DNS_DOMAIN_NAME_SIZE_MAX + 1];
  L7_BOOL   enable = L7_FALSE;
  L7_char8  stat[L7_SYS_DISPLAY_SIZE + L7_DNS_DOMAIN_NAME_SIZE_MAX];
  L7_char8  strAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 nameserver =0, numhost=0;
  L7_inet_addr_t serverAddr;
  L7_inet_addr_t inetAddress;
  L7_RC_t   rc = L7_FAILURE;
  L7_ushort16 timeout = L7_NULL;
  L7_uint32 retries = L7_NULL;
  L7_BOOL serverFound = L7_FALSE;

  if (usmDbDNSClientAdminModeGet(&enable) == L7_SUCCESS)
  {
    if ((enable != FD_DNS_CLIENT_ADMIN_MODE_ENABLED ) || EWSSHOWALL(ewsContext)
        == L7_SHOW_DEFAULT)
    {
      sprintf(stat, "\r\nno ip domain lookup");
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }
  if (usmDbDNSClientQueryTimeoutGet(&timeout) == L7_SUCCESS)
  {
    if ((timeout != FD_DNS_CLIENT_QUERY_TIMEOUT) || 
        EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      sprintf(stat, "\r\nip domain timeout %d", timeout);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }
  if (usmDbDNSClientRetransmitsGet(&retries) == L7_SUCCESS)
  {
    if ((retries != FD_DNS_NO_OF_RETRIES) || 
        EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      sprintf(stat, "\r\nip domain retry %d", retries);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }

  memset(domain, L7_NULL, sizeof(domain));
  rc = usmDbDNSClientDefaultDomainGet(domain);
  if ( ( L7_SUCCESS == rc ) &&
      ( strlen( domain ) > 0 ) )
  {
    sprintf(stat, "\r\nip domain name \"%s\"", domain);
    EWSWRITEBUFFER(ewsContext, stat);
  }

  memset(domain, L7_NULL, sizeof(domain));
  memset(stat, L7_NULL, sizeof(stat));
  rc = usmDbDNSClientDomainNameListGetNext(domain);
  if (rc == L7_SUCCESS)
  {
    if ((usmDbDhcpDomainNameOptionIsPresent(domain) != L7_TRUE)
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
        && (usmDbDhcp6cDomainNameOptionIsPresent(domain) != L7_TRUE)
#endif
       )
    {
      osapiSnprintf(stat, sizeof(stat), "\r\nip domain list \"%s\"", domain);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }
  while(usmDbDNSClientDomainNameListGetNext(domain) == L7_SUCCESS)
  {
    if ((usmDbDhcpDomainNameOptionIsPresent(domain) != L7_TRUE)
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
        && (usmDbDhcp6cDomainNameOptionIsPresent(domain) != L7_TRUE)
#endif
       )
    {
      memset(stat, L7_NULL, sizeof(stat));
      osapiSnprintf(stat, sizeof(stat), "\r\nip domain list \"%s\"", domain);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }

  memset(stat, L7_NULL, sizeof(stat));
  inetAddressReset(&serverAddr);
  rc = usmDbDNSClientNameServerUnorderedListEntryGetNext(&serverAddr);
  osapiSnprintf(stat, sizeof(stat), "\r\nip name server");
  if (rc == L7_SUCCESS)
  {
    if ((usmdbDhcpDNSOptionIsPresent(&serverAddr) != L7_TRUE)
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
        && (usmDbDhcp6cDNSOptionIsPresent(&serverAddr) != L7_TRUE)
#endif
       )
    {
      if (inetAddrHtop(&serverAddr, strAddr) == L7_SUCCESS)
      {
        serverFound = L7_TRUE;
        osapiSnprintfcat(stat, sizeof(stat), " %s", strAddr);
      }
    }
  }

  for (nameserver=1; nameserver < L7_DNS_NAME_SERVER_ENTRIES; nameserver++)
  {
    rc = usmDbDNSClientNameServerUnorderedListEntryGetNext(&serverAddr);
    if (rc == L7_SUCCESS)
    {
      if ((usmdbDhcpDNSOptionIsPresent(&serverAddr) != L7_TRUE)
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
          && (usmDbDhcp6cDNSOptionIsPresent(&serverAddr) != L7_TRUE)
#endif
         )
      {
        if (inetAddrHtop(&serverAddr, strAddr) == L7_SUCCESS)
        {
          serverFound = L7_TRUE;
          osapiSnprintfcat(stat, sizeof(stat), " %s", strAddr);
        }
      }
    }
    else
    {
      break;
    }
  }
  if(L7_TRUE == serverFound)
  {
    EWSWRITEBUFFER(ewsContext, stat);
  }

  memset(hostname, L7_NULL, sizeof(hostname));
  strcpy(hostname, "");
  inetAddressReset(&inetAddress);

  for (numhost=0; numhost<L7_DNS_STATIC_HOST_ENTRIES; numhost++)
  {
    rc = usmDbDNSClientStaticHostEntryNextGet(hostname, &inetAddress);
    if (rc == L7_SUCCESS)
    {
      if (L7_INET_GET_FAMILY(&inetAddress) == L7_AF_INET)
      {
        if (inetAddrHtop(&inetAddress, strAddr) == L7_SUCCESS)
        {
          osapiSnprintf(stat , sizeof(stat), "\r\nip host \"%s\" %s", hostname, strAddr);
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }
    else
    {
      break;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the Dot1x Auth Server User database running configuration
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 * @end
 *********************************************************************/
L7_RC_t cliRunningConfigIntAuthServ(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 userCount;
  L7_char8  strUserName[L7_LOGIN_SIZE];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 encryptedpwd[L7_ENCRYPTED_PASSWORD_SIZE];

  for (userCount = 0; userCount < L7_MAX_IAS_USERS; userCount++ )
  {
    bzero(strUserName, sizeof(strUserName));
    if (usmDbUserMgrIASUserDBUserNameGet(userCount, strUserName, sizeof(strUserName)) == L7_SUCCESS)
    {
      /* skip user if empty.  The name list will contain blanks if a user is deleted.  */
      if (strcmp(strUserName, CLIEMPTYSTRING) == 0)
      {
        continue;
      }
      bzero(encryptedpwd, sizeof(encryptedpwd));
      if (usmDbUserMgrIASUserDBUserPasswordGet(userCount, encryptedpwd, sizeof(encryptedpwd)) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%s %s %s %s",
                                pStrInfo_base_AAA,
                                pStrInfo_base_IASUser_1,
                                pStrInfo_base_Usrname1, strUserName);
        EWSWRITEBUFFER(ewsContext, buf);

        if(strlen(encryptedpwd) != 0)
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%s %s %s",
                                pStrInfo_base_Passwd_4, encryptedpwd, pStrInfo_base_Encrypted);
          EWSWRITEBUFFER(ewsContext, buf);
        }

        osapiSnprintf(buf, sizeof(buf), "\r\n%s",pStrInfo_common_Exit);

        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
  }
  return L7_SUCCESS;
}
