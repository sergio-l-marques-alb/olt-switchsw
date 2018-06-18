/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/cli_show_pimdm.c
 *
 * @purpose show commands for the pimdm cli functionality
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
 * @author   srikrishnas
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
#include <datatypes.h>
#include <usmdb_ip_api.h>
#include <usmdb_util_api.h>
#include <clicommands_l3.h>
#include <clicommands_mcast.h>
#include <l3_commdefs.h>
#include <usmdb_mib_pim_api.h>
#include "usmdb_l3.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_api.h"
#endif

#include "usmdb_mib_pim_rfc5060_api.h"
#include "clicommands_card.h"

/*********************************************************************
*
* @purpose Displays PIM-DM router info
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
* @cmdsyntax    show ip pimdm
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpPimdm(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 val = L7_DISABLE;
  L7_uint32 intSlot, intPort;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 nextInterface;
  L7_uint32 interface;
  L7_uint32 unit, numArgs;

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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpPimDm_2);
  }

  memset(stat, L7_NULL, sizeof(stat));
  /* display prompt....*/
  cliFormat(ewsContext, pStrInfo_ipmcast_AdminMode);
  if (usmDbPimRouterAdminModeGet(unit, L7_AF_INET, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val,pStrInfo_common_Dsbl_1));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Heading for the router PIM-DM interface summary */
  if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_McastNoPortsAvailable);
  }

  /* Display the PIM-DM interface info */
  ewsTelnetWriteAddBlanks (2, 0, 4, 10, L7_NULLPTR, ewsContext,pStrInfo_common_PimDmIntfStatus);
  ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_IntfIntfModeProtoState);
  ewsTelnetWrite(ewsContext,"\r\n---------  --------------  ----------------\r");

  /* start for loop for all the interfaces */
  while (interface)
  {
    /* display only the visible interfaces - and non loop/tnnl for now */
    if ((usmDbPimdmIsValidIntf(unit, interface) == L7_TRUE) &&
        (usmDbIpIntfExists(unit, interface) == L7_TRUE))
    {
      memset(buf, L7_NULL, sizeof(buf));
      memset(strSlotPort, L7_NULL, sizeof(strSlotPort));
      if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
      }
      ewsTelnetPrintf (ewsContext, "\r\n%-11.9s",buf);

      memset(stat, L7_NULL, sizeof(stat));
      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbPimInterfaceModeGet(unit,
                               L7_AF_INET, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),"%-16s",strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-16s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      memset(buf, L7_NULL, sizeof(buf));
      if(usmDbPimdmInterfaceOperationalStateGet (unit, L7_AF_INET, interface) == L7_TRUE)
      {
        osapiSnprintf(buf, sizeof(buf),  "%-16s", pStrInfo_common_Operational);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  "%-16s", pStrInfo_common_NonOperational);
      }
      ewsTelnetWrite(ewsContext, buf);

      /*end if interface exists*/
    } /* end if visible interface */

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

  return cliSyntaxReturnPrompt (ewsContext, "");
}
/*********************************************************************
*
* @purpose Displays PIM-DM interface info.
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
* @cmdsyntax    show ip pimdm interface <slot/port>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpPimdmInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 val = L7_DISABLE,helloInt;
  L7_uint32 intSlot, intPort, interface;
  L7_uint32 unit, numArgs;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 nbrCount =0,drIpAddress;
  L7_inet_addr_t inetDrIpAddress;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpPimDmIntf_1, cliSyntaxInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  {
    if (cliSlotPortToInterface(argv[index + argSlotPort], &unit, &intSlot, &intPort, &interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }

  if ( cliValidateRtrIntf(ewsContext, interface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_IntfMode);
  if (usmDbPimInterfaceModeGet(unit, L7_AF_INET, interface, &val) == L7_SUCCESS)
  {
    ewsTelnetPrintf (ewsContext, strUtilEnabledDisabledGet(val,pStrInfo_common_Dsbl_1));
  }

  memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_HelloIntvlSecs);
  if (usmDbPimIntfHelloIntervalGet(unit, L7_AF_INET,
                                        interface, &helloInt) == L7_SUCCESS)
  {
    ewsTelnetPrintf (ewsContext, "%d", helloInt);
  }

  memset(stat, L7_NULL, sizeof(stat));
  memset(buf, L7_NULL, sizeof(buf));
  cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_NeighborCount);

  if ((usmDbPimInterfaceNeighborCountGet(unit, L7_AF_INET,
                                         interface, &nbrCount) == L7_SUCCESS))
  {
    osapiSnprintf(buf, sizeof(buf), "%d", nbrCount);
    osapiSnprintf(stat, sizeof(stat), buf);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%d", nbrCount);
    osapiSnprintf(stat, sizeof(stat), buf);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset(stat, L7_NULL, sizeof(stat));
  memset(buf, L7_NULL, sizeof(buf));
  cliFormat(ewsContext, pStrInfo_common_DesignatedRtr);

  if ((usmDbPimInterfaceDRGet(unit, L7_AF_INET, interface,
                              &inetDrIpAddress) == L7_SUCCESS))
  {
    inetAddressGet(L7_AF_INET, &inetDrIpAddress, &drIpAddress);
    if (usmDbInetNtoa(drIpAddress, buf) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);

    }
  }
  else
  {
    /*osapiSnprintf(stat, sizeof(stat), "%-15s", usmWebFindNLSToken("1312"));*/
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_NotSupported);
  }
  return cliSyntaxReturnPrompt (ewsContext, stat);
}

