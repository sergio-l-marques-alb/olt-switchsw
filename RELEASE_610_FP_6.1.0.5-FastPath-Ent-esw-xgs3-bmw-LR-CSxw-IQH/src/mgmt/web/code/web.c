/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/emweb/web/base/web.c
*
* @purpose Code in support of the various EmWeb html pages
*
* @component unitmgr
*
* @comments tba
*
* @create 06/12/2000
*
* @author tgaunce
* @end
*
**********************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "l7_common.h"

#ifndef L7_PRODUCT_SMARTPATH
#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_base_common.h"
#include "strlib_base_web.h"
#include "strlib_qos_common.h"
#include "strlib_qos_web.h"
#include "web.h"
#include "ews_api.h"
#include "ews.h"
#include "ewnet.h"
#include "ew_form.h"
#include "ew_proto.h"
#include "log.h"
#include "web_buffer.h"
#include "usmdb_user_mgmt_api.h"
#include "usmdb_util_api.h"
#include "filter_exports.h"
#include "policy_exports.h"
#include "dot1x_exports.h"
#include "dot1q_exports.h"
#include "sshd_exports.h"
#include "sslt_exports.h"
#include "dot1q_exports.h"
#include "nim_exports.h"
#include "snooping_exports.h"
#include "doscontrol_exports.h"
#include "mirror_exports.h"
#include "dot1s_exports.h"
#include "auto_install_exports.h"
#include "usmdb_sim_api.h"
#include "dot1s_exports.h"
#include "usmdb_registry_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_qos_acl_api.h"
#include "dot3ad_exports.h"
#include "dot1ag_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_dot1s_api.h"
#include "util_pstring.h"

extern L7_int32 isalnum(L7_int32 c);
static L7_char8  buf[256];
static L7_char8  usmWebPageBuf[4096];
static L7_char8  webBuffer[256];
static L7_uint32 currentGarpError;
L7_uchar8 webStaticContentBuffer[WEB_STATIC_CONTENT_BUFFER_SIZE];
/*********************************************************************
* @purpose  used to convert mac address to hex
*
* @param    buf   contains ascii string "xx:xx:xx:xx:xx:xx"
* @param    mac   return hex data
*
* @returns L7_TRUE successfully converted
*          L7_FALSE if not valid MAC address
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL usmWebConvertMac(L7_uint32 numBytes, L7_uchar8 * buf, L7_uchar8 * mac)
{
  L7_uint32 i,j, digit_count=0;
  L7_uchar8 mac_address[24];

  if (strlen(buf) != (numBytes*3-1))
  {      /* test string length */
    return L7_FALSE;
  }

  for (i=0,j=0; i<(numBytes*3-1); i++,j++)
  {
    digit_count++;
    switch (buf[i])
    {
      case '0':
        mac_address[j]=0x0;
        break;
      case '1':
        mac_address[j]=0x1;
        break;
      case '2':
        mac_address[j]=0x2;
        break;
      case '3':
        mac_address[j]=0x3;
        break;
      case '4':
        mac_address[j]=0x4;
        break;
      case '5':
        mac_address[j]=0x5;
        break;
      case '6':
        mac_address[j]=0x6;
        break;
      case '7':
        mac_address[j]=0x7;
        break;
      case '8':
        mac_address[j]=0x8;
        break;
      case '9':
        mac_address[j]=0x9;
        break;
      case 'a':
      case 'A':
        mac_address[j]=0xA;
        break;
      case 'b':
      case 'B':
        mac_address[j]=0xB;
        break;
      case 'c':
      case 'C':
        mac_address[j]=0xC;
        break;
      case 'd':
      case 'D':
        mac_address[j]=0xD;
        break;
      case 'e':
      case 'E':
        mac_address[j]=0xE;
        break;
      case 'f':
      case 'F':
        mac_address[j]=0xF;
        break;
      case ':':
        if (digit_count != 3)
        {    /* if more or less than 2 digits return false */
          return L7_FALSE;
        }
        j--;
        digit_count=0;
        break;
      default:
        return FALSE;
        break;
    }
  }

  for (i = 0; i < numBytes; i++)
  {
    mac[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return TRUE;
}

/**************************************************************************
 * Function: usmWebComponentPresent
 *
 * Description:
 *     Is the specified component present ?
 *
 * Parameters:
 *     compName : Component name string
 *
 * Return:
 *     L7_TRUE if component is present
 *     L7_FALSE otherwise
 *
 * Notes:
 *     This fn is invoked by an emweb macro
 *************************************************************************/

L7_BOOL usmWebComponentPresent(L7_char8 * compName)
{
  L7_uint32 runCheck = L7_FALSE;
  L7_uint32 componentId=0;
  L7_BOOL   rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (strcmp(compName, pStrInfo_base_CliWeb) == 0)
  {
    componentId = L7_CLI_WEB_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_CaptivePortal) == 0)
  {
    componentId = L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Snmp2) == 0)
  {
    componentId = L7_SNMP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Layer3) == 0)
  {
    componentId = L7_LAYER3_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Cos) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Bgp) == 0)
  {
    componentId = L7_FLEX_BGP_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(compName, pStrInfo_common_Ospf) == 0)
  {
    componentId = L7_OSPF_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Rip) == 0)
  {
    componentId = L7_RIP_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Rlim) == 0)
  {
    componentId = L7_RLIM_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Dvmrp_1) == 0)
  {
    componentId = L7_FLEX_DVMRP_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Vrrp) == 0)
  {
    componentId = L7_VRRP_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(compName, pStrInfo_base_RtrDisc) == 0 )
  {
    componentId = L7_RTR_DISC_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Pimdm) == 0)
  {
    componentId = L7_FLEX_PIMDM_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Acl) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Mcast) == 0)
  {
    componentId = L7_FLEX_MCAST_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Pimsm) == 0)
  {
    componentId = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Pimdm) == 0)
  {
    componentId = L7_FLEX_PIMDM_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_common_Mpls) == 0)
  {
    componentId = L7_FLEX_MPLS_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Diffserv) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_CableTest) == 0)
  {
    componentId = L7_CABLE_TEST_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_MacFilter_1) == 0)
  {
    componentId = L7_FILTER_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Snooping) == 0)
  {
    componentId = L7_SNOOPING_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Dot1s_1) == 0)
  {
    componentId = L7_DOT1S_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_IpMap) == 0)
  {
    componentId = L7_IP_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Pbvlan) == 0)
  {
    componentId = L7_PBVLAN_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_OutboundTelnet) == 0)
  {
    componentId = L7_TELNET_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Dvlan) == 0)
  {
    componentId = L7_DVLANTAG_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Sshd) == 0)
  {
    componentId = L7_FLEX_SSHD_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Sslt) == 0)
  {
    componentId = L7_FLEX_SSLT_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Stacking) == 0)
  {
    componentId = L7_FLEX_STACKING_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(compName, pStrInfo_base_PortMaclock) == 0 )
  {
    componentId = L7_PORT_MACLOCK_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_ProtectedPorts) == 0)
  {
    componentId = L7_PROTECTED_PORT_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(compName, pStrInfo_base_DosCntrl) == 0 )
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_IpsubnetVlan) == 0)
  {
    componentId = L7_VLAN_IPSUBNET_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_MacVlan) == 0)
  {
    componentId =  L7_VLAN_MAC_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if ((strcmp(compName, pStrInfo_base_Ipv6_2) == 0) || (strcmp(compName, pStrInfo_common_Ipv6_3) == 0))
  {
    componentId =  L7_FLEX_IPV6_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Ipv6Mgmt) == 0)
  {
    componentId =  L7_FLEX_IPV6_MGMT_COMPONENT_ID;
    runCheck = L7_TRUE;  
  }
  else if (strcmp(compName, pStrInfo_base_Dhcpv6) == 0)
  {
    componentId =  L7_FLEX_DHCP6S_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(compName, pStrInfo_base_Ospfv3) == 0 )
  {
    componentId = L7_FLEX_OSPFV3_MAP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(compName, pStrInfo_base_Ws_1) == 0)
  {
    componentId =  L7_FLEX_WIRELESS_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_sFlow) == 0)
  {
    componentId =  L7_SFLOW_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_DynamicArpInspection) == 0)
  {
    componentId =  L7_DAI_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_dhcp_snooping) == 0)
  {
    componentId = L7_DHCP_SNOOPING_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
 /*
  else if (strcmp(compName, pStrInfo_base_ipsg) == 0)
  {
    componentId = L7_IPSG_COMPONENT_ID;
    runCheck = L7_TRUE;
  }*/
  else if (strcmp(compName, pStrInfo_base_DhcpFiltering) ==0)
  {
    componentId = L7_DHCP_FILTERING_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_ISDP) == 0)
  {
    componentId =  L7_ISDP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_tr069) == 0)
  {
    componentId =  L7_TR069_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_Dot1adComponent) == 0)
  {
    componentId =  L7_FLEX_METRO_DOT1AD_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(compName, pStrInfo_base_dot3ah) == 0)
  {
    componentId =  L7_DOT3AH_COMPONENT_ID;
    runCheck = L7_TRUE;
  }
  else
  {
    runCheck = L7_FALSE;
  }

  if (runCheck == L7_TRUE)
  {
    rc = usmDbComponentPresentCheck(unit, componentId);
  }
  else
  {
    rc = L7_FALSE;
  }

  return rc;
}

