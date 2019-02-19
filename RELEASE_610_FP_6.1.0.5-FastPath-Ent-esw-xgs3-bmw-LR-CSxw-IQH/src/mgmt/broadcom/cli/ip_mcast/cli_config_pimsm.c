/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/ip_mcast/cli_config_pimsm.c
*
* @purpose config commands for the cli
*
* @component user interface
*
* @comments
*
* @create  04/05/2002
*
* @author  srikrishnas
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_cli.h"
#include "l3_mcast_commdefs.h"
#include "l3_mcast_defaultconfig.h"
#include "cliapi.h"
#include <usmdb_mib_pimsm_api.h>
#include <usmdb_pimsm_api.h>
#include "clicommands_mcast.h"
#include "cli_config_script.h"

#include "clicommands_card.h"
#include "usmdb_ip_api.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"

#include "usmdb_mib_pim_rfc5060_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_mcast_api.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_pimsm_api.h"
#endif



/*********************************************************************
* @purpose    Configures the PIM-SM administrative mode of
* the router to enable or disable.
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 argMode = 1;
    L7_uint32 unit, numArgs, intMode=L7_DISABLE, protocol;
    L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
    L7_char8 protoStat[L7_CLI_MAX_STRING_LENGTH];
    L7_RC_t rc = L7_FAILURE;

    cliSyntaxTop(ewsContext);

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    numArgs = cliNumFunctionArgsGet();
    if((ewsContext->commType == CLI_NORMAL_CMD) && (( numArgs != 0 ) && (numArgs != 1)))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSm_1);
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmModeNo);
    }

    if(ewsContext->commType == CLI_NO_CMD)
    {
      intMode=FD_PIMSM_DEFAULT_ADMIN_MODE;
    }
    else if((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs == 0))
    {
      intMode=L7_ENABLE;
    }
    else if((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs == 1))
    {
      OSAPI_STRNCPY_SAFE(strMode,argv[index+ argMode]);
      cliConvertToLowerCase(strMode);
    if (strcmp(strMode, pStrInfo_common_Lvl7Clr) == 0)
      {
             usmDbPimsmClearRoutes(unit);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Successful);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
     rc = usmDbPimsmAdminModeSet(unit, L7_AF_INET, intMode);
    if (rc != L7_SUCCESS )
    {
      if ((intMode == L7_ENABLE) && (rc == L7_ERROR))
      {
        usmDbMcastIpProtocolGet(unit, L7_AF_INET, &protocol);
        cliMcastProtocolStrPrint(protocol, stat, sizeof(stat), "%s");
        osapiSnprintf(protoStat,sizeof(protoStat),pStrInfo_ipmcast_McastProtoCfgured, stat); 
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, protoStat);
      }
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_SetPimSmAdminModeOtherMcastProtoMightBeEnbld);
    }
  }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the PIM-SM SSM
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm ssm {default|<group-address><group-mask>}
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsmSsm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 unit, numArgs, ssmMode = FD_PIMSM_SSM_MODE, grpCheckMask =0;
    L7_uint32 argGrpAddress = 1;
    L7_char8  strGrpAddress[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 argGrpMask = 2;
    L7_char8  strGrpMask[L7_CLI_MAX_STRING_LENGTH];
    L7_inet_addr_t groupAddr, groupMask;
    L7_RC_t rc = L7_FAILURE;
    L7_uchar8  strDefSSMGrpAddr[L7_CLI_MAX_STRING_LENGTH];
    L7_uchar8  defSSMGrpMaskLen = 8;
    L7_inet_addr_t inetDefSSMGrpAddr;

    cliSyntaxTop(ewsContext);

    inetAddressZeroSet(L7_AF_INET, &groupAddr);
    inetAddressZeroSet(L7_AF_INET, &groupMask);

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    numArgs = cliNumFunctionArgsGet();
    if((ewsContext->commType == CLI_NORMAL_CMD) &&
       (numArgs != 1) && (numArgs != 2))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSm);
    }
    else if((ewsContext->commType == CLI_NO_CMD) &&
            (numArgs != 1) && (numArgs != 2))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSm_2);
    }

    if (((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs == 2))||
        ((ewsContext->commType == CLI_NO_CMD) && (numArgs == 2)))
    {
      if (strlen(argv[index+argGrpAddress]) >= sizeof(strGrpAddress))
      {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmGrpAddr);
      }

      if (strlen(argv[index+argGrpMask]) >= sizeof(strGrpMask))
      {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpMask);
      }

      memset(strGrpAddress, L7_NULL, sizeof(strGrpAddress));
      memset(strGrpMask, L7_NULL, sizeof(strGrpMask));
      strcpy(strGrpAddress, argv[index + argGrpAddress]);
      strcpy(strGrpMask, argv[index + argGrpMask]);

      if (usmDbParseInetAddrFromStr(strGrpAddress, &groupAddr) != L7_SUCCESS)
      {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmStaticRpRpIp);
      }

      if (usmDbParseInetAddrFromStr(strGrpMask, &groupMask) != L7_SUCCESS)
      {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpMask);
      }
      inetAddressGet(L7_AF_INET, &groupMask, &grpCheckMask);

      if (usmDbNetmaskIsContiguous(grpCheckMask) != L7_TRUE)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NonContiguousMask);
      }
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /* Check if default range is already configured. If the current range  
         falls under default range, then reject this new one.  */

      osapiStrncpy(strDefSSMGrpAddr, FD_PIMSM_SSM_RANGE_GROUP_ADDRESS, 
                   sizeof(strDefSSMGrpAddr));
      usmDbParseInetAddrFromStr(strDefSSMGrpAddr, &inetDefSSMGrpAddr);

      if (usmDbPimsmSsmRangeEntryGet(L7_AF_INET, &inetDefSSMGrpAddr, defSSMGrpMaskLen)
                                      == L7_SUCCESS) 
      {
        if (inetAddrCompareAddrWithMask(&inetDefSSMGrpAddr, defSSMGrpMaskLen,
                                        &groupAddr, defSSMGrpMaskLen) == L7_NULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, 
                                                 ewsContext, pStrErr_ipmcast_PimSmDefSSMRangeConfigured);
        }
      }
      ssmMode = L7_ENABLE;
    }
    else
    {
      ssmMode = FD_PIMSM_SSM_MODE;
    }
  }
  else if (((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs == 1))||((ewsContext->commType == CLI_NO_CMD) && (numArgs == 1)))
  {
      memset(strGrpAddress, L7_NULL, sizeof(strGrpAddress));
      memset(strGrpMask, L7_NULL, sizeof(strGrpMask));
      strcpy(strGrpAddress, FD_PIMSM_SSM_RANGE_GROUP_ADDRESS);
      strcpy(strGrpMask, FD_PIMSM_SSM_RANGE_GROUP_MASK);

      if (usmDbParseInetAddrFromStr(strGrpAddress,
                                    &groupAddr) != L7_SUCCESS)
      {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmStaticRpRpIp);
      }

      if (usmDbParseInetAddrFromStr(strGrpMask, &groupMask) != L7_SUCCESS)
      {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpIp);
      }

      inetAddressGet(L7_AF_INET, &groupMask, &grpCheckMask);

      if (usmDbNetmaskIsContiguous(grpCheckMask) != L7_TRUE)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NonContiguousMask);
      }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ssmMode = L7_ENABLE;
    }
    else
    {
        ssmMode = FD_PIMSM_SSM_MODE;
    }
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
      rc = usmDbPimsmSsmRangeSet(unit, L7_AF_INET, ssmMode,
                                &groupAddr, &groupMask);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_ipmcast_PimsmSSMTableFull);
        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_ipmcast_PimSmSSMRangeConfigured);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_ipmcast_PimsmSSMRangeSet);
        }
      }
  }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the PIM-SM data threshold Rate
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm spt-threshold <0-2000>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsmSptThreshold(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 argThresholdrate = 1;
    L7_char8 strThresholdRate[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 thresholdRate=FD_PIMSM_DEFAULT_DATA_THRESHOLD_RATE;
    L7_uint32 unit, numArgs;

    cliSyntaxTop(ewsContext);

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    numArgs = cliNumFunctionArgsGet();
    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmSptThresh);
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmSptThreshNo);
    }

    /*     check for valid threshold rate  */
    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs == 1))
    {
      OSAPI_STRNCPY_SAFE(strThresholdRate,argv[index+ argThresholdrate]);
      if ( cliCheckIfInteger(strThresholdRate) != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmSptThresh);
      }
      else
    {
           (void)cliConvertTo32BitUnsignedInteger(strThresholdRate, &thresholdRate);
    }
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs == 0 ))
  {
       thresholdRate = FD_PIMSM_DEFAULT_DATA_THRESHOLD_RATE;
  }

   /* threshold rate should be between 0 and 2000 */
   if ( (thresholdRate >= L7_PIMSM_DATATHRESHOLD_RATE_MIN) &&
        (thresholdRate <= L7_PIMSM_DATATHRESHOLD_RATE_MAX) )
   {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        if (usmDbPimsmDataThresholdRateSet(unit, L7_AF_INET,
                            thresholdRate) != L7_SUCCESS )
        {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_SetDataThreshRate);
        }
   }
  }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Range);
   }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the PIM-SM join prune interval
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm join-prune-interval <0-18000>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsmJoinPruneInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{

    L7_uint32 argJoinPruneIntvl = 1;
    L7_uint32 iface;
    L7_char8  strJoinPruneIntvl[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 joinPruneIntvl = FD_PIMSM_DEFAULT_JOIN_PRUNE_INTERVAL;
    L7_uint32 unit, numArgs, slot, port;

    cliSyntaxTop(ewsContext);

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    numArgs = cliNumFunctionArgsGet();

    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmJoinPruneIntvl);
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmJoinPruneIntvlNo);
    }
    /* get switch ID based on presence/absence of STACKING package */
    if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot=EWSSLOT(ewsContext);
  port=EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs == 1 ))
    {
      /* check for join prune interval  */
    osapiStrncpySafe(strJoinPruneIntvl, argv[index+ argJoinPruneIntvl], sizeof(strJoinPruneIntvl));
      if ( cliCheckIfInteger(strJoinPruneIntvl) != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmJoinPruneIntvl);
      }
      else
      {
           (void)cliConvertTo32BitUnsignedInteger(strJoinPruneIntvl, &joinPruneIntvl);
      }
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs == 0 ))
    {
         joinPruneIntvl = FD_PIMSM_DEFAULT_JOIN_PRUNE_INTERVAL;
    }

    /* join prune interval should be between 0 and 18000 */
    if ( (joinPruneIntvl >= L7_PIMSM_INTERFACE_JOINPRUNE_INTERVAL_MIN) &&
         (joinPruneIntvl <= L7_PIMSM_INTERFACE_JOINPRUNE_INTERVAL_MAX) )
    {
         /*******Check if the Flag is Set for Execution*************/
         if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
         if (usmDbPimsmInterfaceJoinPruneIntervalSet(unit, L7_AF_INET, iface,
                                               joinPruneIntvl) != L7_SUCCESS)
         {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_SetJoinPruneIntvl);
         }
    }
  }
    else
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Range);
    }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);

}

