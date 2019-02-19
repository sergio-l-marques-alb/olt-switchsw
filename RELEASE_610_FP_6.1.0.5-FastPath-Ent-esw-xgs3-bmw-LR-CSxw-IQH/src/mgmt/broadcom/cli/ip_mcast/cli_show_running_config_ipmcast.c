/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/ip_mcast/cli_show_running_config_ipmcast.c
*
* @purpose show running config commands for ip_mcast
*
* @component user interface
*
* @comments
*
* @create  18/08/2003
*
* @author   Samip
* @end
*
**********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_igmp_api.h"
#include "usmdb_mib_dvmrp_api.h"
#include "usmdb_mib_pim_api.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_pimsm_api.h"
#endif

#include "comm_mask.h"
#include "cliapi.h"
#include "config_script_api.h"

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"

#include "l3_mcast_defaultconfig.h"
#include "usmdb_mib_mcast_api.h"
#include "cli_show_running_config.h"

#ifdef L7_MCAST_PACKAGE
/*********************************************************************
* @purpose  To print the running configuration of mcast Info
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

L7_RC_t cliRunningConfigMcastInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if (usmDbMcastAdminModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, val, FD_MCAST_DEFAULT_ADMIN_MODE, pStrInfo_ipmcast_IpMcast);
  }
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  To print the running configuration of dvmrp, pimsm, pimdm
*                     and pim-trap
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

L7_RC_t cliRunningConfigMiscInfo(EwsContext ewsContext, L7_uint32 unit)
{
   L7_uint32       val;
   L7_char8        stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8        rpGrpAddrBuf[L7_CLI_MAX_STRING_LENGTH],
                   rpGrpMaskBuf[L7_CLI_MAX_STRING_LENGTH],
                   inetAddrBuf[L7_CLI_MAX_STRING_LENGTH],
                   rpAddrBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32       ipGrp, ipMask, metric, ipSrc = 0;
   L7_char8        ipGrpBuf[L7_CLI_MAX_STRING_LENGTH], 
                   ipMaskBuf[L7_CLI_MAX_STRING_LENGTH], 
                   ipSrcBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32        u=1, s, p ; 
   L7_inet_addr_t  inetRpGrpAddr, inetGrpMask, 
                   inetSsmRangeAddr, inetbsrAddr, inetRpIpAddr,inetCandRpGrpAddr;
   L7_inet_addr_t  inetIpGrp, inetIpMask, inetIpSrc;
   L7_uchar8       prefixLength = L7_NULL;
   L7_uchar8 candprefixLength =L7_NULL;

   L7_uint32       bsrPriority = L7_NULL, bsrHashMaskLen = L7_NULL;
   L7_uint32       intIfNum = L7_NULL;
   L7_RC_t         retVal = L7_FAILURE;
   L7_uint32 indexstart=L7_NULL;
   L7_BOOL override = L7_NULL;
   L7_inet_addr_t defaultSSMAddr, defaultSSMMask;

   inetAddressZeroSet(L7_AF_INET, &inetRpGrpAddr);
   inetAddressZeroSet(L7_AF_INET, &inetGrpMask);
   inetAddressZeroSet(L7_AF_INET, &inetRpIpAddr);
   inetAddressZeroSet(L7_AF_INET, &inetbsrAddr);

   if (usmDbComponentPresentCheck(unit, L7_FLEX_DVMRP_MAP_COMPONENT_ID))
   {
     if(usmDbDvmrpAdminModeGet(unit, &val) == L7_SUCCESS )
    {
      cliShowCmdEnable(ewsContext,val,FD_DVMRP_DEFAULT_ADMIN_MODE,pStrInfo_common_IpDvmrp_1);
     }
   }

   if (usmDbDvmrpTrapModeGet(unit, L7_DVMRP_TRAP_ALL, &val) == L7_SUCCESS) 
  {
    cliShowCmdEnable(ewsContext,val,(FD_TRAP_DVMRP & L7_DVMRP_TRAP_ALL),pStrInfo_ipmcast_IpDvmrpTrapflags);
  }
   if (usmDbMgmdAdminModeGet(unit, L7_AF_INET, &val) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext,val,FD_IGMP_DEFAULT_ADMIN_MODE,pStrInfo_common_IpIgmp);
   }
  if (usmDbMgmdRouterAlertMandatoryGet(unit, L7_AF_INET, &val) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext,val,FD_IGMP_DEFAULT_ROUTER_ALERT_CHECK,pStrInfo_common_IpIgmpRouterAlert);
  }
 
   if (usmDbComponentPresentCheck(unit, L7_FLEX_PIMDM_MAP_COMPONENT_ID))
   {
     if (usmDbPimRouterAdminModeGet(unit, L7_AF_INET, &val) == L7_SUCCESS )
    {
      cliShowCmdEnable(ewsContext,val,FD_PIMDM_DEFAULT_ADMIN_MODE,pStrInfo_ipmcast_IpPimdm);
     }
   }

   if (usmDbComponentPresentCheck(unit, L7_FLEX_PIMSM_MAP_COMPONENT_ID))
   {
     if (usmDbPimsmAdminModeGet(unit, L7_AF_INET, &val) == L7_SUCCESS )
    {
      cliShowCmdEnable(ewsContext,val,FD_PIMSM_DEFAULT_ADMIN_MODE,pStrInfo_common_IpPimsm);
    }
    if (usmDbPimsmDataThresholdRateGet (unit, L7_AF_INET, &val) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,val,FD_PIMSM_DEFAULT_DATA_THRESHOLD_RATE,pStrInfo_ipmcast_IpPimsmSptThresh);
   }

    if(usmDbPimsmRegisterThresholdRateGet (unit, L7_AF_INET, &val) == L7_SUCCESS) 
    {
      cliShowCmdInt(ewsContext,val,FD_PIMSM_DEFAULT_REG_THRESHOLD_RATE,pStrInfo_ipmcast_IpPimsmRegisterThresh);
    }
      
      while (usmDbPimsmCliStaticRpNextGet(L7_AF_INET,&inetRpIpAddr,&inetRpGrpAddr,&prefixLength,&indexstart,&override)== L7_SUCCESS)

      {
        memset(stat, L7_NULL, sizeof(stat));
        memset(rpGrpAddrBuf, L7_NULL, sizeof(rpGrpAddrBuf));
        memset(rpGrpMaskBuf, L7_NULL, sizeof(rpGrpMaskBuf));
        memset(rpAddrBuf,L7_NULL,sizeof(rpAddrBuf));
        inetMaskLenToMask(L7_AF_INET, prefixLength, &inetGrpMask);
        if ((inetAddrHtop(&inetRpGrpAddr, rpGrpAddrBuf) == L7_SUCCESS) &&
            (inetAddrHtop(&inetGrpMask, rpGrpMaskBuf) == L7_SUCCESS)&&(inetAddrHtop(&inetRpIpAddr,rpAddrBuf)==L7_SUCCESS)&&(override==L7_FALSE))
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_ipmcast_IpPimsmRpAddr,
                            rpAddrBuf,rpGrpAddrBuf, rpGrpMaskBuf);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        else 
        {
         osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_ipmcast_IpPimsmRpAddroverride,
                            rpAddrBuf,rpGrpAddrBuf, rpGrpMaskBuf);
          EWSWRITEBUFFER(ewsContext, stat);
        }

      }
     
    indexstart=L7_NULL; 
    inetAddressZeroSet(L7_AF_INET, &inetCandRpGrpAddr);
    intIfNum =0;
    

        while (usmDbPimsmCliCandidateRPEntryNextGet(unit, L7_AF_INET, &intIfNum,
                                               &inetCandRpGrpAddr, &candprefixLength,&indexstart) == L7_SUCCESS)

      {
        memset(stat, L7_NULL, sizeof(stat));
        memset(rpGrpAddrBuf, L7_NULL, sizeof(rpGrpAddrBuf));
        memset(rpGrpMaskBuf, L7_NULL, sizeof(rpGrpMaskBuf));
        inetMaskLenToMask(L7_AF_INET, candprefixLength, &inetGrpMask);
        if ((usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS) &&
            (inetAddrHtop(&inetCandRpGrpAddr, rpGrpAddrBuf) == L7_SUCCESS) &&
            (inetAddrHtop(&inetGrpMask, rpGrpMaskBuf) == L7_SUCCESS))
        {
          sprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_ipmcast_IpPimsmRpCandidateIntf_Fp,
                          cliDisplayInterfaceHelp(u, s, p), rpGrpAddrBuf, 
                          rpGrpMaskBuf);
            EWSWRITEBUFFER(ewsContext, stat);
         }
       }
     inetAddressZeroSet(L7_AF_INET, &inetSsmRangeAddr);
     prefixLength = L7_NULL;
     if (usmDbPimsmSsmRangeEntryGet(L7_AF_INET, &inetSsmRangeAddr, 
                                    prefixLength) == L7_FAILURE)
     {
       osapiStrncpy(ipGrpBuf, FD_PIMSM_SSM_RANGE_GROUP_ADDRESS, 
                    sizeof(ipGrpBuf));
       osapiStrncpy(ipMaskBuf, FD_PIMSM_SSM_RANGE_GROUP_MASK, 
                    sizeof(ipMaskBuf));
       usmDbParseInetAddrFromStr(ipGrpBuf, &defaultSSMAddr);
       usmDbParseInetAddrFromStr(ipMaskBuf, &defaultSSMMask);

       memset(ipGrpBuf, L7_NULL, sizeof(ipGrpBuf));
       memset(ipMaskBuf, L7_NULL, sizeof(ipMaskBuf));

       retVal = usmDbPimsmSsmRangeEntryNextGet(L7_AF_INET, &inetSsmRangeAddr, 
                                               &prefixLength);
       while (retVal == L7_SUCCESS)
       {
         memset(stat, L7_NULL, sizeof(stat));
         memset(inetAddrBuf, L7_NULL, sizeof(inetAddrBuf));
         memset(rpGrpMaskBuf, L7_NULL, sizeof(rpGrpMaskBuf));
         inetMaskLenToMask(L7_AF_INET, prefixLength, &inetGrpMask);
         if ((L7_INET_ADDR_COMPARE(&inetSsmRangeAddr, &defaultSSMAddr) == L7_NULL) &&
             (L7_INET_ADDR_COMPARE(&inetGrpMask, &defaultSSMMask) == L7_NULL))
         {
           sprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, "ip pimsm ssm default");
           EWSWRITEBUFFER(ewsContext, stat);
         }
         else if ((inetAddrHtop(&inetGrpMask, rpGrpMaskBuf) == L7_SUCCESS) &&
             (inetAddrHtop(&inetSsmRangeAddr, inetAddrBuf) == L7_SUCCESS))
         {
           sprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_ipmcast_IpPimsmSsm,
                          inetAddrBuf, rpGrpMaskBuf);
           EWSWRITEBUFFER(ewsContext, stat);
         }
         retVal = usmDbPimsmSsmRangeEntryNextGet(L7_AF_INET, &inetSsmRangeAddr, 
                                                 &prefixLength);
       }
     }
     if (usmDbPimsmCandBSREntryGet(L7_AF_INET) == L7_SUCCESS)
     {
       if ((usmDbPimsmCandBSRInterfaceGet(L7_AF_INET,
                                               &intIfNum) == L7_SUCCESS) &&
           (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS) &&
           (usmDbPimsmCandBSRAddrGet(L7_AF_INET, 
                                         &inetbsrAddr) == L7_SUCCESS) &&
           (usmDbPimsmCandBSRPriorityGet(L7_AF_INET, 
                                         &bsrPriority) == L7_SUCCESS) &&
           (usmDbPimsmCandBSRHashMaskLengthGet(L7_AF_INET, 
                                         &bsrHashMaskLen) == L7_SUCCESS))
       {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_ipmcast_IpPimsmBsrCandidateIntf_Fp,
                        cliDisplayInterfaceHelp(u, s, p), 
                        bsrHashMaskLen, bsrPriority);
          EWSWRITEBUFFER(ewsContext, stat);
       }
     }
 }

  if(usmDbPimTrapModeGet(L7_PIM_TRAP_ALL, &val) == L7_SUCCESS) 
  {
    cliShowCmdEnable(ewsContext,val,(FD_TRAP_PIM & L7_PIM_TRAP_ALL),pStrInfo_ipmcast_IpPimTrapflags);
  }

  /* START : Mcast Static MRoute (ipv4) - running config */
  inetAddressZeroSet(L7_AF_INET, &inetIpSrc);
  inetAddressZeroSet(L7_AF_INET, &inetIpMask);
  inetAddressZeroSet(L7_AF_INET, &inetIpGrp);
  while (usmDbMcastStaticMRouteEntryNextGet (unit, L7_AF_INET, &inetIpSrc, &inetIpMask)
                                          == L7_SUCCESS)
  {
    memset (stat, 0, sizeof(stat));
    if ((usmDbMcastStaticMRouteRpfAddressGet (unit, L7_AF_INET, &inetIpSrc, &inetIpMask, &inetIpGrp) == L7_SUCCESS) &&
        (usmDbMcastStaticMRoutePreferenceGet (unit, L7_AF_INET, &inetIpSrc, &inetIpMask, &metric) == L7_SUCCESS))
    {
      memset (ipSrcBuf, 0, sizeof(ipSrcBuf));
      memset (ipMaskBuf, 0, sizeof(ipMaskBuf));
      memset (ipGrpBuf, 0, sizeof(ipGrpBuf));

      inetAddressGet (L7_AF_INET, &inetIpSrc, &ipSrc);
      inetAddressGet (L7_AF_INET, &inetIpMask, &ipMask);
      inetAddressGet (L7_AF_INET, &inetIpGrp, &ipGrp);       

      if ((usmDbInetNtoa (ipSrc, ipSrcBuf) == L7_SUCCESS) &&
          (usmDbInetNtoa (ipMask, ipMaskBuf) == L7_SUCCESS) && 
          (usmDbInetNtoa (ipGrp,ipGrpBuf) == L7_SUCCESS))
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_ipmcast_IpMcastStaticroute,ipSrcBuf,ipMaskBuf,ipGrpBuf,metric);
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  /* END : Mcast Static MRoute (ipv4) - running config */


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of mcast mroute boundary info
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
L7_RC_t cliRunningConfigMcastMrouteBoundaryInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{          

   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 ipGrp = 0, ipMask = 0;
   L7_uint32 intIface =1;
   L7_inet_addr_t inetIpGrp, inetIpMask;

   if (usmDbMcastIsValidIntf(unit, interface) != L7_TRUE)
   {
     /* nothing to do */
     return L7_SUCCESS;
   }

   while(usmDbMcastMrouteBoundaryEntryNextGet(unit, &intIface, &inetIpGrp,
			            &inetIpMask) == L7_SUCCESS)
  {
   if(intIface== interface)
   {
     memset(buf, L7_NULL, sizeof(buf));
     memset(stat, L7_NULL, sizeof(stat));
     inetAddressGet(L7_AF_INET, &inetIpGrp, &ipGrp);
     if(usmDbInetNtoa(ipGrp, buf) == L7_SUCCESS)
     {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_IpMcastBoundary_1, buf);
     }
     memset(buf, L7_NULL, sizeof(buf));
     inetAddressGet(L7_AF_INET, &inetIpMask, &ipMask);
     if(usmDbInetNtoa(ipMask, buf) == L7_SUCCESS)
     {
      OSAPI_STRNCAT(stat, buf);
     }
     EWSWRITEBUFFER(ewsContext,stat);
     break;
   }
    }
   return L7_SUCCESS;
}

#endif /* end if mcast package included */
