/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cnfgr_flex_packages.c
*
* @purpose Package Controller Main File
*
* @component cnfgr
*
* @comments none
*
* @create 07/23/2001
*
* @author bmutz
*
* @end
*
**********************************************************************/


#define CNFGR_FLEX_FEATURE_SUPPORT

#include <l7_common.h>
#include <osapi.h>
#include "cnfgr.h"
#include "cnfgr_features.h"
#include "l7_flex_packages.h"
#include "snooping_exports.h"
#include "sshd_exports.h"
#include "wireless_exports.h"
#include "sysapi_hpc.h"

#ifdef L7_TRAFFIC_SERVICES_PACKAGE
  #include "cnfgr_ts.h"
#endif

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  #include "acl_api.h"
#endif

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
  #include "l7_diffserv_api.h"
#endif

#ifdef L7_QOS_FLEX_PACKAGE_COS
  #include "l7_cos_api.h"
#endif

#ifdef L7_BGP_PACKAGE
  #include "l3_bgp_default_cnfgr.h"
#endif

#ifdef L7_MCAST_PACKAGE
  #include "l3_mcast_default_cnfgr.h"
#endif

/* local function prototypes */
static L7_BOOL cnfgrFlexIsPimsmPresent();
static L7_BOOL cnfgrFlexQosAclIsFeaturePresent(L7_uint32 featureId);
static L7_BOOL cnfgrFlexQosDiffServIsFeaturePresent(L7_uint32 featureId);
static L7_BOOL cnfgrFlexQosCosIsFeaturePresent(L7_uint32 featureId);
static L7_BOOL cnfgrFlexQosIscsiIsFeaturePresent(L7_uint32 featureId);
#ifdef L7_DHCP_SNOOPING_PACKAGE
static L7_BOOL cnfgrFlexDhcpSnoopingIsFeaturePresent(L7_uint32 featureId);
#endif
#ifdef L7_STACKING_PACKAGE
static L7_BOOL cnfgrFlexStackingIsFeaturePresent(L7_uint32 featureId);
#endif
#ifdef L7_WIRELESS_PACKAGE
static L7_BOOL cnfgrFlexWirelessIsFeaturePresent(L7_uint32 featureId);
#endif /* L7_WIRELESS_PACKAGE */
static L7_BOOL cnfgrFlexSshdIsFeaturePresent(L7_uint32 featureId);

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
static L7_BOOL cnfgrFlexMetroDot1adIsFeaturePresent(L7_uint32 featureId);
static L7_BOOL cnfgrFlexMetroIsDot1adPresent();
#endif
#endif /* L7_METRO_FLEX_PACKAGE */

#ifdef L7_MCAST_PACKAGE
static L7_BOOL cnfgrFlexMgmdIsFeaturePresent(L7_uint32 featureId);
#endif

/* QOS Package feature present matricies for this platform */
L7_BOOL cnfgrFlexQosAclFeaturePresentMatrix[L7_ACL_FEATURE_ID_TOTAL] = { L7_FALSE};
L7_BOOL cnfgrFlexQosDiffServFeaturePresentMatrix[L7_DIFFSERV_FEATURE_ID_TOTAL] = { L7_FALSE};
L7_BOOL cnfgrFlexQosCosFeaturePresentMatrix[L7_COS_FEATURE_ID_TOTAL] = { L7_FALSE};
L7_BOOL dsMatrix[L7_DHCP_SNOOPING_FEATURE_ID_TOTAL] = { L7_FALSE};


