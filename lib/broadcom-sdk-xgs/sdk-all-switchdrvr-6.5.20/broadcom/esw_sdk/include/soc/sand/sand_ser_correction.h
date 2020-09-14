/*
 * $Id: sand_ser_correction.h, v1 16/06/2014 09:55:39 itamar $
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implement header for ser correction.
 */

#ifndef _SAND_SER_CORRECTION_H_
#define _SAND_SER_CORRECTION_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/defs.h>
#include <shared/shrextend/shrextend_debug.h>

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

shr_error_e sand_mdb_ecc_correction(
    int unit,
    const uint32 data_len_bits,
    uint32* data,
    uint32* ecc);

#endif /*_SAND_SER_CORRECTION_H_ */

