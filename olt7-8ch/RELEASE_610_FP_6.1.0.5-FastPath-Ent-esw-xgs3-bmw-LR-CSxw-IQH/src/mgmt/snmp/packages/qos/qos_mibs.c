/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: qos_mibs.c
*
* Purpose: Util functions shared across all agents.
*
* Created by: Colin Verne
*
* Component: SNMP
*
*********************************************************************/
/* lvl7_@p0095 start */
              /* /src/l7public/por/public/ */
/* lvl7_@p0095 end */

#include <l7_common_l3.h>       
#include "snmp_api.h"
#include "usmdb_common.h"
#include "acl_exports.h"
#include "cos_exports.h"
#include "diffserv_exports.h"
#include "iscsi_exports.h"
#include "usmdb_util_api.h"

int
k_private_qos_initialize(void)
{
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_FEATURE_SUPPORTED) == L7_TRUE ||
      usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_FEATURE_SUPPORTED) == L7_TRUE ||
      usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE) 
        SnmpSupportedMibTableEntryCreate("FASTPATH-QOS-MIB",
                                         "FASTPATH Flex QOS Support",
                                         "fastPathQOS");

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_FEATURE_SUPPORTED) == L7_TRUE) 
       SnmpSupportedMibTableEntryCreate("FASTPATH-QOS-ACL-MIB",
                                        "FASTPATH Flex QOS ACL",
                                        "fastPathQOSACL");
  
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_FEATURE_SUPPORTED) == L7_TRUE) 
       SnmpSupportedMibTableEntryCreate("FASTPATH-QOS-COS-MIB",
                                        "FASTPATH Flex QOS COS",
                                        "fastPathQOSCOS");
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_VOIP_COMPONENT_ID) == L7_TRUE)
  {
       SnmpSupportedMibTableEntryCreate("FASTPATH-QOS-AUTOVOIP-MIB",
                                        "FASTPATH Flex QOS VOIP",
                                        "fastPathQOSAUTOVOIP");
  }
  
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE) 
  {
#ifdef L7_INCLUDE_DIFFSERV_STD_MIB_SUPPORT
      SnmpSupportedMibTableEntryCreate("RFC 3289 - DIFFSERV-DSCP-TC",
                                       "Management Information Base for the Textual Conventions used in DIFFSERV-MIB",
                                       "diffServDSCPTC");

      SnmpSupportedMibTableEntryCreate("RFC 3289 - DIFFSERV-MIB",
                                       "Management Information Base for the Differentiated Services Architecture",
                                       "diffServMib");

      SnmpSupportedMibTableEntryCreate("FASTPATH-QOS-DIFFSERV-EXTENSIONS-MIB",
                                       "FASTPATH Flex QOS DiffServ Private MIBs' definitions",
                                       "fastPathQOSDiffServExtensions");
#endif

      SnmpSupportedMibTableEntryCreate("FASTPATH-QOS-DIFFSERV-PRIVATE-MIB",
                                       "FASTPATH Flex QOS DiffServ Private MIBs' definitions",
                                       "fastPathQOSDiffServPrivate");
  }


  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ISCSI_COMPONENT_ID, L7_ISCSI_FEATURE_SUPPORTED) == L7_TRUE) 
  {
      SnmpSupportedMibTableEntryCreate("FASTPATH-QOS-ISCSI-MIB",
                                       "FASTPATH Flex QOS iSCSI Flow Acceleration MIBs' definitions",
                                       "fastPathIscsiFlowAcceleration");
  }
  return 1;
}
