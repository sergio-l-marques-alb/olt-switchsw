/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/cli_show_vrrp_l3.c
 *
 * @purpose cli functions that implements the vrrp show functionality
 *
 * @component user interface
 *
 * @comments
 *
 * @create  12/10/2001
 *
 * @author  Samip Garg
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "cliapi.h"
#include "datatypes.h"
#include "string.h"
#include "l7_vrrp_api.h"
#include "clicommands.h"
#include "cli.h"
#include "ews.h"
#include "clicommands_l3.h"
#include "usmdb_mib_vrrp_api.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "clicommands_card.h"

/*********************************************************************
*
* @purpose  To show the vrrp general router information
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
* @cmdsyntax show ip vrrp
*
* @cmdhelp show general information about vrrp router
*
* @cmddescript
*   The command shows vrrp  router information
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVrrp(EwsContext ewsContext,
                                L7_uint32 argc,
                                const L7_char8 * * argv,
                                L7_uint32 index)
{
  L7_RC_t rc;
  L7_BOOL mode;
  L7_uint32 paramValue;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowVrrpIntfInfo);
  }

  /* VRRP Administrative Mode */

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat( ewsContext, pStrInfo_common_IgmpProxyAdminMode);
  rc = usmDbVrrpOperAdminStateGet(unit, &mode);
  ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode,pStrInfo_common_Dsbl_1));

  /* Router Checksum Errors */
  rc = usmDbVrrpRouterChecksumErrorGet(unit, &paramValue);
  cliFormat( ewsContext, pStrInfo_routing_RtrChecksumErrs);
  ewsTelnetPrintf (ewsContext, "%u",paramValue);

  /* Router Version Errors */
  rc = usmDbVrrpRouterVerErrorGet(unit, &paramValue);
  cliFormat( ewsContext, pStrInfo_routing_RtrVerErrs);
  ewsTelnetPrintf (ewsContext, "%u",paramValue);

  /* Router VRID Errors */
  rc = usmDbVrrpRouterVridErrorGet(unit, &paramValue);
  cliFormat( ewsContext, pStrInfo_routing_RtrVridErrs);
  return cliSyntaxReturnPrompt (ewsContext,"%u",paramValue);
}

/*********************************************************************
*
* @purpose  To show the detailed interface tracking on VRRP Interface
*
*
* @param EwsContext ewsContext
* @param L7_uchar8 vrid
* @param L7_uint32 intIfNum
*
*
* @notes none
*
*
* @end
*
*********************************************************************/
const L7_char8 *showVrrpTrackIntf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, 
    L7_uchar8 vrid, L7_uint32 intfcNum, L7_uint32 *count)
{

  L7_uint32 u,s,p,unit;
  static L7_uint32 trackIntIfNum;
  L7_BOOL val;
  L7_uchar8 prio_dec;
  L7_INTF_TYPES_t sysIntfType;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_FAILURE;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_uint32 numRows = 0;
 
  unit = cliGetUnitId(); 
  if (firstTime == L7_TRUE)
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_TrackIntfStateDecPrio);
    ewsTelnetWrite(ewsContext,"\r\n--------------- ----- ------------------");
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    firstTime= L7_FALSE;
    *count +=2;
  } 

  rc = usmDbVrrpVrIdIntfTrackIntfNextGet(unit, vrid, intfcNum, &trackIntIfNum, &prio_dec ); 
    
    while ((rc == L7_SUCCESS) && (trackIntIfNum != 0))
    {
      if (usmDbIntfTypeGet(trackIntIfNum, &sysIntfType) == L7_SUCCESS &&
          sysIntfType != L7_CPU_INTF &&
          sysIntfType != L7_LOOPBACK_INTF &&
          sysIntfType != L7_TUNNEL_INTF)
      {
        rc = usmDbUnitSlotPortGet(trackIntIfNum, &u, &s, &p);
        if (rc == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "\r\n%-16.6s ", cliDisplayInterfaceHelp(u, s, p));
          ewsTelnetWrite(ewsContext,stat);
        }

        usmDbVrrpOperTrackIntfStateGet(unit, vrid, intfcNum, trackIntIfNum, &val);
        switch (val)                                                        /* val = up or down */
        {
          case L7_TRUE:
            osapiSnprintf(stat,sizeof(stat),"%-6.5s ",pStrInfo_routing_Up);
            break;
          default:
            osapiSnprintf(stat,sizeof(stat),"%-6.5s ",pStrInfo_common_Down_1);
        }
        ewsTelnetWrite(ewsContext,stat);

        osapiSnprintf(stat,sizeof(stat),"%d ",prio_dec);
        ewsTelnetWrite(ewsContext,stat);
        numRows++;        
        *count = *count+1;
      }         
      if ((*count > CLI_MAX_SCROLL_LINES-6))
      {
        break;
      }
      rc = usmDbVrrpVrIdIntfTrackIntfNextGet(unit, vrid, intfcNum, &trackIntIfNum, &prio_dec );

    }/* end of while */

  if (rc == L7_SUCCESS)   
  {
    cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
    cliSyntaxBottom(ewsContext);

    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

    for (val=1; val<argc; val++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[val]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;
  }
   else if (numRows == 0 )
  {
    osapiSnprintf(stat,sizeof(stat),"%s",pStrErr_routing_NoTrackingIntfConfig);
    ewsTelnetWrite(ewsContext,stat);
  }
  firstTime = L7_TRUE;
  trackIntIfNum = 0;
  numRows = 0;
  return "NULL"; 
}

