/** \file oam_oamp_crc.h
 * $Id$
 *
 * OAM utilities procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef OAM_OAMP_UTILS_H_INCLUDED
/*
 * {
 */
#define OAM_OAMP_UTILS_H_INCLUDED

 /*
  * Include files.
  * {
  */
#include <sal/types.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */

/* Used to encode a portion of a packet into crc. */
#define OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION  (16)
#define OAM_OAMP_CRC_BUFFER_SIZE                   (128)
#define OAM_OAMP_CRC_MASK_MSB_BIT_SIZE  (64)
#define OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE BITS2BYTES(OAM_OAMP_CRC_MASK_MSB_BIT_SIZE)
#define OAM_OAMP_CRC_MASK_LSB_BYTE_SIZE (120)

/*
 * }
 */

/*
 * See .h file
 */
/**
 * \brief
 *   This function calculates a 16 bit CRC over a 128 byte
 *   buffer with a configurable mask
 * \param [in] buffer  -
 *   128 byte source buffer.
 * \param [in] mask-
 *  pointer to masking structure
 * \param [out] crc_p-
 *  pointer to 16 bit output target
 * \return
 *   * 16 bit output
 * \remark
 *   * None
 * \see
 *  * None
 */

typedef struct
{
    uint8 msb_mask[OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE];
    uint32 lsbyte_mask[BITS2WORDS(OAM_OAMP_CRC_MASK_LSB_BYTE_SIZE)];
} oam_oamp_crc_mask_t;

int dnx_oam_oamp_crc16(
    int unit,
    const uint8 *buff,
    uint32 start_index,
    uint32 data_length,
    uint16 *calculated_crc);

#endif /* OAM_OAMP_UTILS_H_INCLUDED */
