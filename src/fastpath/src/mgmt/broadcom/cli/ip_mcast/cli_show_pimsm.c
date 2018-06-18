/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/ip_mcast/cli_show_pimsm.c
*
* @purpose show commands for the pimsm cli functionality
*
* @component user interface
*
* @comments
*           An output value of Err means that the usmDb returned != SUCCESS   english num 1312
*           An output value of ------ means that the value type was incompatible,
*           and even though the usmDb return SUCCESS; the value is garbage.
*
* @create
*
* @author  srikrishnas
* @end
*
**********************************************************************/
#include <cliapi.h>
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_cli.h"
#include "l3_mcast_commdefs.h"
#include "l3_mcast_defaultconfig.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <datatypes.h>
#include <usmdb_ip_api.h>
#include <usmdb_util_api.h>
#include <clicommands_l3.h>
#include <l3_commdefs.h>
#include <clicommands_mcast.h>
#include <usmdb_mib_pimsm_api.h>
#include <usmdb_pimsm_api.h>
#include "usmdb_l3.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_pimsm_api.h"
#endif

#include "clicommands_card.h"
#include "osapi.h"

/*********************************************************************
*
* @purpose  Displays pimsm candidate RP Table information
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip pimsm rp-candidate
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpPimsmRpCandidate(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
#ifdef PIMSM_REMOVE
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 ipVal =-1, mode = L7_DISABLE;
    L7_uint32 cRPGroupAddress, cRPGroupMask;
    L7_inet_addr_t inetcRPGroupAddress, inetcRPGroupMask, inetIpVal;
    L7_RC_t rc;
    L7_uint32 unit, numArgs;

    cliSyntaxTop(ewsContext);
    inetAddressZeroSet(L7_AF_INET, &inetcRPGroupAddress);
    inetAddressZeroSet(L7_AF_INET, &inetcRPGroupMask);
    inetAddressZeroSet(L7_AF_INET, &inetIpVal);

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    numArgs = cliNumFunctionArgsGet();
    if (numArgs != 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpPimSmRpCandidate);
    }

    if ((usmDbPimsmAdminModeGet (unit, L7_AF_INET, &mode) != L7_SUCCESS) ||
        (mode != L7_ENABLE))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_ipmcast_MustInitPimSm);
    }

    cRPGroupAddress = 0;   /* set these to invalid values*/
    cRPGroupMask = 0;
    inetAddressSet(L7_AF_INET, &cRPGroupAddress, &inetcRPGroupAddress);
    inetAddressSet(L7_AF_INET, &cRPGroupMask, &inetcRPGroupMask);

   rc = usmDbPimsmCandRPEntryNextGet(unit, &inetcRPGroupAddress, &inetcRPGroupMask, L7_AF_INET);

   if(rc == L7_ERROR)
   {
       cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_ipmcast_RtrIsNotCfguredAsCBsrAndCRp);
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_GrpGrpRpUpTimeExpiryTimeInfoType);
  ewsTelnetWriteAddBlanks (1, 0, 0, 6, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_AddrMaskAddrHhMmSsHhMmSsSrc);
   ewsTelnetWrite(ewsContext,"\r\n--------- -------- -------- ---------- ----------- ------- -----\r\n");

   while (rc == L7_SUCCESS)
   {

       memset(stat, L7_NULL, sizeof(stat));
       memset(buf, L7_NULL, sizeof(buf));
       if (usmDbInetNtoa(cRPGroupAddress, buf) == L7_SUCCESS)
       {
           osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
       }
       else
       {
      osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
       }
       ewsTelnetWrite(ewsContext, stat);

       memset(stat, L7_NULL, sizeof(stat));
       memset(buf, L7_NULL, sizeof(buf));
       if (usmDbInetNtoa(cRPGroupMask, buf) == L7_SUCCESS)
       {
           osapiSnprintf(stat, sizeof(stat), "%-16.15s", buf);
       }
       else
       {
      osapiSnprintf(stat, sizeof(stat), "%-16.15s", pStrErr_common_Err);
       }
       ewsTelnetWrite(ewsContext, stat);

       memset(stat, L7_NULL, sizeof(stat));
       memset(buf, L7_NULL, sizeof(buf));
       if(usmDbPimsmCandRPAddressGet(unit, &inetcRPGroupAddress,
              &inetcRPGroupMask, &inetIpVal, L7_AF_INET)== L7_SUCCESS)
       {
           if (usmDbInetNtoa(ipVal, buf) == L7_SUCCESS)
           {
               osapiSnprintf(stat, sizeof(stat), "%-16.15s", buf);
           }
           else
           {
        osapiSnprintf(stat, sizeof(stat), "%-16.15s", pStrErr_common_Err);
           }
       }
       else
       {
      osapiSnprintf(stat, sizeof(stat), "%-16.15s", pStrErr_common_Err);
       }
       ewsTelnetWrite(ewsContext, stat);

       rc = usmDbPimsmCandRPEntryNextGet(unit, &inetcRPGroupAddress,
                                              &inetcRPGroupMask, L7_AF_INET);
       cliSyntaxBottom(ewsContext);
   }
