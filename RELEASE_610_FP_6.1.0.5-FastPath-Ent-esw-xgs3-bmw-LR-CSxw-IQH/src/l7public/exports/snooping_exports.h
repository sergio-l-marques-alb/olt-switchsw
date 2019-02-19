/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snooping_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application
*
* @component
*
* @comments
*
*
* @Notes   This file is included by l7_product.h
*
* @created 03/26/2008
*
* @author akulkarn
* @end
*
**********************************************************************/

#ifndef  __SNOOPING_EXPORTS_H__
#define  __SNOOPING_EXPORTS_H__

#include "flex.h"

// PTin added
#define SNOOP_INTF_INDICES  L7_INTF_INDICES
#define SNOOP_GPON_INDICES  8
#define SNOOP_MAX_NUMBER_OF_CLIENTS_PER_PON_INTF  128
#define SNOOP_MAX_NUMBER_OF_CLIENTS               ((L7_uint16) SNOOP_MAX_NUMBER_OF_CLIENTS_PER_PON_INTF*SNOOP_GPON_INDICES)
#define SNOOP_IGMP_DEFAULT_PRIO                   5
// PTin end

/********************Feature List supported by the component *****************************/
/* Snooping Component Feature List */
typedef enum
{
  L7_IGMP_SNOOPING_FEATURE_SUPPORTED = 0,       /* IGMP Snooping support statement */
  L7_IGMP_SNOOPING_FEATURE_PER_VLAN,            /* IGMP Snooping per VLAN support */
  L7_MLD_SNOOPING_FEATURE_SUPPORTED,            /* MLD Snooping support statement  */
  L7_MLD_SNOOPING_FEATURE_PER_VLAN,             /* MLD Snooping per VLAN support */
  L7_SNOOPING_FEATURE_VLAN_PRECONFIG,           /* IGMP/MLD Snooping per VLAN config
                                                   on yet to be created VLANs  */

  L7_SNOOPING_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_SNOOPING_FEATURE_IDS_t;

/******************** common constants defined by the component *****************************/

typedef enum
{
  L7_SNOOP_QUERIER_DISABLED = 0, /* Querier Disabled */
  L7_SNOOP_QUERIER_QUERIER,      /* Querier State */
  L7_SNOOP_QUERIER_NON_QUERIER,  /* Non-Querier State */

} L7_SNOOP_QUERIER_STATE_t;

#define L7_IGMP_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL          2
#define L7_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL          3600
#define L7_IGMP_SNOOPING_MIN_RESPONSE_TIME                      1
#define L7_IGMP_SNOOPING_MAX_RESPONSE_TIME                      25
#define L7_IGMP_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME              0
#define L7_IGMP_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME              3600

#define L7_IGMP_SNOOPING_QUERIER_MIN_VERSION                    1
#define L7_IGMP_SNOOPING_QUERIER_MAX_VERSION                    2
#define L7_IGMP_SNOOPING_QUERIER_MIN_QUERY_INTERVAL             1
#define L7_IGMP_SNOOPING_QUERIER_MAX_QUERY_INTERVAL             1800
#define L7_IGMP_SNOOPING_QUERIER_MIN_EXPIRY_INTERVAL            60
#define L7_IGMP_SNOOPING_QUERIER_MAX_EXPIRY_INTERVAL            300

/*-----------------------------------------*/
/*  MLD  Snooping Public Constants         */
/*-----------------------------------------*/
#define L7_MLD_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL          2
#define L7_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL          3600
#define L7_MLD_SNOOPING_MIN_RESPONSE_TIME                      1
#define L7_MLD_SNOOPING_MAX_RESPONSE_TIME                      65
#define L7_MLD_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME              0
#define L7_MLD_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME              3600

#define L7_MLD_SNOOPING_QUERIER_MIN_VERSION                    1
#define L7_MLD_SNOOPING_QUERIER_MAX_VERSION                    1
#define L7_MLD_SNOOPING_QUERIER_MIN_QUERY_INTERVAL             1
#define L7_MLD_SNOOPING_QUERIER_MAX_QUERY_INTERVAL             1800
#define L7_MLD_SNOOPING_QUERIER_MIN_EXPIRY_INTERVAL            60
#define L7_MLD_SNOOPING_QUERIER_MAX_EXPIRY_INTERVAL            300


typedef enum
{
  L7_DHCP_SNOOPING_IPSG = 0,
  L7_DHCP_SNOOPING_DB_PERSISTENCY,
  L7_DHCP_SNOOPING_FEATURE_ID_TOTAL
} L7_DHCP_SNOOPING_FEATURE_IDS_t;

/******************** conditional Override *****************************/

#ifdef INCLUDE_SNOOPING_EXPORTS_OVERRIDES
#include "snooping_exports_overrides.h"
#endif

#endif /* __SNOOPING_EXPORTS_H__ */