/*********************************************************************
*
* @purpose Displays PIM-DM interface stats table.
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
* @cmdsyntax    show ip pimdm interface stats {<slot/port>|all}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpPimdmInterfaceStats(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 intU_IDX;
  L7_uint32 all=L7_FALSE;
  L7_uint32 intSlot, intPort, interface = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mode, drIpAddress;
  L7_uint32 intHelloInterval;
  L7_uint32 nbrCount, nbrIpAddr,tmpIntfNo;
  L7_RC_t rc;
  L7_uint32 unit, numArgs, ipAddr;
  L7_inet_addr_t inetDrIpAddress, inetIpAddr;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpPimDmIntfStats_1, cliSyntaxInterfaceHelp());
  }

  if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
  {
    all = L7_TRUE;

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      if ( cliSlotPortToInterface(argv[index + argSlotPort], &unit, &intSlot, &intPort, &interface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }

    if ( cliValidateRtrIntf(ewsContext, interface) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

  }

  if(usmDbPimRouterAdminModeGet(unit, L7_AF_INET, &mode) == L7_SUCCESS)
  {
    if(mode != L7_ENABLE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_MustInitPim);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 37, 5, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_HelloDesignated);
  ewsTelnetWriteAddBlanks (1, 0, 0, 9, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IntfIpAddrNbrCountIntvlRtr);
  ewsTelnetWrite(ewsContext,"\r\n---------- --------------- --------- ---------- ---------------");

  nbrIpAddr = 0;
  tmpIntfNo = 0;
  if (all == L7_FALSE)
  {
    rc = usmDbPimdmInterfaceEntryGet(unit, L7_AF_INET,interface );
  }
  else
  {
    /* Get the first one*/
    interface = 0;
    rc = usmDbPimdmInterfaceEntryNextGet(unit, L7_AF_INET, &interface);
  }
  while (rc == L7_SUCCESS)
  {
    /* write port/slot*/
    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbUnitSlotPortGet(interface, &intU_IDX, &intSlot, &intPort) == L7_SUCCESS))
    {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(intU_IDX, intSlot, intPort));
      osapiSnprintf(stat, sizeof(stat), "\r\n%-11s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "\r\n%-11s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if (usmDbPimInterfaceIPAddressGet(unit, L7_AF_INET,
                                    interface, &inetIpAddr) == L7_SUCCESS)
    {
      inetAddressGet(L7_AF_INET, &inetIpAddr,  &ipAddr);
      if (usmDbInetNtoa(ipAddr, buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbPimInterfaceNeighborCountGet(unit, L7_AF_INET,
                                           interface, &nbrCount) == L7_SUCCESS))
    {
      osapiSnprintf(buf, sizeof(buf), "%d", nbrCount);
      osapiSnprintf(stat, sizeof(stat), "%-10s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-10s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbPimIntfHelloIntervalGet (unit, L7_AF_INET,
                                       interface, &intHelloInterval) == L7_SUCCESS))
    {
      osapiSnprintf(buf, sizeof(buf), "%d", intHelloInterval);
      osapiSnprintf(stat, sizeof(stat), "%-11s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-11s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbPimInterfaceDRGet(unit, L7_AF_INET, interface,
                                &inetDrIpAddress) == L7_SUCCESS))
    {
      inetAddressGet(L7_AF_INET, &inetDrIpAddress, &drIpAddress);
      if (usmDbInetNtoa(drIpAddress, buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-15s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-15s", pStrErr_common_Err);
      }
    }
    else
    {
      /*osapiSnprintf(stat, sizeof(stat), "%-15s", usmWebFindNLSToken("1312"));*/
      osapiSnprintf(stat, sizeof(stat), "%-15s", pStrErr_common_NotSupported);

    }

    ewsTelnetWrite(ewsContext, stat);
    if (!all)
    {
      /* break if diplay is for the specific entry*/
      break;
    }
    rc = usmDbPimInterfaceEntryNextGet(unit, L7_AF_INET, &interface);
  }   /* while*/

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose Displays PIM-DM neighbor table
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
* @cmdsyntax    show ip pimdm neighbor [{<slot/port>|all}]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpPimdmNeighbor(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 val, all=L7_FALSE;
  L7_uint32 intSlot, intPort, interface=0,intU_IDX=0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mode, nbrIpAddr, nbrIntIfNum ;
  L7_uint32 unit, numArgs;
  L7_RC_t rc;
  usmDbTimeSpec_t timeSpec;
  L7_inet_addr_t inetNbrIpAddr;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ((numArgs != 0)&&(numArgs != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpPimDmNeighbor_1, cliSyntaxInterfaceHelp());
  }

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
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      if ( cliSlotPortToInterface(argv[index + argSlotPort], &unit, &intSlot, &intPort, &interface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }

    if ( cliValidateRtrIntf(ewsContext, interface) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }

  if(usmDbPimRouterAdminModeGet(unit, L7_AF_INET, &mode) == L7_SUCCESS)
  {
    if(mode != L7_ENABLE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_MustInitPim);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 27, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_UpTimeExpiryTime);
  ewsTelnetWriteAddBlanks (1, 0, 0, 3, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_NeighborAddrIntfHhMmSsHhMmSs);
  ewsTelnetWrite(ewsContext,"\r\n--------------- ---------- --------- -----------");

  nbrIpAddr = 0;
  inetAddressSet(L7_AF_INET, &nbrIpAddr, &inetNbrIpAddr);

  nbrIntIfNum = 0;
  /* Get the first one*/
  rc = usmDbPimdmNeighborEntryNextGet(unit, L7_AF_INET, &nbrIntIfNum, &inetNbrIpAddr);
  while (rc == L7_SUCCESS)
  {
    val = nbrIntIfNum;
    /* Check if the neighbor belongs to this interface*/
    if(usmDbPimNeighborIfIndexGet(unit, L7_AF_INET,
                                  &inetNbrIpAddr, &nbrIntIfNum) != L7_SUCCESS )
    {
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_PimDmIntfNotFound);
    }
    if( !all )
    {
      if (interface != val)
      {
        /*Get the next neighbor*/
        rc = usmDbPimdmNeighborEntryNextGet(unit, L7_AF_INET,
                                          &nbrIntIfNum, &inetNbrIpAddr);
        continue;
      }
    }

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    inetAddressGet(L7_AF_INET, &inetNbrIpAddr, &nbrIpAddr);
    if (usmDbInetNtoa(nbrIpAddr, buf) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbUnitSlotPortGet(val, &intU_IDX, &intSlot, &intPort) == L7_SUCCESS))
    {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(intU_IDX, intSlot, intPort));
      osapiSnprintf(stat, sizeof(stat), "%-11s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-11s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbPimNeighborUpTimeSpecGet(unit, L7_AF_INET,
                                       &inetNbrIpAddr, &timeSpec) == L7_SUCCESS))
    {
      osapiSnprintf(buf,sizeof(buf),"%2.2d:%2.2d:%2.2d", timeSpec.hours,
                    timeSpec.minutes,timeSpec.seconds); /* todo: hh:mm:ss*/
      osapiSnprintf(stat, sizeof(stat), "%-10s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-10s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbPimNeighborExpiryTimeSpecGet(unit, L7_AF_INET,
                                           &inetNbrIpAddr, &timeSpec) == L7_SUCCESS))
    {
      osapiSnprintf(buf,sizeof(buf),"%2.2d:%2.2d:%2.2d", timeSpec.hours,
                    timeSpec.minutes,timeSpec.seconds); /* todo: hh:mm:ss*/
      osapiSnprintf(stat, sizeof(stat), "%-11s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-12s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
    rc = usmDbPimdmNeighborEntryNextGet(unit, L7_AF_INET, &nbrIntIfNum, &inetNbrIpAddr);
  }   /* while*/

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays trap flag info for PIM
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
* @cmdsyntax     show trapflags
*
* @cmdhelp
*
* @cmddescript This commands takes no options
*
* @end
*
*********************************************************************/
void commandShowPimTrapflags(EwsContext ewsContext)
{
  L7_uint32 val;

  if (usmDbPimTrapModeGet(L7_PIM_TRAP_ALL, &val) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_PimTraps);
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
  }
  return;
}
