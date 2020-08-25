/*
 * ! \file diag_dnxc.diag.h 
 * structure, definition and utilities for diagnostic pack 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNXC_DIAG_H_INCLUDED
#define DIAG_DNXC_DIAG_H_INCLUDED

/* 
* { Local Defines
*/

#define BLOCK_MAX_INDEX (256)   /* currently 126 blocks in Ramon and 213 blocks in Jericho2 */
#define NOF_BITS_PER_WORD (32)
#define FULL_32_BITS_MASK (0xffffffff)
#define ASCII_CODE_OF_0 (48)
#define ASCII_CODE_OF_9 (57)
#define BLOCK_NAME_LENGTH 6     /* Length of block name */

/* 
* } Local Defines
*/

/* 
* { Local Functions
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
 * \param [out] overflow - indicates if sum of val_1+val_2 has overflow 
 * \return 
 *      Standard error handling
 */
shr_error_e sum_value_to_str(
    uint32 *val_1,
    uint32 *val_2,
    int nof_bit,
    int in_hex,
    char *buf,
    int *overflow);

/**
 * \brief
 *   To start gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \param [in] interval - indicate how long gtimer keeps enabled (usec)
 * \return
 *      Standard error handling
 */
shr_error_e sh_dnxc_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int interval);

/**
 * \brief
 *   To stop gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \return
 *      Standard error handling
 */
shr_error_e sh_dnxc_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids);

/**
 * \brief
 *   Check if the register match the blocks specified by "*match_n"
 * \param [in] unit - Unit #
 * \param [in] match_n - to specify block name that block should match with
 * \param [out] nof_block - to get number of blocks that this register matches with
 * \param [out] block_ids - an integer array to get all of block ID that this register matches with
 * \return
 *      Standard error handling
 */
shr_error_e counter_block_name_match(
    int unit,
    char *match_n,
    int *nof_block,
    int *block_ids);
/* 
* } Local Functions
*/

/* 
* { Local Structures
*/

/* 
* } Local Structures
*/

extern sh_sand_man_t Sh_dnxc_diag_ser_man;
extern sh_sand_cmd_t Sh_dnxc_diag_ser_cmds[];
#endif /* DIAG_DNXC_DIAG_H_INCLUDED */
