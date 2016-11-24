/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/ip_mcast/cli_show_mcastengine.c
*
* @purpose show commands for the mcast forwarding engine cli functionality
*
* @component user interface
*
* @comments
*           An output value of Err means that the usmDb returned != SUCCESS
*           english num 1312
*           An output value of ------ means that the value type was
*           incompatible,
*           and even though the usmDb return SUCCESS; the value is garbage.
*
* @create  15/05/2002
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
#include <cliapi.h>
#include <datatypes.h>
#include <usmdb_ip_api.h>
#include <usmdb_util_api.h>
#include <clicommands_l3.h>
#include <clicommands_mcast.h>
#include <l3_commdefs.h>
#include <clicommands_mcast.h>
#include <usmdb_mib_mcast_api.h>
#include "usmdb_l3.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_mcast_api.h"
#include "usmdb_pimsm_api.h"
#endif

#include "clicommands_card.h"

/*********************************************************************
*
* @purpose  Displays multicast route table information
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
* @cmdsyntax    show ip mcast mroute {detail|summary}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpMcastMroute(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 ipMrouteGrp, ipMrouteSrc, ipMrouteSrcMask, outIntIfNumStart = 0;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipVal, time, flag;
  L7_uint32 count,  val, intIfNum, slot, port, u, count1, outIntIfNumCheck = 0;
  L7_uint32 unit, numArgs, argSlotPort = 1,outIntIfNum;
  static L7_inet_addr_t inetIpMrouteSrc, inetIpMrouteGrp,inetIpMrouteSrcMask, inetIpVal, inetRpAddr;
  L7_RC_t rc;
  L7_uint32 line_count=0;
  static L7_RC_t rc_pimsm_sg;
  static L7_BOOL firstTime = L7_TRUE;
  usmDbTimeSpec_t timeSpec;
  L7_INTF_MASK_t      outIntIfMask;
  L7_uint32 oifListStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32  numOifs = 0;
  
  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    cliSyntaxTop(ewsContext);
    ipMrouteGrp = 0;
    ipMrouteSrc = 0;
    ipMrouteSrcMask = 0;
    inetAddressSet(L7_AF_INET, &ipMrouteGrp,&inetIpMrouteGrp);
    inetAddressSet(L7_AF_INET, &ipMrouteSrc,&inetIpMrouteSrc);
    inetAddressSet(L7_AF_INET, &ipMrouteSrcMask,&inetIpMrouteSrcMask);
    inetAddressZeroSet(L7_AF_INET, &inetRpAddr);
    outIntIfNumStart = 0;
    rc_pimsm_sg = L7_SUCCESS;
    firstTime = L7_TRUE;
  }
  else
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      rc_pimsm_sg = L7_SUCCESS;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpMcastMroute_1);
  }

  if (strcmp(argv[index+argSlotPort], pStrInfo_common_McastGrpsDetail) <= 0)
  {

    ewsTelnetWriteAddBlanks (1, 0, 19, 37, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_McastRouteTbl);
    ewsTelnetWriteAddBlanks (1, 0, 32, 26, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_ExpiryUpTime);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_SrcIpGrpIpTimeSecsSecsRpfNeighborFlags_1);
    ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- ------------- ------------- --------------- -----");
    line_count=4;

    {
      cliCmdScrollSet( L7_FALSE);
      for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
      {
       if(usmDbMcastIpMRouteEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                           &inetIpMrouteSrc, &inetIpMrouteSrcMask) != L7_SUCCESS)
       {
         ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
         return cliSyntaxReturnPrompt (ewsContext, "");
       }
       memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        inetAddressGet(L7_AF_INET, &inetIpMrouteSrc, &ipMrouteSrc);
        if (ipMrouteSrc != 0)
        {
          if (usmDbInetNtoa(ipMrouteSrc, buf) == L7_SUCCESS)
          {
            osapiSnprintf(stat,sizeof(stat),"\r\n%-16s", buf);
          }
          else
          {
            osapiSnprintf(stat,sizeof(stat),"\r\n%-16s", pStrErr_common_Err);
          }
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat),"\r\n%-16s", "       *");
        }
        ewsTelnetWrite(ewsContext, stat);        /* Source IP */

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        inetAddressGet(L7_AF_INET, &inetIpMrouteGrp, &ipMrouteGrp);
        if (usmDbInetNtoa(ipMrouteGrp, buf) == L7_SUCCESS)
        {
          osapiSnprintf(stat,sizeof(stat),"%-16s", buf);
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat),"%-16s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);     /* Group IP */

        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbMcastIpMRouteExpiryGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                         &inetIpMrouteSrc, &inetIpMrouteSrcMask, &time);
        if(rc == L7_SUCCESS)
        {
          osapiConvertRawUpTime(time,(L7_timespec *)&timeSpec);
          osapiSnprintf(stat,sizeof(stat), "%2.2d:%2.2d:%2.2d      ", timeSpec.hours,
                                           timeSpec.minutes,timeSpec.seconds);
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-14s", pStrInfo_common_Line);
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat),"%-14s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);    /* Expiry Time */

        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbMcastIpMRouteUpTimeGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                         &inetIpMrouteSrc, &inetIpMrouteSrcMask, &time);
        if(rc == L7_SUCCESS)
        {
          osapiConvertRawUpTime(time,(L7_timespec *)&timeSpec);
          osapiSnprintf(stat,sizeof(stat), "%2.2d:%2.2d:%2.2d      ", timeSpec.hours,
                                           timeSpec.minutes,timeSpec.seconds);
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-14s", pStrInfo_common_Line);
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat),"%-14s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);    /* Up Time */

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbMcastIpMrouteRpfAddressGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                            &inetIpMrouteSrc, &inetIpMrouteSrcMask, &inetIpVal);
        if(rc == L7_SUCCESS)
        {
          inetAddressGet(L7_AF_INET, &inetIpVal, &ipVal);
          if (usmDbInetNtoa(ipVal, buf) == L7_SUCCESS)
          {
            osapiSnprintf(stat,sizeof(stat),"%-16s", buf);
          }
          else
          {
            osapiSnprintf(stat,sizeof(stat),"%-16s", pStrInfo_common_Line);
          }
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-16s", pStrInfo_common_Line);
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat),"%-16s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);    /* RPF Neighbor */

        /* display for RPT/SPT Flags */
        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbMcastIpMRouteFlagsGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                       &inetIpMrouteSrc, &inetIpMrouteSrcMask, &flag);
        if(rc == L7_SUCCESS)
        {

          switch (flag)
          {
          case L7_MCAST_PIMSM_FLAG_RPT:
            osapiSnprintf (stat,sizeof(stat),"%-6s", pStrInfo_ipmcast_Rpt);
            break;
          case L7_MCAST_PIMSM_FLAG_SPT:
            osapiSnprintf (stat,sizeof(stat),"%-6s", pStrInfo_ipmcast_Spt);
            break;
          default:
            osapiSnprintf(stat,sizeof(stat), "%-6s", pStrInfo_common_Null);
            break;
          }
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-6s", pStrInfo_common_Line);
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat),"%-6s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);
       
     }
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpMcastMrouteDetail);
    return pStrInfo_common_Name_2; /* --More-- or (q)uit */
   }
  }

  else if (strcmp(argv[index+argSlotPort], pStrInfo_common_McastShowSummary) <= 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 18, 19, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_McastRouteTblSummary);
    ewsTelnetWriteAddBlanks (1, 0, 43, 7, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IncomingOutgoing);
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_SrcIpGrpIpProtoIntfIntfList);
    ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- ---------- --------- ---------------");
    line_count = 4;
    {
    cliCmdScrollSet( L7_FALSE);
    for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
    {
        if (firstTime == L7_TRUE)
        {
          memset(buf, L7_NULL, sizeof(buf));
          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbMcastIpMRouteEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
              &inetIpMrouteSrc, &inetIpMrouteSrcMask) != L7_SUCCESS)
          {
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliSyntaxReturnPrompt (ewsContext, "");
          }
          inetAddressGet(L7_AF_INET, &inetIpMrouteSrc, &ipMrouteSrc);
          if (ipMrouteSrc != 0)
          {
            if (usmDbInetNtoa(ipMrouteSrc, buf) == L7_SUCCESS)
            {
              osapiSnprintf(stat,sizeof(stat),"\r\n%-16.15s", buf);
            }
            else
            {
              osapiSnprintf(stat,sizeof(stat),"\r\n%-16.15s", pStrErr_common_Err);
            }
          }
          else
          {
            osapiSnprintf(stat,sizeof(stat),"\r\n%-16.15s", "       *");
          }
          ewsTelnetWrite(ewsContext, stat);

          memset(buf, L7_NULL, sizeof(buf));
          memset(stat, L7_NULL, sizeof(stat));
          inetAddressGet(L7_AF_INET, &inetIpMrouteGrp, &ipMrouteGrp);
          if (usmDbInetNtoa(ipMrouteGrp, buf) == L7_SUCCESS)
          {
            osapiSnprintf(stat,sizeof(stat),"%-16.15s", buf);
          }
          else
          {
            osapiSnprintf(stat,sizeof(stat),"%-16.15s", pStrErr_common_Err);
          }
          ewsTelnetWrite(ewsContext, stat);

          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbMcastIpMRouteProtocolGet(L7_AF_INET, unit, &inetIpMrouteGrp,
              &inetIpMrouteSrc, &inetIpMrouteSrcMask, &val) == L7_SUCCESS)
          {
            cliMcastProtocolStrPrint(val, stat, sizeof(stat), "%-11s");
          }
          else
          {
            osapiSnprintf(stat,sizeof(stat),"%-11.10s", pStrErr_common_Err);
          }
          ewsTelnetWrite(ewsContext, stat);

          memset(buf, L7_NULL, sizeof(buf));
          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbMcastIpMRouteIfIndexGet(L7_AF_INET, unit, &inetIpMrouteGrp,
              &inetIpMrouteSrc, &inetIpMrouteSrcMask, &intIfNum) == L7_SUCCESS)
          {
            if (intIfNum == 0)
            {
              osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf),pStrInfo_common_EmptyString);
            }
            else if (usmDbUnitSlotPortGet(intIfNum, &u, &slot,
                                          &port) == L7_SUCCESS)
            {
              osapiSnprintf(buf,sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
            }
            else
            {
              osapiSnprintf(buf,sizeof(buf), pStrErr_common_Err);
            }
          }
          else
          {
            osapiSnprintf(buf,sizeof(buf), pStrErr_common_Err);
          }
          ewsTelnetPrintf (ewsContext, "%-10.9s",buf);
        }
        else
        {
          ewsTelnetWrite(ewsContext, "\r\n                                                     ");
        }
   
      if (usmDbPimsmIsOperational(unit, L7_AF_INET) != L7_TRUE)
      {
        /* outgoing interface list*/
        count1 = 1;
        outIntIfNum = outIntIfNumStart;
          while (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                                       &inetIpMrouteSrc, &inetIpMrouteSrcMask,
                                                       &outIntIfNum) == L7_SUCCESS)
          {
            memset(buf, L7_NULL, sizeof(buf));
            memset(stat, L7_NULL, sizeof(stat));
            if (usmDbUnitSlotPortGet(outIntIfNum, &u, &slot, &port) == L7_SUCCESS)
            {
              count1++;
              osapiSnprintf(buf, sizeof(buf),cliDisplayInterfaceHelp(u, slot, port));
            }
            else
            {
              osapiSnprintf(buf,sizeof(buf), pStrErr_common_Err);
            }
            ewsTelnetPrintf (ewsContext, "%-4s ",buf);
            outIntIfNumStart = outIntIfNum;
            outIntIfNumCheck = outIntIfNum;
            if (count1 > 3)
            {
              if (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                                        &inetIpMrouteSrc, &inetIpMrouteSrcMask,
                                                        &outIntIfNumCheck) == L7_SUCCESS)
              {
                ewsTelnetWrite(ewsContext, "\r\n                                                     ");
                count++;
              }
              count1 = 1;
              if (count >= 15)
              {
                firstTime = L7_FALSE;
                if (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                                          &inetIpMrouteSrc, &inetIpMrouteSrcMask,
                                                          &outIntIfNum) == L7_FAILURE)
                {
                  outIntIfNumStart = 0; 
                  firstTime = L7_TRUE;
                }
                cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
                cliSyntaxBottom(ewsContext);
                cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpMcastMrouteSummary);
                return pStrInfo_common_Name_2; /* --More-- or (q)uit */
              }
            }

          }
          outIntIfNumStart = 0;
          firstTime = L7_TRUE;
        }
        else
        {
          memset(&outIntIfMask, 0, sizeof(L7_INTF_MASK_t));
          usmDbPimsmMapOIFGet(L7_AF_INET, &inetIpMrouteGrp, &inetIpMrouteSrc,
                                   &inetIpMrouteSrcMask, &outIntIfMask);
          memset(oifListStr, 0, sizeof(oifListStr));
          usmDbConvertMaskToList(&outIntIfMask, oifListStr, &numOifs);

          count1 = 1;
          outIntIfNum = 1;
          if (firstTime != L7_TRUE)
          {
            outIntIfNum = outIntIfNumStart + 1;
          }
          for (; outIntIfNum <= numOifs; outIntIfNum++)
          {
            memset(buf, L7_NULL, sizeof(buf));
            memset(stat, L7_NULL, sizeof(stat));
            if (usmDbUnitSlotPortGet(oifListStr[outIntIfNum], &u, &slot, &port) == L7_SUCCESS)
            {
              count1++;
              osapiSnprintf(buf,sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
            }
            else
            {
              osapiSnprintf(buf,sizeof(buf), pStrErr_common_Err);
            }
            ewsTelnetPrintf (ewsContext, "%-4s ",buf);
            outIntIfNumStart = outIntIfNum;

            if (count1 > 3)
            {
              if (outIntIfNum < numOifs)
              {
                ewsTelnetWrite(ewsContext, "\r\n                                                     ");
                count++;
              }
              count1 = 1;
              if (count >= 15)
              {
                firstTime = L7_FALSE;
                if (outIntIfNum >= numOifs)
                {
                  outIntIfNumStart = 1;
                  firstTime = L7_TRUE;
                }
                cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
                cliSyntaxBottom(ewsContext);
                cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpMcastMrouteSummary);
                return pStrInfo_common_Name_2; /* --More-- or (q)uit */
              }
            }
           }
          outIntIfNumStart = 1;
          firstTime = L7_TRUE;
        }
      }
      firstTime = L7_TRUE;
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliSyntaxBottom(ewsContext);
      cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpMcastMrouteSummary);
      return pStrInfo_common_Name_2; /* --More-- or (q)uit */
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpMcastMroute_1);
    cliSyntaxBottom(ewsContext);
  }
  rc_pimsm_sg=L7_SUCCESS;
  firstTime = L7_TRUE;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Displays multicast route table information for the specific group address
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
* @cmdsyntax    show ip mcast mroute group <group ipaddr> {detail|summary}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpMcastMrouteGroup(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argGrpIp = 1;
  L7_uint32 argSlotPort = 2;
  L7_uint32 ipGrp, ipMrouteGrp, ipMrouteSrc, ipMrouteSrcMask;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipVal, time, flag, outIntIfNumCheck = 0;
  L7_uint32 unit, numArgs, slot, port, intIfNum, outIntIfNum, val, u=1;
  L7_int32  count, countIface;
  L7_RC_t   rc;
  static L7_inet_addr_t inetIpMrouteGrp, inetIpMrouteSrc, inetIpMrouteSrcMask;
  L7_inet_addr_t inetIpVal,inetInputGrpAddr;
  L7_uint32 line_count=0;
  static L7_RC_t rc_pimsm_sg;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_uint32 outIntIfNumStart = 0;
  usmDbTimeSpec_t timeSpec;
  L7_INTF_MASK_t      outIntIfMask;
  L7_uint32 oifListStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32  numOifs = 0;

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    cliSyntaxTop(ewsContext);
    ipMrouteGrp = 0;
    ipMrouteSrc = 0;
    ipMrouteSrcMask = 0;
    inetAddressSet(L7_AF_INET, &ipMrouteGrp,&inetIpMrouteGrp);
    inetAddressSet(L7_AF_INET, &ipMrouteSrc,&inetIpMrouteSrc);
    inetAddressSet(L7_AF_INET, &ipMrouteSrcMask,&inetIpMrouteSrcMask);
    rc_pimsm_sg = L7_SUCCESS;
    outIntIfNumStart = 0;
    firstTime = L7_TRUE;
  }
  else
  {
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      rc_pimsm_sg = L7_SUCCESS;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpMcastMrouteGrp_1);
  }

  OSAPI_STRNCPY_SAFE(stat,argv[index + argGrpIp]);
  if (usmDbInetAton(stat, (L7_uint32 *)&ipGrp) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_TacacsInValIp);
  }

  inetAddressSet(L7_AF_INET,&ipGrp,&inetInputGrpAddr);
  if(inetIsInMulticast(&inetInputGrpAddr) != L7_TRUE)
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_TacacsInValIp);
  }

  if (strcmp(argv[index+argSlotPort], pStrInfo_common_McastGrpsDetail) <= 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 19, 37, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_McastRouteTbl);
    ewsTelnetWriteAddBlanks (1, 0, 32, 26, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_ExpiryUpTime);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_SrcIpGrpIpTimeSecsSecsRpfNeighborFlags_1);
    ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- ------------- ------------- --------------- -----");
    line_count = 4;

    cliCmdScrollSet( L7_FALSE);
    count = 0;
    while (count < (CLI_MAX_SCROLL_LINES-6))
    {
    rc = usmDbMcastIpMRouteEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask);

    if (rc != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

      inetAddressGet(L7_AF_INET, &inetIpMrouteGrp,&ipMrouteGrp);
      if (ipGrp != ipMrouteGrp)
      {
            continue;
        }
        count++;
        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        inetAddressGet(L7_AF_INET, &inetIpMrouteSrc, &ipMrouteSrc);
        if (ipMrouteSrc != 0)
        {
            if (usmDbInetNtoa(ipMrouteSrc, buf) == L7_SUCCESS)
            {
              osapiSnprintf(stat,sizeof(stat), "\r\n%-16.15s", buf);
            }
            else
            {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", pStrErr_common_Err);
            }
        }
        else
      {
            osapiSnprintf(stat, sizeof(stat), "\r\n%-16.15s", "       *");
      }
        ewsTelnetWrite(ewsContext, stat);    /* Source IP */

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
      inetAddressSet(L7_AF_INET, &ipMrouteGrp,&inetIpMrouteGrp);
        if (usmDbInetNtoa(ipMrouteGrp, buf) == L7_SUCCESS)
        {
            osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
        }
        else
        {
        osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);  /* Group IP */

        memset(stat, L7_NULL, sizeof(stat));
      rc = usmDbMcastIpMRouteExpiryGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &time);
      if (rc == L7_SUCCESS)
        {
          osapiConvertRawUpTime(time,(L7_timespec *)&timeSpec);
          osapiSnprintf(stat,sizeof(stat), "%2.2d:%2.2d:%2.2d      ", timeSpec.hours,
                                         timeSpec.minutes,timeSpec.seconds);
        }
      else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-14s", pStrInfo_common_Line);
        }
        else
        {
        osapiSnprintf(stat, sizeof(stat), "%-14s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);  /* Expiry Time */

        memset(stat, L7_NULL, sizeof(stat));
      if (usmDbMcastIpMRouteUpTimeGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &time) == L7_SUCCESS)
        {
          osapiConvertRawUpTime(time,(L7_timespec *)&timeSpec);
          osapiSnprintf(stat,sizeof(stat), "%2.2d:%2.2d:%2.2d      ", timeSpec.hours,
                                         timeSpec.minutes,timeSpec.seconds);
        }
        else
        {
        osapiSnprintf(stat, sizeof(stat), "%-14s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);   /* Up Time */

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbMcastIpMrouteRpfAddressGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &inetIpVal);
      if (rc == L7_SUCCESS)
        {
            inetAddressGet(L7_AF_INET, &inetIpVal, &ipVal);
            if (usmDbInetNtoa(ipVal, buf) == L7_SUCCESS)
            {
              osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
            }
            else
            {
          osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_Line);
            }
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-16s", pStrInfo_common_Line);
        }
        else
        {
        osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);     /* RPF Neighbor */

        /* display for RPT/SPT Flags */
        memset(stat, L7_NULL, sizeof(stat));
      rc = usmDbMcastIpMRouteFlagsGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &flag);
      if (rc == L7_SUCCESS)
        {

            switch (flag)
            {
              case L7_MCAST_PIMSM_FLAG_RPT:
          osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_ipmcast_Rpt);
                break;

              case L7_MCAST_PIMSM_FLAG_SPT:
          osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_ipmcast_Spt);
                break;

              default:
          osapiSnprintf(stat, sizeof(stat),  "%-6s", pStrInfo_common_Null);
                break;
            }
        }
      else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-6s", pStrInfo_common_Line);
        }
        else
        {
        osapiSnprintf(stat, sizeof(stat), "%-6s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);    /* Flags */

        /* display for RPT/SPT Flags to be done as well*/
    }
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_ipmcast_ShowIpMcastMrouteGroupDetail, argv[index + argGrpIp]);
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2; /* --More-- or (q)uit */
  }
  else if (strcmp(argv[index+argSlotPort], pStrInfo_common_McastShowSummary) <= 0)
  {
    cliCmdScrollSet( L7_FALSE);

    ewsTelnetWriteAddBlanks (1, 0, 18, 20, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_McastRouteTblSummary);
    ewsTelnetWriteAddBlanks (1, 0, 42, 8, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IncomingOutgoing);
    ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_SrcIpGrpIpProtoIntfIntfList_1);
    ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- --------- --------- ----------------");
    line_count = 4;

    count = 0;
    while (count < (CLI_MAX_SCROLL_LINES-6))
    {
      if (firstTime == L7_TRUE)
      {

    rc = usmDbMcastIpMRouteEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask);

    if (rc != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
      inetAddressGet(L7_AF_INET, &inetIpMrouteGrp, &ipMrouteGrp);
      if (ipGrp != ipMrouteGrp)
      {
        continue;
      }
      count++;
      memset(buf, L7_NULL, sizeof(buf));
      memset(stat, L7_NULL, sizeof(stat));
      inetAddressGet(L7_AF_INET, &inetIpMrouteSrc, &ipMrouteSrc);
      if (ipMrouteSrc != 0)
      {
        if (usmDbInetNtoa(ipMrouteSrc, buf) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", buf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", pStrErr_common_Err);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", "       *");
      }

      ewsTelnetWrite(ewsContext, stat);

      memset(buf, L7_NULL, sizeof(buf));
      memset(stat, L7_NULL, sizeof(stat));
      inetAddressSet(L7_AF_INET, &ipMrouteGrp,&inetIpMrouteGrp);
      if (usmDbInetNtoa(ipMrouteGrp, buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset(stat, L7_NULL, sizeof(stat));
      if (usmDbMcastIpMRouteProtocolGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &val) == L7_SUCCESS)
      {
        cliMcastProtocolStrPrint(val, stat, sizeof(stat), "%-10s");
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-10s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset(buf, L7_NULL, sizeof(buf));
      memset(stat, L7_NULL, sizeof(stat));
      if (usmDbMcastIpMRouteIfIndexGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &intIfNum) == L7_SUCCESS)
      {
        if (intIfNum == 0)
        {
          osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_EmptyString);
        }
        else
        if (usmDbUnitSlotPortGet(intIfNum, &u, &slot, &port) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
        }
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
      }
      ewsTelnetPrintf (ewsContext, "%-10s",buf);
      }
      else
      {
         ewsTelnetWrite(ewsContext, "\r\n                                                    ");
      }
     

      /* outgoing interface list*/
      countIface = 1;
      if (usmDbPimsmIsOperational(unit, L7_AF_INET) != L7_TRUE)
      {
        outIntIfNum = outIntIfNumStart;
        while (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &outIntIfNum) == L7_SUCCESS)
        {
          memset(buf, L7_NULL, sizeof(buf));
          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbUnitSlotPortGet(outIntIfNum, &u, &slot, &port) == L7_SUCCESS)
          {
            countIface++;
            osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
          }
          ewsTelnetPrintf (ewsContext, "%-4s ",buf);
          outIntIfNumStart = outIntIfNum;
          outIntIfNumCheck = outIntIfNum;
          if (countIface > 3)
          {
            if (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                                      &inetIpMrouteSrc, &inetIpMrouteSrcMask,
                                                      &outIntIfNumCheck) == L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, "\r\n                                                    ");
              count++;
            }
            countIface = 1;

            if (count >= 15)
            {
              firstTime = L7_FALSE;
              if (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                                        &inetIpMrouteSrc, &inetIpMrouteSrcMask,
                                                        &outIntIfNum) == L7_FAILURE)
              {
                outIntIfNumStart = 0; 
                firstTime = L7_TRUE;
              }
              cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
              cliSyntaxBottom(ewsContext);
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_ipmcast_ShowIpMcastMrouteGroupSummary, argv[index + argGrpIp]);
              cliAlternateCommandSet(buf); 
              return pStrInfo_common_Name_2;/* --More-- or (q)uit */
            }
          }
        }
        outIntIfNumStart = 0;
        firstTime = L7_TRUE;
      }
      else
      {
        memset(&outIntIfMask, 0, sizeof(L7_INTF_MASK_t));
        usmDbPimsmMapOIFGet(L7_AF_INET, &inetIpMrouteGrp, &inetIpMrouteSrc,
                            &inetIpMrouteSrcMask, &outIntIfMask);
        memset(oifListStr, 0, sizeof(oifListStr));
        usmDbConvertMaskToList(&outIntIfMask, oifListStr, &numOifs);

        outIntIfNum = 1;
        if (firstTime != L7_TRUE)
        {
          outIntIfNum = outIntIfNumStart + 1;
        }
        for ( ; outIntIfNum <= numOifs; outIntIfNum++)
        {
          memset(buf, L7_NULL, sizeof(buf));
          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbUnitSlotPortGet(oifListStr[outIntIfNum], &u, &slot, &port) == L7_SUCCESS)
          {
            countIface++;
            osapiSnprintf(buf,sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
          }
          else
          {
            osapiSnprintf(buf,sizeof(buf), pStrErr_common_Err);
          }
          ewsTelnetPrintf (ewsContext, "%-4s ",buf);
          outIntIfNumStart = outIntIfNum;

          if (countIface > 3)
          {
            if (outIntIfNum < numOifs)
            {
              ewsTelnetWrite(ewsContext, "\r\n                                                    ");
              count++;
            }
            countIface = 1;
            if (count >= 15)
            {
              firstTime = L7_FALSE;
              if (outIntIfNum >= numOifs)
              {
                outIntIfNumStart = 1;
                firstTime = L7_TRUE;
              }
              cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
              cliSyntaxBottom(ewsContext);
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_ipmcast_ShowIpMcastMrouteGroupSummary, argv[index + argGrpIp]);
              cliAlternateCommandSet(buf); 
              return pStrInfo_common_Name_2;/* --More-- or (q)uit */
            }
          }
        }
        outIntIfNumStart = 1;
        firstTime = L7_TRUE;
      }
    }
    firstTime = L7_TRUE;
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_ipmcast_ShowIpMcastMrouteGroupSummary, argv[index + argGrpIp]);
    cliAlternateCommandSet(buf); 
    return pStrInfo_common_Name_2;/* --More-- or (q)uit */

  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpMcastMrouteGrp_1);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays multicast route table information for the specific group address / source address
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
* @cmdsyntax  show ip mcast mroute source <sourceipaddr> {summary | detail}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpMcastMrouteSource(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSrcIp = 1;
  L7_uint32 argSummaryDetail = 2;
  L7_uint32 ipMrouteGrp, ipMrouteSrc, ipMrouteSrcMask;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipSrc, ipVal, time, flag, intIfNum, outIntIfNum, val;
  L7_RC_t   rc;
  L7_uint32 unit, numArgs, slot, port, u=1, outIntIfNumCheck = 0;
  L7_int32 count, countIface;
  static L7_inet_addr_t inetIpMrouteGrp, inetIpMrouteSrc, inetIpMrouteSrcMask;
  L7_inet_addr_t inetIpVal,inetInputSrcAddr;
  L7_uint32 line_count=0;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_uint32 outIntIfNumStart = 0;
  L7_INTF_MASK_t      outIntIfMask;
  L7_uint32 oifListStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32  numOifs = 0;
  usmDbTimeSpec_t timeSpec;

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    cliSyntaxTop(ewsContext);
    ipMrouteGrp = 0;
    ipMrouteSrc = 0;
    ipMrouteSrcMask = 0;
    outIntIfNumStart = 0;
    inetAddressSet(L7_AF_INET, &ipMrouteGrp,&inetIpMrouteGrp);
    inetAddressSet(L7_AF_INET, &ipMrouteSrc,&inetIpMrouteSrc);
    inetAddressSet(L7_AF_INET, &ipMrouteSrcMask,&inetIpMrouteSrcMask);
    firstTime = L7_TRUE;

  }
  else
  {
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
 
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpMcastMrouteSrc_1);
  }

  OSAPI_STRNCPY_SAFE(stat, argv[index + argSrcIp]);
  if ((cliValidIPAddrCheck(stat) != TRUE) ||
      (usmDbInetAton(stat, (L7_uint32 *)&ipSrc) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_SrcAddr);
  }

  inetAddressSet(L7_AF_INET,&ipSrc,&inetInputSrcAddr);
  if(inetIsValidHostAddress(&inetInputSrcAddr) != L7_TRUE)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_SrcAddr);
  }

  OSAPI_STRNCPY_SAFE(stat, argv[index + argSummaryDetail]);

  if (strcmp(stat, pStrInfo_common_McastShowSummary) == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 18, 20, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_McastRouteTblSummary);
    ewsTelnetWriteAddBlanks (1, 0, 42, 8, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IncomingOutgoing);
    ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_SrcIpGrpIpProtoIntfIntfList_1);
    ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- --------- --------- ----------------");
    line_count = 4;
 
    cliCmdScrollSet( L7_FALSE);
    count = 0;
    while (count < (CLI_MAX_SCROLL_LINES-6))
    {
     if (firstTime == L7_TRUE)
     {
     rc = usmDbMcastIpMRouteEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask);

     if (rc != L7_SUCCESS)
     {
       ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
       return cliSyntaxReturnPrompt (ewsContext, "");
     }

      inetAddressGet(L7_AF_INET, &inetIpMrouteSrc, &ipMrouteSrc);
      if (ipSrc != ipMrouteSrc) /* filter the first criteria*/
      {
        continue;
      }
      count++;  
      memset(buf, L7_NULL, sizeof(buf));
      memset(stat, L7_NULL, sizeof(stat));
      inetAddressGet(L7_AF_INET, &inetIpMrouteSrc, &ipMrouteSrc);
      if (ipMrouteSrc != 0)
      {
        if (usmDbInetNtoa(ipMrouteSrc, buf) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", buf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", pStrErr_common_Err);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", "       *");
      }
      ewsTelnetWrite(ewsContext, stat);

      memset(buf, L7_NULL, sizeof(buf));
      memset(stat, L7_NULL, sizeof(stat));
      inetAddressGet(L7_AF_INET, &inetIpMrouteGrp,&ipMrouteGrp);
      if (usmDbInetNtoa(ipMrouteGrp, buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset(stat, L7_NULL, sizeof(stat));
      if (usmDbMcastIpMRouteProtocolGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &val) == L7_SUCCESS)
      {
        cliMcastProtocolStrPrint(val, stat, sizeof(stat), "%-10s");
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-10s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset(buf, L7_NULL, sizeof(buf));
      memset(stat, L7_NULL, sizeof(stat));
      if (usmDbMcastIpMRouteIfIndexGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &intIfNum) == L7_SUCCESS)
      {
        if (intIfNum == 0)
        {
          osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_EmptyString);
        }
        else if (usmDbUnitSlotPortGet(intIfNum, &u, &slot, &port) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
        }
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
      }
      ewsTelnetPrintf (ewsContext, "%-11s",buf);
      }
      else
      {
        ewsTelnetWrite(ewsContext, "\r\n                                                     ");
      }


      /* outgoing interface list*/
      countIface = 1;
      if (usmDbPimsmIsOperational(unit, L7_AF_INET) != L7_TRUE)
      {
        outIntIfNum = outIntIfNumStart;
        while (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &outIntIfNum) == L7_SUCCESS)
        {
          memset(buf, L7_NULL, sizeof(buf));
          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbUnitSlotPortGet(outIntIfNum, &u, &slot, &port) == L7_SUCCESS)
          {
            countIface++;
            osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
          }
          ewsTelnetPrintf (ewsContext, "%-4s ",buf);
          outIntIfNumStart = outIntIfNum;
          outIntIfNumCheck = outIntIfNum;
          if (countIface > 3)
          {
            if (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                                      &inetIpMrouteSrc, &inetIpMrouteSrcMask,
                                                      &outIntIfNumCheck) == L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, "\r\n                                                     ");
              count++;
            }
            countIface = 1;
            if (count >= 15)
            {
              firstTime = L7_FALSE;
              if (usmDbMcastIpMRouteOutIntfEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp,
                                                        &inetIpMrouteSrc, &inetIpMrouteSrcMask,
                                                        &outIntIfNum) == L7_FAILURE)
              {
                outIntIfNumStart = 0; 
                firstTime = L7_TRUE;
              }
              cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
              cliSyntaxBottom(ewsContext);
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_ipmcast_ShowIpMcastMrouteSourceSummary, argv[index + argSrcIp]);
              cliAlternateCommandSet(buf); 
              return pStrInfo_common_Name_2;/* --More-- or (q)uit */
            }
          }
        }
        outIntIfNumStart = 0;
        firstTime = L7_TRUE;
      }
      else
      {
        memset(&outIntIfMask, 0, sizeof(L7_INTF_MASK_t));
        usmDbPimsmMapOIFGet(L7_AF_INET, &inetIpMrouteGrp, &inetIpMrouteSrc,
                            &inetIpMrouteSrcMask, &outIntIfMask);
        memset(oifListStr, 0, sizeof(oifListStr));
        usmDbConvertMaskToList(&outIntIfMask, oifListStr, &numOifs);

        outIntIfNum = 1;
        if (firstTime != L7_TRUE)
        {
          outIntIfNum = outIntIfNumStart + 1;
        }
        for ( ; outIntIfNum <= numOifs; outIntIfNum++)
        {
          memset(buf, L7_NULL, sizeof(buf));
          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbUnitSlotPortGet(oifListStr[outIntIfNum], &u, &slot, &port) == L7_SUCCESS)
          {
            countIface++;
            osapiSnprintf(buf,sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
          }
          else
          {
            osapiSnprintf(buf,sizeof(buf), pStrErr_common_Err);
          }
          ewsTelnetPrintf (ewsContext, "%-4s ",buf);
          outIntIfNumStart = outIntIfNum;
          if (countIface > 3)
          {
            if (outIntIfNum < numOifs)
            {
              ewsTelnetWrite(ewsContext, "\r\n                                                     ");
              count++;
            }
            countIface = 1;
            if (count >= 15)
            {
              firstTime = L7_FALSE;
              if (outIntIfNum >= numOifs)
              {
                outIntIfNumStart = 1;
                firstTime = L7_TRUE; 
              }
              cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
              cliSyntaxBottom(ewsContext);
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_ipmcast_ShowIpMcastMrouteSourceSummary, argv[index + argSrcIp]);
              cliAlternateCommandSet(buf);
              return pStrInfo_common_Name_2; /* --More-- or (q)uit */
            }
          }
        }
        outIntIfNumStart = 1;
        firstTime = L7_TRUE;
      }
    }
    firstTime = L7_TRUE;
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_ipmcast_ShowIpMcastMrouteSourceSummary, argv[index + argSrcIp]);
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2; /* --More-- or (q)uit */
  }
  else if(strcmp(stat, pStrInfo_common_McastGrpsDetail) <= 0)
  {
      ewsTelnetWriteAddBlanks (1, 0, 19, 37, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_McastRouteTbl);
      ewsTelnetWriteAddBlanks (1, 0, 32, 26, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_ExpiryUpTime);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_SrcIpGrpIpTimeSecsSecsRpfNeighborFlags_1);
      ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- ------------- ------------- --------------- -----");
      line_count=4;

      cliCmdScrollSet( L7_FALSE);
      count = 0;
      while (count < (CLI_MAX_SCROLL_LINES-6))
      {
      rc = usmDbMcastIpMRouteEntryNextGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask);

      if (rc != L7_SUCCESS)
      {
       ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
       return cliSyntaxReturnPrompt (ewsContext, "");
      }
        inetAddressGet(L7_AF_INET, &inetIpMrouteSrc, &ipMrouteSrc);
        if (ipSrc != ipMrouteSrc) /* filter the first criteria*/
        {
          continue;
        }
        count++; 
        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));

        if (ipMrouteSrc != 0)
        {
          if (usmDbInetNtoa(ipMrouteSrc, buf) == L7_SUCCESS)
          {
            osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", buf);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", pStrErr_common_Err);
          }
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", "       *");
        }
        ewsTelnetWrite(ewsContext, stat);   /*  Source IP */

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));

        inetAddressGet(L7_AF_INET, &inetIpMrouteGrp,&ipMrouteGrp);
        if (usmDbInetNtoa(ipMrouteGrp, buf) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);  /* Group IP */

        memset(stat, L7_NULL, sizeof(stat));
        rc =usmDbMcastIpMRouteExpiryGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &time);
        if (rc == L7_SUCCESS)
        {
          osapiConvertRawUpTime(time,(L7_timespec *)&timeSpec);
          osapiSnprintf(stat,sizeof(stat), "%2.2d:%2.2d:%2.2d      ", timeSpec.hours,
                                           timeSpec.minutes,timeSpec.seconds);
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-14s", pStrInfo_common_Line);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-14s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);   /* Expiry Time */

        memset(stat, L7_NULL, sizeof(stat));
        if (usmDbMcastIpMRouteUpTimeGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &time) == L7_SUCCESS)
        {
          osapiConvertRawUpTime(time,(L7_timespec *)&timeSpec);
          osapiSnprintf(stat,sizeof(stat), "%2.2d:%2.2d:%2.2d      ", timeSpec.hours,
                                         timeSpec.minutes,timeSpec.seconds);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-14s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);  /* Up Time  */        

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbMcastIpMrouteRpfAddressGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &inetIpVal);
        if (rc == L7_SUCCESS)
        {
          inetAddressGet(L7_AF_INET, &inetIpVal, &ipVal);
          if (usmDbInetNtoa(ipVal, buf) == L7_SUCCESS)
          {
            osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_Line);
          }
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-16s", pStrInfo_common_Line);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);   /* RPF Neighbor */

        /* display for RPT/SPT Flags */
        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbMcastIpMRouteFlagsGet(L7_AF_INET, unit, &inetIpMrouteGrp, &inetIpMrouteSrc, &inetIpMrouteSrcMask, &flag);
        if (rc == L7_SUCCESS)
        {
          switch (flag)
          {
          case L7_MCAST_PIMSM_FLAG_RPT:
            osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_ipmcast_Rpt);
            break;
          case L7_MCAST_PIMSM_FLAG_SPT:
            osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_ipmcast_Spt);
            break;
          default:
            osapiSnprintf(stat, sizeof(stat),  "%-6s", pStrInfo_common_Null);
            break;
          }
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          osapiSnprintf(stat,sizeof(stat),"%-6s", pStrInfo_common_Line);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-6s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);  /* Flags */
      
      }
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliSyntaxBottom(ewsContext);
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_ipmcast_ShowIpMcastMrouteSourceDetail, argv[index + argSrcIp]);
      cliAlternateCommandSet(buf); 
      return pStrInfo_common_Name_2;/* --More-- or (q)uit */

  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays static route associated with the particular source address.
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
* @cmdsyntax    show ip mcast mroute static [<src address>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpMcastMrouteStatic(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSrcIp = 1;
  static L7_uint32 ipSrc, ipMask, ipSrcTemp;
  L7_uint32 ipVal, ipRpfAddress, metric;
  L7_RC_t   rc;
  L7_uint32 unit, numArgs, count;
  L7_inet_addr_t inetIpRpfAddress, inetIpVal;
  static L7_inet_addr_t inetIpSrc, inetIpMask;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if ((numArgs != 0)&&(numArgs != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpMcastMrouteStatic);
  }

  if (numArgs == 1)
  {
    OSAPI_STRNCPY_SAFE(stat,argv[index + argSrcIp]);
    if (usmDbInetAton(stat, (L7_uint32 *)&ipSrc) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_TacacsInValIp);
    }

    inetAddressSet(L7_AF_INET, &ipSrc, &inetIpSrc);
    inetAddressZeroSet(L7_AF_INET, &inetIpMask);

    /* check if the static route with specified source ip address exists */
    if (usmDbMcastMapStaticMRouteMatchingEntryGet(unit, L7_AF_INET, &inetIpSrc, &inetIpMask) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrInfo_ipmcast_McastStaticRouteDoesntExist, stat);
    }

    memset(buf, L7_NULL, sizeof(buf));
    inetAddressGet(L7_AF_INET, &inetIpSrc, &ipSrcTemp);
    rc = usmDbInetNtoa(ipSrcTemp, buf);
    cliFormat(ewsContext, pStrInfo_ipmcast_SrcAddr_1);   /* Print Source Ipaddress*/
    ewsTelnetWrite(ewsContext, buf);

    memset(buf, L7_NULL, sizeof(buf));
    cliFormat(ewsContext, pStrInfo_ipmcast_SrcMask);
    inetAddressGet(L7_AF_INET, &inetIpMask, &ipMask);
    rc = usmDbInetNtoa(ipMask, buf);
    ewsTelnetPrintf (ewsContext, buf);

    memset(buf, L7_NULL, sizeof(buf));
    memset(stat, L7_NULL, sizeof(stat));
    if (usmDbMcastStaticMRouteRpfAddressGet( unit, L7_AF_INET, &inetIpSrc, &inetIpMask, &inetIpRpfAddress) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_ipmcast_RpfAddr);
        inetAddressGet(L7_AF_INET, &inetIpRpfAddress, &ipRpfAddress);
        rc = usmDbInetNtoa(ipRpfAddress, buf);
      ewsTelnetPrintf (ewsContext, buf);
    }

    memset(buf, L7_NULL, sizeof(buf));
    if (usmDbMcastStaticMRoutePreferenceGet(unit, L7_AF_INET, &inetIpSrc, &inetIpMask, &metric) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_Pref_2);
      ewsTelnetPrintf (ewsContext, "%d", metric);
    }
  }
  else if (numArgs == 0)
  {
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      inetAddressZeroSet(L7_AF_INET, &inetIpSrc);
      inetAddressZeroSet(L7_AF_INET, &inetIpMask);
    }
    else
    {
      if(L7_TRUE == cliIsPromptRespQuit())
        {
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliPrompt(ewsContext);
        }
    }

    ewsTelnetWriteAddBlanks (1, 0, 21, 30, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_StaticRoutes);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_SrcIpSrcMaskRpfAddrMetricIntf);
    ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- --------------- ----------");

    for (count = 0; count < 15; count++)
    {
        if (usmDbMcastStaticMRouteEntryNextGet(unit, L7_AF_INET, &inetIpSrc, &inetIpMask) != L7_SUCCESS)
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt (ewsContext, "");
        }

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        inetAddressGet(L7_AF_INET, &inetIpSrc, &ipSrc);
        if (usmDbInetNtoa(ipSrc, buf) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", buf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        inetAddressGet(L7_AF_INET, &inetIpMask, &ipVal);
        if (usmDbInetNtoa(ipVal, buf) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        if (usmDbMcastStaticMRouteRpfAddressGet(unit, L7_AF_INET, &inetIpSrc, &inetIpMask, &inetIpVal) == L7_SUCCESS)
        {
            inetAddressGet(L7_AF_INET, &inetIpVal, &ipVal);
            if (usmDbInetNtoa(ipVal, buf) == L7_SUCCESS)
            {
              osapiSnprintf(stat, sizeof(stat), "%-16s", buf);
            }
            else
            {
          osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
            }
        }
        ewsTelnetWrite(ewsContext, stat);

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        if (usmDbMcastStaticMRoutePreferenceGet(unit, L7_AF_INET, &inetIpSrc, &inetIpMask, &metric) == L7_SUCCESS)
        {
            osapiSnprintf(buf, sizeof(buf), "%d", metric);
        }
        else
        {
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
        }
      ewsTelnetPrintf (ewsContext, "%-11s",buf);
    }
    cliSetCharInputID(1, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpMcastMrouteStatic_1);
    return pStrInfo_common_ScrollMsg;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays the general multicast information.
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
* @cmdsyntax    show ip mcast
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpMcast(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val, protocol;
  L7_int32 unit, numArgs;

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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpMcast_1);
  }

  memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_ipmcast_AdminMode);
  if (usmDbMcastAdminModeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val,pStrInfo_common_Dsbl_1));
  }
  ewsTelnetWrite(ewsContext, stat);

  memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_ipmcast_ProtoState);
  if (usmDbMcastOperationalStateGet(unit) == L7_TRUE)
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Operational);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_NonOperational);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset(stat, L7_NULL, sizeof(stat));
  if (usmDbMcastIpMRouteTableMaxSizeGet(unit, &val) == L7_SUCCESS)
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_TblMaxSize);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  memset(stat, L7_NULL, sizeof(stat));
  if (usmDbMcastIpProtocolGet(unit, L7_AF_INET, &protocol) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_Proto);
    cliMcastProtocolStrPrint(protocol, stat, sizeof(stat), "%s");
    ewsTelnetWrite(ewsContext, stat);
  }

  val = 0;
  memset(stat, L7_NULL, sizeof(stat));
  cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_EntryCount);
  if (usmDbMcastIpMRouteEntryCountGet(unit, &val) == L7_SUCCESS)
  {
    ewsTelnetPrintf (ewsContext, "%u", val);
  }
  else
  {
    ewsTelnetPrintf (ewsContext, pStrErr_common_Err);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays the Interface information for Mcast.
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
* @cmdsyntax    show ip mcast interface <slot/port>
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpMcastInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 all = L7_FALSE;
  L7_uint32 val = L7_DISABLE;
  L7_int32 u = 1;
  L7_uint32 intIface, s, p, slot, port;
  L7_RC_t rc;
  L7_uint32 unit, numArgs;
  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpMcastIntf_1, cliSyntaxInterfaceHelp());
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
      if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &s, &p) != L7_SUCCESS)
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
      if (cliSlotPortToInterface(argv[index + argSlotPort], &unit, &s, &p, &intIface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IntfTtl);
  ewsTelnetWrite(ewsContext,"\r\n--------- -----");

  if (all == L7_FALSE)
  {
    rc = usmDbMcastIpMRouteInterfaceEntryGet(unit, intIface);
  }
  else
  {
    /* Get the first one*/
    intIface = 0; /* setting it to invalid interface Num*/
    rc = usmDbMcastIpMRouteInterfaceEntryNextGet(unit, &intIface);
  }

  while (rc == L7_SUCCESS)
  {
    memset(buf, L7_NULL, sizeof(buf));
    memset(stat, L7_NULL, sizeof(stat));
    if ( usmDbUnitSlotPortGet(intIface, &u, &slot, &port) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
    }
    ewsTelnetPrintf (ewsContext, "\r\n%-10s",buf);

    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbMcastIpMRouteInterfaceTtlGet(unit, intIface, &val) == L7_SUCCESS))
    {
      osapiSnprintf(buf, sizeof(buf),  "%-5u", val);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-5s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext,buf);

    if (!all)
    {
      /* diplay is for the specific entry only*/
      break;
    }
    rc = usmDbMcastIpMRouteInterfaceEntryNextGet(unit, &intIface);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays all the configured administrative scoped
