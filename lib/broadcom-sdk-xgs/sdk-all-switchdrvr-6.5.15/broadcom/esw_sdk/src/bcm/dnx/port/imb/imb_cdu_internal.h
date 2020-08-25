/** \file imb_cdu_internal.h
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef IMB_CDU_INTERNAL_H_INCLUDED
/*
 * {
 */
#define IMB_CDU_INTERNAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <bcm/port.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
 /*
  * CDU reset logic: in-reset = 1, out-of-reset = 0 
  */
#define IMB_CDU_IN_RESET                        (1)
#define IMB_CDU_OUT_OF_RESET                    (0)
#define IMB_CDU_ALL_LANES                       (-1)
#define IMB_CDU_ALL_RMCS                        (-1)
#define IMB_CDU_RMC_INVALID                     (-1)

#define IMB_CDU_PRD_TM_KEY_TC_GET(tm_key, tc)          (tc = (tm_key >> _SHR_PORT_PRD_TM_DP_KEY_SIZE) & _SHR_PORT_PRD_TM_TC_KEY_MASK)
#define IMB_CDU_PRD_TM_KEY_DP_GET(tm_key, dp)          (dp = tm_key & _SHR_PORT_PRD_TM_DP_KEY_MASK)
#define IMB_CDU_PRD_IP_KEY_DSCP_GET(ip_key, dscp)      (dscp = ip_key & _SHR_PORT_PRD_IP_DSCP_KEY_MASK)
#define IMB_CDU_PRD_MPLS_KEY_EXP_GET(mpls_key, exp)    (exp = mpls_key & _SHR_PORT_PRD_MPLS_EXP_KEY_MASK)
#define IMB_CDU_PRD_ETH_KEY_PCP_GET(eth_key, pcp)      (pcp = (eth_key >> _SHR_PORT_PRD_ETH_DEI_KEY_SIZE) & _SHR_PORT_PRD_ETH_PCP_KEY_MASK)
#define IMB_CDU_PRD_ETH_KEY_DEI_GET(eth_key, dei)      (dei = eth_key & _SHR_PORT_PRD_ETH_DEI_KEY_MASK)

#define IMB_CDU_PRD_TCAM_ENTRY_OFFSET_GET(offset_array, offset_index, offset) \
            SHR_BITCOPY_RANGE(&offset, 0, &offset_array, offset_index * 8, 8)

#define IMB_CDU_PRD_TCAM_ENTRY_OFFSET_SET(offset, offset_index, offset_array) \
            SHR_BITCOPY_RANGE(&offset_array, offset_index * 8, &offset, 0, 8)

 /*
  * }
  */

/*
 * Structs
 * {
 */
/*The struct fields match the fields in DBAL table NIF_PRD_CDU_TCAM.*/
typedef struct imb_cdu_internal_prd_tcam_entry_info_s
{
    uint32 ether_code_val;      /* ether type code value */
    uint32 ether_code_mask;     /* ether value mask (1->valid, 0->dont care) */
    uint32 offset_array_val;    /* offset array - all 4 offsets result fields concatenated in a single uint32 */
    uint32 offset_array_mask;   /* offset array mask (1->valid, 0->dont care) */
    uint32 priority;            /* priority for the tcam entry */
    uint32 is_entry_valid;      /* is entry valid to compare */
} imb_cdu_internal_prd_tcam_entry_info_t;
 /*
  * }
  */

/*
 * Functions
 * {
 */
 /**
 * \brief - get port CDU active indication in the specified 
 *        scheduler.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] sch_prio - Scheduler to configure. see 
 *        bcm_port_nif_scheduler_t
 * \param [out] active - active indication. if the CDU has any 
 *        BW in the scheduler returns true, otherwise returns
 *        false.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_scheduler_active_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 *active);

/**
 * \brief - Set the port's CDU as active in the specified 
 *        scheduler
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] sch_prio - scheduler. see 
 *        bcm_port_nif_scheduler_t
 * \param [in] is_active - active indication to set.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * the active indication will be translated to the
 *   bandwidth each scheduler will get for the CDU as
 *   follows:
 *   * TDM/High scheduler - there are 2 bits per CDU, but
 *   whenever the CDU is active in the scheduler both bits will
 *   be set.
 *   * Low scheduler - there are 4 bits per CDU. the weight will
 *   be determined according to CDU bandwidth - for every 100G
 *   the CDU is carrying, one bit will be added in the
 *   scheduler. (when the CDU BW is less then 100G, there will
 *   be one bit set in the scheduler)
 * \see
 *   * None
 */
