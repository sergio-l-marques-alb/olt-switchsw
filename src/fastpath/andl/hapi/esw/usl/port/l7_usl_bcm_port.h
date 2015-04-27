/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_bcm_port.h
*
* @purpose    USL BCM Header file for port 
*
* @component  USL
*
* @comments   none
*
* @create     11/07/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_BCM_PORT_H
#define L7_USL_BCM_PORT_H

#include "l7_common.h"
#include "dot1x_exports.h"
#include "l7_usl_common.h"
#include "bcm/stat.h"
#include "bcm/port.h"
#include "bcm/stg.h"
#include "bcm/vlan.h"
#include "bcmx/types.h"

/* USL Port BCM typedefs */

typedef struct 
{
  L7_VLAN_MASK_t   vlan_membership_mask;
  L7_VLAN_MASK_t   vlan_tag_mask;
} usl_bcm_port_vlan_t;

typedef struct
{
 L7_uchar8 mac_addr[6];
 L7_uint32 vlan_id;
 L7_BOOL   timedout_flg;
} usl_bcm_port_dot1x_client_t;

typedef struct
{
  int             bcmUnit;
  int             bcmPort;
  L7_uint32       adminMode;
} usl_bcm_port_admin_mode_t;

typedef struct
{
  L7_BOOL          pauseTx;
  L7_BOOL          pauseRx;
  L7_uchar8        pauseMacAddr[6];
} usl_bcm_port_pause_config_t;

typedef struct
{
  L7_int32 limit;
  L7_int32 flags;
} usl_bcm_port_rate_limit_t;

typedef struct
{
  L7_int32 flags;
  L7_BOOL  setFlags;
} usl_bcm_port_filter_mode_t;

typedef L7_int32 usl_bcm_port_priority_t;

typedef L7_int32 usl_bcm_port_frame_size_t;

typedef L7_uint32 usl_bcm_port_learn_mode_t;

typedef L7_int32 usl_bcm_port_dtag_mode_t;

typedef L7_ushort16 usl_bcm_port_tpid_t;

typedef struct
{
  L7_int32 mode;
  L7_int32 delay;
  L7_int32 weights[BCM_COS_COUNT];
  L7_int32 minKbps[BCM_COS_COUNT];
  L7_int32 maxKbps[BCM_COS_COUNT];
} usl_bcm_port_cosq_sched_config_t;

typedef struct
{
  L7_int32 rate;
  L7_int32 burst;
} usl_bcm_port_shaper_config_t;

typedef struct
{
  L7_int32 ingressSamplingRate;
  L7_int32 egressSamplingRate;
} usl_bcm_port_sflow_config_t;

typedef struct
{
  bcm_port_frametype_t frameType;
  bcm_port_ethertype_t etherType;
  bcm_vlan_t           vid;
} usl_bcm_port_pbvlan_config_t;

typedef struct
{
  L7_uint32     flags;
  bcm_gport_t   probePort;
  L7_uchar8     stackUnit; /* FASTPATH stack unit number of probePort, needed for directed RPC. */
} usl_bcm_port_mirror_config_t;

typedef struct 
{
  bcm_gport_t bcm_gport;
  L7_uchar8   gain[L7_MAX_CFG_QUEUES_PER_PORT];
  L7_uchar8   flags[L7_MAX_CFG_QUEUES_PER_PORT]; /* BCM_COSQ_DISCARD_CAP_AVERAGE only */

  /* The following three are percentages */
  L7_uchar8   minThreshold[L7_MAX_CFG_QUEUES_PER_PORT][L7_MAX_CFG_DROP_PREC_LEVELS+1];
  L7_uchar8   maxThreshold[L7_MAX_CFG_QUEUES_PER_PORT][L7_MAX_CFG_DROP_PREC_LEVELS+1];
  L7_uchar8   dropProb[L7_MAX_CFG_QUEUES_PER_PORT][L7_MAX_CFG_DROP_PREC_LEVELS+1];
} usl_bcm_port_wred_config_t;

typedef struct
{
  L7_int32    mode;
  L7_uchar8   no_drop_priority_bmp;
} usl_bcm_port_pfc_config_t;

typedef struct
{
  L7_uint32 pfc;
  L7_uchar8 pfc_bmp;
  L7_uint32 pause;
  L7_uint32 mtu;
} usl_bcm_port_mmu_parms_t; 

typedef struct
{
  L7_int32 stat_id;
  L7_uint32 ctr;
} usl_bcm_port_pfc_stat_t ;

