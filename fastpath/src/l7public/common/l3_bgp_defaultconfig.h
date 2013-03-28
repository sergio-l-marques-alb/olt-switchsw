/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l3_bgp_defaultconfig.h
*
* @purpose All Factory default settings are in this file
*
* @component none
*
* @comments Only Factory config files should include this header
*           (ie. nim_config.c, sim_config.c)
*
* @create 02/18/2002
*
* @author
* @end
*
**********************************************************************/

#ifndef INCLUDE_L3_BGP_CONFIG_DEFAULTS
#define INCLUDE_L3_BGP_CONFIG_DEFAULTS

#include <string.h>                /* for memcpy() etc... */
#include <datatypes.h>
#include "l7_product.h"
#include <registry.h>
#include <commdefs.h>
#include <l7_ip_api.h>
#include <l3_commdefs.h>



/* Start of BGP Map Component's Factory Defaults */
/* Start of BGP Map Component's Factory Defaults */
/* Start of BGP Map Component's Factory Defaults */
/* Start of BGP Map Component's Factory Defaults */
/* Start of BGP Map Component's Factory Defaults */



/*  BGP Component Defines */

/* bgpGlobalCfgData_t default values */
#define FD_BGP_DEFAULT_ADMIN_MODE			L7_ENABLE
#define FD_BGP_DEFAULT_TRACE_MODE			L7_DISABLE
#define FD_BGP_DEFAULT_TRAP_MODE			L7_DISABLE
#define FD_BGP_DEFAULT_LOCAL_AS				0
#define FD_BGP_DEFAULT_PROPAGATE_MED_MODE		L7_DISABLE
#define FD_BGP_DEFAULT_CALC_MED_MODE			L7_DISABLE
#define FD_BGP_DEFAULT_DIS_AD_BDR_RTR_MODE		L7_DISABLE
#define FD_BGP_DEFAULT_MIN_AS_ORIG_INTERVAL		15
#define FD_BGP_DEFAULT_MIN_ROUTE_AD_INTERVAL		30
#define FD_BGP_DEFAULT_OPTIONAL_CAP_SUPPORT		0
#define FD_BGP_DEFAULT_ROUTE_REFLECTOR_MODE		L7_DISABLE
#define FD_BGP_DEFAULT_NUM_OF_SNPA			0
#define FD_BGP_DEFAULT_AUTO_RESTART_OPTION		L7_DISABLE

#define FD_BGP_DEFAULT_ROUTE_ORIGIN		L7_BGP4_ORIGIN_IGP
#define FD_BGP_DEFAULT_ROUTE_MED		0
#define FD_BGP_DEFAULT_ROUTE_LOCAL_PREF		100
#define FD_BGP_DEFAULT_LESS_SPEC_ROUTE_SELECT	L7_DISABLE
#define FD_BGP_DEFAULT_ROUTE_COMMUNITY		0
#define FD_BGP_DEFAULT_NEXT_HOP_ADDR_LEN	0
#define FD_BGP_DEFAULT_PATH_AGGR_MODE		L7_DISABLE
#define FD_BGP_DEFAULT_ADDR_AGGR_MODE		L7_DISABLE
#define FD_BGP_DEFAULT_ROUTE_FLAP_MODE		L7_DISABLE
#define FD_BGP_DEFAULT_FLAP_SUPPRESS_LIMIT	600
#define FD_BGP_DEFAULT_FLAP_REUSE_LIMIT		2
#define FD_BGP_DEFAULT_FLAP_PENALTY_INC		100
#define FD_BGP_DEFAULT_FLAP_DELTA_TIME		1
#define FD_BGP_DEFAULT_FLAP_MAX_TIME		900
#define FD_BGP_DEFAULT_FLAP_DAMPING_FACTOR	2
#define FD_BGP_DEFAULT_FLAP_REUSE_SIZE		1024

/* bgpPeerCfgData_s default values */

#define FD_BGP_PEER_DEFAULT_PEER_ID                0
#define FD_BGP_PEER_DEFAULT_PEER_ADMIN_STATUS      L7_BGP_START
#define FD_BGP_PEER_DEFAULT_LOCAL_ADDR             0
#define FD_BGP_PEER_DEFAULT_LOCAL_PORT             0
#define FD_BGP_PEER_DEFAULT_REMOTE_ADDR            0
#define FD_BGP_PEER_DEFAULT_REMOTE_PORT            179
#define FD_BGP_PEER_DEFAULT_REMOTE_AS              0
#define FD_BGP_PEER_DEFAULT_CONN_RETRY_INTERVAL    120
#define FD_BGP_PEER_DEFAULT_HOLD_TIME_CFG          90
#define FD_BGP_PEER_DEFAULT_KEEP_ALIVE_CFG         30
#define FD_BGP_PEER_DEFAULT_OPTIONAL_CAPS          0
#define FD_BGP_PEER_DEFAULT_ROUTE_REFLECTOR_CLIENT L7_DISABLE
#define FD_BGP_PEER_DEFAULT_CONFED_MEMBER_FLAG     L7_DISABLE
#define FD_BGP_PEER_DEFAULT_NEXT_HOP_SELF          L7_DISABLE
#define FD_BGP_PEER_DEFAULT_AUTH_CODE              L7_BGP_NULL_AUTH
#define FD_BGP_PEER_DEFAULT_AUTH_LENGTH            0
#define FD_BGP_PEER_DEFAULT_LOCAL_IF_ADDR          0
#define FD_BGP_PEER_DEFAULT_MSG_SEND_LIMIT         100
#define FD_BGP_PEER_DEFAULT_TX_DELAY_INTERVAL      5
#define FD_BGP_PEER_DEFAULT_PFX_WARN_ONLY          L7_FALSE
#define FD_BGP_PEER_DEFAULT_PFX_LIMIT              2000
#define FD_BGP_PEER_DEFAULT_PFX_THRESH             75
#define FD_BGP_PEER_DEFAULT_CONFED_ID              0

/* Defaults related to Route Redistribution. */

/* By default, BGP does not redistribute. */
#define FD_BGP_DEFAULT_REDISTRIBUTE                L7_FALSE

/* By default, no distribute list out is configured. */
#define FD_BGP_DEFAULT_DIST_LIST_OUT               0

/* By default, redistributed routes don't contain a MED attribute. */
#define FD_BGP_DEFAULT_METRIC                      0

/* By default, BGP does not allow redistribution of a default route. */
#define FD_BGP_DEFAULT_ORIG_DEFAULT_ROUTE          L7_FALSE

/* By default, BGP does not redistribute external routes from OSPF. */
#define FD_BGP_DEFAULT_REDIST_MATCH_OSPF_TYPE      L7_OSPF_METRIC_TYPE_INTERNAL
#define L7_RTO_PREFRENCE_DEFAULT                   170
/* End of BGP Component's Factory Defaults */

#endif /* include_config_defaults */