/**************************************************************************
 * Function: usmWebFeaturePresent
 *
 * Description:
 *     Is the specified feature present (supported) ?
 *
 * Parameters:
 *     compName : Feature Description
 *
 * Return:
 *     L7_TRUE if feature is present
 *     L7_FALSE otherwise
 *
 * Notes:
 *     This fn is invoked by an emweb macro
 *************************************************************************/

L7_BOOL usmWebFeaturePresent(L7_char8 * featureDescription)
{
  L7_uint32 runCheck = L7_FALSE;
  L7_uint32 componentId=0;
  L7_uint32 featureId=0;
  L7_BOOL   rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (strcmp(featureDescription, pStrInfo_base_FilterSrcPortFiltering) == 0)
  {
    componentId = L7_FILTER_COMPONENT_ID;
    featureId = L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_FilterDestPortFiltering) == 0)
  {
    componentId = L7_FILTER_COMPONENT_ID;
    featureId = L7_FILTER_UCAST_DEST_PORT_FILTERING_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchDstipFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchSupportsMaskingFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchDstl4portFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchEveryFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_EVERY_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchIpdscpFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchIpprecedenceFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchIptosFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchSupportsMaskingFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchProtoFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchSrcipFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchSupportsMaskingFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchSrcl4portFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclIntfDirectionInboundFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclIntfDirectionOutboundFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclIntfVlanSupportFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclVlanDirectionOutboundFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclVlanDirectionInboundFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchSrcl4portRangeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchDstl4portRangeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclTypeMacFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_TYPE_MAC_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchCosFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_COS_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchCos2Feat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_COS2_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchDstmacFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchDstmacMaskFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchEtypeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchSrcmacFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchSrcmacMaskFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchVidFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_VLANID_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchVidRangeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchVid2Feat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRuleMatchVid2RangeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclLogDenyFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_LOG_DENY_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclLogPermitFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_LOG_PERMIT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclAssignQueueFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_ASSIGN_QUEUE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclRedirectFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_REDIRECT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclMirrorFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_MIRROR_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclIntfDiffservCoexistFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclIntfLagSupportFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclIntfMultipleAclFeat) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_INTF_MULTIPLE_ACL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclTypeFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_TYPE_IPV6_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclSrcipFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclSrcl4portFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclSrcl4portRangeFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclDestipFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclDstl4portFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclDstl4portRangeFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclFlowlblFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservFeatSupported) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_FEATURE_SUPPORTED;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_Ipv6AclIpDscpFeatSupport) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_ACL_RULE_MATCH_IPV6_IPDSCP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassTypeAllFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_TYPE_ALL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassTypeAnyFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_TYPE_ANY_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassTypeAclFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_TYPE_ACL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_DiffservClassL3protoIpv4Feat) == 0)
  {
     componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
     featureId = L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID;
     runCheck = L7_TRUE;
  }
  else if(strcmp(featureDescription, pStrInfo_base_DiffservClassL3protoIpv6Feat) == 0)
  {
     componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
     featureId = L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID;
     runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchCosFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchCos2Feat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_COS2_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchVlan2Feat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchEtypeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrPoliceColorawareFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchDstipFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_DSTIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchDstl4portFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_DSTL4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchDstmacFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchEveryFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpdscpFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPDSCP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpprecedenceFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPPRECEDENCE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIptosFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPTOS_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchProtoFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_PROTOCOL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchRefclassFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_REFCLASS_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchSrcipFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_SRCIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchSrcl4portFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_SRCL4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchSrcmacFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchVlanFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID;
    runCheck = L7_TRUE;
  }            /* IPv6 feature changes */
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6DstipFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_DSTIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6Dstl4portFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6Dstl4portRangeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6FlowlblFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6IpdscpFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_IPDSCP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6ProtoFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_PROTOCOL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6SrcipFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_SRCIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6Srcl4portFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIpv6Srcl4portRangeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchIPv6macFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchExclFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchSupportsMaskingFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_SUPPORTS_MASKING_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchSupportsRangesFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservClassMatchSupportsMaskingFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_CLASS_MATCH_SUPPORTS_MASKING_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrMarkIpdscpvalFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrMarkIpprecedencevalFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrPoliceSimpleFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_POLICE_SIMPLE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrPoliceSinglerateFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrPoliceTworateFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrMarkCosvalFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1adPBFeat) == 0)
  {
    componentId = L7_FLEX_METRO_DOT1AD_COMPONENT_ID;
    featureId = L7_FLEX_METRO_DOT1AD_PB_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrMarkCosAsCos2Feat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrDropFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrMarkCos2valFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrAssignQueueFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPhbEfFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_PHB_EF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPhbAf4xFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_PHB_AF4X_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPhbAf3xFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_PHB_AF3X_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPhbAf2xFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_PHB_AF2X_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPhbAf1xFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_PHB_AF1X_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPhbCsFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_PHB_CS_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPhbOtherFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_PHB_OTHER_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservServiceInAllportsFeat) == 0)
  {
    componentId = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    featureId = L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1sFeat) == 0)
  {
    componentId = L7_DOT1S_COMPONENT_ID;
    featureId = L7_DOT1S_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1sBPDUGuardFeat) == 0)
  {
    componentId = L7_DOT1S_COMPONENT_ID;
    featureId = L7_DOT1S_FEATURE_BPDUGUARD;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1sBPDUFilterFeat) == 0)
  {
    componentId = L7_DOT1S_COMPONENT_ID;
    featureId = L7_DOT1S_FEATURE_BPPDUFILTER;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1sBPDUFloodFeat) == 0)
  {
    componentId = L7_DOT1S_COMPONENT_ID;
    featureId = L7_DOT1S_FEATURE_BPPDUFLOOD;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1sRootGuardFeat) == 0)
  {
    componentId = L7_DOT1S_COMPONENT_ID;
    featureId = L7_DOT1S_FEATURE_ROOTGUARD;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1sLoopGuardFeat) == 0)
  {
    componentId = L7_DOT1S_COMPONENT_ID;
    featureId = L7_DOT1S_FEATURE_LOOPGUARD;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_SnmpUsr) == 0)
  {
    componentId = L7_SNMP_COMPONENT_ID;
    featureId = L7_SNMP_USER_SUPPORTED;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_SnmpUsrAuth) == 0)
  {
    componentId = L7_SNMP_COMPONENT_ID;
    featureId = L7_SNMP_USER_AUTHENTICATION_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_SnmpUsrEncryption) == 0)
  {
    componentId = L7_SNMP_COMPONENT_ID;
    featureId = L7_SNMP_USER_ENCRYPTION_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_PolicyBcastCntrlFeat) == 0)
  {
    componentId = L7_POLICY_COMPONENT_ID;
    featureId = L7_POLICY_BCAST_CONTROL_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_PolicyMcastCntrlFeat) == 0)
  {
    componentId = L7_POLICY_COMPONENT_ID;
    featureId = L7_POLICY_MCAST_CONTROL_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_PolicyUcastCntrlFeat) == 0)
  {
    componentId = L7_POLICY_COMPONENT_ID;
    featureId = L7_POLICY_UCAST_CONTROL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_PolicyPortBcastCntrlFeat) == 0)
  {
    componentId = L7_POLICY_COMPONENT_ID;
    featureId = L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_PolicyPortMcastCntrlFeat) == 0)
  {
    componentId = L7_POLICY_COMPONENT_ID;
    featureId = L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_PolicyPortUcastCntrlFeat) == 0)
  {
    componentId = L7_POLICY_COMPONENT_ID;
    featureId = L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_PolicyPortFlowCntrlFeat) == 0)
  {
    componentId = L7_POLICY_COMPONENT_ID;
    featureId = L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AclFeatSupported) == 0)
  {
    componentId = L7_FLEX_QOS_ACL_COMPONENT_ID;
    featureId = L7_ACL_FEATURE_SUPPORTED;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosFeatSupported) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_FEATURE_SUPPORTED;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueCfgPerIntfFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueDropCfgPerIntfFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueIntfShapingFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueMinBwFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_MIN_BW_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueMaxBwFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_MAX_BW_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosMapTrustModeFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_MAP_TRUST_MODE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosMapTrustModePerIntfFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosMapIpprecFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_MAP_IPPREC_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosMapIpprecPerIntfFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosMapIpdscpFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_MAP_IPDSCP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosMapIpdscpPerIntfFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueMgmtIntfOnlyFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueWredSupportFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueWredDecayExpSysOnlyFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueTdropThreshFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueWredMaxThreshFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueSchedStrictOnlyFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_SCHED_STRICT_ONLY_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_CosQueueBucketFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_COS_QUEUE_BUCKET_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DiffservPolicyAttrMarkCos2valFeat) == 0)
  {
    componentId = L7_FLEX_QOS_COS_COMPONENT_ID;
    featureId = L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_FdbAgetimePerVlan) == 0)
  {
    componentId = L7_FDB_COMPONENT_ID;
    featureId = L7_FDB_AGETIME_PER_VLAN_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1xSetKeytxenabledFeat) == 0)
  {
    componentId = L7_DOT1X_COMPONENT_ID;
    featureId = L7_DOT1X_SET_KEYTXENABLED_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1xVlansAssignedFeat) == 0)
  {
    componentId = L7_DOT1X_COMPONENT_ID;
    featureId = L7_DOT1X_VLANASSIGN_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1xSetCntrldirFeat) == 0)
  {
    componentId = L7_DOT1X_COMPONENT_ID;
    featureId = L7_DOT1X_SET_CONTROLDIR_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1xMacBasedAuthFeat)==0) 
  {
      componentId = L7_DOT1X_COMPONENT_ID;
      featureId = L7_DOT1X_MACBASEDAUTH_FEATURE_ID;
      runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1pUsrPriPerIntfFeat) == 0)
  {
    componentId = L7_DOT1P_COMPONENT_ID;
    featureId = L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Dot1pFeat) == 0)
  {
    componentId = L7_DOT1P_COMPONENT_ID;
    featureId = L7_DOT1P_FEATURE_SUPPORTED;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_SshdFeat) == 0)
  {
    componentId = L7_FLEX_SSHD_COMPONENT_ID;
    featureId = L7_SSHD_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_SecureTransferFeat) == 0)
  {
    componentId = L7_FLEX_SSHD_COMPONENT_ID;
    featureId = L7_SSHD_SECURE_TRANSFER_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_SsltFeat) == 0)
  {
    componentId = L7_FLEX_SSLT_COMPONENT_ID;
    featureId = L7_SSLT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_L7DvlantagFeatSupported) == 0)
  {
    componentId = L7_DVLANTAG_COMPONENT_ID;
    featureId = L7_DVLANTAG_FEATURE_SUPPORTED;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_L7DvlantagCustidFeatId) == 0)
  {
    componentId = L7_DVLANTAG_COMPONENT_ID;
    featureId = L7_DVLANTAG_CUSTID_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_L7DvlantagEthertypeFeatId) == 0)
  {
    componentId = L7_DVLANTAG_COMPONENT_ID;
    featureId = L7_DVLANTAG_ETHERTYPE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_L7DvlantagEthertypePerIntfFeatId) == 0)
  {
    componentId = L7_DVLANTAG_COMPONENT_ID;
    featureId = L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_L7DvlanMultiTpidsFeatId) == 0)
  {
    componentId = L7_DVLANTAG_COMPONENT_ID;
    featureId = L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_L7NimJumboframesFeat) == 0)
  {
    componentId = L7_NIM_COMPONENT_ID;
    featureId = L7_NIM_JUMBOFRAMES_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_L7NimJumboframesPerIntfFeat) == 0 )
  {
    componentId = L7_NIM_COMPONENT_ID;
    featureId = L7_NIM_JUMBOFRAMES_PER_INTERFACE_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_L7FpsFeat) == 0 )
  {
    componentId = L7_FLEX_STACKING_COMPONENT_ID;
    featureId = L7_FPS_FEATURE_ID;   /* Front-panel stacking support */
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_L7FpsPortModeCfgFeat) == 0 )
  {
    componentId = L7_FLEX_STACKING_COMPONENT_ID;
    featureId = L7_FPS_PORT_MODE_CFG_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_L7FpsQosReplacementFeat) == 0 )
  {
    componentId = L7_FLEX_STACKING_COMPONENT_ID;
    featureId = L7_FPS_QOS_REPLACEMENT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_L7Dot1qIngressfilterFeatId) == 0 )
  {
    componentId = L7_DOT1Q_COMPONENT_ID;
    featureId = L7_DOT1Q_INGRESSFILTER_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_IgmpSnooping_1) == 0 )
  {
    componentId = L7_SNOOPING_COMPONENT_ID;
    featureId = L7_IGMP_SNOOPING_FEATURE_SUPPORTED;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_MldSnooping_1) == 0 )
  {
    componentId = L7_SNOOPING_COMPONENT_ID;
    featureId = L7_MLD_SNOOPING_FEATURE_SUPPORTED;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_IgmpSnoopingPerVlan) == 0 )
  {
    componentId = L7_SNOOPING_COMPONENT_ID;
    featureId = L7_IGMP_SNOOPING_FEATURE_PER_VLAN;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_MldSnoopingPerVlan) == 0 )
  {
    componentId = L7_SNOOPING_COMPONENT_ID;
    featureId = L7_MLD_SNOOPING_FEATURE_PER_VLAN;
    runCheck = L7_TRUE;
  }
  else if ( strcmp(featureDescription, pStrInfo_base_WebAppletCustomizatonFeat) == 0 )
  {
    componentId = L7_CLI_WEB_COMPONENT_ID;
    featureId = L7_WEB_APPLET_CUSTOMIZATON_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_MultipleProtPortGrps) == 0)
  {
    componentId = L7_PROTECTED_PORT_COMPONENT_ID;
    featureId = L7_PROTECTED_PORT_MAX_GROUPS;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlSipdipFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_SIPDIP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlSmacdmacFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_SMACDMAC_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlFirstfragFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlTcpfragFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_TCPFRAG_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlTcpflagFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_TCPFLAG_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlTcpflagSeqFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlTcpFinUrgPshFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlTcpOffsetFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_TCPOFFSET_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlTcpSynFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_TCPSYN_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlTcpSynFinFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlL4portFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_L4PORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlTcpportFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_TCPPORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlUdpportFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_UDPPORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlIcmpFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_ICMP_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlIcmpv4Feat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_ICMPV4_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlIcmpv6Feat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_ICMPV6_FEATURE_ID;
    runCheck = L7_TRUE;
  }

  else if (strcmp(featureDescription, pStrInfo_base_DosCntrlIcmpFragFeat) == 0)
  {
    componentId = L7_DOSCONTROL_COMPONENT_ID;
    featureId = L7_DOSCONTROL_ICMPFRAG_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_DirectedPortMirroring) == 0)
  {
    componentId = L7_PORT_MIRROR_COMPONENT_ID;
    featureId = L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_ProxyArp) == 0)
  {
    componentId = L7_IP_MAP_COMPONENT_ID;
    featureId = L7_IPMAP_FEATURE_PROXY_ARP;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_Multinetting) == 0)
  {
    componentId = L7_IP_MAP_COMPONENT_ID;
    featureId = L7_IPMAP_FEATURE_MULTINETTING;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_RejectRoute) == 0)
  {
    componentId = L7_IP_MAP_COMPONENT_ID;
    featureId = L7_IPMAP_FEATURE_REJECTROUTE;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_httpDownloadFeature) == 0)
  {
    componentId = L7_CLI_WEB_COMPONENT_ID;
    featureId = L7_HTTP_FILE_DOWNLOAD_FEATURE;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_HashModeWebFeature) == 0)
  {
    componentId = L7_DOT3AD_COMPONENT_ID;
    /*featureId = L7_DOT3AD_HASHMODE_FEATURE_ID;*/
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_HashModeWebFeature1) == 0)
  {
    componentId = L7_DOT3AD_COMPONENT_ID;
    featureId = L7_DOT3AD_HASHMODE_SA_VLAN_ETYPE_INTF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_HashModeWebFeature2) == 0)
  {
    componentId = L7_DOT3AD_COMPONENT_ID;
    featureId = L7_DOT3AD_HASHMODE_DA_VLAN_ETYPE_INTF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_HashModeWebFeature3) == 0)
  {
    componentId = L7_DOT3AD_COMPONENT_ID;
    featureId = L7_DOT3AD_HASHMODE_SADA_VLAN_ETYPE_INTF_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_HashModeWebFeature4) == 0)
  {
    componentId = L7_DOT3AD_COMPONENT_ID;
    featureId = L7_DOT3AD_HASHMODE_SIP_SPORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_HashModeWebFeature5) == 0)
  {
    componentId = L7_DOT3AD_COMPONENT_ID;
    featureId = L7_DOT3AD_HASHMODE_DIP_DPORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_HashModeWebFeature6) == 0)
  {
    componentId = L7_DOT3AD_COMPONENT_ID;
    featureId = L7_DOT3AD_HASHMODE_SIP_DIP_SPORT_DPORT_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_ip_source_guard) == 0)
  {
    componentId = L7_DHCP_SNOOPING_COMPONENT_ID;
    featureId = L7_DHCP_SNOOPING_IPSG;
    runCheck = L7_TRUE;
  }
  else if (strcmp(featureDescription, pStrInfo_base_AutoInstallFeature) == 0)
  {
    componentId = L7_AUTO_INSTALL_COMPONENT_ID;
    featureId = L7_AUTOINSTALL_FEATURE_ID;
    runCheck = L7_TRUE;
  }
  else
  {
    runCheck = L7_FALSE;
  }

  if (runCheck == L7_TRUE)
  {
    rc = usmDbFeaturePresentCheck(unit, componentId, featureId);
  }
  else
  {
    rc = L7_FALSE;
  }

  return rc;
}

