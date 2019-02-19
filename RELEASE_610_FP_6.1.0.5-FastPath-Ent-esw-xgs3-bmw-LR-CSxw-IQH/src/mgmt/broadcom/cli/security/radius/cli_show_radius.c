/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/radius/cli_show_radius.c
 *
 * @purpose Radius show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  4/8/2003
 *
 * @author  jflanagan
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "l7_common.h"
#include "radius_exports.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_radius_api.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_radius.h"

#include "osapi_support.h"

/*********************************************************************
*
* @purpose  display radius accounting summary information
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
* @notes add range checking
*
* @cmdsyntax  show radius accounting [ { name | statistics <ipaddr|hostname>  }]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowRadiusAccounting(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index)
{
  L7_uint32 numArgs;  
  static L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  static L7_char8 host[L7_DNS_HOST_NAME_LEN_MAX];
  static L7_BOOL firstDisplay = L7_TRUE;  
  static L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 unit;
  L7_uchar8 *hostAddrTraverse = L7_NULLPTR;
  L7_BOOL firstRow = L7_TRUE;
  L7_char8 rowString[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val;
  L7_BOOL boolVal;
  
  L7_uint32 argName = 1, argNameVal = 2;
  L7_uint32 argStatsName = 2, argStatsNameVal =3;  
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];  
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 hostLen = L7_NULL;  
  L7_char8 splitLabel[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 lineCount = 0 ,  cmdIndex = 0;
  
  L7_uint32 argHost = 1,argStats = 1, argStatsHost = 2;
  L7_BOOL verified;
  L7_double64 doubleStat;
  L7_uint32 ipAddr;

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);    
  numArgs = cliNumFunctionArgsGet();

  if ( numArgs <  1 ||  numArgs > 3 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_ShowRadiusAcctSummary2);
  }
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  { /* if our question has been answered */
     if (L7_TRUE == cliIsPromptRespQuit())
     {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
     }
  }
  else
  {
    firstDisplay = L7_TRUE;
    addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
    memset (host, 0, sizeof(host));
    memset (name, 0, sizeof(name));
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_ErrCouldNot, ewsContext, 
                     pStrErr_common_UnitId_1);
  }
  /*show radius accounting name ..*/
  if(strcmp(argv[index+argName],pStrInfo_secuirty_serverName) == 0 )
  {
    if (numArgs == argName) /* show radius accounting name */
    {
      if (firstDisplay == L7_TRUE)
      {
        /* For first time display, get the First entry */
        if(usmDbRadiusNamedAcctServerOrderlyGetNext(unit,name,name,host,&addrType)
                      != L7_SUCCESS )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  
                     ewsContext,  pStrErr_security_NoRadiusAcctSrvrsCfgured);
        }


        /* Print headings */

        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                     pStrInfo_security_SrvrNameHostAddPortSecretConfig);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                     pStrInfo_security_SrvrNameHostAddPortSecretConfig1);
        ewsTelnetWrite(ewsContext,          
          "\r\n-------------------------------- ------------------------ ------ ----------");
      }

      do
      {
        hostAddrTraverse = host;
        if(strlen(host) == 0 )
        {
          osapiStrncpySafe(host,pStrErr_common_NotAvailable,
                     strlen(pStrErr_common_NotAvailable)+1);
        }

        while (*hostAddrTraverse  != L7_EOS)
        {
          if (firstRow == L7_TRUE)
          {
            /* Displaying First Row after concatenating all column entries */
            memset (rowString, 0, sizeof(rowString));

            /* Server Name */
            memset (buf, 0, sizeof(buf));
            sprintf( buf, "\r\n%-32s", name);
            OSAPI_STRNCAT(rowString, buf);
             
            /* Host Address */
            memset (buf, 0, sizeof(buf));
            cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
            osapiSnprintf(buf, sizeof(buf), " %-24s", splitLabel);               

            OSAPI_STRNCAT(rowString, buf);

            /* Port */
            memset (buf, 0, sizeof(buf));
            if (usmDbRadiusAccountingHostNameServerPortNumGet(unit, host, 
                                                addrType, &val) != L7_SUCCESS)
            {
              sprintf( buf, " %-5s",  pStrInfo_common_NotApplicable);
            }                                    /* "N\A" */
            else
            {
              sprintf( buf, " %-5u", val);
            }
            OSAPI_STRNCAT(rowString, buf);

            /* Secret Configured */
            memset ( buf, 0, sizeof(buf));
            if ( usmDbRadiusAccountingHostNameServerSharedSecretConfigured( 
                     unit, host, addrType, &boolVal ) != L7_SUCCESS )
            {
              sprintf( buf, "  %-10s ",  pStrInfo_common_NotApplicable);
            }                                     /* "N\A" */
            else
            {
              if (boolVal == L7_TRUE)
              {
                sprintf( buf, "  %-10s ",  pStrInfo_common_Yes);
              }                                       /* "Yes"*/
              else
              {
                sprintf( buf, "  %-10s ",  pStrInfo_common_No);
              }                                      /* "No"*/
            }
            OSAPI_STRNCAT(rowString, buf);

            ewsTelnetWrite( ewsContext, rowString);
            firstRow = L7_FALSE;
            lineCount++;
          } /* end of if (firstRow == L7_TRUE)*/
          else
          {
            /* Displaying remaining hostname in second Row */
            memset (rowString, 0, sizeof(rowString));
            memset (splitLabel, 0, sizeof(splitLabel));
            memset (buf, 0, sizeof(buf));
            memset(buf,' ',33);
            osapiSnprintf(rowString, sizeof(rowString), "\r\n%33s", buf); 
            /* Get the split hostname which fits in available space 
             * (24 characters) in a row 
             */
            cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
            OSAPI_STRNCAT(rowString, splitLabel);
            ewsTelnetWrite( ewsContext, rowString);
            lineCount++;
          }
        } /* end of while */
        firstRow = L7_TRUE;
        memset (host, 0, sizeof(host));
        rc = usmDbRadiusNamedAcctServerOrderlyGetNext(unit, name, name, host, &addrType);
        if (rc != L7_SUCCESS)
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
        hostLen = strlen(host); 
        if (lineCount >= (CLI_MAX_SCROLL_LINES-4))
        {
          /* Print in next display, if number of lines in next host name
           * exceeds maximum limit.
           */
          firstDisplay = L7_FALSE;
          break;
        }
      } while (rc == L7_SUCCESS);

      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

      for (cmdIndex=1; cmdIndex<argc; cmdIndex++)
      {
        OSAPI_STRNCAT(cmdBuf, " ");
        OSAPI_STRNCAT(cmdBuf, argv[cmdIndex]);
      }
      cliAlternateCommandSet(cmdBuf);

      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */    
    }  /* show radius accounting name */
    else if (numArgs == argNameVal) /* show radius accounting name <servername> */
    {   

      /*verify if the specified string contains all the alpha-numeric characters*/
      if (cliIsAlphaNum((L7_char8 *)argv[index+argNameVal]) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_security_SrvrName);
      }

      if (strlen(argv[index+argNameVal]) > L7_RADIUS_SERVER_NAME_LENGTH)
      {      
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                              pStrErr_security_SrvrNameMustBeCharsOrLess,
                              L7_RADIUS_SERVER_NAME_LENGTH);
      }
      osapiStrncpy(name, argv[index+argNameVal], L7_RADIUS_SERVER_NAME_LENGTH+1);        

      /* Verify if the specified named RADIUS accounting server exists*/
      memset(host,0,sizeof(host));
      if( usmDbRadiusNamedAcctServerAddrGet(unit, name, 
                     host, &addrType) != L7_SUCCESS )
      {      
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusAcctSrvrInvalid);
      }
      if(L7_IP_ADDRESS_TYPE_DNS == addrType )
      {
        ipAddr=0;
        usmDbRadiusServerHostIPAddrGet(unit,host,RADIUS_SERVER_TYPE_ACCT, 
                     &ipAddr);
      }

      /* nameBasedDisplay = L7_TRUE*/
      radiusAccountingStatus(ewsContext, unit, name, host, addrType,ipAddr, L7_TRUE);
    }/*show radius accounting name <servername>*/
  }/*show radius accounting name ..*/

  /* show radius accounting statistics .. */
  else if (strcmp(argv[index+argStats],pStrInfo_common_Dot1xShowStats) == 0)
  {
    /* show radius accounting statistics name <servername> */
    if( numArgs == 3 &&
        strcmp(argv[index+argStatsName],pStrInfo_secuirty_serverName) == 0)
    {      
      /*verify if the specified string contains all the alpha-numeric characters*/
      if (cliIsAlphaNum((L7_char8 *)argv[index+argStatsNameVal]) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_security_SrvrName);
      }

      if (strlen(argv[index+argStatsNameVal]) > L7_RADIUS_SERVER_NAME_LENGTH)
      {      
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                     pStrErr_security_SrvrNameMustBeCharsOrLess,
                     L7_RADIUS_SERVER_NAME_LENGTH);
      }
      osapiStrncpySafe(name, argv[index+argStatsNameVal], 
                     L7_RADIUS_SERVER_NAME_LENGTH+1);
    
      /* Verify if the specified named RADIUS accounting server exists*/
      if( usmDbRadiusNamedAcctServerAddrGet(unit, name, 
                     host, &addrType) != L7_SUCCESS )
      {      
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusAcctSrvrInvalid);
      }
      else if(strlen(host) == 0 )
      {
          osapiStrncpySafe(host,pStrErr_common_NotAvailable,
                     strlen(pStrErr_common_NotAvailable)+1);
      }
    }
    /* show radius accounting  ( statistics <ipaddr | dnsnmae> )*/
    else if( numArgs == 2 ) 
    {
      if (strlen(argv[index+argStatsHost]) >= sizeof(host))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_common_CfgNwIp);
      }
      /* Verify if the specified ip address is valid */
      osapiStrncpySafe(host, argv[index + argStatsHost], sizeof(host));
      /*Validate & Get the address type for Host Address */
      if (cliIPHostAddressValidate(ewsContext, 
                     host, &ipAddr, &addrType) != L7_SUCCESS)
      {
        /* Invalid Host Address*/
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
      }
      /* Verify if the specified ip address is a RADIUS accounting server */
      if ((usmDbRadiusAccountingServerIPHostNameVerify(unit, host, 
                     addrType, &verified) != L7_SUCCESS) ||
                     (verified != L7_TRUE))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusAcctSrvrInvalid);
      }
      memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
      if( usmDbRadiusServerAcctHostNameGet(unit, host, addrType, 
                     name) != L7_SUCCESS )
      {
        sprintf(name,pStrErr_common_NotAvailable);
      }
    }
    
    /* Accounting Server Name*/
    cliFormat(ewsContext, pStrInfo_security_AcctSrvrName2);    /* "Accounting Server Name\0" */
    ewsTelnetWrite(ewsContext, name);

    /* Accounting Server Host Address */
    cliFormat(ewsContext, pStrErr_security_AcctSrvrHostAddr_1);    /* "Accounting Server Host\0" */
    ewsTelnetWrite(ewsContext, host);

    /* Round Trip Time */
    if ( usmDbRadiusAcctServerStatRTTGet( unit, host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      doubleStat = val / 100.0;
      osapiSnprintf(buf, sizeof(buf), "%.2f", doubleStat);
    }
    cliFormat(ewsContext, pStrErr_security_RoundTripTime);    /* "Round Trip Time\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Requests */
    if ( usmDbRadiusAcctServerStatReqGet( unit, host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    cliFormat(ewsContext, pStrErr_security_Reqs);    /* "Requests\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Retransmissions */
    if ( usmDbRadiusAcctServerStatRetransGet( unit, host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    cliFormat(ewsContext, pStrErr_security_Retransmissions);    /* "Retransmissions\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Responses */
    if ( usmDbRadiusAcctServerStatResponseGet(unit, host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    cliFormat(ewsContext, pStrErr_security_Resps);    /* "Responses\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Malformed Responses */
    if ( usmDbRadiusAcctServerStatMalformedResponseGet( unit, 
                     host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    cliFormat(ewsContext, pStrErr_security_MalformedResps);    /* "Malformed Responses\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Bad Authenticators */
    if ( usmDbRadiusAcctServerStatBadAuthGet( unit, host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    cliFormat(ewsContext, pStrInfo_security_BadAuthenticators);    /* "Bad Authenticators\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Pending Requests */
    if ( usmDbRadiusAcctServerStatPendingReqGet( unit, 
                     host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }

    cliFormat(ewsContext, pStrErr_security_PendingReqs_1);    /* "Pending Requests\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Timeouts */
    if ( usmDbRadiusAcctServerStatTimeoutsGet( unit, host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    cliFormat(ewsContext, pStrInfo_security_Timeouts);    /* "Timeouts\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Unknown Types */
    if ( usmDbRadiusAcctServerStatUnknownTypeGet( unit, host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    cliFormat(ewsContext, pStrInfo_security_UnknownTypes);    /* "Unknown Types\0" */
    ewsTelnetWrite( ewsContext, buf );

    /* Packets Dropped */
    if ( usmDbRadiusAcctServerStatPktsDroppedGet( unit, 
                     host, &val) != L7_SUCCESS )
    {
      sprintf( buf,  pStrInfo_common_NotApplicable);
    }                               /* "N\A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    cliFormat(ewsContext, pStrErr_security_PktsDropped_1);    /* "Packets Dropped\0" */
    ewsTelnetWrite( ewsContext, buf );
  }/* show radius accounting statistics .. */

  else if ( numArgs == argHost) /* show radius accounting ( <ipaddr | dnsname> )*/
  {    
    if (strlen(argv[index+argHost]) >= sizeof(host))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  
                     ewsContext, pStrErr_common_CfgNwIp);
    }      
    /* Verify if the specified ip address is valid */
    osapiStrncpySafe(host, argv[index + argHost], sizeof(host));
    /*Validate & Get the address type for Host Address */
    if (cliIPHostAddressValidate(ewsContext, 
                     host, &ipAddr, &addrType) != L7_SUCCESS)
    {
      /* Invalid Host Address*/
      return cliSyntaxReturnPrompt (ewsContext, 
                     pStrInfo_common_EmptyString);
    }
    /* Verify if the specified ip address is a RADIUS accounting server */
    if ((usmDbRadiusAccountingServerIPHostNameVerify(unit, host, 
                     addrType, &verified) != L7_SUCCESS) ||
                     (verified != L7_TRUE))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusAcctSrvrInvalid);
    }
    memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
    
    if( usmDbRadiusServerAcctHostNameGet(unit, host, 
                     addrType, name) != L7_SUCCESS )
    {
      sprintf(name,pStrErr_common_NotAvailable);
    }

    if(L7_IP_ADDRESS_TYPE_DNS == addrType )
    {
      ipAddr=0;
      usmDbRadiusServerHostIPAddrGet(unit,host,RADIUS_SERVER_TYPE_ACCT, 
                     &ipAddr);
    }
    radiusAccountingStatus(ewsContext, unit, name, host, addrType,ipAddr, 
                     L7_FALSE);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display radius accounting server status.
*
* @param EwsContext ewsContext
* @param L7_uint32 unit 
* @param L7_char8 *name
* @param L7_char8 *host
* @param L7_IP_ADDRESS_TYPE_t addrType)
*
* @returntype void
*
*
* @end
*
*********************************************************************/
void radiusAccountingStatus(EwsContext ewsContext,L7_uint32 unit, 
                     L7_char8 *name, L7_char8 *host,
                     L7_IP_ADDRESS_TYPE_t addrType,
                     L7_uint32 ipAddr,L7_BOOL nameBasedDisplay)
{ 
  L7_uint32 val;
  L7_char8 buf[L7_DNS_HOST_NAME_LEN_MAX];
  L7_BOOL boolVal;

  if(L7_TRUE == nameBasedDisplay)
  {
    /* Accounting Server Name*/
    cliFormat(ewsContext, pStrInfo_security_AcctSrvrName2);    
    ewsTelnetWrite(ewsContext, name);

    radiusAcctServerDisplay(ewsContext,host,addrType,ipAddr);
  }
  else
  {
    radiusAcctServerDisplay(ewsContext,host,addrType,ipAddr);
    /* Accounting Server Name*/
    cliFormat(ewsContext, pStrInfo_security_AcctSrvrName2);    
    ewsTelnetWrite(ewsContext, name);
  }

  memset(buf, 0, sizeof(buf));

  if ( usmDbRadiusAccountingModeGet(unit, &val ) != L7_SUCCESS )
  {    
    osapiSnprintf(buf, sizeof(buf), 
                   strUtilEnableDisableGet(val,pStrInfo_common_NotApplicable));

  }                                      /* "N/A" */
  else
  {
    /* "Disable" */
    osapiSnprintf(buf, sizeof(buf), 
                   strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
  }

  cliFormat(ewsContext, pStrInfo_common_ApProfileRadiusAcctMode); /* "Accounting Mode\0" */
  ewsTelnetWrite(ewsContext, buf);

  /* Port */
  cliFormat( ewsContext,  pStrInfo_common_Port_2 ); /* Port\0" */
  memset ( buf, 0, sizeof(buf));
  if (usmDbRadiusAccountingHostNameServerPortNumGet(unit, host, 
                     addrType, &val) != L7_SUCCESS)
  {
    sprintf( buf,  pStrInfo_common_NotApplicable );
  }                                 /* "N/A" */
  else
  {
    sprintf( buf, "%u", val);
  }
  ewsTelnetWrite(ewsContext,buf);

  /* Secret Configured */
  cliFormat( ewsContext,  pStrInfo_security_SecretCfgured); /* "Secret Configured\0" */
  memset ( buf, 0, sizeof(buf));
  if ( usmDbRadiusAccountingHostNameServerSharedSecretConfigured( unit, host,
                     addrType, &boolVal ) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                               /* "N\A" */
  else
  {
    if (boolVal == L7_TRUE)
    {
      sprintf( buf,  pStrInfo_common_Yes);
    }                                /* "Yes"*/
    else
    {
      sprintf( buf,  pStrInfo_common_No);
    }                               /* "No"*/
  }

  ewsTelnetWrite( ewsContext, buf);
}

/*********************************************************************
*
* @purpose  display radius server statistics.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking
*
* @cmdsyntax  show radius statistics { name <servername> | <ipaddr | dnsname> } 
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowRadiusStatistics(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArgs;  
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_uchar8 host[L7_DNS_HOST_NAME_LEN_MAX];  
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 unit;
  L7_uint32 val;
  
  L7_uint32 argName = 1, argNameVal = 2;  
  L7_char8 buf[L7_DNS_HOST_NAME_LEN_MAX];

  L7_uint32 argHost = 1;
  L7_BOOL verified;
  L7_double64 doubleStat;
  L7_uint32 ipAddr;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  if ( numArgs <  1 ||  numArgs > 2 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_ShowRadiusSrvrStats);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_ErrCouldNot, ewsContext, 
                     pStrErr_common_UnitId_1);
  }
  /* show radius statistics name <servername> */
  if( 2 == numArgs  &&
        strcmp(argv[index+argName],pStrInfo_secuirty_serverName) == 0) 
  {
 
    /*verify if the specified string contains all the alpha-numeric characters*/
    if (cliIsAlphaNum((L7_char8 *)argv[index+argNameVal]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_security_SrvrName);
    }

    if (strlen(argv[index+argNameVal]) > L7_RADIUS_SERVER_NAME_LENGTH)
    {      
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                     pStrErr_security_SrvrNameMustBeCharsOrLess,
                     L7_RADIUS_SERVER_NAME_LENGTH);
    }
    osapiStrncpySafe(name, argv[index+argNameVal], 
                     L7_RADIUS_SERVER_NAME_LENGTH+1);    

    /* Verify if the specified named RADIUS server exists*/
    if( usmDbRadiusNamedServerAddrGet(unit, name, 
                     host, &addrType) != L7_SUCCESS )
    {      
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusSrvrInvalid);
    }    
    else if(strlen(host) == 0)
    {
      osapiStrncpySafe(host,pStrErr_common_NotAvailable,
                     strlen(pStrErr_common_NotAvailable)+1);
    }
  }
  else if( 1 == numArgs ) /* show radius statistics ( <ipaddr | dnsnmae> )*/
  {      
    if (strlen(argv[index+argHost]) >= sizeof(host))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  
                     ewsContext, pStrErr_common_CfgNwIp);
    }
    /* Verify if the specified ip address is valid */
    osapiStrncpySafe(host, argv[index + argHost], sizeof(host));
    /*Validate & Get the address type for Host Address */
    if (cliIPHostAddressValidate(ewsContext, 
                     host, &ipAddr, &addrType) != L7_SUCCESS)
    {
      /* Invalid Host Address*/
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
    }
    /* Verify if the specified ip address is a RADIUS accounting server */
    if ((usmDbRadiusHostNameServerAddrVerify(unit, host, 
                     addrType, &verified) != L7_SUCCESS) ||
          (verified != L7_TRUE))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusSrvrInvalid);
    }
    memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);

    if( usmDbRadiusServerHostNameGet(unit, host, addrType, name) != L7_SUCCESS )
    {
      sprintf(name,pStrErr_common_NotAvailable);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     "radius servers stats: Invalid Inputs!!!");
  }

  /* Authenticating Server Name*/
  cliFormat(ewsContext, pStrInfo_security_AuthSrvrName);    /* "Accounting Server Name\0" */
  ewsTelnetWrite(ewsContext, name);

  /* Authenticating Server Host Address */
  cliFormat(ewsContext, pStrInfo_common_SrvrHost);    /* "Accounting Server Host\0" */
  ewsTelnetWrite(ewsContext, host);

  /* Round Trip Time */
  if ( usmDbRadiusServerStatRTTGet(unit, host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);   /* "N\A" */
  }
  else
  {
    doubleStat = val / 100.0;
    osapiSnprintf(buf, sizeof(buf), "%.2f", doubleStat);
  }

  cliFormat(ewsContext, pStrErr_security_RoundTripTime);    /* "Round Trip Time\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Access Requests */
  if ( usmDbRadiusServerStatAccessReqGet( unit, host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }

  cliFormat(ewsContext, pStrInfo_security_AccessReqs);    /* "Access Requests\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Access Retransmissions */
  if ( usmDbRadiusServerStatAccessRetransGet( unit, host, 
                     &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }

  cliFormat(ewsContext, pStrErr_security_AccessRetransmissions_1);    /* "Access Retransmissions\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Access Accepts */
  if ( usmDbRadiusServerStatAccessAcceptGet(unit, host, 
                                                      &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrErr_security_AccessAccepts_1);    /* "Access Accepts\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Access Rejects */
  if ( usmDbRadiusServerStatAccessRejectGet( unit, host, &val) != L7_SUCCESS )
  {
    sprintf( buf, pStrInfo_common_NotApplicable);
  }                                             /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext,pStrInfo_security_AccessRejects);    /* "Access Rejects\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Access Challenges */
  if ( usmDbRadiusServerStatAccessChallengeGet( unit, 
                    host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrErr_security_AccessChallenges_1);    /* "Access Challenges\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Malformed Access Responses */
  if ( usmDbRadiusServerStatMalformedAccessResponseGet( unit, 
                     host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrInfo_security_MalformedAccessResps);    /* "Malformed Access Responses\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Bad Authenticators */
  if ( usmDbRadiusServerStatBadAuthGet( unit, host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrInfo_security_BadAuthenticators);    /* "Bad Authenticators\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Pending Requests */
  if ( usmDbRadiusServerStatPendingReqGet( unit, host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrErr_security_PendingReqs_1);    /* "Pending Requests\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Timeouts */
  if ( usmDbRadiusServerStatTimeoutsGet( unit, host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrInfo_security_Timeouts);    /* "Timeouts\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Unknown Types */
  if ( usmDbRadiusServerStatUnknownTypeGet( unit, host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrInfo_security_UnknownTypes);    /* "Unknown Types\0" */
  ewsTelnetWrite( ewsContext, buf );

  /* Packets Dropped */
  if ( usmDbRadiusServerStatPktsDroppedGet( unit, host, &val) != L7_SUCCESS )
  {
    sprintf( buf,  pStrInfo_common_NotApplicable);
  }                                 /* "N\A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrErr_security_PktsDropped_1);    /* "Packets Dropped\0" */
  return cliSyntaxReturnPrompt (ewsContext, buf );
}

/*********************************************************************
*
* @purpose  display radius  server status.
*
* @param EwsContext ewsContext
* @param L7_uint32 unit 
* @param L7_char8 *name
* @param L7_char8 *host
* @param L7_IP_ADDRESS_TYPE_t addrType
* @param L7_BOOL flag
*
* @returntype void
*
*
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
void radiusStatus(EwsContext ewsContext,L7_uint32 unit, L7_char8 *name, 
                     L7_char8 *host, L7_IP_ADDRESS_TYPE_t addrType, 
                      L7_uint32 ipAddr,L7_BOOL nameBasedDisplay)
{
  L7_uint32 val;
  L7_char8 buf[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 buf2[L7_DNS_HOST_NAME_LEN_MAX];
  L7_BOOL boolVal;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;
  L7_uchar8 strIpAddr[20];

  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);

  if(L7_TRUE == nameBasedDisplay )
  {
    /* Auth Server Name*/
    cliFormat(ewsContext, pStrInfo_security_AuthSrvrName);    
    ewsTelnetWrite(ewsContext, name);

    radiusAuthServerDisplay(ewsContext, host,addrType,ipAddr,
                     pStrInfo_security_CurrentSrvrDNSAddr,
                     pStrInfo_security_CurrentSrvrIPAddr);

  }
  else /* host address based status display */
  {
    radiusAuthServerDisplay(ewsContext, host,addrType,ipAddr,
                     pStrInfo_security_RadSrvrDNSAddr,
                     pStrInfo_security_RadSrvrIPAddr);

    /* Auth Server Name*/
    cliFormat(ewsContext, pStrInfo_security_AuthSrvrName);    
    ewsTelnetWrite(ewsContext, name);
  }
/*
  memset(host,0,sizeof(host));
  if(L7_IP_ADDRESS_TYPE_DNS == addrType ) 
  {
    osapiStrncpySafe(host,hostAddr.host.hostName,sizeof(host));      
  }
  else
  {
    osapiStrncpySafe(host,osapiInet_ntoa(hostAddr.host.ipAddr),sizeof(host));      
  }
*/

  /* Max Number of Retransmits */
  memset ( buf, 0, sizeof(buf));
  if (usmDbRadiusServerRetransGet(strIpAddr, &val, &paramStatus) != L7_SUCCESS)
  {
    sprintf( buf,  pStrErr_common_NotAvailable );
  }                                      /* "N/A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrErr_security_MaxNumOfReTxs_1);    /* "Max Number of Retransmits\0" */
  ewsTelnetWrite(ewsContext, buf);

  /* Timeout Duration */
  memset ( buf, 0, sizeof(buf));
  if ( usmDbRadiusServerTimeOutGet(strIpAddr, &val, &paramStatus) != L7_SUCCESS )
  {
    sprintf( buf,  pStrErr_common_NotAvailable );
  }                                      /* "N/A" */
  else
  {
    sprintf( buf, "%u", val );
  }
  cliFormat(ewsContext, pStrErr_security_TimeoutDuration);    /* "Timeout Duration\0" */
  ewsTelnetWrite(ewsContext, buf);

  /* Accounting Mode */
  if ( usmDbRadiusAccountingModeGet(unit, &val ) != L7_SUCCESS )
  {
    sprintf( buf,  pStrErr_common_NotAvailable );
  }                                      /* "N/A" */
  else
  {  /* "Disable" */
    osapiSnprintf(buf, sizeof(buf), 
                     strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));                                       
  }

  cliFormat(ewsContext, pStrInfo_common_ApProfileRadiusAcctMode); /* "Accounting Mode\0" */
  ewsTelnetWrite(ewsContext, buf);

  /* Attribute 4 */
  if ( usmDbRadiusAttribute4Get(unit,&boolVal, &val ) != L7_SUCCESS )
  {
    sprintf( buf,  pStrErr_common_NotAvailable );  /* "N/A" */
    sprintf( buf2,  pStrErr_common_NotAvailable );  /* "N/A" */
  }
  else
  {
    memset ( buf, 0, sizeof(buf) );
    memset ( buf2, 0, sizeof(buf2) );
    sprintf(buf,strUtilEnableDisableGet(boolVal,pStrInfo_common_Dsbl_1));                                       /* "Disable" */

    usmDbInetNtoa(val, buf2);

  }
  cliFormat(ewsContext, pStrErr_security_RadiusAttr_1); /* "Attribute 4 Mode\0" */

  ewsTelnetWrite(ewsContext, buf);
  cliFormat(ewsContext, pStrErr_security_RadiusAttr_2); /* "Attribute 4 Mode\0" */
  ewsTelnetWrite(ewsContext, buf2); 

  /* Port */
  cliFormat( ewsContext,  pStrInfo_common_Port_2 ); /* Port\0" */
  memset (buf, 0, sizeof(buf));
  if (usmDbRadiusHostNameServerPortNumGet(unit, host, 
                     addrType, &val) != L7_SUCCESS)
  {
    sprintf( buf, pStrErr_common_NotAvailable);
  }                                    /* "N\A" */
  else
  {
    sprintf( buf,"%u", val);
  }
  ewsTelnetWrite(ewsContext, buf); 

  /* Type */
  cliFormat( ewsContext,  pStrInfo_security_AuthType ); 
  if (usmDbRadiusHostNameServerEntryTypeGet(unit, host, 
                     addrType, &val) != L7_SUCCESS)
  {
    sprintf( buf,pStrErr_common_NotAvailable);
  }                                     /* "N\A" */
  else
  {
    memset ( buf, 0, sizeof(buf));
    if (val == L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY)
    {
      sprintf( buf,pStrInfo_security_Primary);
    }                                            
    else
    {
      sprintf( buf,pStrInfo_security_Secondary);
    }                                             
  }
  ewsTelnetWrite(ewsContext, buf); 

  /* Secret Configured */
  cliFormat( ewsContext,  pStrInfo_security_SecretCfgured); /* "Secret Configured\0" */
  memset ( buf, 0, sizeof(buf));
  if ( usmDbRadiusHostNameServerSharedSecretConfigured(host, &boolVal ) != L7_SUCCESS ) 
  {
    sprintf( buf,pStrErr_common_NotAvailable);
  }                                     /* "N\A" */
  else
  {
    if (boolVal == L7_TRUE)
    {
      sprintf( buf,pStrInfo_common_Yes);
    }                                       /* "Yes"*/
    else
    {
      sprintf( buf,pStrInfo_common_No);
    }                                      /* "No"*/
  }
  ewsTelnetWrite(ewsContext, buf); 

  /* Message Authenticator */
  cliFormat( ewsContext,  pStrInfo_security_MsgAuthenticator); 
  memset ( buf, 0, sizeof(buf));
  if (usmDbRadiusHostNameServerIncMsgAuthModeGet(unit, host, 
                     addrType,&val ) != L7_SUCCESS)
  {
    sprintf( buf,pStrErr_common_NotAvailable);
  }                                     /* "N\A" */
  else
  {
    sprintf(buf,strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));                                            /* "Disable" */
  }
  ewsTelnetWrite(ewsContext, buf); 

  return;
}

/*********************************************************************
*
* @purpose  display radius server summary and detailed information
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
* @notes add range checking
*
* @cmdsyntax  show radius 
*                    servers  [ { name [ <servername> ] | <ipaddr|hostname>  }]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowRadiusServers(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 * * argv,
                                         L7_uint32 index)
{
  static L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];

  L7_BOOL boolVal;
  L7_BOOL firstRow = L7_TRUE;
  L7_uint32 val;
  L7_uint32 numArgs, lineCount = 0,  cmdIndex = 0;
  L7_char8 buf[L7_DNS_HOST_NAME_LEN_MAX];
  static L7_char8 host[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uchar8 buf2[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 rowString[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 splitLabel[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  static L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_IP_ADDRESS_TYPE_t addrType1 = L7_IP_ADDRESS_TYPE_UNKNOWN; /*dummy*/
  L7_uchar8 *hostAddrTraverse = L7_NULLPTR;
  static L7_BOOL firstDisplay = L7_TRUE; /*useful in table display*/
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 hostLen = L7_NULL;  
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;

  L7_uint32  argServers = 1, argName = 2, argNameVal = 3, argHost = 2;
  L7_uint32 ipAddr;
  L7_BOOL verified;  
  static L7_BOOL tableDisplay = L7_FALSE;/*useful in table display*/

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);
  numArgs = cliNumFunctionArgsGet();
  memset (rowString, 0, sizeof(rowString));

  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);

  if ( numArgs > 3 ) 
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_ShowRadiusSummary_1);
  }
  
  /* if 'show radius servers' or 'show radius servers name' previously issued 
   * for summary table display which has more than 24 entries, only 24 rows 
   * might h'v been displayed till now. So, verifying if (M)ore Or (Q)quit 
   * entered in order for the rest of the rows to be displayed.
   */
  if( L7_TRUE == tableDisplay && cliGetCharInputID() != CLI_INPUT_EMPTY )
  { 
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      tableDisplay = L7_FALSE;
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    firstDisplay = L7_TRUE;
    addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
    memset (host, 0, sizeof(host));  
    memset(name,0,sizeof(name));
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_ErrCouldNot, ewsContext, 
                     pStrErr_common_UnitId_1);
  }

  if(0 ==  numArgs ) /* show radius */
  {
    /* "Number of Configured Auth Servers\0" */
    bzero(buf,sizeof(buf));
    if(usmDbRadiusConfiguredServersCountGet(unit,
                     RADIUS_SERVER_TYPE_AUTH, &val) == L7_SUCCESS )
    {
      osapiSnprintf(buf, sizeof(buf), "%u", val );
      cliFormat(ewsContext, pStrInfo_security_NumAuthServers);    
    }
    else
    {
      osapiSnprintf(buf,sizeof(buf),pStrErr_common_NotAvailable);
    }
    ewsTelnetWrite(ewsContext, buf);
    
    /* "Number of Configured Acct Servers\0" */
    bzero(buf,sizeof(buf));
    if(usmDbRadiusConfiguredServersCountGet(unit,
                     RADIUS_SERVER_TYPE_ACCT, &val) == L7_SUCCESS )
    {
      osapiSnprintf(buf, sizeof(buf), "%u", val );
      cliFormat(ewsContext, pStrInfo_security_NumAcctServers);    
    }
    else
    {
      osapiSnprintf(buf,sizeof(buf),pStrErr_common_NotAvailable);
    }
    ewsTelnetWrite(ewsContext, buf);

    /* "Number of Configured Named Auth Servers Groups\0" */
    bzero(buf,sizeof(bzero));
    if(usmDbRadiusConfiguredNamedServersGroupCountGet(unit,
                     RADIUS_SERVER_TYPE_AUTH, &val) == L7_SUCCESS )
    {
      osapiSnprintf(buf, sizeof(buf), "%u", val );
      cliFormat(ewsContext, pStrInfo_security_NumAuthServerGrps);    
    }
    else
    {
      osapiSnprintf(buf,sizeof(buf),pStrErr_common_NotAvailable);
    }
    ewsTelnetWrite(ewsContext, buf);
    
    /* "Number of Configured Named Acct Servers Groups\0" */
    bzero(buf,sizeof(bzero));
    if(usmDbRadiusConfiguredNamedServersGroupCountGet(unit,
                     RADIUS_SERVER_TYPE_ACCT, &val) == L7_SUCCESS )
    {
      osapiSnprintf(buf, sizeof(buf), "%u", val );
      cliFormat(ewsContext, pStrInfo_security_NumAcctServerGrps);    
    }
    else
    {
      osapiSnprintf(buf,sizeof(buf),pStrErr_common_NotAvailable);
    }
    ewsTelnetWrite(ewsContext, buf);

    /* Max Number of Retransmits */
    memset ( buf, 0, sizeof(buf));
    if (usmDbRadiusServerRetransGet(strIpAddr, &val, &paramStatus) != L7_SUCCESS)
    {
      sprintf( buf,  pStrErr_common_NotAvailable );
    }                                      /* "N/A" */
    else
    {
      sprintf( buf, "%u", val );
    }
    
    /* "Max Number of Retransmits\0" */
    cliFormat(ewsContext, pStrErr_security_MaxNumOfReTxs_1);    
    ewsTelnetWrite(ewsContext, buf);

    /* Timeout Duration */
    memset ( buf, 0, sizeof(buf));
    if ( usmDbRadiusServerTimeOutGet(strIpAddr, &val, &paramStatus) != L7_SUCCESS )
    {
      sprintf( buf,  pStrErr_common_NotAvailable );
    }                                      /* "N/A" */
    else
    {
      sprintf( buf, "%u", val );
    }

    /* "Timeout Duration\0" */
    cliFormat(ewsContext, pStrErr_security_TimeoutDuration);    
    ewsTelnetWrite(ewsContext, buf);

    /* Accounting Mode */
    if ( usmDbRadiusAccountingModeGet(unit, &val ) != L7_SUCCESS )
    {
      sprintf( buf,  pStrErr_common_NotAvailable );
    }                                      /* "N/A" */
    else
    {  /* "Disable" */
      osapiSnprintf(buf, sizeof(buf), 
                     strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));                                       
    }
  
    /* "Accounting Mode\0" */
    cliFormat(ewsContext, pStrInfo_common_ApProfileRadiusAcctMode); 
    ewsTelnetWrite(ewsContext, buf);

    /* Attribute 4 */
    if ( usmDbRadiusAttribute4Get(unit,&boolVal, &val ) != L7_SUCCESS )
    {
      sprintf( buf,  pStrErr_common_NotAvailable );  /* "N/A" */
      sprintf( buf2,  pStrErr_common_NotAvailable );  /* "N/A" */
    }
    else
    {
      memset ( buf, 0, sizeof(buf) );
      memset ( buf2, 0, sizeof(buf2) );
      sprintf(buf,strUtilEnableDisableGet(boolVal,pStrInfo_common_Dsbl_1));                                       /* "Disable" */

      usmDbInetNtoa(val, buf2);

    }
    cliFormat(ewsContext, pStrErr_security_RadiusAttr_1); /* "Attribute 4 Mode\0" */

    ewsTelnetWrite(ewsContext, buf);
    cliFormat(ewsContext, pStrErr_security_RadiusAttr_2); /* "Attribute 4 Value\0" */
    ewsTelnetWrite(ewsContext, buf2); 
  }
  else if(numArgs == argServers) /* show radius ( servers )*/
  {
    if (firstDisplay == L7_TRUE)
    {
      /* For first time display, get the First entry *
      memset (host, 0, sizeof(host) );*/
      if (usmDbRadiusServerFirstIPHostNameGet(unit, 
                     host, &addrType) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  
                     ewsContext,  pStrErr_security_NoRadiusSrvrsCfgured);
      }

      /* Print headings */

      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                     pStrInfo_security_CurrentIpAddrPortTypeCfguredAuthenticator);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                     pStrInfo_security_CurrentIpAddrPortTypeCfguredAuthenticator1);
      ewsTelnetWrite(ewsContext,
                     "\r\n---- ------------------------ -------------------------------- ----- ---------");
    }

    do
    {
      hostAddrTraverse = host;
      while (*hostAddrTraverse  != L7_EOS)
      {
        if (firstRow == L7_TRUE)
        {
          /* Get and store the current Server Host Address */
          memset (buf2, 0, sizeof(buf2));
          memset(name,0,sizeof(name));

          if( usmDbRadiusServerHostNameGet(unit, host, addrType, name) != L7_SUCCESS )
          {
            sprintf(name,pStrErr_common_NotAvailable);
          }
          else 
          {
            (void)usmDbRadiusNamedServerAddrGet(unit, name, buf2, &addrType1);
          }

          /* Displaying First Row after concatenating all column entries */
          /* Check for current server */
          memset (rowString, 0, sizeof(rowString));
          if (osapiStrncmp(buf2, pStrInfo_security_Empty, 
                             sizeof(buf2)) != 0 && 
                osapiStrncmp(host, buf2, sizeof(host)) == 0)
          {
            osapiSnprintf(rowString, sizeof(rowString), 
                                       "%s", "\r\n  *  "); 
          }
          else
          {
            osapiSnprintf(rowString, sizeof(rowString), 
                                       "%s", "\r\n     "); 
          }
          /* Host Address */
          memset (buf, 0, sizeof(buf));
          if (osapiStrncmp(host, pStrInfo_security_Empty, sizeof(host)) == 0 )
          {
            sprintf( buf, "%-24s",  pStrInfo_common_NotApplicable);/* "N\A" */
          }
          else
          {
            /* Get the split hostname which fits in available space 
             * (24 characters) in a row 
             */
             cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
             osapiSnprintf(buf, sizeof(buf), "%-24s", splitLabel);
          }
          OSAPI_STRNCAT(rowString, buf);
  
          /* Server Name */
          sprintf( buf, " %-32s", name);
          OSAPI_STRNCAT(rowString, buf);

          /* Port */
          memset (buf, 0, sizeof(buf));
          if (usmDbRadiusHostNameServerPortNumGet(unit, host, 
                                                addrType, &val) != L7_SUCCESS)
          {
            sprintf( buf, " %-5s",  pStrInfo_common_NotApplicable);
          }                                    /* "N\A" */
          else
          {
            sprintf( buf, " %-5u", val);
          }
          OSAPI_STRNCAT(rowString, buf);

          /* Type */
          if (usmDbRadiusHostNameServerEntryTypeGet(unit, host, 
                                             addrType, &val) != L7_SUCCESS)
          {
            sprintf( buf, " %-9s",  pStrInfo_common_NotApplicable);
          }                                     /* "N\A" */
          else
          {
            memset ( buf, 0, sizeof(buf));
            if (val == L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY)
            {
              sprintf( buf, " %-9s",  pStrInfo_security_Primary);
            }                                            /* "Primary" */
            else
            {
              sprintf( buf, " %-9s",  pStrInfo_security_Secondary);
            }                                              /* "Secondary" */
          }
          OSAPI_STRNCAT(rowString, buf);

          ewsTelnetWrite( ewsContext, rowString);
          firstRow = L7_FALSE;
          lineCount++;
        } /* end of if (firstRow == L7_TRUE)*/
        else
        {
          /* Displaying remaining hostname in second Row */
          memset (rowString, 0, sizeof(rowString));
          memset (splitLabel, 0, sizeof(splitLabel));
          /* Leaving space for first column*/
          osapiSnprintf(rowString, sizeof(rowString), "%s", "\r\n     "); 
        
          /* Get the split hostname which fits in available space 
           * (24 characters) in a row 
           */
          cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
          OSAPI_STRNCAT(rowString, splitLabel);
          ewsTelnetWrite( ewsContext, rowString);
          lineCount++;
        }
      } /* end of while */
      firstRow = L7_TRUE;
      rc = usmDbRadiusServerNextIPHostNameGet(unit,host, host, &addrType);
      if (rc != L7_SUCCESS)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      hostLen = strlen(host); 
      if (lineCount >= (CLI_MAX_SCROLL_LINES-4))
      {
        /* Print in next display, if number of lines in next host name
         * exceeds maximum limit.
         */
        firstDisplay = L7_FALSE;
        break;
      }
    } while (rc == L7_SUCCESS);

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

    for (cmdIndex=1; cmdIndex<argc; cmdIndex++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[cmdIndex]);
    }
    cliAlternateCommandSet(cmdBuf);
   
    /* By setting this flag, we are expecting (M) or (q) be char. input */
    tableDisplay = L7_TRUE;
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }
  /*show radius (servers name .. )*/
  else if(strcmp(argv[index+argName],pStrInfo_secuirty_serverName) == 0 )
  {
    if (numArgs == argName) /* show radius ( servers name )*/
    {
      if (firstDisplay == L7_TRUE)
      {
        /* For first time display, get the First entry */
        memset (host, 0, sizeof(host) );
        memset(name,0,sizeof(name));
        if(usmDbRadiusNamedServerOrderlyGetNext(unit,
                     name, name, host,&addrType) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  
                     ewsContext,  pStrErr_security_NoRadiusSrvrsCfgured);
        }


        /* Print headings */

        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                     pStrInfo_security_SrvrNameHostAddPortSecretConfig);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                     pStrInfo_security_SrvrNameHostAddPortSecretConfig1);
        ewsTelnetWrite(ewsContext,          
          "\r\n-------------------------------- ------------------------ ------ ----------");
      }
      do
      {
        hostAddrTraverse = host;
        if(strlen(host) == 0 )
        {
          osapiStrncpySafe(host,pStrErr_common_NotAvailable,
                     strlen(pStrErr_common_NotAvailable)+1);
        }
        while (*hostAddrTraverse  != L7_EOS)
        {
          if (firstRow == L7_TRUE)
          {
            /* Displaying First Row after concatenating all column entries */
            memset (rowString, 0, sizeof(rowString));

            /* Server Name */
            memset (buf, 0, sizeof(buf));
            sprintf( buf, "\r\n%-32s", name);
            OSAPI_STRNCAT(rowString, buf);
             
            /* Host Address */
            /* Get the split hostname which fits in available space 
             * (24 characters) in a row 
             */
            memset (buf, 0, sizeof(buf));
            cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
            osapiSnprintf(buf, sizeof(buf), " %-24s", splitLabel);
            OSAPI_STRNCAT(rowString, buf);

            /* Port */
            memset (buf, 0, sizeof(buf));
            if (usmDbRadiusHostNameServerPortNumGet(unit, host, 
                                                addrType, &val) != L7_SUCCESS)
            {
              sprintf( buf, " %-5s",  pStrInfo_common_NotApplicable);
            }                                    /* "N\A" */
            else
            {
              sprintf( buf, " %-5u", val);
            }
            OSAPI_STRNCAT(rowString, buf);

            /* Secret Configured */
            memset ( buf, 0, sizeof(buf));
            if ( usmDbRadiusHostNameServerSharedSecretConfigured(host, &boolVal ) != L7_SUCCESS ) 
            {
              sprintf( buf, "  %-10s ",  pStrInfo_common_NotApplicable);
            }                                     /* "N\A" */
            else
            {
              if (boolVal == L7_TRUE)
              {
                sprintf( buf, "  %-10s ",  pStrInfo_common_Yes);
              }                                       /* "Yes"*/
              else
              {
                sprintf( buf, "  %-10s ",  pStrInfo_common_No);
              }                                      /* "No"*/
            }

            OSAPI_STRNCAT(rowString, buf);
            ewsTelnetWrite( ewsContext, rowString);
            firstRow = L7_FALSE;
            lineCount++;
          } /* end of if (firstRow == L7_TRUE)*/
          else
          {
            /* Displaying remaining hostname in second Row */
            memset (rowString, 0, sizeof(rowString));
            memset (splitLabel, 0, sizeof(splitLabel));

            memset (buf, 0, sizeof(buf));
            memset(buf,' ',33);
            osapiSnprintf(rowString, sizeof(rowString), "\r\n%33s", buf);         
        
            /* Get the split hostname which fits in available space 
             * (24 characters) in a row 
             */
            cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
            OSAPI_STRNCAT(rowString, splitLabel);
            ewsTelnetWrite( ewsContext, rowString);
            lineCount++;
          }
        } /* end of while */
        firstRow = L7_TRUE;
        memset (host, 0, sizeof(host));
        rc = usmDbRadiusNamedServerOrderlyGetNext(unit, name, name, host,&addrType);

        if (rc != L7_SUCCESS)
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt (ewsContext, "");
        }

        hostLen = strlen(host); 
        if (lineCount >= (CLI_MAX_SCROLL_LINES-4))
        {
          /* Print in next display, if number of lines in next host name
           * exceeds maximum limit.
           */
          firstDisplay = L7_FALSE;
          break;
        }
      } while (rc == L7_SUCCESS);
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

      cliSyntaxBottom(ewsContext);

      osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

      for (cmdIndex=1; cmdIndex<argc; cmdIndex++)
      {
        OSAPI_STRNCAT(cmdBuf, " ");
        OSAPI_STRNCAT(cmdBuf, argv[cmdIndex]);
      }
      cliAlternateCommandSet(cmdBuf);

      /* By setting this flag, we are expecting (M) or (q) be char. input */
      tableDisplay = L7_TRUE;
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */    
    }
    else if (numArgs == argNameVal) /* show radius ( servers name <servername> )*/
    {    
      /*verify if the specified string contains all the alpha-numeric characters*/
      if (cliIsAlphaNum((L7_char8 *)argv[index+argNameVal]) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_security_SrvrName);
      }

    if (strlen(argv[index+argNameVal]) > L7_RADIUS_SERVER_NAME_LENGTH)
    {      
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                              pStrErr_security_SrvrNameMustBeCharsOrLess,
                              L7_RADIUS_SERVER_NAME_LENGTH);
    }
    osapiStrncpySafe(name, argv[index+argNameVal], 
                     L7_RADIUS_SERVER_NAME_LENGTH+1);    
    
    memset(host,0,sizeof(host));
    /* Verify if the specified named RADIUS server exists*/
    if( usmDbRadiusNamedServerAddrGet(unit, name, 
                     host, &addrType) != L7_SUCCESS )
    {      
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  
                     ewsContext, pStrErr_security_NoRadiusNamedSrvrExist);
    }

    if(L7_IP_ADDRESS_TYPE_DNS == addrType )
    {
      ipAddr=0;
      usmDbRadiusServerHostIPAddrGet(unit,host,RADIUS_SERVER_TYPE_AUTH, &ipAddr);
    }

    /* nameBasedDisplay = L7_TRUE*/
    radiusStatus(ewsContext, unit, name, host, addrType, ipAddr,L7_TRUE);

    }/*show radius (servers name <servername> )*/
  }/*show radius (servers name ..)*/
  else if ( numArgs == argHost) /* show radius ( servers <ipaddr | dnsname> )*/
  {    
    if (strlen(argv[index+argHost]) >= sizeof(host))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_common_CfgNwIp);
    }      
    /* Verify if the specified ip address is valid */
    osapiStrncpySafe(host, argv[index + argHost], sizeof(host));
    /*Validate & Get the address type for Host Address */
    if (cliIPHostAddressValidate(ewsContext, 
                     host, &ipAddr, &addrType) != L7_SUCCESS)
    {
      /* Invalid Host Address*/
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
    }
    /* Verify if the specified ip address is a RADIUS server */
    if ((usmDbRadiusHostNameServerAddrVerify(unit, host, 
                     addrType, &verified) != L7_SUCCESS) ||
                     (verified != L7_TRUE))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusSrvrInvalid);
    }
    memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
    

    if( usmDbRadiusServerHostNameGet(unit, host, addrType, name) != L7_SUCCESS )
    {
      osapiStrncpy(name,pStrErr_common_NotAvailable,sizeof(name));
    }

    if(L7_IP_ADDRESS_TYPE_DNS == addrType )
    {
      ipAddr=0;
      usmDbRadiusServerHostIPAddrGet(unit,host,RADIUS_SERVER_TYPE_AUTH,&ipAddr);
    }
    /* nameBasedDisplay = L7_FALSE*/
    radiusStatus(ewsContext, unit, name, host,addrType, ipAddr,L7_FALSE);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}


