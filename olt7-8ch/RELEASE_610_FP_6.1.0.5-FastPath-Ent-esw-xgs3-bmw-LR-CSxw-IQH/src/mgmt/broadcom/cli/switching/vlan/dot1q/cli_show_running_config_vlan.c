/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_running_config_switching.c
 *
 * @purpose show running config commands for vlan
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
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_snooping_api.h"
#include "comm_mask.h"
#include "cliapi.h"
#include "config_script_api.h"
#include "usmdb_pml_api.h"
#include "osapi_support.h"

#ifndef _L7_OS_LINUX_
#include <inetLib.h>        /* for converting from IP to L7_int32 */
#endif /* _L7_OS_LINUX_ */

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"
#include "usmdb_vlan_ipsubnet_api.h"
#include "usmdb_vlan_mac_api.h"
#include "dot1q_api.h"
#include "cli_show_running_config.h"
#include "util_enumstr.h"
#include "snooping_exports.h"
#include "ptin_translate_api.h"

#ifdef L7_ROUTING_PACKAGE
#include "usmdb_rtrdisc_api.h"
#include "l3_mcast_defaultconfig.h"
#endif
#include "cliutil.h"

#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  To print the running configuration router vlan info
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
L7_RC_t cliRunningConfigRtrVlanInfo(EwsContext ewsContext, L7_uint32 unit,
                                    L7_uint32 nextvid)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intIfNum = 0;
  L7_uint32 vlanId;
  L7_uint32 intfId;
  

  while(usmDbNextIntIfNumberByTypeGet(L7_LOGICAL_VLAN_INTF, intIfNum, &intIfNum ) == L7_SUCCESS)
  {
    if (usmDbIpVlanRtrIntIfNumToVlanId(unit, intIfNum, &vlanId) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    else if (usmDbVlanIntfIdGet(vlanId, &intfId) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    else
    {
      osapiSnprintfAddBlanks(1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), 
                             "vlan routing %u %u", vlanId, intfId);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }
  return L7_SUCCESS;
}
#endif

/*********************************************************************
* @purpose  To print the running configuration of VLAN
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
L7_RC_t cliRunningConfigVlanInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_uint32 pvid = 0;
  L7_uint32 nextvid;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 groupID, nextGroupID;
  L7_BOOL first = L7_TRUE;
  L7_RC_t rc;
  L7_uint32 igmpMode;
  L7_uint32 fastLeaveMode;
  L7_ushort16 groupMembershipInt;
  L7_ushort16 maxResponseTime;
  L7_ushort16 mcrtExpiryTime;
  L7_uint32 ipv4Addr = 0;
  L7_uchar8 family = L7_AF_INET;
  L7_uchar8 ipAddr[L7_IP6_ADDR_LEN],nullIp[L7_IP6_ADDR_LEN];
  L7_uint32 xlateVid;
  L7_uint32 vids[4500],i = 0;

  groupID = 0;
  nextGroupID = 0;
  first = L7_TRUE;
  memset(nullIp, 0, L7_IP6_ADDR_LEN);
  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_VlanDbase);
  nextvid = L7_DOT1Q_DEFAULT_VLAN;
  /* default VLAN may not exist if the system is on its way up */
  if( usmDbVlanIDGet(unit, nextvid) ==  L7_SUCCESS )
  {
    if (usmDbNextVlanGet(unit, DOT1Q_NULL_VLAN, &nextvid) == L7_SUCCESS)
    {
    do
    {
      /* print VLAN ID */
      pvid = nextvid;

      rc = usmDbVlanTypeGet(unit, nextvid, &val);
      if (rc != L7_SUCCESS || val == L7_DOT1Q_DYNAMIC || val == L7_DOT1Q_WS_DYNAMIC)
      {
        continue;
      }
       if(nextvid == FD_DOT1Q_DEFAULT_VLAN)
       {
         if (EWSSHOWALL (ewsContext) != L7_SHOW_DEFAULT)
         {  
            continue;
         }
       }   
       vids[i] = nextvid;
       i++;

     } while (usmDbNextVlanGet(unit, pvid, &nextvid) == L7_SUCCESS);
    }/* End of get first valid vlan */
  }
  
  if(i > 0)   /*If there are any non default vlans exist */
  displayRangeFormat(ewsContext, vids, i, pStrInfo_common_MacAclVlan_2);

  nextvid = L7_DOT1Q_DEFAULT_VLAN;
  pvid =0;

  if( usmDbVlanIDGet(unit, nextvid) ==  L7_SUCCESS )
  {
    do
    {
      /* print VLAN ID */
      pvid = nextvid;

      rc = usmDbVlanTypeGet(unit, nextvid, &val);
      if (rc != L7_SUCCESS || val == L7_DOT1Q_DYNAMIC || val == L7_DOT1Q_WS_DYNAMIC)
      {
        continue;
      }

      /* Default vlan 1 is alreday created */

      /* print optional VLAN name */
      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      /*
       * vlan name set to "Default" in dot1qBuildDefaultQvlanData.
       * It should set to Factory Default.
       */
      if( usmDbVlanNameGet(unit, pvid, stat) ==  L7_SUCCESS)
      {
        if( pvid == FD_DOT1Q_DEFAULT_VLAN && (strcmp(stat, FD_DOT1Q_DEFAULT_VLAN_NAME) !=0 ) )
        {
          if (strcmp(stat,pStrInfo_common_EmptyString) !=0 )
          {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),pStrInfo_switching_VlanName_3, pvid, stat);
            EWSWRITEBUFFER(ewsContext, buf);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_switching_NoVlanName, pvid);
            EWSWRITEBUFFER(ewsContext, buf);
          }
        }
        else if ( pvid != FD_DOT1Q_DEFAULT_VLAN && (strcmp(stat,pStrInfo_common_EmptyString) !=0 ))
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),pStrInfo_switching_VlanName_3, pvid, stat);
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }

    } while (usmDbNextVlanGet(unit, pvid, &nextvid) == L7_SUCCESS);
  }

  if (usmDbComponentPresentCheck(unit, L7_SNOOPING_COMPONENT_ID) == L7_TRUE)
  {
    if(usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID, L7_IGMP_SNOOPING_FEATURE_PER_VLAN) == L7_TRUE)
    {
      family = L7_AF_INET;
      for(pvid = 1 ; pvid < (L7_DOT1Q_MAX_VLAN_ID+1) ; pvid++)
      {

        /* IGMP Snooping */
        if((usmDbSnoopVlanModeGet(unit, pvid, &igmpMode,family) == L7_SUCCESS)
           && (igmpMode != L7_DISABLE))
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_SetIgmp_1, pvid);
          EWSWRITEBUFFER(ewsContext, stat);

          // PTin added
          /* Mrouter VLAN Address */
          if (usmDbSnoopQuerierVlanXlateGet(pvid, &xlateVid) == L7_SUCCESS)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_SetIgmpQuerierVlanXlate, pvid, xlateVid);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }

        if((usmDbSnoopVlanFastLeaveModeGet(unit, pvid, &fastLeaveMode,family) == L7_SUCCESS)
           && (fastLeaveMode != L7_DISABLE))
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_SetIgmpFastLeave_2, pvid);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        if(usmDbSnoopVlanGroupMembershipIntervalGet(unit, pvid, &groupMembershipInt,family) == L7_SUCCESS)
        {
          cliShowCmdIntAddInt (ewsContext, groupMembershipInt, FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL,
                               pStrInfo_common_SetIgmpGrpMbrshipIntvl, pvid);
        }
        if(usmDbSnoopVlanMaximumResponseTimeGet(unit, pvid, &maxResponseTime,family) == L7_SUCCESS)
        {
          cliShowCmdIntAddInt (ewsContext, maxResponseTime, FD_IGMP_SNOOPING_MAX_RESPONSE_TIME,
                               pStrInfo_switching_SetIgmpMaxresponse, pvid);
        }
        if(usmDbSnoopVlanMcastRtrExpiryTimeGet(unit, pvid, &mcrtExpiryTime,family) == L7_SUCCESS)
        {
          cliShowCmdIntAddInt (ewsContext, mcrtExpiryTime, FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME,
                               pStrInfo_common_SetIgmpMcrtrexpiretime, pvid);
        }

        /* MLD Snooping Querier */
        /* Querier VLAN Mode */
        if(usmDbSnoopQuerierVlanModeGet(pvid, &igmpMode,family) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), pStrInfo_switching_SetIgmpQuerier_1, pvid);
          cliShowCmdEnable (ewsContext, igmpMode, FD_IGMP_SNOOPING_QUERIER_VLAN_MODE, stat);
        }

        /* Querier VLAN Address */
        if (usmDbSnoopQuerierVlanAddressGet(pvid, &ipv4Addr, family) == L7_SUCCESS)
        {
          if ((ipv4Addr != L7_NULL)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
          {
            if (usmDbInetNtoa(ipv4Addr, buf) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_SetIgmpQuerierAddr, pvid, buf);
              EWSWRITEBUFFER(ewsContext, stat);
            }
          }
        }

        /* Querier Election mode */
        if(usmDbSnoopQuerierVlanElectionModeGet(pvid, &igmpMode,family) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), pStrInfo_switching_SetIgmpQuerierElectionParticipate, pvid);
          cliShowCmdEnable (ewsContext, igmpMode, FD_IGMP_SNOOPING_QUERIER_VLAN_ELECTION_MODE, stat);
        }
      }/* End of vlan iterations */
    }/* end of igmp snooping per vlan feature check */

    if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                                 L7_MLD_SNOOPING_FEATURE_PER_VLAN) == L7_TRUE)
    {
      /* MLD Snooping */
      family = L7_AF_INET6;
      for(pvid = 1 ; pvid < (L7_DOT1Q_MAX_VLAN_ID+1) ; pvid++)
      {
        if((usmDbSnoopVlanModeGet(unit, pvid, &igmpMode,family) == L7_SUCCESS)
           && (igmpMode != L7_DISABLE))
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_SetMld_1, pvid);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        if((usmDbSnoopVlanFastLeaveModeGet(unit, pvid, &fastLeaveMode,family) == L7_SUCCESS)
           && (fastLeaveMode != L7_DISABLE))
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_SetMldFastLeave_1,pvid);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        if(usmDbSnoopVlanGroupMembershipIntervalGet(unit, pvid, &groupMembershipInt,family) == L7_SUCCESS)
        {
          cliShowCmdIntAddInt (ewsContext, groupMembershipInt, FD_MLD_SNOOPING_GROUP_MEMBERSHIP_INTERVAL,
                               pStrInfo_common_SetMldGrpMbrshipIntvl, pvid);
        }
        if(usmDbSnoopVlanMaximumResponseTimeGet(unit, pvid, &maxResponseTime,family) == L7_SUCCESS)
        {
          cliShowCmdIntAddInt (ewsContext, maxResponseTime, FD_MLD_SNOOPING_MAX_RESPONSE_TIME,
                               pStrInfo_switching_SetMldMaxrespons, pvid);
        }
        if(usmDbSnoopVlanMcastRtrExpiryTimeGet(unit, pvid, &mcrtExpiryTime,family) == L7_SUCCESS)
        {
          cliShowCmdIntAddInt (ewsContext, mcrtExpiryTime, FD_MLD_SNOOPING_MCAST_RTR_EXPIRY_TIME,
                               pStrInfo_switching_SetMldMcrtrexpiretime, pvid);
        }
        /* MLD Snooping Querier */
        /* Querier VLAN Mode */
        if(usmDbSnoopQuerierVlanModeGet(pvid, &igmpMode,family) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof (stat), pStrInfo_switching_SetMldQuerier_1, pvid);
          cliShowCmdEnable (ewsContext, igmpMode, FD_MLD_SNOOPING_QUERIER_VLAN_MODE, stat);
        }
        /* Querier VLAN Address */
        if (usmDbSnoopQuerierVlanAddressGet(pvid, ipAddr, L7_AF_INET6) == L7_SUCCESS)
        {
          if ((memcmp(ipAddr, nullIp, L7_IP6_ADDR_LEN) != 0)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
          {
            if (osapiInetNtop(L7_AF_INET6, ipAddr, buf, L7_CLI_MAX_STRING_LENGTH) != L7_NULL)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_SetMldQuerierAddr, pvid, buf);
              EWSWRITEBUFFER(ewsContext, stat);
            }
          }
        }

        /* Querier Election mode */
        if(usmDbSnoopQuerierVlanElectionModeGet(pvid, &igmpMode,family) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof (stat), pStrInfo_switching_SetMldQuerierElectionParticipate, pvid);
          cliShowCmdEnable (ewsContext, igmpMode, FD_MLD_SNOOPING_QUERIER_VLAN_ELECTION_MODE, stat);
        }
      }/* End of VLAN iterations */
    }/* MLD Snooping per vlan config check */
  }/* component present check */