/**************************************************************************
 * Function: usmWebComponentPresent1
 *
 * Description:
 *     Return Opening Comment if the specified component is not present.
 *
 * Parameters:
 *     compName : Component name string
 *
 * Return:
 *     Opening multi-line comment tag if component not present
 *     Nothing if component present
 *
 * Notes:
 *     This fn is invoked by an emweb macro and is used to comment
 *     out blocks of html code
 *************************************************************************/

L7_char8 *usmWebComponentPresentHtml1(L7_char8 * compName)
{
  L7_uint32 runCheck = L7_FALSE;
  L7_uint32 componentId=0;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (strcmp(compName, pStrInfo_base_Snmp2) == 0)
  {
    componentId = L7_SNMP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }

  memset(webBuffer, 0, sizeof(webBuffer));
  if (runCheck == L7_TRUE)
  {
    if (usmDbComponentPresentCheck(unit, componentId) == L7_FALSE)
    {
      osapiSnprintf(webBuffer, sizeof(webBuffer), "<!--");
    }
  }

  return webBuffer;
}

/**************************************************************************
 * Function: usmWebComponentPresent2
 *
 * Description:
 *     Return "Closing comment if the specified component is not present.
 *
 * Parameters:
 *     compName : Component name string
 *
 * Return:
 *     Closing multi-line comment tag if component not present
 *     Nothing if component present
 *
 * Notes:
 *     This fn is invoked by an emweb macro and is used to comment
 *     out blocks of html code
 *************************************************************************/

