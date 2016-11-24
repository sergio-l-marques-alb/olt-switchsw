/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_dvlan.c
 *
 * @purpose show running config commands for the dvlan tagging
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/05/2007
 *
 * @author  Amit Kulkarni
 *
 * @end
 *
 **********************************************************************/

#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_switching_cli.h"
#include "strlib_base_cli.h"

#include "usmdb_counters_api.h"
#include "cli_web_exports.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dvlantag_api.h"
#include "defaultconfig.h"

#include "cliapi.h"
#include "ews.h"
#include "ews_cli.h"

#include "cliutil.h"
#include "cli_show_running_config.h"
#include "clicommands_card.h"


/*********************************************************************
* @purpose  To print the running configuration of DVLANs
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
L7_RC_t cliRunningConfigDVlanInfo(EwsContext ewsContext, L7_uint32 unit,L7_uint32 interface)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 rc,  rc1;
  L7_uint32 slot, port;
  L7_uint32 sysIntfType;
  L7_uint32 val, tpIdx;

  rc = usmDbIntfTypeGet(interface, &sysIntfType);
  rc1 = usmDbUnitSlotPortGet(interface, &unit, &slot, &port);
  if ((rc == L7_SUCCESS) &&
      (rc1 == L7_SUCCESS) &&
      (usmDbDvlantagIntfValidCheck(interface) == L7_SUCCESS))
  {
    if (sysIntfType == L7_LAG_INTF  &&  usmDbDot3adIsConfigured(unit,interface) == L7_FALSE)
    {
      /* Do Nothing */
    }
    else
    {
      if(usmDbDvlantagIntfModeGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext,val,FD_DVLANTAG_DEFAULT_PORT_MODE,pStrInfo_switching_ModeDvlanTunnel);
      }
      if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_CUSTID_FEATURE_ID) == L7_TRUE)
      {
        if(usmDbDvlantagIntfCustIdGet(unit, interface, &val) == L7_SUCCESS)
        {
          cliShowCmdInt(ewsContext, val, FD_DVLANTAG_DEFAULT_CUSTID, pStrInfo_switching_DvlanTunnelCustomerId);
        }
      }

      if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE &&
          usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_TRUE &&
          usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
      {
        for(tpIdx = 0; tpIdx<L7_DVLANTAG_MAX_TPIDS; tpIdx++)
        {
          if(usmDbDvlantagIntfEthertypeGet(unit, interface, &val, tpIdx) == L7_SUCCESS)
          {
            if (val != 0x00 && !(val == L7_DVLANTAG_DEFAULT_ETHERTYPE && tpIdx == 0))
            {
              osapiSnprintf (stat, sizeof(stat), "%s %s ", pStrInfo_base_DvlanTunnel, pStrInfo_common_Ethertype);
              // PTin added
              if (val==L7_DVLANTAG_802_1Q_ETHERTYPE)
              {
                osapiSnprintf(buf, sizeof(buf), "%s ", pStrInfo_common_DvlanEtherType802);
              }
              else if (val==L7_DVLANTAG_VMAN_ETHERTYPE)
              {
                osapiSnprintf(buf, sizeof(buf), "%s ", pStrInfo_switching_DvlanEtherTypeVman);
              }
              else
              {
                osapiSnprintf(buf, sizeof(buf), "%s %d ", pStrInfo_switching_DvlanEtherTypeCustom, val);
              }
//            switch (val)
//            {
//              case L7_DVLANTAG_802_1Q_ETHERTYPE:
//                osapiSnprintf(buf, sizeof(buf), "%s ", pStrInfo_common_DvlanEtherType802);
//                break;
//              case L7_DVLANTAG_VMAN_ETHERTYPE:
//                osapiSnprintf(buf, sizeof(buf), "%s ", pStrInfo_switching_DvlanEtherTypeVman);
//                break;
//              default:
//                osapiSnprintf(buf, sizeof(buf), "%s %d ", pStrInfo_switching_DvlanEtherTypeCustom, val);
//                break;
//            }
              // PTin end
              osapiStrncat(stat, buf, strlen(stat));
              EWSWRITEBUFFER(ewsContext, stat);
            }
          }
        }
      }
    }

  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  To print the global running configuration of DVLANs
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

