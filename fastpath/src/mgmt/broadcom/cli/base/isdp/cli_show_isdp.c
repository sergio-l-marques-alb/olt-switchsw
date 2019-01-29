/******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 ******************************************************************************
 *
 * @filename cli_show_isdp.c
 *
 * @purpose show for the isdp
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  21/11/2007
 *
 * @author  Rostyslav Ivasiv
 *
 * @end
 *
 *****************************************************************************/
#include "cliapi.h"
#include "strlib_base_cli.h"
#include "strlib_common_web.h"
#include "ews.h"
#include "usmdb_isdp_api.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_2233_stats_api.h"
#include "isdp_api.h"
#include "usmdb_util_api.h"
#include "clicommands_card.h"
#include "compdefs.h"
#include "local.h"
#include "l3_addrdefs.h"
#include "osapi_support.h"
#include <string.h>

/*********************************************************************
*
* @purpose  display one output line for show isdp neighbors
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_char8*  deviceId      @b((input))  mac address
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
void cliIsdpShowNeighborWrite(EwsContext ewsContext, L7_uint32 intIfNum, L7_char8* deviceId)
{
  L7_uint32 cap = 0;
  L7_uchar8 holdTime = 0;
  L7_char8 platform[L7_ISDP_PLATFORM_LEN];
  L7_char8 portId[L7_ISDP_PORT_ID_LEN];
  L7_char8 capCh[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 holdTimePrn[16];
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];

  memset(platform, 0x00, sizeof(platform));
  memset(portId, 0x00, sizeof(portId));
  memset(holdTimePrn, 0x00, sizeof(holdTimePrn));
  memset(stat, 0x00, sizeof(stat));

  usmdbIsdpIntfNeighborPlatformGet(intIfNum, deviceId, platform);
  usmdbIsdpIntfNeighborPortIdGet(intIfNum, deviceId, portId);
  usmdbIsdpIntfNeighborCapabilitiesGet(intIfNum, deviceId, &cap);
  usmdbIsdpIntfNeighborHoldTimeGet(intIfNum, deviceId, &holdTime);
  usmDbIfNameGet(0, intIfNum, stat);

  usmdbIsdpCapabilityCharGet(cap, capCh, L7_CLI_MAX_STRING_LENGTH);
  osapiSnprintf(&holdTimePrn[0], sizeof(holdTimePrn), "%u", holdTime);

  ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnNeighborTable, deviceId,
      stat, holdTimePrn, capCh, platform, portId);
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
}

/*********************************************************************
*
* @purpose  display one device information for show isdp neighbors detail
* and show isdp entry
*
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_char8*  deviceId      @b((input))  mac address
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
static void cliIsdpShowNeighborDetailWrite(EwsContext ewsContext, L7_uint32 intIfNum,
    L7_char8* deviceId)
{
  L7_RC_t rcFlag = L7_SUCCESS;
  L7_uint32 ipAddress = 0;
  L7_uint32 ipAddressIndex = 0;
  L7_uchar8 holdTime = 0;
  L7_char8 protoVersion = 0;
  L7_uint32 lastChangeTime = 0;
  L7_uint32 cap = 0;
  L7_char8 capCh[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 version[L7_ISDP_VERSION_LEN];
  L7_char8 platform[L7_ISDP_PLATFORM_LEN];
  L7_char8 portId[L7_ISDP_PORT_ID_LEN];
  L7_char8 address[IPV6_DISP_ADDR_LEN];
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_timespec timespec;

  memset(buf, 0x00, sizeof(buf));
  memset(capCh, 0x00, sizeof(capCh));
  memset(platform, 0x00, sizeof(platform));
  memset(version, 0x00, sizeof(version));
  memset(portId, 0x00, sizeof(portId));
  memset(address, 0x00, IPV6_DISP_ADDR_LEN);
  memset(stat, 0x00, sizeof(stat));

  ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable,
        pStrInfo_base_PrnDeviceID, deviceId);
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);

  rcFlag = usmdbIsdpIntfNeighborAddressGetNext(intIfNum, deviceId, 0,
      &ipAddressIndex);

  ewsTelnetPrintf(ewsContext, "%s\r\n", pStrInfo_base_PrnEntryAddresses);

  while(rcFlag == L7_SUCCESS)
  {
    if(usmdbIsdpIntfNeighborIpAddressGet(intIfNum, deviceId, ipAddressIndex, &ipAddress)
      == L7_SUCCESS)
    {
      osapiInetNtop(L7_AF_INET, (L7_uchar8*)&ipAddress, address, IPV6_DISP_ADDR_LEN);
      ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable,
                      pStrInfo_base_PrnEntryIpAddresses, address);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    rcFlag = usmdbIsdpIntfNeighborAddressGetNext(intIfNum, deviceId, ipAddressIndex, &ipAddressIndex);
  }

  if(usmdbIsdpIntfNeighborCapabilitiesGet(intIfNum, deviceId, &cap)
      == L7_SUCCESS)
  {
    usmdbIsdpCapabilityStringGet(cap, capCh, L7_CLI_MAX_STRING_LENGTH);
    ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable,
        pStrInfo_base_PrnCapability, capCh);
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }

  if(usmdbIsdpIntfNeighborPlatformGet(intIfNum, deviceId, platform) == L7_SUCCESS)
  {
    ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable,
        pStrInfo_base_PrnPlatform, platform);
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }

  if (usmDbIfNameGet(0, intIfNum, stat) == L7_SUCCESS)
  {
    ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable,
                                    pStrInfo_common_Intf , stat);
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }

  if(usmdbIsdpIntfNeighborPortIdGet(intIfNum, deviceId, portId) == L7_SUCCESS)
  {
    ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable,
        pStrInfo_base_PrnPortId, portId);
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }

  if(usmdbIsdpIntfNeighborHoldTimeGet(intIfNum, deviceId, &holdTime) == L7_SUCCESS)
  {
    ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable_,
        pStrInfo_base_PrnHoldtime, holdTime);
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }

  if(usmdbIsdpIntfNeighborProtocolVersionGet(intIfNum, deviceId, &protoVersion)
                                                                 == L7_SUCCESS)
  {
    ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable_,
        pStrInfo_base_PrnAdvVer, protoVersion);
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }

  if(usmdbIsdpIntfNeighborLastChangeTimeGet(intIfNum, deviceId,
                                                &lastChangeTime) == L7_SUCCESS)
  {
    osapiConvertRawUpTime(lastChangeTime, &timespec);
    osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, pStrInfo_base_D, timespec.days,
                            timespec.hours, timespec.minutes, timespec.seconds);
    ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnEntryTable,
                                        pStrInfo_base_PrnLastTimeChanged, buf);
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }

  if(usmdbIsdpIntfNeighborVersionGet(intIfNum, deviceId, version) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrInfo_base_PrnVersion);
    ewsTelnetWrite(ewsContext, " :");
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite(ewsContext, version);
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  }
}

/******************************************************************************
*
* @purpose  Display ISDP neighbors table.
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
* @cmdsyntax  show process neighbors <cr>|detail|([unit/]slot/port <cr>|detail)
*
* @cmdhelp Display ISDP neighbors table.
*
* @cmddescript Display ISDP neighbors table.
*
* @end
*
******************************************************************************/
const L7_char8 * commandShowIsdpNeighbors(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 * * argv,
                                          L7_uint32 index)
{
  L7_uint32 unit, slot, port, mode, count, i;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 numArg = 0;
  static L7_char8 deviceId[L7_ISDP_DEVICE_ID_LEN];
  static L7_uint32 intIfNum = 0;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];

  memset(cmdBuf, 0x00, sizeof(cmdBuf));

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    return cliSyntaxReturnPrompt(ewsContext, pStrInfo_base_IsdpModePrn);
  }

  numArg = cliNumFunctionArgsGet();
  if(cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      intIfNum = 0;
      memset(deviceId, 0x00, sizeof(deviceId));
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if((((numArg == 1) || (argc == 5)) &&
       (cliValidSpecificUSPCheck(argv[index + numArg], &unit, &slot, &port)
			  == L7_SUCCESS)))
    {
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
      {
	memset(deviceId, 0x00, sizeof(deviceId));
	ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
	return cliPrompt(ewsContext);
      }
      rc = usmdbIsdpIntfNeighborGetNext(intIfNum, 0, deviceId);
    }
    else
    {
      rc = usmdbIsdpNeighborGetNext(intIfNum, deviceId, &intIfNum, deviceId);

    }
    if (rc != L7_SUCCESS)
    {
      intIfNum = 0;
      memset(deviceId, 0x00, sizeof(deviceId));
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
		  L7_NULLPTR, ewsContext, pStrErr_base_IsdpNeighborGetNext);
    }
  }

  if (numArg == 0)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
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

    count = 0;

    while((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
    {
      cliIsdpShowNeighborWrite(ewsContext, intIfNum, deviceId);
      rc = usmdbIsdpNeighborGetNext(intIfNum, deviceId, &intIfNum, deviceId);
      if (rc != L7_SUCCESS)
      {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      intIfNum = 0;
      memset(deviceId, 0x00, sizeof(deviceId));
      return cliPrompt(ewsContext);
      }
      count++;
    }
  }
  /*show isdp neighbor detail*/
  if ((numArg == 1) &&
    (strcmp(argv[index + numArg], pStrInfo_base_IsdpNeighborsDetail) == 0))
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
    count = 0;
    while((rc == L7_SUCCESS)  && (count < CLI_MAX_SCROLL_LINES-6))
    {
      cliIsdpShowNeighborDetailWrite(ewsContext, intIfNum, deviceId);
      rc = usmdbIsdpNeighborGetNext(intIfNum, deviceId, &intIfNum, deviceId);
      if (rc != L7_SUCCESS)
      {
	ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
	intIfNum = 0;
	memset(deviceId, 0x00, sizeof(deviceId));
	return cliPrompt(ewsContext);
      }
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
      count = count + 16;
    }
  }
  /* show isdp neighbor [unit/]slot/port */
  else
  {
    if((numArg == 1) &&
       (cliValidSpecificUSPCheck(argv[index + numArg], &unit, &slot, &port)
				 == L7_SUCCESS))
    {
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
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

      count = 0;
      while((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
      {
	cliIsdpShowNeighborWrite(ewsContext, intIfNum, deviceId);
	rc = usmdbIsdpIntfNeighborGetNext(intIfNum, deviceId, deviceId);
       if (rc != L7_SUCCESS)
       {
	 ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
	 intIfNum = 0;
	 memset(deviceId, 0x00, sizeof(deviceId));
	 return cliPrompt(ewsContext);
       }
       count++;
      }
    }
  }
  /*show isdp neighbor [unit/]slot/port detail*/
  if (argc == 5)
  {
    if(cliValidSpecificUSPCheck(argv[index + 1], &unit, &slot, &port) ==
				   L7_SUCCESS)
    {
      count = 0;
      while((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
      {
	ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
	cliIsdpShowNeighborDetailWrite(ewsContext, intIfNum, deviceId);
	rc = usmdbIsdpIntfNeighborGetNext(intIfNum, deviceId, deviceId);
	if (rc != L7_SUCCESS)
	{
	  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
	  intIfNum = 0;
	  memset(deviceId, 0x00, sizeof(deviceId));
	  return cliPrompt(ewsContext);
	}
	count = count + 16;
      }
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
  for (i=1; i<argc; i++)
  {
    OSAPI_STRNCAT(cmdBuf, " ");
    OSAPI_STRNCAT(cmdBuf, argv[i]);
  }
  cliAlternateCommandSet(cmdBuf);
  return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
}

/******************************************************************************
*
* @purpose  Displays global ISDP settings.
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
* @cmdsyntax  show isdp
*
* @cmdhelp Displays global ISDP settings.
*
* @cmddescript Displays global ISDP settings.
*
* @end
*
******************************************************************************/
const L7_char8 * commandShowIsdp(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 * * argv,
                                 L7_uint32 index)
{
  L7_uint32  timer, holdTime, mode;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 deviceIdFormatCpb;
  L7_uint32 deviceIdFormat;
  L7_char8 capCh[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 deviceId[L7_ISDP_DEVICE_ID_LEN];
  L7_timespec timespec;

  memset(buf, 0x00, sizeof(buf));
  memset(capCh, 0x00, sizeof(capCh));
  memset(deviceId, 0x00, sizeof(deviceId));

  cliSyntaxTop(ewsContext);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    return cliSyntaxReturnPrompt(ewsContext, pStrInfo_base_IsdpModePrn);
  }

  if(usmdbIsdpTimerGet(&timer) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0,
           pStrErr_base_FailedToGet, ewsContext, pStrInfo_base_IsdpTimer);
  }

  if(usmdbIsdpHoldTimeGet(&holdTime) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0,
        pStrErr_base_FailedToGet, ewsContext, pStrInfo_base_IsdpHoldtime);
  }

  if(usmdbIsdpV2ModeGet(&mode) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0,
                      pStrErr_base_FailedToGet, ewsContext,
                           pStrInfo_base_IsdpAdvertiseMode);
  }
  cliFormat(ewsContext, pStrInfo_base_TimerPrn);
  ewsTelnetPrintf(ewsContext, "%d", timer);


  cliFormat(ewsContext, pStrInfo_base_HoldtimePrn);
  ewsTelnetPrintf(ewsContext, "%d", holdTime);

  if(mode  == L7_ENABLE)
  {
    cliFormat(ewsContext, pStrInfo_base_V2AdvPrn);
    ewsTelnetPrintf(ewsContext, "%s", pStrInfo_base_Enabled);
  }
  else
  {
    cliFormat(ewsContext, pStrInfo_base_V2AdvPrn);
    ewsTelnetPrintf(ewsContext, "%s", pStrInfo_base_Disabled);
  }

  cliFormat(ewsContext, pStrInfo_base_NeighborTblLastChangePrn);
  osapiConvertRawUpTime(usmdbIsdpNeighborsTableLastChangeTimeGet(), &timespec);
  osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, pStrInfo_base_D, timespec.days,
                            timespec.hours, timespec.minutes, timespec.seconds);
  ewsTelnetPrintf(ewsContext, "%s", buf);

  if ( L7_SUCCESS != usmDbIsdpDeviceIdGet(deviceId))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0,
                       pStrErr_base_FailedToGet, ewsContext,
                                              pStrInfo_base_PrnDeviceID);
  }
  cliFormat(ewsContext, pStrInfo_base_PrnDeviceID);
  ewsTelnetPrintf(ewsContext, "%s", deviceId);

  if(usmdbIsdpDeviceIdFormatCapabilityGet(&deviceIdFormatCpb) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0,
                         pStrErr_base_FailedToGet, ewsContext,
                               pStrInfo_base_IsdpDevIdFormatCpb);
  }
  usmdbIsdpDevIdFormatCapabilityStringGet(deviceIdFormatCpb, capCh,
                              L7_CLI_MAX_STRING_LENGTH);

  cliFormat(ewsContext, pStrInfo_base_DevIdFormatCapPrn);
  ewsTelnetPrintf(ewsContext, "%s", capCh);

  if(usmdbIsdpDeviceIdFormatGet(&deviceIdFormat) != L7_SUCCESS)
  {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0,
                      pStrErr_base_FailedToGet, ewsContext,
                           pStrInfo_base_IsdpDevIdFormat);
  }

  usmdbIsdpDevIdFormatStringGet(deviceIdFormat, capCh, L7_CLI_MAX_STRING_LENGTH);
  cliFormat(ewsContext, pStrInfo_base_DevIdFormatPrn);
  ewsTelnetPrintf(ewsContext, "%s", capCh);

  cliSyntaxBottom(ewsContext);

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliPrompt(ewsContext);
}