#ifdef L7_PBVLAN_PACKAGE
  cliRunningConfigProtocolVlanInfo(ewsContext, unit);
#endif
  

#ifdef L7_ROUTING_PACKAGE
  cliRunningConfigRtrVlanInfo( ewsContext, unit, L7_DOT1Q_DEFAULT_VLAN);
#endif

#ifdef L7_IPVLAN_PACKAGE
  cliRunningConfigIpSubnetVlanInfo(ewsContext, unit);
#endif

#ifdef L7_MACVLAN_PACKAGE
  cliRunningConfigMacVlanInfo(ewsContext, unit);
#endif

  EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_Exit);

  return L7_SUCCESS;
}



L7_RC_t cliRunningConfigInterfaceClassofServiceDot1p(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_uint32 priority, val, globalVal;

  /* ClassofService Trust */
  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
  {
    for (priority = L7_DOT1P_MIN_PRIORITY; priority <= L7_DOT1P_MAX_PRIORITY; priority++)
    {
      if ((usmDbDot1dTrafficClassGet(unit, interface, priority, &val) == L7_SUCCESS &&
           usmDbDot1dTrafficClassGet (unit, L7_ALL_INTERFACES, priority, &globalVal) == L7_SUCCESS))
      {
        cliShowCmdIntAddInt (ewsContext, val, globalVal, pStrInfo_switching_ClassofserviceDot1pMapping, priority);
      }
    } /* endfor */
  }    /* end class of service port info */

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

L7_RC_t cliRunningConfigClassofserviceInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 priority, globalVal, defaultVal;

  for (priority = L7_DOT1P_MIN_PRIORITY; priority <= L7_DOT1P_MAX_PRIORITY; priority++)
  {
    if ((usmDbDot1dTrafficClassGet (unit, L7_ALL_INTERFACES, priority, &globalVal) == L7_SUCCESS &&
         usmDbDot1dDefaultTrafficClassGet(unit, L7_ALL_INTERFACES, priority, &defaultVal) == L7_SUCCESS))
    {
      cliShowCmdIntAddInt (ewsContext, globalVal, defaultVal, pStrInfo_switching_ClassofserviceDot1pMapping,priority);
    }

  }
  return L7_SUCCESS;
}