*           multicast boundaries.
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
* @cmdsyntax    show ip mcast boundary {<slot/port>|all}
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpMcastBoundary(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 ipGrp = 0, ipMask = 0;
  L7_uint32 intIface = 1, slot, port, all = L7_FALSE, intIfNum;
  L7_int32 u = 1;
  L7_RC_t   rc;
  L7_uint32 unit, numArgs;
  L7_inet_addr_t inetIpGrp, inetIpMask;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpMcastBoundary, cliSyntaxInterfaceHelp());
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
      if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      if (cliSlotPortToInterface(argv[index + argSlotPort], &unit, &slot, &port, &intIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
  }

  /* Intializing to Zero */
  inetAddressSet(L7_AF_INET, &ipGrp, &inetIpGrp);
  inetAddressSet(L7_AF_INET, &ipMask, &inetIpMask);

  rc = usmDbMcastMrouteBoundaryEntryNextGet(unit, &intIface, &inetIpGrp, &inetIpMask);

  ewsTelnetWriteAddBlanks (1, 0, 10, 13, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_McastBoundary_1);
  ewsTelnetWriteAddBlanks (1, 0, 0, 11, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IntfGrpIpMask);
  ewsTelnetWrite(ewsContext,"\r\n--------- --------------- ---------------\r\n");

  while(rc == L7_SUCCESS)
  {
    if(all == L7_FALSE)
    {
      if(intIface != intIfNum)
      {
        rc = usmDbMcastMrouteBoundaryEntryNextGet(unit, &intIface, &inetIpGrp, &inetIpMask);
        continue;
      }
    }

    memset(buf, L7_NULL, sizeof(buf));
    memset(stat, L7_NULL, sizeof(stat));
    if ( usmDbUnitSlotPortGet(intIface, &u, &slot, &port) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
    }
    ewsTelnetPrintf (ewsContext, "%-10s",buf);

    inetAddressGet(L7_AF_INET, &inetIpGrp, &ipGrp);
    memset(buf, L7_NULL, sizeof(buf));
    memset(stat, L7_NULL, sizeof(stat));
    if(usmDbInetNtoa(ipGrp, buf) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%-16.15s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(buf, L7_NULL, sizeof(buf));
    memset(stat, L7_NULL, sizeof(stat));
    inetAddressGet(L7_AF_INET, &inetIpMask, &ipMask);
    if(usmDbInetNtoa(ipMask, buf) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%-16.15s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-16s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    rc = usmDbMcastMrouteBoundaryEntryNextGet(unit, &intIface, &inetIpGrp, &inetIpMask);
    cliSyntaxBottom(ewsContext);
  }

  return cliPrompt(ewsContext);
}
/*********************************************************************
* @purpose  Get the Protocol string for a given protocol
*
* @param  protocol  @b{(input)}  Protocol Id
* @param  str       @b{(input)}  Protocol string
*
* @returns  none  
* @end
*
*********************************************************************/
void cliMcastProtocolStrPrint(L7_MCAST_IANA_PROTO_ID_t protocol, L7_uchar8 *str,
                            L7_uint32 strLen, L7_char8 const *fmt)
{
  switch (protocol)
  {
  case L7_MCAST_IANA_MROUTE_PIM_SM:
    osapiSnprintf(str, strLen, fmt, pStrInfo_common_Pimsm);
    break;
  case L7_MCAST_IANA_MROUTE_PIM_DM:
    osapiSnprintf(str, strLen, fmt, pStrInfo_common_Pimdm);
    break;
  case L7_MCAST_IANA_MROUTE_DVMRP:
    osapiSnprintf(str, strLen, fmt, pStrInfo_common_Dvmrp_1);
    break;
  case L7_MCAST_IANA_MROUTE_IGMP_PROXY:
    osapiSnprintf(str, strLen, fmt, pStrInfo_ipmcast_IgmpProxy);
    break;
  default:
    osapiSnprintf(str, strLen, fmt, pStrInfo_ipmcast_NoProtoEnbld);
  }
}