/*********************************************************************
*
* @purpose  To show the detailed route tracking on VRRP Interface
*
*
* @param EwsContext ewsContext
* @param L7_uchar8 vrid
* @param L7_uint32 intIfNum
*
*
* @notes none
*
*
* @end
*
*********************************************************************/
const L7_char8 *showVrrpTrackRoute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, 
    L7_uchar8 vrid, L7_uint32 intfcNum, L7_uint32 *count)
{

  L7_uint32 maskLength,unit,val;
  static L7_uint32 ipAddr = 0,mask=0;
  L7_uchar8 prio_dec;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strFinal [2*L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL reachable = L7_FALSE;
  static L7_BOOL firstTime = L7_TRUE;
  static L7_uint32 numRows = 0;
  L7_RC_t rc;
  
  unit = cliGetUnitId();
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_TrackRouteStateDecPrio);
    ewsTelnetWrite(ewsContext,"\r\n---------------------  --------- ------------------");
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    *count +=2;
  }
    
  rc = usmDbVrrpVridIntfTrackRouteNextGet(unit, vrid, intfcNum, &ipAddr, &mask, &prio_dec );

    while ((rc == L7_SUCCESS) && (ipAddr != 0) && (mask !=0) )
    {
        memset(buf, 0, sizeof(buf));
        memset (strFinal, 0 , sizeof (strFinal));
        rc = usmDbInetNtoa(ipAddr, buf);
        osapiSnprintf(stat, sizeof(stat),  "%s/", buf);
        OSAPI_STRNCAT(strFinal, stat);

        memset(buf, 0, sizeof(buf));
        rc = usmDbInetNtoa(mask, buf);
        maskLength = usmCliMaskLength (mask);
        osapiSnprintf(stat, sizeof(stat),  "%d ", maskLength);
        OSAPI_STRNCAT(strFinal, stat);
        
        osapiSnprintf(stat, sizeof(stat),"\r\n%-22.19s",strFinal);
        ewsTelnetWrite(ewsContext,stat);

        usmDbVrrpOperTrackRouteReachabilityGet(unit, vrid, intfcNum, ipAddr, mask, &reachable); 
        switch(reachable)
        {
          case L7_TRUE  : osapiSnprintf(stat,sizeof(stat),"%-11.5s ",pStrInfo_common_True2);
                        break;

          case L7_FALSE : osapiSnprintf(stat,sizeof(stat),"%-11.5s ",pStrInfo_common_False2);
                        break;
            
          default     : osapiSnprintf(stat,sizeof(stat),"%-11.5s ",pStrInfo_common_False2);
                        break;
            
        }
        ewsTelnetWrite(ewsContext,stat);
        osapiSnprintf(stat,sizeof(stat),"%d ",prio_dec);
        ewsTelnetWrite(ewsContext,stat);

        numRows++;
        *count = *count+1;
        if ((*count > CLI_MAX_SCROLL_LINES-6))
        {
          break;
        }

        rc = usmDbVrrpVridIntfTrackRouteNextGet(unit, vrid, intfcNum, &ipAddr, &mask, &prio_dec );
  }/* end of while */
    
    if (rc == L7_SUCCESS)
    {
      cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
      cliSyntaxBottom(ewsContext);

      osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

      for (val=1; val<argc; val++)
      {
        OSAPI_STRNCAT(cmdBuf, " ");
        OSAPI_STRNCAT(cmdBuf, argv[val]);
      }
      cliAlternateCommandSet(cmdBuf);
      return pStrInfo_common_Name_2;
    }
    else if (numRows == 0)
    {
      osapiSnprintf(stat,sizeof(stat),"%s",pStrErr_routing_NoTrackingRouteConfig);
      ewsTelnetWrite(ewsContext,stat);
    }
  firstTime = L7_TRUE;
  ipAddr = 0;
  mask = 0;
  numRows = 0;
  return "NULL";
}




