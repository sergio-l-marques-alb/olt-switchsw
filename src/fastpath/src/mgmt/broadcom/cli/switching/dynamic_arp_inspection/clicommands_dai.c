
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_dai.c
*
* @purpose Dynamic ARP Inspection commands
*
* @component Dynamic ARP Inspection
*
* @comments contains the code to build the dai CLI tree
* @comments 
*
* @create  09/17/2007
*
* @author  Kiran Kumar Kella
* @end
*
*********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cli_mode.h"
#include "clicommands_dai.h"
#include "usmdb_dai_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"

#ifdef L7_STACKING_PACKAGE
#include "clicommands_stacking.h"
#endif

/*********************************************************************
*
* @purpose  Build the tree node for DAI Global config mode
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalDai(EwsCliCommandP depth4)
{
  EwsCliCommandP depth5, depth6, depth7, depth8, depth9, depth10;
  L7_char8  vlanRangeHelp[L7_CLI_MAX_LARGE_STRING_LENGTH];

  sprintf(vlanRangeHelp, pStrInfo_base_VlanRange_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* ip arp inspection validate {[src-mac] [dst-mac] [ip]} */
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Validate, 
                         pStrInfo_base_DaiValidate,
                         commandDaiValidateOptions, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_DstMac,
                         pStrInfo_base_DaiDstMac,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Ip_2,
                         pStrInfo_base_DaiIp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Ip_2,
                         pStrInfo_base_DaiIp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_SrcMac,
                         pStrInfo_base_DaiSrcMac,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_DstMac,
                         pStrInfo_base_DaiDstMac,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Ip_2,
                         pStrInfo_base_DaiIp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Ip_2,
                         pStrInfo_base_DaiIp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* ip arp inspection vlan <vlan-list> */
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_VlanKeyword, 
                         pStrInfo_base_DaiVlan,
                         commandDaiVlan,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_VlanList_1, vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* ip arp inspection vlan <vlan-list> logging */
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LoggingInvalid,
                         pStrInfo_base_DaiVlanLogging,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* ip arp inspection filter <acl-name> vlan <vlan-list> [static] */
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Filter,
                         pStrInfo_base_DaiVlanAclFilter,
                         commandDaiVlanArpAclFilter, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Name,
                         pStrInfo_base_ArpAccessListName,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                         pStrInfo_base_DaiVlanAclFilter,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_VlanList_1, vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode(depth8, pStrInfo_base_AclStatic, pStrInfo_base_DaiAclStatic,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                          NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree node for DAI Interface commands
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeIfDai(EwsCliCommandP depth4)
{
  EwsCliCommandP depth5, depth6, depth7, depth8, depth9, depth10, depth11;

  /* ip arp inspection trust */
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Trust, pStrInfo_base_DaiTrust, 
                         commandDaiIfTrust, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  
  /* ip arp inspection limit {rate <pps> [burst interval <seconds>] | none} */
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Limit, pStrInfo_base_DaiRateLimit, 
                         commandDaiIfRateLimit, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Rate, pStrInfo_base_DaiRate, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_Range0to300, pStrInfo_base_DaiRate0To300,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_Burst, pStrInfo_base_DaiBurst, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_base_Interval, pStrInfo_base_DaiBurst, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(depth9, pStrInfo_base_Range1to15, pStrInfo_base_DaiBurst1To15,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  depth11 = ewsCliAddNode(depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                          NULL, L7_NO_OPTIONAL_PARAMS);
  
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LimitNone, pStrInfo_base_DaiRateNone,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree node for DAI Show
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes
*
* @end
*
*********************************************************************/
void buildTreeShowDai(EwsCliCommandP depth5)
{
  EwsCliCommandP depth6, depth7, depth8, depth9;
  L7_char8  vlanRangeHelp[L7_CLI_MAX_LARGE_STRING_LENGTH];

  sprintf(vlanRangeHelp, pStrInfo_base_VlanRange_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* interfaces */
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Intfs_1,
                         pStrInfo_base_DaiIfShow,
                         commandShowDaiIf, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* <u/s/p> */
  depth7 = buildTreeInterfaceHelp(depth6, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, 
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* vlan */
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_VlanKeyword, pStrInfo_base_DaiVlanShow,
                         commandShowDaiVlan, L7_NO_OPTIONAL_PARAMS);
  /* <vlan-list> */
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_VlanList_1, vlanRangeHelp,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* statistics */
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Statistics, pStrInfo_base_DaiStatsShow,
                         commandShowDaiStatsBrief, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* vlan */
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword, pStrInfo_base_DaiVlanStatsShow,
                         commandShowDaiStatsVlan, L7_NO_OPTIONAL_PARAMS);
  /* <vlan-list> */
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_VlanList_1, vlanRangeHelp,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the DAI clear commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeClearDai(EwsCliCommandP depth5)
{
  /* depth5 = clear ip arp inspection */
  EwsCliCommandP depth6, depth7;

  /* clear ip arp inspection statistics */
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Statistics, pStrInfo_base_DaiStatsClear, 
                         commandClearDaiStats, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

}


/*********************************************************************
*
* @purpose  To build the ARP ACL Menu
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreeArpAccessListConfig()
{
  EwsCliCommandP depth1, depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliArpAccessListMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_ARP_ACL_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode (depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS );
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );

  /* command  : permit ip host <sender-ip> mac host <sender-mac> */
  depth2 = ewsCliAddNode (depth1, pStrInfo_base_Permit, pStrInfo_base_ArpAclPermit,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Ip_2, pStrInfo_base_ArpAclIp,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_base_Host_4, pStrInfo_base_ArpAclIp,
                          commandArpAclPermitRule, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_base_SenderIp, pStrInfo_base_ArpAclIp, NULL, 4,
                          L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode (depth5, pStrInfo_base_AclMac, pStrInfo_base_ArpAclMac, NULL, 2,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode (depth6, pStrInfo_base_Host_4, pStrInfo_base_ArpAclMac,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_SenderMac, pStrInfo_base_ArpAclMac, NULL, 4,
                         L7_NODE_DATA_TYPE, L7_MAC_ADDRESS_SVL_DATA_TYPE, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode (depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
}

/*********************************************************************
*
* @purpose  To build the ARP ACL mode
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreeGlobalArpAccessList(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* depth2 = "arp " */

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_ArpAcl, pStrInfo_base_ArpAccessList,
                         cliArpAccessListMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_base_ArpAccessListName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_TYPE_MODE, L7_ARP_ACL_CONFIG_MODE);
}

/*********************************************************************
*
* @purpose Function to set or exit from the ARP ACL mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliArpAccessListMode(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 **argv, L7_uint32 index)
{
  return cliArpAccessListPrompt(index+1, pStrInfo_base_ArpAclCfgModePrompt,
                                pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the ARP ACL mode.
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmddescript  Enters the arp-access-list config mode.
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliArpAccessListPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                       L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                       const L7_char8 **argv, EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_uint32 unit, rc;
  L7_char8 strName[L7_ARP_ACL_NAME_LEN_MAX+1];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType != CLI_NO_CMD)
  {
    prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv,
                                                  ewsContext,cliGetMode(L7_GLOBAL_CONFIG_MODE));
    if (prompt != NULL)
    {
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return prompt;
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ((argc == depth + 1) &&
      (mainMenu[cliCurrentHandleGet()] !=cliGetMode(L7_ARP_ACL_CONFIG_MODE)))
  {

    if (strlen(argv[depth]) > L7_ARP_ACL_NAME_LEN_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ArpAclNameNameStringError);
    }

    osapiStrncpySafe(strName,argv[depth], sizeof(strName));

    /* Check to see if ARP ACL exists */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Does not exist - create and add */
        if (usmDbArpAclGet(strName) != L7_SUCCESS)
        {
          rc = usmDbArpAclCreate(strName);
          if (rc != L7_SUCCESS)
          {
            if (rc == L7_TABLE_IS_FULL)
            {
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, 
                                                     pStrInfo_MaxOfArpAclsIsAlreadyCfgured,
                                                     L7_ARP_ACL_CMDS_MAX);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                                     pStrInfo_ArpAcl_UnableToCreate);
            }
          }
        }
      } /* End of if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT) */

      strcpy(EWSARPACLNAME(ewsContext),strName);

      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_ARP_ACL_CONFIG_MODE));
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbArpAclGet(strName) == L7_SUCCESS)
        {
          if (usmDbArpAclDelete(strName) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,
                                                   ewsContext, pStrErr_ArpAccessListDelFail);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,
                                                 ewsContext, pStrErr_ArpAccessListDoesntExist);
        }
      } /* End of if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT) */

      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    cliSyntaxBottom(ewsContext);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return NULL;
}

