
#ifndef _FASTPATH_INC_H_
#define _FASTPATH_INC_H_

#include "commdefs.h"
#include "l3_addrdefs.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "osapi_support.h"
#include "usmdb_sim_api.h"
#include "usmdb_unitmgr_api.h"
#ifdef L7_STACKING_PACKAGE
#include "unitmgr_api.h"
/* for stacking run_ifs */
#include "fpobj_stackingRunifUtils.h"
#endif
#include "l7_resources.h"
#include "l7_web_session_api.h"
#include "sntp_api.h"
#include "user_mgr_api.h"
#include "l7utils_api.h"
#include "l3_mcast_defaultconfig.h"
#include "l7_product.h"
#include "l3_commdefs.h"
#include "l3_compdefs.h"
#include "l3_mcast_commdefs.h"
#include "radius_api.h"
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#endif
#include "sflow_exports.h"
#include "tacacs_exports.h"
#include "sslt_exports.h"
#include "snooping_exports.h"
#include "garp_exports.h"
#include "sshd_exports.h"
#include "tacacs_exports.h"
#include "acl_exports.h"
#include "voicevlan_exports.h"
#include "timezone_exports.h"
#include "user_manager_exports.h"
#include "ping_exports.h"
#include "traceroute_exports.h"
#include "ipv6_commdefs.h"
#include "dhcps_exports.h"
#include "lldp_exports.h"
#include "dot1s_exports.h"
#include "dhcp_snooping_exports.h"
#include "dot3ad_exports.h"
#include "tr069_exports.h"
#include "log_exports.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_qos_cos_api.h"
#ifdef L7_WIRELESS_PACKAGE
#include "wireless_exports.h"
#include "wireless_commdefs.h"
#include "usmdb_wdm_ap_api.h"
#endif
#include "usmdb_mib_diffserv_api.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "l3_defaultconfig.h"
#include "rmon_exports.h"
#include "iscsi_exports.h"
#include "dhcp_client_api.h"
#include "usmdb_sdm.h"

#include "pml_exports.h"

#ifdef L7_DOT3AH_PACKAGE
#include "dot3ah_exports.h"
#endif

#ifdef L7_TR069_PACKAGE
#include "tr069_exports.h"
#endif

#ifndef _L7_OS_ECOS_
#define diag_printf printf
#endif

#define FALSE 0
#define TRUE  !(FALSE)

#include "poe_exports.h"

#ifdef L7_IPV6_PACKAGE
#include "ipv6_commdefs.h"
#include "ipv6_defaultconfig.h"
#endif
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#include "usmdb_dhcp6s_api.h"
#endif
#ifdef L7_WIRELESS_PACKAGE
#include "wireless_defaultconfig.h"
/******These includes are to support RUN IFS for wireless objects*******/
#include "usmdb_wdm_api.h" 
/*****RUN IFS FOR WIRELESS ENDS**************/
/*********** common .h file inclusion for wireless run if routines ************/
#include "fpobj_wirelessRunIfUtils.h"
#endif
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#include "usmdb_dhcp6s_api.h"
#endif

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_pimsm_api.h"
#endif

#ifdef L7_QOS_PACKAGE
#include "l7_cos_api.h"
#endif
#ifdef L7_DOT1AG_PACKAGE
#include "dot1ag_exports.h"
#endif

#include "timerange_exports.h"

/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
#define L7_Min 10
#define L7_Max 100
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */
/* TEMP */

#endif