void radiusAuthServerDisplay(EwsContext ewsContext,L7_char8 *host, 
                     L7_IP_ADDRESS_TYPE_t addrType, L7_uint32 ipAddr,
                     L7_char8 *dnsAddrString, L7_char8 *ipAddrString)
{
  if( L7_IP_ADDRESS_TYPE_DNS == addrType)
  {      
    cliFormat(ewsContext, dnsAddrString);    
    ewsTelnetWrite(ewsContext, host);
  }

    /* "RADIUS Server IP Address\0" */
  cliFormat(ewsContext, ipAddrString);    
  if( L7_IP_ADDRESS_TYPE_DNS == addrType)
  {      
    ewsTelnetWrite(ewsContext, 
                     (( 0 == ipAddr)? 
                     pStrInfo_security_DNSAddrNotResolved : 
                     osapiInet_ntoa(ipAddr))
                  );
  }
  else
  {
    ewsTelnetWrite(ewsContext,host);
  }
}
void radiusAcctServerDisplay(EwsContext ewsContext,L7_char8 *host, 
                     L7_IP_ADDRESS_TYPE_t addrType,L7_uint32 ipAddr)
{
    if( L7_IP_ADDRESS_TYPE_DNS == addrType)
    {
      /* "RADIUS Accounting Server DNS Address\0" */
      cliFormat(ewsContext, pStrInfo_security_AcctSrvrDNSAddr);    
      ewsTelnetWrite(ewsContext, host);
    }
    /* "RADIUS Accounting Server IP Address\0" */
    cliFormat(ewsContext, pStrInfo_security_AcctSrvrIPAddr);    
    if( L7_IP_ADDRESS_TYPE_DNS == addrType)
    {
    ewsTelnetWrite(ewsContext, 
                     (( 0 == ipAddr)? 
                       pStrInfo_security_DNSAddrNotResolved : 
                       osapiInet_ntoa(ipAddr))
                  );
    }
  else
  {
    ewsTelnetWrite(ewsContext,host);
  }

}

