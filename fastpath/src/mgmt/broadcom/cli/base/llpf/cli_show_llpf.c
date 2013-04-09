/******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2009
 *
 ******************************************************************************
 *
 * @filename cli_show_llpf.c
 *
 * @purpose show for the llpf
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/02/2009
 *
 * @author  Vijayanand K(kvijayan)
 *
 * @end
 *
 *****************************************************************************/
#include <string.h>

#include "strlib_base_cli.h"
#include "strlib_common_web.h"
#include "cliapi.h"
#include "ews.h"
#include "usmdb_llpf_api.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "local.h"
#include "usmdb_2233_stats_api.h"
#include "usmdb_common.h"

/******************************************************************************
*
* @purpose  Displays LLPF mode per interface.
* @param  ewsContext    @b{(input)} The current Context
* @param  argc          @b{(input)} argc
* @param  **argv        @b{(input)} argv
* @param index          @b{(input)} index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  show llpf interface all|([unit/]slot/port <cr>)
*
* @cmdhelp Displays LLPF block protocol mode(s) per interface.
*
* @end
*
******************************************************************************/
const L7_char8 * commandShowLlpfInterface(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 * * argv,
                                          L7_uint32 index)
{
  L7_uint32 unit, slot, port, counter, protocolCnt=L7_NULL,i;
  L7_uint32 mode = L7_DISABLE;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 numArg = 0;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32 intIfNum = 0;


  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);


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

  if (numArg == 2)
  {
      ewsTelnetWriteAddBlanks (1, 0, 7, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LlpfBlock_1);
      ewsTelnetWriteAddBlanks (1, 0, 7, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LlpfBlockProtocol);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LlpfBlockMode);
      ewsTelnetWrite(ewsContext,"\r\n------ -------- -------- -------- -------- -------- -------- ---------\r\n");

      if(strcmp(argv[index + numArg], pStrInfo_common_All) == 0)
      {
      counter = 0;
      while((rc == L7_SUCCESS) && (counter < CLI_MAX_SCROLL_LINES-6))
      {
        if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) != L7_SUCCESS)
        {
         ewsTelnetPrintf (ewsContext, "Interface:%s is out of range", cliDisplayInterfaceHelp(unit, slot, port));
         return cliPrompt(ewsContext);
        }  
       
        if(usmDbLlpfIntfIsValid(intIfNum) == L7_TRUE)
        {
          ewsTelnetPrintf (ewsContext, "%-7.6s",cliDisplayInterfaceHelp(unit, slot, port));
          for(protocolCnt =0;protocolCnt < L7_LLPF_BLOCK_TYPE_LAST ;protocolCnt++)
          {
            if(usmDbLlpfIntfBlockModeGet(intIfNum, protocolCnt, &mode) == L7_SUCCESS)
            {
              if(mode == L7_ENABLE)
              {
                ewsTelnetWriteAddBlanks(0,0,0,2,L7_NULLPTR,ewsContext,pStrInfo_common_Enbld);
	            } 
              else
	            {
                ewsTelnetWriteAddBlanks(0,0,0,1,L7_NULLPTR,ewsContext,pStrInfo_common_Dsbld);
	            }
            }
          }
          ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
        }
        rc = usmDbValidIntIfNumNext(intIfNum, &intIfNum);
      	if (rc != L7_SUCCESS)
	      { 
      	  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          intIfNum = 0;
          return cliPrompt(ewsContext);
        }
      	counter ++;
      }
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
       else
       {
         intIfNum = 0;
         return cliPrompt(ewsContext);
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
      
        if (usmDbLlpfIntfIsValid(intIfNum) != L7_TRUE)
        {
          intIfNum = 0;
          ewsTelnetWrite(ewsContext, pStrErr_base_LlpfCheckInvalidPort);
          ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
          return cliPrompt(ewsContext);
        }
        ewsTelnetPrintf (ewsContext, "%-7.6s",cliDisplayInterfaceHelp(unit, slot, port));
        for(protocolCnt =0; protocolCnt < L7_LLPF_BLOCK_TYPE_LAST ;protocolCnt++)
        {
      	  if(usmDbLlpfIntfBlockModeGet(intIfNum, protocolCnt, &mode) == L7_SUCCESS)
          {
            if(mode == L7_ENABLE)
	          {
              ewsTelnetWriteAddBlanks(0,0,0,2,L7_NULLPTR,ewsContext,pStrInfo_common_Enbld);
	          }
	          else
	          {
              ewsTelnetWriteAddBlanks(0,0,0,1,L7_NULLPTR,ewsContext,pStrInfo_common_Dsbld);
	          }
	        }
	        rc = L7_FAILURE;
        }	
        intIfNum = 0;
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      }
  } 
  return cliPrompt(ewsContext);
}
