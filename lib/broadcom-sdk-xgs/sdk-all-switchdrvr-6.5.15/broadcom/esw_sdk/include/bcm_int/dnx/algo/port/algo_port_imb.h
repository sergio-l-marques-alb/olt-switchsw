/** \file algo_port_imb.h
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef ALGO_PORT_IMB_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_IMB_H_INCLUDED

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
  * }
  */
/*
 * Functions
 * {
 */
 /**
 * \brief - calculate the thershold after overflow for the fifo 
 *        according to the FIFO size.
 * After FIFO reches overflow, writing to the fifo will resume only after fifo level 
 * goes below this value.  
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] fifo_size - fifo size in entries
 * \param [out] thr_after_ovf - threshold after overflow 
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_threshold_after_ovf_get(
    int unit,
    bcm_port_t port,
    uint32 fifo_size,
    uint32 *thr_after_ovf);

/**
 * \brief - calculate the weight to be set for the CDU in the 
 * Low priority scheduler. the weight is set per CDU - the 
 * bigger the weight, the CDU will get more BW in the RR. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_port_include - indication wether to include /
 *        exclude specified port for the weight calculation
 * \param [out] weight - weight for the CDU 
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_algo_imb_cdu_scheduler_weight_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *weight);

/**
 * \brief - calculate the weight to be set for the RMC in the 
 * Low priority scheduler. the weight is set per RMC in the CDU 
 * - the more weight the RMC has, it will get more 
 *   BW in the RR.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] weight - weight for the RMC
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_algo_imb_rmc_scheduler_weight_get(
    int unit,
    bcm_port_t port,
    int *weight);

/**
 * \brief - algorithm to get the RMC id according to the lane 
 * index in the CDU and scheduler priority. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] prio - scheduler priority. see 
 *        bcm_port_nif_scheduler_t
 * \param [out] rmc_id - output RMC id.
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_algo_imb_cdu_port_rmc_id_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t prio,
    int *rmc_id);

/**
 * \brief - algorithm to calculate the first and last entry of a
 *        specific logical fifo in the CDU memory.
 * 
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] nof_entries - size of the logical fifo to be set. 
 *        can be set ot -1 for equal devision of the memory
 *        between all logical fifos related to the port (can be
 *        up to 3 logical fifos per port)
 * \param [in] prev_last_entry - in case where there is more 
 *        than one logical fifo, prev_last_entry is the last
 *        entry of the previous logical fifo. this is the offset
 *        from which to start allocating for the current logical
 *        fifo. if set to -1, the allocation will start from
 *        offset 0. (expected to be set to -1 for the first
 *        logical fifo)
 * \param [in] nof_prio_groups - how many logical fifos 
 *        the port has.
 * \param [out] first_entry - output first entry
 * \param [out] last_entry - output last entry
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_algo_imb_rmc_entry_range_get(
    int unit,
    bcm_port_t port,
    int nof_entries,
    int prev_last_entry,
    int nof_prio_groups,
    uint32 *first_entry,
    uint32 *last_entry);

/**
 * \brief - algorithm to return start TX threshold for the port. 
 * this threshold represent the number of 64B words to be 
 * accumulated in the MLF before transmitting towards the PM. 
 * This is to prevent TX MAC starvation and is important for 
 * systems with oversubscription. 
 * 
 * \param [in] unit - chip unit id
 * \param [out] tx_start_thr - tx start threshold value
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_tx_start_thr_get(
    int unit,
    int *tx_start_thr);

/**
 * \brief - segment is a resource of ILKN, ehich has to be 
 *        shared between the two ports of the same ILKN core.
 *        the division can be as follows:
 * 1. ILKN0 has 4 segments (ILKN1 is not active) 
 * 2. ILKN0 and ILKN1 get two segments each. 
 * the number of segment a port should get is determined by the 
 * number of lanes, lane rate, burst short value and core clock 
 * of the device. if there is not enough segments to support the 
 * port, an error should be returned. 
 *  
 * \param [in] unit - chip unit id. 
 * \param [in] port - logical port # 
 * \param [out] nof_segments - returned nof segments.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_nof_segments_get(
    int unit,
    bcm_port_t port,
    int *nof_segments);

 /*
  * }
  */
#endif/*_ALGO_PORT_IMB_H_INCLUDED__*/