L7_char8 *usmWebComponentPresentHtml2(L7_char8 * compName)
{
  L7_uint32 runCheck = L7_FALSE;
  L7_uint32 componentId=0;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (strcmp(compName, pStrInfo_base_Snmp2) == 0)
  {
    componentId = L7_SNMP_COMPONENT_ID;
    runCheck = L7_TRUE;
  }

  memset(webBuffer, 0, sizeof(webBuffer));
  if (runCheck == L7_TRUE)
  {
    if (usmDbComponentPresentCheck(unit, componentId) == L7_FALSE)
    {
      osapiSnprintf(webBuffer, sizeof(webBuffer), "-->");
    }
  }

  return webBuffer;
}

/**************************************************************************
 * Function: usmWebUserAccessGet
 *
 * Description:
 *     Return the access priveledges for the specified user
 *
 * Parameters:
 *
 * Return:
 *
 * Notes:
 *************************************************************************/

L7_uint32 usmWebUserAccessGet(EwsContext ewsContext)
{
  L7_uint32      userAccess = USMWEB_UNKNOWN;

  if ((ewsContext->session != NULL) && (ewsContext->session->active == L7_TRUE))
  {
    switch (ewsContext->session->access_level)
        {
          case L7_LOGIN_ACCESS_READ_WRITE:  /* READWRITE users get access to it all */
      userAccess = USMWEB_READWRITE;
            break;
          case L7_LOGIN_ACCESS_READ_ONLY:
      userAccess = USMWEB_READONLY;
            break;
          case L7_LOGIN_ACCESS_NONE:
      userAccess = USMWEB_READONLY;
            break;
          default:
      userAccess = USMWEB_READONLY;
            break;
        }
      }

  return userAccess;
}

/**************************************************************************
 * Function: usmWebInitialize
 *
 * Description:
 *     Initialization of various elements for the web code
 *
 * Parameters:
 *
 * Return:
 *
 * Notes:
 *************************************************************************/

void usmWebInitialize( void )
{
}

/*********************************************************************
*
* @purpose convert an elapsed  time stamp into a string
*
* @param buffer pointer to a string buffer
* @param timeticks timestamp
*
* @returns none
*
* @notes The string buffer is filled with the time in the format of
*        days, hours and minutes
*
* @end
*
*********************************************************************/
void usmWebUtilCvtTime2String(L7_char8 * buffer, L7_uint32 timeticks)
{
  sprintf(buffer,"%d %s, %d %s, %d %s",
          timeticks/(24 * 60 * 60 * 100),
      pStrInfo_base_Days_2,
      (timeticks/(60*60*100) ) % 24,
      pStrInfo_common_Hours_2,
      (timeticks/(60*100) ) % 60,
      pStrInfo_base_Mins );

}

/*********************************************************************
*
* @purpose Get the number of active interfaces in the box
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
L7_uint32 usmWebNumActiveDot1qInterfacesGet(L7_uint32 unit)
{
  L7_RC_t   rc;
  L7_uint32 numInterfaces;
  L7_uint32 intIfNum, prevIntf;

  numInterfaces = 0;

  rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_L2_TUNNEL_INTF, 
                                 0, &intIfNum);

  while (rc == L7_SUCCESS)
  {
    numInterfaces++;
    prevIntf = intIfNum;
    rc = usmDbIntIfNumTypeNextGet(unit,  USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_L2_TUNNEL_INTF, 
                                  0, prevIntf, &intIfNum);

  }

  return numInterfaces;
}

/*********************************************************************
*
* @purpose  Get the Port Options for slots 0 to 3 slots
*
* @param   unit            L7_uint32 unit index
*
* @returns  usmWebPageBuf  L7_char8 Array of port options for each slot
*                          present
* @notes    none
*
* @end
*********************************************************************/
L7_char8 *usmWebPortOptionsGet(L7_uint32 unit)
{

  L7_uint32  numPorts;
  L7_uint32  slotNum;
  L7_uint32  i;
  L7_uint32  slotPop[L7_MAX_SLOTS_PER_UNIT];

  memset(usmWebPageBuf, 0, sizeof(usmWebPageBuf));
  usmDbSlotsPopulatedGet(unit, slotPop);
  for (slotNum=0; slotNum < L7_MAX_PHYSICAL_SLOTS_PER_UNIT+1; slotNum++)
  {
    if (slotPop[slotNum] == L7_TRUE)
    {
      osapiSnprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_Case, slotNum);
      osapiSnprintfcat(usmWebPageBuf, sizeof(usmWebPageBuf), buf);
/*      strcat(usmWebPageBuf, buf); */
      if (usmDbPortsPerSlotGet(unit, slotNum, &numPorts) != L7_SUCCESS)
      {
        return L7_NULLPTR;
      }
      osapiSnprintfcat(usmWebPageBuf, sizeof(usmWebPageBuf), pStrInfo_base_AbNewArrayAll);
/*      strcat(usmWebPageBuf, pStrInfo_base_AbNewArrayAll); */
      for (i=0; i<numPorts; i++)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_P, i+1);
        osapiSnprintfcat(usmWebPageBuf, sizeof(usmWebPageBuf), buf);
/*        strcat(usmWebPageBuf, buf); */
      }
      strcatAddBlanks (0, 1, 0, 0, L7_NULLPTR, usmWebPageBuf, pStrErr_common_AclDelSyntax);
    }
  }

  osapiSnprintfcat(usmWebPageBuf, sizeof(usmWebPageBuf), pStrInfo_base_DeflAbNewArrayErrorBreak);
/*  strcat(usmWebPageBuf, pStrInfo_base_DeflAbNewArrayErrorBreak); */

  return usmWebPageBuf;
}

/*********************************************************************
*
* @purpose Generate the list of valid slots for the emweb iterate slot
*          select
* @param
*
* @returns
*
* @end
*
*********************************************************************/

void *ewaFormSelect_slotIncludeLag(EwsContext             context,
                                   EwsFormSelectOptionP   optionp,
    void * iterator )
{
  static L7_uint32 slotNum;
  static L7_uint32 selectedSlot;
  static L7_uint32 slotPop[L7_MAX_SLOTS_PER_UNIT];
  L7_uint32 slotPresent;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  if (iterator == NULL)
  {
    slotNum = 0;
    usmDbSlotsPopulatedGet(unit, slotPop);

    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(appInfo));
      if (appInfo.data[3] == L7_TRUE)
      {
        selectedSlot = appInfo.data[1];
    }
    }

  }
  else
  {
    slotNum++;
  }

  slotPresent = L7_FALSE;
  if (slotNum < L7_MAX_PHYSICAL_SLOTS_PER_UNIT+1)
  {
    while (slotPresent == L7_FALSE)
    {
      if (slotPop[slotNum] == L7_TRUE)
      {
        slotPresent = L7_TRUE;
        optionp->valuep = (void *) &slotNum;
        osapiSnprintf(buf, sizeof buf, pStrInfo_base_Slot_1, slotNum);
        bufChoice = ewsContextNetHandle(context)->buffer;
        memset(bufChoice, 0, APP_BUFFER_SIZE);
        osapiStrncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
        optionp->choice = bufChoice;
        if (selectedSlot == slotNum)
        {
          optionp->selected = TRUE;
        }
        else
        {
          optionp->selected = FALSE;
        }
      }
      else
      {
        if (slotNum < (L7_MAX_PHYSICAL_SLOTS_PER_UNIT-1+1))
        {
          slotNum++;
        }
        else
        {
          break;
        }
      }
    }
    if (slotPresent == L7_TRUE)
    {
      return (void *) optionp;
    }
    else
    {
      slotNum++;
    }
  }

  return NULL;
}

