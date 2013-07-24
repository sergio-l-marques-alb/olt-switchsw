/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
* @filename  llpf_cfg.c
*
* @purpose   LLPF Configuration file.
*
* @component LLPF
*
* @comments none
*
* @create 10/08/2009
*
* @author Vijayanand K(kvijayan)
*
* @end
*
**********************************************************************/

/* Common header file includes */
#include "l7_common.h"
#include "osapi.h"
#include "log.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "comm_mask.h"
#include "nvstoreapi.h"

/* Component related header file includes */
#include "llpf_cnfgr.h"
#include "llpf_cfg.h"
#include "llpf_util.h"

extern llpfCfgData_t *llpfCfgData;

/*********************************************************************
* @purpose  Build default llpf config data
*
* @param    ver   @{{input}} Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void llpfBuildDefaultConfigData(L7_uint32 ver)
{

 L7_uint32 cfgIndex;

 /* Loop through all the interfaces */

  memset((char*)llpfCfgData, 0, sizeof(llpfCfgData_t));

  for (cfgIndex = 1; cfgIndex < L7_LLPF_MAX_INTF; cfgIndex++)
  {
    /* Loop through all the protocols for a given interface */
      llpfBuildIntfDefaultConfigData(cfgIndex);
  }

  /* Build header */
  strcpy(llpfCfgData->cfgHdr.filename,LLPF_CFG_FILENAME);
  llpfCfgData->cfgHdr.version = ver;
  llpfCfgData->cfgHdr.componentID = L7_LLPF_COMPONENT_ID;
  llpfCfgData->cfgHdr.type = L7_CFG_DATA;
  llpfCfgData->cfgHdr.length = sizeof(llpfCfgData_t);
  llpfCfgData->cfgHdr.dataChanged = L7_FALSE;

}
/*********************************************************************
* @purpose  Build default llpf Interface config data
*
* @param    ver   @{{input}}Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void llpfBuildIntfDefaultConfigData(L7_uint32 intfNum)
{

  L7_uint32 protocol;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intfNum, L7_SYSNAME, ifName);

    /* Check whether a valid LLPF Interface is specified for default config */
    if(llpfIntfIsValid(intfNum) != L7_TRUE)
    {
      return;
    }
    for(protocol = L7_LLPF_BLOCK_TYPE_ISDP;
        protocol < L7_LLPF_BLOCK_TYPE_LAST; protocol++)
    {

      switch(protocol)
      {
        case L7_LLPF_BLOCK_TYPE_ISDP:

          if(FD_LLPF_DEFAULT_ISDP_MODE == L7_ENABLE)
          {
            llpfIntfFilterModeSet(intfNum,L7_LLPF_BLOCK_TYPE_ISDP);
          }
          else
          {
            llpfIntfFilterModeClear(intfNum,L7_LLPF_BLOCK_TYPE_ISDP);
          }
       break;

       case L7_LLPF_BLOCK_TYPE_VTP:
         if(FD_LLPF_DEFAULT_VTP_MODE == L7_ENABLE)
         {
           llpfIntfFilterModeSet(intfNum,L7_LLPF_BLOCK_TYPE_VTP);
         }
         else
         {
           llpfIntfFilterModeClear(intfNum,L7_LLPF_BLOCK_TYPE_VTP);
         }
       break;

      case L7_LLPF_BLOCK_TYPE_DTP:
        if(FD_LLPF_DEFAULT_DTP_MODE == L7_ENABLE)
        {
          llpfIntfFilterModeSet(intfNum,L7_LLPF_BLOCK_TYPE_DTP);
        }
        else
        {
          llpfIntfFilterModeClear(intfNum,L7_LLPF_BLOCK_TYPE_DTP);
        }
      break;

      case L7_LLPF_BLOCK_TYPE_UDLD:
        if(FD_LLPF_DEFAULT_UDLD_MODE == L7_ENABLE)
        {
          llpfIntfFilterModeSet(intfNum,L7_LLPF_BLOCK_TYPE_UDLD);
        }
        else
        {
          llpfIntfFilterModeClear(intfNum,L7_LLPF_BLOCK_TYPE_UDLD);
        }
        break;

      case L7_LLPF_BLOCK_TYPE_PAGP:
        if(FD_LLPF_DEFAULT_PAGP_MODE == L7_ENABLE)
        {
          llpfIntfFilterModeSet(intfNum,L7_LLPF_BLOCK_TYPE_PAGP);
        }
        else
        {
          llpfIntfFilterModeClear(intfNum,L7_LLPF_BLOCK_TYPE_PAGP);
        }
       break;

      case L7_LLPF_BLOCK_TYPE_SSTP:
        if(FD_LLPF_DEFAULT_SSTP_MODE == L7_ENABLE)
        {
          llpfIntfFilterModeSet(intfNum,L7_LLPF_BLOCK_TYPE_SSTP);
        }
        else
        {
          llpfIntfFilterModeClear(intfNum,L7_LLPF_BLOCK_TYPE_SSTP);
        }
      break;

      case L7_LLPF_BLOCK_TYPE_ALL:
        if(FD_LLPF_DEFAULT_ALL_MODE == L7_ENABLE)
        {
          llpfIntfFilterModeSet(intfNum,L7_LLPF_BLOCK_TYPE_ALL);
        }
        else
        {
          llpfIntfFilterModeClear(intfNum,L7_LLPF_BLOCK_TYPE_ALL);
        }
       break;

       default:
         L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_LLPF_COMPONENT_ID,
                "\r\n Invalid Protocol ID has been specified for Interface:%s,ID:%d\r\n",
                 ifName, protocol);
        break;
      }
    }
}

/*********************************************************************
* @purpose  Checks if LLPF user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL llpfHasDataChanged(void)
{
  return llpfCfgData->cfgHdr.dataChanged;
}

/*********************************************************************
* @purpose  Restores  LLPF user config data change Flag
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/

L7_BOOL llpfResetDataChanged(void)
{
  llpfCfgData->cfgHdr.dataChanged = L7_FALSE;
  return llpfCfgData->cfgHdr.dataChanged;
}

/*********************************************************************
* @purpose  Saves LLPF user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t llpfSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (llpfCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    llpfCfgData->cfgHdr.dataChanged = L7_FALSE;
    llpfCfgData->checkSum = nvStoreCrc32((L7_char8 *)llpfCfgData,
                                           sizeof(llpfCfgData_t) - sizeof(llpfCfgData->checkSum));

    if (sysapiCfgFileWrite(L7_LLPF_COMPONENT_ID,
                           LLPF_CFG_FILENAME,
                           (L7_char8 *)llpfCfgData,
                           sizeof(llpfCfgData_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_LLPF_COMPONENT_ID,
      "\r\n Failed to write LLPF configi to file %s \r \n",LLPF_CFG_FILENAME);
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Restores LLPF user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 llpfRestore(void)
{

  llpfBuildDefaultConfigData(llpfCfgData->cfgHdr.version);
  /* Implement a Apply Default Config Data if needed */

  llpfCfgData->cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}