#endif
    return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Displays pimsm BSR information
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip pimsm bsr
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpPimsmBsr(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 mode = L7_DISABLE;
    L7_inet_addr_t inetElecBsrAddr;
    L7_uint32 unit, numArgs, bsrPriority=0, hashMaskLen=0;
    usmDbTimeSpec_t CRPHoldTime;
    L7_timespec expiryTime;
    L7_uint32 exptime=0; 

    cliSyntaxTop(ewsContext);
    inetAddressZeroSet(L7_AF_INET, &inetElecBsrAddr);

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    numArgs = cliNumFunctionArgsGet();
    if (numArgs != 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpPimSmBsr);
    }

   if ((usmDbPimsmAdminModeGet (unit, L7_AF_INET, &mode) != L7_SUCCESS) ||
       (mode != L7_ENABLE))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_ipmcast_MustInitPimSm);
   }

   memset(stat, L7_NULL, sizeof(stat));
   memset(buf, L7_NULL, sizeof(buf));
  cliFormat(ewsContext, pStrInfo_common_PimSmElectedBsrAddr);
   if (usmDbPimsmElectedBSRAddressGet(unit, L7_AF_INET,
                                      &inetElecBsrAddr) == L7_SUCCESS)
   {
       if (inetAddrHtop(&inetElecBsrAddr, buf) == L7_SUCCESS)
       {
      strcpy(stat, buf);
       }
       else
       {
           osapiStrncpySafe(stat, pStrErr_common_Err, sizeof(pStrErr_common_Err));
       }
       ewsTelnetWrite(ewsContext, stat);

       memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_common_PimSmElectedBsrPri);
       if (usmDbPimsmElectedBSRPriorityGet(unit, L7_AF_INET,
                                           &bsrPriority) == L7_SUCCESS)
       {
           sprintf(buf,"%d", bsrPriority);
       }
       else
       {
           osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
       }
       ewsTelnetWrite(ewsContext, buf);

       memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_common_PimSmElectedBsrHashMaskLen);
       if (usmDbPimsmElectedBSRHashMaskLengthGet(unit, L7_AF_INET,
                                                 &hashMaskLen) == L7_SUCCESS)
       {
           sprintf(buf,"%d", hashMaskLen);
       }
       else
       {
           osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
       }
       ewsTelnetWrite(ewsContext, buf);

       memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_common_PimSmElectedBsrMsg);
       if (usmDbPimsmElectedBSRExpiryTimeGet(unit, L7_AF_INET,
                                             &exptime)== L7_SUCCESS)
       {
           osapiConvertRawUpTime(exptime,&expiryTime);
           sprintf(buf,"%2.2d:%2.2d:%2.2d", expiryTime.hours,
                         expiryTime.minutes, expiryTime.seconds);
       }
       else
       {
           osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
       }
       ewsTelnetWrite(ewsContext, buf);

       memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_common_PimSmElectedBsrRp);
       if (usmDbPimsmElectedCRPAdvTimeSpecGet(unit, L7_AF_INET,
                                              &CRPHoldTime)== L7_SUCCESS)
       {
           sprintf(buf,"%2.2d:%2.2d:%2.2d", CRPHoldTime.hours,
                    CRPHoldTime.minutes, CRPHoldTime.seconds);
       }
       else
       {
           osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
       }
       ewsTelnetWrite(ewsContext, buf);
   }
   else
   {
    ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_BsrIsNotCfgured);
   }
   return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Displays pimsm information on the router
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
* @cmdsyntax    show ip pimsm
*
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const char *commandShowIpPimsm(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
    L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 intSlot, intPort, interface;
    L7_uint32 nextInterface;
    L7_uint32 val = L7_DISABLE;
    L7_uint32 unit, numArgs;
    L7_inet_addr_t  ssmRangeAddr;
    L7_uchar8 prefixLen = L7_NULL;
    L7_RC_t   retVal = L7_FAILURE;
    L7_char8  *dispbuf = L7_NULLPTR;

    cliSyntaxTop(ewsContext);

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    numArgs = cliNumFunctionArgsGet();
    if (numArgs != 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpPimSm_1);
  }

  memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_ipmcast_AdminMode);
    if (usmDbPimsmAdminModeGet (unit, L7_AF_INET, &val) == L7_SUCCESS)
    {
    osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_DataThreshRateKbps);
    if (usmDbPimsmDataThresholdRateGet (unit, L7_AF_INET, &val) == L7_SUCCESS)
    {
        osapiSnprintf(stat, sizeof(stat),  "%d", val);
    }
    else
    {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext,stat);

    memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_RegisterThreshRateKbps);
    if (usmDbPimsmRegisterThresholdRateGet (unit,
                                            L7_AF_INET, &val) == L7_SUCCESS)
    {
        osapiSnprintf(stat, sizeof(stat),  "%d", val);
    }
    else
    {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext,stat);

    /* Heading for the pimsm interface summary */
    if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_McastNoPortsAvailable);
    }

    cliSyntaxBottom(ewsContext);
    /* Display the PIMSM SSM Range info */
  ewsTelnetWriteAddBlanks (1, 0, 5, 7, L7_NULLPTR, ewsContext,pStrInfo_common_SsmRangeTbl);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_GrpAddrPrefixLen);
    ewsTelnetWrite(ewsContext,"\r\n----------------------------\r\n");
    inetAddressZeroSet(L7_AF_INET, &ssmRangeAddr); /* First Entry*/
    prefixLen = L7_NULL;                           /* First Entry*/
    retVal = usmDbPimsmSsmRangeEntryNextGet(L7_AF_INET,
                                            &ssmRangeAddr, &prefixLen);
    while (retVal == L7_SUCCESS)
    {
      memset(stat, L7_NULL, sizeof(stat));
      memset(buf, L7_NULL, sizeof(buf));
      if (inetAddrHtop(&ssmRangeAddr, buf) == L7_SUCCESS)
      {
          sprintf(stat,"\n%s/%d", buf, prefixLen);
      }
      else
      {
      sprintf(stat,"\n%s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);     /* Group Address/Prefix Len */
      retVal = usmDbPimsmSsmRangeEntryNextGet(L7_AF_INET,
                                              &ssmRangeAddr, &prefixLen);
    }

    /* Display the PIMSM interface info */
  ewsTelnetWriteAddBlanks (2, 0, 2, 19, L7_NULLPTR, ewsContext,pStrInfo_common_PimSmIntfStatus);
  ewsTelnetWriteAddBlanks (1, 0, 0, 3, L7_NULLPTR, ewsContext,pStrInfo_common_IntfIntfModeProtoState);
    ewsTelnetWrite(ewsContext,"\r\n---------  --------------  ----------------\r\n");

    /* start for loop for all the interfaces */
    while (interface)
    {
        /* display only the visible interfaces */
        if ((usmDbPimsmIsValidIntf(unit, interface) == L7_TRUE) &&
                             (usmDbIpIntfExists(unit, interface) == L7_TRUE))
        {
            memset(buf, L7_NULL, sizeof(buf));
            memset(strSlotPort, L7_NULL, sizeof(strSlotPort));
            if (usmDbUnitSlotPortGet(interface, &val,
                                     &intSlot, &intPort) == L7_SUCCESS)
            {
              dispbuf = cliDisplayInterfaceHelp(val, intSlot, intPort);
              osapiSnprintf(buf, sizeof(buf), "%-11.9s", dispbuf);
            }
            else
            {
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
            }
      ewsTelnetPrintf (ewsContext, buf);

            memset(stat, L7_NULL, sizeof(stat));
            memset(buf, L7_NULL, sizeof(buf));
            if (usmDbPimInterfaceModeGet(unit, L7_AF_INET,
                                           interface, &val) == L7_SUCCESS)
            {
        osapiSnprintf(stat,sizeof(stat),"%-16s",strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-16s", pStrInfo_common_Blank);
      }
      ewsTelnetWrite(ewsContext, stat);

          memset(buf, L7_NULL, sizeof(buf));
          if (usmDbPimsmInterfaceOperationalStateGet(unit, L7_AF_INET,
                                        interface) == L7_TRUE)
      {
        osapiSnprintf(buf, sizeof(buf),  "%-16s", pStrInfo_common_Operational);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  "%-16s", pStrInfo_common_NonOperational);
      }
          ewsTelnetWrite(ewsContext, buf);

            cliSyntaxBottom(ewsContext);
            /*end if interface exists*/
        }   /* end if visible interface */
        /* Get the next interface */
        if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
    {
            interface = nextInterface;
    }
    else
    {
            interface   = 0;
    }
  }
    return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Displays pimsm statistical information on the speciied interface
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
* @cmdsyntax    show ip pimsm interface [<slot/port>]
*
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const char *commandShowIpPimsmInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
    L7_BOOL   all;
    L7_uint32 intIface;
    L7_char8     buf[L7_CLI_MAX_STRING_LENGTH];
    L7_char8     stat[L7_CLI_MAX_STRING_LENGTH];
    L7_char8     cmdBuf[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32    lineCount = 0;
    L7_uint32    argSlotPort = 1;
    L7_uint32    val = L7_DISABLE;
    L7_uint32    i, u, s, p, slot, port;
    L7_uint32    unit, numArgs;
    L7_IP_ADDR_t ipAddr =0, ipmask;
    L7_RC_t      rc = L7_SUCCESS;
    L7_inet_addr_t inetIpAddr;
    L7_char8  *dispbuf = L7_NULLPTR;

    cliSyntaxTop(ewsContext);
    inetAddressSet(L7_AF_INET, &ipAddr, &inetIpAddr);

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    cliCmdScrollSet( L7_FALSE);
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {      /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      intIface = 0;
    }

    numArgs = cliNumFunctionArgsGet();
    if (numArgs >= 2)
    {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpPimSmIntfStats, cliSyntaxInterfaceHelp());
    }

    if (numArgs == 0)
    {
      all = L7_TRUE;
      rc = usmDbPimsmInterfaceEntryNextGet(unit, L7_AF_INET, &intIface);
                                                    /* get the next entry */
      if (rc != L7_SUCCESS)
      {
      return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
    else
    {
      all = L7_FALSE;
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if (cliValidSpecificUSPCheck(argv[index+argSlotPort],
                                     &unit, &s, &p) != L7_SUCCESS)
        {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }

        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIface) != L7_SUCCESS)
        {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
      else
      {
        if (cliSlotPortToInterface(argv[index + argSlotPort],
                                   &unit, &s, &p, &intIface) != L7_SUCCESS)
        {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }

      if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      rc = usmDbPimsmInterfaceEntryGet(unit, L7_AF_INET, intIface);
    }
    if (rc != L7_SUCCESS)
    {
    ewsTelnetWrite( ewsContext, pStrInfo_common_PimSmNoIntfs);
         cliSyntaxBottom(ewsContext);
    }
    while (rc == L7_SUCCESS)
    {
      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbUnitSlotPortGet(intIface, &u, &slot, &port) == L7_SUCCESS)
    {
      dispbuf = cliDisplayInterfaceHelp(u, slot, port);
      osapiSnprintf(buf, sizeof(buf), "%s", dispbuf);
    }
    else
    {
      osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
    }
    cliFormat(ewsContext, pStrInfo_common_SlotPortWithoutUnit_1);
      ewsTelnetWrite(ewsContext, buf);  /* Slot/Port */

      if (usmDbIpRtrIntfIpAddressGet(unit, intIface, &ipAddr,
                                            &ipmask) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbInetNtoa(ipAddr, buf);
      strcpy(stat, buf);
      cliFormat(ewsContext, pStrInfo_common_ApIpAddr);
        ewsTelnetWrite(ewsContext, stat); /* IP Address */

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbInetNtoa(ipmask, buf);
      strcpy(stat, buf);
      cliFormat(ewsContext, pStrInfo_common_SubnetMask_3);
        ewsTelnetWrite(ewsContext, stat); /* Subnet Mask */
      }
      else
      {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
      cliFormat(ewsContext, pStrInfo_common_ApIpAddr);
      ewsTelnetWrite(ewsContext, stat);   /* IP Address */
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
      cliFormat(ewsContext, pStrInfo_common_SubnetMask_3);
        ewsTelnetWrite(ewsContext, stat); /* Subnet Mask */
      }

      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbPimIntfHelloIntervalGet(unit, L7_AF_INET,
                                              intIface, &val) == L7_SUCCESS)
      {
        sprintf(buf, "%u", val);
      }
      else
      {
        osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
    cliFormat(ewsContext, pStrInfo_common_HelloIntvlSecs);
      ewsTelnetWrite(ewsContext,buf); /* Hello Interval */

      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbPimsmInterfaceJoinPruneIntervalGet(unit, L7_AF_INET,
                                                  intIface, &val) == L7_SUCCESS)
      {
        sprintf(buf, "%u", val);
      }
      else
      {
        osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
    cliFormat(ewsContext, pStrInfo_common_JoinPruneIntvlSecs);
      ewsTelnetWrite(ewsContext,buf);  /* Join Prune Interval */

      memset(buf, L7_NULL, sizeof(buf));
      if ((usmDbPimsmNeighborCountGet(unit, L7_AF_INET, intIface,
                                      &val) == L7_SUCCESS))
    {
      sprintf(buf, "%u", val);
    }
    else
    {
      osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
    }
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_NeighborCount);
      ewsTelnetWrite(ewsContext,buf); /* Neighbor Count */

      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbPimsmInterfaceDRGet(unit, L7_AF_INET,
                                   intIface, &inetIpAddr) == L7_SUCCESS)
      {
        inetAddressGet(L7_AF_INET, &inetIpAddr, &ipAddr);
        rc = usmDbInetNtoa(ipAddr, buf);
      }
      else
      {
        osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
    cliFormat(ewsContext, pStrInfo_common_DesignatedRtr);
      ewsTelnetWrite(ewsContext,buf);  /* Designated Router */

      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbPimsmInterfaceDRPriorityGet(unit, L7_AF_INET,
                                   intIface, &val) == L7_SUCCESS)
    {
      sprintf(buf, "%u", val);
    }
    else
    {
      osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
    }
    cliFormat(ewsContext, pStrInfo_common_DrPri);
      ewsTelnetWrite(ewsContext,buf); /* DR Priority */

      memset(buf, L7_NULL, sizeof(buf));
      if ((usmDbPimsmInterfaceBsrBorderGet(unit, L7_AF_INET,
                                           intIface, &val) == L7_SUCCESS))
      {
        osapiSnprintf(buf,sizeof(buf),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
      }
      else
      {
        osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
    cliFormat(ewsContext, pStrInfo_common_BsrBorder);
      ewsTelnetWrite(ewsContext, buf); /* BSR Border */

      if (all != L7_TRUE)
      {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
      }

      lineCount++;

      rc= usmDbPimsmInterfaceEntryNextGet(unit, L7_AF_INET, &intIface);
                                                /* Get the next entry */

      if ((lineCount >= CLI_MAX_SCROLL_LINES-6) && (rc == L7_SUCCESS))
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

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays the reason for pruning of the downstream interface belonging to the
* specified nexthopgroup ip
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
* @cmdsyntax    show router pimsm nexthopprunereason <nexthopgroup>
*
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const char *commandShowRouterPimsmNextHopPruneReason(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 argNextHopGroup = 1;
    L7_uint32 pruneReason, mode = L7_DISABLE;
    L7_uint32 val, intSlot, intPort;
    L7_uint32 ipVal;
    L7_uint32 unit;
    L7_uint32 ipMRouteNextHopGroup, ipMRouteNextHopSource;
    L7_uint32 ipMRouteNextHopMask, ipMRouteNextHopIfIndex;
    L7_uint32 ipMRouteNextHopAddress;
    L7_inet_addr_t inetipMRouteNextHopGroup, inetipMRouteNextHopSource;
    L7_inet_addr_t inetipMRouteNextHopMask, inetipMRouteNextHopAddress;
    L7_inet_addr_t inetIpVal;

    cliSyntaxTop(ewsContext);

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    if (cliNumFunctionArgsGet() != 1)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowRtrPimSmNextHopPruneReason);
    }

    if ((usmDbPimsmAdminModeGet (unit, L7_AF_INET, &mode) != L7_SUCCESS) ||
        (mode != L7_ENABLE))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_ipmcast_MustInitPimSm);
  }

    OSAPI_STRNCPY_SAFE(stat,argv[index + argNextHopGroup]);

    if (usmDbInetAton(stat, (L7_uint32 *)&ipVal) != L7_SUCCESS)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_common_McastInvalidIpAddr);
    }

    ipMRouteNextHopGroup =0;
    ipMRouteNextHopSource = 0;
    ipMRouteNextHopMask = 0;
    ipMRouteNextHopIfIndex = 0;
    ipMRouteNextHopAddress = 0;
    inetAddressSet(L7_AF_INET, &ipMRouteNextHopGroup,
                               &inetipMRouteNextHopGroup);
    inetAddressSet(L7_AF_INET, &ipMRouteNextHopSource,
                               &inetipMRouteNextHopSource);
    inetAddressSet(L7_AF_INET, &ipMRouteNextHopMask,
                               &inetipMRouteNextHopMask);
    inetAddressSet(L7_AF_INET, &ipMRouteNextHopAddress,
                               &inetipMRouteNextHopAddress);

    while (usmDbPimsmIpMRouteNextHopEntryNextGet(unit, L7_AF_INET,
          &inetipMRouteNextHopGroup,  &inetipMRouteNextHopSource,
          &inetipMRouteNextHopMask, &ipMRouteNextHopIfIndex,
          &inetipMRouteNextHopAddress) == L7_SUCCESS)
    {

        if (ipMRouteNextHopGroup == ipVal)
        {
              if( usmDbPimsmIpMRouteNextHopPruneReasonGet(unit, L7_AF_INET,
                    &inetIpVal, &inetipMRouteNextHopGroup,
                    &inetipMRouteNextHopMask,
                    ipMRouteNextHopIfIndex, &inetipMRouteNextHopAddress,
                    &pruneReason) == L7_SUCCESS)
              {

                  memset(stat, L7_NULL, sizeof(stat));
                  memset(buf, L7_NULL, sizeof(buf));
        cliFormat(ewsContext, pStrInfo_ipmcast_NextHopSrc);
                  if (usmDbInetNtoa(ipMRouteNextHopSource, buf) == L7_SUCCESS)
                  {
          osapiSnprintf(stat, sizeof(stat), buf);
                  }
                  else
                  {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
                  }
                  ewsTelnetWrite(ewsContext, stat);

                  memset(stat, L7_NULL, sizeof(stat));
                  memset(buf, L7_NULL, sizeof(buf));
        cliFormat(ewsContext, pStrInfo_ipmcast_NextHopSrcMask);
                  if (usmDbInetNtoa(ipMRouteNextHopMask, buf) == L7_SUCCESS)
                  {
          osapiSnprintf(stat, sizeof(stat), buf);
                  }
                  else
                  {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
                  }
                  ewsTelnetWrite(ewsContext, stat);

                  memset(buf, L7_NULL, sizeof(buf));
                  memset(stat, L7_NULL, sizeof(stat));
                  val = 0;
        cliFormat(ewsContext, pStrInfo_ipmcast_NextHopIntfIdx);
                  if (usmDbUnitSlotPortGet(ipMRouteNextHopIfIndex, &val, &intSlot, &intPort) == L7_SUCCESS)
                  {
          osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
                  }
                  else
                  {
          osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
                  }
        ewsTelnetPrintf (ewsContext, buf);

        memset(stat, L7_NULL, sizeof(stat));
        memset(buf, L7_NULL, sizeof(buf));
        cliFormat(ewsContext, pStrInfo_ipmcast_NextHopAddr_1);
                  if (usmDbInetNtoa(ipMRouteNextHopAddress, buf) == L7_SUCCESS)
                  {
          osapiSnprintf(stat, sizeof(stat), buf);
                  }
                  else
                  {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
                  }
                  ewsTelnetWrite(ewsContext, stat);

                  memset(stat, L7_NULL, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_ipmcast_MrouteNextHopPruneReason);
                  switch (pruneReason)
                  {
                    case L7_PIMSM_PRUNEREASON_OTHER:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Other);
                      break;
                    case L7_PIMSM_PRUNEREASON_PRUNE:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_ipmcast_Prune);
                      break;
                    case L7_PIMSM_PRUNEREASON_ASSERT:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_ipmcast_Assert);
                      break;
                    default:
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
                  }
                  ewsTelnetWrite(ewsContext, stat);
              }
        }
        cliSyntaxBottom(ewsContext);
    }

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose  Displays the router's PIM neighbor for the specified interface
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip pimsm neighbor [{<slot/port>|all}]
*
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const char *commandShowIpPimsmNeighbor(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  static L7_uint32 intIfIndex, nextCount;
  static L7_inet_addr_t nbrIpAddr;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 all=L7_FALSE;
  L7_uint32 mode = L7_DISABLE;
  L7_int32 u = 1;
  L7_uint32 intIface = 0, s, p, slot, port;
  L7_uint32 unit, numArgs, drPriority = L7_NULL;
  usmDbTimeSpec_t timeSpec;
  L7_RC_t rc = L7_FAILURE;
  L7_char8 altCmdStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 count;
  L7_char8  *dispbuf = L7_NULLPTR;

  numArgs = cliNumFunctionArgsGet();
  if ((numArgs != 0) && (numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_ipmcast_ShowIpPimSmNeighbor_1, cliSyntaxInterfaceHelp());
  }

  memset(altCmdStr, L7_NULL, sizeof(altCmdStr));
  for(count=0; count<argc; count++)
  {
    osapiStrncat(altCmdStr, argv[count], strlen(argv[count]));
    osapiStrncat(altCmdStr, " ", 1);
  }
  
  cliCmdScrollSet( L7_FALSE);

  if ((numArgs == 0) || (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0))
  {
    all = L7_TRUE;
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if (tolower(cliGetCharInput()) == 'q')
    {
      intIfIndex = 0; nextCount = 1;
      inetAddressZeroSet(L7_AF_INET, &nbrIpAddr);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
    intIface = intIfIndex;
    rc = L7_SUCCESS;
  }
  else /* coming into the command function for the first time */
  {
    intIfIndex = 0; /* First Entry */
    nextCount = 1; /* First field in the record */
    inetAddressZeroSet(L7_AF_INET, &nbrIpAddr);

    cliSyntaxTop(ewsContext);

    if ((numArgs != 0) && (strcmp(argv[index+argSlotPort], pStrInfo_common_All) != 0))
    {
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if (cliValidSpecificUSPCheck(argv[index+argSlotPort],
                                        &unit, &s, &p) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }

        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIface) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
      else
      {
        if (cliSlotPortToInterface(argv[index + argSlotPort],
                                      &unit, &s, &p, &intIface) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }

      if (cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }

    if ((usmDbPimsmAdminModeGet (unit, L7_AF_INET, &mode) != L7_SUCCESS) ||
        (mode != L7_ENABLE))
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_MustInitPimSm);
    }

    if (intIface != L7_NULL)
    {
      intIfIndex = intIface;
    }
    /* Get the first neighbor*/
    rc = usmDbPimsmNeighborEntryNextGet(unit,  L7_AF_INET, &intIfIndex,
                                              &nbrIpAddr);
    if (rc != L7_SUCCESS)
    {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_PimSmNoNeighbors);
    }
  }

  count = 1;
  while (count <= (CLI_MAX_SCROLL_LINES-2))
  {
    /* Check if the neighbor belongs to this interface*/
    if( !all )
    {
      if (intIface != intIfIndex)
      {
        /*Get the next neighbor*/
        rc = usmDbPimsmNeighborEntryNextGet(unit,  L7_AF_INET,
                   &intIfIndex, &nbrIpAddr);
        if (rc != L7_SUCCESS)
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt (ewsContext, "");          
        }
        continue;
      }
    }
    else
    {
      intIface = intIfIndex;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-2)) && (nextCount == 1) )
    {
      memset(buf, L7_NULL, sizeof(buf));
      if ( usmDbUnitSlotPortGet(intIface, &u, &slot, &port) == L7_SUCCESS)
      {
        dispbuf = cliDisplayInterfaceHelp(u, slot, port);
        osapiSnprintf(buf, sizeof(buf), "%s", dispbuf);  
      }
      else
      {
        osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
      cliFormat(ewsContext, pStrInfo_common_SlotPortWithoutUnit_1);
      ewsTelnetWrite(ewsContext, buf);     /* Slot/Port */
      count++;
      nextCount = 2;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-2)) && (nextCount == 2) )
    {
      memset(stat, L7_NULL, sizeof(stat));
      memset(buf, L7_NULL, sizeof(buf));
      if (inetAddrHtop(&nbrIpAddr, buf) == L7_SUCCESS)
      {
        strcpy(stat, buf);
      }
      else
      {
        osapiStrncpySafe(stat, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
      cliFormat(ewsContext, pStrInfo_common_NeighborAddr_1);
      ewsTelnetWrite(ewsContext, stat); /* Neigbor Address */
      count++;
      nextCount = 3;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-2)) && (nextCount == 3) )
    {  
      memset(buf, L7_NULL, sizeof(buf));
      if ((usmDbPimsmNeighborUpTimeSpecGet(unit, L7_AF_INET, intIfIndex,
                                       &nbrIpAddr, &timeSpec) == L7_SUCCESS))
      {
        sprintf(buf,"%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                       timeSpec.minutes,timeSpec.seconds);
      }
      else
      {
        osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
      cliFormat(ewsContext, pStrInfo_common_UpTimeHhMmSs);
      ewsTelnetWrite(ewsContext, buf); /* Neighbor Up Time */
      count++;
      nextCount = 4;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-2)) && (nextCount == 4) )
    {
      memset(buf, L7_NULL, sizeof(buf));
      if ((usmDbPimsmNeighborExpiryTimeSpecGet(unit, L7_AF_INET,
                           intIfIndex, &nbrIpAddr, &timeSpec) == L7_SUCCESS))
      {
        sprintf(buf,"%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                      timeSpec.minutes,timeSpec.seconds);
      }
      else
      {
        osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
      cliFormat(ewsContext, pStrInfo_common_ExpiryTimeHhMmSs);
      ewsTelnetWrite(ewsContext, buf); /* Neighbor Expiry Time */
      count++;
      nextCount = 5;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-2)) && (nextCount == 5) )
    {
      memset(buf, L7_NULL, sizeof(buf));
      if ((usmDbPimsmNeighborDRPriorityGet(unit,  L7_AF_INET, intIfIndex,
              &nbrIpAddr, &drPriority) == L7_SUCCESS))
      {
        sprintf(buf, "%u", drPriority);
      }
      else
      {
        osapiStrncpySafe(buf, pStrErr_common_Err, sizeof(pStrErr_common_Err));
      }
      cliFormat(ewsContext, pStrInfo_common_DrPri);
      ewsTelnetWrite(ewsContext, buf); /* DR Priority */
      count++;
      nextCount = 6;
    }

    /* This should be last the condition in the record. If any field needs to be added
       in the record, adjust nextCount accordingly. */
    if( (count <= (CLI_MAX_SCROLL_LINES-2)) && (nextCount == 6) )
    {
      cliSyntaxBottom(ewsContext);
      count++;
      nextCount = 1;
    }

    if((count > (CLI_MAX_SCROLL_LINES-2)) && (nextCount != 1))
    {
      break;
    }
    else
    {
      if(usmDbPimsmNeighborEntryNextGet(unit,  L7_AF_INET, &intIfIndex, &nbrIpAddr) != L7_SUCCESS)
      {
        intIfIndex = 0; nextCount = 1;
        inetAddressZeroSet(L7_AF_INET, &nbrIpAddr);
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");

      }
    }
  }/* end of while*/
  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliAlternateCommandSet(altCmdStr);
  return pStrInfo_common_Name_2;
}