/*********************************************************************
* @purpose  Check to see if a Flex package component is present
*
* @param    L7_uint32   componentId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL cnfgrFlexIsComponentPresent(L7_COMPONENT_IDS_t componentId)
{

  switch (componentId)
  {
  case L7_FLEX_PACKAGES_ID:
    return L7_TRUE;
    break;

  case L7_FLEX_MPLS_MAP_COMPONENT_ID:
    #ifdef L7_TRAFFIC_SERVICES_PACKAGE
    return L7_TRUE;
      #endif
    break;

  case L7_FLEX_QOS_ACL_COMPONENT_ID:
      #ifdef L7_QOS_FLEX_PACKAGE_ACL
    return L7_TRUE;
      #endif
    break;

  case L7_FLEX_QOS_DIFFSERV_COMPONENT_ID:
      #ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
    return L7_TRUE;
      #endif
    break;

  case L7_FLEX_QOS_COS_COMPONENT_ID:
      #ifdef L7_QOS_FLEX_PACKAGE_COS
    return L7_TRUE;
      #endif
    break;

  case L7_FLEX_QOS_ISCSI_COMPONENT_ID:
      #ifdef L7_QOS_FLEX_PACKAGE_ISCSI
    return L7_TRUE;
      #endif
    break;

  case L7_FLEX_BGP_MAP_COMPONENT_ID:
    #ifdef L7_BGP_PACKAGE
    return L7_TRUE;
    #endif
    break;

  case L7_FLEX_OPENSSL_COMPONENT_ID:
    #ifdef L7_OPENSSL_PACKAGE
      return L7_TRUE;
    #endif
    break;

  case L7_FLEX_SSHD_COMPONENT_ID:
  case L7_FLEX_SSLT_COMPONENT_ID:
    #ifdef L7_MGMT_SECURITY_PACKAGE
      return L7_TRUE;
    #endif
    break;

  case L7_FLEX_WIRELESS_COMPONENT_ID:
    #ifdef L7_WIRELESS_PACKAGE
      return L7_TRUE;
    #endif
    break;

  case L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID:
    #ifdef L7_CAPTIVE_PORTAL_PACKAGE
      return L7_TRUE;
    #endif
    break;

  case L7_FLEX_MCAST_MAP_COMPONENT_ID:
  case L7_FLEX_PIMDM_MAP_COMPONENT_ID:
  case L7_FLEX_MGMD_MAP_COMPONENT_ID:
  case L7_FLEX_DVMRP_MAP_COMPONENT_ID:
    #ifdef L7_MCAST_PACKAGE
    return L7_TRUE;
    #endif
    break;

  case L7_FLEX_PIMSM_MAP_COMPONENT_ID:
    return cnfgrFlexIsPimsmPresent();
    break;

  case L7_FLEX_STACKING_COMPONENT_ID:
    #ifdef L7_STACKING_PACKAGE
    return L7_TRUE;
    #endif
    break;

  case L7_FLEX_IPV6_MAP_COMPONENT_ID:
  case L7_FLEX_OSPFV3_MAP_COMPONENT_ID:
  case L7_FLEX_DHCP6S_MAP_COMPONENT_ID:
    #ifdef L7_IPV6_PACKAGE
    return L7_TRUE;
    #endif
    break;

  case L7_FLEX_IPV6_MGMT_COMPONENT_ID:
    #ifdef L7_IPV6_MGMT_PACKAGE
    return L7_TRUE;
    #endif
    break;

  case L7_DHCP_SNOOPING_COMPONENT_ID:
    #ifdef L7_DHCP_SNOOPING_PACKAGE
    return L7_TRUE;
    #endif
    break;

  case L7_FLEX_METRO_DOT1AD_COMPONENT_ID:
    #ifdef L7_METRO_FLEX_PACKAGE
    #ifdef L7_DOT1AD_PACKAGE
    return L7_TRUE;
    #endif
    #endif
    break;

  default:
    break;
  }

  return L7_FALSE;

}

/*********************************************************************
* @purpose  Check to see if PIM-SM is present in this configuration.
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_BOOL cnfgrFlexIsPimsmPresent()
{
#ifdef L7_MCAST_PACKAGE
    L7_BASE_TECHNOLOGY_TYPES_t  techType;

    techType = cnfgrBaseTechnologyTypeGet();

    switch (techType)
    {
    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:   /* PTin added: XGS5 family supported */
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
        return L7_TRUE;
        break;

    default:
        return L7_FALSE;
        break;
    }

    return L7_FALSE;
#else
    return L7_FALSE;
#endif
}

/*********************************************************************
* @purpose  Check to see if a Flex package component feature is present
*
* @param    L7_uint32   componentId
* @param    L7_uint32   featureId
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    A feature is a specific part of a component, whose availability
*           is sometimes dictated by the capabilities of the underlying
*           platform.
*
* @end
*********************************************************************/
L7_BOOL cnfgrFlexIsFeaturePresent(L7_COMPONENT_IDS_t componentId, L7_uint32 featureId)
{

  if (componentId < 1 || componentId >= L7_LAST_COMPONENT_ID)
    return L7_FALSE;

  if (cnfgrFlexIsComponentPresent(componentId) != L7_TRUE)
    return L7_FALSE;

  /* Once it is verified the component is present, it depends on the
   * design of each component as to whether there are any individual
   * features that are conditionally supported based on environmental
   * factors, such as hardware platform, operating system, etc.
   */
  switch (componentId)
  {
  case L7_FLEX_PACKAGES_ID:
  case L7_FLEX_MPLS_MAP_COMPONENT_ID:
  case L7_FLEX_SSLT_COMPONENT_ID:
    return L7_TRUE;
    break;

  case L7_FLEX_SSHD_COMPONENT_ID:
    return cnfgrFlexSshdIsFeaturePresent(featureId);
    break;

  case L7_FLEX_QOS_ACL_COMPONENT_ID:
    return cnfgrFlexQosAclIsFeaturePresent(featureId);
    break;

  case L7_FLEX_QOS_DIFFSERV_COMPONENT_ID:
    return cnfgrFlexQosDiffServIsFeaturePresent(featureId);
    break;

  case L7_FLEX_QOS_COS_COMPONENT_ID:
    return cnfgrFlexQosCosIsFeaturePresent(featureId);
    break;

  case L7_FLEX_QOS_ISCSI_COMPONENT_ID:
    return cnfgrFlexQosIscsiIsFeaturePresent(featureId);
    break;

  case L7_FLEX_STACKING_COMPONENT_ID:
    #ifdef L7_STACKING_PACKAGE
    return cnfgrFlexStackingIsFeaturePresent(featureId);
    #endif
    break;

  case L7_FLEX_WIRELESS_COMPONENT_ID:
    #ifdef L7_WIRELESS_PACKAGE
    return cnfgrFlexWirelessIsFeaturePresent(featureId);
    #endif /* L7_WIRELESS_PACKAGE */
    break;
  case L7_FLEX_METRO_DOT1AD_COMPONENT_ID:
    #ifdef L7_METRO_FLEX_PACKAGE
    #ifdef L7_DOT1AD_PACKAGE
    return cnfgrFlexMetroDot1adIsFeaturePresent(featureId);
    #endif
    #endif
    break;

  case L7_DHCP_SNOOPING_COMPONENT_ID:
    #ifdef L7_DHCP_SNOOPING_PACKAGE
    return cnfgrFlexDhcpSnoopingIsFeaturePresent(featureId);
    #endif
    break;

  case L7_FLEX_MGMD_MAP_COMPONENT_ID:
      #ifdef L7_MCAST_PACKAGE
      return cnfgrFlexMgmdIsFeaturePresent(featureId);
      #endif

  default:
    return L7_FALSE;
    break;
  }

  return L7_FALSE;
}