L7_RC_t cliRunningConfigGlobalDVlanInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val, tpIdx;

  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE &&
    usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_TRUE)
  {
    for(tpIdx = 0; tpIdx<L7_DVLANTAG_MAX_TPIDS; tpIdx++)
    {
      if(usmDbDvlantagEthertypeGet(unit, &val, tpIdx) == L7_SUCCESS)
      {
        if (val != 0x00 && !(val == L7_DVLANTAG_DEFAULT_ETHERTYPE && tpIdx == 0))
        {
          osapiSnprintf (stat, sizeof(stat), "%s %s ", pStrInfo_base_DvlanTunnel, pStrInfo_common_Ethertype);
          // PTin added
          if (val==L7_DVLANTAG_802_1Q_ETHERTYPE)
          {
            osapiSnprintf(buf, sizeof(buf), "%s ", pStrInfo_common_DvlanEtherType802);
          }
          else if (val==L7_DVLANTAG_VMAN_ETHERTYPE)
          {
            osapiSnprintf(buf, sizeof(buf), "%s ", pStrInfo_switching_DvlanEtherTypeVman);
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf), "%s %d ", pStrInfo_switching_DvlanEtherTypeCustom, val);
          }
//        switch (val)
//        {
//          case L7_DVLANTAG_802_1Q_ETHERTYPE:
//            osapiSnprintf(buf, sizeof(buf), "%s ", pStrInfo_common_DvlanEtherType802);
//            break;
//          case L7_DVLANTAG_VMAN_ETHERTYPE:
//            osapiSnprintf(buf, sizeof(buf), "%s ", pStrInfo_switching_DvlanEtherTypeVman);
//            break;
//          default:
//            osapiSnprintf(buf, sizeof(buf), "%s %d ", pStrInfo_switching_DvlanEtherTypeCustom, val);
//            break;
//        }
          // PTin end
          osapiStrncat(stat, buf, strlen(stat));

          if(usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID) == L7_TRUE)
          {
            if(tpIdx == 0)
            {
              osapiStrncat(stat, pStrInfo_common_DefTpid, strlen(pStrInfo_common_DefTpid));
            }
          }
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Display dvlan information for configured interfaces
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking, replace help string
*
* @cmdsyntax  show dvlan-tunnel
*
* @cmdhelp Display double vlan tunnel information about configured interfaces
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDVlanTunnel(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc, rc1;
  L7_uint32 val, unitNum;
  L7_uint32 unit, slot, port;
  L7_uint32 i = 0;
  L7_uint32 numArg;
  L7_uint32 nextInterface;
  L7_BOOL hasInterfaces = L7_FALSE;
  L7_uint32 tpidValue, defaultTpid=L7_NULL;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowDvlanTunnel);
  }

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, "TPIDs Configured");
  for(i=0; i<L7_DVLANTAG_MAX_TPIDS; i++)
  {
    if(usmDbDvlantagEthertypeGet(unit, &tpidValue, i) == L7_SUCCESS)
    {
      if(i==0)
      {
        defaultTpid = tpidValue;
      }

      if(tpidValue == 0)
        continue;

      ewsTelnetPrintf(ewsContext, "0x%x ", tpidValue);
    }
  }

  if(usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID) == L7_TRUE)
  {
    cliFormat(ewsContext,"Default TPID");
    ewsTelnetPrintf(ewsContext, "0x%x", defaultTpid);
  }
  
  cliFormat(ewsContext, pStrInfo_switching_IntfsEnbldForDvlanTunneling);

  rc = usmDbValidIntIfNumFirstGet(&nextInterface);

  do
  {
    i=nextInterface;

    rc = usmDbDvlantagIntfValidCheck(i);
    rc1 = usmDbUnitSlotPortGet(i, &unitNum, &slot, &port);
    if ((rc == L7_SUCCESS) && (rc1 == L7_SUCCESS))
    {
        rc = usmDbDvlantagIntfModeGet(unit, i, &val);
        if (val != L7_ENABLE)
      {
          continue;
      }

      memset (stat, 0, sizeof(stat));
      if (hasInterfaces == L7_FALSE)
      {
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(unitNum, slot, port));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "\r\n                                                %s",
                  cliDisplayInterfaceHelp(unitNum, slot, port));
      }
        ewsTelnetWrite(ewsContext, stat);
        hasInterfaces = L7_TRUE;
    }
  } while (usmDbValidIntIfNumNext(i, &nextInterface) == L7_SUCCESS);

  if (hasInterfaces == L7_FALSE)
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_None_1);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Display dvlan information based on the slot/port
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking, replace help string
*
* @cmdsyntax  show dvlan {<slot/port> | all}
*
* @cmdhelp Display double vlan tagging information about a specific interface
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDVlanTunnelInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uchar8 stat[81];
  L7_uint32 argSlotPort=1;
  L7_RC_t rc, rc1;
  L7_uint32 val, unitNum;
  L7_uint32 unit, slot, port;
  L7_uint32 i = 0, j=0;
  L7_uint32 numArg;
  L7_uint32 interface, nextInterface;
  L7_BOOL all = L7_FALSE;
  L7_BOOL CustomerIdPresent = L7_FALSE;
  L7_int32  retVal;
  L7_uint32 tpidValue;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowDvlanIntf, cliSyntaxInterfaceHelp());
  }

  if ( strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0 )
  {
    rc = usmDbValidIntIfNumFirstGet(&nextInterface);
    all = L7_TRUE;
    unit = cliGetUnitId();
  }
  else
  {
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

    if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) != L7_SUCCESS)
    {
      if (retVal == -1)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }

    nextInterface = interface;
  }

  if(usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_CUSTID_FEATURE_ID) == L7_TRUE)
  {
    CustomerIdPresent = L7_TRUE ;
  }

  if(CustomerIdPresent == L7_TRUE)
  {
    ewsTelnetWrite(ewsContext,"\r\n                                            ");
    ewsTelnetWriteAddBlanks (1, 0, 0, 5, L7_NULLPTR, ewsContext,pStrInfo_switching_IntfModeCustomerIdEthertype);
    ewsTelnetWrite(ewsContext,"\r\n--------- ------- ----------- --------------");
  }
  else
  {
    ewsTelnetWrite(ewsContext,"\r\n                                ");
    ewsTelnetWriteAddBlanks (1, 0, 0, 5, L7_NULLPTR, ewsContext,pStrInfo_switching_IntfModeEthertype);
    ewsTelnetWrite(ewsContext,"\r\n--------- ------- --------------");
  }

  do
  {
    i=nextInterface;

    rc = usmDbDvlantagIntfValidCheck(i);
    rc1 = usmDbUnitSlotPortGet(i, &unitNum, &slot, &port);
    if ((rc == L7_SUCCESS) && (rc1 == L7_SUCCESS))
    {
      ewsTelnetPrintf (ewsContext, "\r\n%-10s", cliDisplayInterfaceHelp(unitNum,slot,port));

      memset (stat, 0, sizeof(stat));
      rc = usmDbDvlantagIntfModeGet(unit, i, &val);
      ewsTelnetPrintf (ewsContext, "%-7s ",strUtilEnableDisableGet(val, pStrInfo_common_Blank));

        if (CustomerIdPresent == L7_TRUE)
        {
        memset (stat, 0, sizeof(stat));
         rc = usmDbDvlantagIntfCustIdGet(unit, i, &val);
        ewsTelnetPrintf (ewsContext, "%-11d ", val);
      }

      memset (stat, 0, sizeof(stat));

      for(j=0; j<L7_DVLANTAG_MAX_TPIDS; j++)
      {
        if (usmDbDvlantagIntfEthertypeGet(unit, i, &tpidValue, j) == L7_SUCCESS)
        {
          if(tpidValue == 0)
            continue;
          ewsTelnetPrintf(ewsContext, "0x%x ", tpidValue);
        }
      }
#if 0      
      rc = usmDbDvlantagIntfEthertypeGet(unit, i, &val);
      if (rc != L7_SUCCESS)
      {
        val = L7_DVLANTAG_VMAN_ETHERTYPE;
      }
        switch (val)
        {
        case L7_DVLANTAG_802_1Q_ETHERTYPE:
        osapiSnprintf(stat, sizeof(stat), "%-14s ", pStrInfo_common_DvlanEtherType802);
            break;
        case L7_DVLANTAG_VMAN_ETHERTYPE:
        osapiSnprintf(stat, sizeof(stat), "%-14s ", pStrInfo_switching_DvlanEtherTypeVman);
            break;
        default:
        osapiSnprintf(buf, sizeof(buf), "%s (%d) ", pStrInfo_switching_DvlanEtherTypeCustom, val);
        osapiSnprintf(stat, sizeof(stat), "%-14s", buf);
            break;
        }
        ewsTelnetWrite(ewsContext,stat);
#endif
    }
    if (all != L7_TRUE)
    {
        break;
    }
  } while (usmDbValidIntIfNumNext(i, &nextInterface) == L7_SUCCESS);

  return cliSyntaxReturnPrompt (ewsContext, "");

}

