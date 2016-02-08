/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/clicommands_dhcps.c
*
* @purpose create the tree for CLI Dhcps
*
* @component user interface
*
* @comments none
*
* @create  03/05/2007
*
* @author nshrivastav
* @end
*
**********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "clicommands_dhcps.h"

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE

#include "clicommands_ipv6.h"

#endif
#endif

/*********************************************************************
 *
 * @purpose  Build the tree nodes for DHCP Pool Config mode commands
 *
 * @param void
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void buildTreeDhcpsPoolConfigMode()
{
  EwsCliCommandP depth1, depth2, depth3, depth4, depth5, depth6, depth7;
  EwsCliCommandP depth8, depth9, depth10, depth11, depth12, depth13;
  EwsCliCommandP tmp_depth4, tmp_depth5, tmp_depth6, tmp_depth7, tmp_depth8, tmp_depth9, tmp_depth10;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliDhcpsPoolConfigMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_DHCP_POOL_CONFIG_MODE, depth1);

  /* command    :       bootfile     */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Bootfile, pStrInfo_base_DhcpsBootFile, commandBootfile, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Filename, pStrInfo_base_DhcpsFileName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       bootfile     -       End     */

  /* command    :       client-identifier       */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_ClientId_2, pStrInfo_base_DhcpsClientId, commandClientIdentifier, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_UniqueId, pStrInfo_base_DhcpsId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       client-identifier       -       End     */

  /* command    :       client-name     */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_ClientName_2, pStrInfo_base_DhcpsClientName, commandClientName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name, pStrInfo_base_DhcpsName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       client-name     -       End     */

  /* command    :       default-router  */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_DeflRtr, pStrInfo_base_DhcpsDeflRtr, commandDefaultRouter, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_IpAddr1, pStrInfo_base_DhcpsDeflRtrAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IpAddr2, pStrInfo_base_DhcpsDeflRtrAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth4 = depth4;

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_IpAddr3, pStrInfo_base_DhcpsDeflRtrAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth5 = depth5;

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_IpAddr4, pStrInfo_base_DhcpsDeflRtrAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth6 = depth6;

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_IpAddr5, pStrInfo_base_DhcpsDeflRtrAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth7 = depth7;

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_IpAddr6, pStrInfo_base_DhcpsDeflRtrAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth8 = depth8;

  depth9  = ewsCliAddNode(depth8, pStrInfo_base_IpAddr7, pStrInfo_base_DhcpsDeflRtrAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth9 = depth9;

  depth10 = ewsCliAddNode(depth9, pStrInfo_base_IpAddr8, pStrInfo_base_DhcpsDeflRtrAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth10 = depth10;

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(tmp_depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(tmp_depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(tmp_depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(tmp_depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(tmp_depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(tmp_depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth11 = ewsCliAddNode(tmp_depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       default-router  -       End     */

  /* command    :       dns-server      */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DnsSrvr_1, pStrInfo_common_DhcpsDns, commandDnsServer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_IpAddr1, pStrInfo_base_DhcpsDnsAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IpAddr2, pStrInfo_base_DhcpsDnsAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth4 = depth4;

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_IpAddr3, pStrInfo_base_DhcpsDnsAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth5 = depth5;

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_IpAddr4, pStrInfo_base_DhcpsDnsAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth6 = depth6;

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_IpAddr5, pStrInfo_base_DhcpsDnsAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth7 = depth7;

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_IpAddr6, pStrInfo_base_DhcpsDnsAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth8 = depth8;

  depth9  = ewsCliAddNode(depth8, pStrInfo_base_IpAddr7, pStrInfo_base_DhcpsDnsAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth9 = depth9;

  depth10 = ewsCliAddNode(depth9, pStrInfo_base_IpAddr8, pStrInfo_base_DhcpsDnsAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth10 = depth10;

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(tmp_depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(tmp_depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(tmp_depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(tmp_depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(tmp_depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(tmp_depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth11 = ewsCliAddNode(tmp_depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       dns-server      -       End     */

  /* command    :       domain-name     */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_DomainName, pStrInfo_common_DhcpsDoMainName, commandDomainName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Domain, pStrInfo_common_DhcpsNameOfDoMain, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       domain-name     -       End     */

  /* command    :       hardware-address        */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_HardwareAddr_2, pStrInfo_base_DhcpsHardwareAddr, commandHardwareAddress, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_HardwareAddr, pStrInfo_base_DhcpsHardwareAddr_1, NULL, 2, L7_NODE_DATA_TYPE, L7_MAC_ADDRESS_SVL_DATA_TYPE);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Type, pStrInfo_base_DhcpsType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       hardware-address        -       End     */

  /* command    :       host    */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Host_2, pStrInfo_base_DhcpsHost, commandHost, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Addr_1, pStrInfo_base_DhcpsHostAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mask, pStrInfo_base_DhcpsHostMask, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_PrefixLen, pStrInfo_base_DhcpsHostPrefixLen, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       host    -       End     */

  /* command    :       lease   */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lease, pStrInfo_base_DhcpsLease, commandLease, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Range0to59, pStrInfo_base_DhcpsDays, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Range0to23, pStrInfo_base_DhcpsHours, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to59, pStrInfo_base_DhcpsMinutes, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_DhcpsLeaseInfinite_1, pStrInfo_base_DhcpsLeaseInfinite, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       lease   -       End     */

  /* command    :       netbios-name-server      */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_NetbiosNameSrvr_1, pStrInfo_base_DhcpsNetbiosNameSrvr, commandNetbiosNameServer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_IpAddr1, pStrInfo_base_DhcpsNetbiosAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IpAddr2, pStrInfo_base_DhcpsNetbiosAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth4 = depth4;

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_IpAddr3, pStrInfo_base_DhcpsNetbiosAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth5 = depth5;

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_IpAddr4, pStrInfo_base_DhcpsNetbiosAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth6 = depth6;

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_IpAddr5, pStrInfo_base_DhcpsNetbiosAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth7 = depth7;

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_IpAddr6, pStrInfo_base_DhcpsNetbiosAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth8 = depth8;

  depth9  = ewsCliAddNode(depth8, pStrInfo_base_IpAddr7, pStrInfo_base_DhcpsNetbiosAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth9 = depth9;

  depth10 = ewsCliAddNode(depth9, pStrInfo_base_IpAddr8, pStrInfo_base_DhcpsNetbiosAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  tmp_depth10 = depth10;

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(tmp_depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(tmp_depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(tmp_depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(tmp_depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(tmp_depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(tmp_depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth11 = ewsCliAddNode(tmp_depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       netbios-name-server      -       End     */

  /* command    :       netbios-node-type     */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_NetbiosNodeType_2, pStrInfo_base_DhcpsNetbiosNodeType, commandNetbiosNodeType, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_BNode, pStrInfo_base_DhcpsNetBiosType, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_HNode, pStrInfo_base_DhcpsNetBiosTypeModeH, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MNode, pStrInfo_base_DhcpsNetBiosTypeModeM, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_PNode, pStrInfo_base_DhcpsNetBiosTypeModeP, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       netbios-node-type     -       End     */

  /* command    :       next-server     */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_NextSrvr_1, pStrInfo_base_DhcpsNextSrvr, commandNextServer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_IpAddr, pStrInfo_base_DhcpsNextSrvrAddr, NULL, 2,  L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       next-server     -       End     */

  /* command    :       network */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_WsShowRunningNwMode, pStrInfo_base_DhcpsNw, commandNetwork, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_NwNum, pStrInfo_base_DhcpsNwNum, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mask, pStrInfo_base_DhcpsMask, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_PrefixLen, pStrInfo_base_DhcpsPrefixLen, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       network -       End     */

  /* command    :       option      */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Option_1, pStrInfo_base_DhcpsOption, commandOption, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Range1to254, pStrInfo_base_DhcpsCode, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_AsCiiOption, pStrInfo_base_DhcpsDataFmtAsCii, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_String1, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_HexOption, pStrInfo_base_DhcpsDataFmtHex, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_String1, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_String2, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_String3, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_String4, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_String5, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth10 = ewsCliAddNode(depth9, pStrInfo_base_String6, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth11 = ewsCliAddNode(depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth11  = ewsCliAddNode(depth10, pStrInfo_base_String7, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth12 = ewsCliAddNode(depth11, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth12 = ewsCliAddNode(depth11, pStrInfo_base_String8, pStrInfo_base_DhcpsOptionString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth13 = ewsCliAddNode(depth12, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_IpOption, pStrInfo_base_DhcpsDataFmtIp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_IpAddr1, pStrInfo_base_DhcpsOptionString, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_IpAddr2, pStrInfo_base_DhcpsOptionString, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_IpAddr3, pStrInfo_base_DhcpsOptionString, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_IpAddr4, pStrInfo_base_DhcpsOptionString, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_IpAddr5, pStrInfo_base_DhcpsOptionString, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth10 = ewsCliAddNode(depth9, pStrInfo_base_IpAddr6, pStrInfo_base_DhcpsOptionString, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth11 = ewsCliAddNode(depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth11  = ewsCliAddNode(depth10, pStrInfo_base_IpAddr7, pStrInfo_base_DhcpsOptionString, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth12 = ewsCliAddNode(depth11, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth12 = ewsCliAddNode(depth11, pStrInfo_base_IpAddr8, pStrInfo_base_DhcpsOptionString, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth13 = ewsCliAddNode(depth12, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command    :       option      -       End     */

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
 *
 * @purpose  Build the tree nodes for DHCP Global Config mode commands
 *           starting with the 'ip' keyword
 *
 * @param EwsCliCommandP depth2
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void buildTreeDhcpsConfig(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7;


  /* command      :       ip dhcp bootp automatic    */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Bootp_1, pStrInfo_base_DhcpsBootpAuto, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Auto_2, pStrInfo_base_DhcpsBootpAuto, commandIpDhcpBootpAutomatic, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* command    :       ip dhcp bootp automatic    -       End     */

  /* command      :       ip dhcp conflict logging    */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Conflict, pStrInfo_base_DhcpsConflictLogging, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Logging_3, pStrInfo_base_DhcpsConflictLogging, commandIpDhcpConflictLogging, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* command    :       ip dhcp conflict logging    -       End     */

  /* command      :     ip dhcp excluded-address        */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_ExcldAddr, pStrInfo_base_DhcpsExcldAddr, commandIpDhcpExcludedAddress, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LowAddr, pStrInfo_base_DhcpsLowAddr, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_HighAddr, pStrInfo_base_DhcpsHighAddr, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* command      :     ip dhcp excluded-address    - End  */

  /* command    :       ip dhcp ping packets    */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ping_3, pStrInfo_base_DhcpsPingPkts, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_OspfTrapTxReTx, pStrInfo_base_DhcpsPingPkts, commandDhcpPingPackets, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_0210, pStrInfo_base_DhcpsPingPktsNum, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command    :       ip dhcp ping packets    -       End     */

  /* command      :     ip dhcp pool    */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Pool_2, pStrInfo_base_DhcpsIpPool, cliDhcpsPoolConfigMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Name, pStrInfo_base_DhcpsIpPoolName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_TYPE_MODE, L7_DHCP_POOL_CONFIG_MODE);
  /* command      :     ip dhcp pool    - End  */

  

}

/*********************************************************************
 *
 * @purpose  Build the tree nodes for DHCP Global Config mode commands
 *
 * @param EwsCliCommandP depth1
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void buildTreeDhcpsServiceConfig(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  /* command      :       service dhcp    */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Service, pStrInfo_base_DhcpsServiceDhcp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dhcp_1, pStrInfo_base_DhcpsServiceDhcp, commandServiceDhcp, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Dhcpv6_2, pStrInfo_base_DhcpsServiceDhcpv6, commandServiceDhcpV6, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif
#endif
  /* command    :       service dhcp    -       End     */
}

/*********************************************************************
 *
 * @purpose  Build the tree nodes for DHCP Show commands
 *
 * @param EwsCliCommandP depth3
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void buildTreeShowDHCPs(EwsCliCommandP depth4)
{
  EwsCliCommandP  depth5, depth6, depth7, depth8;

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Binding, pStrInfo_base_DhcpsIpShowBinding, commandShowDhcpsBinding, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Addr_1, pStrInfo_base_DhcpsIpBindingAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_base_DhcpsIpBindingAll, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command      :     show ip dhcp binding    - End  */

  /* command      :     show ip dhcp conflict    */
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Conflict, pStrInfo_base_DhcpsIpShowConflict, commandShowDhcpsConflict, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Addr_1, pStrInfo_base_DhcpsIpConflictAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_base_DhcpsIpConflictAll, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command      :     show ip dhcp conflict    - End  */

  /* command      :     show ip dhcp global config  */
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Global, pStrInfo_base_DhcpsIpShowGlobalConf, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cfg_8, pStrInfo_base_DhcpsIpShowGlobalConf, commandShowDhcpsGlobalConfig, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command      :     show ip dhcp global config    - End  */

  /* command      :     show ip dhcp pool config  */
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Pool_2, pStrInfo_base_DhcpsIpShowPoolConf, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cfg_8, pStrInfo_base_DhcpsIpShowPoolConf, commandShowDhcpsPoolConfig, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Name, pStrInfo_base_DhcpsShowPoolSpecific, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_All, pStrInfo_base_DhcpsShowAllPool, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command      :     show ip dhcp global config    - End  */

  /* command      :     show ip dhcp server statistics  */
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Srvr_1, pStrInfo_base_DhcpsIpShowStats, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowStats, pStrInfo_base_DhcpsIpShowStats, commandShowDhcpsStatistics, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command      :     show ip dhcp server statistics    - End  */
}

/*********************************************************************
 *
 * @purpose  Build the tree nodes for DHCP Clear commands
 *
 * @param EwsCliCommandP depth4
 *
 * @returntype void
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void buildTreeClearDHCPs(EwsCliCommandP depth4)
{
  EwsCliCommandP depth5, depth6, depth7;

  /* command      :     clear ip dhcp binding   */
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Binding, pStrInfo_base_ClrBinding, commandClearIpDhcpBinding, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_IpAddr, pStrInfo_base_DhcpsClrIpAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, "* ", pStrInfo_base_DhcpsClrAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command      :     clear ip dhcp conflict  */
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Conflict, pStrInfo_base_ClrConflict, commandClearIpDhcpConflict, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_IpAddr, pStrInfo_base_DhcpsClrConflictIpAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, "* ", pStrInfo_base_DhcpsClrAllConflict, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command      :     clear ip dhcp server statistics   */
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Srvr_1, pStrInfo_base_DhcpClrServStats, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowStats, pStrInfo_base_DhcpClrServStats, commandClearIpDhcpServerStatistics, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
 *
 * @purpose  Build the DHCP Pool Config Mode Tree.
 *
 * @param void
 *
 * @returntype void
 *
 * @note This function create the DHCP Pool Config tree.
 *
 * @end
 *
 *********************************************************************/
void buildTreeDhcpsPool()
{
  EwsCliCommandP depth1,depth2,depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliDhcpsPoolConfigMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_DHCP_POOL_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 =  ewsCliAddNode ( depth1, pStrInfo_common_Help_2, pStrInfo_common_DispHelpForVariousSpecialKeys, commandError, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeDhcpsPoolConfigMode(depth1);
}




 