int imb_cdu_port_scheduler_active_set(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 is_active);

/**
 * \brief - set the requested weight in the low priority RMC 
 *        scheduler. this schduler is performing RR on all RMCs
 *        with traffic, and each rMC has more than one bit in
 *        the RR. the weight is actually the number of bits in
 *        the RR.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc id to set the weight to
 * \param [in] weight - weight value
 *   
 * \return
 *   int - see _SHR_E_ *
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_rmc_low_prio_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int weight);

/**
 * \brief - Configure the RMC priority (Low, High, TDM)
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc id to configure
 * \param [in] sch_prio - scheduler. see
 *        bcm_port_nif_scheduler_t
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_cdu_port_rmc_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    bcm_port_nif_scheduler_t sch_prio);

/**
 * \brief - Enable / disable the MAC in the CDU. 
 * this method should be called only when loading the first port
 * on the MAC, or removing the last port on the MAC. 
 * when MAC is enabled, the MAC mode will be set according to 
 * mac_mode, when MAC is disabled, mac_mode argument will be 
 * ignored, and MAC mode will be set to difault reg value. 
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] mac_mode - MAC operation mode. see 
 *        DBAL_FIELD_MAC_MODE
 * \param [in] enable - enable indication
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_mac_enable_set(
    int unit,
    bcm_port_t port,
    int mac_mode,
    int enable);

/**
 * \brief - Reset the Async unit in the CDU
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] in_reset - reset indication. 
 * in_reset = 1 --> put the async unit in reset 
 * in_reset = 0 --> take the async unit out of reset 
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_async_unit_reset(
    int unit,
    bcm_port_t port,
    int in_reset);

/**
 * \brief - Reset the port in the NMG
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port 
 * \param [in] rmc_id - logical fifo. if -1 means set all RMCs 
 *        of the port. 
 * \param [in] in_reset - reset indication. 
 * in_reset = 1 --> put in reset 
 * in_reset = 0 --> take out of reset 
 *   
 * \return
 *   int - see _SHR_E_ *
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_rx_nmg_reset(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset);

/**
 * \brief - map RMC (logical FIFO) to a physical lane in the CDU
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - logical fifo info. 
 * \param [in] is_map_valid - indication whether to map /unmap 
 *        the rmc to lane. this value should be 0 when the port
 *        is on the process of being removed.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_rmc_to_lane_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid);

/**
 * \brief - map physical lane in the CDU + PRD priority to a 
 *        specific RMC (logical FIFO)
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - logical fifo info
 * \param [in] is_map_valid - map valid indication: TRUE => map, 
 *        FALSE=> unmap
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_lane_to_rmc_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid);

/**
 * \brief - Set the logical FIFO threshold after overflow. 
 * after the fifo reaches overflow, it will not resume writing 
 * until fifo level will get below this value. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - logical fifo info
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_thr_after_ovf_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc);

/**
 * \brief - Reset the PM for the entire CDU.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port. only used to retrieve CDU id
 * \param [in] in_reset - reset indication. 
 * in_reset = TRUE -> put in reset 
 * in_reset = FALSE -> take out of reset 
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_pm_reset(
    int unit,
    bcm_port_t port,
    int in_reset);

/**
 * \brief - Enable / disable Tx data to the PM
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indocation
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_tx_data_to_pm_enable_set(
    int unit,
    bcm_port_t port,
    int enable);

int imb_cdu_tx_data_to_pm_enable_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int lane,
    int enable);

/**
 * \brief - Enable / disable egress flush. Egress flush means 
 *        continously geanting credits to the port in order to
 *        free occupied resources.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] flush_enable - enable flush indication
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_tx_egress_flush_set(
    int unit,
    bcm_port_t port,
    int flush_enable);

int imb_cdu_tx_egress_flush_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int first_lane,
    int flush_enable);

/**
 * \brief - Reset the port TX in the CDU.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] in_reset - reset indication. 
 * in_reset = TRUE  --> put in reset 
 * in_reset = FALSE --> take out of reset 
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_tx_reset(
    int unit,
    bcm_port_t port,
    int in_reset);

/**
 * \brief - Reset the port TX in the NMG
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port 
 * \param [in] in_reset - reset indication. 
 * in_reset = 1 --> put in reset 
 * in_reset = 0 --> take out of reset 
 *   
 * \return
 *   int - see _SHR_E_ *
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_tx_nmg_reset(
    int unit,
    bcm_port_t port,
    int in_reset);

/**
 * \brief - Reset the port RX in the CDU.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port 
 * \param [in] rmc_id - logical fifo. if -1 means set all RMCs 
 *        of the port.
 * \param [in] in_reset - reset indication. 
 * in_reset = TRUE  --> put in reset 
 * in_reset = FALSE --> take out of reset 
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_rx_reset(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset);

/**
 * \brief - get all master logical ports on a CDU
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] cdu_ports - bitmap of master ports on the CDU
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * cdu_ports);

/**
 * \brief - get all ilkn master logical ports on a CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] cdu_ilkn_ports - bitmap of master ports on the CDU
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_ilkn_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * cdu_ilkn_ports);

/**
 * \brief - set TX start threshold. this threshold represent the 
 *        number of 64B words to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 * 
 * \param [in] unit - chip unit id
 * \param [in] core - device core 
 * \param [in] cdu_id - cdu index iside the core
 * \param [in] lane - lane inside the CDU
 * \param [in] start_thr - threshold value
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_tx_start_thr_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int lane,
    uint32 start_thr);

/**
 * \brief - set TX start threshold. this threshold represent the 
 *        number of 64B words to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 * 
 * \param [in] unit - chip unit id
 * \param [in] core - device core 
 * \param [in] cdu_id - cdu index iside the core
 * \param [in] lane - lane inside the CDU
 * \param [out] start_thr - threshold value
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_tx_start_thr_get(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int lane,
    uint32 *start_thr);

/**
 * \brief - Enable EEE for the CDU
 * 
 * \param [in] unit - chip unit id
 * \param [in] core - device core 
 * \param [in] cdu_id - cdu index inside the core 
 * \param [in] enable - enable indication
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_eee_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int enable);

/**
 * \brief - Configure the CDU in the specified Scheduler
 * 
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] sch_prio - scheduler to configure. see 
 *        bcm_port_nif_scheduler_t
 * \param [in] weight - each scheduler has more than one bit per 
 *        CDU. each bit translates to BW in the scheduler.
 *        weight specifies how many bits the CDU should have in
 *        the specified scheduler.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * High/TDM prio schedulers have 2 bits per CDU. (valid
 *     values are 0-2)
 *   * Low prio scheduler has 4 bits per CDU. (valid values are
 *     0-4)
 * \see
 *   * None
 */