/*********************************************************************
*
* @purpose Generate the list of valid slots for the emweb iterate slot select
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/

void *ewaFormSelect_genSlot(EwsContext             context,
                            EwsFormSelectOptionP   optionp,
    void * iterator )
{
  static L7_uint32 slotNum;
  static L7_uint32 selectedSlot;
  static L7_uint32 slotPop[L7_MAX_SLOTS_PER_UNIT];
  L7_uint32 slotPresent;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  if (iterator == NULL)
  {
    slotNum = 0;
    usmDbSlotsPopulatedGet(unit, slotPop);

    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      if (appInfo.data[3] == L7_TRUE)
      {
        selectedSlot = appInfo.data[1];
    }
    }

  }
  else
  {
    slotNum++;
  }

  slotPresent = L7_FALSE;
  if (slotNum < L7_MAX_PHYSICAL_SLOTS_PER_UNIT)
  {
    while (slotPresent == L7_FALSE)
    {
      if (slotPop[slotNum] == L7_TRUE)
      {
        slotPresent = L7_TRUE;
        optionp->valuep = (void *) &slotNum;
        osapiSnprintf(buf, sizeof buf, pStrInfo_base_Slot_1, slotNum);
        bufChoice = ewsContextNetHandle(context)->buffer;
        memset(bufChoice, 0, APP_BUFFER_SIZE);
        osapiStrncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
        optionp->choice = bufChoice;
        if (selectedSlot == slotNum)
        {
          optionp->selected = TRUE;
        }
        else
        {
          optionp->selected = FALSE;
        }
      }
      else
      {
        if (slotNum < (L7_MAX_PHYSICAL_SLOTS_PER_UNIT-1))
        {
          slotNum++;
        }
        else
        {
          break;
        }
      }
    }
    if (slotPresent == L7_TRUE)
    {
      return (void *) optionp;
    }
    else
    {
      slotNum++;
    }
  }

  return NULL;
}

void usmWebUspStrGet(L7_uint32 unit, L7_uint32 u, L7_uint32 s, L7_uint32 p, L7_char8 * buf, L7_uint32 size)
{
  if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
  {
    osapiSnprintf(buf, size, "%d/%d/%d", u, s, p);
  }
  else
  {
    osapiSnprintf(buf, size, "%d/%d", s, p);
  }
}

/*********************************************************************
*
* @purpose Generate the list of configured physical and include
*          Logical Router Ports as well.  Exclude lag members.
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *ewaFormSelect_phys_plus_rtr_port(EwsContext             context,
                                       EwsFormSelectOptionP   optionp,
    void * iterator )
{
  static L7_uint32 itrValid;
  static L7_uint32 sel_intifnum;
  static L7_uint32 intIfNum;
  L7_uint32 sysIntfType;
  L7_uint32 status;
  L7_uint32 lag;
  static L7_uint32 unit;
  L7_BOOL   intfFound;
  L7_char8  buf[APP_BUFFER_SIZE];  /* slot.port */
  L7_char8 * bufChoice;
  L7_uint32 u,s,p;
  L7_RC_t   rc;
  EwaNetHandle     net;
  usmWeb_AppInfo_t appInfo;

  itrValid = L7_FALSE;

  if (iterator == L7_NULL)
  {
    unit = usmDbThisUnitGet();
    rc = usmDbValidIntIfNumFirstGet(&intIfNum);
    sel_intifnum = 0;
    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      sel_intifnum = appInfo.data[0];
    }
  }
  else
  {
    rc = usmDbValidIntIfNumNext(intIfNum, &intIfNum);
  }

  if (rc != L7_SUCCESS)
  {
    return L7_NULL;
  }

  intfFound = L7_FALSE;

  while ((intfFound == L7_FALSE) && (rc == L7_SUCCESS))
  {
    if (usmDbIntfTypeGet(intIfNum, &sysIntfType) == L7_SUCCESS)
    {
      status = usmDbDot3adIntfIsMemberGet(unit, intIfNum, &lag);
      if ( ( ((sysIntfType == L7_PHYSICAL_INTF) &&
              (usmDbComponentPresentCheck(unit, L7_IP_MAP_COMPONENT_ID) == L7_TRUE)) ||
             (sysIntfType == L7_LOGICAL_VLAN_INTF)    ) &&
           (status != L7_SUCCESS))
      {
        intfFound = L7_TRUE;
      }
      else
      {
        rc = usmDbValidIntIfNumNext(intIfNum, &intIfNum);
      }
    }
  }

  if (intfFound == L7_TRUE)
  {
    if (sel_intifnum == 0)
    {                        /* If no selection use the first valid routing interface */
      sel_intifnum =  intIfNum;
    }

    rc = usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    optionp->valuep = (void *) &intIfNum;
    usmWebUspStrGet(unit, u, s, p, buf, sizeof(buf));

    bufChoice = ewsContextNetHandle(context)->buffer;
    memset(bufChoice, 0, APP_BUFFER_SIZE);
    osapiStrncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
    optionp->choice = bufChoice;
    if (intIfNum == sel_intifnum)
    {
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }
    itrValid = L7_TRUE;
  }

  if (itrValid == L7_TRUE)
  {
    return (void *) optionp;
  }
  else
  {
    return L7_NULL;
  }

}

/*********************************************************************
*
* @purpose Generate the list of configured physical ports and
*           lags as well.
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *ewaFormSelect_phys_port(EwsContext             context,
                              EwsFormSelectOptionP   optionp,
    void * iterator )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  static L7_uint32 selected_index;
  static L7_uint32 intIfNum;
  L7_uint32 u, s, p, prevIntf;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_RC_t rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  if (iterator == L7_NULL)
  {
    /*setting selected index to first MSTI other than CIST*/
    prevIntf = 0;
    selected_index = 0;

    if (usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF, 0,
                                  &intIfNum) != L7_SUCCESS)
    {
      return L7_NULL;
    }

    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_index = appInfo.data[0];
    }
    else
    {
      selected_index = intIfNum;
    }

    rc = usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    if (rc == L7_SUCCESS)
    {
      optionp->valuep = (void *) &intIfNum;
      usmWebUspStrGet(unit, u, s, p, buf, sizeof(buf));

      bufChoice = ewsContextNetHandle(context)->buffer;
      memset(bufChoice, 0, APP_BUFFER_SIZE);
      osapiStrncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
      optionp->choice = bufChoice;
      if (selected_index == intIfNum)
      {
        optionp->selected = TRUE;
      }
      else
      {
        optionp->selected = FALSE;
      }
      return (void *) optionp;
    }
  }

  /***************************** if iterator != NULL *********************************/

  prevIntf = intIfNum;
  rc = usmDbIntIfNumTypeNextGet(unit,  USM_PHYSICAL_INTF, 0,
                                prevIntf, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    if (rc == L7_SUCCESS)
    {
      optionp->valuep = (void *) &intIfNum;
      usmWebUspStrGet(unit, u, s, p, buf, sizeof(buf));

      bufChoice = ewsContextNetHandle(context)->buffer;
      memset(bufChoice, 0, APP_BUFFER_SIZE);
      osapiStrncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
      optionp->choice = bufChoice;
      if (intIfNum == selected_index)
      {
        optionp->selected = TRUE;
      }
      else
      {
        optionp->selected = FALSE;
      }
      return (void *) optionp;
    }
  }
  else
  {
    return L7_NULL;
  }
  return L7_NULL;

}

/*********************************************************************
*
* @purpose Generate the list of configured physical ports and
*           lags as well.
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *ewaFormSelect_phys_plus_lag_port(EwsContext             context,
                                       EwsFormSelectOptionP   optionp,
    void * iterator )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  static L7_uint32 selected_index;
  static L7_uint32 intIfNum;
  L7_uint32 u, s, p, prevIntf;
  L7_uchar8 buf[APP_BUFFER_SIZE];    /* 5 used to cover the length of the slot.port string*/
  L7_char8 * bufChoice;
  L7_RC_t rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  if (iterator == L7_NULL)
  {
    /*setting selected index to first MSTI other than CIST*/
    prevIntf = 0;
    selected_index = 0;

    if (usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0,
                                  &intIfNum) != L7_SUCCESS)
    {
      return L7_NULL;
    }

    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_index = appInfo.data[0];
    }
    else
    {
      selected_index = intIfNum;
    }

    rc = usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    if (rc == L7_SUCCESS)
    {
      optionp->valuep = (void *) &intIfNum;
      usmWebUspStrGet(unit, u, s, p, buf, sizeof(buf));

      bufChoice = ewsContextNetHandle(context)->buffer;
      memset(bufChoice, 0, APP_BUFFER_SIZE);
      osapiStrncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
      optionp->choice = bufChoice;
      if (selected_index == intIfNum)
      {
        optionp->selected = TRUE;
      }
      else
      {
        optionp->selected = FALSE;
      }
      return (void *) optionp;
    }
  }

  /***************************** if iterator != NULL *********************************/

  prevIntf = intIfNum;
  rc = usmDbIntIfNumTypeNextGet(unit,  USM_PHYSICAL_INTF | USM_LAG_INTF, 0,
      prevIntf, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
    if (rc == L7_SUCCESS)
    {
      optionp->valuep = (void *) &intIfNum;
      usmWebUspStrGet(unit, u, s, p, buf, sizeof(buf));

      bufChoice = ewsContextNetHandle(context)->buffer;
      memset(bufChoice, 0, APP_BUFFER_SIZE);
      osapiStrncpy(bufChoice, buf, APP_BUFFER_SIZE-1);
      optionp->choice = bufChoice;
      if (intIfNum == selected_index)
      {
        optionp->selected = TRUE;
      }
      else
      {
        optionp->selected = FALSE;
      }
      return (void *) optionp;
    }
  }
  else
  {
    return L7_NULL;
  }
  return L7_NULL;

}

