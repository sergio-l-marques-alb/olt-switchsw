/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_rpc_port.h
*
* @purpose    RPC BCMX/BCM API's for port 
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

#ifndef L7_RPC_PORT_H
#define L7_RPC_PORT_H

#include "l7_usl_bcm_port.h"


/*********************************************************************
*
* @purpose RPC Client API to set the broadcast rate threshold for a port
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
int l7_rpc_client_rate_bcast_set(bcmx_lport_t port, 
                                 usl_bcm_port_rate_limit_t bcast_limit);


/*********************************************************************
*
* @purpose RPC Client API to set the Multicast rate threshold for a port
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
int l7_rpc_client_rate_mcast_set(bcmx_lport_t port,
                                 usl_bcm_port_rate_limit_t mcast_limit);


/*********************************************************************
*
* @purpose RPC Client API to set the DLF rate threshold for a port
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
int l7_rpc_client_rate_dlfbc_set(bcmx_lport_t port, 
                                 usl_bcm_port_rate_limit_t dlf_limit);


/*********************************************************************
*
* @purpose RPC Client API to set the Ingress filtering mode for a port
*
* @param   port    -  The LPORT 
* @param   mode    -  Filtering mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_member_set(bcmx_lport_t port, 
                                       usl_bcm_port_filter_mode_t mode);


/*********************************************************************
*
* @purpose RPC Client API to Set the default priority for a port
*
* @param   port        -  The LPORT 
* @param   priority    -  Deafult priority for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_untagged_priority_set(bcmx_lport_t port, 
                                             usl_bcm_port_priority_t priority);

/*********************************************************************
*
* @purpose RPC Client API to set the max frame size allowed on a port
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
int l7_rpc_client_port_frame_max_set(bcmx_lport_t port, 
                                     usl_bcm_port_frame_size_t max_frame_size);


/*********************************************************************
*
* @purpose RPC Client API to set the learn mode for a port
*
* @param   port           -  The LPORT 
* @param   learnMode      -  Learn mode of the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_learn_set(bcmx_lport_t port, 
                                 usl_bcm_port_learn_mode_t learn_mode);


/*********************************************************************
*
* @purpose RPC Client API to set the dtag mode for a port
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
int l7_rpc_client_dtag_mode_set(bcmx_lport_t port, 
                                usl_bcm_port_dtag_mode_t dtag_mode);


/*********************************************************************
*
* @purpose RPC Client API to set the tpid for a port
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
int l7_rpc_client_port_tpid_set(bcmx_lport_t port, 
                                usl_bcm_port_tpid_t tpid);

/*********************************************************************
*
* @purpose RPC Client API to add a dtag tpid for a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_tpid_add(bcmx_lport_t port, usl_bcm_port_tpid_t tpid);

/*********************************************************************
*
* @purpose RPC Client API to delete a dtag tpid from a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_tpid_delete(bcmx_lport_t port, usl_bcm_port_tpid_t tpid);

/*********************************************************************
*
* @purpose RPC Client API to set the default vid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} pvid data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_untagged_vlan_set(bcmx_lport_t lport, bcm_vlan_t vid);


/*********************************************************************
*
* @purpose RPC Client API to set the discard mode for a port
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
int l7_rpc_client_port_discard_set(bcmx_lport_t port, 
                                   bcm_port_discard_t mode);


/*********************************************************************
*
* @purpose RPC Client API to set the phy medium config for a port
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
int l7_rpc_client_port_medium_config_set(bcmx_lport_t port,
                                         bcm_port_medium_t medium,
                                         bcm_phy_config_t  *config);


/*********************************************************************
*
* @purpose RPC Client API to set the flow-control config for a port
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
int l7_rpc_client_port_flow_control_set(bcmx_lport_t port, 
                                        usl_bcm_port_pause_config_t pauseConfig);


/*********************************************************************
*
* @purpose RPC Client API to set the Cos queue sched config for a port
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
int l7_rpc_client_port_cosq_sched_set(bcmx_lport_t port, 
                                      usl_bcm_port_cosq_sched_config_t cosqSchedConfig);


/*********************************************************************
*
* @purpose RPC Client API to set the rate shaper config for a port
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
int l7_rpc_client_port_rate_egress_set(bcmx_lport_t port, 
                                       usl_bcm_port_shaper_config_t shaperConfig);


/*********************************************************************
*
* @purpose RPC Client API to add/remove port to/from vlans 
*
*
* @param    port               @{(input)} Lport 
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
int l7_rpc_client_port_vlan_config(bcmx_lport_t port, 
                                   usl_bcm_port_vlan_t *vlanConfig, L7_BOOL cmd);


/*********************************************************************
*
* @purpose Set the spanning-tree state for a port/stg
*
*
* @param    stg                @{(input)}  Stg id
* @param    port               @{(input)} Lport 
* @param    stpState           @{(input)}  State configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int l7_rpc_client_stg_stp_set(bcm_stg_t stg, bcmx_lport_t port, 
                              bcm_stg_stp_t stpState);


/*********************************************************************
*
* @purpose RPC Client API to configure protocol based vlans on a port 
*
*
* @param    port               @{(input)} Lport 
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
int l7_rpc_client_port_protocol_vlan_config(bcmx_lport_t port, 
                                            usl_bcm_port_pbvlan_config_t pbvlanConfig, 
                                            L7_BOOL cmd);


/*********************************************************************
*
* @purpose RPC Client API to set the dot1x state for the port
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
int l7_rpc_client_port_dot1x_config(bcmx_lport_t port, 
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
l7_rpc_client_port_sfp_diag_get(bcmx_lport_t port,
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
l7_rpc_client_port_copper_diag_get(bcmx_lport_t port, bcm_port_cable_diag_t *cd);

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
l7_rpc_client_port_dot1x_client_block(bcmx_lport_t port, 
                                      usl_bcm_port_dot1x_client_t *client_cmd);

/*********************************************************************
*
* @purpose unblock an unauthoriezed dot1x client for the specified port.
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
l7_rpc_client_port_dot1x_client_unblock(bcmx_lport_t port, 
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
l7_rpc_client_port_dot1x_client_timeout_get(bcmx_lport_t port, 
                                            usl_bcm_port_dot1x_client_t *client_cmd);

/*********************************************************************
*
* @purpose RPC Client API to get all statistics for the specified port.
*
* @param   port - BCMX Lport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int
l7_rpc_client_stat_get(bcmx_lport_t port, uint64 stats[snmpValCount]);

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
l7_rpc_client_port_stat_get(bcmx_lport_t port, uint64 stats[snmpValCount]);

/*********************************************************************
*
* @purpose RPC Client API to set the mirroring configuration for the port
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
int l7_rpc_client_port_mirror_set(bcmx_lport_t port, 
                                  usl_bcm_port_mirror_config_t mirrorConfig);


/*********************************************************************
*
* @purpose RPC Client API to set the admin mode for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_enable_set(bcmx_lport_t port, 
                                  int enable);


/*********************************************************************
*
* @purpose RPC Client API to set the admin mode for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_wred_set(bcmx_lport_t port, 
                                usl_bcm_port_wred_config_t *wredParams);


/*********************************************************************
*
* @purpose RPC Client API to set the sflow config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_sflow_config_set(bcmx_lport_t port, 
                                        usl_bcm_port_sflow_config_t *sflowConfig);

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_ingress_enable_set(bcmx_lport_t port, L7_BOOL enable);

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_ingress_miss_drop_set(bcmx_lport_t port, L7_BOOL drop);

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_egress_enable_set(bcmx_lport_t port, L7_BOOL enable);

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_egress_miss_drop_set(bcmx_lport_t port, L7_BOOL drop);

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_key_first_set(bcmx_lport_t port, bcm_vlan_translate_key_t key);

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_key_second_set(bcmx_lport_t port, bcm_vlan_translate_key_t key);

/*********************************************************************
*
* @purpose RPC Client API to set the PFC configuration for the port
*
*
* @param    port               @{(input)} Lport 
* @param    pfcConfig       @{(input)} PFC configuration
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_pfc_config_set(bcmx_lport_t port, 
                                      usl_bcm_port_pfc_config_t pfcConfig);

/*********************************************************************
*
* @purpose RPC Client API to get the PFC stat for the port
*
*
* @param    port       @{(input)} Lport 
* @param    stat       @{(input)} PFC configuration
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_pfc_stat_get(bcmx_lport_t port, 
                                    usl_bcm_port_pfc_stat_t *stat);

/*********************************************************************
*
* @purpose RPC Client API to clear the PFC stats for the port
*
*
* @param    port               @{(input)} Lport 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_pfc_stats_clear(bcmx_lport_t port);

/*********************************************************************
*
* @purpose Handle custom bcmx commands for port configuration 
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} config data
* @param    rv            @{(output)} return value if the command
*                                     was executed
*
*
* @returns L7_TRUE: If the command was handled
*          L7_FALSE: If the command was not handled
*
* @notes 
*
* @end
*
*********************************************************************/

L7_BOOL l7_rpc_server_port_handler(int unit, bcm_port_t port, int setget, 
                                   int type, uint32 *args, int *rv);

#endif