#ifdef L7_MCAST_PACKAGE
/*********************************************************************
* @purpose  Check to see if a feature is present for the Multicast
*           component.
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
static L7_BOOL cnfgrFlexMgmdIsFeaturePresent(L7_uint32 featureId)
{
  if (featureId >= L7_MGMD_FEATURE_ID_TOTAL)
    return L7_FALSE;

  if (featureId == L7_MGMD_FEATURE_CHECK_RTR_ALERT_OPTION)
  {
    switch (cnfgrBaseTechnologySubtypeGet())
    {
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x:
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x:
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x:
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5682x:
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520:
           return L7_TRUE;
           break;
      default:
           return L7_FALSE;
           break;
    }
  }
  return L7_TRUE;
}
#endif

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Check to see if a feature is present for the Stacking
*           component.
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
static L7_BOOL cnfgrFlexStackingIsFeaturePresent(L7_uint32 featureId)
{
 L7_BOOL support = L7_FALSE;
 L7_BASE_TECHNOLOGY_SUBTYPES_t techSubtype;

  if (featureId == L7_STACKING_FEATURE_SUPPORTED)
  {
    return L7_TRUE;
  }

  if (featureId == L7_STACKING_NSF_FEATURE_ID)
  {
#ifdef L7_NSF_PACKAGE
    return sysapiHpcLocalNsfSupported();
#else
    return L7_FALSE;
#endif
  }
  if (featureId == L7_STACKING_SFS_FEATURE_ID)
  {
#if L7_FEAT_STACK_FIRMWARE_SYNC
    return L7_TRUE;
#else
    return L7_FALSE;
#endif
  }
  techSubtype = cnfgrBaseTechnologySubtypeGet();
#if (defined(L7_FRONT_PANEL_STACKING) && (L7_FRONT_PANEL_STACKING == 1))

  switch (featureId)
  {
  case L7_FPS_FEATURE_ID:
  case L7_FPS_PORT_MODE_CFG_FEATURE_ID:
    support = L7_TRUE;
    break;

  case L7_FPS_QOS_REPLACEMENT_FEATURE_ID:
    /* QOS Replacement id is only valid on TUCANA */
    if (techSubtype == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665)
      support = L7_TRUE;
    else
      support = L7_FALSE;
    break;

  default:
    support = L7_FALSE;
    break;
  }
#endif

  return support;

}
#endif /* L7_STACKING_PACKAGE */