/*********************************************************************
*
* @purpose  To show the detailed information of interface
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
* @cmdsyntax show ip vrrp interface <slot/port> <vrId>
*
* @cmdhelp Show the configuration information of a virtual router.
*
* @cmddescript
*   This command shows the virtual router information.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVrrpInterface(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 * * argv,
                                         L7_uint32 index)
{
  L7_RC_t rc;
  L7_uchar8 vrID;
  L7_uint32 intfcNum, s, p, val;
  L7_uchar8 vmac[L7_MAC_ADDR_LEN];    /*changes made on 12/12*/
  L7_vrrpStartState_t mode;
  L7_AUTH_TYPES_t authType;
  L7_uchar8 priority;
  L7_uchar8 advIntvl;
  L7_BOOL prmtFlg;
  L7_uint32 ipAddress;
  L7_vrrpState_t state;
  L7_char8 buffer[L7_MACADDR_SIZE];
  L7_char8 ipAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 nextIpAddress;
  L7_uint32 unit, numArgs;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 secondaryAddrCount = 0;
  L7_uint32 count = 0;
  static L7_BOOL firstTime = L7_TRUE, printTrackIntf = L7_FALSE;

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      firstTime = L7_TRUE;
      printTrackIntf = L7_FALSE;
      ewsCliDepth(ewsContext, cliPrevDepthGet(),argv);
      return cliPrompt(ewsContext);
    }
  }
  
  numArgs = cliNumFunctionArgsGet();

  if (numArgs != 2)
  {
    if (numArgs == 1)
    {
      ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowVrrpIntfCmdNotFound);
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf),  pStrErr_routing_ShowVrrpIntfDetail, cliSyntaxInterfaceHelp());
      ewsTelnetWrite( ewsContext, buf);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index+1], &unit, &s, &p) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index + 1], &s,&p,&intfcNum) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliPrompt(ewsContext);
  }


  if( cliConvertRouterID(ewsContext,argv[index + 2],&vrID)
     != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
 if (firstTime == L7_TRUE) 
 {
  /* IP Address */
  if (usmDbVrrpIpAddressNextGet(unit, vrID, intfcNum, 0, &ipAddress) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
  }

  memset (ipAddr, 0,L7_CLI_MAX_STRING_LENGTH);
  rc = usmDbInetNtoa( ipAddress, ipAddr);
  cliFormat( ewsContext, pStrInfo_routing_IpAddrPrimary);
  ewsTelnetWrite( ewsContext, ipAddr);
  count++;
  do
  {
    rc = usmDbVrrpIpAddressNextGet(unit, vrID, intfcNum, ipAddress, &nextIpAddress);

    if (rc != L7_SUCCESS || nextIpAddress == L7_NULL_IP_ADDR || ipAddress == nextIpAddress)
    {
      break;
    }

    ipAddress = nextIpAddress;
    secondaryAddrCount++;
    count++;
    memset (ipAddr, 0,L7_CLI_MAX_STRING_LENGTH);
    rc = usmDbInetNtoa( ipAddress, ipAddr);
    if (secondaryAddrCount == 1)
    {
      cliFormat(ewsContext,pStrInfo_routing_IpAddresSecondary);
    }                                                                             /* ip addr */
    else
    {
      cliFormat(ewsContext,pStrInfo_common_EmptyString);
    }
    ewsTelnetWrite( ewsContext, ipAddr);

  } while (rc == L7_SUCCESS);

  /* VMAC Address */
  rc = usmDbVrrpOperVirtualMacAddrGet(unit, vrID,intfcNum, vmac);
  osapiSnprintf(buffer, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x",vmac[0],vmac[1],vmac[2],vmac[3],
                vmac[4],vmac[5]);
  cliFormat( ewsContext, pStrInfo_routing_VmacAddr);
  ewsTelnetWrite( ewsContext, buffer);

  /* Authentication Type */
  rc = usmDbVrrpOperAuthTypeGet(unit,vrID,intfcNum,&authType);
  cliFormat( ewsContext, pStrInfo_common_AuthType_1);
  if( authType == L7_AUTH_TYPE_NONE)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_None_1);
  }
  else if(authType == L7_AUTH_TYPE_SIMPLE_PASSWORD)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_Simple_1);
  }

  /* Priority */
  rc = usmDbVrrpOperPriorityGet(unit,vrID,intfcNum,&priority);
  osapiSnprintf(buffer, sizeof(buffer), "%u",priority);
  cliFormat( ewsContext, pStrInfo_common_Pri_1);
  ewsTelnetWrite( ewsContext, buffer);

  /* configured priority */
  rc = usmDbVrrpConfigPriorityGet(unit,vrID,intfcNum,&priority);
  osapiSnprintf(buffer, sizeof(buffer), "%u",priority);
  cliFormat( ewsContext, pStrInfo_common_ConfigPriority);
  ewsTelnetWrite( ewsContext, buffer);

  /* advertisement interval */
  rc = usmDbVrrpOperAdvertisementIntervalGet(unit,vrID,intfcNum,&advIntvl);
  osapiSnprintf(buffer, sizeof(buffer), "%u",advIntvl);
  cliFormat( ewsContext, pStrInfo_routing_AdvertisementIntvlSecs);
  ewsTelnetWrite( ewsContext, buffer);

  /* preempt mode */
  rc = usmDbVrrpOperPreemptModeGet(unit,vrID,intfcNum,&prmtFlg);
  cliFormat( ewsContext, pStrInfo_routing_PreEmptMode);
  ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(prmtFlg,pStrInfo_common_Dsbl_1));

  /* administrative mode */
  rc = usmDbVrrpOperVrrpModeGet(unit, vrID,intfcNum,&mode);
  cliFormat( ewsContext, pStrInfo_common_AdministrativeMode);
  if( mode == L7_VRRP_UP)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_Enbl_1);
  }
  else
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_Dsbl_1);
  }

  /* State */
  rc = usmDbVrrpOperStateGet(unit, vrID,intfcNum,&state);
  cliFormat( ewsContext, pStrInfo_common_State);
  if( state == L7_VRRP_STATE_INIT)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_routing_Initialized);
  }
  else if( state == L7_VRRP_STATE_MASTER)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_Master);
  }
  else if( state == L7_VRRP_STATE_BACKUP)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_Backup);
  }
  count +=8; /* for the above fields */
  firstTime = L7_FALSE;
 }

  if (count > CLI_MAX_SCROLL_LINES-6)
  {
    cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
    cliSyntaxBottom(ewsContext);

    osapiSnprintf(buf, sizeof(buf), argv[0]);

    for (val=1; val<argc; val++)
    {
      OSAPI_STRNCAT(buf, " ");
      OSAPI_STRNCAT(buf, argv[val]);
    }
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2;
  }


  /* for showing the tracking information for VRRP */
  if (printTrackIntf == L7_FALSE)
  {
    if (strcmp (showVrrpTrackIntf(ewsContext, argc, argv, vrID, intfcNum, &count), pStrInfo_common_Name_2) == 0)
    {
      return pStrInfo_common_Name_2;
    }
    else 
    {
      printTrackIntf = L7_TRUE;
    }
  }
  if (strcmp(showVrrpTrackRoute(ewsContext, argc, argv,vrID, intfcNum, &count), pStrInfo_common_Name_2) == 0)
     return pStrInfo_common_Name_2;

  printTrackIntf = L7_FALSE; 
  firstTime = L7_TRUE;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To show virtual routers summary
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
* @cmdsyntax show ip vrrp interface brief
*
* @cmdhelp show the summary report of all the configured virtual router
*
* @cmddescript  Shows the detailed information of all the configured
*               virtual router
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVrrpInterfaceBrief(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 * * argv,
                                              L7_uint32 index)
{

  L7_RC_t rc;
  L7_uchar8 vrId,nextVrId;
  L7_uint32 vrIdTemp;
  L7_vrrpState_t state;
  L7_uint32 infNum, nextInfNum;
  L7_vrrpStartState_t mode;
  L7_uint32 ipAddress;
  L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 ipAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unitIndex, slotIndex, port;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowVrrpIntfBrief);
  }

  memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
  ewsTelnetWriteAddBlanks (1, 0, 0, 4, L7_NULLPTR,  ewsContext, pStrInfo_routing_IntfVridIpAddrModeState);
  ewsTelnetWrite( ewsContext, "\r\n---------     ----     --------------     ------    ------------");

  if(usmDbVrrpOperFirstGet(unit,&vrId, &infNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  vrIdTemp = vrId;            /* use 32-bit temp value for comparison */
  while( vrIdTemp >= L7_VRID_RANGE_MIN && vrIdTemp <= L7_VRID_RANGE_MAX)
  {
    memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
    rc = usmDbUnitSlotPortGet(infNum, &unitIndex,&slotIndex, &port);
    rc = usmDbVrrpOperIpAddressGet(unit,vrId,infNum,&ipAddress);
    /*osapiSnprintf(buffer, sizeof(buffer), "\r\n%2d/%-2d         %-3d      ",slotIndex,port,vrId);*/
    ewsTelnetPrintf (ewsContext, "\r\n%-14.9s%-3d      ", cliGetIntfName(infNum, unitIndex,slotIndex,port), vrId);

    memset (ipAddr, 0,L7_CLI_MAX_STRING_LENGTH);
    rc = usmDbInetNtoa( ipAddress, ipAddr);
    ewsTelnetPrintf (ewsContext, "%-19s",ipAddr);
    rc = usmDbVrrpOperVrrpModeGet(unit, vrId,infNum,&mode);

    rc = usmDbVrrpOperStateGet(unit, vrId,infNum,&state);
    if( mode == L7_VRRP_UP )
    {
      ewsTelnetPrintf (ewsContext, "%-10s",pStrInfo_common_Enbl_1);
    }
    else
    {
      ewsTelnetPrintf (ewsContext, "%-10s",pStrInfo_common_Dsbl_1);
    }
    if( state == L7_VRRP_STATE_INIT)
    {
      ewsTelnetPrintf (ewsContext, "%-11s",pStrInfo_common_Initialize);
    }
    else if( state == L7_VRRP_STATE_MASTER)
    {
      ewsTelnetPrintf (ewsContext, "%-11s",pStrInfo_common_Master);
    }
    else if( state == L7_VRRP_STATE_BACKUP)
    {
      ewsTelnetPrintf (ewsContext, "%-11s",pStrInfo_common_Backup);
    }

    if (usmDbVrrpOperNextGet(unit, vrId,infNum,&nextVrId, &nextInfNum) == L7_SUCCESS)
    {
      vrIdTemp = vrId = nextVrId;
      infNum = nextInfNum;
    }
    else
    {
      vrIdTemp = vrId = 0;
    }

  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To show the detailed statistical information of interface
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
* @cmdsyntax show ip vrrp interface stats <slot/port> <vrId>
*
* @cmdhelp to show the stat information
*
* @cmddescript
*   command shows the statistical information of a virtual router
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowVrrpInterfaceStats(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_RC_t rc;
  L7_uchar8 vrID;
  L7_uint32 intfcNum, s, p;
  L7_uint32 uptime, days, hrs, mins, secs;
  L7_uint32 vrEnableTime=0;
  L7_vrrpProtocolType_t protocol;
  L7_vrrpState_t state;

  L7_uint32 masterState, addReceived;
  L7_uint32 addIntErrors, authFailedPkt,ttlFailedPkt, zeroPriorRcvPkt,zeroPriorSentPkt;
  L7_uint32 invalidTypePkt, addListerrPkt, invAuthTypePkt, authMismatchPkt,pktError;
  L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_ShowVrrpIntfStats_1, cliSyntaxInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index+1], &unit, &s, &p) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index + 1], &s,&p,&intfcNum) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliPrompt(ewsContext);
  }


  if( cliConvertRouterID(ewsContext,argv[index + 2],&vrID)
     != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Show uptime as 0 if router is in init state */
  if ((usmDbVrrpOperStateGet(unit, vrID, intfcNum, &state) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
  }
  if(state != L7_VRRP_STATE_INIT)
  {
    if(usmDbVrrpOperUpTimeGet(unit, vrID, intfcNum, &vrEnableTime) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
    }
    /* Figure out how long the virtual router has been up. */
    uptime = osapiUpTimeRaw() - vrEnableTime;
  }
  else
  {
    uptime = 0;
  }

  days = uptime/(3600*24);
  hrs = uptime/3600 - days*24;
  mins = uptime/60 - hrs*60 - days*24*60;
  secs = uptime % 60;
  osapiSnprintf(buffer, sizeof(buffer), pStrInfo_common_DaysHrsMinsSecs, days, hrs, mins, secs);
  cliFormat( ewsContext, pStrInfo_routing_Uptime);
  ewsTelnetWrite( ewsContext, buffer);

  /* Protocol */
  rc = usmDbVrrpOperProtocolGet(unit, vrID, intfcNum, &protocol);
  cliFormat( ewsContext, pStrInfo_common_Proto);
  if( protocol == L7_VRRP_IP)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_common_Ip);
  }
  else if( protocol == L7_VRRP_BRIDGE)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_routing_Bridge);
  }
  else if( protocol == L7_VRRP_DECNET)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_routing_Decnet);
  }
  else if( protocol == L7_VRRP_OTHER)
  {
    ewsTelnetWrite( ewsContext, pStrInfo_routing_Other2);
  }

  /* State Transitioned to Master */
  rc = usmDbVrrpStatsBecomeMaster(unit,vrID,intfcNum, &masterState);
  osapiSnprintf(buffer, sizeof(buffer), "%u",masterState);
  cliFormat( ewsContext, pStrInfo_routing_StateTransitionedToMaster);
  ewsTelnetWrite( ewsContext, buffer);

  /* Advertisement Received */
  rc = usmDbVrrpStatsAdvertiseRcvd(unit, vrID, intfcNum, &addReceived);
  osapiSnprintf(buffer, sizeof(buffer), "%u",addReceived);
  cliFormat( ewsContext, pStrInfo_routing_AdvertisementRcvd);
  ewsTelnetWrite( ewsContext, buffer);

  /* Advertisement Interval Errors */
  rc = usmDbVrrpStatsAdvertiseIntervalErrorsGet(unit, vrID, intfcNum, &addIntErrors);
  osapiSnprintf(buffer, sizeof(buffer), "%u",addIntErrors);
  cliFormat( ewsContext, pStrInfo_routing_AdvertisementIntvlErrs);
  ewsTelnetWrite( ewsContext, buffer);

  /* Authentication Failure */
  rc = usmDbVrrpStatsAuthFailuresGet(unit, vrID, intfcNum, &authFailedPkt);
  osapiSnprintf(buffer, sizeof(buffer), "%u",authFailedPkt);
  cliFormat( ewsContext, pStrErr_common_AuthFailure);
  ewsTelnetWrite( ewsContext, buffer);

  /* IP TTL Errors */
  rc = usmDbVrrpStatsIpTTLFailedGet(unit, vrID, intfcNum, &ttlFailedPkt);
  osapiSnprintf(buffer, sizeof(buffer), "%u",ttlFailedPkt);
  cliFormat( ewsContext, pStrInfo_routing_IpTtlErrs);
  ewsTelnetWrite( ewsContext, buffer);

  /* Zero Priority Packets Received */
  rc = usmDbVrrpStatsPriorityZeroPktsRcvdGet(unit, vrID, intfcNum, &zeroPriorRcvPkt);
  osapiSnprintf(buffer, sizeof(buffer), "%u",zeroPriorRcvPkt);
  cliFormat( ewsContext, pStrInfo_routing_ZeroPriPktsRcvd);
  ewsTelnetWrite( ewsContext, buffer);

  /* Zero Priority Packets Sent */
  rc = usmDbVrrpStatsPriorityZeroPktsSentGet(unit, vrID, intfcNum, &zeroPriorSentPkt);
  osapiSnprintf(buffer, sizeof(buffer), "%u",zeroPriorSentPkt);
  cliFormat( ewsContext, pStrInfo_routing_ZeroPriPktsSent);
  ewsTelnetWrite( ewsContext, buffer);

  /* Invalid Type Packets Received */
  rc = usmDbVrrpStatsInvalidTypePktsRcvdGet(unit, vrID, intfcNum,&invalidTypePkt);
  osapiSnprintf(buffer, sizeof(buffer), "%u",invalidTypePkt);
  cliFormat( ewsContext, pStrErr_routing_TypePktsRcvd);
  ewsTelnetWrite( ewsContext, buffer);

  /* Address List Errors */
  rc = usmDbVrrpStatsAddressListErrorsGet(unit, vrID, intfcNum, &addListerrPkt);
  osapiSnprintf(buffer, sizeof(buffer), "%u",addListerrPkt);
  cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR,  ewsContext, pStrInfo_routing_AddrListErrs);
  ewsTelnetWrite( ewsContext, buffer);

  /* Invalid Authentication Type */
  rc = usmDbVrrpStatsInvalidAuthTypeGet(unit, vrID, intfcNum, &invAuthTypePkt);
  osapiSnprintf(buffer, sizeof(buffer), "%u",invAuthTypePkt);
  cliFormat( ewsContext, pStrErr_routing_AuthType_2);
  ewsTelnetWrite( ewsContext, buffer);

  /* Auth Type Mismatch */
  rc = usmDbVrrpStatsAuthTypeMismatchGet(unit, vrID, intfcNum,&authMismatchPkt);
  osapiSnprintf(buffer, sizeof(buffer), "%u",authMismatchPkt);
  cliFormat( ewsContext, pStrInfo_routing_AuthTypeMismatch);
  ewsTelnetWrite( ewsContext, buffer);

  /* Packet Length Errors */
  rc = usmDbVrrpStatsPktLengthGet(unit, vrID, intfcNum, &pktError);
  cliFormat( ewsContext, pStrInfo_routing_PktLenErrs);
  return cliSyntaxReturnPrompt (ewsContext,"%u",pktError);

}