/* USL BCMX port command */
typedef enum {
  USL_BCMX_PORT_SET_CMD_FIRST = 1,  
  USL_BCMX_POLICY_SET_HANDLER = USL_BCMX_PORT_SET_CMD_FIRST,    /* Policy manager SET commands */
  USL_BCMX_PORT_FLOW_CONTROL_SET, /* Set the flow-control mode for a port */
  USL_BCMX_PORT_ADMIN_MODE_SET, /* Set the admin mode for ports */
  USL_BCMX_DOT1X_CLIENT_BLOCK,     /*Block unauthorized dot1x client*/
  USL_BCMX_DOT1X_CLIENT_UNBLOCK,   /*Unblock unauthorized dot1x client*/
  USL_BCMX_PORT_BCAST_RATE_SET,
  USL_BCMX_PORT_MCAST_RATE_SET,
  USL_BCMX_PORT_DLF_RATE_SET,
  USL_BCMX_PORT_FILTER_MODE_SET,
  USL_BCMX_PORT_PRIORITY_SET,
  USL_BCMX_PORT_MAX_FRAME_SET,
  USL_BCMX_PORT_LEARN_MODE_SET,
  USL_BCMX_PORT_DTAG_MODE_SET,
  USL_BCMX_PORT_TPID_SET,
  USL_BCMX_PORT_TPID_ADD,
  USL_BCMX_PORT_TPID_DELETE,
  USL_BCMX_PORT_PVID_SET,
  USL_BCMX_PORT_DISCARD_SET,
  USL_BCMX_PORT_PHY_CONFIG_SET,
  USL_BCMX_PORT_COSQ_SCHED_CONFIG_SET,
  USL_BCMX_PORT_SHAPER_CONFIG_SET,
  USL_BCMX_PORT_VLAN_CONFIG_SET,
  USL_BCMX_PORT_SFLOW_CONFIG_SET,
  USL_BCMX_PORT_STG_STATE_SET,
  USL_BCMX_PORT_PBVLAN_CONFIG_SET,
  USL_BCMX_PORT_DOT1X_CONFIG_SET,
  USL_BCMX_PORT_MIRROR_CONFIG_SET,
  USL_BCMX_PORT_WRED_CONFIG_SET,
  USL_BCMX_PORT_MCAST_GROUPS_UPDATE,
  USL_BCMX_PORT_VLAN_XLATE_INGRESS_ENABLE_SET,
  USL_BCMX_PORT_VLAN_XLATE_INGRESS_MISS_DROP_SET,
  USL_BCMX_PORT_VLAN_XLATE_EGRESS_ENABLE_SET,
  USL_BCMX_PORT_VLAN_XLATE_EGRESS_MISS_DROP_SET,
  USL_BCMX_PORT_VLAN_XLATE_KEY_FIRST_SET,
  USL_BCMX_PORT_VLAN_XLATE_KEY_SECOND_SET,
  USL_BCMX_PORT_PFC_CONFIG_SET,
  USL_BCMX_PORT_PFC_STAT_GET,
  USL_BCMX_PORT_PFC_STATS_CLEAR,

  /* Always last */
  USL_BCMX_PORT_SET_CMD_LAST

} USL_BCMX_PORT_SET_COMMAND_t;

typedef enum
{
  USL_BCMX_PORT_GET_CMD_FIRST = USL_BCMX_PORT_SET_CMD_LAST + 1,
  USL_BCMX_STATS_GET = USL_BCMX_PORT_GET_CMD_FIRST,  /* Stats Get command */
  USL_BCMX_PORT_STATS_GET,                           /* Port Stats Get command */
  USL_BCMX_DOT1X_CLIENT_TIMEOUT_GET, /*Get dot1x client timeout*/
  USL_BCMX_PORT_SFP_DIAG_GET,
  USL_BCMX_PORT_COPPER_DIAG_GET,
  
  USL_BCMX_PORT_GET_CMD_LAST

} USL_BCMX_PORT_GET_COMMAND_t;

/*********************************************************************
*
* @purpose BCM API to set the broadcast rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    bcast_limit   @{(input)} Bcast rate threshold data
*
* @returns BCMX Error Code
*
* @notes   
*
* @end
*
*********************************************************************/
int usl_bcm_port_rate_bcast_set (int unit, 
                                 bcm_port_t port, 
                                 usl_bcm_port_rate_limit_t *bcast_limit);