/*********************************************************************
* @purpose  Check to see if a feature is present for the QOS ACL component
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
static L7_BOOL cnfgrFlexQosAclIsFeaturePresent(L7_uint32 featureId)
{
    static L7_BOOL    aclFeatureInit = L7_FALSE;
    L7_BOOL    *pMatrix = cnfgrFlexQosAclFeaturePresentMatrix;

    /* check input */
    if (featureId >= L7_ACL_FEATURE_ID_TOTAL)
      return L7_FALSE;

    /* only do the following the first time this function is called, since
     * the technology type does not change while the system is running
     */
    if (aclFeatureInit == L7_FALSE)
    {
      L7_uchar8  *pPlatformList = L7_NULLPTR;
      L7_uint32  i, listEnt = 0;
      L7_BASE_TECHNOLOGY_TYPES_t  techType;
      L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubtype;

      /* set all ACL features to false in support matrix */
      for (i = 0; i < L7_ACL_FEATURE_ID_TOTAL; i++)
        pMatrix[i] = L7_FALSE;

      techType = cnfgrBaseTechnologyTypeGet();
      techSubtype = cnfgrBaseTechnologySubtypeGet();

      switch (techType)
      {
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
        pPlatformList = cnfgrQosAclFeaturesBcm;
        listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_BCM_LIST_ENTRIES;
        break;

      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
        switch (techSubtype)
        {
        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5630x:
          pPlatformList = cnfgrQosAclFeaturesBcmXgs3Helix;
          listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_XGS3_HELIX_LIST_ENTRIES;
          break;

        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5631x:
          pPlatformList = cnfgrQosAclFeaturesBcmXgs3HelixPlus;
          listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_XGS3_HELIXPLUS_LIST_ENTRIES;
          break;

        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5650x:
          pPlatformList = cnfgrQosAclFeaturesBcmXgs3Firebolt;
          listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_XGS3_FIREBOLT_LIST_ENTRIES;
          break;

        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x:
          pPlatformList = cnfgrQosAclFeaturesBcmXgs3Firebolt2;
          listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_XGS3_FIREBOLT2_LIST_ENTRIES;
          break;

        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x:
                     if (cnfgrBaseTechnologyNpdTypeIdGet() == __BROADCOM_56224_ID)
              {
                /* Raven has similar CAP features as Firebolt (do not treat as Raptor) */
                pPlatformList = cnfgrQosAclFeaturesBcmXgs3Firebolt;
                listEnt =  (L7_uint32)CNFGR_QOS_ACL_FEATURES_XGS3_FIREBOLT_LIST_ENTRIES;
              }
              else
              {
                pPlatformList = cnfgrQosAclFeaturesBcmXgs3RaptorEnterprise;
                listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_XGS3_RAPTOR_ENTERPRISE_LIST_ENTRIES;
              }
          break;

        default:
          return L7_FALSE;

        }
        break;

      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:   /* PTin added: XGS5 family supported */
          pPlatformList = cnfgrQosAclFeaturesBcmXgs4;
          listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_XGS4_LIST_ENTRIES;
          break;

      case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        switch (techSubtype)
        {
          case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115:
            pPlatformList = cnfgrQosAclFeaturesBcmRobo53xx;
            listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_ROBO_53xx_LIST_ENTRIES;
            break;
          default:
            return L7_FALSE;
        }
        break;





      case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
        switch (techSubtype)
        {
          case L7_BASE_TECHNOLOGY_SUBTYPE_MASTERDRIVER_NETAPPL:
            pPlatformList = cnfgrQosAclFeaturesBcmXgs3Firebolt2;
            listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_XGS3_FIREBOLT2_LIST_ENTRIES;
          break;

          default:
        pPlatformList = cnfgrQosAclFeaturesFull;
        listEnt = (L7_uint32)CNFGR_QOS_ACL_FEATURES_FULL_LIST_ENTRIES;
        break;
        }
        break;

      /* any platform that does not support ACL defaults to here
       *
       * NOTE: There is no platform list in this case, so all matrix
       *       entries remain set to 'false' indicating no support.
       */
      default:
        break;

      } /* endswitch */

      /* if a platform list was found, use its entries to index the
       * support matrix, setting that entry's value to 'true' indicating
       * the feature is supported
       */
      if (pPlatformList != L7_NULLPTR)
      {
        for (i = 0; i < listEnt; i++)
          pMatrix[ pPlatformList[i] ] = L7_TRUE;
      }

      /* Special Case:  The XGS 5665 with Front Panel Stacking (FPS) cannot
       *                support the redirect feature, since it requires
       *                additional filters on all ports, including the
       *                stacking ports.
       */
      if (techSubtype == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665)
      {
        if (cnfgrIsFeaturePresent (L7_FLEX_STACKING_COMPONENT_ID,
                                   L7_FPS_FEATURE_ID) == L7_TRUE)
          pMatrix[L7_ACL_REDIRECT_FEATURE_ID] = L7_FALSE;
      }

      /* Special case. These features may not be available for the product type
                       Use the configured value rather than the initialized one.

      */
      if (techSubtype == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x)
      {
         pMatrix[L7_ACL_MIRROR_FEATURE_ID] = L7_FEAT_ACL_MIRROR;
         pMatrix[L7_ACL_LOG_DENY_FEATURE_ID] = L7_FEAT_ACL_LOG_DENY;
      }

      
      /* Special case. If Time Ranges component is not present disable the Time 
                       based ACL feature
       */      
      if (cnfgrIsComponentPresent(L7_TIMERANGES_COMPONENT_ID) != L7_TRUE)
      {
        pMatrix[L7_ACL_PER_RULE_ACTIVATION_ID] = L7_FALSE;
      }

      aclFeatureInit = L7_TRUE;

    } /* endif not initialized */


    /* Check the feature support matrix.  If DiffServ not supported, don't
     * check the individual feature ID.
     */
    if (pMatrix[L7_ACL_FEATURE_SUPPORTED] == L7_FALSE)
      return L7_FALSE;

    return pMatrix[featureId];
}

