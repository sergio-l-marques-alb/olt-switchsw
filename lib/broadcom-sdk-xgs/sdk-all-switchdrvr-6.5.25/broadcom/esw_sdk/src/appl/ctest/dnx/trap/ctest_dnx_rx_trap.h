/** \file ctest_dnx_rx_trap.h
 * Purpose: Sematic testing of traps.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef CTEST_DNX_RX_TRAP_H_INCLUDED
#define CTEST_DNX_RX_TRAP_H_INCLUDED

/*************
 * INCLUDES  *
 *************/

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - compare 2 between set & get values
 *
 * \param [in] unit         - Number of hardware unit used
 * \param [in] set_value_p - Pointer to set value
 * \param [in] get_value_p - Pointer to get value
 * \param [in] size_of_value - size of value
 * \param [in] value_name - value name
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e ctest_dnx_rx_trap_compare_values(
    int unit,
    void *set_value_p,
    void *get_value_p,
    uint32 size_of_value,
    char *value_name);

/**
* \brief
*   Create trap and set trap action.
* \param [in] unit - unit ID
* \param [in] trap_type - trap type
* \param [in] trap_id_p - trap id
* \param [in] trap_config_p - trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
shr_error_e ctest_dnx_rx_trap_create_and_set(
    int unit,
    bcm_rx_trap_t trap_type,
    int *trap_id_p,
    bcm_rx_trap_config_t * trap_config_p);

/**
* \brief
*   Create trap and set trap action.
* \param [in] unit - unit ID
* \param [in] trap_id - trap id
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
shr_error_e ctest_dnx_rx_trap_destroy(
    int unit,
    int trap_id);

 /**
 * \brief
 *   Set trap action profile for given trap_type.
 * \param [in] unit - unit ID
 * \param [in] trap_type - trap type
 * \param [in] trap_gport - encodes the trap_action_profile
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e ctest_dnx_rx_trap_profile_set(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_gport_t trap_gport);

/**
* \brief
*   Clear trap action profile for given trap_type.
* \param [in] unit - unit ID
* \param [in] trap_type - trap type
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
shr_error_e ctest_dnx_rx_trap_profile_clear(
    int unit,
    bcm_rx_trap_t trap_type);

/**
 * \brief
 *   Example for InRIF create.
 *
 * \param [in] unit - The unit number.
 * \param [out] rif_p - RIF
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_inrif_create_example(
    int unit,
    bcm_if_t * rif_p);

/**
 * \brief
 *   Example for ETH InRIF create.
 *
 * \param [in] unit - The unit number.
 * \param [out] rif_p - RIF
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_eth_inrif_create_example(
    int unit,
    bcm_if_t * rif_p);

/**
 * \brief
 *   Example for OutRIF create.
 *
 * \param [in] unit - The unit number.
 * \param [out] rif_p - RIF
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_outrif_create_example(
    int unit,
    bcm_if_t * rif_p);

/**
 * \brief
 *  Example for InLIF create.
 *
 * \param [in] unit - The unit number.
 * \param [out] lif_p - LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_inlif_create_example(
    int unit,
    bcm_gport_t * lif_p);

/**
 * \brief
 *   Example for OutLIF create.
 *
 * \param [in] unit - The unit number.
 * \param [out] lif_p - LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_outlif_create_example(
    int unit,
    bcm_gport_t * lif_p);

/**
 * \brief
 *   Example for default ESEM entry create.
 *
 * \param [in] unit - The unit number.
 * \param [out] vlan_port_id_p - vlan_port_id of default ESEM entry.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_esem_default_example(
    int unit,
    bcm_gport_t * vlan_port_id_p);

/**
 * \brief
 *   Example for ESEM entry with match criteria create.
 *
 * \param [in] unit - The unit number.
 * \param [out] vlan_port_id_p - vlan_port_id of match ESEM entry.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_esem_match_example(
    int unit,
    bcm_gport_t * vlan_port_id_p);

/**
 * \brief
 *   Destroy RIF created by exapmle (either InRIF or OutRIF).
 *
 * \param [in] unit - The unit number.
 * \param [in] rif_p - RIF.
 * \param [in] rif_type - ingress or egress RIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_rif_destroy_example(
    int unit,
    bcm_if_t * rif_p,
    bcm_rx_trap_lif_type_t rif_type);

/**
 * \brief
 *   Destroy LIF created by exapmle (either InLIF or OutLIF).
 *
 * \param [in] unit - The unit number.
 * \param [in] lif_p - LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_lif_destroy_example(
    int unit,
    bcm_gport_t * lif_p);

/**
 * \brief
 *   Destroy ESEM entry created by exapmle (either default or match).
 *
 * \param [in] unit - The unit number.
 * \param [out] vlan_port_id_p - vlan_port_id of default ESEM entry.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e ctest_dnx_rx_trap_esem_destroy_example(
    int unit,
    bcm_gport_t * vlan_port_id_p);

#endif /* CTEST_DNX_RX_TRAP_H_INCLUDED */