/******************************************************************************
*
* @purpose  Displays ISDP entries information.
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
* @cmdsyntax  show isdp entry *|<device id>
*
* @cmdhelp Displays ISDP entries information.
*
* @cmddescript Displays ISDP entries information.
*
* @end
*
******************************************************************************/
const L7_char8 * commandShowIsdpEntry(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  static L7_char8 deviceId[L7_ISDP_DEVICE_ID_LEN];
  static L7_uint32 intIfNum = 0;
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mode, count, i;

  memset(cmdBuf, 0x00, sizeof(cmdBuf));

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    return cliSyntaxReturnPrompt(ewsContext, pStrInfo_base_IsdpModePrn);
  }

  if(cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      intIfNum = 0;
      memset(deviceId, 0x00, sizeof(deviceId));
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if(strcmp(argv[index], pStrInfo_common_All) == 0)
    {
      memset(deviceId, 0x00, sizeof(deviceId));
      rc = usmdbIsdpNeighborGetNext(intIfNum, deviceId, &intIfNum, deviceId);
    }
    else
    {
      if((strcmp(argv[index], pStrInfo_base_IsdpEntryName) != 0) &&
			    (strlen(argv[index]) > 0))
      {
	memset(deviceId, 0x00, sizeof(deviceId));
	osapiStrncpy(deviceId, argv[index], sizeof(deviceId));
	rc = usmdbIsdpDeviceIdNeighborGetNext(0, deviceId,
				  &intIfNum, deviceId);
      }
      else
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_PrnNoDeviceID);
      }
    }
    if (rc != L7_SUCCESS)
    {
      intIfNum = 0;
      memset(deviceId, 0x00, sizeof(deviceId));
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
		     L7_NULLPTR, ewsContext, pStrErr_base_IsdpNeighborGetNext);
    }
  }

  if(strcmp(argv[index], pStrInfo_common_All) == 0)
  {
    count = 0;
    while((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      cliIsdpShowNeighborDetailWrite(ewsContext, intIfNum, deviceId);
      rc = usmdbIsdpNeighborGetNext(intIfNum, deviceId, &intIfNum, deviceId);
      if (rc != L7_SUCCESS)
      {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      intIfNum = 0;
      memset(deviceId, 0x00, sizeof(deviceId));
      return cliPrompt(ewsContext);
      }
      count = count + 16;/* 16 rows may be in entry*/
    }
  }
  else if((strcmp(argv[index], pStrInfo_base_IsdpEntryName) != 0) &&
			   (strlen(argv[index]) > 0))
  {
    count = 0;
    while((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      cliIsdpShowNeighborDetailWrite(ewsContext, intIfNum, deviceId);
      rc = usmdbIsdpDeviceIdNeighborGetNext(intIfNum, deviceId,
					    &intIfNum, deviceId);
      if (rc != L7_SUCCESS)
      {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      intIfNum = 0;
      memset(deviceId, 0x00, sizeof(deviceId));
      return cliPrompt(ewsContext);
      }
      count = count + 16;
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
  for (i=1; i<argc; i++)
  {
    OSAPI_STRNCAT(cmdBuf, " ");
    OSAPI_STRNCAT(cmdBuf, argv[i]);
  }
  cliAlternateCommandSet(cmdBuf);
  return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
}

/******************************************************************************
*
* @purpose  Displays ISDP statistics.
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
* @cmdsyntax  show isdp traffic
*
* @cmdhelp Display ISDP statistics
*
* @cmddescript Display ISDP statistics
*
* @end
*
******************************************************************************/
const L7_char8 * commandShowIsdpTraffic(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 **argv,
                                        L7_uint32 index)
{
  L7_uint32 mode = L7_ENABLE;

  cliSyntaxTop(ewsContext);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    return cliSyntaxReturnPrompt(ewsContext, pStrInfo_base_IsdpModePrn);
  }
    cliFormat(ewsContext, pStrInfo_base_IsdpPacketsReceived);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficPduReceivedGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpPacketsTransmitted);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficPduTransmitGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpV1PacketsReceived);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficV1PduReceivedGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpV1PacketsTransmitted);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficV1PduTransmitGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpV2PacketsReceived);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficV2PduReceivedGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpV2PacketsTransmitted);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficV2PduTransmitGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpBadHeader);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficBadHeaderPduReceivedGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpChecksumError);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficChkSumErrorPduReceivedGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpTransmissionFailure);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficFailurePduTransmitGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpInvalidFormat);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficInvalidFormatPduReceivedGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpTableFull);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficTableFullGet());

    cliFormat(ewsContext, pStrInfo_base_IsdpIpAddressTableFull);
    ewsTelnetPrintf(ewsContext, "%d", usmdbIsdpTrafficIpAddressTableFullGet());

    cliSyntaxBottom(ewsContext);

    return cliPrompt(ewsContext);
}