/*********************************************************************
*
* @purpose BCM API to set the multicast rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    mcast_limit   @{(input)} Mcast rate threshold data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_rate_mcast_set (int unit, 
                                 bcm_port_t port, 
                                 usl_bcm_port_rate_limit_t *mcast_limit);

/*********************************************************************
*
* @purpose BCM API to set the dlf rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    dlf_limit     @{(input)} Dlf rate threshold data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_rate_dlfbc_set (int unit, 
                                 bcm_port_t port, 
                                 usl_bcm_port_rate_limit_t *dlf_limit);

/*********************************************************************
*
* @purpose BCM API to set the filtering mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    mode          @{(input)} Filter mode data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_member_set (int unit, 
                                  bcm_port_t port, 
                                  usl_bcm_port_filter_mode_t *mode);

/*********************************************************************
*
* @purpose BCM API to set the priority for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_untagged_priority_set (int unit, 
                                        bcm_port_t port, 
                                        usl_bcm_port_priority_t  *priority);

/*********************************************************************
*
* @purpose BCM API to set the max frame size for a port
*
*
* @param    unit            @{(input)} Local bcm unit number
* @param    port            @{(input)} Local bcm port number 
* @param    max_frame_size  @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_frame_max_set(int unit, 
                               bcm_port_t port, 
                               usl_bcm_port_frame_size_t *max_frame_size);

/*********************************************************************
*
* @purpose BCM API to set the learn mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    learn_mode    @{(input)} Learn mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_learn_set(int unit, 
                           bcm_port_t port, 
                           usl_bcm_port_learn_mode_t *learn_mode);

/*********************************************************************
*
* @purpose BCM API to set the dtag mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    dtag_mode     @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_dtag_mode_set(int unit, 
                               bcm_port_t port, 
                               usl_bcm_port_dtag_mode_t *dtag_mode);

/*********************************************************************
*
* @purpose BCM API to set the tpid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    tpid          @{(input)} Tpid data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_tpid_set(int unit, 
                          bcm_port_t port, 
                          usl_bcm_port_tpid_t *tpid);

/*********************************************************************
*
* @purpose BCM API to add a tpid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    tpid          @{(input)} Tpid data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_tpid_add(int unit, 
                          bcm_port_t port, 
                          usl_bcm_port_tpid_t *tpid);

/*********************************************************************
*
* @purpose BCM API to delete a tpid from a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    tpid          @{(input)} Tpid data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_tpid_delete(int unit, 
                             bcm_port_t port, 
                             usl_bcm_port_tpid_t *tpid);

/*********************************************************************
*
* @purpose BCM API to set the default vid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    pvid          @{(input)} pvid data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_untagged_vlan_set(int unit, 
                                   bcm_port_t port, 
                                   bcm_vlan_t *pvid);

/*********************************************************************
*
* @purpose BCM API to set the discard mode for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    mode          @{(input)} discard mode data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_discard_set(int unit, 
                             bcm_port_t port, 
                             bcm_port_discard_t *mode);

/*********************************************************************
*
* @purpose BCM API to set the phy medium config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    medium        @{(input)} phy medium data
* @param    config        @{(input)} phy config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_medium_config_set(int unit, 
                                   bcm_port_t port, 
                                   bcm_port_medium_t medium,
                                   bcm_phy_config_t *config);


/*********************************************************************
*
* @purpose BCM API to set the flow control settings for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} flow-control data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_flow_control_set(int unit, 
                                  bcm_port_t port, 
                                  usl_bcm_port_pause_config_t  *pauseConfig);

/*********************************************************************
*
* @purpose BCM API to set the cosq sched config for a port
*
* @param    unit              @{(input)} Local bcm unit number
* @param    port              @{(input)} Local bcm port number 
* @param    cosqSchedConfig   @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_cosq_sched_set(int unit, 
                                bcm_port_t port, 
                                usl_bcm_port_cosq_sched_config_t  *cosqSchedConfig);

/*********************************************************************
*
* @purpose BCM API to set the rate shaper config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_rate_egress_set(int unit, 
                                 bcm_port_t port, 
                                 usl_bcm_port_shaper_config_t *shaperConfig);

/*********************************************************************
*
* @purpose BCM API to add/remove port to/from vlans 
*
* @param    unit               @{(input)} unit 
* @param    port               @{(input)} port 
* @param    vlanConfig         @{(input)} Vlan configuration
* @param    cmd                @{(input)} L7_TRUE: Add ports to vlan
*                                         L7_FALSE: Remove ports from vlan
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_config(int unit, 
                             bcm_port_t port,
                             usl_bcm_port_vlan_t *vlanConfig,
                             L7_BOOL cmd);

/*********************************************************************
*
* @purpose BCM API to set the Stg stp state config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    stg           @{(input)} stg-id
* @param    stpState      @{(input)} stpState
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_stg_stp_set(int unit, 
                        bcm_port_t port, 
                        bcm_stg_t stg,
                        bcm_stg_stp_t stpState);
     
/*********************************************************************
*
* @purpose BCM API to protocol based vlan config for a port
*
* @param    unit               @{(input)} Local bcm unit number
* @param    port               @{(input)} Local bcm port number 
* @param    pbvlanConfig       @{(input)} protocol Vlan configuration
* @param    cmd                @{(input)} L7_TRUE: Add pbvlan config
*                                         L7_FALSE: Remove pbvlan config
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_protocol_vlan_config(int unit, 
                                      bcm_port_t port,
                                      usl_bcm_port_pbvlan_config_t *pbvlanConfig,
                                      L7_BOOL cmd);

/*********************************************************************
*
* @purpose BCM API to to set the dot1x state for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    dot1xConfig   @{(input)} dot1x config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_dot1x_config(int unit, 
                              bcm_port_t port, 
                              L7_DOT1X_PORT_STATUS_t dot1xState);

/*********************************************************************
*
* @purpose dot1x client block Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   This is the custom handler that runs on the slave devices.
*
*
* @end
*
*********************************************************************/
int usl_bcm_port_dot1x_client_block(int unit, 
                                    bcm_port_t port, 
                                    usl_bcm_port_dot1x_client_t *pClientCmd);

