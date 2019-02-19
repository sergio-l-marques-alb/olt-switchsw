/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  auto_install_debug.c
*
* @purpose   auto-install debug file
*
* @component auto_install
*
* @comments
*
* @create    21/01/2008
*
* @author    ipopov
*
* @end
*
**********************************************************************/

#include "auto_install_debug.h"
#include "auto_install_control.h"
#include "commdefs.h"
#include "sysapi.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* auto-install runtime data */
extern autoInstallRunTimeData_t  *autoInstallRunTimeData;

extern L7_uchar8 *autoInstallStateStrs[AUTO_INSTALL_MAX_STATE + 1];

/* auto-install debug enable/disable flags */
static L7_uint32 autoInstallDebugTraceCfg[AUTO_INSTALL_DBG_FLAG_LAST_ENTRY + 1];

/*********************************************************************
* @purpose  Check whether event flag is out of range
*
* @param    event_flag  @b{(input)} one of AUTO_INSTALL_TRACE_* flags
*           allowed in config
*
* @returns  L7_BOOL
*
* @notes    
*
* @end
*********************************************************************/
static L7_BOOL autoInstallDebugIsTraceEvent(L7_uint32 event_flag)
{    
  if((event_flag == AUTO_INSTALL_DBG_FLAG_FIRST_ENTRY) || (event_flag >= AUTO_INSTALL_DBG_FLAG_LAST_ENTRY))
  {
    sysapiPrintf("Auto Install debug event is out of range");
    return L7_FALSE;
  } 
      
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Record an auto-install event trace if trace configuration
*           permits 
*
* @param    event_flag  @b{(input)} one of AUTO_INSTALL_TRACE_* flags
*           allowed in config
* @param    format      @b{(input)} format string
* @param    ...         @b{(input)} additional arguments (per format
*           string)
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void autoInstallDebugTrace(L7_uint32 event_flag, L7_char8 * format, ...)
{
  va_list arg;
  if(autoInstallDebugIsTraceEvent(event_flag) == L7_FALSE)
  {
    return;
  }

  if( autoInstallDebugTraceCfg[event_flag] == L7_TRUE )
  {
    va_start (arg, format);
    vprintf(format, arg);
    va_end (arg);
  }    
}

/*********************************************************************
* @purpose  Set enabled events mask 
*
* @param    eventMask   @b{(input)} enabled events mask
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void autoInstallDebugTraceSet(L7_uint32 flag)
{  
  L7_uint32 i = AUTO_INSTALL_DBG_FLAG_FIRST_ENTRY + 1;
  
  if (flag == 0xFF)
  {
    for(; i < AUTO_INSTALL_DBG_FLAG_LAST_ENTRY; i++)
    {
      autoInstallDebugTraceCfg[i] = L7_TRUE;    
    }
    return;
  }
  
  if(autoInstallDebugIsTraceEvent(flag) == L7_TRUE)
  {
    autoInstallDebugTraceCfg[flag] = L7_TRUE;
  }
}

/*********************************************************************
* @purpose  Reset enabled events mask 
*
* @param    eventMask   @b{(input)} disabled events mask
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void autoInstallDebugTraceClear(L7_uint32 flag)
{  
  if (flag == 0xFF)
  {
    memset(autoInstallDebugTraceCfg, 0, sizeof(autoInstallDebugTraceCfg));
    return;
  }
  
  if(autoInstallDebugIsTraceEvent(flag) == L7_TRUE)
  {
    autoInstallDebugTraceCfg[flag] = L7_FALSE;
  }
}

/*********************************************************************
*
* @purpose  dump auto-install configuration
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallCfgDump()
{
  sysapiPrintf("\n");
  sysapiPrintf("AutoInstall Configuration\n");
  sysapiPrintf("===========================\n\n");

  sysapiPrintf("AutoInstall State    : %s\n", autoInstallRunTimeData->startStop == L7_TRUE ? "Started" : "Stopped");
  sysapiPrintf("AutoSave State       : %s\n", autoInstallRunTimeData->autoSave == L7_TRUE ? "Enabled" : "Disabled");
  sysapiPrintf("Unicast Retry-Count  : %d\n", autoInstallRunTimeData->unicastRetryCnt);
  sysapiPrintf("\n");

  sysapiPrintf("\n===========================\n");
  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  dump auto-install configuration
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallRunTimeDataDump()
{
  L7_uchar8 dispStr[IPV6_DISP_ADDR_LEN];
  L7_inet_addr_t inetIp;
  autoInstallRunTimeData_t  *autoInstallData = autoInstallRunTimeData;

  sysapiPrintf("\n");
  sysapiPrintf("AutoInstall RunTime Data.\n");
  sysapiPrintf("===========================\n\n");
  sysapiPrintf("Current State: %s\n", autoInstallStateStrs[autoInstallData->currentState]);
  sysapiPrintf("Current State: %s\n", (autoInstallData->isFirstCycle == L7_TRUE)? "TRUE" : "FALSE");
  sysapiPrintf("BootParams : %x \n", autoInstallData->bootConfigParams.offeredOptionsMask);
  sysapiPrintf("Specific File Retry Count : %d\n", autoInstallData->specificFileRetryCounter);
  sysapiPrintf("Predefined File Retry Count : %d\n", autoInstallData->predefinedFileRetryCounter);
  sysapiPrintf("Host File Retry Count : %d\n", autoInstallData->hostFileRetryCounter);
  inetAddressSet(L7_AF_INET, &autoInstallData->tftpIpAddress, &inetIp);
  sysapiPrintf("TFTP IP address : %s\n", inetAddrPrint(&inetIp,dispStr));
  sysapiPrintf("Boot File name : %s\n", autoInstallData->bootFileName);
  sysapiPrintf("Host File name : %s\n", autoInstallData->hostFileName);
  inetAddressSet(L7_AF_INET, &autoInstallData->currentIpAddress, &inetIp);
  sysapiPrintf("Current IP address : %s\n", inetAddrPrint(&inetIp,dispStr));
  sysapiPrintf("Current File name : %s\n", autoInstallData->currentFileName);
  sysapiPrintf("\n");
  sysapiPrintf("\n===========================\n");
  
  return L7_SUCCESS;
}