int imb_cdu_scheduler_config_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    bcm_port_nif_scheduler_t sch_prio,
    int weight);

/**
 * \brief - Get CDU configuration in specified scheduler
 * 
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] sch_prio - scheduler to configure. see 
 *        bcm_port_nif_scheduler_t
 * \param [out] weight - each scheduler has more than one bit 
 *        per CDU. each bit translates to BW in the scheduler.
 *        weight specifies how many bits the CDU should have in
 *        the specified scheduler.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_scheduler_config_get(
    int unit,
    bcm_core_t core,
    int cdu_id,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 *weight);

/**
 * \brief - Set the logical FIFO threshold after overflow. 
 * after the fifo reaches overflow, it will not resume writing 
 * until fifo level will get below this value. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - CDU id inside the core
 * \param [in] rmc_id - logical fifo id
 * \param [in] thr_after_ovf - value to set as threshold after 
 *        overflow
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_rmc_thr_after_ovf_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int rmc_id,
    int thr_after_ovf);

/**
 * \brief - get the RMC current FIFO level
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc id to check fifo level
 * \param [out] fifo_level - RMC FIFO fullnes level 
 *  
 * \return
 *   int - see _SHR_E_* 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_rmc_level_get(
    int unit,
    bcm_port_t port,
    uint32 rmc_id,
    uint32 *fifo_level);

/**
 * \brief - set PRD Parser wether to ignore indication of IP 
 *        dscp even if packet is identified as IP.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ignore_ip_dscp - ignore IP DSCP indication
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_ignore_ip_dscp_set(
    int unit,
    bcm_port_t port,
    uint32 ignore_ip_dscp);

/**
 * \brief - get from PRD parser indication wether it ignores IP 
 *        DSCP
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ignore_ip_dscp - ignore IP DSCP indication
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_ignore_ip_dscp_get(
    int unit,
    bcm_port_t port,
    uint32 *ignore_ip_dscp);

/**
 * \brief - set PRD parser to ignore MPLS EXP even if packet is 
 *        identified as MPLS
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ignore_mpls_exp - ignore MPLS EXP indication
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_ignore_mpls_exp_set(
    int unit,
    bcm_port_t port,
    uint32 ignore_mpls_exp);

/**
 * \brief - get from PRD parser indication wether it ignores 
 *        MPLS EXP
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ignore_mpls_exp - ignore MPLS EXP indication
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_ignore_mpls_exp_get(
    int unit,
    bcm_port_t port,
    uint32 *ignore_mpls_exp);

/**
 * \brief - set PRD Parser wether to ignore inner tag PCP DEI 
 *        indication even if packet is identified as double
 *        tagged
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ignore_inner_tag - ignore inner VLAN tag 
 *        indication
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_ignore_inner_tag_set(
    int unit,
    bcm_port_t port,
    uint32 ignore_inner_tag);

/**
 * \brief - get from PRD parser indication wether it ignores 
 *        inner VLAN tag
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ignore_inner_tag - ignore inner VLAN tag 
 *        indication
 *   
 * \return
 *   int -see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_ignore_inner_tag_get(
    int unit,
    bcm_port_t port,
    uint32 *ignore_inner_tag);

/**
 * \brief - set PRD Parser wether to ignore outer tag PCP DEI 
 *        indication even if packet is identified as VLAN tagged
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ignore_outer_tag - ignore outer VLAN tag 
 *        indication
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_ignore_outer_tag_set(
    int unit,
    bcm_port_t port,
    uint32 ignore_outer_tag);

/**
 * \brief - get from PRD parser indication wether it ignores 
 *        outer VLAN tag
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ignore_outer_tag - ignore outer VLAN tag 
 *        indication
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_ignore_outer_tag_get(
    int unit,
    bcm_port_t port,
    uint32 *ignore_outer_tag);

/**
 * \brief - set defualt priority for the PRD hard stage parser.
 *        if the packet is not identified as IP/MPLS/double
 *        tagged/single tagged or if the relevant indications
 *        are set to ignore, the default priority will be given
 *        to the packet. (the priority from the hard stage can
 *        be later overriden by the priority from the soft
 *        stage.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] defualt_priority - default prioroity
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_default_priority_set(
    int unit,
    bcm_port_t port,
    uint32 defualt_priority);

/**
 * \brief - get default priority given in the PRD hard stage 
 *        parser.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] defualt_priority - default priority
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_prd_default_priority_get(
    int unit,
    bcm_port_t port,
    uint32 *defualt_priority);

/**
 * \brief - fill the TM priority map (PRD hard stage). add the 
 *        the map priorty value to be returned per TC + DP
 *        values
 * 
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] tc - TC value (Traffic Class)
 * \param [in] dp - DP value (Drop Presedence)
 * \param [in] priority - priority to set for TC+DP
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_map_tm_tc_dp_set(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 priority);

/**
 * \brief - get information from TM priority map (PRD hard 
 *        stage). get the priority given for a conbination of
 *        TC+DP.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tc - TC value (Traffic Class)
 * \param [in] dp - DP value (Drop Presedence)
 * \param [out] priority - priority for the TC+DP
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_map_tm_tc_dp_get(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 *priority);

/**
 * \brief - fill the IP priority table (PRD hard stage). set 
 *        priority value for a specific DSCP.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] dscp - DSCP field value
 * \param [in] priority - priority for the DSCP
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_map_ip_dscp_set(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 priority);

/**
 * \brief - get information from IP priority table (PRD hard 
 *        stage). get the priorty returned for a specific DSCP
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] dscp - DSCP field value
 * \param [out] priority - returned priority
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_map_ip_dscp_get(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 *priority);

/**
 * \brief - fill the MPLS priority table (PRD hard stage). set 
 *        priority value for a specific EXP value.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] exp - EXP field value
 * \param [in] priority - priority for the EXP
 *   
 * \return
 *   int - see .h file
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_map_mpls_exp_set(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 priority);

/**
 * \brief - get information from the MPLS priority table. get 
 *        the priority returned for a specific EXP value
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] exp - EXP field value
 * \param [out] priority - priority for the EXP
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_map_mpls_exp_get(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 *priority);

/**
 * \brief - fill the ETH (VLAN) Prioroity table (PRD hard 
 *        stage). set a priority value to match a spcific
 *        PCP+DEI combination
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pcp - PCP value
 * \param [in] dei - DEI value
 * \param [in] priority - priority for the PCP+DEI
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_map_eth_pcp_dei_set(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 priority);

/**
 * \brief - get information from the ETH (VLAN) priority table 
 *        (PRD hard stage). get the priorty returned for a
 *        specific combination of PCP+DEI
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pcp - PCP value
 * \param [in] dei - DEI value
 * \param [out] priority - priority for the PCP+DEI
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_map_eth_pcp_dei_get(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 *priority);

/**
 * \brief - set PRD thresholds per priority. based on the 
 *        prioroity given from the parser, the packet is sent to
 *        the correct RMC. in each RMC there is a thresholds per
 *        priority which is mapped to it.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - RMC id in the CDU
 * \param [in] priority - priority to set the threshold to
 * \param [in] threshold - threshold value
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 priority,
    uint32 threshold);

/**
 * \brief - get the current threshold for a specific priority in 
 *        a specific RMC.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port 
 * \param [in] rmc - RMC id inside the CDU
 * \param [in] priority - priotiy to get threshold value
 * \param [out] threshold - returned threshold value
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 priority,
    uint32 *threshold);

/**
 * \brief - set one of the port's TPID values to be recognized 
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - TPID index (0-3)
 * \param [in] tpid_value - TPID value 
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value);

/**
 * \brief - get one of the TPID values recognized by the PRD 
 *        Parser.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - index of the TPID recognized(0-3)
 * \param [out] tpid_value - TPID value
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value);

/**
 * \brief - get PRD drop counter value, per RMC
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - RMC index insdie the CDU
 * \param [out] count - counter value
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_drop_count_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint64 *count);

/**
 * \brief - set configurable ether type to a ether type code. 
 *        the ether type codes are meaningful in the PRD soft
 *        stage (TCAM).
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code (1-6)
 * \param [in] ether_type_val - ether type value to map to the 
 *        ether type code
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val);

/**
 * \brief - get the ether type value mapped to a specific ether 
 *        type code, out of the configurable ether types (codes
 *        1-6). 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code (1-6)
 * \param [out] ether_type_val - ether type mapped to the ether 
 *        type code
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val);

/**
 * \brief - fill the PRD soft stage TCAM table. there are 32 
 *        entries in the table. if there is a hit, the priority
 *        for the packet will be taken from the TCAM entry
 *        information 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] entry_index - index in the table to set
 * \param [in] entry_info - information to fill the table entry
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_tcam_entry_set(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    imb_cdu_internal_prd_tcam_entry_info_t * entry_info);

/**
 * \brief - get information from the PRD soft stage (TCAM) 
 *        table. get specific entry information
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] entry_index - index in the TCAM table to get 
 *        information from
 * \param [out] entry_info - entry information
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_tcam_entry_get(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    imb_cdu_internal_prd_tcam_entry_info_t * entry_info);

/**
 * \brief - set the PRD soft stage (TCAM) key. there is a key 
 *        per ether type (total of 16 keys). the key is build of
 *        4 offstes given in the packet header. from each
 *        offset, 8 bits are taken to create a total of 32 bit.
 *        when comparing to the TCMA entries, the ether type
 *        code joins the key to create 36bit key:
 *        |ether type code|offset 4|offset 3|offset 2|offset 1|
 *        ----------------------------------------------------- 
 *        35              31       23       15       7        0
 *  
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code (0-15)
 * \param [in] offset_index - index of the offset to set to the 
 *        key (0-3)
 * \param [in] offset_value - offset value 
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value);

/**
 * \brief - get information about the TCAM key for a specific 
 *        ether type. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to recognize 
 *        the key.
 * \param [in] offset_index - which offset of the key to get
 * \param [out] offset_value - returned offset value
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value);

/**
 * \brief - set the offset base for the TCAM key. it means the 
 *        offsets that compose the key will start from this
 *        offset base. the offset base have 3 options:
 * 0=>start of packet 
 * 1=>end of eth header 
 * 2=>end of header after eth header. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to set hte 
 *        offset to. (for different ether types we might want
 *        different offset base)
 * \param [in] offset_base - offset base value. this offset base 
 *        should already match the DBAL enum type. see
 *        NIF_PRD_TCAM_KEY_OFFSET_BASE
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base);

/**
 * \brief - get the current offset base for the given ether 
 *        code.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to retrieve the 
 *        offset base.
 * \param [out] offset_base - returned offset base.
 *   
 * \return
 *   int -xee _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base);

/**
 * \brief - set ether type size (in bytes) for the given ether 
 *        type code. the ether type size is only used if the
 *        offset base for the key is "end of header after eth
 *        header"
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code of the ether 
 *        type
 * \param [in] ether_type_size - ether type size value
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size);

/**
 * \brief - get the ether type currently set for a specific 
 *        ether type code
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code for the ether 
 *        type size
 * \param [out] ether_type_size - returned ether type size
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size);

/**
 * \brief - set Control Frame properties to be recognized by the
 *        PRD parser. if a control frame is identified, it
 *        automatically gets the higest priroity (3). each
 *        packet is compared against all control frame
 *        properties of the CDU
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to 
 *        set (0-3)
 * \param [in] control_frame_config - information to recognize 
 *        to as control frame 
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

/**
 * \brief - get Control Frame properties recognized by the 
 *        parser. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to 
 *        get (0-3)
 * \param [in] control_frame_config - control frame information
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

/**
 * \brief - enable PRD hard stage per RMC. this effectively 
 *        enable the PRD feature.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - RMC id inside the CDU
 * \param [in] enable - enable indication
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_hard_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 enable);

/**
 * \brief - get enable indication for the PRD hard stage per RMC
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - RMC id inside the CDU
 * \param [out] enable - enable indication
 *   
 * \return
 *   int -see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_hard_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 *enable);

/**
 * \brief - enable PRD soft stage per port. PRD soft stage is a 
 *        TCAM for which the user configures both the table and
 *        the key. if there is a hit in the TCAM, the priority
 *        from the TCAM entry will override the priority from
 *        the hard stage
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_eth - enable soft stage for eth port 
 *        (should only be set for eth ports)
 * \param [in] enable_tm - enable soft stage for TM ports 
 *        (should only be set for ports with TM headers -
 *        ITMH/FTMH)
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_soft_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable_eth,
    uint32 enable_tm);

/**
 * \brief - get enable indication for PRD soft stage
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable_eth - soft stage enabled for eth port
 * \param [out] enable_tm - soft stage enabled for TM port
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_soft_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable_eth,
    uint32 *enable_tm);

/**
 * \brief - set ITMH TC + DP offsets for the PRD.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tc_offset - TC offset on ITMH header
 * \param [in] dp_offset - DP offset in ITMH header
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_itmh_offsets_set(
    int unit,
    bcm_port_t port,
    uint32 tc_offset,
    uint32 dp_offset);

/**
 * \brief - get ITMH TC and DP offstes in PRD
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tc_offset - TC offset in ITMH header
 * \param [out] dp_offset - DP offset in ITMH header
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_itmh_offsets_get(
    int unit,
    bcm_port_t port,
    uint32 *tc_offset,
    uint32 *dp_offset);

/**
 * \brief - set FTMH  TC + DP offsets for the PRD
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tc_offset - TC offset in FTMH header
 * \param [in] dp_offset - DP offset in FTMH header
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_ftmh_offsets_set(
    int unit,
    bcm_port_t port,
    uint32 tc_offset,
    uint32 dp_offset);

/**
 * \brief - get FTMH TC + DP offsets in PRD
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tc_offset - TC offset in FTMH header
 * \param [in] dp_offset - DP offset in FTMH header
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_ftmh_offsets_get(
    int unit,
    bcm_port_t port,
    uint32 *tc_offset,
    uint32 *dp_offset);

/**
 * \brief - enable checking for MPLS special label, if packet is 
 *        identified as MPLS
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_mpls_special_label_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable);

/**
 * \brief - get enable indication for MPLS special label.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_mpls_special_label_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);

/**
 * \brief - set MPLS special label properties. if one of the 
 *        MPLS labels is identified as special label and its
 *        value match one of the special label values, priority
 *        for the packet is taken from the special label
 *        properties.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] special_label_index - special label index to 
 *        configure (0-3)
 * \param [in] label_config - special label configuration: 
 *        value, priority, TDM indication.
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

/**
 * \brief - get current configuration of MPLS special label 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] special_label_index - index of the special label 
 *        to retrieve
 * \param [out] label_config - special label properties: 
 *        value, priority, TDM indication.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

/**
 * \brief - set outer tag size for the port. if port is port 
 *        extander, the outer tag size should be set to 8B,
 *        otherwise 4B
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] outer_tag_size - outer tag size, compatibale to 
 *        DBAL enum field. see VLAN_OUTER_TAG_SIZE.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_outer_tag_size_set(
    int unit,
    bcm_port_t port,
    uint32 outer_tag_size);

/**
 * \brief - get current outer tag size for the port
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] outer_tag_size - outer tag size, compatible to 
 *        DBAL enum type. see VLAN_OUTER_TAG_SIZE
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_outer_tag_size_get(
    int unit,
    bcm_port_t port,
    uint32 *outer_tag_size);

/**
 * \brief - set PRD port type, acocrding to the header type of 
 *        the port
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] prd_port_type - PRD port type. should match DBAL 
 *        enum type. see NIF_PRD_PORT_TYPE.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_port_type_set(
    int unit,
    bcm_port_t port,
    uint32 prd_port_type);

/**
 * \brief - get PRD port type for the port
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] prd_port_type - PRD port type. matches the DBAL 
 *        enum type. see NIF_PRD_PORT_TYPE.
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_port_type_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_port_type);

 /**
  * \brief - algorithm to get CDU PortMacro lane boundaries,
  *         including the lower boundry and the upper boundary.
  *  
  * \param [in] unit - chip unit id. 
  * \param [in] port - logical port # 
  * \param [out] lower_bound - the first Phy ID for this CDU.
  * \param [out] upper_bound - the last Phy ID for this CDU.
  *   
  * \return
  *   int 
  *   
  * \remarks
  *   * None
  * \see
  *   * None
  */
