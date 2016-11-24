/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cnfgr.c
*
* @purpose Hardware and Software System Controller Main File
*
* @component cnfgr
*
* @comments none
*
* @create 09/14/2000
*
* @author bmutz
*
* @end
*
**********************************************************************/

#include <string.h>                /* for memcpy() etc... */

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "cnfgr.h"
#include "log.h"
#include "nimapi.h"
#include "simapi.h"
#include "dot1s_api.h"
#include "layer2_api.h"
#include "dot1q_api.h"
#include "dtlapi.h"
#include "cfg_change_api.h"
#include "fdb_api.h"
#include "dot3ad_api.h"
#include "garpapi.h"
#include "snmp_api.h"
#include "snmp_util_api.h"
#include "cli_web_mgr_api.h"
#include "trapapi.h"
#include "std_policy_api.h"
#include "defaultconfig_pkg.h"
#include "l7_flex_packages.h"
#include "mirror_api.h"
#include "filter_api.h"
#include "pbvlan_api.h"
#include "mfdb_api.h"
#include "user_mgr_api.h"
#include "dot1x_api.h"
#include "sslt_api.h"
#include "dvlantag_api.h"
#include "sysapi_hpc_slotmapper.h"
#include "sysapi_hpc_chassis.h"
#include "sysapi.h"
#include "snooping_exports.h"
#include "dot3ad_exports.h"
#include "bxs_exports.h"
#include "dot3ah_exports.h"
#include "dtl_exports.h"
#include "mfdb_exports.h"
#include "nim_exports.h"
#include "sim_exports.h"
#include "log_exports.h"
#include "unitmgr_api.h"
#include "poe_api.h"

/* local function prototypes */
static L7_uint32 currPhase = 0;

static L7_BOOL managementState = L7_FALSE;

/* Debug to set the management unit and start phase 3 */
L7_RC_t cnfgrDebugMgmtUnitSet(L7_BOOL state)
{
  managementState = state;

  return L7_SUCCESS;
}

L7_BOOL isManagementUnit(void)
{
  return managementState;
}

/*********************************************************************
* @purpose  Check to see if IPSUBNET BASED VLAN is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsVlanIpSubnetPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubType;
  L7_BOOL retVal = L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();
  techSubType = cnfgrBaseTechnologySubtypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
      retVal = L7_TRUE;
      break;
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
      switch (techSubType)
      {
        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5673:
          retVal = L7_TRUE;
          break;
        default:
          retVal = L7_FALSE;
          break;
      }
      break;
     case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        retVal = L7_FALSE;
        break;

    default:
      retVal = L7_FALSE;
      break;
  }
  return retVal;
}

/*********************************************************************
* @purpose  Check to see if MAC BASED VLAN is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsVlanMacPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubType;
  L7_BOOL retVal = L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();
  techSubType = cnfgrBaseTechnologySubtypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:

      retVal = L7_TRUE;
      break;
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
      switch (techSubType)
      {
        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5673:
          retVal = L7_TRUE;
          break;
        default:
          retVal = L7_FALSE;
          break;
      }
      break;
     case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        switch (techSubType)
        {
         case  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115:   /* ROBO */
           retVal = L7_TRUE;
           break;
         default:
            retVal = L7_FALSE;
            break;
        }
        break;

    default:
      retVal = L7_FALSE;
      break;
  }
  return retVal;
}

/*********************************************************************
* @purpose  Check to see if PBVLAN is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL
cnfgrIsPbVlanPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
      /* BCM 5615/5645 does not support PBVLAN */
      return L7_FALSE;
      break;

    default:
      return L7_TRUE;
      break;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Check to see if CLI or WEB customization Feature is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsCliWebCustomizationFeaturePresent(L7_uint32 featureId)
{
  L7_BOOL result = L7_FALSE;
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  techType = cnfgrBaseTechnologyTypeGet();

  switch(featureId)
  {
    case L7_WEB_FEATURE_SUPPORTED:
      result = L7_TRUE;
      break;

    case L7_TXT_CFG_FEATURE_ID:
      result = L7_TRUE;
      break;

    case L7_HTTP_FILE_DOWNLOAD_FEATURE:
      result = L7_TRUE;
      break;

    case L7_TXT_RUN_CFG_FEATURE_ID:
      result = FD_TXT_RUN_CFG_SUPPORTED;
      break;

    default:
      break;
  }

  return result;
}

/*********************************************************************
* @purpose  Check to see if IGMP Snooping Feature is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL
cnfgrIsSnoopingFeaturePresent( L7_SNOOPING_FEATURE_IDS_t featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BOOL featureSupported = L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();
  switch (featureId)
  {
    case L7_IGMP_SNOOPING_FEATURE_PER_VLAN:
      featureSupported = L7_TRUE;
      break;

    case L7_IGMP_SNOOPING_FEATURE_SUPPORTED:
      featureSupported = L7_TRUE;
      break;

    case L7_MLD_SNOOPING_FEATURE_SUPPORTED:
       featureSupported = (L7_FEAT_MLD_SNOOPING && ((techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3) || 
                                                    (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)));
       break;
    case L7_MLD_SNOOPING_FEATURE_PER_VLAN:
       featureSupported = (L7_FEAT_MLD_SNOOPING && ((techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3) || 
                                                    (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)));
       break;
    case L7_SNOOPING_FEATURE_VLAN_PRECONFIG:
#if L7_FEAT_SNOOPING_VLAN_PRECONFIG
      if (L7_FEAT_SNOOPING_VLAN_PRECONFIG) 
      {
        featureSupported = L7_TRUE;
      }
      break;
#endif
    default:
      featureSupported = L7_FALSE;
      break;
  }

  return featureSupported;
}

/*********************************************************************
* @purpose  Check to see if Static MAC Filtering is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL
cnfgrIsFilteringPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  techType = cnfgrBaseTechnologyTypeGet();

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Check if Cable test is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsCableTestPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t techType;
  L7_BOOL                    compPresent = L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
      compPresent = L7_TRUE;
      break;
#ifdef L7_PRODUCT_SMB
#ifdef L7_METRO_FLEX_PACKAGE
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
     compPresent = L7_TRUE;
     break;
#endif /* L7_METRO_FLEX_PACKAGE */
#endif /* L7_PRODUCT_SMB */

    default:
      break;
  }

  return compPresent;
}

/*********************************************************************
* @purpose  Check to see if Double Vlan Tagging is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsDvlantagPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubType;
  L7_BOOL retVal = L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();
  techSubType = cnfgrBaseTechnologySubtypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
      /* Broadcom Strata does not support double vlan tagging */
      retVal = L7_FALSE;
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
      switch(techSubType)
      {
        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665:
        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5695:
        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5673:
          retVal = L7_TRUE;
          break;

        default:
          retVal = L7_FALSE;
          break;
      }
      break;
     case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
      switch(techSubType)
      {
        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115:
         retVal = L7_TRUE;
         break;
        default:
          retVal = L7_FALSE;
          break;
      }
      break;

    default:
      retVal = L7_TRUE;
      break;
  }

  return retVal;
}

