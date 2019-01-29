/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/iscsi/cli_config_iscsi.c
 *
 * @purpose iscsi config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/24/2008
 *
 * @author  Rajakrishna
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "iscsi_exports.h"
#include "ews.h"
#include "clicommands_iscsi.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_util_diffserv_api.h"
#include "clicommands_card.h"
#include "usmdb_qos_iscsi.h"

/*********************************************************************
*
* @purpose  enable/disable iscsi mode globally.
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
* @cmdsyntax  iscsi enable
*
* @cmdhelp Enable/Disable iscsi Mode.
*
* @cmddescript
*   This is a configurable value and can be Enabled or Disabled. It
*   allows you to enable or disable iscsi
*   on global mode. The default value of this
*   parameter is disabled.
*
* @end
*
*********************************************************************/
const L7_char8  *commandIscsiEnable( EwsContext ewsContext, 
                                        L7_uint32 argc, 
                                        const L7_char8 **argv, 
                                        L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 mode = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_Iscsi);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiNo);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(usmDbIscsiAdminModeSet(mode) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiAdminModeSet);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}    

/*********************************************************************
*
* @purpose  Set the iscsi Target Port Parameters.
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
* @cmdsyntax  
*  iscsi target port tcp-port-1 [tcp-port-2.… tcp-port-16] [address ip-address] [name targetname]
*  no iscsi target port tcp-port-1 [tcp-port-2.… tcp-port-16] [address ip-address]
*
* @cmdhelp Set the Target Port Parameters.
*
* @cmddescript
*   This is a configurable value and can be set to certain values. It
*
* @end
*
*********************************************************************/
const L7_char8  *commandIscsiTargetPort( EwsContext ewsContext, 
                                                    L7_uint32 argc, 
                                                    const L7_char8 **argv, 
                                                    L7_uint32 index)
{
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH]={0,};
  L7_uint32 argValue = 1;
  L7_char8  strIPaddr[L7_CLI_MAX_STRING_LENGTH]={0,};
  L7_uint32 tcpPort[L7_ISCSI_MAX_TARGET_TCP_PORTS]={0,};
  L7_uint32 numPorts=1;
  L7_uint32 numArgs;
  L7_uint32 i;
  L7_inet_addr_t inetAddr;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  memset(&inetAddr, 0, sizeof(inetAddr));
  inetAddr.family = L7_AF_INET;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if((numArgs < 1) || (numArgs >20))
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiTargetPort);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 
  else if(ewsContext->commType == CLI_NO_CMD)
  {
     if((numArgs < 1) || (numArgs >18))
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiTargetPortNo);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    } 
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    cliConvertTo32BitUnsignedInteger(argv[index+argValue],&tcpPort[0]);
    argValue++;
    while(argValue<=numArgs)
    {
      if(!osapiStrncmp(argv[index + argValue],
                                   pStrInfo_qos_IscsiAddress,
                                   strlen(argv[index + argValue])))
      {
        osapiStrncpySafe((L7_char8 *)strIPaddr, 
                                      argv[index+argValue+1],
                                      sizeof(strIPaddr));
        if (usmDbParseInetAddrFromStr(strIPaddr, &inetAddr) != L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiIpAddress);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        argValue++;
      }
      else if(!osapiStrncmp(argv[index + argValue],
                                           pStrInfo_qos_IscsiName,
                                           strlen(argv[index + argValue])))
      {
         osapiStrncpy(name, (L7_char8 *) (argv[index + argValue+1]),
                      L7_CLI_MAX_STRING_LENGTH);
         argValue++;
      }
      else
      {
        cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                                  &tcpPort[argValue-1]);
        numPorts++;
      }
      argValue++;
      
    }
  }
  else  if(ewsContext->commType == CLI_NO_CMD)
  {
    cliConvertTo32BitUnsignedInteger(argv[index+argValue],&tcpPort[0]);
    argValue++;
    while(argValue<=numArgs)
    {
      if(!osapiStrncmp(argv[index + argValue],
                                   pStrInfo_qos_IscsiAddress,
                                   strlen(argv[index + argValue])))
      {
        osapiStrncpySafe((L7_char8 *)strIPaddr, 
                                      argv[index+argValue+1],
                                      sizeof(strIPaddr));
        if (usmDbParseInetAddrFromStr(strIPaddr, &inetAddr) != L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiIpAddress);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        argValue++;
      }
      else
      {
        cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                   &tcpPort[argValue-1]);
        numPorts++;
      }
      argValue++;
      
    }

  } 

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      for(i=0;i<numPorts;i++)
      {
        if(usmDbIscsiTargetTcpPortAdd(tcpPort[i], &inetAddr, name) != L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiTargetPortAddEntry);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      for(i=0;i<numPorts;i++)
      {
        if(usmDbIscsiTargetTcpPortDelete(tcpPort[i], &inetAddr)!= L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiTargetPortDeleteEntry);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
  }
    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
i* @purpose  Sets/Resets the COS profile to iSCSI flows.
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
* @cmdsyntax  iscsi cos {vpt vpt | dscp dscp} [remark]
*                     no iscsi cos
*
* @cmdhelp Sets/Resets the COS profile to iSCSI flows.
*
* @cmddescript
*  Sets/Resets the COS profile to iSCSI flows.
*   
*
* @end
*
*********************************************************************/
const L7_char8 *commandIscsiCos( EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 **argv,
                                 L7_uint32 index)
{
  
  L7_uint32 remark = 0;
  L7_uint32 vptVal = 0, dscpVal = 0;
  L7_uint32 numArg;
  L7_uint32 argValue = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
 
  
  numArg = cliNumFunctionArgsGet();
  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if((numArg != 2) && (numArg != 3))
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiCos);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiCosNo);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

     /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if(!osapiStrncmp(argv[index + argValue],
                                   pStrInfo_qos_IscsiVpt,
                                   strlen(argv[index + argValue])))
      {
        cliConvertTo32BitUnsignedInteger(argv[index+ argValue+1],&vptVal);
        if(usmDbIscsiTagFieldSet(L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
                                                   != L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiPRECValueSet);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        if(usmDbIscsiVlanPrioritySet(vptVal) != L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiVlanPrioritySet);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      } 
      else  if(!osapiStrncmp(argv[index + argValue],
                                            pStrInfo_qos_IscsiDscp,
                                            strlen(argv[index + argValue])))
      {
        cliConvertTo32BitUnsignedInteger(argv[index+ argValue+1],&dscpVal);
        if(usmDbIscsiTagFieldSet(L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
                                                   != L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiPRECValueSet);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        if(usmDbIscsiDscpSet(dscpVal) != L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext,pStrErr_qos_IscsiDscpSet);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }   
      if(numArg == 3)
      {
        remark = L7_ENABLE;
      }
      else
      {
        remark = L7_DISABLE;
      }
      if(usmDbIscsiMarkingModeSet(remark) != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiRemarkModeSet);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      } 
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if(usmDbIscsiTagFieldSet(FD_QOS_ISCSI_TAG_SELECTOR)
                                                != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext,  pStrErr_qos_IscsiPRECValueSet);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      if(usmDbIscsiVlanPrioritySet(FD_QOS_ISCSI_VPT_VALUE) != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiVlanPrioritySet);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      if(usmDbIscsiMarkingModeSet(FD_QOS_ISCSI_MARKING_ENABLED) != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiRemarkModeSet);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      } 
    }
    else
    {
      return cliSyntaxReturnPrompt(ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set aging time for iSCSI sessions.
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
* @cmdsyntax  iscsi aging time <time>
*             no iscsi aging time
*
* @cmdhelp Set aging time for iSCSI sessions.
*
* @cmddescript
*   This is a configurable value and can be set to certain values. 
*
* @end
*
*********************************************************************/
const L7_char8  *commandIscsiAgingTime(EwsContext ewsContext, 
                                       L7_uint32 argc, 
                                       const L7_char8 **argv, 
                                       L7_uint32 index)
{
  L7_uint32 argValue = 1;
  L7_uint32 timeValue=0;
  L7_uint32 numArgs;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArgs != 1)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiAgingTime);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArgs != 0)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiAgingTimeNo);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      cliConvertTo32BitUnsignedInteger(argv[index+ argValue],&timeValue);

      if(usmDbIscsiTimeOutIntervalSet(timeValue*60) != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, " ");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if(usmDbIscsiTimeOutIntervalSet(FD_QOS_ISCSI_SESSION_TIME_OUT_INTERVAL)
                                                             != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrErr_qos_IscsiAgingTimeSet);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      return cliSyntaxReturnPrompt(ewsContext, "");
    }
  }

    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}