/*********************************************************************
* @purpose  Check to see if a feature is present for the QOS DiffServ
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
static L7_BOOL cnfgrFlexQosDiffServIsFeaturePresent(L7_uint32 featureId)
{
  static L7_BOOL    diffServFeatureInit = L7_FALSE;
  L7_BOOL           *pMatrix = cnfgrFlexQosDiffServFeaturePresentMatrix;

  /* check input */
  if (featureId >= L7_DIFFSERV_FEATURE_ID_TOTAL)
    return L7_FALSE;

  /* only do the following the first time this function is called, since
   * the technology type does not change while the system is running
   */
  if (diffServFeatureInit == L7_FALSE)
  {
    L7_uchar8  *pPlatformList = L7_NULLPTR;
    L7_uint32  i, listEnt = 0;
    L7_BASE_TECHNOLOGY_TYPES_t  techType;
    L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubtype;

    /* set all DiffServ features to false in support matrix */
    for (i = 0; i < L7_DIFFSERV_FEATURE_ID_TOTAL; i++)
      pMatrix[i] = L7_FALSE;

    techType = cnfgrBaseTechnologyTypeGet();
    techSubtype = cnfgrBaseTechnologySubtypeGet();

    switch (techType)
    {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
      pPlatformList = cnfgrQosDiffServFeaturesBcmStrata;
      listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_STRATA_LIST_ENTRIES;
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
      pPlatformList = cnfgrQosDiffServFeaturesBcmXgs;
      listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS_LIST_ENTRIES;
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
      switch (techSubtype)
      {
      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5630x:
        pPlatformList = cnfgrQosDiffServFeaturesBcmXgs3Helix;
        listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS3_HELIX_LIST_ENTRIES;
        break;

      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5631x:
        pPlatformList = cnfgrQosDiffServFeaturesBcmXgs3HelixPlus;
        listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS3_HELIXPLUS_LIST_ENTRIES;
        break;

      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5650x:
        pPlatformList = cnfgrQosDiffServFeaturesBcmXgs3Firebolt;
        listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS3_FIREBOLT_LIST_ENTRIES;
        break;

      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x:
        pPlatformList = cnfgrQosDiffServFeaturesBcmXgs3Firebolt2;
        listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS3_FIREBOLT2_LIST_ENTRIES;
        break;

      case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x:
          if (cnfgrBaseTechnologyNpdTypeIdGet() == __BROADCOM_56224_ID)
          {
            /* Raven has similar CAP features as Firebolt (do not treat as Raptor) */
            pPlatformList = cnfgrQosDiffServFeaturesBcmXgs3Firebolt;
            listEnt =  (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS3_FIREBOLT_LIST_ENTRIES;
          }
          else
          {
            pPlatformList = cnfgrQosDiffServFeaturesBcmXgs3Raptor;
            listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS3_RAPTOR_LIST_ENTRIES;
          }

        break;

      default:
        return L7_FALSE;
      }
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:   /* PTin added: XGS5 family supported */
        pPlatformList = cnfgrQosDiffServFeaturesBcmXgs4;
        listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS4_LIST_ENTRIES;
        break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        switch (techSubtype)
        {
          case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115:
          pPlatformList = cnfgrQosDiffServFeaturesBcmRobo53xx;
          listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_ROBO_53xx_LIST_ENTRIES;
          break;

          default:
            return L7_FALSE;
        }
        break;




    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
        switch (techSubtype)
        {
          case L7_BASE_TECHNOLOGY_SUBTYPE_MASTERDRIVER_NETAPPL:
            pPlatformList = cnfgrQosDiffServFeaturesBcmXgs3Firebolt2;
            listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_XGS3_FIREBOLT2_LIST_ENTRIES;
          break;

          default:
      pPlatformList = cnfgrQosDiffServFeaturesFull;
      listEnt = (L7_uint32)CNFGR_QOS_DIFFSERV_FEATURES_FULL_LIST_ENTRIES;
      break;
        }
      break;

    /* any platform that does not support DiffServ defaults to here
     *
     * NOTE: There is no platform list in this case, so all matrix
     *       entries remain set to 'false' indicating no support.
     */
    default:
      break;

    } /* endswitch */

    /* if a platform list was found, use its entries to index the
     * support matrix, setting that entry's value to 'true' indicating
     * the feature is supported
     */
    if (pPlatformList != L7_NULLPTR)
    {
      for (i = 0; i < listEnt; i++)
        pMatrix[ pPlatformList[i] ] = L7_TRUE;
    }

    /* Special Case:  The XGS 5665 with Front Panel Stacking (FPS) cannot
     *                support the redirect feature, since it requires
     *                additional filters on all ports, including the
     *                stacking ports.
     */
    if (techSubtype == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665)
    {
      if (cnfgrIsFeaturePresent (L7_FLEX_STACKING_COMPONENT_ID,
                                 L7_FPS_FEATURE_ID) == L7_TRUE)
        pMatrix[L7_DIFFSERV_POLICY_ATTR_REDIRECT_FEATURE_ID] = L7_FALSE;
    }

    /* Special case. These features may not be available for the product type
                     Use the configured value rather than the initialized one.

    */
    if (techSubtype == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x)
    {
       pMatrix[L7_DIFFSERV_POLICY_ATTR_MIRROR_FEATURE_ID] = L7_FEAT_DIFFSERV_POLICY_ATTR_MIRROR;
       pMatrix[L7_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL_FEATURE_ID] = L7_FEAT_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL;
    }

    diffServFeatureInit = L7_TRUE;

  } /* endif not initialized */


  /* Check the feature support matrix.  If DiffServ not supported, don't
   * check the individual feature ID.
   */
  if (pMatrix[L7_DIFFSERV_FEATURE_SUPPORTED] == L7_FALSE)
    return L7_FALSE;

  return pMatrix[featureId];
}

