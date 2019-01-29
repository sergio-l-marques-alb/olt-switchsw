/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_dhcps.c
 *
 * @purpose config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/03/2003
 *
 * @author  Kavleen Bhati
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include <errno.h>

/* For internet addr translation routines */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "l7_common.h"
#include "dhcps_exports.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "l7_relay_api.h"

#include "ews.h"

#include "clicommands_dhcps.h"

#include "usmdb_dhcps_api.h"

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
#include "usmdb_dhcp6s_api.h"
#endif
#endif

/*********************************************************************
*
* @purpose    Configures the duration of the lease
*               for an IP address that is assigned from a DHPCP server to a DHCP client
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: lease <days> [hours] [minutes] or lease <infinite>
*
* @cmdsyntax for no command: no lease
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandLease(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argDay      = 1;
  L7_uint32 argHour     = 2;
  L7_uint32 argMinute   = 3;
  L7_uint32 argInfinite = 1;
  L7_char8 strInfinite[9];
  L7_uint32 numArg;
  L7_uint32 intDay = 0;
  L7_uint32 intHour = 0;
  L7_uint32 intMinute = 0;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg > 3) || (numArg < 1)
        || ((strcmp(strInfinite, pStrInfo_common_DhcpsLeaseInfinite_1) == 0) && (numArg != 1)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLease);
    }
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg > 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoLease);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    OSAPI_STRNCPY_SAFE(strInfinite, argv[index+ argInfinite]);
    cliConvertToLowerCase(strInfinite);

    if (strcmp(strInfinite, pStrInfo_common_DhcpsLeaseInfinite_1) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Set lease time to 0xFFFFFFFF */
        if (usmDbDhcpsLeaseTimeSet(unit, 0xFFFFFFFF, EWSPOOLNAME(ewsContext)) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetLeaseTime);
        }
      }
    }
    else
    {
      /* Convert 'day' argument into an integer value */
      if ( cliConvertTo32BitUnsignedInteger(argv[index+argDay], &intDay) == L7_SUCCESS)
      {
        if (intDay > L7_DHCPS_LEASETIME_DAY_MAX || intDay < L7_DHCPS_LEASETIME_DAY_MIN)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidDays);
        }
        else
        {
          /* Since 1 day = 24*60 minutes */
          intDay = intDay * 1440;
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLease);
      }

      if (numArg > 1)
      {
        /* Convert 'hours' argument into an integer value */
        if ( cliConvertTo32BitUnsignedInteger(argv[index+argHour], &intHour) == L7_SUCCESS)
        {
          if (intHour > L7_DHCPS_LEASETIME_HRS_MAX || intHour < L7_DHCPS_LEASETIME_HRS_MIN)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidHours);
          }
          else
          {
            intHour = intHour * 60;
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLease);
        }
      }

      if (numArg > 2)
      {
        /* Convert 'minutes' argument into an integer value */
        if ( cliConvertTo32BitUnsignedInteger(argv[index+argMinute], &intMinute) == L7_SUCCESS)
        {
          if (intMinute < L7_DHCPS_LEASETIME_MINS_MIN-1 || intMinute > L7_DHCPS_LEASETIME_MINS_MAX)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidMinutes);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLease);
        }
      }

      intMinute = intDay + intHour + intMinute;
      if ((intMinute < L7_DHCPS_LEASETIME_MIN) || (intMinute > (L7_DHCPS_LEASETIME_MAX-1)))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidLeaseTime);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Set the lease time */
        if ( usmDbDhcpsLeaseTimeSet(unit, intMinute, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetLeaseTime);
        }
      }
    }
  }
  else if(  ewsContext->commType == CLI_NO_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDhcpsLeaseTimeReset(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantResetLeaseTime);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    specifies the number of packets a DHCP server
*                 sends to a pool address as part of a ping operation
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @Mode  Global Config
*
* @cmdsyntax for normal command:  ip dhcp ping packets  <0, 2-10>
*
* @cmdsyntax for no command: no ip dhcp ping packets
*
* @cmdhelp
*
* @cmddescript  The default value for number of ping packets is 2.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpPingPackets(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argPingPkts = 1;
  L7_uint32 intPingPkts;
  L7_uint32 numArg;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();

  if((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgIpDhcpPingPkts);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoIpDhcpPingPkts);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Convert the argument containing number of ping packets to an integer value */
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argPingPkts], &intPingPkts) == L7_SUCCESS)
    {
      if ((intPingPkts > L7_DHCPS_PINGPKTNO_MAX || intPingPkts < L7_DHCPS_PINGPKTNOSET_MIN) &&
          intPingPkts != L7_DHCPS_PINGPKTNO_MIN)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_InvalidNoOfPingPkts);
        cliSyntaxBottom(ewsContext);
      }
      else if (intPingPkts == L7_DHCPS_PINGPKTNO_MIN)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if(usmDbDhcpsNoOfPingPktReset(unit) != L7_SUCCESS)
          {

            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantResetNoOfIpPingPkts);
          }
        }
      }
      else
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbDhcpsNoOfPingPktSet(unit, intPingPkts) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,pStrErr_base_CantSetNoOfIpPingPkts);
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgIpDhcpPingPkts);
    }
  }
  /* In 'no' mode */
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Reset the number of ping packets to default */
      if (usmDbDhcpsNoOfPingPktSet(unit, FD_DHCPS_DEFAULT_PINGPKTNO) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                               pStrErr_base_CantResetNoOfIpPingPkts);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enables or disables the DHCP server and relay agent features on the router
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Global Config
*
* @cmdsyntax for normal command: service dhcp
*
* @cmdsyntax for no command: no service dhcp
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandServiceDhcp(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 numArg;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();

  if(numArg != 0)
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgServiceDhcp);
    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoServiceDhcp);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Set service dhcp */
    if ( usmDbDhcpsAdminModeSet(unit, mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetServiceDhcp);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the subnet number and mask for a DHCP address pool
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: network network-number [mask|prefix-length]
*
* @cmdsyntax for no command: no network
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandNetwork(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 argMask = 2;
  L7_uint32 IPaddr;
  L7_uint32 intMask;
  L7_uint32 intPrefixLength = 0xFFFFFFFF;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMask[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 2))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNw);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoNw);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    memset (strIPaddr, 0,sizeof(strIPaddr));
    if (strlen(argv[index+argIPaddr]) >= sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);
    /* Convert Ip address into 32 bit integer */
    if (usmDbInetAton(strIPaddr, &IPaddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argMask], &intMask) == L7_SUCCESS)
    {
      if ((intMask < 0) || (intMask > 32))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNw);
      }

      /* Convert prefix-length into mask (32 bit integer) */
      intMask= intPrefixLength << (32-intMask);
    }
    /* If cliConvertTo32BitUnsignedInteger returns error, check if the mask is Ip address */
    else
    {
      /* Initialize */
      memset (strMask, 0,sizeof(strMask));
      /* Check the string size */
      if (strlen(argv[index+argMask]) >= sizeof(strMask))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNw);
      }

      /* Copy the argument into strMask */
      OSAPI_STRNCPY_SAFE(strMask, argv[index + argMask]);

      /* Convert mask into an integer value */
      if (usmDbInetAton(strMask, &intMask) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNw);
      }
      if (usmDbNetmaskValidate(intMask) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSubnetMask);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set network */
      if ( usmDbDhcpsNetworkSet(unit, EWSPOOLNAME(ewsContext), IPaddr, intMask) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetNw);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the network */
      if ( usmDbDhcpsNetworkDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantResetNw);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Specifies the IP servers available to a DHCP client
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
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command:  dns-server <address1> [<address2>...<address8>]
*
* @cmdsyntax for no command: no dns-server
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDnsServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 intIPaddr[L7_DHCPS_DNS_SERVER_MAX];
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;
  L7_uint32 intCounter;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  unit = cliGetUnitId();
  numArg = cliNumFunctionArgsGet();

  if( ((numArg > 8) || (numArg < 1)) && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDnsSrvr);
  }

  /* If arguments are provided in 'no' mode*/
  if ((ewsContext->commType == CLI_NO_CMD) && (numArg > 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoDnsSrvr);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    for (intCounter = 0; intCounter < numArg; intCounter++)
    {
      if (strlen(argv[index+intCounter+1]) >= sizeof(strIPaddr))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }

      osapiStrncpySafe(strIPaddr, argv[index+intCounter+1], sizeof(strIPaddr));

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strIPaddr, &intIPaddr[intCounter]) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }

      /* IP address should be between 0.0.0.1 and 255.255.255.255 */
      if ((intIPaddr[intCounter] < 1) || (intIPaddr[intCounter] > 0xFFFFFFFF))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }
    }

    /* For arguments that are not specified in the command */
    for (intCounter = numArg; intCounter < L7_DHCPS_DNS_SERVER_MAX; intCounter++)
    {
      intIPaddr[intCounter] = 0;
    }

    /* Set the DNS Servers*/
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbDhcpsDnsServersSet(unit, EWSPOOLNAME(ewsContext), intIPaddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDhcp6DnsSrvr);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /* Reset the DNS Servers*/

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDhcpsDnsServersReset(unit,EWSPOOLNAME(ewsContext)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantResetDnsSrvr);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Specifies the default router list for a DHCP client
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
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command:  default-router <address1> [<address2>...<address8>]
*
* @cmdsyntax for no command: no default-router
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDefaultRouter(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 intIPaddr[L7_DHCPS_DEFAULT_ROUTER_MAX];
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;
  L7_uint32 intCounter;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  unit = cliGetUnitId();
  numArg = cliNumFunctionArgsGet();

  if( ((numArg > 8) || (numArg < 1)) && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDeflRtr);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg > 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoDeflRtr);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Till number of arguments specified */
    for (intCounter = 0; intCounter < numArg; intCounter++)
    {
      if (strlen(argv[index+intCounter+1]) >= sizeof(strIPaddr))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }

      osapiStrncpySafe(strIPaddr, argv[index+intCounter+1], sizeof(strIPaddr));

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strIPaddr, &intIPaddr[intCounter]) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }

      /* IP address should be between 0.0.0.1 and 255.255.255.255 */
      if ((intIPaddr[intCounter] < 1) || (intIPaddr[intCounter] > 0xFFFFFFFF))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }
    }

    /* For arguments that are not specified in the command */
    for (intCounter = numArg; intCounter < L7_DHCPS_DEFAULT_ROUTER_MAX; intCounter++)
    {
      intIPaddr[intCounter] = 0;
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbDhcpsRoutersSet(unit, EWSPOOLNAME(ewsContext), intIPaddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetDeflRtr);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Reset the Default routers*/
      if (usmDbDhcpsDefaultRoutersIpReset(unit,EWSPOOLNAME(ewsContext)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetDeflRtr);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the subnet number and mask for a manual binding of a DHCP client
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: host <address> [mask | prefix-length]
*
* @cmdsyntax for no command: no host
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandHost(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 argMask = 2;
  L7_uint32 intIPaddr;
  L7_uint32 intMask;
  L7_uint32 intPrefixLength = 0xFFFFFFFF;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMask[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && ((numArg < 1) || (numArg > 2)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgHost);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoHost);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    memset (strIPaddr, 0,sizeof(strIPaddr));
    if (strlen(argv[index+argIPaddr]) >= sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);
    /* Convert Ip address into 32 bit integer */
    if (usmDbInetAton(strIPaddr, &intIPaddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    if (numArg == 1)
    {
      if (((intIPaddr & L7_CLASS_D_ADDR_NETWORK) != L7_CLASS_D_ADDR_NETWORK) &&
          ((intIPaddr & L7_CLASS_E_ADDR_NETWORK) != L7_CLASS_E_ADDR_NETWORK))
      {
        if ((intIPaddr & L7_CLASS_C_ADDR_NETWORK) == L7_CLASS_C_ADDR_NETWORK)
        {
          intMask = 0xFFFFFF00;  /* Mask for class C */
        }
        else if ((intIPaddr & L7_CLASS_B_ADDR_NETWORK) == L7_CLASS_B_ADDR_NETWORK)
        {
          intMask = 0xFFFF0000; /* Mask for class B */
        }
        else
        {
          intMask = 0xFF000000; /* Mask for class A */
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }
    }
    else if (cliConvertTo32BitUnsignedInteger(argv[index+argMask], &intMask) == L7_SUCCESS)
    {
      if ((intMask < 0) || (intMask > 32))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgHost);
      }

      /* Convert prefix-length into mask (32 bit integer) */
      intMask= intPrefixLength << (32-intMask);
    }
    /* If cliConvertTo32BitUnsignedInteger returns error, check if the mask is Ip address */
    else
    {
      /* Initialize */
      memset (strMask, 0,sizeof(strMask));
      /* Check the string size */
      if (strlen(argv[index+argMask]) >= sizeof(strMask))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgHost);
      }

      /* Copy the argument into strMask */
      OSAPI_STRNCPY_SAFE(strMask, argv[index + argMask]);

      /* Convert mask into an integer value */
      if (usmDbInetAton(strMask, &intMask) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgHost);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set host */
      if (usmDbDhcpsHostSet(unit, EWSPOOLNAME(ewsContext), intIPaddr, intMask) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetHost);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the host */
      if ( usmDbDhcpsHostDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelHost);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the hardware address of a DHCP client
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: hardware-address <hardware-address> [type]
*
* @cmdsyntax for no command: no hardware-address
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandHardwareAddress(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMacAddr = 1;
  L7_uint32 argType = 2;
  L7_uint32 intType;
  L7_char8 strMACaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && ((numArg < 1) || (numArg > 2)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgHardwareAddr);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoHardwareAddr);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argMacAddr]) >= sizeof(buf) )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidMacAddr);
    }
    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMACaddr, 0, sizeof(strMACaddr) );
    if ( cliConvertMac(buf, strMACaddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
    }

    if (numArg == 2)
    {
      /* Convert the argument containing type to an integer value */
      if ( cliConvertTo32BitUnsignedInteger(argv[index+argType], &intType) == L7_SUCCESS)
      {
        if (intType != L7_DHCPS_ETHERNET && intType != L7_DHCPS_IEEE_802)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_InvalidProto);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgHardwareAddr);
      }
    }
    else
    {
      intType = L7_DHCPS_ETHERNET;
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set hardware-address */
      if ( usmDbDhcpsHwAddressSet(unit, EWSPOOLNAME(ewsContext), strMACaddr, intType) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetHardwareAddr);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the hardware-address */
      if ( usmDbDhcpsHwAddressDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelHardwareAddr);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the client name of a DHCP client
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: client-name <name>
*
* @cmdsyntax for no command: no client-name
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandClientName(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argName = 1;
  L7_char8 strName[L7_DHCPS_HOST_NAME_MAXLEN + 1];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgClientName);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoClientName);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argName]) > L7_DHCPS_CLIENT_NAME_MAXLEN )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidClientName);
    }
    OSAPI_STRNCPY_SAFE(strName, argv[index + argName]);

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set client-name */
      if ( usmDbDhcpsClientNameSet(unit, EWSPOOLNAME(ewsContext), strName) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetClientName);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the client-name */
      if (usmDbDhcpsClientNameDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelClientName);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the IP addresses that a DHCP server should not assign to DHCP clients
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Global Config
*
* @cmdsyntax for normal command: ip dhcp excluded-address <low-address> [high-address]
*
* @cmdsyntax for no command: no ip dhcp excluded-address <low-address> [high-address]
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpExcludedAddress(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argLowAddress = 1;
  L7_char8 strLowAddress[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intLowAddress;
  L7_uint32 argHighAddress = 2;
  L7_char8 strHighAddress[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intHighAddress;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* Get the number of agruments */
  numArg = cliNumFunctionArgsGet();

  /* If number of arguments is neither 1 nor 2 */
  if (numArg < 1 || numArg > 2)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgIpDhcpExcldAddr);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgIpDhcpExcldAddr);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Get the unit Id */
  unit = cliGetUnitId();

  memset (strLowAddress, 0,sizeof(strLowAddress));
  if (strlen(argv[index+argLowAddress]) >= sizeof(strLowAddress))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }

  OSAPI_STRNCPY_SAFE(strLowAddress, argv[index + argLowAddress]);

  /* Convert Ip address into 32 bit integer */
  if (usmDbInetAton(strLowAddress, &intLowAddress) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }

  /* If high ip address is specified */
  if (numArg == 2)
  {
    memset (strHighAddress, 0,sizeof(strHighAddress));
    if (strlen(argv[index+argHighAddress]) >= sizeof(strHighAddress))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    OSAPI_STRNCPY_SAFE(strHighAddress, argv[index + argHighAddress]);

    /* Convert Ip address into 32 bit integer */
    if (usmDbInetAton(strHighAddress, &intHighAddress) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }
  }
  else
  {
    /* If high ip address is not specified */
    intHighAddress = intLowAddress;
  }

  /* If low ip address is lesser than high ip address */
  if (intLowAddress > intHighAddress)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LowGreaterThanHigh);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set ip dhcp excluded-address */
      if ( usmDbDhcpsExcludedAddressRangeAdd(unit, intLowAddress, intHighAddress) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetExcldAddr);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete ip dhcp excluded-address */
      if ( usmDbDhcpsExcludedAddressRangeDelete(unit, intLowAddress, intHighAddress) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelExcldAddr);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the client identifier of a DHCP client
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: client-identifier <unique-identifier>
*
* @cmdsyntax for no command: no client-identifier
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandClientIdentifier(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argClientId = 1;
  L7_char8 strClientId[L7_DHCPS_CLIENT_ID_MAXLEN+1];
  L7_char8 buf[(L7_DHCPS_CLIENT_ID_MAXLEN*3)]; /* client id is accepted in the xx:xx:.. format */
  L7_uint32 intClientIdLen;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgClientId);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoClientId);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argClientId]) >= sizeof(buf) )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidClientId);
    }
    OSAPI_STRNCPY_SAFE(buf, argv[index + argClientId]);

    memset ( strClientId, 0, sizeof(strClientId) );
    intClientIdLen = L7_DHCPS_CLIENT_ID_MAXLEN;
    if ( cliConvertClientId(buf, strClientId, &intClientIdLen) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidClientId);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set client-identifier */
      if ( usmDbDhcpsClientIdSet(unit, EWSPOOLNAME(ewsContext), strClientId, intClientIdLen) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetClientId);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the client-identifier */
      if ( usmDbDhcpsClientIdDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelClientId);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Deletes an automatic address binding from the DHCP server database
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Privileged EXEC
*
* @cmdsyntax for normal command: clear ip dhcp binding <address|*>
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearIpDhcpBinding(EwsContext ewsContext, L7_uint32 argc,const L7_char8 *
                                          * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 intIPaddr;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ClrBindings);
  }

  unit = cliGetUnitId();

  memset (strIPaddr, 0,sizeof(strIPaddr));
  /* If argument is * */
  if (strcmp(argv[index+argIPaddr], pStrInfo_base_DhcpsShowAll) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Clear all bindings */
      if (usmDbDhcpsBindingClearAll(unit, L7_DHCPS_ACTIVE_LEASE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantClrDhcpBinding);
      }
    }
  }
  else
  {
    if (strlen(argv[index+argIPaddr]) >= sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);
    /* Convert Ip address into 32 bit integer */
    if (usmDbInetAton(strIPaddr, &intIPaddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Clear bindings to a specific address */
      if (usmDbDhcpsBindingClear (unit, intIPaddr, L7_DHCPS_ACTIVE_LEASE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantClrDhcpBinding);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Resets all DHCP Server counters
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Privileged EXEC
*
* @cmdsyntax for normal command: clear ip dhcp server statistics
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearIpDhcpServerStatistics(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ClrSrvrStats);
  }

  unit = cliGetUnitId();

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Clear server statistics */
    if (usmDbDhcpsStatisticsClear(unit) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantClrSrvrStats);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures whether the server keeps track of conflicting (abandoned) addresses
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Global Config
*
* @cmdsyntax for normal command: ip dhcp conflict logging
*
* @cmdsyntax for no command: no ip dhcp conflict logging
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpConflictLogging(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 logging = L7_ENABLE;
  L7_uint32 numArg;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();

  if(numArg != 0)
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgConflictLogging);
    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoConflictLogging);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    logging = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    logging = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Set conflict logging status */
    if ( usmDbDhcpsConflictLoggingSet(unit, logging) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetConflictLogging);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Clears an address conflict from the DHCP Server database
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Privileged EXEC
*
* @cmdsyntax for normal command: clear ip dhcp conflict <address|*>
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearIpDhcpConflict(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 intIPaddr;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ClrConflict_1);
  }

  unit = cliGetUnitId();

  memset (strIPaddr, 0,sizeof(strIPaddr));
  /* If argument is * */
  if (strcmp(argv[index+argIPaddr], pStrInfo_base_DhcpsShowAll) == 0)
  {
    /* Clear all conflicts */
    if (usmDbDhcpsClearAllConflict(unit) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantClrConflict);
    }
  }
  else
  {
    if (strlen(argv[index+argIPaddr]) >= sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);
    /* Convert Ip address into 32 bit integer */
    if (usmDbInetAton(strIPaddr, &intIPaddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Clear conflict for a specific address */
      if (usmDbDhcpsClearConflict (unit, intIPaddr) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantClrConflict);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures NetBIOS Windows Internet Naming Service (WINS)
*           name servers that are available to DHCP clients
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
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command:  netbios-name-server <address1> [<address2>...<address8>]
*
* @cmdsyntax for no command: no netbios-name-server
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandNetbiosNameServer(EwsContext ewsContext, L7_uint32 argc,

                                         const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 intIPaddr[L7_DHCPS_NETBIOS_NAME_SERVER_MAX];
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;
  L7_uint32 intCounter;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  numArg = cliNumFunctionArgsGet();

  if( ((numArg > 8) || (numArg < 1)) && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNetbiosNameSrvr);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoNetBiosNameSrvr);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Till number of arguments specified */
    for (intCounter = 0; intCounter < numArg; intCounter++)
    {
      osapiStrncpySafe(strIPaddr, argv[index+intCounter+1], sizeof(strIPaddr));

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strIPaddr, &intIPaddr[intCounter]) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }

      /* IP address should be between 0.0.0.1 and 255.255.255.255 */
      if ((intIPaddr[intCounter] < 1) || (intIPaddr[intCounter] > 0xFFFFFFFF))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }
    }

    /* For arguments that are not specified in the command */
    for (intCounter = numArg; intCounter < L7_DHCPS_NETBIOS_NAME_SERVER_MAX; intCounter++)
    {
      intIPaddr[intCounter] = 0;
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbDhcpsNetbiosNameServerAddressSet(unit, EWSPOOLNAME(ewsContext), intIPaddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetNetbiosNameSrvr);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the NetBIOS name servers*/
      if (usmDbDhcpsNetbiosNameServerAddressDelete(unit,EWSPOOLNAME(ewsContext)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetNetbiosNameSrvr);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the NetBIOS node type for DHCP clients
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: netbios-node-type <type>
*
* @cmdsyntax for no command: no netbios-node-type
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandNetbiosNodeType(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argType = 1;
  L7_char8 strType[8];
  L7_uint32 nodeType;
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNetbiosNodeType);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoNetBiosNodeType);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argType]) >= sizeof(strType) )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidNetbiosNodeType);
    }
    OSAPI_STRNCPY_SAFE(strType, argv[index + argType]);

    if (strcmp(strType, pStrInfo_base_BNode) == 0)
    {
      nodeType = L7_DHCPS_B_NODE;
    }
    else if (strcmp(strType, pStrInfo_base_PNode) == 0)
    {
      nodeType = L7_DHCPS_P_NODE;
    }
    else if (strcmp(strType, pStrInfo_base_MNode) == 0)
    {
      nodeType = L7_DHCPS_M_NODE;
    }
    else if (strcmp(strType, pStrInfo_base_HNode) == 0)
    {
      nodeType = L7_DHCPS_H_NODE;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidNetbiosNodeType);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set netBIOS node type */
      if ( usmDbDhcpsNetbiosNodeTypeSet(unit, EWSPOOLNAME(ewsContext), nodeType) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetNetbiosNodeType);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete netBIOS node type */
      if (usmDbDhcpsNetbiosNodeTypeDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelBiosNodeType);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Specifies the domain name for a DHCP client
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: domain-name <domain>
*
* @cmdsyntax for no command: no domain-name
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandDomainName(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argName = 1;
  L7_char8 strName[L7_DHCPS_DOMAIN_NAME_MAXLEN + 1];
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDoMainName);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoDoMainName);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argName]) >= sizeof(strName) )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidDoMainName);
    }

    OSAPI_STRNCPY_SAFE(strName, argv[index + argName]);

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set domain-name */
      if ( usmDbDhcpsDomainNameSet(unit, EWSPOOLNAME(ewsContext), strName) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetDoMainName);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the domain-name */
      if (usmDbDhcpsDomainNameDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelDomainName);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the DHCP Server parameters and address pools
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: bootfile <filename>
*
* @cmdsyntax for no command: no bootfile
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandBootfile(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argFileName = 1;
  L7_char8 strFileName[L7_DHCPS_BOOT_FILE_NAME_MAXLEN + 1];
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgBootFile);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoBootFile);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argFileName]) >= sizeof(strFileName) )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidFileName);
    }
    OSAPI_STRNCPY_SAFE(strFileName, argv[index + argFileName]);

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set bootfile */
      if ( usmDbDhcpsBootfileSet(unit, EWSPOOLNAME(ewsContext), strFileName) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetBootFile);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the bootfile */
      if (usmDbDhcpsBootfileDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelBootFile);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enables the allocation of the addresses to bootp client from automatic address pool
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Global Config
*
* @cmdsyntax for normal command: ip dhcp bootp automatic
*
* @cmdsyntax for no command: no ip dhcp bootp automatic
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpBootpAutomatic(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 allocation = L7_DISABLE;
  L7_uint32 numArg;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();

  if(numArg != 0)
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgBootpAuto);
    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoBootpAuto);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    allocation = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    allocation = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Set bootp allocation status */
    if ( usmDbDhcpsBootpAutomaticSet(unit, allocation) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetBootpAuto);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the next server in the boot process of a DHCP client
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: next-server <ip-address>
*
* @cmdsyntax for no command: no next-server <ip-address>
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandNextServer(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 IPaddr;
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  if ((numArg != 1) && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNextSrvr);
  }
  else if ((numArg != 0) && (ewsContext->commType == CLI_NO_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoNextSrvr);
  }

  unit = cliGetUnitId();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    memset (strIPaddr, 0,sizeof(strIPaddr));
    if (strlen(argv[index+argIPaddr]) >= sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);

    /* Convert Ip address into 32 bit integer */
    if (usmDbInetAton(strIPaddr, &IPaddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set next-server */
      if ( usmDbDhcpsNextServerSet(unit, EWSPOOLNAME(ewsContext), IPaddr) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetNextSrvr);
      }
    }
  }
  /* If no command is to be executed */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete the next-server */
      if ( usmDbDhcpsNextServerDelete(unit, EWSPOOLNAME(ewsContext)) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelNextSrvr);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the DHCP Server options
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  DHCP Pool Config
*
* @cmdsyntax for normal command: option <code> {ascii <string> | hex <string1> [<string2>...<string8> ] |
*                                               ip <address1> [<address2>...<address8> ]}
*
* @cmdsyntax for no command: no option <code>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandOption(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 argCode = 1;
  L7_uint32 argDataFormat = 2;
  L7_uint32 argAscii = 3;
  L7_uint32 intDataFormat;
  L7_uint32 intIpAddr;
  L7_uint32 intDataLen = 0;
  L7_uint32 binaryLength = 0;
  L7_uint32 counter;
  L7_uint32 intCode;
  L7_uint32 asciiLen = 0;
  L7_uchar8 chrDataFormat;
  L7_uchar8 chrStatus;
  L7_uchar8 asciiData[2*L7_DHCPS_POOL_OPTION_LENGTH+1];
  L7_char8 strDataFormat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 binaryString[2*L7_DHCPS_POOL_OPTION_LENGTH+1];
  L7_uchar8 strHexadecimal[2*L7_DHCPS_POOL_OPTION_LENGTH+1];
  L7_uchar8 ipString[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpString[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 optionData[L7_DHCPS_POOL_OPTION_LENGTH+1];
  L7_BOOL alreadyExists = L7_FALSE;
  L7_uchar8 prevOptionData[L7_DHCPS_POOL_OPTION_LENGTH+1];
  L7_uint32 intPrevDataLen = 0;
  L7_uchar8 chrPrevDataFormat = 0;
  L7_uint32 unit, numArg;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* Get the number of arguments */
  numArg = cliNumFunctionArgsGet();

  /* Get the unit */
  unit = cliGetUnitId();

  /* get code */
  intCode = atoi(argv[index+argCode]);

  /* If arguments are less than 1 or more than 10 */
  if (((((numArg < 3) || (numArg > 10)) && (intCode != CLI_OPTION68))
       || ((intCode == CLI_OPTION68) && (numArg < 2))) && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgOption);
  }
  else if ((numArg != 1) && (ewsContext->commType == CLI_NO_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoOption);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* If code specified is between 1 and 255 */
    if ((intCode < L7_DHCPS_OPTION_CONFIGURE_MIN) || (intCode > L7_DHCPS_OPTION_CONFIGURE_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_base_InvalidCode);
    }

    OSAPI_STRNCPY_SAFE(strDataFormat, argv[index+argDataFormat]);

    /* Get the data for the specified code */
    memset (optionData, 0, L7_DHCPS_POOL_OPTION_LENGTH);
    if(usmDbDhcpsOptionGet(unit, EWSPOOLNAME(ewsContext), (L7_uchar8)intCode,
                           optionData, &intDataLen, &chrDataFormat, &chrStatus ) == L7_SUCCESS)
    {
      /* If the data type stored for the code does not match the code specified in the command */
      if (((chrDataFormat == L7_DHCPS_ASCII) && (strcmp(strDataFormat, pStrInfo_common_AsCiiOption) != 0))
          || ((chrDataFormat == L7_DHCPS_HEX) && (strcmp(strDataFormat, pStrInfo_common_HexOption) != 0))
          || ((chrDataFormat == L7_DHCPS_IP) && (strcmp(strDataFormat, pStrInfo_common_IpOption) != 0)))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,pStrErr_base_InvalidDataFmt);
      }

      OSAPI_STRNCPY_SAFE(prevOptionData, optionData);
      intPrevDataLen = intDataLen;
      chrPrevDataFormat = chrDataFormat;
      alreadyExists = L7_TRUE;
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Create row for option */
      rc = usmDbDhcpsOptionCreate (unit, EWSPOOLNAME(ewsContext), (L7_uchar8)intCode);

      if ( rc == L7_FAILURE )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantCreateOptionRow);
      }
    }

    /* If data type is "ascii" */
    if (strcmp(strDataFormat, pStrInfo_common_AsCiiOption) == 0)
    {
      /* no ascii value is specified or more than one value is specified */
      if ((numArg - 2) > 1)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgOption);
      }
      else
      {
        OSAPI_STRNCPY_SAFE(asciiData, argv[index + argAscii]);
        asciiLen = strlen(asciiData);

        /* Concatenate all the strings */
        if((intDataLen + asciiLen) <= L7_DHCPS_POOL_OPTION_LENGTH)
        {
          memcpy(&optionData[intDataLen], asciiData, asciiLen);
          intDataLen += asciiLen;
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_ExceedsMaxDataLen);
        }

        /* Set the data type to ascii */
        intDataFormat = L7_DHCPS_ASCII;
      }
    }
    /* If data type is "hex" */
    else if (strcmp(strDataFormat, pStrInfo_common_HexOption) == 0)
    {
      /* If more than 8 strings are specified in the command line for hex */
      if ((numArg - 2) > 8)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgOption);
      }
      else
      {
        /* For number of strings specified */
        for (counter = 3; counter <= numArg; counter++ )
        {
          memset (binaryString, 0, sizeof(binaryString));
          /* Convert the string to binary data */
          OSAPI_STRNCPY_SAFE(strHexadecimal, argv[index + counter]);

          if (usmDbHexToBin(strHexadecimal, binaryString, &binaryLength) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidHexFmt);
          }
          /* Concatenate all the binary strings */
          if((binaryLength + intDataLen) <= L7_DHCPS_POOL_OPTION_LENGTH)
          {
            memcpy(&optionData[intDataLen], binaryString, binaryLength);
            intDataLen += binaryLength;
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_ExceedsMaxDataLen);
          }
        }

        /* Set the data type to hexadecimal */
        intDataFormat = L7_DHCPS_HEX;
      }
    }
    /* If data type is "ip" */
    else if (strcmp(strDataFormat, pStrInfo_common_IpOption) == 0)
    {
      /* If more than 8 strings are specified */
      if ((numArg - 2) > 8)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgOption);
      }
      else
      {
        /* For number of arguments */
        for (counter = 3; counter <= numArg; counter++ )
        {
          memset (ipString, 0, L7_CLI_MAX_STRING_LENGTH);
          OSAPI_STRNCPY_SAFE(strIpString, argv[index + counter]);

          /* Convert IP address to 32 bit integer */
          if (usmDbInetAton(strIpString, &intIpAddr) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
          }

          /* Concatenate all the binary strings */
          if((intDataLen + 4) <= L7_DHCPS_POOL_OPTION_LENGTH)
          {
            memcpy(&optionData[intDataLen], &intIpAddr, 4);
            intDataLen += 4;
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_ExceedsMaxDataLen);
          }

        }
        /* Set the data type to ip */
        intDataFormat = L7_DHCPS_IP;
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgOption);
    }

    if (intDataLen >  L7_DHCPS_POOL_OPTION_LENGTH)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_ExceedsMaxDataLen);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set option */
      if ( usmDbDhcpsOptionSet(unit, EWSPOOLNAME(ewsContext), (L7_uchar8)intCode,
                               optionData, intDataLen, (L7_uchar8)intDataFormat) != L7_SUCCESS )
      {
        /* Delete if exist */
        if (rc == L7_ALREADY_CONFIGURED)
        {
          if (alreadyExists == L7_FALSE)
          {
            usmDbDhcpsOptionDelete(unit, EWSPOOLNAME(ewsContext), (L7_uchar8)intCode);
          }
          else
          {
            usmDbDhcpsOptionSet(unit, EWSPOOLNAME(ewsContext), (L7_uchar8)intCode,
                                prevOptionData, intPrevDataLen, (L7_uchar8)chrPrevDataFormat);
          }
        }
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantSetOption);
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Delete option*/
      if (usmDbDhcpsOptionDelete(unit, EWSPOOLNAME(ewsContext), (L7_uchar8)intCode) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelOption);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  used to create a char array for client identifier
