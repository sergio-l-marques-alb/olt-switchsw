/******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2009
 *
 ******************************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_llpf.c
 *
 * @purpose config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/02/2009
 *
 * @author  vijayanand K(kvijayan)
 *
 * @end
 *
 *****************************************************************************/
#include "cliapi.h"
#include "ews.h"
#include "ews_cli.h"
#include "cliutil.h"
#include "usmdb_llpf_api.h"
#include "usmdb_util_api.h"
#include "strlib_base_cli.h"
#include "osapi.h"

/******************************************************************************
*
* @purpose  Enables/Disables llpf on a port.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] llpf enable
*
* @cmdhelp Enables/Disables llpf on a port..
*
* @end
*
******************************************************************************/
static L7_RC_t cliUtilLlpfProtoValGet(const L7_char8 *protocolStr,L7_uint32 *protocolValue)
{
  L7_RC_t rc = L7_FAILURE;

 if (protocolStr != L7_NULL) 
 {
  rc = L7_SUCCESS;
  if(!(osapiStrncmp(pStrInfo_base_LlpfIsdp,protocolStr,strlen(pStrInfo_base_LlpfIsdp))))
  {
   *protocolValue = L7_LLPF_BLOCK_TYPE_ISDP;  
  }
  else if(!(osapiStrncmp(pStrInfo_base_LlpfVtp,protocolStr,strlen(pStrInfo_base_LlpfVtp))))
  {
   *protocolValue = L7_LLPF_BLOCK_TYPE_VTP;  
  }
  else if(!(osapiStrncmp(pStrInfo_base_LlpfDtp,protocolStr,strlen(pStrInfo_base_LlpfDtp))))
  {
   *protocolValue = L7_LLPF_BLOCK_TYPE_DTP;  
  }
  else if(!(osapiStrncmp(pStrInfo_base_LlpfUdld,protocolStr,strlen(pStrInfo_base_LlpfUdld))))
  {
   *protocolValue = L7_LLPF_BLOCK_TYPE_UDLD;  
  }
  else if(!(osapiStrncmp(pStrInfo_base_LlpfPagp,protocolStr,strlen(pStrInfo_base_LlpfPagp))))
  {
   *protocolValue = L7_LLPF_BLOCK_TYPE_PAGP;  
  }
  else if(!(osapiStrncmp(pStrInfo_base_LlpfSstp,protocolStr,strlen(pStrInfo_base_LlpfSstp))))
  {
   *protocolValue = L7_LLPF_BLOCK_TYPE_SSTP;  
  }
  else if(!(osapiStrncmp(pStrInfo_base_LlpfAll,protocolStr,strlen(pStrInfo_base_LlpfAll))))
  {
   *protocolValue = L7_LLPF_BLOCK_TYPE_ALL;  
  }
  else
  {
   rc = L7_FAILURE; 
  }
 }
 else
 {
  rc = L7_FAILURE;
 }
  return rc;
}
/******************************************************************************
*
* @purpose  Enables/Disables llpf on a port.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  [no] llpf enable
*
* @cmdhelp Enables/Disables llpf on a port..
*
* @end
*
******************************************************************************/
const L7_char8 *commandIntfLlpfEnable(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 **argv,
                                 L7_uint32 index)
{
  L7_uint32 unit,s,p,numArg,protocolValue = L7_NULL;
  L7_uint32 interface = L7_NULL,blockSeeker;
  L7_RC_t status = L7_SUCCESS;
  L7_char8  warningBuf[L7_CLI_MAX_STRING_LENGTH],protoBuff[L7_TYPE_NAME_SIZE];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  
  cliUtilLlpfProtoValGet((L7_char8 *)argv[numArg],&protocolValue);
   if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                   ewsContext, pStrInfo_common_NoValidPortsInBox_1);
    } 
    do
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
          usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
      {
         continue;
      }
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        /*******Check if the Flag is Set for Execution*************/  
        if(ewsContext->commType == CLI_NORMAL_CMD)
        {
              /* Do Set here */
          if(usmDbLlpfIntfBlockModeSet(interface, protocolValue,L7_TRUE) != L7_SUCCESS)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet, ewsContext,
                                     pStrInfo_base_LlpfEnabled);
            continue;
          }
        }    
        else if(ewsContext->commType == CLI_NO_CMD)
        {
           /* Do a Clear here */
          if(usmDbLlpfIntfBlockModeSet(interface, protocolValue,L7_FALSE) != L7_SUCCESS)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet, ewsContext,
                                                        pStrInfo_base_LlpfDisabled);
            continue;
          }
        }
      } 
    }while((L7_SUCCESS == usmDbValidIntIfNumNext(interface, &interface)));

    /*************Set Flag for Script Successful******/
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      /* warning message for set and unset */
       blockSeeker = strlen(pStrInfo_base_LlpfBlock);
      memset(warningBuf,0x00,sizeof(warningBuf)); 
      if(!(osapiStrncmp(pStrInfo_base_LlpfAll,argv[numArg],strlen(pStrInfo_base_LlpfAll))))
      {
         osapiSnprintf(warningBuf,L7_CLI_MAX_STRING_LENGTH,
                      "Warning: Configuring %s will block the PDUs (with a DMAC of 01:00:00:0C:CC:CX) from being forwarded\r\n",
                     argv[numArg]);  
      }
      else
      {
        /* This logic is to extract the protocol field in the cli arguments */
         memset(protoBuff,0x00,sizeof(protoBuff));
         osapiStrncpy(protoBuff,(argv[numArg] + blockSeeker),strlen((argv[numArg] + blockSeeker)));
         cliConvertToUpperCase(protoBuff);
         osapiSnprintf(warningBuf,L7_CLI_MAX_STRING_LENGTH,
                       "Warning: Configuring %s will block the %s PDUs from being forwarded\r\n",
                       argv[numArg],protoBuff);  
      }
      ewsTelnetWrite(ewsContext,warningBuf);  
    } 
    if (status == L7_SUCCESS)
    {
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    }
    return cliPrompt(ewsContext);
}