int imb_cdu_port_pm_boundary_get(
    int unit,
    bcm_port_t port,
    int *lower_bound,
    int *upper_bound);

/**
 * \brief - Reset the port credit in the CDU.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_credit_tx_reset(
    int unit,
    bcm_port_t port);

/**
 * \brief - Get NIF RX fifo status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc index
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_port_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    int rmc_id,
    uint32 *max_occupancy,
    uint32 *fifo_level);

/**
 * \brief - Get NIF TX fifo status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 * \param [out] pm_credits - indicate the credits from PM tx buffer
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_port_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits);

/**
 * \brief - map NIF port to EGQ interface
 *
 * \param [in] unit   - chip unit id
 * \param [in] core   - device core
 * \param [in] nif_id - NIF ID. range of 0-47.
 * \param [in] egq_if - EQQ interface. range of 0-63.
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_nif_egq_interface_map(
    int unit,
    bcm_core_t core,
    uint32 nif_id,
    uint32 egq_if);

/**
 * \brief - map SIF instance (port) into NIF port
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] sif_instance_id - statistics interface instance (port) id
 * \param [in] inner_cdu_id - inner_cdu_id (which extructed from the logical port)
 * \param [in] first_lane_in_cdu - first_lane_in_cdu (which extructed from the logical port)
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_port_to_stif_instance_map(
    int unit,
    int core,
    int sif_instance_id,
    int inner_cdu_id,
    int first_lane_in_cdu);

/**
 * \brief - map SIF instance (port) into NIF port
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_id - imb instance id
 * \param [in] in_reset - reset state
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_pm_reset_set(
    int unit,
    int imb_id,
    uint32 in_reset);

/**
 * \brief - map SIF instance (port) into NIF port
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_id - imb instance id
 * \param [out] in_reset - reset state
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_pm_reset_get(
    int unit,
    int imb_id,
    uint32 *in_reset);

/**
 * \brief - Set PRD logic in Bypass mode. the PRD parser will be bypassed.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] inner_cdu_id - cdu index inside the core
 * \param [in] is_bypass - bypass mode
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_bypass_set(
    int unit,
    int core,
    int inner_cdu_id,
    uint32 is_bypass);

/**
 * \brief - Set PRD logic in Bypass mode. the PRD parser will be bypassed.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] inner_cdu_id - cdu index inside the core
 * \param [out] is_bypass - bypass mode
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_bypass_get(
    int unit,
    int core,
    int inner_cdu_id,
    uint32 *is_bypass);

/**
 * \brief - Enable Rx Fifos (RMCs) to generate Priority FC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] inner_cdu_id - CDU index inside the core
 * \param [in] rmc - Rx Fifo to enable
 * \param [in] priority - priority of the generated PFC
 * \param [in] enable - TRUE or FALSE to enable/disable the PFC generation
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_port_gen_pfc_from_rmc_enable_set(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    int rmc,
    uint32 priority,
    uint32 enable);

/**
 * \brief - Get the enable value for Rx Fifos (RMCs) to generate Priority FC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] inner_cdu_id - CDU index inside the core
 * \param [in] rmc - Rx Fifo for which to get the value
 * \param [in] priority - priority of the generated PFC
 * \param [out] enable - TRUE or FALSE, indicating enabled/disabled PFC generation
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_port_gen_pfc_from_rmc_enable_get(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    int rmc,
    uint32 priority,
    uint32 *enable);

/**
 * \brief - Enable/disable value Rx Fifos (RMCs) to generate Link Level FC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] inner_cdu_id - CDU index inside the core
 * \param [in] lane_in_cdu - Lane inside the CDU
 * \param [in] rmc - Rx Fifo for which to set the value
 * \param [in] enable - TRUE or FALSE to enable/disable the LLFC generation
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_port_gen_llfc_from_rmc_enable_set(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    uint32 lane_in_cdu,
    int rmc,
    uint32 enable);

/**
 * \brief - Enable / disable PM to stop TX on all of a port's lanes according to a LLFC signal from CFC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] inner_cdu_id - CDU index inside the core
 * \param [in] lane_in_cdu - Lane inside the CDU
 * \param [in] enable - TRUE or FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    uint32 lane_in_cdu,
    uint32 enable);

/**
 * \brief - Set the LLFC/PFC thresholds for FC generation from Quad MAC Lane FIFO (QMLF).
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] inner_cdu_id - CDU index inside the core
 * \param [in] rmc - Rx Fifo for which to set the value
 * \param [in] flags - flags indicating the threshold to set
 * \param [in] threshold - Threshold value
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_port_fc_rx_qmlf_threshold_set(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    int rmc,
    uint32 flags,
    uint32 threshold);

/**
 * \brief - Get the LLFC/PFC thresholds for FC generation from Quad MAC Lane FIFO (QMLF).
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] inner_cdu_id - CDU index inside the core
 * \param [in] rmc - Rx Fifo for which to set the value
 * \param [in] flags - flags indicating the threshold to set
 * \param [out] threshold - Threshold value
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_port_fc_rx_qmlf_threshold_get(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    int rmc,
    uint32 flags,
    uint32 *threshold);

/**
 * \brief - Set the Flow Control Interface in/out of reset for a specified CDU.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] cdu_id - CDU index inside the core
 * \param [in] in_reset - TRUE/FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_fc_reset_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    uint32 in_reset);

/**
 * \brief - Enable/disable the generation of a LLFC based on a signal from Global Resources thresholds for a specified CDU.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] cdu_id - CDU index inside the core
 * \param [in] lane - Lane inside the CDU
 * \param [in] enable - TRUE/FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_llfc_from_glb_rsc_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int lane,
    uint32 enable);

/**
 * \brief - Per CDU map the received PFC - each priority received by NIF should be mapped to a priority that will be sent to EGQ in order to stop a Q-pair.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] cdu_id - CDU index inside the core
 * \param [in] nif_priority - priority of the received PFC
 * \param [in] egq_priority - priority to be sent to EGQ
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_pfc_rec_priority_map(
    int unit,
    bcm_core_t core,
    int cdu_id,
    uint32 nif_priority,
    uint32 egq_priority);

/**
 * \brief - Per CDU unmap all priorities of the received PFC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] cdu_id - CDU index inside the core
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_internal_pfc_rec_priority_unmap(
    int unit,
    bcm_core_t core,
    int cdu_id);

 /*
  * }
  */
#endif/*_IMB_CDU_INTERNAL_H_INCLUDED_*/
