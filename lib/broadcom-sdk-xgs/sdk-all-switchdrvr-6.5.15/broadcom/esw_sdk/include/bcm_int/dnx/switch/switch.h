/** \file bcm_int/dnx/switch/switch.h
 * 
 * Internal DNX SWITCH APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef SWITCH_H_INCLUDED
/* { */
#define SWITCH_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */

/*
 * }
 */

/*
 * DEFINES
 * {
 */
/** Port header type index for both directions - IN and OUT */
#define DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH 0
/** Port header type index IN */
#define DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN 1
/** Port header type index OUT */
#define DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT 2

#define DNX_SWITCH_INTR_BLOCK_MAX_NUM       2048
#define DNX_SWITCH_INTR_BLOCK_WIDTH         BITS2WORDS(DNX_SWITCH_INTR_BLOCK_MAX_NUM)

/*
 * }
 */

/**
 * \brief
 * Verify network_group_id range
*/
shr_error_e dnx_switch_network_group_id_verify(
    int unit,
    int is_ingress,
    bcm_switch_network_group_t network_group_id);

/**
 * \brief - Get routed learning mode for supported applications according to input flags
 *
 * \param [in] unit - unit Id
 * \param [out] arg - output flags (BCM_SWITCH_CONTROL_L3_LEARN_XXX)
 *
 * \return
 *   shr_error_e - Error type
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_switch_control_routed_learn_get(
    int unit,
    int *arg);

/**
 * \brief - Retrieve the switch header type according to port and port direction
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] direction - Port direction
 * \param [out] switch_header_type - Switch Header Type
 *
 * Port direction can be 0, 1 and 2. 0 means both directions (in and out), 1 means 'in' and 2 means 'out'.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_switch_header_type_get(
    int unit,
    bcm_port_t port,
    int direction,
    int *switch_header_type);

/* } */
#endif /* SWITCH_H_INCLUDED */
