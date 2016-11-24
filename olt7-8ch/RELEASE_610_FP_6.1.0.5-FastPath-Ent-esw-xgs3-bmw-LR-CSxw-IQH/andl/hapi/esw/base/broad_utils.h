/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_utils.h
*
* @purpose   Common Utilities to help with developing the ANDL Module
*
* @component hapi
*
* @comments
*
* @create    12/19/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_UTILS_H
#define INCLUDE_BROAD_UTILS_H

#include <l7_common.h>
#include "broad_common.h"
#include "bcm/stat.h"
#include "bcm/custom.h"

typedef enum {
  HAPI_BROAD_BCMX_PORT_STATS_GET = 1,  
/*HAPI_BROAD_BCMX_PORT_DOT1P_SET = 2, */      /* obsoleted */
  HAPI_BROAD_BCMX_PORT_TO_VLANS_ADD = 3,      /* Add port to group of VLANs */
  HAPI_BROAD_BCMX_PORT_FROM_VLANS_DELETE = 4, /* Remove port from group of VLANs */
  HAPI_BROAD_BCMX_POLICY_SET_HANDLER = 5,     /* Policy manager SET commands */
  HAPI_BROAD_BCMX_POLICY_GET_HANDLER = 6,     /* Policy manager GET commands */
  HAPI_BROAD_BCMX_IPMC_L2_PORT_ADD = 7,   /* Add L2 ports to a multicast group */
  HAPI_BROAD_BCMX_IPMC_L2_PORT_DELETE = 8,  /* Delete L2 ports from a multicast group */
  HAPI_BROAD_BCMX_IPMC_L3_PORT_ADD = 9,   /* Add L3 ports to a multicast group */
  HAPI_BROAD_BCMX_IPMC_L3_PORT_DELETE = 10, /* Delete L3 ports from a multicast group */
  HAPI_BROAD_BCMX_IPMC_L2_PORT_SET = 11,   /* Set L2 ports for a multicast group */
  HAPI_BROAD_BCMX_DOT1X_CLIENT_TIMEOUT_GET = 12, /*Get dot1x client timeout*/
  HAPI_BROAD_BCMX_PORT_FLOW_CONTROL_SET = 13, /* Set the flow-control mode for a port */
  HAPI_BROAD_BCMX_PORT_ADMIN_MODE_SET = 14, /* Set the admin mode for ports */
  HAPI_BROAD_BCMX_DOT1X_CLIENT_BLOCK = 15,     /*Block unauthorized dot1x client*/
  HAPI_BROAD_BCMX_DOT1X_CLIENT_UNBLOCK = 16,   /*Unblock unauthorized dot1x client*/
  HAPI_BROAD_BCMX_PORT_DIAGNOSTIC_GET = 17,
  HAPI_BROAD_BCMX_PORT_WRED_SET = 18,       /* Set WRED parms for all queues/colors of a port */
  HAPI_BROAD_BCMX_PORT_CUSTOM_STATS_GET = 19,  
  HAPI_BROAD_BCMX_PORT_DOT1AD_RULE_ADD = 20, /* Handler for DOT1AD VFP rule */
  HAPI_BROAD_BCMX_PORT_DOT1AD_RULE_DELETE = 21, /* Handler for DOT1AD VFP rule */
  HAPI_BROAD_BCMX_MULITCAST_JOIN_GROUPS = 22,
  HAPI_BROAD_BCMX_MULITCAST_LEAVE_GROUPS = 23,

} HAPI_BROAD_CUSTOM_BCMX_COMMAND_t;

typedef struct 
{
  L7_VLAN_MASK_t   vlan_membership_mask;
  L7_VLAN_MASK_t   vlan_tag_mask;
} HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t;

/* This macro sets a bit corresponding to the VLAN in the VLAN membership
** mask. 
*/
#define BROAD_BCMX_CUSTOM_VLAN_MEMBER_SET(vid,mask) \
            ((mask).value[(vid - 1) / 8] |=  \
              (1 << ((vid - 1) % 8) ))

/* This macro clears a bit corresponding to the VLAN in the VLAN membership
** mask. 
*/
#define BROAD_BCMX_CUSTOM_VLAN_MEMBER_CLEAR(vid,mask) \
            ((mask).value[(vid - 1) / 8] &=  \
              (~(1 << ((vid - 1) % 8) )))