/*********************************************************************
* @purpose  Check to see if Protected port is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsProtectedPortPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubType;
  L7_BOOL retVal = L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();
  techSubType = cnfgrBaseTechnologySubtypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
      retVal = L7_TRUE;
      break;
    default:
      retVal = L7_FALSE;
      break;
  }
  return retVal;
}

/*********************************************************************
* @purpose  Check to see if Port Mirroring is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL
cnfgrPortMirrorPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  techType = cnfgrBaseTechnologyTypeGet();

  switch(techType)
  {
    default:
      return L7_TRUE;
      break;
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Check to see if CLI/Web is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrCliWebPresent()
{
#ifdef L7_CLI_PACKAGE
  return L7_TRUE;
#else
  return L7_FALSE;
#endif
}

/*********************************************************************
* @purpose  Check to see if DoS Control is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsDosControlPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BOOL retVal = L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
      retVal = L7_TRUE;
      break;

    default:
      retVal = L7_FALSE;
      break;
  }

  return retVal;
}

/*********************************************************************
* @purpose  Check if specified SFLOW component ID is supported
*
* @param    none
*
* @return  L7_TRUE
* @return  L7_FALSE
*
* @note
*
*
* @end
*********************************************************************/
L7_BOOL cnfgrIssFlowPresent()
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BOOL retVal = L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
      retVal = L7_TRUE;
      break;

    default:
      retVal = L7_FALSE;
      break;
  }

  return retVal;
}

/*********************************************************************
* @purpose  Check to see if a component is present.
*
* @param    L7_uint32  componentId
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsComponentPresent(L7_COMPONENT_IDS_t  componentId)
{
  if (componentId < 1 || componentId >= L7_LAST_COMPONENT_ID)
    return L7_FALSE;

  switch (componentId)
  {
    case L7_SIM_COMPONENT_ID:
    case L7_NIM_COMPONENT_ID:
    case L7_POLICY_COMPONENT_ID:
#ifndef L7_PRODUCT_SMARTPATH
    case L7_TRAPMGR_COMPONENT_ID:
#endif
    case L7_DOT1P_COMPONENT_ID:
    case L7_DOT1S_COMPONENT_ID:
    case L7_DOT1X_COMPONENT_ID:
    case L7_DOT3AD_COMPONENT_ID:
#ifdef L7_GARP_PACKAGE
    case L7_GARP_COMPONENT_ID:
#endif
    case L7_CNFGR_COMPONENT_ID:
    case L7_DRIVER_COMPONENT_ID:
    case L7_DTL_COMPONENT_ID:
    case L7_DOT1Q_COMPONENT_ID:
    case L7_STATSMGR_COMPONENT_ID:
    case L7_FDB_COMPONENT_ID:
    case L7_CMD_LOGGER_COMPONENT_ID:
#ifdef L7_ROUTING_PACKAGE
    case L7_RTR_DISC_COMPONENT_ID:
#endif
#ifdef L7_OSPF_PACKAGE
    case L7_OSPF_MAP_COMPONENT_ID:
#endif
#ifdef L7_RIP_PACKAGE
    case L7_RIP_MAP_COMPONENT_ID:
#endif
#ifdef L7_VRRP_PACKAGE
    case L7_VRRP_MAP_COMPONENT_ID:
#endif
#ifdef L7_RLIM_PACKAGE
    case L7_RLIM_COMPONENT_ID:
#endif
    case L7_MFDB_COMPONENT_ID:
#ifdef L7_ROUTING_PACKAGE
    case L7_GMRP_COMPONENT_ID:
#endif
    case L7_USER_MGR_COMPONENT_ID:
#if 0
      /* intentionally remove. code left for historical purposes.*/
    case L7_IPV6_PROVISIONING_COMPONENT_ID:
#endif
    case L7_LOG_COMPONENT_ID:
    case L7_SNTP_COMPONENT_ID:
#ifdef L7_DAI_PACKAGE
    case L7_DAI_COMPONENT_ID:
#endif
      return L7_TRUE;
      break;

    case L7_FLEX_QOS_VOIP_COMPONENT_ID:
      return L7_TRUE;
      break;
    case L7_CLI_WEB_COMPONENT_ID:
      return cnfgrCliWebPresent();
      break;

    case L7_PORT_MIRROR_COMPONENT_ID:
      return cnfgrPortMirrorPresent();
      break;
#ifdef L7_PBVLAN_PACKAGE
    case L7_PBVLAN_COMPONENT_ID:
      return cnfgrIsPbVlanPresent();
      break;
#endif

    case L7_SNOOPING_COMPONENT_ID:
      return L7_TRUE;
      break;

    case L7_FILTER_COMPONENT_ID:
      return cnfgrIsFilteringPresent();
      break;

    case L7_CABLE_TEST_COMPONENT_ID:
      return cnfgrIsCableTestPresent();
      break;

#ifdef L7_ROUTING_PACKAGE
    case L7_IP_MAP_COMPONENT_ID:
      return L7_TRUE;
      break;
#endif
    case L7_SNMP_COMPONENT_ID:
      return snmpPresent();
      break;

    case L7_LAYER3_COMPONENT_ID:
#ifdef L7_ROUTING_PACKAGE
      return L7_TRUE;
#endif
      return L7_FALSE;
      break;

    case L7_FLEX_PACKAGES_ID:
    case L7_FLEX_PIMDM_MAP_COMPONENT_ID:
    case L7_FLEX_MPLS_MAP_COMPONENT_ID:
    case L7_FLEX_QOS_ACL_COMPONENT_ID:
    case L7_FLEX_QOS_COS_COMPONENT_ID:
    case L7_FLEX_QOS_DIFFSERV_COMPONENT_ID:
    case L7_FLEX_QOS_ISCSI_COMPONENT_ID:
    case L7_FLEX_BGP_MAP_COMPONENT_ID:
    case L7_FLEX_MGMD_MAP_COMPONENT_ID:
    case L7_FLEX_MCAST_MAP_COMPONENT_ID:
    case L7_FLEX_DVMRP_MAP_COMPONENT_ID:
    case L7_FLEX_PIMSM_MAP_COMPONENT_ID:
    case L7_FLEX_STACKING_COMPONENT_ID:
    case L7_FLEX_SSHD_COMPONENT_ID:
    case L7_FLEX_SSLT_COMPONENT_ID:
    case L7_FLEX_IPV6_MAP_COMPONENT_ID:
    case L7_FLEX_OSPFV3_MAP_COMPONENT_ID:
    case L7_FLEX_DHCP6S_MAP_COMPONENT_ID:
    case L7_FLEX_WIRELESS_COMPONENT_ID:
    case L7_FLEX_IPV6_MGMT_COMPONENT_ID:
    case L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID:
    case L7_FLEX_METRO_DOT1AD_COMPONENT_ID:
      return cnfgrFlexIsComponentPresent(componentId);
      break;

    case L7_SERVICE_PORT_PRESENT_ID:
      return SERVICE_PORT_PRESENT;
      break;

    case L7_PORT_MACLOCK_COMPONENT_ID:
      return L7_TRUE;
      break;