/******************************************************************************
*
* @purpose  Displays ISDP mode per interface.
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
* @cmdsyntax  show process interface all|([unit/]slot/port <cr>)
*
* @cmdhelp Displays ISDP mode per interface.
*
* @cmddescript Displays ISDP mode per interface.
*
* @end
*
******************************************************************************/
const L7_char8 * commandShowIsdpInterface(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 * * argv,
                                          L7_uint32 index)
{
  L7_uint32 unit, slot, port, counter, i;
  L7_uint32 mode = L7_ENABLE;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 numArg = 0;
  static L7_uint32 intIfNum = 0;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  interface[L7_NIM_IFNAME_SIZE];

  memset(interface, 0x00, sizeof(interface));
  memset(cmdBuf, 0x00, sizeof(cmdBuf));

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    return cliSyntaxReturnPrompt(ewsContext, pStrInfo_base_IsdpModePrn);
  }

  numArg = cliNumFunctionArgsGet();

  if(cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      intIfNum = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if(strcmp(argv[index + numArg], pStrInfo_common_All) == 0)
    {
      if(intIfNum == 0)
      {
        rc = usmDbValidIntIfNumFirstGet(&intIfNum);
        if (rc != L7_SUCCESS)
        {
          intIfNum = 0;
          return cliPrompt(ewsContext);
        }
      }
    }
  }

  if (numArg == 1)
  {
    if(strcmp(argv[index + numArg], pStrInfo_common_All) == 0)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnInterfaceEntryTable,
                    pStrInfo_common_Intf, pStrInfo_common_Mode_1);
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);

      ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnInterfaceEntryTable,
                      pStrInfo_base_StrRow, pStrInfo_base_StrRow);
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      counter = 0;
      while((rc == L7_SUCCESS) && (counter < CLI_MAX_SCROLL_LINES-6))
      {
        if(usmdbIsdpIntfModeGet(intIfNum, &mode) == L7_SUCCESS)
        {
          usmDbIfNameGet(0, intIfNum, interface);
          if(mode  == L7_ENABLE)
          {
            ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnInterfaceEntryTable,
                              interface, pStrInfo_common_Enbld);
            ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
          }
          else
          {
          ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnInterfaceEntryTable,
                                   interface, pStrInfo_common_Dsbld);
          ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
          }
          counter++;
        }
        rc = usmDbValidIntIfNumNext(intIfNum, &intIfNum);
        if (rc != L7_SUCCESS)
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          intIfNum = 0;
          return cliPrompt(ewsContext);
        }
      } /* while((rc == L7_SUCCESS) && (counter < CLI_MAX_SCROLL_LINES-6)) */

      if(rc == L7_SUCCESS)
      {
	cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

	osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
	for (i=1; i<argc; i++)
	{
	  OSAPI_STRNCAT(cmdBuf, " ");
	  OSAPI_STRNCAT(cmdBuf, argv[i]);
	}
	cliAlternateCommandSet(cmdBuf);
	return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }
    }
    else if(cliValidSpecificUSPCheck(argv[index + numArg], &unit, &slot, &port)
                                   == L7_SUCCESS)
    {
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
      {
        intIfNum = 0;
        return cliPrompt(ewsContext);
      }
      
      if (usmdbIsdpIsValidIntf(intIfNum) != L7_TRUE)
      {
        intIfNum = 0;
        ewsTelnetWrite(ewsContext, pStrErr_base_IsdpCheckInvalidPort);
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
        return cliPrompt(ewsContext);
      }

      if(usmdbIsdpIntfModeGet(intIfNum, &mode) == L7_SUCCESS)
      {
	ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
	ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnInterfaceEntryTable,
		      pStrInfo_common_Intf, pStrInfo_common_Mode_1);
	ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);

	ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnInterfaceEntryTable,
		      pStrInfo_base_StrRow, pStrInfo_base_StrRow);
	ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
	usmDbIfNameGet(0, intIfNum, interface);
	if(mode == L7_ENABLE)
	{
	  ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnInterfaceEntryTable,
			    interface, pStrInfo_common_Enbld);
	  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
	}
	else
	{
	   ewsTelnetPrintf(ewsContext, pStrInfo_base_PrnInterfaceEntryTable,
			    interface, pStrInfo_common_Dsbld);
	   ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
	}
	rc = L7_FAILURE;
	}
	intIfNum = 0;
      }
    }
    return cliPrompt(ewsContext);
}