*
* @param    buf         contains ascii string "xx:xx:xx:xx:xx:xx.....:xx:xx"
* @param    clientIdLength   provides the length of the client identifier
* @param    clientId         return hex data
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL cliConvertClientId(L7_uchar8 * buf, L7_uchar8 * clientId, L7_uint32 * clientIdBytes)
{
  L7_uint32 i,j, digit_count=0;
  L7_uint32 clientLen;
  L7_uchar8 client_identifier[(L7_DHCPS_CLIENT_ID_MAXLEN*2)+1];

  clientLen = strlen(buf);

  if ((clientLen % 3) != 2)
  {
    return L7_FALSE;
  }

  if(((clientLen+1)/3) > *clientIdBytes)
  {
    return L7_FALSE;
  }

  for ( i=0,j=0; i<clientLen; i++,j++ )
  {
    digit_count++;
    switch ( buf[i] )
    {
    case '0':
      client_identifier[j]=0x0;
      break;
    case '1':
      client_identifier[j]=0x1;
      break;
    case '2':
      client_identifier[j]=0x2;
      break;
    case '3':
      client_identifier[j]=0x3;
      break;
    case '4':
      client_identifier[j]=0x4;
      break;
    case '5':
      client_identifier[j]=0x5;
      break;
    case '6':
      client_identifier[j]=0x6;
      break;
    case '7':
      client_identifier[j]=0x7;
      break;
    case '8':
      client_identifier[j]=0x8;
      break;
    case '9':
      client_identifier[j]=0x9;
      break;
    case 'a':
    case 'A':
      client_identifier[j]=0xA;
      break;
    case 'b':
    case 'B':
      client_identifier[j]=0xB;
      break;
    case 'c':
    case 'C':
      client_identifier[j]=0xC;
      break;
    case 'd':
    case 'D':
      client_identifier[j]=0xD;
      break;
    case 'e':
    case 'E':
      client_identifier[j]=0xE;
      break;
    case 'f':
    case 'F':
      client_identifier[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FALSE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FALSE;
      /*PASSTHRU*/
    }
  }

  *clientIdBytes = (clientLen+1)/3;

  for ( i = 0; i < (*clientIdBytes); i++ )
  {
    clientId[i] = ( (client_identifier[(i*2)] << 4) + client_identifier[(i*2)+1] );
  }
  return L7_TRUE;
}
