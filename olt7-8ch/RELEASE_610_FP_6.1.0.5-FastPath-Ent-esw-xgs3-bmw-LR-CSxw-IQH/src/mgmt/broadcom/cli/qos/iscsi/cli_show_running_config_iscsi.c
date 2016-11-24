/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/cos/cli_show_running_config_iscsic.c
 * @purpose show running config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  11/24/2004
 *
 * @author  Rajakrishna
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "iscsi_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "config_script_api.h"
#include "datatypes.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_show_running_config.h"
#include "usmdb_qos_iscsi.h"

/*********************************************************************
* @purpose  To print the running configuration of iscsi  info
*
* @param    EwsContext ewsContext
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningGlobalConfigIscsiInfo(EwsContext ewsContext)
{
  L7_uint32 mode;
  L7_uint32 entryId=0;
  L7_uint32 nextEntryId=0;
  L7_char8  strAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  targetName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 tcpPort=0;
  L7_uint32 agingTime;
  L7_inet_addr_t targetIpAddress;
  L7_QOS_COS_MAP_INTF_MODE_t cosType;
  L7_uint32 cosVal, defCosVal;
  L7_uint32 rc;
  L7_uint32 remark;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  
  /* Administrative Mode */
  if (usmDbIscsiAdminModeGet(&mode) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,mode,FD_QOS_ISCSI_ADMIN_MODE,pStrInfo_qos_IscsiEnableCmd);
  }

  /* iscsi targets and ports */
  /* 
   * do this in two passes - first pass is to find any default config entries that have 
   * been removed from active config... need to do removals first so we have all the table 
   * entries available when we start adding non-default entries or we may run out of space
   * before we fit in a valid config 
   */
  rc = usmDbIscsiTargetTableFirstGet(&entryId, L7_TRUE);
  while(rc == L7_SUCCESS)
  {
    L7_BOOL entryIsDefault, entryIsDeletedDefault; 

    entryIsDefault = usmDbIscsiTargetTableIsEntryInDefaultConfig(entryId);
    entryIsDeletedDefault = usmDbIscsiTargetTableEntryDeletedDefaultConfigFlagGet(entryId);

    if ((entryIsDefault == L7_TRUE) && (entryIsDeletedDefault == L7_TRUE))
    {
      if(usmDbIscsiTargetTableEntryTcpPortGet(entryId,&tcpPort) == L7_SUCCESS)
      {
  memset(buf, 0, sizeof(buf)); 
        osapiSnprintf(buf,sizeof(buf), "%s ", pStrInfo_common_No_1);

        memset(stat, 0, sizeof(stat)); 
        osapiSnprintf(stat, sizeof(stat),"%s %s %s %u",pStrInfo_qos_Iscsi_1,pStrInfo_qos_IscsiTarget,pStrInfo_qos_IscsiPort,tcpPort);
        strcat(buf,stat); 

        /* currently the no form of this command requires the address field even if address is not configured TODO */
        if(usmDbIscsiTargetTableEntryIpAddressGet(entryId,&targetIpAddress)==L7_SUCCESS)
        {
          if (targetIpAddress.family == L7_AF_INET)
          {
            inetAddrHtop(&targetIpAddress, strAddr);
  memset(stat, 0, sizeof(stat)); 
            osapiSnprintf(stat,sizeof(stat), " %s %s", pStrInfo_qos_IscsiAddress,strAddr);
            strcat(buf,stat); 
          }
        }

        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
    rc = usmDbIscsiTargetTableNextGet(entryId, &nextEntryId, L7_TRUE);
    entryId = nextEntryId;
  }/*End-of-While-Loop*/

  /* now do it again to add all the non-default entries*/
  rc = usmDbIscsiTargetTableFirstGet(&entryId, L7_FALSE);
  while(rc == L7_SUCCESS)
  {
    L7_BOOL entryIsDefault; 

    entryIsDefault = usmDbIscsiTargetTableIsEntryInDefaultConfig(entryId);

    /* don't include default entries in text-based config */
    if (entryIsDefault == L7_FALSE)
    {
      if(usmDbIscsiTargetTableEntryTcpPortGet(entryId,&tcpPort) ==L7_SUCCESS)
      {
        memset(buf, 0, sizeof(buf)); 
        memset(stat, 0, sizeof(stat)); 
        osapiSnprintf(stat, sizeof(stat),"%s %s %s %u",pStrInfo_qos_Iscsi_1,pStrInfo_qos_IscsiTarget,pStrInfo_qos_IscsiPort,tcpPort);
        strcat(buf,stat); 

        if(usmDbIscsiTargetTableEntryIpAddressGet(entryId,&targetIpAddress)==L7_SUCCESS)
        {
          if((targetIpAddress.family == L7_AF_INET) && (targetIpAddress.addr.ipv4.s_addr))
          {
            inetAddrHtop(&targetIpAddress, strAddr);
            memset(stat, 0, sizeof(stat)); 
            osapiSnprintf(stat,sizeof(stat)," %s %s",pStrInfo_qos_IscsiAddress,strAddr);
            strcat(buf,stat); 
          }
        }

        if(usmDbIscsiTargetTableEntryTargetNameGet(entryId,targetName)==L7_SUCCESS)
        {
          if(strlen(targetName))
          {
            memset(stat, 0, sizeof(stat)); 
            osapiSnprintf(stat,sizeof(stat)," %s %s",pStrInfo_qos_IscsiName,targetName);
            strcat(buf,stat); 
          }
        }
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
    rc = usmDbIscsiTargetTableNextGet(entryId, &nextEntryId, L7_TRUE);
    entryId = nextEntryId;
  }/*End-of-While-Loop*/

  /* iscsi cos {vpt <1-7>|dscp <0-63>} [remark] */
  memset(buf, 0, sizeof(buf)); 
  memset(stat, 0, sizeof(stat)); 
  if(usmDbIscsiTagFieldGet(&cosType)==L7_SUCCESS)
  {
    if((cosType == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P) || (cosType == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP ))
    {
      /* Get the default CosValue */
      switch(FD_QOS_ISCSI_TAG_SELECTOR)
      {
        case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
             defCosVal = FD_QOS_ISCSI_VPT_VALUE;
             break;
        case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
             defCosVal = FD_QOS_ISCSI_DSCP_VALUE;
             break; 
        case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
             defCosVal = FD_QOS_ISCSI_PREC_VALUE;
             break; 
        default:
             defCosVal = FD_QOS_ISCSI_VPT_VALUE;
             break; 
      }/*End-of-switch*/
      /* Get the configured TAG selector and the cos-value. */
      osapiSnprintf(buf,sizeof(buf),"%s %s",pStrInfo_qos_Iscsi_1,pStrInfo_qos_IscsiCos);
      if(cosType == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        if(usmDbIscsiDscpGet(&cosVal) == L7_SUCCESS)
        {
          osapiSnprintf(stat,sizeof(stat)," %s %u",pStrInfo_qos_IscsiDscp,cosVal);
          strcat(buf,stat); 
        }
      }
      else
      {
        if(usmDbIscsiVlanPriorityGet(&cosVal) ==L7_SUCCESS)
        {
          osapiSnprintf(stat,sizeof(stat)," %s %u",pStrInfo_qos_IscsiVpt,cosVal);
          strcat(buf,stat); 
        }
      }
      /* Getting Remark */
      if(usmDbIscsiMarkingModeGet(&remark) == L7_SUCCESS)
      {
        if (remark != FD_QOS_ISCSI_MARKING_ENABLED || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          memset(stat, 0, sizeof(stat)); 
          osapiSnprintf(stat,sizeof(stat)," %s",pStrInfo_qos_IscsiRemark);
          strcat(buf,stat); 
        }
      }
        
      if ((cosType != FD_QOS_ISCSI_TAG_SELECTOR) || (cosVal != defCosVal) || (remark != FD_QOS_ISCSI_MARKING_ENABLED) || 
          (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
  }
  
  /* iscsi aging-time <1-43200> */ 
  memset (buf, 0, sizeof(buf)); 
  if(usmDbIscsiTimeOutIntervalGet(&agingTime) == L7_SUCCESS)
  {
    if (agingTime != FD_QOS_ISCSI_SESSION_TIME_OUT_INTERVAL || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),pStrInfo_qos_IscsiAgingTimeCmd, (agingTime/60));
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }
  
  return L7_SUCCESS;
}