/**************************************************************************
 * Function: usmWebGarpErrorGet
 *
 * Description:
 *     Returns error info to garp
 *
 * Parameters:
 *
 * Return:
 *
 * Notes:
 *************************************************************************/
L7_uint32 usmWebGarpErrorGet()
{
  return currentGarpError;
}

/**************************************************************************
 * Function: usmWebGarpErrorSet
 *
 * Description:
 *     Sets the error info for garp
 *
 * Parameters:
 *
 * Return:
 *
 * Notes:
 *************************************************************************/

void usmWebGarpErrorSet(L7_uint32 error)
{
  currentGarpError = error;
}

L7_char8 *usmWebLogFormat(L7_char8 * msg)
{
  sprintf(webBuffer, "\n<tr><td width=\"5px\">&nbsp;&nbsp;&nbsp;&nbsp;</td><td class=\"msgtabledata\"> %s </td></tr>\n", msg);
  return webBuffer;
}

L7_char8 *usmWebEventLogFormat(EwsContext ewsContext, L7_char8 * msg)
{
  L7_char8 * tmp[30];
  L7_int32 count, cnt;
  L7_char8 * buf;
  L7_BOOL          watchdog;
  L7_char8         tmpbuf[APP_BUFFER_SIZE];
  EwaNetHandle     net;
  usmWeb_AppInfo_t appInfo;

  net = ewsContextNetHandle(ewsContext);
  if (net->app_pointer != L7_NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
  }

  buf = ewsContextNetHandle(ewsContext)->buffer;
  memset(buf, 0, APP_BUFFER_SIZE);
  memset(tmpbuf, 0, APP_BUFFER_SIZE);
  memset(&webBuffer, 0, sizeof(webBuffer));
  osapiStrncat(tmpbuf, "\n<tr>", APP_BUFFER_SIZE-strlen(tmpbuf)-1);

  count = 0;
  cnt = 0;
  watchdog = L7_FALSE;

  tmp[count] = strtok(msg, " ");

  while (tmp[count] != NULL)
  {
    count++;
    tmp[count] = strtok(NULL, " ");
  }

  while (tmp[cnt] != NULL)
  {
    if (cnt == (count-4))
    {
      if (tmp[cnt] != NULL && tmp[cnt+1] != NULL && tmp[cnt+2] != NULL && tmp[cnt+3] != NULL)
      {
        osapiSnprintf(webBuffer, sizeof(webBuffer), "<td class=\"msgtabledata\">%s  %s %s %s</td>",
            tmp[cnt], tmp[cnt+1], tmp[cnt+2], tmp[cnt+3]);
        cnt += 4;
        osapiStrncat(tmpbuf, webBuffer, APP_BUFFER_SIZE-strlen(tmpbuf)-1);
      }
      else
      {
        cnt++;
      }
    }
    else
    {
      /* This is kind of a hack, but this will take care of the one line
         watchdog entries that sometimes show up */
      if (watchdog)
      {
        osapiSnprintf(webBuffer, sizeof(webBuffer),
            "<td class=\"msgtabledata\">&nbsp;</td><td class=\"msgtabledata\">&nbsp;</td>");
      }
      else
      {
        osapiSnprintf(webBuffer, sizeof(webBuffer), "<td class=\"msgtabledata\">%s</td>", tmp[cnt]);
      }
      osapiStrncat(tmpbuf, webBuffer, APP_BUFFER_SIZE-strlen(tmpbuf));

      if (!strncmp(pStrInfo_base_Watchdog, tmp[cnt], 8))
      {
        watchdog = L7_TRUE;
      }
      else
      {
        watchdog = L7_FALSE;
      }

      cnt++;
    }
  }

  osapiStrncat(tmpbuf, "</tr>\n", APP_BUFFER_SIZE-strlen(tmpbuf)-1);
  strcpy(buf, tmpbuf);

  return buf;
}

/*********************************************************************
*
* @purpose Get first MST Id and return the list of MST instances
*
* @param context  EmWeb/Server request context handle
*
* @param unit  Unit Index
*
* @param buf pointer to the list of MST ids
*
* @param val pointer to number of MST ids
*
* @returns mstid id of first MSTI other than CIST
*
* @end
*
*********************************************************************/
L7_uint32 usmWebDot1sMstiListFirstGet(L7_uint32 unit, L7_uint32 * buf, L7_uint32 * val)
{
  L7_RC_t rc;
  L7_uint32 mstid, count;

  mstid = DOT1S_CIST_ID;
  count = L7_MAX_MULTIPLE_STP_INSTANCES + 1;
  rc = usmDbDot1sInstanceList(unit, buf, &count);
  if (rc == L7_SUCCESS)
  {
    *val = count;
    if (count > 1)
    {
      mstid = buf[1];
    }
  }

  return mstid;
}

/*********************************************************************
*
* @purpose Get Bridge Id in display form from an integer
*
* @param context  EmWeb/Server request context handle
*
* @param *buf pointer to where the bridge ID is stored for return
*
* @param *temp pointer to where bridge id is stored as L7_uchar8
*
* @param val length of the string containing the bridge id
*
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/

L7_RC_t usmWebToDot1sBridgeId(L7_uchar8 * buf, L7_uchar8 * temp, L7_uint32 val)
{
  L7_uint32 i;

  sprintf(buf, "%02x", temp[0]);
  for (i = 1; i < val; i++)
  {
    sprintf(buf, "%s:%02x", buf, temp[i]);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get Port Id in display form from an integer
*
* @param context  EmWeb/Server request context handle
*
* @param *buf pointer to where the port ID is stored for return
*
* @param val Port ID as an integer
*
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/

L7_RC_t usmWebToDot1sPortId(L7_uchar8 * buf, L7_uint32 val)
{
  L7_uint32 val1, val2;

  /*the port ID is formed from the last 4 bytes of interface num in
  hex format with a ':' in the middle*/
  val1 = (val >> 8) & 0x000000ff;
  val2 = val & 0x000000ff;
  sprintf(buf, "%02x:%02x", val1, val2);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the port spanning tree mode
*
* @param context  EmWeb/Server request context handle
*
* @param unit  Unit Index
*
* @param intIfNum interface number for port
*
* @param stp_mode specifies whether port is to be spanning tree
*
*        disabled or enabled
*
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/

