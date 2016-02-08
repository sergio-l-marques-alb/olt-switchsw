/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_dhcp_snooping.c
*
* @purpose DHCP Snooping commands
*
* @component DHCP Snooping
*
* @comments contains the code to build the dhcp snooping config tree
* @comments 
*
* @create  03/16/2007
*
* @author  rrice
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "clicommands_dhcp_snooping.h"
#include "usmdb_dhcp_snooping.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "snooping_exports.h"

#ifdef L7_STACKING_PACKAGE
#include "clicommands_stacking.h"
#endif

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
*
* @purpose  Build the tree node for IPSG Global config mode
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
void buildTreeGlobalIpsg(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8, depth9;
  L7_uchar8 vlanRange[32];

  depth3 = ewsCliAddNode(depth2, "binding", "Add a static IPSG entry",
                         commandIpsgEntry,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Macaddr, pStrInfo_common_CfgNetMacAddr,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_VlanKeyword, "Specify VLAN ID",
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
  depth6 = ewsCliAddNode(depth5, vlanRange, "A single VLAN ID",
                         NULL, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr,
                         NULL, 4,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                            L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth8 = ewsCliAddNode(depth7, "interface", "Specify interface",
                         NULL, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = buildTreeInterfaceHelp(depth8, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_BOTH);
  ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}
#endif




/*********************************************************************
*
* @purpose  Build the tree node for DHCP Snooping Global config mode
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
void buildTreeGlobalDhcpSnooping(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9;
  L7_uchar8 vlanRange[32];
  L7_char8  vlanRangeHelp[L7_CLI_MAX_LARGE_STRING_LENGTH];
  sprintf(vlanRangeHelp, pStrInfo_base_VlanRange_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);


  /* ip dhcp snooping */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Snooping1, 
                         pStrInfo_base_DhcpSnoopingGlobal, 
                         commandIpDhcpSnooping, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifdef L7_IPSG_PACKAGE
  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_DB_PERSISTENCY)) == L7_TRUE)
  {
    /* ip dhcp snooping database */

    depth3 = ewsCliAddNode(depth2, "database",
                           pStrInfo_base_DhcpSnooping_db_url,
                         commandIpDhcpSnoopingDbUrl, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);    
    
    /* ip dhcp snooping database <url> */

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_db_url,
                           pStrInfo_base_DhcpSnooping_db_url,
                           NULL, L7_NO_OPTIONAL_PARAMS);

    ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_db_local,
                           pStrInfo_base_DhcpSnooping_db_local,
                           NULL, L7_NO_OPTIONAL_PARAMS);


    ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

    /* ip dhcp snooping database write-delay <value> */
  
    depth4 =  ewsCliAddNode(depth3, "write-delay",
                           pStrInfo_base_DhcpSnooping_db_store_interval,
                           commandIpDhcpSnoopingDbStoreInterval,2,L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, "<interval>",
                           pStrInfo_base_DhcpSnooping_db_store_interval,
                           NULL,
                           L7_NO_OPTIONAL_PARAMS); 
       

    ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  }