#ifdef L7_DVLAN_PACKAGE
    case L7_DVLANTAG_COMPONENT_ID:
      return cnfgrIsDvlantagPresent();
      break;
#endif

    case L7_PROTECTED_PORT_COMPONENT_ID:
      return cnfgrIsProtectedPortPresent();
      break;

    case L7_DOSCONTROL_COMPONENT_ID:
      return cnfgrIsDosControlPresent();
      break;

#ifdef L7_IPVLAN_PACKAGE
    case L7_VLAN_IPSUBNET_COMPONENT_ID:
      return cnfgrIsVlanIpSubnetPresent();
      break;
#endif

#ifdef L7_MACVLAN_PACKAGE
    case L7_VLAN_MAC_COMPONENT_ID:
      return cnfgrIsVlanMacPresent();
      break;
#endif
#ifdef L7_DHCPS_PACKAGE
    case L7_DHCPS_MAP_COMPONENT_ID:
      return L7_TRUE;
      break;
#endif
#ifdef L7_DHCP_FILTER_PACKAGE
    case  L7_DHCP_FILTERING_COMPONENT_ID:
      return L7_TRUE;
      break;
#endif
#ifdef L7_DHCP_SNOOPING_PACKAGE
    case L7_DHCP_SNOOPING_COMPONENT_ID:
      return L7_TRUE;
      break;
#endif
    case L7_DHCP_RELAY_COMPONENT_ID:
      return L7_TRUE;
      break;
#ifdef L7_CP_WIO_PACKAGE
  case L7_CP_WIO_COMPONENT_ID:
    return L7_TRUE;
    break; 
#endif

#ifdef L7_POE_PACKAGE
    case L7_POE_COMPONENT_ID:
      return L7_TRUE;
      break;
#endif
#ifdef L7_SFLOW_PACKAGE
    case L7_SFLOW_COMPONENT_ID:
      return cnfgrIssFlowPresent();
      break;
#endif
    case L7_BOX_SERVICES_COMPONENT_ID:
      return L7_TRUE;
      break;
    case L7_LINK_DEPENDENCY_COMPONENT_ID:
      return L7_FEAT_LINK_DEPENDENCY;
      break;


#ifdef L7_AUTO_INSTALL_PACKAGE
  case L7_AUTO_INSTALL_COMPONENT_ID:    
    return L7_TRUE;  
    break;
#endif /* L7_AUTO_INSTALL_PACKAGE */

#ifdef L7_ISDP_PACKAGE
    case L7_ISDP_COMPONENT_ID:
      return L7_TRUE;
      break;
#endif /* L7_ISDP_PACKAGE */

case L7_VOICE_VLAN_COMPONENT_ID:
      return L7_TRUE;
     break;

#ifdef L7_DOT3AH_PACKAGE
  case L7_DOT3AH_COMPONENT_ID:
    return L7_TRUE;
    break;
#endif /* L7_DOT3AH_PACKAGE */

#ifdef L7_DOT1AG_PACKAGE
  case L7_DOT1AG_COMPONENT_ID:
    return L7_TRUE;
    break;
#endif /* L7_DOT1AG_PACKAGE */

#ifdef L7_TR069_PACKAGE
  case L7_TR069_COMPONENT_ID:    
    return L7_TRUE;
    break;
#endif /* L7_TR069_PACKAGE */
    
  default:
      return L7_FALSE;
      break;
  }
}

/*********************************************************************
* @purpose  Check to see if a feature is present for SNMP component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon code available in build.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrSNMPIsFeaturePresent(L7_uint32 featureId)
{
  if (featureId >= L7_SNMP_USER_FEATURE_ID_TOTAL)
    return L7_FALSE;

  switch (featureId)
  {
    case L7_SNMP_USER_SUPPORTED:
      if (L7_FEAT_SNMP_USER_MAPPING) {
        return L7_TRUE;
      }
      break;

    case L7_SNMP_USER_AUTHENTICATION_FEATURE_ID:
    case L7_SNMP_USER_ENCRYPTION_FEATURE_ID:
      return SnmpUserTableHasEncryption();
      break;

    default:
      return L7_FALSE;
      break;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for Port Mirroring
*           component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the capabilities of the
*           hardware platform.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrPortMirrorIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  if (featureId >= L7_MIRRORING_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (featureId)
  {
    case L7_MIRRORING_MULT_SRCPORTS_FEATURE_ID:
    case L7_MIRRORING_CPU_INTF_SUPPORTED_FEATURE_ID:
      if (techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER)
      {
        return L7_FALSE;
      }
      else
      {
        return L7_TRUE;
      }
      break;
    case L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID:
      if( (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3) || (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO) || 
          (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4))
      {
        return L7_TRUE;
        break;
      }
      else if (techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER)
      {
        return L7_TRUE;
        break;
      }
      else
      {
        return L7_FALSE;
        break;
      }
    case L7_MIRRORING_LAG_INTF_SOURCE_SUPPORTED_FEATURE_ID:
	  if(( techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3 ) || 
         ( techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4 )) {
		  return L7_TRUE;
      }
    break;
    default:
      return L7_FALSE;
      break;
  }
  return L7_FALSE;
}


/*********************************************************************
* @purpose  Check if DOT1S or DOT1D is supported.
*
* @param    L7_uint32   featureId
*
* @return  L7_TRUE if DOT1S is supported
* @return  L7_FALSE if DOT1D is supported
*
* @note    Feature support is dependent upon value of the
*          L7_BRIDGE_SPEC_SUPPORTED constant in platform.h
*
* @note    Assumes caller has verified this component is present in
*           the system.
*          dot1d code has been removed so this check should always return success.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrDot1sIsFeaturePresent(L7_uint32 featureId)
{
  L7_BOOL featurePresent = L7_TRUE;

   switch (featureId)
   {
     case L7_DOT1S_FEATURE_ROOTGUARD:
       featurePresent = L7_FEAT_DOT1S_ROOTGUARD;
       break;
     case L7_DOT1S_FEATURE_LOOPGUARD:
       featurePresent = L7_FEAT_DOT1S_LOOPGUARD;
       break;
     case L7_DOT1S_FEATURE_BPPDUFLOOD:
       featurePresent = L7_FEAT_DOT1S_BPPDUFLOOD;
       break;
     case L7_DOT1S_FEATURE_BPDUGUARD:
       featurePresent = L7_FEAT_DOT1S_BPDUGUARD;
       break;
     case L7_DOT1S_FEATURE_BPPDUFILTER:
       featurePresent = L7_FEAT_DOT1S_BPPDUFILTER;
       break;
     default:
       featurePresent = L7_TRUE;
       break;

   }

   return featurePresent;
}

/*********************************************************************
* @purpose  Check if multiple protected port group is supported.
*
* @param    L7_uint32   featureId
*
* @return  L7_TRUE
* @return  L7_FALSE
*
* @note    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrIsProtectedPortFeaturePresent(L7_uint32 featureId)
{
  L7_uint32 minProtectedGroups = 1;
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubType;

  techType = cnfgrBaseTechnologyTypeGet();
  techSubType = cnfgrBaseTechnologySubtypeGet();

  if (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO &&
      techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115)
  {
    minProtectedGroups = 0; /* 53115 supports only one 
                               Protected group */
  }
  if (L7_PROTECTED_PORT_MAX_GROUPS > minProtectedGroups)
  {
    return L7_TRUE;
  }
  else
    return L7_FALSE;
}