/*********************************************************************
*
* @purpose  Displays the pim information for the candidate RP's for
*           the specified
* multicast group addres or the group mask
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip pimsm rp mapping [rp-address]
*
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const char *commandShowIpPimsmRpMapping(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
    L7_uint32 argRpAddress = 2;
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 rpIpStr[L7_CLI_MAX_STRING_LENGTH], origin = L7_NULL;
    L7_uint32 mode = L7_DISABLE;
    L7_uint32 unit, numArgs;
    L7_inet_addr_t rpAddr;
    L7_inet_addr_t inetRpGroupAddress, inetRpAddress, inetRpGroupMask;
    L7_RC_t   rc = L7_FAILURE;

    cliSyntaxTop(ewsContext);
    inetAddressZeroSet(L7_AF_INET, &rpAddr);

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    numArgs = cliNumFunctionArgsGet();
    if ((numArgs != 1) && (numArgs != 2))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpPimSmRp_1);
    }

    if ((usmDbPimsmAdminModeGet (unit, L7_AF_INET, &mode) != L7_SUCCESS) ||
        (mode != L7_ENABLE))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_ipmcast_MustInitPimSm);
    }

    if(numArgs == 2)
    {
    osapiStrncpySafe(rpIpStr, argv[index + argRpAddress], sizeof(rpIpStr));

        if (usmDbParseInetAddrFromStr(rpIpStr, &rpAddr) != L7_SUCCESS)
        {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_common_McastInvalidIpAddr);
        }
    }

    inetAddressZeroSet(L7_AF_INET, &inetRpGroupAddress);
    inetAddressZeroSet(L7_AF_INET, &inetRpAddress);
    inetAddressZeroSet(L7_AF_INET, &inetRpGroupMask);

    rc = usmDbPimsmRpGrpMappingEntryNextGet(unit, L7_AF_INET, &origin,
        &inetRpGroupAddress, &inetRpGroupMask,
        &inetRpAddress);

    while (rc == L7_SUCCESS)
    {
       if(numArgs == 2)
       {
         if (L7_INET_IS_ADDR_EQUAL(&inetRpAddress, &rpAddr) != L7_TRUE)
                               /* Matching Address could Not Found*/
         {
             rc = usmDbPimsmRpGrpMappingEntryNextGet(unit, L7_AF_INET, &origin,
                 &inetRpGroupAddress, &inetRpGroupMask,
                 &inetRpAddress);
             continue;
         }
       }
       memset(stat, L7_NULL, sizeof(stat));
       memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_common_GrpAddr_1);
       if (inetAddrHtop(&inetRpGroupAddress, buf) == L7_SUCCESS)
       {
      strcpy(stat, buf);
       }
       else
       {
         osapiStrncpySafe(stat, pStrErr_common_Err, sizeof(pStrErr_common_Err));
       }
       ewsTelnetWrite(ewsContext, stat); /* Group Address */

       memset(stat, L7_NULL, sizeof(stat));
       memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_common_GrpMask_1);
       if (inetAddrHtop(&inetRpGroupMask, buf) == L7_SUCCESS)
       {
      strcpy(stat, buf);
       }
       else
       {
         osapiStrncpySafe(stat, pStrErr_common_Err, sizeof(pStrErr_common_Err));
       }
       ewsTelnetWrite(ewsContext, stat); /* Group Mask */

       memset(stat, L7_NULL, sizeof(stat));
       memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_common_RpAddr_1);
       if (inetAddrHtop(&inetRpAddress, buf) == L7_SUCCESS)
       {
      strcpy(stat, buf);
       }
       else
       {
         osapiStrncpySafe(stat, pStrErr_common_Err, sizeof(pStrErr_common_Err));
       }
       ewsTelnetWrite(ewsContext, stat); /* RP Address */

       memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_common_Origin_1);
       if (origin == CLIPIMSM_GROUP_RP_MAP_ORIGIN_BSR)
    {
      osapiSnprintf(buf, sizeof(buf), "%s", pStrInfo_common_PimSmGrpRpMapBsr);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "%s", pStrInfo_common_PimSmGrpRpMapStatic); 
    }
    ewsTelnetWrite(ewsContext, buf);    /* Origin */

    rc = usmDbPimsmRpGrpMappingEntryNextGet(unit, L7_AF_INET, &origin,
                                            &inetRpGroupAddress, &inetRpGroupMask,
                                            &inetRpAddress);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays pimsm static RP by group
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip pimsm rphash <groupaddress>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const char *commandShowIpPimsmRpHash(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_char8       buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8       stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8       groupIpStr[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32      unit;
   L7_uchar8      origin;
  L7_inet_addr_t inetGroupIp, inetRpIpAddr;

   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   if (cliNumFunctionArgsGet() != 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpPimSmRpHash_1);
  }

   OSAPI_STRNCPY_SAFE(groupIpStr,argv[index + 1]);
   if (usmDbParseInetAddrFromStr(groupIpStr, &inetGroupIp) != L7_SUCCESS)
   {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpIp);
   }

   if(inetIsInMulticast(&inetGroupIp) != L7_TRUE)
   {
     cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_UserInput, ewsContext,
                   pStrErr_common_UsrInputGrpAddrMcastRange);
   }

   inetAddressZeroSet(L7_AF_INET, &inetRpIpAddr);
   if (usmDbPimsmGroupToRPMappingGet(L7_AF_INET, &inetGroupIp, &inetRpIpAddr,
                                     &origin) == L7_SUCCESS)
   {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_RpType_1);
    ewsTelnetWriteAddBlanks (1, 0, 0, 15, L7_NULLPTR, ewsContext,pStrInfo_common_Addr_2);
       ewsTelnetWrite(ewsContext,"\r\n---------------- -----\r\n");

       memset(stat, L7_NULL, sizeof(stat));
       memset(buf, L7_NULL, sizeof(buf));
       if (inetAddrHtop(&inetRpIpAddr, buf) == L7_SUCCESS)
       {
          sprintf(stat,"%-17s", buf);
       }
       else
       {
      sprintf(stat,"%-17s", pStrErr_common_Err);
       }
       ewsTelnetWrite(ewsContext, stat); /* RP Address */

       memset(buf, L7_NULL, sizeof(buf));
       if (origin == CLIPIMSM_GROUP_RP_MAP_ORIGIN_BSR)
    {
      sprintf(buf, "%-6s", pStrInfo_common_PimSmGrpRpMapBsr);
    }
    else
    {
      sprintf(buf, "%-6s", pStrInfo_common_PimSmGrpRpMapStatic);
    }
    ewsTelnetWrite(ewsContext, buf);        /* Origin */
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpHashNorpsGrp);
   }
   return cliPrompt(ewsContext);
}
