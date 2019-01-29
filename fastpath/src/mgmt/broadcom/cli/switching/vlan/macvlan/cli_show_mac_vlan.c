/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_mac_vlan.c
 *
 * @purpose MAC association to VLAN show commands for cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/30/2005
 *
 * @author  rjain
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "clicommands_card.h"
#include "ews.h"
#include "usmdb_vlan_mac_api.h"
#include "fdb_exports.h"
#include "cli_web_exports.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  display enteries of MAC association to VLAN
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show vlan association mac [<macaddr>]
*
* @cmdhelp
*
* @cmddescript   display summary information about MAC association to VLAN
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVlanAssociationMac(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv,
                                              L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 mac_address[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strMacAddr[L7_ENET_MAC_ADDR_LEN];
  L7_uint32 unit = 0, numArg = 0, count = 0, byte = 0;
  L7_uint32 argMACAddress = 1;
  static L7_uint32 nextVlanId = 0;
  L7_FDB_TYPE_t fdb_type = L7_SVL;
  L7_enetMacAddr_t tempMac;
  static L7_enetMacAddr_t temp, macAddr;
  L7_uchar8 vidMac[L7_CLI_MAX_STRING_LENGTH];

  memset(stat, 0, sizeof(stat));
  memset(strMacAddr, 0, sizeof(strMacAddr));
  memset(tempMac.addr, 0, L7_ENET_MAC_ADDR_LEN);
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* The no.of arguments can be 0 or 1 depending on whether the user wants to
     see entire MAC to VLAN association or a specific entry in the database. */
  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowVlanMacToVlan);
  }

  if(usmDbFDBTypeOfVLGet(unit, &fdb_type) != L7_SUCCESS)
  {
    fdb_type = L7_SVL;
  }

  if (numArg == 1)
  {
    OSAPI_STRNCPY_SAFE(stat, argv[index + argMACAddress]);
    if ( cliConvertMac(stat, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
    }

    if (cliIsRestrictedFilterMac(strMacAddr) == L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_MacAddr_1);
    }

    for(byte = 0; byte < L7_ENET_MAC_ADDR_LEN; byte++)
    {
      tempMac.addr[byte] = strMacAddr[byte];
    }

    if((usmDbVlanMacGet(unit, tempMac, &nextVlanId)) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_switching_NoEntryInMacVlanDbase);
    }

    if ( fdb_type == L7_IVL )
    {
      memset(vidMac, 0, sizeof(vidMac));
      rc = usmDbEntryVidMacCombine(nextVlanId, tempMac.addr, vidMac);
      if( rc == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_MacAddrVlanId);
        ewsTelnetWrite(ewsContext,"\r\n-----------------------  -------");
        cliSyntaxBottom(ewsContext);
        osapiSnprintf(mac_address,sizeof(mac_address),"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                      vidMac[0], vidMac[1], vidMac[2], vidMac[3], vidMac[4], vidMac[5], vidMac[6], vidMac[7]);
        osapiSnprintf(stat,sizeof(stat),"%-25s ", mac_address);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_switching_CombineVlanIdMacAddr);
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_MacAddrVlanId_1);
      ewsTelnetWrite(ewsContext,"\r\n-----------------  -------");
      cliSyntaxBottom(ewsContext);
      osapiSnprintf(mac_address,sizeof(mac_address),"%02X:%02X:%02X:%02X:%02X:%02X",
                    tempMac.addr[0], tempMac.addr[1], tempMac.addr[2], tempMac.addr[3], tempMac.addr[4], tempMac.addr[5]);
      osapiSnprintf(stat,sizeof(stat),"%-19s ", mac_address);
    }
    ewsTelnetWrite(ewsContext, stat);

    osapiSnprintf(buf, sizeof(buf), "%d",nextVlanId);
    ewsTelnetPrintf (ewsContext, "%-8s", buf);
  }
  else
  {

    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      if ( fdb_type == L7_IVL )
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_MacAddrVlanId);
        ewsTelnetWrite(ewsContext,"\r\n-----------------------  -------");
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_MacAddrVlanId_1);
        ewsTelnetWrite(ewsContext,"\r\n-----------------  -------");
        cliSyntaxBottom(ewsContext);
      }

      memset(temp.addr, 0, L7_ENET_MAC_ADDR_LEN);
      rc = usmDbVlanMacGetNext(unit, temp, &macAddr, &nextVlanId);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_switching_NoEntryInMacVlanDbase);
      }
    }

    for ( count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
    {

      if ( fdb_type == L7_IVL )
      {
        memset(vidMac, 0, sizeof(vidMac));
        rc =usmDbEntryVidMacCombine(nextVlanId, macAddr.addr, vidMac);
        if( rc == L7_SUCCESS)
        {
          osapiSnprintf(mac_address,sizeof(mac_address),"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                        vidMac[0], vidMac[1], vidMac[2], vidMac[3], vidMac[4], vidMac[5], vidMac[6], vidMac[7]);
          osapiSnprintf(stat,sizeof(stat),"%-25s ", mac_address);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_switching_CombineVlanIdMacAddr);
        }
      }
      else
      {
        osapiSnprintf(mac_address,sizeof(mac_address),"%02X:%02X:%02X:%02X:%02X:%02X",
                      macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
        osapiSnprintf(stat,sizeof(stat),"%-19s ", mac_address);
      }
      ewsTelnetWrite(ewsContext, stat);

      osapiSnprintf(buf, sizeof(buf), "%d",nextVlanId);
      ewsTelnetPrintf (ewsContext, "%-8s", buf);
      cliSyntaxBottom(ewsContext);

      memcpy(temp.addr, macAddr.addr, L7_ENET_MAC_ADDR_LEN);

      if ((usmDbVlanMacGetNext(unit, temp, &macAddr, &nextVlanId)) != L7_SUCCESS)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_switching_ShowVlanAssociationMacCmd);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 * @purpose  To print the running configuration of mac vlan Info
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
L7_RC_t cliRunningConfigMacVlanInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_enetMacAddr_t macAddr,temp;
  L7_uint32 nextVlanId;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  memset(temp.addr, 0, L7_ENET_MAC_ADDR_LEN);
  rc = usmDbVlanMacGetNext(unit, temp, &macAddr, &nextVlanId);
  if(rc == L7_SUCCESS)
  {
    do
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_switching_VlanAssociationMac, macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5], nextVlanId);
      EWSWRITEBUFFER(ewsContext, buf);
      memcpy(temp.addr, macAddr.addr, L7_ENET_MAC_ADDR_LEN);
    } while((usmDbVlanMacGetNext(unit, temp, &macAddr, &nextVlanId)) == L7_SUCCESS);
  }
  return rc;
}
