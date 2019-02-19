/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_filter.c
 *
 * @purpose static filter show commands for CLI
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  4/20/2008
 *
 * @author  akulkarn
 *
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "filter_exports.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_mfdb_api.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>
#include "clicommands_card.h"
#include "clicommands_filter.h"
#include "usmdb_filter_api.h"


/*********************************************************************
 *
 * @purpose  display static MAC filter information
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
 * @cmdsyntax  show macfilter
 *
 * @cmdhelp Display static MAC filter info.
 *
 * @cmddescript
 *   Show static MAC filter information.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShowMacFilter(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_BOOL srcSupport;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  srcSupport = usmDbFeaturePresentCheck(unit, L7_FILTER_COMPONENT_ID,L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID);

  /* destination port configuration for multicast mac addresses is supported for all platforms */
  if (srcSupport == L7_TRUE)
  {
    return commandShowMacFilterSourcePort(ewsContext,argc,argv,index);
  }
  else
  {
    return commandShowMacFilterNoSourcePort(ewsContext,argc,argv,index);
  }

}

/*********************************************************************
*
* @purpose  display static MAC filter information
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
* @cmdsyntax  show macfilter
*
* @cmdhelp Display static MAC filter info.
*
* @cmddescript
*   This is the function that executes on a platform that does have source port
*   filtering.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowMacFilterSourcePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc = L7_SUCCESS;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   static L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
   static L7_uint32 vlanId;
   L7_uint32 argMacAddr = 1;
   L7_uint32 argVlanId = 2;
   static L7_BOOL all;
   L7_uint32 srcCount = 0;
   L7_uint32 destCount = 0;
   L7_uint32 slot, port, unitNum, i;
   L7_uint32 lineCount = 0;
   L7_uint32 srcIntfList[L7_FILTER_MAX_INTF];
   L7_uint32 destIntfList[L7_FILTER_MAX_INTF];
   L7_char8  interfaceListSrc[256];
   L7_char8  interfaceListDest[256];
   L7_uint32 unit;
   L7_uint32 maxDisplaylength, displayItems, k;
   L7_BOOL firstLine;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  cliCmdScrollSet( L7_FALSE);
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

    if (cliNumFunctionArgsGet() == 1)
    {
      if (strcmp(argv[index+argMacAddr], pStrInfo_common_All) == 0)
      {
        all = L7_TRUE;
        if (L7_SUCCESS != usmDbFilterFirstGet(unit, mac, &vlanId))               /* get the first entry after the zero parameter */
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_EmptyMacFilterList);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowMacFilter);
      }
    }
    else if (cliNumFunctionArgsGet() == 2)
    {
      all = L7_FALSE;
      if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidMacAddrList);
      }
      OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

      memset ( mac, 0, sizeof(mac) );
      if (cliConvertMac(buf, mac) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr );
      }

      sscanf(argv[index+argVlanId], "%d", &vlanId);
      if (usmDbVlanIDGet(unit, vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
      }

      if (usmDbFilterIsConfigured(unit, mac, vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
      }

    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowMacFilter);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 34, 4, L7_NULLPTR, ewsContext,pStrInfo_base_SrcDst);
  ewsTelnetWriteAddBlanks (1, 0, 3, 6, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrVlanIdPortSPortS);
   ewsTelnetWrite(ewsContext,"\r\n-----------------  -------  -------------------  -------------------");

   while (rc == L7_SUCCESS)
   {

    memset(srcIntfList, 0, sizeof(srcIntfList));
    memset(destIntfList, 0, sizeof(destIntfList));

    rc = usmDbFilterSrcIntfListGet(unit, mac, vlanId, &srcCount, srcIntfList);
    if (rc==L7_NOT_SUPPORTED)
    {
      srcCount=0;
      rc=L7_SUCCESS;
    }
    rc = usmDbFilterDstIntfListGet(unit, mac, vlanId, &destCount, destIntfList);
    if (rc==L7_NOT_SUPPORTED)
    {
      destCount = 0;
      rc= L7_SUCCESS;
    }

    if (lineCount >= CLI_MAX_SCROLL_LINES-6)
    {
      lineCount = 0;
      break;
    }

    lineCount++;
    /* print MAC */
    memset (stat, 0,sizeof(stat));
    osapiSnprintf(buf,sizeof(buf),"\r\n%02X:%02X:%02X:%02X:%02X:%02X ",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    ewsTelnetPrintf (ewsContext, "%-22.22s ", buf);

    /* print VLAN Id */
    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    osapiSnprintf(buf, sizeof(buf), "%d", vlanId);
    ewsTelnetPrintf (ewsContext, "%-7.7s", buf);

    /* print source and destination ports */
    /* 19 is the column length and 7 is considered as the standard length for printing
       interfaces in the u/s/p format with one space for comma */
    maxDisplaylength = 19;
    displayItems = maxDisplaylength/7;
    i=0;
    firstLine = L7_TRUE;

    while((srcCount > 0) || (destCount > 0))
    {
      bzero(interfaceListSrc, sizeof(interfaceListSrc));
      bzero(interfaceListDest, sizeof(interfaceListDest));

      for(k = 1; k <= displayItems; k++)
      {
        if((firstLine == L7_FALSE) && (k==1))
        {
          ewsTelnetPrintf (ewsContext, "\r\n%-28s", pStrInfo_common_EmptyString);
        }

        if(srcIntfList[i] != 0)
        {
          usmDbUnitSlotPortGet(srcIntfList[i], &unitNum, &slot, &port);
          osapiSnprintf(stat, sizeof(stat),"%-6s", cliDisplayInterfaceHelp(unitNum, slot, port));
          srcCount--;
          OSAPI_STRNCAT(interfaceListSrc, stat);
          if (srcIntfList[i+1] != 0)
          {
            OSAPI_STRNCAT(interfaceListSrc, ",");
          }
        }

        if(destIntfList[i] != 0)
        {
          usmDbUnitSlotPortGet(destIntfList[i], &unitNum, &slot, &port);
          osapiSnprintf(stat, sizeof(stat), "%-6s", cliDisplayInterfaceHelp(unitNum, slot, port));
          destCount--;
          OSAPI_STRNCAT(interfaceListDest, stat);
          if (destIntfList[i+1]!= 0)
          {
            OSAPI_STRNCAT(interfaceListDest, ",");
          }
        }
        i++;

        if((destCount ==0) && (srcCount ==0))
          break;
      }

      while(strlen(interfaceListSrc) <= maxDisplaylength+2)
        OSAPI_STRNCAT(interfaceListSrc, " ");

      OSAPI_STRNCAT(interfaceListSrc, interfaceListDest);
      ewsTelnetWrite(ewsContext,interfaceListSrc);
      lineCount++;

      if(firstLine == L7_TRUE)
      {
        firstLine = L7_FALSE;
      }
    }

    cliSyntaxBottom(ewsContext);

    if ((all == L7_TRUE) &&  (L7_SUCCESS != usmDbFilterNextGet(unit, mac, vlanId, mac, &vlanId)))
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else if (all != L7_TRUE)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
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
* @purpose  display static MAC filter information without dest port
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
* @cmdsyntax  show macfilter
*
* @cmdhelp Display static MAC filter info.
*
* @cmddescript
*   This is the function that executes on a platform that does not have dest port
*   filtering.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowMacFilterNoDestPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   static L7_RC_t   rc;
   L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  macStr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
   static L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
   static L7_uint32 vlanId;
   static L7_uint32 srcIntfIndex;
   L7_uint32 argMacAddr = 1;
   L7_uint32 argVlanId = 2;
   L7_BOOL   printLine;
   static L7_BOOL all;
   static L7_uint32 srcCount;
   L7_uint32 slot, port, unitNum;
   L7_uint32 lineCount = 0;
   static L7_uint32 srcIntfList[L7_FILTER_MAX_INTF];
   L7_BOOL   commaFlag = L7_FALSE;
   static L7_BOOL   macLine;
   L7_uint32 unit;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliCmdScrollSet( L7_FALSE);
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
    if (cliNumFunctionArgsGet() == 1)
    {
      if (strcmp(argv[index+argMacAddr], pStrInfo_common_All) == 0)
      {
        all = L7_TRUE;
        if (L7_SUCCESS != usmDbFilterFirstGet(unit, mac, &vlanId))               /* get the first entry after the zero parameter */
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterListIsEmpty);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowMacFilter);
      }
    }
    else if (cliNumFunctionArgsGet() == 2)
    {
      all = L7_FALSE;
      if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidMacAddrList);
      }
      OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);
      OSAPI_STRNCPY_SAFE(macStr, argv[index + argMacAddr]);

      memset ( mac, 0, sizeof(mac) );
      if (cliConvertMac(buf, mac) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr );
      }

      sscanf(argv[index+argVlanId], "%d", &vlanId);
      if (usmDbVlanIDGet(unit, vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
      }

      if (usmDbFilterIsConfigured(unit, mac, vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
      }

    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowMacFilter);
    }
    srcIntfIndex = 0;
    rc = usmDbFilterSrcIntfListGet(unit, mac, vlanId, &srcCount, srcIntfList);
    macLine = L7_TRUE;
  }

  ewsTelnetWriteAddBlanks (1, 0, 34, 7, L7_NULLPTR, ewsContext,pStrInfo_common_Src);
  ewsTelnetWriteAddBlanks (1, 0, 3, 6, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrVlanIdPortS);
   ewsTelnetWrite(ewsContext,"\r\n-----------------  -------  -----------------------------------------");

   macLine = L7_TRUE;  /* Always show the mac addr and vlan id at the top of the page */

   while (( rc == L7_SUCCESS ) && (lineCount < CLI_MAX_SCROLL_LINES-6))
   {

      if (macLine == L7_TRUE)
      {
         /* print MAC */
      memset (stat, 0,sizeof(stat));
      osapiSnprintf(buf,sizeof(buf),"\r\n%02X:%02X:%02X:%02X:%02X:%02X ",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      ewsTelnetPrintf (ewsContext, "%-22.22s ", buf);

      /* print VLAN Id */
      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), "%d", vlanId);
      ewsTelnetPrintf (ewsContext, "%-7.7s", buf);

         /* print source port */

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
         commaFlag=L7_TRUE;
      }

      while (srcIntfIndex < srcCount)
      {
         if (srcIntfIndex >= (srcCount-1))
      {
        commaFlag = L7_FALSE;
      }

         if (usmDbUnitSlotPortGet(srcIntfList[srcIntfIndex], &unitNum, &slot, &port) == L7_SUCCESS)
         {
            printLine = L7_FALSE;
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(unitNum, slot, port));
            if (commaFlag == L7_TRUE)
        {
          OSAPI_STRNCAT(stat, ",");
        }
            if ((strlen(buf) + strlen(stat)) < 40)
            {
          OSAPI_STRNCAT(buf, stat);
               srcIntfIndex++;
               if (srcIntfIndex == srcCount)
          {
                  printLine = L7_TRUE;
          }
            }
            else
            {
               printLine = L7_TRUE;
            }
            if (printLine == L7_TRUE)
            {
               if (macLine == L7_TRUE)
               {
            osapiSnprintf(stat, sizeof(stat), buf);
                  macLine = L7_FALSE;
               }
               else
               {
            osapiSnprintf(stat, sizeof(stat), "\r\n                            %s", buf);
               }
               ewsTelnetWrite(ewsContext,stat);
          memset (buf, 0,sizeof(buf));
               lineCount++;
            }
            if (lineCount >= (CLI_MAX_SCROLL_LINES-6))
        {
               break;
        }
         }
      }

      if (srcIntfIndex >= srcCount)
      {
         if (all != L7_TRUE)
         {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");
         }
         if (usmDbFilterNextGet(unit, mac, vlanId, mac, &vlanId) == L7_SUCCESS)
         {
            srcIntfIndex = 0;
            rc = usmDbFilterSrcIntfListGet(unit, mac, vlanId, &srcCount, srcIntfList);
            macLine = L7_TRUE;
         }
         else
         {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");
         }
      }
   }

   cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  memset (buf, 0, sizeof(cmdBuf));
  if (cliNumFunctionArgsGet() == 1)
  {
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), pStrInfo_base_ShowMacAddrTblStaticAll);
  }
  else
  {
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), pStrInfo_base_ShowMacAddrTblStatic_1, macStr, vlanId);
  }

  cliAlternateCommandSet(cmdBuf);

  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose  display static MAC filter information
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
* @cmdsyntax  show macfilter
*
* @cmdhelp Display static MAC filter info.
*
* @cmddescript
*   This is the command that executes on platorms the do not have
*   source port filtering
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowMacFilterNoSourcePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc = L7_SUCCESS;
   L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
   L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   static L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
   static L7_uint32 vlanId;
   L7_uint32 argMacAddr = 1;
   L7_uint32 argVlanId = 2;
   static L7_BOOL all;
   L7_uint32 srcCount = 0;
   L7_uint32 destCount = 0;
   L7_uint32 slot, port, unitNum, i, temp;
   L7_uint32 lineCount = 0;
   L7_uint32 srcIntfList[L7_FILTER_MAX_INTF];
   L7_uint32 destIntfList[L7_FILTER_MAX_INTF];
   L7_char8  interfaceListSrc[256];
   L7_char8  interfaceListDest[256];
   L7_BOOL   commaFlag = L7_FALSE;
   L7_BOOL   first = L7_TRUE;
   L7_uint32 unit;

   cliSyntaxTop(ewsContext);

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliCmdScrollSet( L7_FALSE);
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

    if (cliNumFunctionArgsGet() == 1)
    {
      if (strcmp(argv[index+argMacAddr], pStrInfo_common_All) == 0)
      {
        all = L7_TRUE;
        if (L7_SUCCESS != usmDbFilterFirstGet(unit, mac, &vlanId))               /* get the first entry after the zero parameter */
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_EmptyMacFilterList);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowMacFilter);
      }
    }
    else if (cliNumFunctionArgsGet() == 2)
    {
      all = L7_FALSE;
      if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidMacAddrList);
      }
      OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

      memset ( mac, 0, sizeof(mac) );
      if (cliConvertMac(buf, mac) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
      }

      sscanf(argv[index+argVlanId], "%d", &vlanId);
      if (usmDbVlanIDGet(unit, vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
      }

      if (usmDbFilterIsConfigured(unit, mac, vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
      }

    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowMacFilter);
    }
  }
  ewsTelnetWriteAddBlanks (1, 0, 32, 4, L7_NULLPTR, ewsContext,pStrInfo_base_Dst);
  ewsTelnetWriteAddBlanks (1, 0, 3, 6, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrVlanIdPortS);
   ewsTelnetWrite(ewsContext,"\r\n-----------------  -------  -------------------");

   while (rc == L7_SUCCESS)
   {
      rc = usmDbFilterSrcIntfListGet(unit, mac, vlanId, &srcCount, srcIntfList);
      rc = usmDbFilterDstIntfListGet(unit, mac, vlanId, &destCount, destIntfList);

      if ((srcCount%4) > 0 || (destCount%4) > 0)
    {
      temp=1;
    }
    else
    {
      temp=0;
    }

    if (((srcCount/4  + temp + lineCount) >= CLI_MAX_SCROLL_LINES-6) ||
        ((destCount/4 + temp + lineCount) >= CLI_MAX_SCROLL_LINES-6))
    {
      break;
    }

    lineCount++;
    /* print MAC */
    memset (stat, 0,sizeof(stat));
    osapiSnprintf(buf,sizeof(buf),"\r\n%02X:%02X:%02X:%02X:%02X:%02X ",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    ewsTelnetPrintf (ewsContext, "%-22.22s ", buf);

    /* print VLAN Id */
    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    osapiSnprintf(buf, sizeof(buf), "%d", vlanId);
    ewsTelnetPrintf (ewsContext, "%-7.7s", buf);

      /* print destination ports */

    memset(interfaceListSrc, 0, sizeof(interfaceListSrc));
    memset(interfaceListDest, 0, sizeof(interfaceListDest));
    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
      commaFlag=L7_FALSE;

      for (i = 0; i < destCount; i++)
      {
         if (usmDbUnitSlotPortGet(destIntfList[i], &unitNum, &slot, &port) == L7_SUCCESS)
         {
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(unitNum, slot, port));
        if (commaFlag == L7_TRUE)
        {
          OSAPI_STRNCAT(interfaceListDest, ",");
        }
        commaFlag = L7_TRUE;
        OSAPI_STRNCAT(interfaceListDest, stat);
      }
    }
    first = L7_TRUE;
    osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListDest,40));
    do
    {
      if (first == L7_TRUE)
      {
        first = L7_FALSE;
        osapiSnprintf(stat, sizeof(stat), "%-19s", buf);
      }
      else
      {
        lineCount++;
        ewsTelnetPrintf (ewsContext, "\r\n%-28s", pStrInfo_common_EmptyString);

        osapiSnprintf(stat, sizeof(stat), "%-19s", buf);
      }
      ewsTelnetWrite(ewsContext,stat);

      osapiSnprintf(buf, sizeof(buf), cliProcessStringOutput(interfaceListDest,40));
      }
      while (strcmp(buf, interfaceListDest) != 0);

      if ((all == L7_TRUE) &&  (L7_SUCCESS != usmDbFilterNextGet(unit, mac, vlanId, mac, &vlanId)))
      {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else if (all != L7_TRUE)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

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
* @purpose  display static MAC filter information
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
* @cmdsyntax  show mac-address-table static { <macaddr> <vlanid> | all}
*
* @cmdhelp Display static MAC filter info.
*
* @cmddescript
*   Show static MAC filter information.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowMacAddressTableStatic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit;
  L7_BOOL srcSupport;
   L7_uint32 numArg;        /* New variable Added */

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  srcSupport = usmDbFeaturePresentCheck(unit, L7_FILTER_COMPONENT_ID,L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID);

  /* destination port configuration for multicast mac addresses is supported for all platforms */
  if (srcSupport == L7_TRUE)
  {
    return commandShowMacFilterSourcePort(ewsContext,argc,argv,index);
  }
  else
  {
    return commandShowMacFilterNoSourcePort(ewsContext,argc,argv,index);
  }

}

