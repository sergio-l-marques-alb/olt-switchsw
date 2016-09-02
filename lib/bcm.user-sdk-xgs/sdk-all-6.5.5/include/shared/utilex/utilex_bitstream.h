/*! \file utilex_bitstream.h 
 * 
 * All common utilities related to bitstream. 
 *  
 */
/*
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

#ifndef UTILEX_BITSTREAM_H_INCLUDED
/* { */
#define UTILEX_BITSTREAM_H_INCLUDED

/*************
* INCLUDES  *
*************/

#include <shared/shrextend/shrextend_debug.h>

/*
 * In 'utilex_bitstream_set_field'/'utilex_bitstream_get_field':
 * 'nof_bits' can be at most 32.
*/
#define UTILEX_BIT_STREAM_FIELD_SET_SIZE    (SAL_UINT32_NOF_BITS)

/*!
 * \brief
 * Clears an array of bits composed of elements of 32-bits each (bit stream)
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size -
 *     Array length (number of 32-bits elements)
 * \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *bit_stream  - \n
 *   Pointer to array of uint32s which is zeroed
 */
shr_error_e utilex_bitstream_clear(
  uint32 * bit_stream,
  uint32 size);
/*!
 * \brief
 * Fills a bit stream (array of uint32s) by '1's
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size -
 *     Array length (number of 32-bits elements)
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *bit_stream  - \n
 *   Pointer to array of uint32s which is filled with '1's
 */
shr_error_e utilex_bitstream_fill(
  uint32 * bit_stream,
  uint32 size);

/*!
 * \brief
 * Set/clear 1 bit within the bit steream (uint32 array) as per 'bit_indicator'
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] place -
 *     Location of bit to load (counted in bits). BIT0 (LS bit) is at
 *     location '0', BIT1 (next bit) is at location '1',...,
 *     BIT32 is at at location '0' of the second uint32.
 *   \param [in] bit_indicator -
 *     Indicator (0 or non-zero) to the value to load into
 *     specified bit.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *bit_stream  -
 *   Bit loaded at the specified location with specified value.
 */
shr_error_e utilex_bitstream_set(
  uint32 * bit_stream,
  uint32 place,
  uint32 bit_indicator);

/*!
 * \brief
 * set field onto the bitstream
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] start_bit -
 *     The first bit number to set (the next one is 'start_bit+1'). See
 *     'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] nof_bits -
 *     Number of bits to set in 'bit_stream' and to take from 'field'.
 *     Range 0:32.
 *   \param [in] field  -
 *     Field to set. The 'nof_bits' LS bits are used as field image
 *     and are copied into the bit stream.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *bit_stream  -
 *     pointer to an array of uint32s,
 *     which is loaded by the bit field contained in 'field'
 * \remark
 *  Example:
 * \code{.c}
 *  {
 *    uint32
 *      bit_stream[2];
 *
 *    utilex_bitstream_clear(bit_stream, 2); ==> bit_stream = {0x0, 0x0}
 *    utilex_bitstream_set_field(bit_stream, 4, 16, 0x12345); ==> bit_stream = {0x0, 0x23450}
 *  }
 * \endcode
 */
shr_error_e utilex_bitstream_set_field(
  uint32 * bit_stream,
  uint32 start_bit,
  uint32 nof_bits,
  uint32 field);

/*!
 * \brief
 * Get field (some number of consequent bits) from the bitstream (array of uints32s)
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] start_bit -
 *     The first bit number to get (the next one is 'start_bit+1')
 *   \param [in] nof_bits -
 *     Number of bits to get from 'bit_stream' and to take from 'field'.
 *     Range 0:32.
 *   \param [in] field -
 *     Pointer to memory location to load with value from the bit stream.
 *     Points to one 'uint32'.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *field -
 *     Loaded with value from the bit stream.
 * \remark
 *   Example:
 * \code{.c}
 *  {
 *    uint32
 *      bit_stream[2],
 *      field;
 *
 *    bit_stream[0] = 0x23450;
 *    bit_stream[1] = 0x0;   ==> bit_stream = {0x0, 0x23450}
 *    utilex_bitstream_get_field(bit_stream, 8, 12, &field); ==> field is  0x234
 *  }
 * \endcode
 */
