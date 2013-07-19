/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  l7_usl_bcmx_port.h
*
* @purpose   USL port bcmx API's and data-structures
*
* @component USL
*
* @comments
*
* @create    11/06/2008
*
* @author    nshrivastav
*
* @notes     This is the old broad_utils.h file.
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_L7_USL_BCMX_PORT_H
#define INCLUDE_L7_USL_BCMX_PORT_H

#include "l7_common.h"
#include "broad_common.h"
#include "l7_usl_bcm_port.h"


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
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
int custom_bcmx_port_handler(int unit, bcm_port_t port, int setget, int type,
                             int length, uint32 *args, int *actual_length, void *user_data);
#else
int custom_bcmx_port_handler(int unit, bcm_port_t port, int setget, int type,
                             uint32 *args);
#endif

/*********************************************************************
*
* @purpose Set the broadcast rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    bcast_limit    @{(input)} Bcast rate threshold limits
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_rate_bcast_set(bcmx_lport_t port, 
                            usl_bcm_port_rate_limit_t bcast_limit);

/*********************************************************************
*
* @purpose Set the multicast rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    mcast_limit    @{(input)} Mcast rate threshold limits
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_rate_mcast_set(bcmx_lport_t port, 
                            usl_bcm_port_rate_limit_t mcast_limit);


/*********************************************************************
*
* @purpose Set the dlf rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    dlf_limit      @{(input)} Dlf rate threshold limits
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_rate_dlfbc_set(bcmx_lport_t port, 
                            usl_bcm_port_rate_limit_t dlfbc_limit);

/*********************************************************************
*
* @purpose Set the Ingress filtering mode for a port
*
* @param   port        -  The LPORT 
* @param   flterMode   -  Filtering mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_member_set(bcmx_lport_t port, 
                                  usl_bcm_port_filter_mode_t filterMode);


/*********************************************************************
*
* @purpose Set the priority for a port
*
* @param   port       -  The LPORT 
* @param   priority   -  Default priority for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_untagged_priority_set(bcmx_lport_t port, 
                                        usl_bcm_port_priority_t priority);

/*********************************************************************
*
* @purpose Set the max frame size allowed on a port
*
* @param   port           -  The LPORT 
* @param   maxFrameSize   -  Max frame size data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_frame_max_set(bcmx_lport_t port, 
                                usl_bcm_port_frame_size_t max_frame_size);


/*********************************************************************
*
* @purpose Set the learn mode for a port
*
* @param   port           -  The LPORT 
* @param   learnMode      -  Learn mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_learn_set(bcmx_lport_t port, 
                            usl_bcm_port_learn_mode_t learn_mode);

/*********************************************************************
*
* @purpose Set the dtag mode for a port
*
* @param   port           -  The LPORT 
* @param   dtagMode       -  Learn mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_dtag_mode_set(bcmx_lport_t port, 
                                usl_bcm_port_dtag_mode_t dtag_mode);


/*********************************************************************
*
* @purpose Set the tpid for a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  Tpid of the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_tpid_set(bcmx_lport_t port, usl_bcm_port_tpid_t tpid);

/*********************************************************************
*
* @purpose Add a tpid for a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_tpid_add(bcmx_lport_t port, usl_bcm_port_tpid_t tpid);

/*********************************************************************
*
* @purpose Delete a tpid from a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_tpid_delete(bcmx_lport_t port, usl_bcm_port_tpid_t tpid);

/*********************************************************************
*
* @purpose Set the default vid for a port
*
*
* @param    port           @{(input)} Lport 
* @param    vid            @{(input)} Default vid
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_untagged_vlan_set(bcmx_lport_t lport, bcm_vlan_t vid);

/*********************************************************************
*
* @purpose Set the discard mode for a port
*
*
* @param    port           @{(input)} Lport 
* @param    mode           @{(input)} Discard mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_discard_set(bcmx_lport_t port, bcm_port_discard_t mode);

/*********************************************************************
*
* @purpose Set the phy medium config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    medium         @{(input)} Medium of the phy
* @param    config         @{(input)} Medium configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_medium_config_set(bcmx_lport_t port,
                                    bcm_port_medium_t medium,
                                    bcm_phy_config_t  *config);

/*********************************************************************
*
* @purpose Set the flow-control config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    pauseConfig    @{(input)} Pause configuration for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_flow_control_set(bcmx_lport_t port, 
                                   usl_bcm_port_pause_config_t pauseConfig);

/*********************************************************************
*
* @purpose Set the Cos queue Sched config for a port
*
*
* @param    port               @{(input)} Lport 
* @param    cosqSchedConfig    @{(input)} Cosq Sched configuration for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_cosq_sched_set(bcmx_lport_t port, 
                                 usl_bcm_port_cosq_sched_config_t cosqSchedConfig);

/*********************************************************************
*
* @purpose Set the rate shaper config for a port
*
*
* @param    port               @{(input)} Lport 
* @param    shaperConfig       @{(input)} Rate shaper configuration for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_rate_egress_set(bcmx_lport_t port, 
                                  usl_bcm_port_shaper_config_t shaperConfig);

/*********************************************************************
*
* @purpose Add port to vlans 
*
*
* @param    port               @{(input)} Lport 
* @param    vlanConfig         @{(input)}  Vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_member_add(bcmx_lport_t port, 
                                  usl_bcm_port_vlan_t *vlanConfig);

/*********************************************************************
*
* @purpose Remove port from vlans 
*
*
* @param    port               @{(input)} Lport 
* @param    vlanConfig         @{(input)}  Vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_member_delete(bcmx_lport_t port, 
                                     usl_bcm_port_vlan_t *vlanConfig);

/*********************************************************************
*
* @purpose Set the spanning-tree state for a port/stg
*
*
* @param    port               @{(input)} Lport 
* @param    vlanConfig         @{(input)}  Vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_stg_stp_set(bcm_stg_t stg, bcmx_lport_t port, 
                         bcm_stg_stp_t stp_state);

/*********************************************************************
*
* @purpose Associate a vlan with a protocol on a port
*
*
* @param    port               @{(input)} Lport 
* @param    pbvlanConfig       @{(input)} Protocol-based vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_protocol_vlan_add(bcmx_lport_t port, 
                                    usl_bcm_port_pbvlan_config_t pbvlanConfig);

/*********************************************************************
*
* @purpose Dis-associate a vlan with a protocol on a port
*
*
* @param    port               @{(input)} Lport 
* @param    pbvlanConfig       @{(input)} Protocol-based vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_protocol_vlan_delete(bcmx_lport_t port, 
                                       usl_bcm_port_pbvlan_config_t pbvlanConfig);

/*********************************************************************
*
* @purpose Set the dot1x state for the port
*
*
* @param    port               @{(input)} Lport 
* @param    dot1xStatus        @{(input)} Dot1x state for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_dot1x_config(bcmx_lport_t port, 
                               L7_DOT1X_PORT_STATUS_t dot1xStatus);

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
usl_bcmx_port_sfp_diag_get(bcmx_lport_t port,
                           int32 *temperature,
                           uint32 *voltage,
                           uint32 *current,
                           uint32 *txPower,
                           uint32 *rxPower,
                           uint32 *txFault,
                           uint32 *los);

/*********************************************************************
 *
 * @purpose Get copper diagnostics for the specified port.
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
usl_bcmx_port_copper_diag_get(bcmx_lport_t port, bcm_port_cable_diag_t *cd);

/*********************************************************************
*
* @purpose Block an unauthorized dot1x client for the specified port.
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
usl_bcmx_port_dot1x_client_block(bcmx_lport_t port,
                                 usl_bcm_port_dot1x_client_t *client_cmd);

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
usl_bcmx_port_dot1x_client_unblock(bcmx_lport_t port,
                                   usl_bcm_port_dot1x_client_t *client_cmd);

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
usl_bcmx_port_dot1x_client_timeout_get(bcmx_lport_t port,
                                       usl_bcm_port_dot1x_client_t *client_cmd);

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
usl_bcmx_stat_get(bcmx_lport_t port, uint64 stats[snmpValCount]);

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
usl_bcmx_port_stat_get(bcmx_lport_t port, uint64 stats[snmpValCount]);

/*********************************************************************
*
* @purpose Set the mirroring configuration for the port
*
*
* @param    port               @{(input)} Lport 
* @param    mirrorConfig       @{(input)} Mirroring configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_mirror_set(bcmx_lport_t port, 
                             usl_bcm_port_mirror_config_t mirrorConfig);


/*********************************************************************
*
* @purpose Set the admin mode for a port
*
* @param   port    {(input)} 
* @param   enable  {(input)}
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_enable_set(bcmx_lport_t port, int enable);


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
int usl_bcmx_port_wred_set(bcmx_lport_t port,
                           usl_bcm_port_wred_config_t *wredParams);


/*********************************************************************
*
* @purpose Set sflow parameters on a port
*
* @param   port - BCMX Lport
* @param   sflowConfig - Pointer to sflow config
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_sample_rate_set(bcmx_lport_t port,
                                  usl_bcm_port_sflow_config_t *sflowConfig);

/*********************************************************************
*
* @purpose Get sflow parameters on a port
*
* @param   port - BCMX Lport
* @param   sflowConfig - Pointer to sflow config
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_sample_rate_get(bcmx_lport_t port,
                                  usl_bcm_port_sflow_config_t *sflowConfig);

/*********************************************************************
*
* @purpose Configure ingress VLAN translation for a port
*
* @param   port
* @param   enable
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_ingress_enable_set(bcmx_lport_t port, L7_BOOL enable);

/*********************************************************************
*
* @purpose Configure ingress VLAN translation for a port
*
* @param   port
* @param   drop
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_ingress_miss_drop_set(bcmx_lport_t port, L7_BOOL drop);

/*********************************************************************
*
* @purpose Configure egress VLAN translation for a port
*
* @param   port
* @param   enable
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_egress_enable_set(bcmx_lport_t port, L7_BOOL enable);

/*********************************************************************
*
* @purpose Configure egress VLAN translation for a port
*
* @param   port
* @param   drop
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_egress_miss_drop_set(bcmx_lport_t port, L7_BOOL drop);

/*********************************************************************
*
* @purpose Configure VLAN translation key for a port
*
* @param   port
* @param   key
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_key_first_set(bcmx_lport_t port, bcm_vlan_translate_key_t key);

/*********************************************************************
*
* @purpose Configure VLAN translation key for a port
*
* @param   port
* @param   key
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_key_second_set(bcmx_lport_t port, bcm_vlan_translate_key_t key);
/*********************************************************************
*
* @purpose Set the pfc configuration for the port
*
*
* @param    port            @{(input)} Lport 
* @param    pfcConfig       @{(input)} PFC configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_pfc_config_set(bcmx_lport_t port, 
                                usl_bcm_port_pfc_config_t pfcConfig);

/*********************************************************************
*
* @purpose Get the pfc stat for the port
*
*
* @param    port      @{(input)} Lport 
* @param    stat      @{(input)} PFC configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_pfc_stat_get(bcmx_lport_t port, usl_bcm_port_pfc_stat_t *stat);

/*********************************************************************
*
* @purpose Clear the pfc stats for the port
*
*
* @param    port      @{(input)} Lport 
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_pfc_stats_clear(bcmx_lport_t port);

#endif  /* INCLUDE_L7_USL_BCMX_PORT_H */
