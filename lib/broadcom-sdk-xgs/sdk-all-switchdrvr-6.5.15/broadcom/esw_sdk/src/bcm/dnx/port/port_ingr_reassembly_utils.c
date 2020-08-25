/** \file port_ingr_reassembly_utils.c
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/** ----------------------------------------------------------------------------------------------------------- */
/** STUBS */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

/**
 * \brief - Returns TRUE iff port belongs to interleaved interface
 * 
 */
shr_error_e
dnx_port_ingr_reassembly_is_interface_interleaved(
    int unit,
    bcm_port_t port,
    int *is_interleaved)
{
    /*
     * Currently assume that ILKN interface is always interleaved,
     * as it looks like there are enough reassembly contexts (and there is no ILKN) in Jer2.
     * If in future devices it does not hold,
     * need to add support for marking interface as interleaved in port SW DB
     *
     * Original implementation in Jer was:
     *     ilkn_config->interleaved 
     */
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    /*
     * if both non interleaved ILKN and dedicated TDM context are supported
     * interface with dedicated TDM context should be excluded here
     */

    *is_interleaved = (port_type == DNX_ALGO_PORT_TYPE_NIF_ILKN);

exit:
    SHR_FUNC_EXIT;
}