shr_error_e utilex_bitstream_get_field(
  uint32 * bit_stream,
  uint32 start_bit,
  uint32 nof_bits,
  uint32 * field);

/*!
 * \brief
 * Set 1 bit in pointed uint32 array to 1
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] place         -
 *     The number of the bit to set (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *bit_stream  -
 *     Updated bit on this uint32s array
 */
shr_error_e utilex_bitstream_set_bit(
  uint32 * bit_stream,
  uint32 place);

/*!
 * \brief
 *  set range of bits in the uint32 array to 1
 *  The range is [start_place -- end_place]
 *  INCLUDING end_place.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] start_place  -
 *     The start bit number to set (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] end_place    -
 *     The end bit number to set (counted in bits)
 *     This bit is also get set.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *bit_stream  -
 *   bits from 'start place' to end place are set to 1
 */
shr_error_e utilex_bitstream_set_bit_range(
  uint32 * bit_stream,
  uint32 start_place,
  uint32 end_place);

/*!
 * \brief
 *  Set 1 bit in the bitstream (uint32 array) to 0
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] place         -
 *     The bit number to reset (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *bit_stream  -
 *   The specified bit ('place') is reset
 */
shr_error_e utilex_bitstream_reset_bit(
  uint32 * bit_stream,
  uint32 place);

/*!
 * \brief
 *  Set range of bits in the uint32 array to 0
 *  The range is [start_place -- end_place]
 *  INCLUDING end_place.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] start_place  -
 *     The start bit number to reset (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] end_place    -
 *     The end bit number to reset (counted in bits)
 *     This bit is also reset.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *  *bit_stream  -
 *  Bits from 'start_place' to 'end_place' are reset
 */
shr_error_e utilex_bitstream_reset_bit_range(
  uint32 * bit_stream,
  uint32 start_place,
  uint32 end_place);

/*!
 * \brief
 *  Return the value (0/1) of 1 bit in the bitstream  (uint32 array)
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] place         -
 *     The bit number to fetch (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par DIRECT OUTPUT:
 *   int - bit value (0 / 1)
 */
int utilex_bitstream_test_bit(
  uint32 * bit_stream,
  uint32 place);

/*!
 * \brief
 *  Read the bit and auto-clear it.
 *  1. Return the value (0/1) of 1 bit in the uint32 array
 *  2. Set the bit to zero.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] place         -
 *     The bit number to fetch and reset (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par DIRECT OUTPUT:
 *   Bit value (0 / 1)
 * \par INDIRECT OUTPUT:
 *   *bit_stream  -
 *   Bit in 'place' is set to '0'.
 */
int utilex_bitstream_test_and_reset_bit(
  uint32 * bit_stream,
  uint32 place);

/*!
 * \brief
 *  Read the bit and auto-set it.
 *  1. Return the value (0/1) of 1 bit in the uint32 array
 *  2. Set the bit to one.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] place         -
 *     The bit number to fetch and set (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par DIRECT OUTPUT:
 *   Bit value (0 / 1)
 * \par INDIRECT OUTPUT
 *   *bit_stream  -
 *   Bit in 'place' is set to '1'.
 */
int utilex_bitstream_test_and_set_bit(
  uint32 * bit_stream,
  uint32 place);

/*!
 * \brief
 *  Flip the value of specific bit (XOR with one)
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] place         -
 *     The bit number to set (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   *bit_stream  -
 *   Bit in 'place' is flipped (XOR with one).
 */
shr_error_e utilex_bitstream_bit_flip(
  uint32 * bit_stream,
  uint32 place);

/*!
 * \brief
 *  Test whether specified range contains '1' or all bits are '0'.
 *  The range is [start_place -- end_place]
 *  INCLUDING end_place.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] start_place  -
 *     The start bit number (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] end_place    -
 *     The end bit number (counted in bits)
 *     This bit is also checked.
 * \par DIRECT OUTPUT:
 *   TRUE if 1 was found within the bit range
 * \par INDIRECT OUTPUT
 *   None
 * \sa utilex_bitstream_test_bit
 *     utilex_bitstream_have_one
 */
int utilex_bitstream_have_one_in_range(
  uint32 * bit_stream,
  uint32 start_place,
  uint32 end_place);