/* This macro returns a non-zero value if the port is a member of the specified VLAN.
*/
#define BROAD_BCMX_CUSTOM_IS_VLAN_MEMBER(vid,mask) \
            ((mask).value[(vid - 1) / 8] & \
              (1 << ((vid - 1) % 8) ))

typedef struct
{
  L7_uchar8 ipmc_index_mask [(L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL / 8) + 1];
  L7_uchar8 ipmc_index;
  L7_uint32 vlan_id;
  L7_BOOL   tagged;
  L7_uchar8 mac[6];
  L7_uint32 ttl;
  bcm_pbmp_t pbmp; /* L2 port bitmap */
  bcm_pbmp_t ubmp; /* L2 port bitmap */
} HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t;

/* This macro sets a bit corresponding to the IPMC index in the membership
** mask. 
*/
#define BROAD_BCMX_CUSTOM_IPMC_INDEX_MEMBER_SET(ipmc_index,mask) \
            (mask[(ipmc_index) / 8] |=  \
              (1 << ((ipmc_index) % 8) ))

/* This macro clears a bit corresponding to the IPMC index in the membership
** mask. 
*/
#define BROAD_BCMX_CUSTOM_IPMC_INDEX_MEMBER_CLEAR(ipmc_index,mask) \
            (mask[(ipmc_index) / 8] &=  \
              (~(1 << ((ipmc_index) % 8) )))

/* This macro returns a non-zero value if the IPMC index is a member of the specified mask.
*/
#define BROAD_BCMX_CUSTOM_IS_IPMC_INDEX_MEMBER(ipmc_index,mask) \
            (mask[(ipmc_index) / 8] & \
              (1 << ((ipmc_index) % 8) ))

typedef struct
{
 L7_uchar8 mac_addr[6];
 L7_uint32 vlan_id;
 L7_BOOL   timedout_flg;
}HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t;


/* Customx cmd for flow-control mode */
typedef struct
{
  bcm_port_pause_t pauseMode;
  L7_BOOL          pauseTx;
  L7_BOOL          pauseRx;
  L7_uchar8        pauseMacAddr[6];
} HAPI_BROAD_CUSTOM_BCMX_PAUSE_CMD_t;

/* Customx cmd for admin mode */
typedef struct
{
  int             bcmUnit;
  int             bcmPort;
  L7_uint32       adminMode;
} HAPI_BROAD_CUSTOM_BCMX_PORT_ADMIN_MODE_t;

typedef struct 
{
  bcm_gport_t bcm_gport;
  L7_uchar8 gain[L7_MAX_CFG_QUEUES_PER_PORT];
  L7_uchar8 flags[L7_MAX_CFG_QUEUES_PER_PORT]; /* BCM_COSQ_DISCARD_CAP_AVERAGE only */
  /* The following three are percentages */
  L7_uchar8 minThreshold[L7_MAX_CFG_QUEUES_PER_PORT][L7_MAX_CFG_DROP_PREC_LEVELS+1];
  L7_uchar8 maxThreshold[L7_MAX_CFG_QUEUES_PER_PORT][L7_MAX_CFG_DROP_PREC_LEVELS+1];
  L7_uchar8 dropProb[L7_MAX_CFG_QUEUES_PER_PORT][L7_MAX_CFG_DROP_PREC_LEVELS+1];
} HAPI_BROAD_CUSTOM_BCMX_PORT_WRED_SET_t;

/*********************************************************************
*
* @purpose Dispatch function to handle custom BCMX port requests.
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*
*
* @end
*
*********************************************************************/
int custom_bcmx_port_handler(int unit, bcm_port_t port, int setget, int type,
            uint32 *args);

