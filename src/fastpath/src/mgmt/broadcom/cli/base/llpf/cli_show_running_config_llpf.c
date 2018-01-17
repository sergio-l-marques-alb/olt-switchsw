/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2009
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_llpf.c
 *
 * @purpose show running config commands for llpf
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/12/2009
 *
 * @author  Vijayanand K (kvijayan)
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "usmdb_util_api.h"
#include "usmdb_llpf_api.h"
#include "defaultconfig.h"
#include "ews.h"
#include "ews_cli.h"

#include "cliutil.h"
#include "cli_show_running_config.h"
#include "clicommands_card.h"

/*********************************************************************
 * @purpose  To print the running configuration of LLPF Interface mode
 *
 * @param    EwsContext ewsContext    @b((input)) The Current Context   
 * @param    L7_uint32  interface     @b((input)) interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
static L7_RC_t cliLlpfUtilRunningConfigInfoGet(L7_uint32 interface,L7_uint32 protocol,
                            L7_char8 *bufPtr,L7_uint32 bufPtrLen)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 protoMode;

  if(bufPtr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  
  if(usmDbLlpfIntfBlockModeGet(interface, protocol, &protoMode) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

   switch(protocol)
   {
     case L7_LLPF_BLOCK_TYPE_ISDP:
      if( FD_LLPF_DEFAULT_ISDP_MODE != protoMode)
      {
        if(protoMode != L7_ENABLE)
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_noLlpfBlock,pStrInfo_base_LlpfIsdp);
        }
        else
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_LlpfBlkProto,pStrInfo_base_LlpfIsdp);
        }
      }
      break;

     case L7_LLPF_BLOCK_TYPE_VTP:
      if( FD_LLPF_DEFAULT_VTP_MODE != protoMode)
      {
        if(protoMode != L7_ENABLE)
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_noLlpfBlock,pStrInfo_base_LlpfVtp);
        }
        else
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_LlpfBlkProto,pStrInfo_base_LlpfVtp);
        }
      }
     break;

     case L7_LLPF_BLOCK_TYPE_DTP:
      if( FD_LLPF_DEFAULT_DTP_MODE != protoMode)
      {
        if(protoMode != L7_ENABLE)
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_noLlpfBlock,pStrInfo_base_LlpfDtp);
        }
        else
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_LlpfBlkProto,pStrInfo_base_LlpfDtp);
        }
      }
     break;

     case L7_LLPF_BLOCK_TYPE_UDLD:
      if( FD_LLPF_DEFAULT_UDLD_MODE != protoMode)
      {
        if(protoMode != L7_ENABLE)
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_noLlpfBlock,pStrInfo_base_LlpfUdld);
        }
        else
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_LlpfBlkProto,pStrInfo_base_LlpfUdld);
        }
      }
     break;

     case L7_LLPF_BLOCK_TYPE_PAGP:
      if( FD_LLPF_DEFAULT_PAGP_MODE != protoMode)
      {
        if(protoMode != L7_ENABLE)
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_noLlpfBlock,pStrInfo_base_LlpfPagp);
        }
        else
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_LlpfBlkProto,pStrInfo_base_LlpfPagp);
        }
      }
      break;

     case L7_LLPF_BLOCK_TYPE_SSTP:
      if( FD_LLPF_DEFAULT_SSTP_MODE != protoMode)
      {
        if(protoMode != L7_ENABLE)
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_noLlpfBlock,pStrInfo_base_LlpfSstp);
        }
        else
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_LlpfBlkProto,pStrInfo_base_LlpfSstp);
        }
      }
      break;

     case L7_LLPF_BLOCK_TYPE_ALL:
      if( FD_LLPF_DEFAULT_ALL_MODE != protoMode)
      {
        if(protoMode != L7_ENABLE)
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_noLlpfBlock,pStrInfo_base_LlpfAll);
        }
        else
        {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufPtr, bufPtrLen, 
                         pStrInfo_base_LlpfBlkProto,pStrInfo_base_LlpfAll);
        }
      }
      break;

     default:
       rc = L7_FAILURE;
       break;
   }
  return rc;
} 
/*********************************************************************
 * @purpose  To print the running configuration of LLPF Interface mode
 *
 * @param    EwsContext ewsContext    @b((input)) The current context
 * @param    L7_uint32  interface     @b((input)) interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningInterfaceConfigLlpfInfo(EwsContext ewsContext, L7_uint32 interface)
{
  L7_uint32 protoIndex;
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbLlpfIntfIsValid (interface) != L7_TRUE)
  {
    return L7_SUCCESS;
  }
  for(protoIndex =L7_LLPF_BLOCK_TYPE_ISDP;protoIndex < L7_LLPF_BLOCK_TYPE_LAST ;protoIndex++)
  {
    /* memset the buffer first */
    memset(buf,0x00,sizeof(buf));
    rc = cliLlpfUtilRunningConfigInfoGet(interface,protoIndex,buf,sizeof(buf));  
    EWSWRITEBUFFER(ewsContext, buf);
  }
  return L7_SUCCESS;
}