/*!
 * \brief
 *  Test whether any bit is set in the whole bitstream.
 *  (work faster than utilex_bitstream_have_one_in_range())
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in bitstream array
 * \par DIRECT OUTPUT:
 *   TRUE if 1 was found within the bit stream
 * \sa utilex_bitstream_have_one_in_range
 */
int utilex_bitstream_have_one(
  uint32 * bit_stream,
  uint32 size                   /* in uint32s */
  );

/*!
 * \brief
 *  'OR' 2 bitstreams with each other.
 *  The results goes into the first bit stream.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream1  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream.
 *     Result goes into this bit stream.
 *   \param [in] *bit_stream2  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in each of the bit streams
 * \par INDIRECT INPUT
 *   * *bit_stream1
 *   * *bit_stream2 \n
 *   The two bit streams to OR with each other.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   *bit_stream1  -
 *   Output of OR operation goes into this bit stream
 */
shr_error_e utilex_bitstream_or(
  uint32 * bit_stream1,
  uint32 * bit_stream2,
  uint32 size                   /* in uint32s */
  );

/*!
 * \brief
 *  AND 2 bitstreams. Put the result in the first.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream1  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *     Result goes into this bit stream.
 *   \param [in] *bit_stream2  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in each of the bit streams
 * \par INDIRECT INPUT
 *   * *bit_stream1
 *   * *bit_stream2 \n
 *   The two bit streams to AND with each other.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   *bit_stream1  -
 *   Output of AND operation goes into this bit stream
 */
shr_error_e utilex_bitstream_and(
  uint32 * bit_stream1,
  uint32 * bit_stream2,
  uint32 size                   /* in uint32s */
  );

/*!
 * \brief
 *  XOR 2 bitstreams. Put the result in the first.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream1  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *     Result goes into this bit stream.
 *   \param [in] *bit_stream2  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in each of the bit streams
 * \par INDIRECT INPUT
 *   * *bit_stream1
 *   * *bit_stream2 \n
 *   The two bit streams to XOR with each other.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   *bit_stream1  -
 *   Output of XOR operation goes into this bit stream
 */
shr_error_e utilex_bitstream_xor(
  uint32 * bit_stream1,
  uint32 * bit_stream2,
  uint32 size                   /* in uint32s */
  );

/*!
 * \brief
 *  Get the parity of a bitstream (array of uint32s)
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] start_bit -
 *     Specifies the bit from which parity is calculated.
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] nof_bits -
 *     Specifies the number of bits from 'start_bit' on
 *     which parity is calculated.
 * \par DIRECT OUTPUT:
 *    0 - numbers of 1 in the stream is even.
 *    1 - numbers of 1 in the stream is odd.
 */
uint32 utilex_bitstream_parity(
  uint32 * bit_stream,
  uint32 start_bit,
  uint32 nof_bits);

/*!
 * \brief
 *  Do 'NOT' operation on the bitstream (array of uints)
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in the bit streams
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   *bit_stream  -
 *   Result off NOT operation goes into this bitstream
 */
shr_error_e utilex_bitstream_not(
  uint32 * bit_stream,
  uint32 size                   /* in uint32s */
  );

/*!
 * \brief
 *  Count the number of "ones" in the bit stream (array of uint32s).
 *  Examples:
 *   + 0x0000_0000 ==> will results with 0.
 *   + 0x0100_0000 ==> will results with 1.
 *   + 0x0100_0000 0x0C00_0000 ==> will results with 3.
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in the bit streams
 * \par DIRECT OUTPUT:
 *   The number of ones on the bitstream
 * \par INDIRECT OUTPUT:
 *   None
 */
uint32 utilex_bitstream_get_nof_on_bits(
  uint32 * bit_stream,
  uint32 size                   /* in uint32s */
  );

/*!
 * \brief
 *  Test function of utilex_bitstream.
 *  View prints and return value to indicate pass/fail
 * 
 * \par DIRECT INPUT
 *   \param [in] silent  -
 *     Print bitstreams if not silent
 * \par DIRECT OUTPUT:
 *   int- \n
 *     * TRUE  - PASS
 *     * FALSE - FAIL  
 */
uint32 utilex_bitstream_offline_test(
  uint32 silent);