/*********************************************************************
*
* @purpose Displays Multicast Forwarding Database Static MAC Filtering
*          entry information
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
* @cmdsyntax    show mac-address-table staticfiltering
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowMacAddressTableStaticFiltering(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
   L7_RC_t rc;
   static usmdbMfdbEntry_t entry;
   usmdbMfdbUserInfo_t compInfo;
   L7_uint32 userId;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 listFor[256];
   L7_uint32 listFilt[256];
   L7_char8   interfaceListFor[256];
   L7_char8   interfaceListFilt[256];
   L7_BOOL commaFlag = L7_FALSE;
   L7_BOOL first = L7_TRUE;
   L7_BOOL emptyFwd = L7_TRUE;
   L7_BOOL emptyFlt = L7_TRUE;
   L7_uint32 u, s, p, i, count, numFwd, numFlt;
   L7_uint32 unit;
   L7_uint32 numArg;        /* New variable Added */

   cliSyntaxTop(ewsContext);

   cliCmdScrollSet( L7_FALSE);
   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowMfdbStatic);
  }

  userId = L7_MFDB_PROTOCOL_STATIC;

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

    /* Heading for the mfdb Static Filtering table */
    ewsTelnetWriteAddBlanks (1, 0, 6, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrTypeDescIntfs);
    ewsTelnetWrite(ewsContext,"\r\n-----------------------  -------  ----------------  -------------------------\r\n");
  }
  else
  {

    memset(&entry, 0, sizeof(usmdbMfdbEntry_t));
    memset(&compInfo, 0, sizeof(usmdbMfdbUserInfo_t));
    if (usmDbMfdbComponentEntryGetNext(unit, entry.usmdbMfdbVidMac, userId, &compInfo) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CurrentlyNoMcastStaticFilteringEntriesInTbl);
    }

    /* Heading for the mfdb Static Filtering table */
    ewsTelnetWriteAddBlanks (1, 0, 6, 3, L7_NULLPTR, ewsContext,pStrInfo_base_MacAddrTypeDescIntfs);
    ewsTelnetWrite(ewsContext,"\r\n-----------------------  -------  ----------------  -------------------------\r\n");
  }

  for (count=0; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    memset (buf, 0,sizeof(buf));
    memset (stat, 0,sizeof(stat));
    osapiSnprintf(buf, sizeof(buf),
        "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X ",
        entry.usmdbMfdbVidMac[0],
        entry.usmdbMfdbVidMac[1],
        entry.usmdbMfdbVidMac[2],
        entry.usmdbMfdbVidMac[3],
        entry.usmdbMfdbVidMac[4],
        entry.usmdbMfdbVidMac[5],
        entry.usmdbMfdbVidMac[6],
        entry.usmdbMfdbVidMac[7]);
    ewsTelnetPrintf (ewsContext, "%-25.26s", buf);

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

      /* put forwarding list in a buffer with commas to print to screen */
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

      /* put filtering list in a buffer with commas to print to screen */
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

  } /* end for */

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_base_ShowMacAddrTblStaticFilteringCmd);                             /*hard coded in command name for now, #define later :-) */
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */

}