/**************************************************************************
* @purpose  Check to see if a feature is present for the QOS COS component
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
***************************************************************************/
static L7_BOOL cnfgrFlexQosCosIsFeaturePresent(L7_uint32 featureId)
{
  static L7_BOOL    cosFeatureInit = L7_FALSE;
  L7_BOOL           *pMatrix = cnfgrFlexQosCosFeaturePresentMatrix;

  /* check input */
  if (featureId >= L7_COS_FEATURE_ID_TOTAL)
    return L7_FALSE;

  /* only do the following the first time this function is called, since
   * the technology type does not change while the system is running
   */
  if (cosFeatureInit == L7_FALSE)
  {
    L7_uchar8  *pPlatformList = L7_NULLPTR;
    L7_uint32  i, listEnt = 0;
    L7_BASE_TECHNOLOGY_TYPES_t  techType;
    L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubtype;

    /* set all COS features to false in support matrix */
    for (i = 0; i < L7_COS_FEATURE_ID_TOTAL; i++)
      pMatrix[i] = L7_FALSE;

    techType = cnfgrBaseTechnologyTypeGet();
    techSubtype = cnfgrBaseTechnologySubtypeGet();

    switch (techType)
    {
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA:
      pPlatformList = cnfgrQosCosFeaturesBcmStrata;
      listEnt = (L7_uint32)CNFGR_QOS_COS_FEATURES_STRATA_LIST_ENTRIES;
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
      pPlatformList = cnfgrQosCosFeaturesBcmXgs3;
      listEnt = (L7_uint32)CNFGR_QOS_COS_FEATURES_XGS3_LIST_ENTRIES;
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS:
      pPlatformList = cnfgrQosCosFeaturesBcmXgs;
      listEnt = (L7_uint32)CNFGR_QOS_COS_FEATURES_XGS_LIST_ENTRIES;
      break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:   /* PTin added: XGS5 family supported */
        pPlatformList = cnfgrQosCosFeaturesBcmXgs4;
        listEnt = (L7_uint32)CNFGR_QOS_COS_FEATURES_XGS4_LIST_ENTRIES;
        break;
        break;

    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
      switch (techSubtype)
      {
        case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115:
          pPlatformList = cnfgrQosCosFeaturesBcmRobo;
          listEnt = (L7_uint32)CNFGR_QOS_COS_FEATURES_ROBO_LIST_ENTRIES;
          break;
        default :
          pPlatformList = cnfgrQosCosFeaturesBcmRobo;
          listEnt = (L7_uint32)CNFGR_QOS_COS_FEATURES_ROBO_LIST_ENTRIES;
      }
      break;

    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
      switch (techSubtype)
      {
        case L7_BASE_TECHNOLOGY_SUBTYPE_MASTERDRIVER_NETAPPL:
          pPlatformList = cnfgrQosCosFeaturesBcmXgs3;
          listEnt = (L7_uint32)CNFGR_QOS_COS_FEATURES_XGS3_LIST_ENTRIES;
        break;

        default:
      pPlatformList = cnfgrQosCosFeaturesFull;
      listEnt = (L7_uint32)CNFGR_QOS_COS_FEATURES_FULL_LIST_ENTRIES;
      break;
      }
      break;

    /* any platform that does not support COS defaults to here
     *
     * NOTE: There is no platform list in this case, so all matrix
     *       entries remain set to 'false' indicating no support.
     */
    default:
      break;

    } /* endswitch */

    /* if a platform list was found, use its entries to index the
     * support matrix, setting that entry's value to 'true' indicating
     * the feature is supported
     */
    if (pPlatformList != L7_NULLPTR)
    {
      for (i = 0; i < listEnt; i++)
        pMatrix[ pPlatformList[i] ] = L7_TRUE;
    }

    /* Special Case:  The 5695/5665 are the only BCM platform that supports
     *                the IP DSCP mapping table.  Turn this feature off
     *                for the other BCM technology subtypes.
     *
     * NOTE: There must be a more elegant way to handle subtype differences,
     *       so avoid the temptation of copying this to use in other places!
     */
    if (techType == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS &&
        techType != L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4 &&
        techType != L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5)
    {
      if (techSubtype == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665)
      {
        /* 5665 does support global DSCP mapping table. */
        pMatrix[L7_COS_MAP_IPDSCP_FEATURE_ID] = L7_TRUE;
        pMatrix[L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID] = L7_FALSE;
      }
      else if (techSubtype != L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5695)
      {
        pMatrix[L7_COS_MAP_IPDSCP_FEATURE_ID] = L7_FALSE;
        pMatrix[L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID] = L7_FALSE;
      }
    }

    cosFeatureInit = L7_TRUE;

  } /* endif not initialized */

  /* Check the feature support matrix.  If COS not supported, don't
   * check the individual feature ID.
   */
  if (pMatrix[L7_COS_FEATURE_SUPPORTED] == L7_FALSE)
    return L7_FALSE;

  return pMatrix[featureId];
}
/*********************************************************************
* @purpose  Check to see if a feature is present for the QOS iSCSI Flow Acceleration
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
static L7_BOOL cnfgrFlexQosIscsiIsFeaturePresent(L7_uint32 featureId)
{
  L7_BASE_TECHNOLOGY_TYPES_t      techType;

  /* check input */
  if (featureId >= L7_ISCSI_FEATURE_ID_TOTAL)
    return L7_FALSE;

  techType = cnfgrBaseTechnologyTypeGet();

  switch (featureId)
  {
  case L7_ISCSI_FEATURE_SUPPORTED:
    /* general support item... if component is present, this is supported */
    return L7_TRUE;
    break;
  case L7_ISCSI_DOT1P_MARKING_FEATURE_ID:
    switch (techType)
    {
    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
      return L7_TRUE;
      break;
    default:
      break;
    }
    break;

  default:
    break;
  }

  return L7_FALSE;
}