/*!
 * \brief
 *  Printing utility. Print 'size' unit32s from input bitstream
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in the bit stream
 * \par INDIRECT OUTPUT:
 *   Printed bitstream
 * \remark
 *   Use LOG_CLI for printing
 */
void utilex_bitstream_print(
  uint32 * bit_stream,
  uint32 size                   /* in uint32s */
  );

/*!
 * \brief
 *  Having two buffers, this function writes the second buffer,
 *  starting from specified offset, the specified number of bits,
 *  taken from the first buffer starting (on the first buffer) from
 *  BIT0.
 * 
 * \par DIRECT INPUT
 *   \param [in] *input_buffer  -
 *     Input buffer from which the function reads, starting at
 *     BIT0
 *   \param [in] start_bit
 *     The number of the first bit to start the writing
 *     into the output buffer
 *   \param [in] nof_bits
 *     Number of bits to read / write
 *   \param [in] *output_buffer  -
 *     Output buffer into which the function writes.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   *output_buffer \n
 *     Updated output buffer. NOTE that writing starts at bit
 *     'start_bit' of 'output_buffer'
 * \remark
 *   Example:
 * \code{.c}
 *  {
 *    shr_error_e error_e ;
 *    uint32 input_buffer[2] ;
 *    uint32 output_buffer[2] ;
 *    uint32 start_bit, nof_bits ;
 *
 *    start_bit = 8 ;
 *    nof_bits = 40 ;
 *    input_buffer[0] = 0xAABBCCDD ;
 *    input_buffer[1] = 0xEEFF0011 ;
 *    output_buffer[0] = 0xFFFFFFFF ;
 *    output_buffer[1] = 0xFFFFFFFF ;
 *    error_e = 
 *      utilex_bitstream_set_any_field(input_buffer,start_bit,nof_bits,output_buffer) ;
 *    ==> At this point, input_buffer remains as it was (0xEEFF0011AABBCCDD) while
 *    ==> output_buffer now contains 0xFFFF11AABBCCDDFF
 *  }
 * \endcode
 */
shr_error_e utilex_bitstream_set_any_field(
  uint32 * input_buffer,
  uint32 start_bit,
  uint32 nof_bits,
  uint32 * output_buffer);

/*!
 * \brief
 *  Having two buffers, the function reads the first buffer
 *  from specified offset and writes the data into the second buffer
 *  starting at BIT0. 
 *  The number of bits read from first buffer equals the number of
 *  bits written into the second buffer.
 *  If some bits are written in part of a uint32 element (on the
 *  second buffer) then this function zeroes all other bits
 *  (from the end of the field to the end of the uint32 element)
 * 
 * \par DIRECT INPUT
 *   \param [in] *input_buffer  -
 *     Input buffer from which the function reads, starting
 *     from bit 'start_bit'.
 *   \param [in] start_bit
 *     The first bit to start the reading in the input buffer
 *   \param [in] nof_bits
 *     Number of bits to read / write
 *   \param [in] *output_buffer
 *     Pointer to output buffer into which the function writes
 * \par DIRECT OUTPUT:
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *output_buffer \n
 *   Modified output buffer. Extracted data is written into
 *   output buffer starting from BIT0. Note that if the number
 *   of bits is not a multiple of 32, then the last uint32
 *   will be filled with zeroes on the bits which have not been
 *   copied from 'input_buffer.
 * \remark
 *   Example:
 * \code{.c}
 *  {
 *    shr_error_e error_e ;
 *    uint32 input_buffer[3] ;
 *    uint32 output_buffer[3] ;
 *    uint32 start_bit, nof_bits ;
 *
 *    start_bit = 8 ;
 *    nof_bits = 40 ;
 *    input_buffer[0] = 0xAABBCCDD ;
 *    input_buffer[1] = 0xEEFF1122 ;
 *    input_buffer[2] = 0x33445566 ;
 *    output_buffer[0] = 0xFFFFFFFF ;
 *    output_buffer[1] = 0xFFFFFFFF ;
 *    output_buffer[2] = 0xFFFFFFFF ;
 *    error_e =
 *      utilex_bitstream_get_any_field(input_buffer,start_bit,nof_bits,output_buffer) ;
 *    ==> At this point, input_buffer remains as it was (0x33445566EEFF1122AABBCCDD) while
 *    ==> output_buffer now contains 0xFFFFFFFF0000001122AABBCC
 *  }
 * \endcode
 */