#ifdef L7_POE_PACKAGE
/*********************************************************************
* @purpose  Check if specified POE feature ID is supported
*
* @param    L7_uint32   featureId
*
* @return  L7_TRUE
* @return  L7_FALSE
*
* @note    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrIsPoeFeaturePresent(L7_uint32 featureId)
{
  L7_BOOL   rc = L7_FALSE;
  L7_uint32 unit;

  switch (featureId)
  {
    case L7_POE_FEATURE_ID:
      unitMgrStackMemberGetFirst(&unit);
      do
      {
        if (L7_SUCCESS == poeIsValidUnit(unit))
        {
          rc = L7_TRUE;
          break;
        }
      } while (L7_SUCCESS == unitMgrStackMemberGetNext(unit, &unit));
      break;
    case L7_POE_FEATURE_SYSTEM_AUTO_RESET_MODE:
    case L7_POE_FEATURE_SYSTEM_NOTIFICATION_CONTROL:
    case L7_POE_FEATURE_SYSTEM_LOGICAL_PORT_MAP_MODE:
    case L7_POE_FEATURE_PORT_DETECTION_MODE:
    case L7_POE_FEATURE_PORT_HIGH_POWER_MODE:
    case L7_POE_FEATURE_PORT_VIOLATION_TYPE:
    case L7_POE_FEATURE_PORT_DISCONNECT_TYPE:
      rc = L7_FEAT_POE_BRCM;
      break;
    default:
      break;
  }
  return rc;
}
#endif

/*********************************************************************
* @purpose  Check if specified Box Services feature ID is supported
*
* @param    L7_uint32   featureId
*
* @return  L7_TRUE
* @return  L7_FALSE
*
* @note    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrIsBoxServicesFeaturePresent(L7_uint32 featureId)
{
  L7_BOOL  rc = L7_TRUE;

  switch (featureId)
  {
  case L7_BXS_FAN_STATUS_FEATURE_ID:
    rc = L7_FEAT_FAN_STATUS;
    break;

  case L7_BXS_PWR_SUPPLY_STATUS_FEATURE_ID:
    rc = L7_FEAT_PWR_SUPPLY_STATUS;
    break;

  case L7_BXS_TEMPERATURE_FEATURE_ID:
    rc = L7_FEAT_TEMPERATURE;
    break;

  case L7_BXS_TEMPERATURE_STATUS_FEATURE_ID:
    rc = L7_FEAT_TEMPERATURE_STATUS;
    break;

  case L7_BXS_SFP_STATUS_FEATURE_ID:
    rc = L7_FEAT_SFP;
    break;

  case L7_BXS_XFP_STATUS_FEATURE_ID:
    rc = L7_FEAT_PLUGIN_XFP_MODULE;
    break;

  default:
    rc = L7_FALSE;
    break;
  }
  return rc;
}


/*********************************************************************
* @purpose  Check to see if a feature is present for Dot3ah component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the capabilities of the
*           hardware platform.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrIsDot3ahFeaturePresent(L7_uint32 featureId)
{
  if (featureId >= L7_DOT3AH_FEATURE_ID_TOTAL)
  {
   return L7_FALSE;
  }

  switch (featureId)
  {
    default:
      return L7_TRUE;
      break;
  }

  return L7_FALSE;
}



/*********************************************************************
* @purpose  Check to see if a feature is present for the Dot1x component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrDot1xIsFeaturePresent(L7_uint32 featureId)
{
  L7_BOOL featurePresent = L7_FALSE;

  if (featureId >= L7_DOT1X_FEATURE_ID_TOTAL)
    return featurePresent;

  switch (featureId)
  {
    case L7_DOT1X_SET_KEYTXENABLED_FEATURE_ID:
      break;
    case L7_DOT1X_SET_CONTROLDIR_FEATURE_ID:
      break;
    case L7_DOT1X_VLANASSIGN_FEATURE_ID:
      featurePresent = L7_FEAT_DOT1X_VLANASSIGN;
      break;
    case L7_DOT1X_MACBASEDAUTH_FEATURE_ID:
      featurePresent = L7_FEAT_DOT1X_MACBASEDAUTH;
      break;
    case L7_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS_FEATURE_ID:
       featurePresent = L7_FEAT_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS;
    break;
    default:
      /* Undefined feature id */
      break;
  }

  return featurePresent;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for IpMap component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon base technology type.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrIpMapIsFeaturePresent(L7_uint32 featureId)
{
  L7_BOOL featurePresent = L7_TRUE;

  if (featureId >= L7_IPMAP_FEATURE_ID_TOTAL)
    return L7_FALSE;

  switch (featureId)
  {
    case L7_IPMAP_FEATURE_LAG_RTING_SUPPORT_FEATURE_ID:
       featurePresent = L7_FALSE;
       break;
    case L7_IPMAP_FEATURE_PROXY_ARP:
      featurePresent = L7_FEAT_IPMAP_PROXY_ARP;
      break;
    case L7_IPMAP_FEATURE_MULTINETTING:
        featurePresent = L7_FEAT_IPMAP_MULTINETTING;
        break;
    case L7_IPMAP_FEATURE_REJECTROUTE:
        featurePresent = L7_FEAT_IPMAP_REJECTROUTE;
        break;
    default:
      featurePresent = L7_TRUE;
      break;
  }
  return featurePresent;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for Static Filtering
*           component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the capabilities of the
*           hardware platform.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrFilteringIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  if (featureId >= L7_FILTER_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (featureId)
  {
    case L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID:
      if ((techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA) ||
          (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS))
      {
        /* Macfiltering is only supported on Broadcom XGA II and if QOS is not present */
        if (cnfgrIsComponentPresent(L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_FALSE &&
            cnfgrIsComponentPresent(L7_FLEX_QOS_COS_COMPONENT_ID) == L7_FALSE &&
            cnfgrIsComponentPresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID) == L7_FALSE)
          return L7_TRUE;
        else
          return L7_FALSE;
      }
      else
        return L7_TRUE;
      break;
    case L7_FILTER_UCAST_DEST_PORT_FILTERING_FEATURE_ID:
      if ((techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA) ||
          (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS)    ||
          (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3)   ||
          (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4))
        return L7_FALSE;
      else
        return L7_TRUE;
      break;

    default:
      return L7_FALSE;
      break;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for Policy
*           component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the capabilities of the
*           hardware platform.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrPolicyIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  if (featureId >= L7_POLICY_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (featureId)
  {
    case L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID:
      return L7_FALSE;
      break;
    case L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID:
      return L7_TRUE;
      break;
    case L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID:
      return L7_TRUE;
      break;
    case L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID:
      return L7_TRUE;
      break;
    case L7_POLICY_BCAST_CONTROL_FEATURE_ID:
      return L7_TRUE;
      break;
    case L7_POLICY_MCAST_CONTROL_FEATURE_ID:
      return L7_TRUE;
      break;
    case L7_POLICY_UCAST_CONTROL_FEATURE_ID:
      return L7_TRUE;
      break;
    default:
      return L7_FALSE;
      break;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for DoS Control
*           component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the capabilities of the
*           hardware platform.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrDosControlIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubType;
 
  if (featureId >= L7_DOSCONTROL_FEATURE_ID_TOTAL)
    return L7_FALSE;
 
  techType = cnfgrBaseTechnologyTypeGet();
  techSubType = cnfgrBaseTechnologySubtypeGet();
 
  if (techType != L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3 &&
      techType != L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO &&
      techType != L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER &&
      techType != L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
  {
     return L7_FALSE;
  }
 
  switch (featureId)
  {
    case L7_DOSCONTROL_SIPDIP_FEATURE_ID:
      return L7_TRUE;
      break;

    case L7_DOSCONTROL_SMACDMAC_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115) 
      {
        return L7_FALSE;
      }
      else
      {
      return L7_TRUE;
      }
      break;

    case L7_DOSCONTROL_TCPFRAG_FEATURE_ID:
      return L7_TRUE;
      break;

    case L7_DOSCONTROL_TCPFLAG_FEATURE_ID:
      return L7_TRUE;
      break;

    case L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115 ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case L7_DOSCONTROL_L4PORT_FEATURE_ID:
      return L7_TRUE;
      break;

    case L7_DOSCONTROL_TCPPORT_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115 ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case L7_DOSCONTROL_UDPPORT_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115 ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case L7_DOSCONTROL_ICMP_FEATURE_ID:
      return L7_TRUE;
      break;

    case  L7_DOSCONTROL_ICMPV4_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case  L7_DOSCONTROL_ICMPV6_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case  L7_DOSCONTROL_ICMPFRAG_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115 ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case  L7_DOSCONTROL_TCPSYN_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115 ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case  L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115 ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case  L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115 ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    case  L7_DOSCONTROL_TCPOFFSET_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x ||
          techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115 ||
          techType == L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER ||
          techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4)
        return L7_TRUE;
      else
        return L7_FALSE;
      break;

    default:
      return L7_FALSE;
      break;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for TRAPMGR component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon base technology type.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrTrapMgrIsFeaturePresent(L7_uint32 featureId)
{
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for DTL component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon base technology type.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrDtlIsFeaturePresent(L7_uint32 featureId)
{
  if (featureId == L7_DTL_CUSTOM_MMU_FEATURE_ID)
  {
    return L7_FEAT_CUSTOM_MMU;
  }

  if (featureId == L7_DTL_CUSTOM_LED_BLINK_FEATURE_ID)
  {
    return L7_FEAT_CUSTOM_LED_BLINK;
  }

  if (featureId == L7_DTL_STRICT_POLICY_LIMIT_FEATURE_ID)
  {
    return L7_FEAT_STRICT_POLICY_LIMIT;
  }


  return L7_TRUE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for FDB component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon base technology type.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrFdbIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  if (featureId >= L7_FDB_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (featureId)
  {
    case L7_FDB_FEATURE_ID:
      return L7_TRUE;
      break;

    case L7_FDB_AGETIME_PER_VLAN_FEATURE_ID:
      switch (techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA: /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:  /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3: /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4: /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO: /* fallthrough */
          return L7_FALSE;
          break;

        case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER: /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_UNKNOWN:       /* fallthrough */
        default:
          return L7_FALSE;
          break;
      }
      break;

    default:
      return L7_FALSE;
      break;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for VOIP component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon base technology type.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrQosVoipFeaturePresent(L7_uint32 featureId)
{
  if (featureId >=L7_QOS_VOIP_FEATURE_ID_TOTAL)
    return L7_FALSE;

  switch(featureId)
  {
    case L7_QOS_VOIP_PROTOCOL_SIP:
      return L7_TRUE;
      break;
    case L7_QOS_VOIP_PROTOCOL_MGCP:
      return L7_FALSE;
      break;
    case L7_QOS_VOIP_PROTOCOL_SCCP:
      return L7_TRUE;
      break;
    case L7_QOS_VOIP_PROTOCOL_H323:
      return L7_TRUE;
      break;
    default:
      return L7_FALSE;
      break;
  }
}

/*********************************************************************
* @purpose  Check to see if a feature StaticFiltering is present for MFDB component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon base technology type.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrMfdbStaticFilteringFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  if (featureId >= L7_MFDB_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (featureId)
  {
    case L7_MFDB_STATICFILTERING_FEATURE_ID:
      if ( (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA) ||
           (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS)    ||
           (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3)   ||
           (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4))
      {
        if (cnfgrIsComponentPresent(L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_FALSE &&
            cnfgrIsComponentPresent(L7_FLEX_QOS_COS_COMPONENT_ID) == L7_FALSE &&
            cnfgrIsComponentPresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID) == L7_FALSE)
        {
          return L7_TRUE;
          break;
        }
        else
        {
          return L7_FALSE;
          break;
        }
      }
      else
      {
        return L7_TRUE;
        break;
      }

    default:
      return L7_TRUE;
      break;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for DOT1P component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the capabilities of the
*           hardware platform.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrDot1pIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  if (featureId >= L7_DOT1P_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (featureId)
  {
    case L7_DOT1P_FEATURE_SUPPORTED:
      return L7_TRUE;
      break;

    case L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID:

      switch (techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER: /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA: /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO: /* fallthrough */
          return L7_TRUE;
          break;

        default:
          return L7_FALSE;
          break;
      }
      break;

    case L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID:
      return L7_TRUE;
      break;

    case L7_DOT1P_CONFIGURE_USER_REGENT_FEATURE_ID:
      return L7_FALSE;
      break;

    default:
      return L7_FALSE;
      break;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for Double Vlan tagging
*           component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the capabilities of the
*           hardware platform.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrDvlantagIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubType;
  L7_BOOL retVal = L7_FALSE;

  if (featureId >= L7_DVLANTAG_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();
  techSubType = cnfgrBaseTechnologySubtypeGet();

  switch (featureId)
  {
    case L7_DVLANTAG_FEATURE_SUPPORTED:

    switch (techType)
    {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
      retVal = L7_FALSE;
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
      switch (techSubType)
      {
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665:
        if (cnfgrIsFeaturePresent (L7_FLEX_STACKING_COMPONENT_ID,
                                   L7_FPS_FEATURE_ID) == L7_TRUE)
        {
          /* If Front panel stack is enabled on the 5665 then DVLAN tagging
          ** is not supported.
          */
          retVal = L7_FALSE;
        }
        else
        {
          retVal = L7_TRUE;
        }

        break;
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5695:
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5673:
        retVal = L7_TRUE;
        break;

      default:
        retVal = L7_TRUE;
        break;
      }
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
      switch (techSubType)
      {
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115:
        retVal = L7_TRUE;
        break;
      default:
        retVal = L7_FALSE;
        break;
      }
      break;

    default:
      retVal = L7_TRUE;
      break;
    }
    break;

    case L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID:

      switch (techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
          retVal = L7_FALSE;
          break;
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        {
          switch (techSubType)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x:
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520:
              retVal = L7_TRUE;
              break;
            default:
              retVal = L7_FALSE;
              break;
          }
        }
        break;
        default:
          retVal = L7_TRUE;
          break;
      }

      break;

    case L7_DVLANTAG_ETHERTYPE_FEATURE_ID:

      switch (techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
          switch(techSubType)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665:
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5695:
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5673:
              retVal = L7_TRUE;
              break;

            default:
              retVal = L7_FALSE;
              break;
          }
          break;
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
          retVal = L7_TRUE;
          break;

        default:
          retVal = L7_FALSE;
          break;
      }

      break;

    case L7_DVLANTAG_CUSTID_FEATURE_ID:

      switch (techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
          retVal = L7_FALSE;
          break;

        default:
          retVal = L7_TRUE;
          break;
      }

      break;

  case L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID:

    switch(techType)
    {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
          retVal = L7_TRUE;
          break;

        default:
          retVal = L7_FALSE;
          break;
    }
    break;
    
    default:
      retVal = L7_FALSE;
      break;
  }

  return retVal;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for NIM component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon base technology type.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrNimIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  if (featureId >= L7_NIM_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  if (featureId == L7_NIM_JUMBOFRAMES_FEATURE_ID )
  {
    switch (techType)
    {

      case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER: /* fallthrough */
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:  /* fallthrough */
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3: /* fallthrough */
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4: /* fallthrough */
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        return L7_TRUE;
        break;

      default:
        return L7_FALSE;
        break;
    }
  }
  else if (featureId == L7_NIM_JUMBOFRAMES_PER_INTERFACE_FEATURE_ID)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for SIM component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the presence of any feature keys
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrSimIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;

  if (featureId >= L7_SIM_FEATURE_ID_TOTAL)
    return L7_FALSE;

  if (featureId == L7_SIM_FEATURE_KEYING_FEATURE_ID )
  {
    if(L7_SYSAPI_FEATURE_KEYING_SUPPORTED == L7_TRUE)
    {
      return sysapiFeatureKeyPresent();
    }

    return L7_SYSAPI_FEATURE_KEYING_SUPPORTED;
  }
  else if(featureId == L7_SIM_DRIVER_SHELL_FEATURE_ID)
  {
    techType = cnfgrBaseTechnologyTypeGet();

    switch (techType)
    {
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        return L7_TRUE;
        break;

      default:
        return L7_FALSE;
        break;
    }
  }
  else if (featureId == L7_SIM_DNI8541_BLADESERVER)
  {
    return L7_FEAT_DNI8541_BLADESERVER;
  }
  else if(featureId == L7_SIM_ALLOW_LOCAL_MAC_USAGE_FEATURE_ID)
  {
#if defined(FEAT_METRO_CPE_V1_0)
        return L7_FALSE;
#else
        return L7_TRUE;
#endif
  }     

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for LOGGING component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the presence of definition
*           in platform.h
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrLoggingIsFeaturePresent(L7_uint32 featureId)
{
  if (featureId >= L7_LOG_PERSISTENT_FEATURE_ID)
  {
    return L7_PERSISTENT_LOG_SUPPORTED;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for SNTP component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the presence of definition
*           in platform.h
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrSNTPIsFeaturePresent(L7_uint32 featureId)
{
  switch (featureId)
  {
    case L7_SNTP_UNICAST_FEATURE_ID:
    case L7_SNTP_BROADCAST_FEATURE_ID:
    case L7_SNTP_PROTO_IPV4_FEATURE_ID:
    case L7_SNTP_PROTO_DNS_FEATURE_ID:
      return L7_TRUE;
      break;

    case L7_SNTP_MULTICAST_FEATURE_ID:
    case L7_SNTP_PROTO_IPV6_FEATURE_ID:
      return L7_FALSE;
      break;

    default:
      return L7_FALSE;
      break;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present for Dot3ad component
*
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Feature support is dependent upon the capabilities of the
*           hardware platform.
*
* @notes    Assumes caller has verified this component is present in
*           the system.
*
* @end
*********************************************************************/
static L7_BOOL cnfgrDot3adIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t techSubType;

  if (featureId <= L7_DOT3AD_FEATURE_ID || featureId >= L7_DOT3AD_FEATURE_ID_TOTAL)
  {
    return L7_FALSE;
  }
  techSubType = cnfgrBaseTechnologySubtypeGet();
  switch (featureId)
  {
    case L7_DOT3AD_HASHMODE_SA_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_DA_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_SADA_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_GLOBAL_FEATURE_ID:
      if (techSubType == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115)
      {
        return L7_TRUE;
      }
      else
      {
        return L7_FALSE;
      }
    case L7_DOT3AD_HASHMODE_SA_VLAN_ETYPE_INTF_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_DA_VLAN_ETYPE_INTF_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_SADA_VLAN_ETYPE_INTF_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_SIP_SPORT_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_DIP_DPORT_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_SIP_DIP_SPORT_DPORT_FEATURE_ID:
    case L7_DOT3AD_HASHMODE_INTF_FEATURE_ID:
      techType = cnfgrBaseTechnologyTypeGet();
      switch (techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER: /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:  /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3: /* fallthrough */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4: /* fallthrough */
          return L7_TRUE;
        default:
          return L7_FALSE;
          break;
      }
    case L7_DOT3AD_LAG_PRECREATE_FEATURE_ID:
      return L7_FEAT_LAG_PRECREATE;
      break;
    default:
      return L7_FALSE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check to see if a feature is present.  A feature is
*           a specific part of a component.
*
* @param    L7_uint32  componentId
* @param    L7_uint32  featureId
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsFeaturePresent(L7_COMPONENT_IDS_t  componentId, L7_uint32 featureId)
{
  L7_BOOL retVal = L7_FALSE;

  if (componentId < 1 || componentId >= L7_LAST_COMPONENT_ID)
    return L7_FALSE;

  if (cnfgrIsComponentPresent(componentId) != L7_TRUE)
    return L7_FALSE;

  switch (componentId)
  {
    case L7_SIM_COMPONENT_ID:
      retVal = cnfgrSimIsFeaturePresent(featureId);
      break;

    case L7_TRAPMGR_COMPONENT_ID:
      retVal = cnfgrTrapMgrIsFeaturePresent(featureId);
      break;

    case L7_DTL_COMPONENT_ID:
      retVal = cnfgrDtlIsFeaturePresent(featureId);
      break;

    case L7_GARP_COMPONENT_ID:
    case L7_CNFGR_COMPONENT_ID:
    case L7_DRIVER_COMPONENT_ID:
    case L7_PBVLAN_COMPONENT_ID:
    case L7_STATSMGR_COMPONENT_ID:
    case L7_CMD_LOGGER_COMPONENT_ID:
    case L7_RTR_DISC_COMPONENT_ID:
    case L7_OSPF_MAP_COMPONENT_ID:
    case L7_FLEX_IPV6_MAP_COMPONENT_ID:
#ifdef L7_OSPFV3
    case L7_FLEX_OSPFV3_MAP_COMPONENT_ID:
#endif
    case L7_RIP_MAP_COMPONENT_ID:
    case L7_VRRP_MAP_COMPONENT_ID:
    case L7_LAYER3_COMPONENT_ID:
    case L7_IPV6_PROVISIONING_COMPONENT_ID:
    case L7_VLAN_IPSUBNET_COMPONENT_ID:
    case L7_VLAN_MAC_COMPONENT_ID:
#ifdef L7_IPV6_MGMT_PACKAGE
    case L7_FLEX_IPV6_MGMT_COMPONENT_ID:
#endif
    case L7_VOICE_VLAN_COMPONENT_ID:
      retVal = L7_TRUE;
      break;
    case L7_CLI_WEB_COMPONENT_ID:
      retVal = cnfgrIsCliWebCustomizationFeaturePresent(featureId);
      break;
    case L7_SNOOPING_COMPONENT_ID:
      retVal = cnfgrIsSnoopingFeaturePresent(featureId);
      break;

    case L7_DOT1Q_COMPONENT_ID:
      retVal = L7_FALSE;
      break;
    case L7_DOT3AD_COMPONENT_ID:
      retVal = cnfgrDot3adIsFeaturePresent(featureId);
      break;

    case L7_FDB_COMPONENT_ID:
      retVal = cnfgrFdbIsFeaturePresent(featureId);
      break;

    case L7_DOT1S_COMPONENT_ID:
      retVal = cnfgrDot1sIsFeaturePresent(featureId);
      break;

    case L7_DOT1X_COMPONENT_ID:
      retVal = cnfgrDot1xIsFeaturePresent(featureId);
      break;

    case L7_IP_MAP_COMPONENT_ID:
      retVal = cnfgrIpMapIsFeaturePresent(featureId);
      break;

    case L7_SNMP_COMPONENT_ID:
      retVal = cnfgrSNMPIsFeaturePresent(featureId);
      break;

    case L7_PORT_MIRROR_COMPONENT_ID:
      retVal = cnfgrPortMirrorIsFeaturePresent(featureId);
      break;

    case L7_FILTER_COMPONENT_ID:
      retVal = cnfgrFilteringIsFeaturePresent(featureId);
      break;

    case L7_POLICY_COMPONENT_ID:
      retVal = cnfgrPolicyIsFeaturePresent(featureId);
      break;

    case L7_DOSCONTROL_COMPONENT_ID:
      retVal = cnfgrDosControlIsFeaturePresent(featureId);
      break;

    case L7_MFDB_COMPONENT_ID:
      retVal = cnfgrMfdbStaticFilteringFeaturePresent(featureId);
      break;

    case L7_FLEX_QOS_VOIP_COMPONENT_ID:
      retVal = cnfgrQosVoipFeaturePresent(featureId);
      break;
    case L7_FLEX_PACKAGES_ID:
    case L7_FLEX_MPLS_MAP_COMPONENT_ID:
    case L7_FLEX_QOS_ACL_COMPONENT_ID:
    case L7_FLEX_QOS_COS_COMPONENT_ID:
    case L7_FLEX_QOS_DIFFSERV_COMPONENT_ID:
    case L7_FLEX_QOS_ISCSI_COMPONENT_ID:
    case L7_FLEX_MGMD_MAP_COMPONENT_ID:
    case L7_FLEX_MCAST_MAP_COMPONENT_ID:
    case L7_FLEX_PIMDM_MAP_COMPONENT_ID:
    case L7_FLEX_DVMRP_MAP_COMPONENT_ID:
    case L7_FLEX_PIMSM_MAP_COMPONENT_ID:
    case L7_FLEX_STACKING_COMPONENT_ID:
    case L7_FLEX_SSHD_COMPONENT_ID:
    case L7_FLEX_SSLT_COMPONENT_ID:
    case L7_FLEX_WIRELESS_COMPONENT_ID:
    case L7_DHCP_SNOOPING_COMPONENT_ID:
    case L7_FLEX_METRO_DOT1AD_COMPONENT_ID:
      retVal = cnfgrFlexIsFeaturePresent(componentId, featureId);
      break;

    case L7_DOT1P_COMPONENT_ID:
      retVal = cnfgrDot1pIsFeaturePresent(featureId);
      break;

    case L7_DVLANTAG_COMPONENT_ID:
      retVal = cnfgrDvlantagIsFeaturePresent(featureId);
      break;

    case L7_NIM_COMPONENT_ID:
      retVal = cnfgrNimIsFeaturePresent(featureId);
      break;

    case L7_LOG_COMPONENT_ID:
      retVal = cnfgrLoggingIsFeaturePresent(featureId);
      break;

    case L7_SNTP_COMPONENT_ID:
      retVal = cnfgrSNTPIsFeaturePresent(featureId);
      break;

    case L7_PROTECTED_PORT_COMPONENT_ID:
      return cnfgrIsProtectedPortFeaturePresent(featureId);
      break;

#ifdef L7_POE_PACKAGE
    case L7_POE_COMPONENT_ID:
      return cnfgrIsPoeFeaturePresent(featureId);
      break;
#endif

    case L7_BOX_SERVICES_COMPONENT_ID:
      return cnfgrIsBoxServicesFeaturePresent(featureId);
      break;
  case L7_AUTO_INSTALL_COMPONENT_ID:
    return L7_TRUE;
     break;

  case L7_DOT3AH_COMPONENT_ID:
      retVal = cnfgrIsDot3ahFeaturePresent(featureId);
     break;


    default:
      retVal = L7_FALSE;
      break;
  }

  return retVal;
}

/*********************************************************************
* @purpose  Determines the base network device technology for the platform.
*
* @returns  L7_BASE_TECHNOLOGY_t  Base technology type value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    From an application software perspective, many individual
*           chip identifiers (rev levels, etc.) operate identically.
*
* @end
*********************************************************************/
L7_BASE_TECHNOLOGY_TYPES_t cnfgrBaseTechnologyTypeGet(void)
{
  L7_uint32     npdType = 0;
  L7_BASE_TECHNOLOGY_TYPES_t  tType;

  if (sysapiRegistryGet(NPD_TYPE_ID, U32_ENTRY, &npdType) != L7_SUCCESS)
    npdType = 0;

  switch (npdType)
  {
    case __BROADCOM_5615_ID:
    case __BROADCOM_5645_ID:
      tType = L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA;
      break;

    case __BROADCOM_5650_ID:
    case __BROADCOM_5665_ID:
    case __BROADCOM_5673_ID:
    case __BROADCOM_5674_ID:
    case __BROADCOM_5690_ID:
    case __BROADCOM_5691_ID:
    case __BROADCOM_5692_ID:
    case __BROADCOM_5693_ID:
    case __BROADCOM_5695_ID:
      tType = L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS;
      break;
    case __BROADCOM_56504_ID:
    case __BROADCOM_56304_ID:
    case __BROADCOM_56314_ID:
    case __BROADCOM_56800_ID:
    case __BROADCOM_5675_ID:
    case __BROADCOM_56514_ID:
    case __BROADCOM_56214_ID:
    case __BROADCOM_56218_ID:
    case __BROADCOM_56224_ID:
    case __BROADCOM_53314_ID:
      tType = L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3;
      break;

    case __BROADCOM_56624_ID:
    case __BROADCOM_56680_ID:
    case __BROADCOM_56820_ID:
    case __BROADCOM_56634_ID:
    case __BROADCOM_56524_ID:
    case __BROADCOM_56636_ID:
      tType = L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4;
      break;

    case __BROADCOM_53115_ID:
      tType = L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO;
      break;

    case __MASTER_DRIVER_ID:
      tType = L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER;
      break;

    case 0:
    default:
      tType = L7_BASE_TECHNOLOGY_TYPE_UNKNOWN;
      break;
  }

  return tType;
}

/*********************************************************************
* @purpose  Determines the base network processing technology subtype
*           for the platform.
*
* @returns  L7_BASE_TECHNOLOGY_SUBTYPE_t  Base technology subtype value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    From an application software perspective, many individual
*           chip identifiers (rev levels, etc.) operate identically.
*           SUBTYPE is used to distinguish between different flavors
*           of a chip family, e.g. features like ECMP that are supported
*           in the 5695, but not the 5690, yet they share the same
*           BASE_TECHNOLOGY
*
* @end
*********************************************************************/
L7_BASE_TECHNOLOGY_SUBTYPES_t cnfgrBaseTechnologySubtypeGet(void)
{
  L7_uint32     npdType = 0;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  tType;

  if (sysapiRegistryGet(NPD_TYPE_ID, U32_ENTRY, &npdType) != L7_SUCCESS)
    npdType = 0;

  switch (npdType)
  {
    case __BROADCOM_5690_ID:
    case __BROADCOM_5691_ID:
    case __BROADCOM_5692_ID:
    case __BROADCOM_5693_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5690;
      break;

    case __BROADCOM_5695_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5695;
      break;

    case __BROADCOM_5665_ID:
    case __BROADCOM_5650_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665;
      break;

    case __BROADCOM_5673_ID:
    case __BROADCOM_5674_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5673;
      break;

    case __BROADCOM_5675_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5675;
      break;

    case __BROADCOM_56304_ID:
/*case __BROADCOM_56303_ID:*/
/*case __BROADCOM_56302_ID:*/
/*case __BROADCOM_56301_ID:*/
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5630x;
      break;

    case __BROADCOM_56314_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5631x;
      break;

    case __BROADCOM_56504_ID:
/*case __BROADCOM_56503_ID:*/
/*case __BROADCOM_56502_ID:*/
/*case __BROADCOM_56501_ID:*/
    case __BROADCOM_56800_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5650x;
      break;

    case __BROADCOM_56624_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x;
      break;
    case __BROADCOM_56680_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5668x;
      break;
    case __BROADCOM_56820_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5682x;
      break;
    case __BROADCOM_56514_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x;
      break;
    case __BROADCOM_56214_ID:
    case __BROADCOM_56218_ID:
    case __BROADCOM_56224_ID:
    case __BROADCOM_53314_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x;
      break;
    case __BROADCOM_56634_ID:
    case __BROADCOM_56524_ID:
    case __BROADCOM_56636_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520;
      break;
    case __BROADCOM_53115_ID:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115;
      break;
    case __MASTER_DRIVER_ID:
#ifdef NETWORK_APPLIANCE_HOST
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_MASTERDRIVER_NETAPPL;
#else
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_NONE;
#endif
      break;

    default:
      tType = L7_BASE_TECHNOLOGY_SUBTYPE_NONE;
      break;
  }

  return tType;
}
/*********************************************************************
* @purpose  Determines the base network processing technology ID
*           for the platform.
*
* @returns  L7_NPD_ID_t 
*
* @notes    This call is required to identify the exact device with in
*           a given technology type and subtype. Finer control for cnfgr
*           features.
*
* @end
*********************************************************************/
L7_NPD_ID_t cnfgrBaseTechnologyNpdTypeIdGet(void)
{
  L7_NPD_ID_t npd_id;

  if (sysapiRegistryGet(NPD_TYPE_ID, U32_ENTRY, &npd_id) != L7_SUCCESS)
  {
    npd_id = 0;
  }

  return npd_id;
}

/*********************************************************************
* @purpose  return SystemInitComplete status.
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsSystemInitComplete()
{
  if (currPhase > 1)
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Indicates if the system is a chassis system.
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsChassis()
{
#ifdef L7_CHASSIS
  return L7_TRUE;
#else
  return L7_FALSE;
#endif
}

/*********************************************************************
* @purpose  Indicates if the system is a chassis CFM or LM system.
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cnfgrIsChassisModule(CHASSIS_MODULETYPE_t moduleType)
{
#ifdef L7_CHASSIS
  SYSAPI_CARD_TYPE_t myModuleType;
  L7_int32 mySlot;
  SYSAPI_CARD_INDEX_t myModuleIndex;

  sysapiHpcCurrentSlotGet(&mySlot);
  sysapiHpcSlotMapMPModuleInfoGet(mySlot,&myModuleType, &myModuleIndex);
  if((CONTROL_MODULE == moduleType ) &&
     (myModuleType == SYSAPI_CARD_TYPE_CONTROL))
  {
    return L7_TRUE;
  }
  else if((LINE_MODULE == moduleType ) &&
          (myModuleType == SYSAPI_CARD_TYPE_LINE))
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
#else
  return L7_FALSE;
#endif
}