/*********************************************************************
* @purpose    Configures the PIM-SM register threshold rate
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm register-threshold <0-2000>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsmRegisterThreshold(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 argRegThresholdRate = 1;
    L7_char8 strRegThresholdRate[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 regThresholdRate = FD_PIMSM_DEFAULT_REG_THRESHOLD_RATE;
    L7_uint32 unit, numArgs;

    cliSyntaxTop(ewsContext);

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    numArgs = cliNumFunctionArgsGet();

    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmRegisterThresh);
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmRegisterThreshNo);
    }

    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs == 1 ))
    {
    /*  check for valid register threshold rate  */
    osapiStrncpySafe(strRegThresholdRate, argv[index+ argRegThresholdRate], sizeof(strRegThresholdRate));
      if ( cliCheckIfInteger(strRegThresholdRate) != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmRegisterThresh);
      }
      else
      {
              (void)cliConvertTo32BitUnsignedInteger(strRegThresholdRate,
                                               &regThresholdRate);
      }
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs == 0 ))
    {
         regThresholdRate = FD_PIMSM_DEFAULT_REG_THRESHOLD_RATE;
    }

    /* threshold rate should be between 0 and 2000 */
    if ( ( regThresholdRate >= L7_PIMSM_REGTHRESHOLD_RATE_MIN) &&
         ( regThresholdRate <= L7_PIMSM_REGTHRESHOLD_RATE_MAX) )
    {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        if ( usmDbPimsmRegisterThresholdRateSet(unit, L7_AF_INET,
                                              regThresholdRate) != L7_SUCCESS)
        {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_SetRegisterThreshRate);
        }
    }
  }
    else
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Range);
    }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the PIM-SM hello Interval for the specified interface
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm hello-interval <0-18000>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsmHelloInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 argHelloIntvl = 1;
    L7_uint32 iface;
    L7_char8 strHelloInterval[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 helloInterval = 0;
    L7_uint32 unit, numArgs, slot, port;

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    cliSyntaxTop(ewsContext);

    numArgs = cliNumFunctionArgsGet();
    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmHelloIntvl);
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmHelloIntvlNo);
    }

    /* get switch ID based on presence/absence of STACKING package */
    if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot=EWSSLOT(ewsContext);
  port=EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs == 1 ))
    {
     /* check for valid Hello Interval  */
     OSAPI_STRNCPY_SAFE(strHelloInterval,argv[index+ argHelloIntvl]);
     if ( cliCheckIfInteger(strHelloInterval) != L7_SUCCESS)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmHelloIntvl);
     }
     else
     {
           (void)cliConvertTo32BitUnsignedInteger(strHelloInterval, &helloInterval);
     }
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs == 0 ))
    {
       helloInterval = FD_PIMSM_DEFAULT_INTERFACE_HELLO_INTERVAL;
    }

   /* Hello Interval should be between 0 and 18000 */
   if ( ( helloInterval >= L7_PIMSM_INTERFACE_HELLO_INTERVAL_MIN) && ( helloInterval <= L7_PIMSM_INTERFACE_HELLO_INTERVAL_MAX) )
   {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        if ( usmDbPimIntfHelloIntervalSet(unit, L7_AF_INET, iface,
                                                 helloInterval) != L7_SUCCESS)
        {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_SetHelloIntvl);
        }
   }
  }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Range);
   }

   /*************Set Flag for Script Successful******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
   return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the PIM-SM  mode  on the interface
* the router to enable or disable.
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIntfIpPimsm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 intMode = L7_DISABLE;
    L7_uint32 iface;
    L7_uint32 unit, slot, port, numArgs;

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    cliSyntaxTop(ewsContext);

    numArgs = cliNumFunctionArgsGet();
    if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSm_1);
    }
    else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmModeNo);
    }

    /* get switch ID based on presence/absence of STACKING package */
    if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot=EWSSLOT(ewsContext);
  port=EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
        intMode = L7_ENABLE;
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
        intMode = FD_PIMSM_DEFAULT_INTERFACE_MODE;
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbPimInterfaceModeSet(unit, L7_AF_INET, iface,
                                    intMode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_IntfAdminModeFailure);
    }
    }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the PIM-SM Static RP
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm rp-address <rp-address> <group-address> <group-mask>
*             [override]
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsmRpAddress(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8  ipAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  groupAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  groupMaskStr[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL   conflict = L7_FALSE;
  L7_uint32 ipAddr;
  L7_uint32 groupAddr;
  L7_uint32 groupMask;
  L7_uint32 rc;
  L7_uint32 numArgs;
  L7_uchar8 prefixLen = L7_NULL;
  L7_inet_addr_t inetGrpAddr, inetGrpMask, inetIpAddr;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();
  inetAddressZeroSet(L7_AF_INET, &inetGrpAddr);
  inetAddressZeroSet(L7_AF_INET, &inetGrpMask);
  inetAddressZeroSet(L7_AF_INET, &inetIpAddr);

  if ((ewsContext->commType == CLI_NORMAL_CMD) && ((numArgs < 3 )
                                                 || (numArgs > 4)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmRpAddr);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 3 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmRpAddrNo);
  }

  if (strlen(argv[index+1]) >= sizeof(ipAddrStr))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmStaticRpRpIp);
  }

  if (strlen(argv[index+2]) >= sizeof(groupAddrStr))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpIp);
  }

  if (strlen(argv[index+3]) >= sizeof(groupMaskStr))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpMask);
  }

  memset(ipAddrStr, L7_NULL, sizeof(ipAddrStr));
  memset(groupAddrStr, L7_NULL, sizeof(groupAddrStr));
  memset(groupMaskStr, L7_NULL, sizeof(groupMaskStr));
  OSAPI_STRNCPY_SAFE(ipAddrStr,argv[index + 1]);
  OSAPI_STRNCPY_SAFE(groupAddrStr,argv[index + 2]);
  OSAPI_STRNCPY_SAFE(groupMaskStr,argv[index + 3]);

  if (usmDbInetAton(ipAddrStr, &ipAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmStaticRpRpIp);
  }

  if (usmDbInetAton(groupAddrStr, &groupAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpIp);
  }
  if (usmDbInetAton(groupMaskStr, &groupMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpMask);
  }