/*********************************************************************
*
* @purpose Get all statistics for the specified port.
*
* @param   port - BCMX Lport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_stat_get(bcmx_lport_t port,
                 uint64 stats[snmpValCount]);

/*********************************************************************
*
* @purpose Get all statistics for the specified gport.
*
* @param   port - BCMX Lport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_port_stat_get(bcmx_lport_t port,
                 uint64 stats[snmpValCount]);



/*********************************************************************
*
* @purpose Add port to VLANs specified in the member list.
*
* @param   port - BCMX Lport
* @param   vlan_cmd - VLAN and Tagging mask.
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int
customx_port_vlan_member_add(bcmx_lport_t port,
                                HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t * vlan_cmd);

/*********************************************************************
*
* @purpose Remove port from VLANs specified in the member list.
*
* @param   port - BCMX Lport
* @param   vlan_cmd - VLAN and Tagging mask.
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int
customx_port_vlan_member_delete(bcmx_lport_t port,
                                HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t * vlan_cmd);

/*********************************************************************
*
* @purpose Get dot1x client timeout for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_dot1x_client_timeout_get(bcmx_lport_t port,
                 HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *client_cmd);

/*********************************************************************
*
* @purpose Block an unauthoriezed dot1x client for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_dot1x_client_block(bcmx_lport_t port,
                 HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *client_cmd);

/*********************************************************************
*
* @purpose Unblock an unauthorized dot1x client for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_dot1x_client_unblock(bcmx_lport_t port,
                             HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *client_cmd);

/*********************************************************************
*
* @purpose Sets the L2 ports for a multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l2_port_set(bcmx_lport_t port,
                         L7_uint32    ipmc_index,
                         bcm_pbmp_t   pbmp,
                         bcm_pbmp_t   ubmp,
                         L7_uint32    vlan_id);

/*********************************************************************
*
* @purpose Add L2 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l2_port_add (bcmx_lport_t port,
                          L7_uint32    *ipmc_index,
                          L7_uint32    num_groups,
                          L7_uint32    vlan_id,
                          L7_uint32    tagged);

/*********************************************************************
*
* @purpose Delete L2 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l2_port_delete (bcmx_lport_t port,
                             L7_uint32    *ipmc_index,
                             L7_uint32    num_groups,
                             L7_uint32    vlan_id);

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l3_port_add (bcmx_lport_t port,
                          L7_uint32    *ipmc_index,
                          L7_uint32    num_groups,
                          L7_uint32    vlan_id,
                          L7_uint32    tagged,
                          L7_uchar8    *mac,
                          L7_uint32    ttl);

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l3_port_delete (bcmx_lport_t port,
                          L7_uint32    *ipmc_index,
                          L7_uint32    num_groups,
                          L7_uint32    vlan_id);

/*********************************************************************
*
* @purpose Get SFP diagnostics for the specified port.
*
* @param   port - BCMX Lport
* @param
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_diagnostic_get(bcmx_lport_t port,
                       int32 *temperature,
                       uint32 *voltage,
                       uint32 *current,
                       uint32 *txPower,
                       uint32 *rxPower,
                       uint32 *txFault,
                       uint32 *los);

/*********************************************************************
*
* @purpose Enable/Disable flow control on a port
*
* @param   port - BCMX Lport
* @param   mode - Flow control mode
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int customx_port_flow_control_set(bcmx_lport_t port,
                                  HAPI_BROAD_CUSTOM_BCMX_PAUSE_CMD_t pauseCmd);

/*********************************************************************
*
* @purpose Set the flow control mode for a port
*
* @param   
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int custom_port_flow_control_set(int unit, bcm_port_t port, int setget, uint32 *args);

/*********************************************************************
*
* @purpose Set WRED parameters on a port
*
* @param   port - BCMX Lport
* @param   wredParams - Pointer to WRED params for all queues/colors
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int customx_port_wred_set(bcmx_lport_t port,
                                  HAPI_BROAD_CUSTOM_BCMX_PORT_WRED_SET_t *wredParams);


/*********************************************************************
*
* @purpose Add the given port to the list of multicast groups
*
* @param   port - BCMX gport
* @param   groups - Pointer to list of multicast groups
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int customx_port_multicast_join_groups(bcm_gport_t port,
                                       int groups[BCM_CUSTOM_ARGS_MAX]);

/*********************************************************************
*
* @purpose Delete the given port from the list of multicast groups
*
* @param   port - BCMX gport
* @param   groups - Pointer to list of multicast groups
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int customx_port_multicast_leave_groups(bcm_gport_t port,
                                        int groups[BCM_CUSTOM_ARGS_MAX]);
#endif  /* INCLUDE_BROAD_UTILS_H */
