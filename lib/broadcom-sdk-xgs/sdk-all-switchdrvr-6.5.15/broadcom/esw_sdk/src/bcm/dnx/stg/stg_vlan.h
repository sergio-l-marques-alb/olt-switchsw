/*! \file stg_vlan.h
 *
 *  Internal DNX STG APIs
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  All Rights Reserved.
 *
 */
#include <bcm_int/dnx_dispatch.h>
#ifndef STG_VLAN_H_INCLUDED
/* { */
#define STG_VLAN_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs
 * {
 */

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
 * DECLARATIONs
 * {
 */

/**
 * \brief - Add the given VSI to the specified stg.
 *
 * \param [in] unit - The unit ID
 * \param [in] stg  - The given stg_id
 * \param [in] vsi  - The given vsi
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   Add the VSI to the stg linked list first (sw_state), then update The VSI
 *   information with the stg (ING_VSI_INFO/EGR_VSI_INFO) in MDB.
 *
 * \see
 *   dnx_stg_vlan_first_set
 *   dnx_vlan_stg_set
 *
 */
shr_error_e dnx_stg_vlan_map_add(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vsi);

/**
 * \brief - Set the given vid to the VSI link list of the stg.
 *
 * \param [in] unit - Unit ID
 * \param [in] stg - The Spanning Tree Group the VSI is associated to.
 * \param [in] vsi - The VSI will be set as the first VSI of the stg.
 *
 * \remark
 *  This procedure set the VSI to the STG's FIRST_VSI table.
 *
 * \see
 *   dnx_stg_vlan_first_get.
 */
shr_error_e dnx_stg_vlan_first_set(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vsi);

/**
 * \brief - Get the first vsi from the VSI link list of the stg.
 *
 * \param [in] unit - Unit ID
 * \param [in] stg - The Spanning Tree Group the VSI is associated to.
 * \param [out] vsi - Pointer to receive the VSI.
 *
 * \remark
 *  None.
 *
 * \see
 *   dnx_stg_vlan_first_set.
 */
shr_error_e dnx_stg_vlan_first_get(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t * vsi);

/**
 * \brief - Set the given vsi to the VSI link list of the stg.
 *
 * \param [in] unit - Unit ID
 * \param [in] cur_vsi - The index to next_vsi.
 * \param [in] next_vsi - The given VSI.
 *
 * \remark
 *  This procedure set the VSI to the NEXT_VSI table.
 *
 * \see
 *   * dnx_stg_vlan_next_get.
 */
shr_error_e dnx_stg_vlan_next_set(
    int unit,
    bcm_vlan_t cur_vsi,
    bcm_vlan_t next_vsi);

/**
 * \brief - Get the next vsi from VSI link list of the stg.
 *
 * \param [in] unit - Unit ID
 * \param [in] cur_vsi - The index to vsi.
 * \param [out] next_vsi - Pointer to receive the next vsi.
 *
 * \remark
 *  None.
 *
 * \see
 *   * dnx_stg_vlan_next_set.
 */
shr_error_e dnx_stg_vlan_next_get(
    int unit,
    bcm_vlan_t cur_vsi,
    bcm_vlan_t * next_vsi);

/*
 * }
 */
#endif /* STG_VLAN_H_INCLUDED */
