/** \file jer2_regs.h
 *
 * Functions for handling registers.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __JER2_REGS_INCLUDED__
/* { */
#define __JER2_REGS_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/types.h>
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/**
 * \brief
 *   Checks if any of the Fabric Quads are disabled and according to that sets
 *   the BROADCAST_ID and LAST_IN_CHAIN for the FSRD.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e
soc_jer2_brdc_fsrd_blk_id_set(
        int unit);

/**
 * \brief
 *   Power up/down a FSRD block.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fsrd_block -
 *   FSRD block id
 * \param [in] enable -
 *   1:enable, 0:disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e
soc_jer2_fsrd_block_enable_set(
        int unit,
        int fsrd_block,
        int enable);

/**
 * \brief
 *   Power up/down a FSRD block.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e
soc_jer2_port_fsrd_block_enable_set(
        int unit,
        soc_port_t logical_port,
        int enable);

/**
 * \brief
 *   Initialize all FSRD blocks to be powered down.
 *   The FSRDs will later powered up by calling bcm_port_probe().
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e
soc_jer2_fsrd_blocks_low_power_init(
        int unit);

/**
 * \brief
 *   Get whether a FMAC block is enabled.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmac_block -
 *   The FMAC block index.
 * \param [out] enable -
 *   1 - FMAC block is enabled.
 *   0 - FMAC block is disabled.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e
soc_jer2_fmac_block_enable_get(
        int unit,
        int fmac_block,
        int *enable);

/* } */

#endif /*__JER2_REGS_INCLUDED__*/