shr_error_e utilex_bitstream_get_any_field(
  uint32 * input_buffer,
  uint32 start_bit,
  uint32 nof_bits,
  uint32 * output_buffer);

/*!
 * \brief
 * Beautify printing utility.
 *  Prints the bit-stream as a GROUP of one-bit items. Each
 *  bit which is non-zero is marked by its location in the
 *  bitstream. The space assigned to display of each bit is
 *  a parameter (max_dec_digits) and the maxiaml number of bits
 *  to display is also a parameter.
 * 
 * \par DIRECT INPUT
 *   \param [in] *bit_stream  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in the bit stream
 *   \param [in] max_dec_digits -
 *     Number of spaces assigned for each non-zero bit
 *     which is displayed.
 *   \param [in] max_nof_printed_items -
 *     Maximum number of items (non-zero bits) to print.
 * \par INDIRECT OUTPUT:
 *   Printed bitstream
 * \remark
 *   This procedures uses LOG_CLI for printing \n
 *   For example: \n
 *   If the bitstream is : \c 0x7FFDEB40EEFF0011AABBCCDD \n
 *   and 'max_dec_digits' is '3' and 'max_nof_printed_items' is '30' \n
 *   then the printout will be: \n
 * \code{.c}
 *     Out of 96, 57 are ON (57 item exists).
 *     [ 0   2   3   4   6   7   10  11  
 *       14  15  16  17  19  20  21  23  
 *       25  27  29  31  32  36  48  49  
 *       50  51  52  53  54  55   .... More Items to print. Exist .... 
 *     ]
 * \endcode
 */
void utilex_bitstream_print_beautify_1(
  uint32 * bit_stream,
  uint32 size,                  /* in uint32s */
  uint32 max_dec_digits,
  uint32 max_nof_printed_items);

/*!
 * \brief
 *  Beautify printing utility.
 *  Prints buffer, which is assumed to be an array of uint32s,
 *  in hex format so that there are specified elements per
 *  line (nof_columns). 
 * 
 * \par DIRECT INPUT
 *   \param [in] *buff  -
 *     Pointer to an array of uint32s,
 *     which function as the bit stream
 *   \param [in] buff_size   -
 *     Number of bytes in the bit stream. If
 *     it is not a multiple of sizeof(uint32) then
 *     the last uint32 is only partially displayed.
 *   \param [in] nof_columns -
 *     Item max printing size in Decimal.
 * \remark
 *   This procedures uses LOG_CLI for printing \n
 *   For example:                                                 \n
 *   If 'buff_size' is 18 and 'nof_columns' is 4 and the          \n
 *   bitstream is : \c 0xA55A0022CCDDEEFF8899AABB4455667700112233 \n
 * \code{.c}
 *   Buffer of 18 Bytes, values given in hexa:
 *   -----------------------------------------
 *   OFFSET LONGS:      0 00112233 44556677 8899AABB CCDDEEFF
 *   OFFSET LONGS:      4 A55A
 * \endcode
 */
shr_error_e utilex_bitstream_buff_print_all(
  unsigned char *buff,
  uint32 buff_size,
  uint32 nof_columns);

/*!
 * \brief
 *  Having two char buffers, orders msB->lsB, the function reads
 *  the first buffer from offset (offset taken from msb) and writes
 *  it to the second buffer.
 * 
 * \par DIRECT INPUT
 *   \param [in] unit  -
 *   \param [in] *input_buffer  -
 *       pointer to an array of uint32s,
 *       which function as the bit stream
 *   \param [in] start_bit_msb   -
 *       offset
 *   \param [in] nof_bits - 
 *  \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 *  \par INDIRECT OUTPUT:
 *   \param [out] output_value 
 */
shr_error_e utilex_bitstream_u8_ms_byte_first_get_field(
  int unit,
  uint8 * input_buffer,
  uint32 start_bit_msb,
  uint32 nof_bits,
  uint32 * output_value);

/* } UTILEX_BITSTREAM_H_INCLUDED*/
#endif
