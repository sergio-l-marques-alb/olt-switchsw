/*
 * ! \file diag_dnxc.diag.h 
 * structure, definition and utilities for diagnostic pack 
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNXC_DIAG_H_INCLUDED
#define DIAG_DNXC_DIAG_H_INCLUDED

/* 
* } Local Defines
*/

/**
 * \brief
 *   translate hex value to a string"
 * \param [in] val - hex value stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies 
 * \param [out] buf - buffer to store the string
 * \return 
 *      Standard error handling
 */
shr_error_e format_value_hex_to_str(
    uint32 *val,
    int nof_bit,
    char *buf);

/**
 * \brief
 *   translate decimal (max 64 bits) or hex value  to a string"
 * \param [in] val - the value stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies 
 * \param [in] in_hex - indicate decimal or hex string is required 
 * \param [out] buf - buffer to store the string
 * \return 
 *      Standard error handling
 */
shr_error_e format_value_to_str(
    uint32 *val,
    int nof_bit,
    int in_hex,
    char *buf);

/**
 * \brief
 *   add two values together and then translate the sum (decimal or hex) to a string
 *   the supported maximal value is 64 bits.
 * \param [in] val_1 - value 1 stored in the uint32 array
 * \param [in] val_2 - value 2 stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies 
 * \param [in] in_hex - indicate decimal or hex string is required 
 * \param [out] buf - buffer to store the string
 * \return 
 *      Standard error handling
 */
shr_error_e sum_value_to_str(
    uint32 *val_1,
    uint32 *val_2,
    int nof_bit,
    int in_hex,
    char *buf);

#endif /* DIAG_DNXC_DIAG_H_INCLUDED */
