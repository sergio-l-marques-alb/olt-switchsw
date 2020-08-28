/** \file oam_init.h
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _OAM_INIT_INCLUDED__
/*
 * {
 */
#define _OAM_INIT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * }
 */
#include <soc/dnx/dbal/dbal.h>

/**
 * \brief -
 * Initialize OAM module.
 * This function calls initialazation functions for
 * OAM procedures
 * 
 * Called from init sequence as part of dnx init pp sequence
 *
 * \par DIRECT INPUT:
 *    \param [in] unit -
 *     Relevant unit.
 * \par DIRECT OUTPUT:
 *    \retval Error indication according to shr_error_e enum
 * 
 */
shr_error_e dnx_oam_init(
    int unit);

/**
 * \brief -
 * De-initialize OAM module. 
 *  
 * Called from init sequence as part of dnx deinit pp sequence
 *
 * \par DIRECT INPUT:
 *    \param [in] unit -
 *     Relevant unit.
 * \par DIRECT OUTPUT:
 *    \retval Error indication according to shr_error_e enum
 *  
 */
shr_error_e dnx_oam_deinit(
    int unit);

/**
 * \brief -
 * Map OAM opcode(8 bits) to internal opcode(4 bits) and set OAM counter offset.
 * Mapping is done via:
 *             ingress(DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP table) &
 *             egress(DBAL_TABLE_EGRESS_ETHERNET_OAM_OPCODE_MAP table)
 *
 * Called from init sequence and could be called for re-map some opcode, that are not used by standart as
 * a part of bcm_oam_opcode_map_set() api.
 *
 * \param [in] unit -
 *     Relevant unit.
 * \param [in] opcode -
 *     OAM opcode that should be mapped
 * \param [in] internal_opcode
 *     Internal OAM opcode
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 */
shr_error_e dnx_oam_opcode_map_set(
    int unit,
    int opcode,
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode);

#endif/*_OAM_INIT_INCLUDED__*/