#ifdef L7_DHCP_SNOOPING_PACKAGE
/*********************************************************************
* @purpose  Check to see if a feature is present for the DHCP Snooping
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
static L7_BOOL cnfgrFlexDhcpSnoopingIsFeaturePresent(L7_uint32 featureId)
{
    L7_uint32         i = 0;
    static L7_BOOL    dsFeatureInit = L7_FALSE;
    L7_NPD_ID_t       npd_id; 

    /* check input */
    if (featureId >= L7_DHCP_SNOOPING_FEATURE_ID_TOTAL)
      return L7_FALSE;

    /* only do the following the first time this function is called, since
     * the technology type does not change while the system is running
     */
    if (dsFeatureInit == L7_FALSE)
    {
      /* set all DHCP Snooping features as not supported by default */
      for (i = 0; i < L7_DHCP_SNOOPING_FEATURE_ID_TOTAL; i++)
      {
        dsMatrix[i] = L7_FALSE;
      }

      npd_id = cnfgrBaseTechnologyNpdTypeIdGet();

#ifdef L7_IPSG_PACKAGE
      switch (npd_id)
      {
        /* The following devices support IPSG. */
        case __BROADCOM_56514_ID:
        case __BROADCOM_56624_ID:
        case __BROADCOM_56680_ID:
        case __BROADCOM_56685_ID:  /* PTin added: new switch 56689 (Valkyrie2) */
        case __BROADCOM_56843_ID:  /* PTin added: new switch 56843 (Trident) */
        case __BROADCOM_56820_ID:
        case __BROADCOM_56634_ID:
        case __BROADCOM_56524_ID:
        case __BROADCOM_56636_ID:
        case __BROADCOM_56334_ID:
        case __BROADCOM_56640_ID:  /* PTin added: new switch 5664x (Triumph3) */
        case __BROADCOM_56340_ID:  /* PTin added: new switch 56340 (Helix4) */
          dsMatrix[L7_DHCP_SNOOPING_IPSG] = L7_TRUE;

        default:
#if L7_FEAT_IPSG_ON_IFP
          dsMatrix[L7_DHCP_SNOOPING_IPSG] = L7_TRUE;
#endif
          break;
      }
#endif
      dsMatrix[L7_DHCP_SNOOPING_DB_PERSISTENCY] = L7_FEAT_DHCP_SNOOPING_DB_PERSISTENCY;  

      dsFeatureInit = L7_TRUE;
    } /* endif not initialized */

    return dsMatrix[featureId];
}
#endif

#ifdef L7_WIRELESS_PACKAGE
/**************************************************************************
* @purpose  Check to see if a feature is present for the Wireless component
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
***************************************************************************/

static L7_BOOL cnfgrFlexWirelessIsFeaturePresent(L7_uint32 featureId)
{
    L7_BASE_TECHNOLOGY_TYPES_t      techType;
    L7_BASE_TECHNOLOGY_SUBTYPES_t   techSubtype;

    /* check input */

    if (featureId >= L7_WIRELESS_FEATURE_ID_TOTAL)
    {
        return L7_FALSE;
    }

    if (featureId == L7_WIRELESS_NETWORK_VISUALIZATION_FEATURE_ID)
    {
        return L7_TRUE;
    }

    techType = cnfgrBaseTechnologyTypeGet();

    /*
    At the moment, we don't yet need the techSubType,
    but it's here for future use
    */

    techSubtype = cnfgrBaseTechnologySubtypeGet();

    switch (techType)
    {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:   /* PTin added: XGS5 family supported */
            switch (featureId)
            {
                case L7_WIRELESS_TUNNELING_FEATURE_ID:
                    #ifdef L7_ROUTING_PACKAGE
                    #ifdef L7_TUNNELING_PACKAGE 
                    return L7_TRUE;
                    #endif
                    #endif /* L7_ROUTING_PACKAGE */
                    break;
                case L7_WIRELESS_DIST_TUNNELING_FEATURE_ID:
                    return L7_TRUE;
                    break;
                case L7_WIRELESS_AP_IMAGE_MODE_INDEPENDENT_FEATURE_ID:
                    return L7_TRUE;
                    break;
                case L7_WIRELESS_AP_IMAGE_MODE_INTEGRATED_FEATURE_ID:
                    return L7_FALSE;
                    break;
                case L7_WIRELESS_TSPEC_FEATURE_ID:
                    return L7_TRUE;
                    break;
                default:
                    break;
                case L7_WIRELESS_L2_CENTTNNL_FEATURE_ID:
                    if (techSubtype == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520)
                    {
                      return L7_TRUE;
                    }
                    break;
            }
            break;
        case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
            switch (featureId)
            {
                case L7_WIRELESS_L2_CENTTNNL_FEATURE_ID:
                    return L7_TRUE;
                    break;
                case L7_WIRELESS_TUNNELING_FEATURE_ID:
                    #ifdef L7_ROUTING_PACKAGE
                    #ifdef L7_TUNNELING_PACKAGE 
                    return L7_TRUE;
                    #endif
                    #endif /* L7_ROUTING_PACKAGE */
                    break;
                case L7_WIRELESS_DIST_TUNNELING_FEATURE_ID:
                    return L7_TRUE;
                    break;
                case L7_WIRELESS_AP_IMAGE_MODE_INDEPENDENT_FEATURE_ID:
                case L7_WIRELESS_AP_IMAGE_MODE_INTEGRATED_FEATURE_ID:
                    return L7_TRUE;
                    break;
                case L7_WIRELESS_TSPEC_FEATURE_ID:
                    return L7_TRUE;
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return L7_FALSE;
}
#endif /* L7_WIRELESS_PACKAGE */

/**************************************************************************
* @purpose  Check to see if a feature is present for the SSH component
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
***************************************************************************/

static L7_BOOL cnfgrFlexSshdIsFeaturePresent(L7_uint32 featureId)
{
  L7_BOOL             featureSupported = L7_FALSE;

  /* check input */
  if (featureId >= L7_SSHD_FEATURE_ID_TOTAL)
  {
    return featureSupported;
  }

  switch (featureId)
  {
    case L7_SSHD_SECURE_TRANSFER_FEATURE_ID:
      featureSupported = L7_FEAT_SSHD_SECURE_TRANSFER;
      break;

    case L7_SSHD_FEATURE_ID:
      featureSupported = L7_TRUE;
      break;

    default:
      break;
  }

  return featureSupported;
}

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
/*********************************************************************
* @purpose  Check to see if PIM-SM is present in this configuration.
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_BOOL cnfgrFlexMetroIsDot1adPresent()
{
  L7_BOOL result = L7_FALSE;
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubtype;

  techType = cnfgrBaseTechnologyTypeGet();
  techSubtype = cnfgrBaseTechnologySubtypeGet();

  switch (techType)
  {
    case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER:
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
         result = L7_TRUE;
         break;
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
          switch(techSubtype)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x:
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520:
              result = L7_TRUE;
            default:
              break;
          }
        break;

    /* PTin added: XGS5 family supported */
    case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:
      result = L7_FALSE;
      break;

    default:
        break;
  }

  return result;
}