if (usmDbNetmaskIsContiguous(groupMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NonContiguousMask);
  }
  inetAddressSet(L7_AF_INET, &groupAddr, &inetGrpAddr);
  inetAddressSet(L7_AF_INET, &groupMask, &inetGrpMask);
  inetAddressSet(L7_AF_INET, &ipAddr, &inetIpAddr);

  if (inetMaskToMaskLen(&inetGrpMask, &prefixLen) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpMask);
  }
  if (numArgs == 4)
  {
    conflict = L7_TRUE;
  }
  else if (numArgs == 3)
  {
    conflict = L7_FALSE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NO_CMD)
    {
      rc = usmDbPimsmStaticRPRemove(L7_AF_INET, &inetIpAddr,
                                     &inetGrpAddr, prefixLen);
    }
    else
    {
      rc = usmDbPimsmStaticRPSet(L7_AF_INET, &inetIpAddr,
                                  &inetGrpAddr, prefixLen, conflict);
    }

    if ( rc != L7_SUCCESS)
    {
      if (ewsContext->commType == CLI_NO_CMD)
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_PimSmStaticRpDeletion);
      }
      else
      {
        if(rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPrompt (ewsContext,pStrErr_common_PimSmStaticRpMax, L7_PIMSM_MAX_STATIC_RP_NUM);
        }
        else if(rc == L7_ALREADY_CONFIGURED)
        {
          return cliSyntaxReturnPrompt (ewsContext,pStrErr_common_PimSmStaticRpDuplicate);
        }
        else
        {
          ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_PimSmStaticRpCreation);
        }
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the PIM-SM BSR Border for the specified interface.
*
* @param      EwsContext       ewsContext
* @param      L7_uint32        argc
* @param      const L7_char8   **argv
* @param      L7_uint32        index
* @returntype const            L7_char8
* @returns    cliPrompt        (ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm bsr-border
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8  *commandIpPimsmBsrBorder(EwsContext ewsContext,
                                        L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 unit, numArgs, slot, port;
    L7_uint32 iface;

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    cliSyntaxTop(ewsContext);

    numArgs = cliNumFunctionArgsGet();

    if((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs != 0))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmBsrBorder_1);
    }
    else if((ewsContext->commType == CLI_NO_CMD) && (numArgs != 0))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmBsrBorderNo);
    }
    /* get switch ID based on presence/absence of STACKING package */
    if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot=EWSSLOT(ewsContext);
  port=EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
  }
  else if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        if (usmDbPimsmInterfaceBsrBorderSet(unit, L7_AF_INET, iface,
                                            L7_ENABLE) != L7_SUCCESS)
        {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_IpPimSmBsrBorderEnblError);
            cliSyntaxBottom(ewsContext);
        }
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        if (usmDbPimsmInterfaceBsrBorderSet(unit, L7_AF_INET, iface,
                                            L7_DISABLE) != L7_SUCCESS)
        {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_IpPimSmBsrBorderDsblError);
            cliSyntaxBottom(ewsContext);
        }
      }
    }
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
}
/*********************************************************************
* @purpose    Configures the PIM-SM DR Priority for the specified interface.
*
* @param      EwsContext       ewsContext
* @param      L7_uint32        argc
* @param      const L7_char8   **argv
* @param      L7_uint32        index
* @returntype const            L7_char8
* @returns    cliPrompt        (ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm dr-priority <0 - 4294967294>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8  *commandIpPimsmDrPriority(EwsContext ewsContext,
                                         L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 unit, numArgs, slot, port;
    L7_uint32 iface;
    L7_uint32 argDrPriority = 1;
    L7_char8  strDrPriority[L7_CLI_MAX_STRING_LENGTH];
    L7_uint32 drPriority =FD_PIMSM_DEFAULT_INTERFACE_DR_PRIORITY;

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    cliSyntaxTop(ewsContext);

    numArgs = cliNumFunctionArgsGet();

    if((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs != 1))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmDrPri_1);
    }
    else if((ewsContext->commType == CLI_NO_CMD) && (numArgs != 0))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmDrPriNo);
    }
    /* get switch ID based on presence/absence of STACKING package */
    if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot=EWSSLOT(ewsContext);
  port=EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }
  else if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* check for valid DR priority value  */
    if(numArgs == 1)
    {
    osapiStrncpySafe(strDrPriority, argv[index + argDrPriority], sizeof(strDrPriority));
        if (cliCheckIfInteger(strDrPriority) != L7_SUCCESS)
        {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimSmDrPri_1);
        }
        else
        {
            cliConvertTo32BitSignedInteger(strDrPriority, &drPriority);
        }
    }
    else if ((ewsContext->commType == CLI_NO_CMD)||(numArgs == 0))
    {
        drPriority =FD_PIMSM_DEFAULT_INTERFACE_DR_PRIORITY;
    }
    /* Check the DR Priority range */
    if ((drPriority >= L7_PIMSM_INTERFACE_DR_PRIORITY_MIN) &&
        (drPriority <= L7_PIMSM_INTERFACE_DR_PRIORITY_MAX))
    {
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
            if (usmDbPimsmInterfaceDRPrioritySet(unit, L7_AF_INET, iface,
                                                 drPriority) != L7_SUCCESS)
            {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_IpPimSmDrPriFail);
                cliSyntaxBottom(ewsContext);
            }
        }
    }
    else
    {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_IpPimSmDrPriOutOfRange);
        cliSyntaxBottom(ewsContext);
    }
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the PIM-SM RP Candidate
*
* @param      EwsContext     ewsContext
* @param      L7_uint32      argc
* @param      const L7_char8 **argv
* @param      L7_uint32      index
* @returntype const          L7_char8
* @returns    cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm rp-candidate interface
*                    <slot/port> <group-address> <group-mask>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsmRpCandidate(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 groupAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 groupMaskStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 groupAddr;
  L7_uint32 groupMask;
  L7_uint32 rc;
  L7_uint32 unit,slot,port,intIface;
  L7_uint32 numArgs;
  L7_inet_addr_t inetGrpAddr, inetGrpMask;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();
  inetAddressZeroSet(L7_AF_INET, &inetGrpAddr);
  inetAddressZeroSet(L7_AF_INET, &inetGrpMask);

  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 3 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmRpCandidate);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 3 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmRpCandidateNo);
  }

  if (strlen(argv[index+2]) >= sizeof(groupAddrStr))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpIp);
  }

  if (strlen(argv[index+3]) >= sizeof(groupMaskStr))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpMask);
  }

  memset(groupAddrStr, L7_NULL, sizeof(groupAddrStr));
  memset(groupMaskStr, L7_NULL, sizeof(groupMaskStr));
  strcpy(groupAddrStr,argv[index + 2]);
  strcpy(groupMaskStr,argv[index + 3]);

  if ( usmDbInetAton(groupAddrStr, &groupAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpIp);
  }

  if ( usmDbInetAton(groupMaskStr, &groupMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmRpCandidateGrpMask);
  }

  if (usmDbNetmaskIsContiguous(groupMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_NonContiguousMask);
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index+argSlotPort],
                                        &unit, &slot, &port) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  {
    if (cliSlotPortToInterface(argv[index + argSlotPort],
                                 &unit, &slot, &port, &intIface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }

  if (cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }
  inetAddressSet(L7_AF_INET, &groupAddr, &inetGrpAddr);
  inetAddressSet(L7_AF_INET, &groupMask, &inetGrpMask);

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NO_CMD)
    {
      rc = usmDbPimsmCandRPSet(L7_AF_INET, intIface,
                                    &inetGrpAddr, &inetGrpMask, L7_DISABLE);
    }
    else
    {
      rc = usmDbPimsmCandRPSet(L7_AF_INET, intIface,
                                    &inetGrpAddr, &inetGrpMask, L7_ENABLE);
    }
    if (rc != L7_SUCCESS)
    {
      if (ewsContext->commType == CLI_NO_CMD)
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_PimSmRpCandidateDeletion);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_PimSmRpCandidateCreation);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
* @purpose    Configures the PIM-SM hash-mask length and priority.
*
* @param      EwsContext     ewsContext
* @param      L7_uint32      argc
* @param      const L7_char8 **argv
* @param      L7_uint32      index
* @returntype const          L7_char8
* @returns    cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip pimsm bsr-candidate interface <slot/port>
*                      [hash-mask-length] [priority]
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimsmBsrCandidate(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8  hashMaskLen[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  strPriority[L7_CLI_MAX_STRING_LENGTH];
  L7_int32  maskLength = FD_PIMSM_CBSR_HASH_MASK_LENGTH;
  L7_int32  priority = FD_PIMSM_CBSR_PRIORITY;
  L7_uint32 argSlotPort = 1;
  L7_uint32 unit, slot, port, intIface, bsrAddr, mode = L7_DISABLE;
  L7_uint32 numArgs, ipMask, scope = L7_NULL;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs != 1) &&
      (numArgs != 2) && (numArgs != 3))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmBsrCandidate);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && (numArgs != 1) &&
           (numArgs != 2 ) && (numArgs != 3))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmBsrCandidateNo);
  }

  if ((numArgs == 2) ||(numArgs == 3))
  {
    if (strlen(argv[index+2]) >= sizeof(hashMaskLen))
    {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmBsrCandidateMaskLen);
    }
  }
  if (numArgs == 3)
  {
    if (strlen(argv[index+3]) >= sizeof(strPriority))
    {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_PimSmBsrCandidatePri);
    }
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index+argSlotPort],
                                        &unit, &slot, &port) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  {
    if (cliSlotPortToInterface(argv[index + argSlotPort],
                                 &unit, &slot, &port, &intIface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }

  if (cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
  }
  if (usmDbIpRtrIntfIpAddressGet(unit, intIface,
                                    &bsrAddr, &ipMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_common_Err);
  }

  if (numArgs == 1)
  {
      if (ewsContext->commType != CLI_NO_CMD)
    {
      mode = L7_ENABLE;
    }
    else
    {
      mode = L7_DISABLE;
    }
    maskLength = FD_PIMSM_CBSR_HASH_MASK_LENGTH;
    priority = FD_PIMSM_CBSR_PRIORITY;
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        if (usmDbPimsmCandBSRSet(unit, L7_AF_INET, intIface,
                         priority, mode, maskLength, scope) != L7_SUCCESS)
        {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_SetBsrCandidate);
      }
        }
  }
  if (numArgs == 2)
  {
      if (ewsContext->commType != CLI_NO_CMD)
      {
      osapiStrncpySafe(hashMaskLen, argv[index + 2], sizeof(hashMaskLen));
      if (cliCheckIfInteger(hashMaskLen) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmBsrCandidate);
          }
          else
          {
              cliConvertTo32BitSignedInteger(hashMaskLen, &maskLength);
          }
          priority = FD_PIMSM_CBSR_PRIORITY;
          mode = L7_ENABLE;
      }
      else
      {
          maskLength = FD_PIMSM_CBSR_HASH_MASK_LENGTH;
          priority = FD_PIMSM_CBSR_PRIORITY;
          mode = L7_DISABLE;
      }

      if ((maskLength >= L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN) &&
          (maskLength <= L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX))
      {
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
            if (usmDbPimsmCandBSRSet(unit, L7_AF_INET, intIface,
                priority, mode, maskLength, scope) != L7_SUCCESS)
            {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_SetBsrCandidate);
            }
      }
    }
      else
      {
      ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Range);
          cliSyntaxBottom(ewsContext);
      }
  }
  if (numArgs == 3)
  {
      if (ewsContext->commType != CLI_NO_CMD)
      {
      osapiStrncpySafe(hashMaskLen, argv[index + 2], sizeof(hashMaskLen));

      osapiStrncpySafe(strPriority, argv[index + 3], sizeof(strPriority));

      if (cliCheckIfInteger(hashMaskLen) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmBsrCandidate);
          }
          else
          {
              cliConvertTo32BitSignedInteger(hashMaskLen, &maskLength);
          }

          if (cliCheckIfInteger(strPriority) != L7_SUCCESS)
          {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpPimSmBsrCandidate);
          }
          else
          {
              cliConvertTo32BitSignedInteger(strPriority, &priority);
          }
          mode = L7_ENABLE;
      }
      else
      {
          maskLength = FD_PIMSM_CBSR_HASH_MASK_LENGTH;
          priority = FD_PIMSM_CBSR_PRIORITY;
          mode = L7_DISABLE;
      }
      if ((maskLength >= L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN) &&
          (maskLength <= L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX) &&
          (priority >= L7_PIMSM_INTERFACE_CBSR_PREFERENCE_MIN) &&
          (priority <= L7_PIMSM_INTERFACE_CBSR_PREFERENCE_MAX))
      {
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
            if (usmDbPimsmCandBSRSet(unit, L7_AF_INET, intIface,
                priority, mode, maskLength, scope) != L7_SUCCESS)
            {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_SetBsrCandidate);
            }
      }
    }
      else
      {
      ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Range);
          cliSyntaxBottom(ewsContext);
      }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