L7_RC_t usmWebDot1sIfSpanningTreeModeSet(L7_uint32 unit, L7_uint32 intIfNum, L7_BOOL stp_mode)
{
  L7_RC_t rc;
  L7_uint32 i;

  if (intIfNum != L7_ALL_INTERFACES)
  {
    if (usmDbDot1sIsValidIntf(unit, intIfNum) == L7_TRUE)
    {
      rc = usmDbDot1sPortStateSet(unit, intIfNum, stp_mode);
    }
    else
    {
      rc = L7_FAILURE;
    }
    return rc;
  }

  rc = L7_SUCCESS;
  for (i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    if (usmDbDot1sIsValidIntf(unit, i) == L7_TRUE)
    {
      if (usmDbDot1sPortStateSet(unit, i, stp_mode) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose To get the u/s/p or s/p formated string
*
* @param intIfNum Internal interface number of the port in quetion
*
* @returns L7_char8 u/s/p Formatted string
*
* @end
*
*********************************************************************/
L7_char8 *usmWebPortDisplayHelp(L7_uint32 intIfNum)
{
  L7_uint32 u, s, p, unit;
  static L7_char8 buf[USMWEB_UNIT_SLOT_PORT_SIZE];

  unit = usmDbThisUnitGet();
  if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
  {
    sprintf (buf, strUtilUspGet (unit, u, s, p, L7_NULLPTR, 0));
  }
    else
  {
    sprintfAddBlanks (0, 0, 0, 0, pStrErr_common_Error, buf,  pStrInfo_common_EmptyString);
  }
  return buf;
}

/*********************************************************************
*
* @purpose To convert the signed integer to a string
*
* @param val  signed integer value
*
* @returns L7_char8 ptr to resulting string
*
* @end
*
*********************************************************************/
L7_char8 *usmWebIntegerToString(L7_int32 val)
{
  static L7_char8 buf[20];
  osapiSnprintf(buf, sizeof(buf), "%d", val);
  return buf;
}

/*********************************************************************
*
* @purpose To get the signed integer input range help string
*
* @param min  minimum value in range
* @param max  maximum value in range
*
* @returns L7_char8 (min to max)
*
* @end
*
*********************************************************************/
L7_char8 *usmWebIntegerRangeHelp(L7_int32 min, L7_int32 max)
{
  static L7_char8 buf[40];
  osapiSnprintf(buf, sizeof(buf), " (%d %s %d)", min, pStrInfo_common_To_5, max);
  return buf;
}

/*********************************************************************
*
* @purpose To get the unsigned integer input range help string
*
* @param min  minimum value in range
* @param max  maximum value in range
*
* @returns L7_char8 (min to max)
*
* @end
*
*********************************************************************/
L7_char8 *usmWebUnsignedIntegerRangeHelp(L7_uint32 min, L7_uint32 max)
{
  static L7_char8 buf[40];
  osapiSnprintf(buf, sizeof(buf), " (%u %s %u)", min, pStrInfo_common_To_5, max);
  return buf;
}

/*********************************************************************
*
* @purpose Set an error message in the appInfo pointer
*
* @param tokenNumber  string index in english.h
* @param *appInfo      pointer to web page info structure
*
* @returns none
*
* @end
*
*********************************************************************/
void usmWebInputFieldSetFailedErrMsg(L7_char8 * tokMsg, usmWeb_AppInfo_t * appInfo)
{
  L7_char8  errMsg[USMWEB_ERROR_MSG_SIZE];

  if (appInfo != NULL)
  {
    appInfo->err.err_flag = L7_TRUE;
    osapiSnprintfAddBlanks (0, 1, 0, 0, pStrErr_common_FailedToSet, errMsg, sizeof(errMsg), pStrErr_common_FailedToSet_1, tokMsg);
    osapiStrncat(appInfo->err.msg, errMsg, USMWEB_ERROR_MSG_SIZE-strlen(appInfo->err.msg)-1);
  }
}

/*********************************************************************
*
* @purpose Set an error message in the appInfo pointer
*
* @param tokenNumber  string index in english.h
* @param *appInfo      pointer to web page info structure
*
* @returns none
*
* @end
*
*********************************************************************/
void usmWebInputFieldGetFailedErrMsg(L7_char8 * tokMsg, usmWeb_AppInfo_t * appInfo)
{
  L7_char8  errMsg[USMWEB_ERROR_MSG_SIZE];

  if (appInfo != NULL)
  {
    appInfo->err.err_flag = L7_TRUE;
    osapiSnprintfAddBlanks (0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrErr_common_FailedToGet, tokMsg);
    osapiStrncat(appInfo->err.msg, errMsg, USMWEB_ERROR_MSG_SIZE-strlen(appInfo->err.msg)-1);
  }
}

/*********************************************************************
*
* @purpose Set an error message in the appInfo pointer
*
* @param tokenNumber  string index in english.h
* @param *appInfo      pointer to web page info structure
*
* @returns none
*
* @end
*
*********************************************************************/
void usmWebInputFieldGenericErrMsg(L7_char8 * tokMsg, usmWeb_AppInfo_t * appInfo)
{
  L7_char8  errMsg[USMWEB_ERROR_MSG_SIZE];

  if (appInfo != NULL)
  {
    appInfo->err.err_flag = L7_TRUE;
    osapiSnprintfAddBlanks (0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrErr_common_CfgAclsStringFmt, tokMsg);
    osapiStrncat(appInfo->err.msg, errMsg, USMWEB_ERROR_MSG_SIZE-strlen(appInfo->err.msg)-1);
  }
}

/*********************************************************************
*
* @purpose check user's input to determine if it is alphanumeric, specifically,
*                    'a'-'z', 'A'-'Z', '0'-'9', '-', '_', '.' or '/'
*
*
* @param *input         the argument entered by user
*
* @returns              L7_SUCCESS if valid
* @returns                          L7_FAILURE if invalid
*
* @notes                none.
*
*
* @end
*
********************************************************************/
L7_RC_t webIsStringAlphaNum(L7_char8 * input)
{
  L7_int32 i, len, val;

  len = strlen(input);
  for (i=0; i<len; i++)
  {
    val = input[i];
    if (val == '\0')
    {
      break;
    }
    if (!(isalnum(val) || val=='-' || val=='_' || val=='.' || val=='/')) /* slash is valid for file paths */
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Check an HTML form field for errors
*
* @param context      web context
* @param *fieldName   name of the HTML field to check
* @param tokenNumber  string index in english.h
* @param *appInfo      pointer to web page info structure
* @param lowerLimit   minumum value for integer field
* @param upperLimit   maximum value for integer field
*
* @returns L7_SUCCESS  if no errors were found
* @returns L7_FAILURE  if an error is found
*
* @end
*
*********************************************************************/
L7_RC_t usmWebInputFieldCheck(EwsContext context, L7_char8 * fieldName, L7_char8 * tokMsg,
    usmWeb_AppInfo_t * appInfo, L7_uint32 lowerLimit, L7_uint32 upperLimit)
{
  L7_char8  errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_RC_t   errorFlag = L7_SUCCESS;
  L7_uchar8 * status = L7_NULLPTR;
  L7_uint32 * valueUint = L7_NULLPTR;
  L7_int32 * valueInt = L7_NULLPTR;
  L7_char8 * valueStr = L7_NULLPTR;
  L7_uint32 element;
  const EwFormField * fieldp;
  EwFieldType fieldType = 0;
  L7_BOOL foundElement = L7_FALSE;
  const EwFormEntry * entryp = context->form_entry;
  L7_BOOL bval;

  memset(errMsg, 0, sizeof(errMsg));
  if (fieldName == NULL || appInfo == NULL)
  {
    return L7_FAILURE;
  }

  for (element = 0, fieldp = entryp->field_list;
      element < entryp->field_count;
      element++, fieldp++)
  {
    if (fieldp->name == NULL)
    {
      continue;
    }

    EMWEB_STRCMP(bval,fieldName,fieldp->name);
    if (!bval)
    {
      fieldType = fieldp->field_type;
      if (fieldType == ewFieldTypeDecimalUint)
      {
        valueUint  = (L7_uint32 *)((uint8 *)context->form + fieldp->value_offset);
      }
      else if (fieldType == ewFieldTypeDecimalInt)
      {
        valueInt  = (L7_int32 *)((uint8 *)context->form + fieldp->value_offset);
      }
      else if (fieldType == ewFieldTypeText)
      {
        valueStr  = *(L7_char8 * *)((uint8 *)context->form + fieldp->value_offset);
      }

      status = (L7_uchar8 *)(context->form + fieldp->status_offset);
      foundElement = L7_TRUE;
      break;
    }
  }

  if (foundElement == L7_FALSE)
  {
    return L7_FAILURE;
  }

  if (!(*status & EW_FORM_RETURNED))
  {
    errorFlag = L7_FAILURE;
    appInfo->err.err_flag = L7_TRUE;
    osapiSnprintfAddBlanks (0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg),
        pStrErr_common_FieldInvalidFmt, tokMsg);
    osapiStrncat(appInfo->err.msg, errMsg, USMWEB_ERROR_MSG_SIZE-strlen(appInfo->err.msg)-1);
  }
  else if (*status & EW_FORM_PARSE_ERROR)
  {
    errorFlag = L7_FAILURE;
    appInfo->err.err_flag = L7_TRUE;
    osapiSnprintfAddBlanks (0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg),
        pStrErr_common_FieldInvalidFmt, tokMsg);
    osapiStrncat(appInfo->err.msg, errMsg, USMWEB_ERROR_MSG_SIZE-strlen(appInfo->err.msg)-1);
  }
  if (errorFlag == L7_FAILURE)
  {
    return errorFlag;
  }

  switch (fieldType)
  {
    case ewFieldTypeText:
      if (webIsStringAlphaNum(valueStr) != L7_SUCCESS)
      {
        errorFlag = L7_FAILURE;
        appInfo->err.err_flag = L7_TRUE;
        osapiSnprintfAddBlanks (0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrErr_common_Alnum, tokMsg);
        osapiStrncat(appInfo->err.msg, errMsg, USMWEB_ERROR_MSG_SIZE-strlen(appInfo->err.msg)-1);
      }
      break;
    case ewFieldTypeDecimalUint:
      if (lowerLimit != upperLimit && (*valueUint < lowerLimit || *valueUint > upperLimit))
      {
        errorFlag = L7_FAILURE;
        appInfo->err.err_flag = L7_TRUE;
        osapiSnprintfAddBlanks (0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg),
            pStrErr_common_FieldOutOfRangeUint, *valueUint, tokMsg, lowerLimit, upperLimit);
        osapiStrncat(appInfo->err.msg, errMsg, USMWEB_ERROR_MSG_SIZE-strlen(appInfo->err.msg)-1);
      }
      break;
    case ewFieldTypeDecimalInt:
      if (lowerLimit != upperLimit && (*valueInt < lowerLimit || *valueInt > upperLimit))
      {
        errorFlag = L7_FAILURE;
        appInfo->err.err_flag = L7_TRUE;
        osapiSnprintfAddBlanks (0, 1, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg),
            pStrErr_common_FieldOutOfRange, *valueInt, tokMsg, lowerLimit, upperLimit);
        osapiStrncat(appInfo->err.msg, errMsg, USMWEB_ERROR_MSG_SIZE-strlen(appInfo->err.msg)-1);
      }
      break;
    default:
      break;
  }

  return errorFlag;
}


/*********************************************************************
*
* @purpose To get the character input range help string
*
* @param min  minimum value in range
* @param max  maximum value in range
*
* @returns L7_char8 (min to max)
*
* @end
*
*********************************************************************/
L7_char8 *usmWebAlphanumericCharacterRangeHelp(L7_uint32 min, L7_uint32 max)
{
  static L7_char8 buf[40];
  osapiSnprintf(buf, sizeof(buf), " (%u %s %u %s)", min, pStrInfo_common_To_5, max, pStr_Info_common_AlphanumericCharacters);
  return buf;
}

/*********************************************************************
*
* @purpose To get the character input range help string
*
* @param min  minimum value in range
* @param max  maximum value in range
*
* @returns L7_char8 (min to max)
*
* @end
*
*********************************************************************/
L7_char8 *usmWebCharacterRangeHelp(L7_uint32 min, L7_uint32 max)
{
  static L7_char8 buf[40];
  osapiSnprintf(buf, sizeof(buf), " (%u %s %u %s)", min, pStrInfo_common_To_5, max, pStr_Info_common_Characters);
  return buf;
}

/*********************************************************************
*
* @purpose To get the character input range help string (
*          valid for IP address and Hostname)
*
* @param min  minimum value in range
* @param max  maximum value in range
*
* @returns L7_char8 (min to max)
*
* @end
*
*********************************************************************/
L7_char8 *usmWebIPAddrHostnameRangeHelp(L7_uint32 min, L7_uint32 max)
{
  static L7_char8 buf[50];
  osapiSnprintf(buf, sizeof(buf), " (%s / %u %s %u %s)", pStr_Info_common_IPAddressFormat, min, pStrInfo_common_To_5, max, pStr_Info_common_AlphanumericCharacters);
  return buf;
}

/*********************************************************************
*
* @purpose To get the character input range help string
*
* @param min  minimum value in range
* @param max  maximum value in range
*
* @returns L7_char8 (min to max)
*
* @Notes    2: milli seconds
*           1 and default : secs
*
* @end
*
*********************************************************************/
L7_char8 *usmWebTimeIntervalRangeHelp(L7_uint32 min, L7_uint32 max, L7_uint32 count)
{
  static L7_char8 buf[40];
  switch(count)
  {
  case 2:
     osapiSnprintf(buf, sizeof(buf), " (%u %s %u %s)", min, pStrInfo_common_To_5, max, pStr_Info_common_MilliSecs);
     break;
  case 1:
  default:
     osapiSnprintf(buf, sizeof(buf), " (%u %s %u %s)", min, pStrInfo_common_To_5, max, pStr_Info_common_Secs_1);
     break;
  }
  return buf;
}

 
#ifdef L7_QOS_PACKAGE
/*********************************************************************
*
* @purpose Get all the acl id's in an options list drop down box
*
* @param context  EmWeb/Server request context handle
*
* @param optionp   Dynamic Select Support
*
* @param iterator pointer to the current iteration
*
* @returns option
*
* @end
*
*********************************************************************/
void *ewaFormSelect_ipv6_acl_list(EwsContext context,
                                  EwsFormSelectOptionP optionp,
    void * iterator)
{
   EwaNetHandle      net = ewsContextNetHandle(context);
   usmWeb_AppInfo_t  appInfo;
   static L7_uint32  aclNum, sel_acl;
   static L7_RC_t    rc;
  L7_char8 * bufChoice = ewsContextNetHandle(context)->buffer,
                     aclName[L7_ACL_NAME_LEN_MAX+1];
   L7_uint32         unit = usmDbThisUnitGet();

   if (iterator == NULL)
   {
      rc = usmDbQosAclNamedIndexGetFirst(unit, L7_ACL_TYPE_IPV6, &aclNum);
      sel_acl = aclNum;

      if (net->app_pointer != NULL)
      {
           memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
           if (appInfo.data[20] != 0)
           {
              sel_acl = appInfo.data[20];
           }
      }
    }
    else
    {
        rc = usmDbQosAclNamedIndexGetNext(unit, L7_ACL_TYPE_IPV6, aclNum, &aclNum);
    }

    if (rc == L7_SUCCESS)
    {
       if(usmDbQosAclNameGet(unit, aclNum, aclName) == L7_SUCCESS)
       {
          osapiStrncpySafe(bufChoice, aclName, APP_BUFFER_SIZE-1);
          optionp->choice = bufChoice;
          optionp->valuep = (void *) &aclNum;
          if (sel_acl == aclNum)
          {
             optionp->selected = TRUE;
          }
          else
          {
             optionp->selected = FALSE;
          }
       }
    return (void *) optionp;
    }
    return L7_NULL;
}

#endif
/*********************************************************************
*
* @purpose check to see if all characters inside a buffer(char8) are integers
*
* @param L7_char8 *buf
*
* @returns  L7_SUCCESS  means that all chars are integers
* @returns  L7_FAILURE  means that there exist some char that is not an integer.
*
* @notes This f(x) checks each letter inside the buf to make certain
         it is an integer
* @end
*
*********************************************************************/
L7_RC_t webCheckIfInteger(L7_char8 * buf)
{
  L7_uint32 i;
  L7_uint32 j = 0;

  if (buf != '\0')
   {
      if (buf[j] == '-')
      {
         j=1;
         /* If the string  starts with '-', then there should be at least one digit
            following '-' like -1, or, -20. */
      if (strlen(buf) == j)
      {                     /* No digit after '-' */
        return L7_FAILURE;
      }
      }

      for (i = j; i < strlen(buf) && buf != '\0'; i++)
      {
         if ((buf[i] < '0' || buf[i] > '9') && (buf != '\0'))
         {
            return L7_FAILURE;
         }
      }
   }
  return L7_SUCCESS;
}
/*
void usmWebGetArchive(void **data, void **code)
{
  *data = (void *)ew_archive_data;
  *code = (void *)ew_archive;
}
*/
/**************************************************************************
 * Function: usmWebPasswdMinLenGet
 *
 * Description:
 *     To get the min password length value
 *
 * Parameters:
 *
 * Return:
 *
 * Notes:
 *************************************************************************/
L7_ushort16 usmWebPasswdMinLenGet()
{
  L7_ushort16 val;

  usmDbMinPassLengthGet(&val);

  return val;
}

/*********************************************************************
* @purpose  Convert incoming hex code stream into a Hexidecimal NCR.
*
* @param    L7_char8 *in  @b{(input)}  String of virgin hex chars
* @param    L7_char8 *out @b{(output)} Converted unicode string
*
* @returns  L7_RC_t
*
* @notes    Input string must be UTF-16 (4-byte hex characters)!
*           We do a bit of input validation because this routine is
*           accessible via the CLI (hidden commands).
*
* @end
*********************************************************************/
L7_RC_t usmWebConvertHexToUnicode(L7_char8 *in, L7_char8 *out)
{
  L7_uint32 idx = 0;
  L7_uint32 jdx = 0;
  L7_uint32 len = 0;
  div_t res;

  if ((L7_NULLPTR == in) || (L7_NULLPTR == out))
  { 
    return L7_FAILURE;
  }

  /* Validate incoming hex string length */
  len = strlen(in);
  res = div(len,4);
  if (0 != res.rem)
  {
    return L7_FAILURE;
  }

  if (L7_SUCCESS != usmDbStringHexadecimalCheck(in))
  {
    return L7_FAILURE;
  }

  while (in[idx] != '\0')
  {
    out[jdx++] = '&';
    out[jdx++] = '#';
    out[jdx++] = 'x';
    out[jdx++] = in[idx++];
    out[jdx++] = in[idx++];
    out[jdx++] = in[idx++];
    out[jdx++] = in[idx++];
    out[jdx++] = ';';
  }
  out[jdx] = '\0';
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose To Return Default VLAN ID configured on the switch
*
* @param val  signed integer value
*
* @returns L7_uint32 to resulting
*
* @end
*
*********************************************************************/
L7_char8 *usmWebGetDefaultVLANID()
{
  L7_uint32 vlanId;
  static L7_char8 buf[USMWEB_BUFFER_SIZE_16]={0};

  vlanId = L7_DOT1Q_DEFAULT_VLAN;
  osapiSnprintf(buf, sizeof(buf), "%d", vlanId);
  return buf;
}
#else

L7_BOOL usmWebComponentPresent(L7_char8* compName)
{
 /* TODO - get all the usmdb component checks here */

  if (strcmp(compName, "PoE") == 0)
  {
#ifdef L7_POE_PACKAGE
          return L7_TRUE;
#else
          return L7_FALSE;
#endif
  }
  return L7_FALSE;
}

#endif