/****************************************************************************
* @purpose  Check to see if Feature is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL cnfgrFlexMetroDot1adIsFeaturePresent(L7_uint32 featureId)
{
  L7_BOOL result = L7_FALSE;
  L7_BASE_TECHNOLOGY_TYPES_t  techType;
  L7_BASE_TECHNOLOGY_SUBTYPES_t  techSubtype;

  techType = cnfgrBaseTechnologyTypeGet();
  techSubtype = cnfgrBaseTechnologySubtypeGet();
  
  switch(featureId)
  {
    case L7_FLEX_METRO_DOT1AD_SUPPORTED_FEATURE_ID:
         return cnfgrFlexMetroIsDot1adPresent();
         break;
    case L7_FLEX_METRO_DOT1AD_VLAN_BASED_PROTOCOL_TUNNEL_FEATURE_ID:
    { 
      switch(techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        {
          switch(techSubtype)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x:
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520:
              result = L7_TRUE;
            break;
            default:
            break;
          }
        }
        break;

        /* PTin added: XGS5 family supported */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:
          result = L7_FALSE;
          break;

        default:
        break;
      }
    }
    break;

    case L7_FLEX_METRO_DOT1AD_INTF_BASED_PROTOCOL_TUNNEL_FEATURE_ID:
    {
      switch(techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        {
          switch(techSubtype)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115:
              result = L7_TRUE;
            break;
            default:
            break;
          }
        }
        break;
        default:
        break;
      }
    }
    break;

    case L7_FLEX_METRO_DOT1AD_PB_FEATURE_ID:
    {
      switch(techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER :
          result = L7_TRUE;
          break;
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:   /* PTin added: XGS5 family supported */
        {
          switch(techSubtype)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x:
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520:
              result = L7_TRUE;
            break;
            default:
            break;
          }
        }
        break;

        /* PTin added: XGS5 family supported */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:
          result = L7_FALSE;
          break;

        default:
        break;
      }
    }
    break;
    case L7_FLEX_METRO_DOT1AD_CPE_FEATURE_ID:
    {
      switch(techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO:
        {
          switch(techSubtype)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115:
              result = L7_TRUE;
            break;
            default:
            break;
          }
        }
        break;
        default:
        break;
      }
    }
    break;

    case L7_FLEX_METRO_DOT1AD_ELINE_ON_LAG_FEATURE_ID:
    {
      switch(techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:   /* PTin added: XGS5 family supported */
        {
          switch(techSubtype)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x:
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520:
              result = L7_TRUE;
            break;
            default:
            break;
          }
        }
        break;

        /* PTin added: XGS5 family supported */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:
          result = L7_FALSE;
          break;

        default:
        break;
      }
    }
    break;
    
    case L7_FLEX_METRO_DOT1AD_VLAN_CROSSCONNECT_ELINE_FEATURE_ID:
    {
      switch(techType)
      {
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4:
        {
          switch(techSubtype)
          {
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x:
            case L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520:
              result = L7_TRUE;
            break;
            default:
            break;
          }
        }
        break;

        /* PTin added: XGS5 family supported */
        case L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5:
          result = L7_FALSE;
          break;

        default:
        break;
      }
    }
    break;

    
    case L7_FLEX_METRO_DOT1AD_DATA_TUNNEL_FEATURE_ID:
      result = L7_TRUE;
      break;

    case L7_FLEX_METRO_DOT1AD_SVLAN_FEATURE_ID:
      result = L7_FALSE;
      break;
   
    default:
      break;
  }
  
  return result;
}
#endif
#endif /*L7_METRO_FLEX_PACKAGE */