#endif
  

  /* ip dhcp snooping verify mac-address */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Verify, 
                         pStrInfo_base_DhcpSnoopingVerifyMac,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_MacAddress,
                         pStrInfo_base_DhcpSnoopingVerifyMac,
                         commandIpDhcpSnoopingVerifyMac, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* ip dhcp snooping vlan <vlan-range> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_VlanKeyword, 
                         pStrInfo_base_DhcpSnoopingVlan,
                         commandIpDhcpSnoopingVlan, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3,pStrInfo_common_VlanList_1,
                          vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* binding */
  depth3 = ewsCliAddNode(depth2, "binding", "Add a static DHCP snooping binding",
                         commandIpDhcpSnoopingBinding,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Macaddr, pStrInfo_common_CfgNetMacAddr,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_VlanKeyword, "Specify VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
  depth6 = ewsCliAddNode(depth5, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, 
                         NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth8 = ewsCliAddNode(depth7, "interface", "Specify interface", 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = buildTreeInterfaceHelp(depth8, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, 
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                          NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the tree node for DHCP Snooping Interface commands
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
void buildTreeIfIpDhcpSnoopingTrust(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8;
  EwsCliCommandP depth9;

  /* ip dhcp snooping */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Snooping1, pStrInfo_base_DhcpSnoopingIntf, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);

  /* log-invalid */
  depth4 = ewsCliAddNode(depth3, "log-invalid", pStrInfo_base_DhcpSnoopingLogInvalid,
                         commandIpDhcpSnoopingLogInvalid,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* trust */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Trust, pStrInfo_base_DhcpSnoopingTrust, 
                         commandIpDhcpSnoopingTrust, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* ip dhcp snooping limit {rate <pps> [burst interval <seconds>] | none} */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Limit, pStrInfo_base_DsRateLimit,
                         commandIpDhcpSnoopingIfRateLimit,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Rate, pStrInfo_base_DsRate,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Range0to300, pStrInfo_base_DsRate0To300,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_Burst, pStrInfo_base_DsBurst,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_Interval, pStrInfo_base_DsBurst,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_base_Range1to15, pStrInfo_base_DsBurst1To15,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LimitNone, pStrInfo_base_DsRateNone,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

}
#ifdef L7_IPSG_PACKAGE
/*********************************************************************
*
* @purpose  Build the tree node for IP Source Guard
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes    ip verify source [port-security]
*           no ip verify source
*
* @end
*
*********************************************************************/
void buildTreeIpsgVerifySource(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  /* ip verify source */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Verify, pStrInfo_base_IpsgVerifySource, 
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   depth4 = ewsCliAddNode(depth3, "source", pStrInfo_base_IpsgVerifySource, 
                          commandIpVerifySource, 
                          2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   depth5 = ewsCliAddNode(depth4, "port-security", pStrInfo_base_IpsgVerifySourceMac,
                          commandIpVerifySourcePortSecurity, 
                          L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                          NULL, L7_NO_OPTIONAL_PARAMS);
}
#endif

/*********************************************************************
*
* @purpose  Build the tree node for DHCP snooping Show
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes    show ip dhcp snooping 
*
* @end
*
*********************************************************************/
void buildTreeShowIpDhcpSnooping(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  EwsCliCommandP depth8;
  L7_uchar8 vlanRange[32];

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Snooping1, 
                         pStrInfo_base_DhcpSnoopingShow, 
                         commandShowIpDhcpSnooping, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_IPSG_PACKAGE

  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_DB_PERSISTENCY)) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, "database", pStrInfo_base_DhcpSnoopingDbShow,
                           commandShowIpDhcpSnoopingDatabase, L7_NO_OPTIONAL_PARAMS);
  
    ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                           NULL, L7_NO_OPTIONAL_PARAMS);
  }
#endif

  depth3 = ewsCliAddNode(depth2, "interfaces", pStrInfo_base_DsIfShow,
                         commandShowIpDhcpSnoopingIf, L7_NO_OPTIONAL_PARAMS);

  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  
  /* <u/s/p> */
  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);


  depth3 = ewsCliAddNode(depth2, "binding", pStrInfo_base_DhcpSnoopingBindingShow,
                         commandShowIpDhcpSnoopingBinding, L7_NO_OPTIONAL_PARAMS);

  
  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

     /***************************************************
   * show ip dhcp snooping binding dynamic
   ***************************************************/

  depth4 = ewsCliAddNode(depth3, "dynamic",
                         "Restrict the output based on dhcp snooping. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip dhcp snooping binding dynamic
    interface <interface-id> */

   depth5 = ewsCliAddNode(depth4, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip dhcp snooping binding dynamic
    interface <interface-id> vlan <vlan-id> */

   depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,

                                           L7_DOT1Q_MAX_VLAN_ID);
   depth8 = ewsCliAddNode(depth7, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip dhcp snooping binding dynamic
     vlan <vlan id> */
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth6 = ewsCliAddNode(depth5, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip dhcp snooping binding dynamic
     vlan <vlan id> interface <interface id> */

   depth7 = ewsCliAddNode(depth6, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /***************************************************
   * show ip source binding static
   ***************************************************/

  depth4 = ewsCliAddNode(depth3, "static",
                         "Restrict the output based on static entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip dhcp snooping binding mac_address static
    interface <interface-id> */

   depth5 = ewsCliAddNode(depth4, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip dhcp snooping binding static
    interface <interface-id> vlan <vlan-id> */

   depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth8 = ewsCliAddNode(depth7, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip dhcp snooping binding static
     vlan <vlan id> */
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);

   depth6 = ewsCliAddNode(depth5, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip dhcp snooping binding static
     vlan <vlan id> interface <interface id> */

   depth7 = ewsCliAddNode(depth6, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);



   /************************************************************
    * show ip dhcp snooping binding interface <interfaceId>
    ************************************************************/

   depth4 = ewsCliAddNode(depth3, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

   /* show ip dhcp snooping binding interface <interface-id> vlan <vlan-id> */

   depth6 = ewsCliAddNode(depth5, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth7 = ewsCliAddNode(depth6, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip dhcp snooping binding interface <interface-id> vlan <vlan-id>
     dhcp-snooping */
  depth8 = ewsCliAddNode(depth7, "dynamic",
                         "Restrict the output based on dynamic entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip dhcp snooping binding interface <interface-id> vlan <vlan-id>
     static */
  depth8 = ewsCliAddNode(depth7, "static",
                         "Restrict the output based on static. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip dhcp snooping binding interface <interface-id> dynamic */
  depth6 = ewsCliAddNode(depth5, "dynamic",
                         "Restrict the output based on dynamic entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
 /* show ip dhcp snooping binding interface <interface-id> dynamic
   vlan <vlanId> */

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
  osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth8 = ewsCliAddNode(depth7, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip dhcp snooping binding interface <interface-id> static */
  depth6 = ewsCliAddNode(depth5, "static",
                         "Restrict the output based on static bindings. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip dhcp snooping binding interface <interface-id> static
   vlan <vlanId> */

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
  osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
  depth8 = ewsCliAddNode(depth7, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);


   /************************************************************
    * show ip dhcp snooping binding vlan <vlan-id>
    ************************************************************/

   depth4 = ewsCliAddNode(depth3, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth5 = ewsCliAddNode(depth4, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

   /* show ip dhcp snooping binding vlan <vlan-id> static */
   depth6 = ewsCliAddNode(depth5, "static",
                         "Restrict the output based on static entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* show ip source binding vlan <vlan-id> static interface <interface-id */
  depth7 = ewsCliAddNode(depth6, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

   /* show ip dhcp snooping binding vlan <vlan-id> dynamic */
   depth6 = ewsCliAddNode(depth5, "static",
                         "Restrict the output based on static entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* show ip dhcp snooping binding vlan <vlan-id> staic interface <interface-id */
  depth7 = ewsCliAddNode(depth6, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip dhcp snooping binding vlan <vlan-id> interface <interface-id> */
  depth6 = ewsCliAddNode(depth5, "interface",
                          pStrInfo_base_IpsgShowIf,NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = buildTreeInterfaceHelp(depth6, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, "dynamic",
                         "Restrict the output based on dynamic entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, "static",
                         "Restrict the output based on static entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);





  depth3 = ewsCliAddNode(depth2, "statistics", pStrInfo_base_DhcpSnoopingStatsShow,
                         commandShowIpDhcpSnoopingStats, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the DHCP snooping clear commands tree
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
void buildTreeClearDhcpSnoop(EwsCliCommandP depth4)
{
  /* depth4 = clear ip dhcp */
  EwsCliCommandP depth5, depth6, depth7, depth8;

  depth5 = ewsCliAddNode(depth4, "snooping", 
                         "Clear DHCP snooping statistics or bindings", 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* clear ip dhcp snooping binding */
  depth6 = ewsCliAddNode(depth5, "binding", "Clear DHCP snooping bindings", 
                         commandClearIpDhcpSnoopingBinding, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* interface */
  depth7 = ewsCliAddNode(depth6, "interface", "Restrict clear to a specific interface", 
                         commandClearIpDhcpSnoopingBinding, L7_NO_OPTIONAL_PARAMS);
  depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, 
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* clear ip dhcp snooping statistics */
  depth6 = ewsCliAddNode(depth5, "statistics", "Clear DHCP snooping statistics", 
                         commandClearIpDhcpSnoopingStats, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
*
* @purpose  IP Source Guard show
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes    show ip verify source [interface <u/s/p>] [static | dynamic]
*
* @end
*
*********************************************************************/
void buildTreeShowIpVerifySource(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6,depth7;

  /* show ip verify */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Verify, 
                         pStrInfo_base_IpsgShow, commandShowIpVerify,
                         L7_NO_OPTIONAL_PARAMS);

  ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
   /* interface */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  /* <u/s/p> */
  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS); 

  /*show ip verify  source */
  depth3 = ewsCliAddNode(depth2, "source", pStrInfo_base_IpsgShow, 
                         commandShowIpVerifySource, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* interface */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_IpsgShowIf, 
                          NULL, L7_NO_OPTIONAL_PARAMS);
  /* <u/s/p> */
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, 
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                NULL, L7_NO_OPTIONAL_PARAMS);

  depth7 = buildTreeInterfaceHelp(depth6, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
}

/*********************************************************************
*
* @purpose  To display IPSG Entries
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes   show ip source binding [ip-address][mac-address]
*                                 [dhcp-snooping | static]
*                                 [inteface interface-id] [vlan vlan-id] 
*
* @end
*
*********************************************************************/
void buildTreeShowIpSourceBinding(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6,depth7;
  EwsCliCommandP depth8;
  L7_uchar8 vlanRange[32];


  /* show ip source */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_IP_Source,
                         pStrInfo_base_Ipsg_Entries_Show,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* show ip source binding */
  depth3 = ewsCliAddNode(depth2, "binding", pStrInfo_base_Ipsg_Entries_Show,
                         commandShowIpsgBinding, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
 
  /***************************************************
   * show ip source binding dhcp-snooping 
   ***************************************************/

  depth4 = ewsCliAddNode(depth3, "dhcp-snooping",
                         "Restrict the output based on dhcp snooping. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip source binding dhcp-snooping
    interface <interface-id> */

   depth5 = ewsCliAddNode(depth4, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip source binding dhcp-snooping
    interface <interface-id> vlan <vlan-id> */

   depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth8 = ewsCliAddNode(depth7, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip source binding dhcp-snooping
     vlan <vlan id> */
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth6 = ewsCliAddNode(depth5, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip source binding dhcp-snooping
     vlan <vlan id> interface <interface id> */

   depth7 = ewsCliAddNode(depth6, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /***************************************************
   * show ip source binding static
   ***************************************************/

  depth4 = ewsCliAddNode(depth3, "static",
                         "Restrict the output based on static entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip source binding mac_address static
    interface <interface-id> */

   depth5 = ewsCliAddNode(depth4, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip source binding static
    interface <interface-id> vlan <vlan-id> */

   depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth8 = ewsCliAddNode(depth7, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip source binding static
     vlan <vlan id> */
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth6 = ewsCliAddNode(depth5, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip source binding static
     vlan <vlan id> interface <interface id> */

   depth7 = ewsCliAddNode(depth6, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);



   /************************************************************
    * show ip source binding interface <interfaceId>
    ************************************************************/

   depth4 = ewsCliAddNode(depth3, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
   depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
   ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

   /* show ip source binding interface <interface-id> vlan <vlan-id> */

   depth6 = ewsCliAddNode(depth5, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth7 = ewsCliAddNode(depth6, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip source binding interface <interface-id> vlan <vlan-id>
     dhcp-snooping */
  depth8 = ewsCliAddNode(depth7, "dhcp-snooping",
                         "Restrict the output based on dhcp-snooping. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  
  /* show ip source binding interface <interface-id> vlan <vlan-id>
     static */
  depth8 = ewsCliAddNode(depth7, "static",
                         "Restrict the output based on dhcp-snooping. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip source binding interface <interface-id> dhcp-snooping */
  depth6 = ewsCliAddNode(depth5, "dhcp-snooping",
                         "Restrict the output based on dhcp-snooping. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
 /* show ip source binding interface <interface-id> dhcp-snooping
   vlan <vlanId> */

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
  osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth8 = ewsCliAddNode(depth7, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip source binding interface <interface-id> static */
  depth6 = ewsCliAddNode(depth5, "static",
                         "Restrict the output based on dhcp-snooping. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

 /* show ip source binding interface <interface-id> static
   vlan <vlanId> */

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
  osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
  depth8 = ewsCliAddNode(depth7, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);


   /************************************************************
    * show ip source binding vlan <vlan-id>
    ************************************************************/

   depth4 = ewsCliAddNode(depth3, pStrInfo_base_VlanKeyword,
                          "Restrict the output based on vlan.",
                          NULL, L7_NO_OPTIONAL_PARAMS);
   osapiSnprintf(vlanRange, 32, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID,
                                            L7_DOT1Q_MAX_VLAN_ID);
   depth5 = ewsCliAddNode(depth4, vlanRange, "A single VLAN ID",
                         NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

   /* show ip source binding vlan <vlan-id> static */
   depth6 = ewsCliAddNode(depth5, "static",
                         "Restrict the output based on static entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* show ip source binding vlan <vlan-id> static interface <interface-id */
  depth7 = ewsCliAddNode(depth6, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

   /* show ip source binding vlan <vlan-id> dhcp-snooping */
   depth6 = ewsCliAddNode(depth5, "dhcp-snooping",
                         "Restrict the output based on dynamic entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* show ip source binding vlan <vlan-id> dynamic interface <interface-id */
  depth7 = ewsCliAddNode(depth6, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  /* show ip source binding vlan <vlan-id> interface <interface-id> */
  depth6 = ewsCliAddNode(depth5, "interface",
                          pStrInfo_base_IpsgShowIf,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = buildTreeInterfaceHelp(depth6, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, "dhcp-snooping",
                         "Restrict the output based on dynamic entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, "static",
                         "Restrict the output based on dynamic entries. " ,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

}
#endif
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
*
* @purpose  Build the tree node for DHCP L2 Relay Global config mode
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
void buildTreeGlobalDhcpL2Relay(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_char8  vlanRangeHelp[L7_CLI_MAX_LARGE_STRING_LENGTH];
  sprintf(vlanRangeHelp, pStrInfo_base_VlanRange_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);


  /* dhcp l2relay */
  depth2 = ewsCliAddNode(depth1, pStrInfo_switching_DhcpL2Relay, 
                         pStrInfo_switching_DhcpL2RelayGlobal, 
                         commandDhcpL2RelayGlobalMode, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* dhcp l2relay circuit-id vlan <vlan-range> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelayCircuitId, 
                         pStrInfo_switching_DhcpL2RelayCircuitIdHelp,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_VlanKeyword, 
                         pStrInfo_switching_DhcpL2RelayCircuitIdVlan,
                         commandDhcpL2RelayVlanCircuitId, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4,pStrInfo_common_VlanList_1,
                          vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* dhcp l2relay  remote-id <remote-id-Str> vlan <vlan-range>*/
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelayRemoteId, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdHelp,
                         commandDhcpL2RelayVlanRemoteId, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_DhcpL2RelayRemoteIdStr, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdStrHelp,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_VlanKeyword, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdVlan,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5,pStrInfo_common_VlanList_1,
                         vlanRangeHelp,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_VlanKeyword, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdVlan,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4,pStrInfo_common_VlanList_1,
                         vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /* dhcp l2relay vlan <vlan-range> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_VlanKeyword, 
                         pStrInfo_switching_DhcpL2RelayVlan,
                         commandDhcpL2RelayVlan, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3,pStrInfo_common_VlanList_1,
                          vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}
/*********************************************************************
*
* @purpose  Build the tree node for DHCP L2 Relay Interface config mode
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
void buildTreeIntfDhcpL2Relay(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;
  
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  EwsCliCommandP depth4, depth5, depth6;
#endif

  /* dhcp l2relay */
  depth2 = ewsCliAddNode(depth1, pStrInfo_switching_DhcpL2Relay, 
                         pStrInfo_switching_DhcpL2RelayGlobal, 
                         commandDhcpL2RelayIntfMode, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* dhcp l2relay trust */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Trust,
                         pStrInfo_switching_DhcpL2RelayIntfTrustHelp,
                         commandDhcpL2RelayIntfTrust,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);


#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  /* dhcp l2relay circuit-id subscription <subscription-name> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelayCircuitId, 
                         pStrInfo_switching_DhcpL2RelayCircuitIdHelp,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_DhcpL2RelaySubscription, 
                         pStrInfo_switching_DhcpL2RelayCircuitIdSubscription,
                         commandDhcpL2RelayCircuitIdSubscription, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4,pStrInfo_switching_DhcpL2RelaySubscriptionStr,
                         pStrInfo_switching_DhcpL2RelaySubscriptionStrHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* dhcp l2relay  remote-id <remote-id-Str> subscription <subscription-name> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelayRemoteId, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdHelp,
                         commandDhcpL2RelayRemoteIdSubscription, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_DhcpL2RelayRemoteIdStr, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdStrHelp,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_switching_DhcpL2RelaySubscription, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdSubscription,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5,pStrInfo_switching_DhcpL2RelaySubscriptionStr,
                         pStrInfo_switching_DhcpL2RelaySubscriptionStrHelp,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_DhcpL2RelaySubscription, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdSubscription,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4,pStrInfo_switching_DhcpL2RelaySubscriptionStr,
                         pStrInfo_switching_DhcpL2RelaySubscriptionStrHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /* dhcp l2relay subscription <subscription-name> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelaySubscription, 
                         pStrInfo_switching_DhcpL2RelayModeSubscription,
                         commandDhcpL2RelaySubscription, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3,pStrInfo_switching_DhcpL2RelaySubscriptionStr,
                         pStrInfo_switching_DhcpL2RelaySubscriptionStrHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif
}
/*********************************************************************
*
* @purpose  Build the tree node for DHCP L2 relay Show
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes    show dhcp l2relay ...
*
* @end
*
*********************************************************************/
void buildTreeShowDhcpL2Relay(EwsCliCommandP depth1)
{
  EwsCliCommandP  depth2, depth3, depth4, depth5;
  L7_char8  vlanRangeHelp[L7_CLI_MAX_LARGE_STRING_LENGTH];
  sprintf(vlanRangeHelp, pStrInfo_base_VlanRange_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);


  depth2 = ewsCliAddNode(depth1, pStrInfo_switching_DhcpL2Relay, 
                         pStrInfo_switching_DhcpL2RelayShow, 
                         commandShowDhcpL2Relay, L7_NO_OPTIONAL_PARAMS);
  /* All*/
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_all_1, 
                         pStrInfo_switching_DhcpL2RelayShowAll,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* Interface <u/s/p>*/
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_interface, 
                         pStrInfo_switching_DhcpL2RelayShowIntf,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_all_1, 
                         pStrInfo_switching_DhcpL2RelayShowIntfAll,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);


  /* Circuit-id <vlan-range> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelayCircuitId, 
                         pStrInfo_switching_DhcpL2RelayCircuitIdShowVlan,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_VlanKeyword, 
                         pStrInfo_switching_DhcpL2RelayCircuitIdShowVlan,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4,pStrInfo_common_VlanList_1,
                          vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* Relay Agent info <vlan-range> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelayRelayAgentInfo, 
                         pStrInfo_switching_DhcpL2RelayAgentInfoShowVlan,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_VlanKeyword, 
                         pStrInfo_switching_DhcpL2RelayCircuitIdShowVlan,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4,pStrInfo_common_VlanList_1,
                          vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* remote-id <vlan-range> */
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelayRemoteId, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdShowVlan,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_VlanKeyword, 
                         pStrInfo_switching_DhcpL2RelayRemoteIdShowVlan,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4,pStrInfo_common_VlanList_1,
                          vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  /* statistics interface {all | <u/s/p>} */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Stats, 
                         pStrInfo_switching_DhcpL2RelayShowIntfStats,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_interface, 
                         pStrInfo_switching_DhcpL2RelayShowIntfSpecificStats,
                         commandShowDhcpL2RelayIntfStats, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_all_1, 
                         pStrInfo_switching_DhcpL2RelayShowIntfAllStats,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
 ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);


#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  /* subscription interface {all | <u/s/p>} */
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_DhcpL2RelaySubscription, 
                         pStrInfo_switching_DhcpL2RelayShowSubscription,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_interface, 
                         pStrInfo_switching_DhcpL2RelayShowSubscriptionIntf,
                         commandShowDhcpL2RelaySubscription, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_all_1, 
                         pStrInfo_switching_DhcpL2RelayShowIntfAll,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  /* vlan <vlan-range>*/
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_VlanKeyword, 
                         pStrInfo_switching_DhcpL2RelayShowVlan,
                         L7_NULL, 
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3,pStrInfo_common_VlanList_1,
                          vlanRangeHelp,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS); 

}
/*********************************************************************
*
* @purpose  Builds the DHCP L2relay clear commands tree
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
void buildTreeClearDhcpL2Relay(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;

  depth2 = ewsCliAddNode(depth1, pStrInfo_switching_DhcpL2Relay, 
                         pStrInfo_switching_DhcpL2RelayClearStatsHelp, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Statistics, 
                         pStrInfo_switching_DhcpL2RelayClearStatsHelp, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_interface, 
                         pStrInfo_switching_DhcpL2RelayClearStatsIntfHelp,
                         commandClearDhcpL2RelayStats, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_all_1,
                         pStrInfo_switching_DhcpL2RelayClearStatsAllHelp,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                NULL, L7_NO_OPTIONAL_PARAMS);
}
#endif
