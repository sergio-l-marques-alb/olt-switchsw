/*
 * $Id: dnxc_ser_correction.h, v1 16/06/2014 09:55:39 itamar $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implement header for ser correction.
 */

#ifndef _DNXC_SER_CORRECTION_H_
#define _DNXC_SER_CORRECTION_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jer2) and DNXF family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
#include <soc/defs.h>
#include <soc/dnxc/legacy/error.h>

/*************
 * FUNCTIONS *
 *************/

shr_error_e calc_ecc(
        int unit,
        const uint32 data_len_bits,
        const uint32 *data,
        uint32 *ecc_data);

shr_error_e ecc_correction(
        int unit,
        soc_mem_t mem,
        const uint32 data_len_bits,
        int xor_flag,
        uint32 *data, 
        uint32 *ecc);

#endif /*_DNXC_SER_CORRECTION_H_ */
