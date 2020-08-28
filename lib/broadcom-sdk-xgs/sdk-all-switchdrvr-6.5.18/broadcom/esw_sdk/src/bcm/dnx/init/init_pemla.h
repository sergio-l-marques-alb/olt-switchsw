/** \file init_pemla.h
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef INIT_PEMLA_H_INCLUDED
/*
 * {
 */
#define INIT_PEMLA_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/init/init.h>

/**
 * \brief - wrapper for PEMLA drv initialization
 *
 */
shr_error_e dnx_init_pemla_init(
    int unit);

/**
 * \brief - wrapper for PEMLA drv deinitialization
 *
 */
shr_error_e dnx_init_pemla_deinit(
    int unit);

/*
 * \brief - PEM Init sequence 
 */
shr_error_e dnx_init_pem_sequence_flow(
    int unit);
/*
 * }
 */
#endif /** INIT_PEMLA_H_INCLUDED */