/*********************************************************************
*
* @purpose dot1x client unblock Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   This is the custom handler that runs on the slave devices.
*
*
* @end
*
*********************************************************************/
int usl_bcm_port_dot1x_client_unblock(int unit, 
                                      bcm_port_t port, 
                                      usl_bcm_port_dot1x_client_t *pClientCmd);

/*********************************************************************
*
* @purpose BCM API to to set the mirror configuration for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_mirror_set(int unit, 
                            bcm_port_t port,
                            usl_bcm_port_mirror_config_t *mirrorConfig);


/*********************************************************************
*
* @purpose BCM API to to set the admin mode configuration for the port
*
* @param    unit     @{(input)} Local bcm unit number
* @param    port     @{(input)} Local bcm port number 
* @param    enable   @{(input)} enable config
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_enable_set(int unit, bcm_port_t port, int enable);

/*********************************************************************
*
* @purpose Set WRED parameters on a port
*
* @param   unit - the local unit
* @param   port - BCMX Lport
* @param   setget - unused
* @param   args - Pointer to WRED params for all queues/colors
*
* @returns BCM Error Code
*
* @comments Companion to customx_port_wred_set(), this function runs 
*           on each unit.
* @end
*
*********************************************************************/
int usl_bcm_port_wred_set(int unit, bcm_port_t port, 
                          usl_bcm_port_wred_config_t *wredParams);

/*********************************************************************
*
* @purpose BCM API to set a port in a vlan
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    cmd           @{(input)} Add or remove the port from vlan
* @param    vlan-id       @{(input)} Vlan id 
* @param    tagged        @{(input)} Port is tagged or not
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_vlan_port_update(int unit, 
                             bcm_port_t port, 
                             USL_CMD_t cmd,
                             bcm_vlan_t vid,
                             L7_BOOL tagged);

/*********************************************************************
*
* @purpose BCM API to to set the sflow configuration for the port
*
* @param    unit         @{(input)} Local bcm unit number
* @param    port         @{(input)} Local bcm port number 
* @param    sflowConfig  @{(input)} sflow config
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_sflow_config_set(int unit, bcm_port_t port, 
                                  usl_bcm_port_sflow_config_t *sflowConfig);

/*********************************************************************
*
* @purpose Configure ingress VLAN translation for a port
*
* @param   unit
* @param   port
* @param   enable
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_ingress_enable_set(int unit, bcm_port_t port, L7_BOOL enable);

/*********************************************************************
*
* @purpose Configure ingress VLAN translation for a port
*
* @param   unit
* @param   port
* @param   drop
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_ingress_miss_drop_set(int unit, bcm_port_t port, L7_BOOL drop);

/*********************************************************************
*
* @purpose Configure egress VLAN translation for a port
*
* @param   unit
* @param   port
* @param   enable
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_egress_enable_set(int unit, bcm_port_t port, L7_BOOL enable);

/*********************************************************************
*
* @purpose Configure egress VLAN translation for a port
*
* @param   unit
* @param   port
* @param   drop
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_egress_miss_drop_set(int unit, bcm_port_t port, L7_BOOL drop);

/*********************************************************************
*
* @purpose Configure VLAN translation key for a port
*
* @param   unit
* @param   port
* @param   key
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_key_first_set(int unit, bcm_port_t port, bcm_vlan_translate_key_t key);

/*********************************************************************
*
* @purpose Configure VLAN translation key for a port
*
* @param   unit
* @param   port
* @param   key
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_key_second_set(int unit, bcm_port_t port, bcm_vlan_translate_key_t key);

/*********************************************************************
*
* @purpose BCM API to to set the PFC configuration for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    pfcConfig     @{(input)} pfc parameters
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_pfc_config_set(int unit, 
                                bcm_port_t port,
                                usl_bcm_port_pfc_config_t *pfcConfig );

/*********************************************************************
*
* @purpose BCM API to to get the PFC stat for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    stat          @{(inout)} The id to get and ctr to return
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_pfc_stat_get(int unit, 
                              bcm_port_t port,
                              usl_bcm_port_pfc_stat_t *stat );

/*********************************************************************
*
* @purpose BCM API to to clear the PFC stats for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_pfc_stats_clear(int unit, bcm_port_t port);
#endif
