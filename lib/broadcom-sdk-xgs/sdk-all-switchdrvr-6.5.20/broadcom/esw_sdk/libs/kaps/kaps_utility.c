/*
 **************************************************************************************
 Copyright 2009-2019 Broadcom Corporation

 This program is the proprietary software of Broadcom Corporation and/or its licensors,
 and may only be used, duplicated, modified or distributed pursuant to the terms and
 conditions of a separate, written license agreement executed between you and
 Broadcom (an "Authorized License").Except as set forth in an Authorized License,
 Broadcom grants no license (express or implied),right to use, or waiver of any kind
 with respect to the Software, and Broadcom expressly reserves all rights in and to
 the Software and all intellectual property rights therein.
 IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization, constitutes the
    valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
    protect the confidentiality thereof,and to use this information only in connection
    with your use of Broadcom integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
    ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
    EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
    SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
    NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
    BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
    WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
    THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
    OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
    ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 **************************************************************************************
 */

#include "kaps_utility.h"
#include "kaps_portable.h"

void
kaps_convert_dm_to_xy(
    uint8_t * data_d,
    uint8_t * data_m,
    uint8_t * data_x,
    uint8_t * data_y,
    uint32_t dataLenInBytes)
{
    uint32_t i;

    for (i = 0; i < dataLenInBytes; i++)
    {
        data_x[i] = data_d[i] & (~data_m[i]);
        data_y[i] = (~data_d[i]) & (~data_m[i]);
    }
}

void
kaps_convert_xy_to_dm(
    uint8_t * data_x,
    uint8_t * data_y,
    uint8_t * data_d,
    uint8_t * data_m,
    uint32_t dataLenInBytes)
{
    uint32_t i;

    for (i = 0; i < dataLenInBytes; ++i)
    {
        data_d[i] = data_x[i] & (~data_y[i]);
        data_m[i] = (~data_x[i]) & (~data_y[i]);
    }
}

static void
lpt_encode(
    uint8_t i_x,
    uint8_t i_y,
    uint8_t * o_x,
    uint8_t * o_y)
{
    uint32_t i, index;
    uint8_t x[4], y[4];

    for (i = 0; i < 4; i++)
    {
        index = i * 2;
        x[i] = (i_x & (0x3 << index)) >> index;
        y[i] = (i_y & (0x3 << index)) >> index;
    }

    for (i = 0; i < 4; i++)
    {
        if (x[i] == 0 && y[i] == 3)
        {
            x[i] = 2;
            y[i] = 3;
        }
        else if (x[i] == 1 && y[i] == 2)
        {
            x[i] = 1;
            y[i] = 3;
        }
        else if (x[i] == 2 && y[i] == 1)
        {
            x[i] = 3;
            y[i] = 1;
        }
        else if (x[i] == 3 && y[i] == 0)
        {
            x[i] = 3;
            y[i] = 2;
        }
        else if (x[i] == 0 && y[i] == 2)
        {
            x[i] = 0;
            y[i] = 3;
        }
        else if (x[i] == 2 && y[i] == 0)
        {
            x[i] = 3;
            y[i] = 0;
        }
        else if (x[i] == 0 && y[i] == 1)
        {
            x[i] = 2;
            y[i] = 1;
        }
        else if (x[i] == 1 && y[i] == 0)
        {
            x[i] = 1;
            y[i] = 2;
        }
        else if (x[i] == 0 && y[i] == 0)
        {
            x[i] = 0;
            y[i] = 0;
        }
        else
        {
            index = i * 2;
            x[i] = (i_x & (0x3 << index)) >> index;
            y[i] = (i_y & (0x3 << index)) >> index;
        }
    }

    *o_x = x[3] << 6 | x[2] << 4 | x[1] << 2 | x[0];
    *o_y = y[3] << 6 | y[2] << 4 | y[1] << 2 | y[0];
}

void
kaps_lpt_encode(
    uint8_t * x,
    uint8_t * y,
    uint32_t length)
{
    uint32_t i;
    uint8_t l_x, l_y;

    /*
     * Converting XY format to LPT encoded format 
     */
    for (i = 0; i < length; i++)
    {
        lpt_encode(x[i], y[i], &l_x, &l_y);
        x[i] = l_x;
        y[i] = l_y;
    }
}

uint32_t
KapsReadBitsInArrray(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos)
{
    uint32_t value = 0;
    uint32_t len;
    uint32_t curr_bit_pos, curr_byte_pos;
    uint32_t mask_data = (uint32_t) ~ 0;
    int32_t counter = 0, total_bits = 0;

    curr_byte_pos = ArrSize - 1 - (EndPos >> 3);
    curr_bit_pos = 7 - (EndPos & 0x7);

    len = EndPos - StartPos + 1;
    counter = len + curr_bit_pos;
    if (len != 32)
        mask_data = ~(~0 << len);
    while (counter > 0 && total_bits < 32)
    {
        value = (value << 8) | (Arr[curr_byte_pos++]);
        counter -= 8;
        total_bits += 8;
    }

    if (len + curr_bit_pos > 32)
    {
        value <<= 8 - (StartPos & 0x7);
        value |= ((Arr[curr_byte_pos] >> (StartPos & 0x7)) & 0xFF);
    }
    else
    {
        value = value >> (StartPos & 0x7);
    }

    return (value & mask_data);
}

/*
              ---------------------------------------------------
Byte index   |  0  |  1  |  2  | ... |     |     |     |ArrSize-1|
of Arr        ---------------------------------------------------
                             |Value (up to 32 bits)|
                              ---------------------
                             ^                     ^
                             |                     |
                           EndPos               StartPos
                        (endByteIdx)         (startByteIdx)
*/

void
KapsWriteBitsInArray(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos,
    uint32_t Value)
{
    uint32_t startByteIdx;
    uint32_t endByteIdx;
    uint32_t byte;
    uint32_t len;
    uint8_t maskValue;

    kaps_assert((EndPos >= StartPos), "endbit cannot be less than startbit");
    kaps_assert((EndPos < (ArrSize << 3)), "endBit exceeds the arr size");

    len = EndPos - StartPos + 1;
    kaps_assert((len <= 32), "Can not write more than 32-bit at a time !");
    /*
     * Value is unsigned 32 bit variable, so it can not be greater than ~0. 
     */
    if (len != 32)
    {
        kaps_assert(Value <= (uint32_t) (~(~0 << len)), "Value is too big to write in the bit field!");
    }

    startByteIdx = ArrSize - ((StartPos >> 3) + 1);
    endByteIdx = ArrSize - ((EndPos >> 3) + 1);

    if (startByteIdx == endByteIdx)
    {
        maskValue = (uint8_t) (0xFE << ((EndPos & 7)));
        maskValue |= ((1 << (StartPos & 7)) - 1);
        Arr[startByteIdx] &= maskValue;
        Arr[startByteIdx] |= (uint8_t) ((Value << (StartPos & 7)));
        return;
    }
    if (StartPos & 7)
    {
        maskValue = (uint8_t) ((1 << (StartPos & 7)) - 1);
        Arr[startByteIdx] &= maskValue;
        Arr[startByteIdx] |= (uint8_t) ((Value << (StartPos & 7)));
        startByteIdx--;
        Value >>= (8 - (StartPos & 7));
    }
    for (byte = startByteIdx; byte > endByteIdx; byte--)
    {
        Arr[byte] = (uint8_t) (Value);
        Value >>= 8;
    }
    maskValue = (uint8_t) (0xFE << ((EndPos & 7)));
    Arr[byte] &= maskValue;
    Arr[byte] |= Value;
}

/* Fills ones in a column of an array where
startBit indicates start of the column and
endBit indicates end of the column; datalen
gives the size of array in terms of bytes;
Other bits in the array remain unchanged*/
void
kaps_FillOnes(
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit)
{
    uint32_t numOfBytes;
    uint32_t startByte;

    kaps_assert((data != NULL), "Null Data Pointer !");
    kaps_assert(((startBit <= (datalen << 3)) && endBit <= startBit), "Invalid start and ending position value!");

    startByte = ((startBit + 8) >> 3);
    numOfBytes = (((startBit - endBit) + 8) >> 3);

    if (numOfBytes == 1)
    {
        KapsWriteBitsInArray(data, datalen, startBit, endBit, (~(0xFFFFFFFF << ((startBit - endBit) + 1))));
        return;
    }

    if ((startBit & 0x7) != 7)
    {
        KapsWriteBitsInArray(&data[datalen - startByte], 1, (startBit & 0x7), 0, (~(0xFFFFFFFF << ((startBit + 1) & 0x7))));
        startByte--;
        numOfBytes--;
    }
    if (numOfBytes > 1)
    {
        kaps_memset(&data[datalen - startByte], 0xff, numOfBytes - 1);

        startByte = startByte + 1 - numOfBytes;
    }
    if (endBit < (startByte << 3))
    {
        KapsWriteBitsInArray(data, datalen, (startByte << 3) - 1, endBit, (~(0xFFFFFFFF << ((startByte << 3) - endBit))));
    }
}

/* Fills zeroes in a column of an array where
startBit indicates start of the column and
endBit indicates end of the column; datalen
gives the size of array in terms of bytes;
Other bits in the array remain unchanged*/
void
kaps_FillZeroes(
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit)
{
    uint32_t numOfBytes;
    uint32_t startByte;

    kaps_assert((data != NULL), "Null Data Pointer !");
    kaps_assert(((startBit <= (datalen << 3)) && endBit <= startBit), "Invalid start and ending position value!");

    startByte = (startBit + 8) >> 3;
    numOfBytes = ((startBit - endBit) + 8) >> 3;

    if (numOfBytes == 1)
    {
        KapsWriteBitsInArray(data, datalen, startBit, endBit, 0);
        return;
    }

    if ((startBit & 0x7) != 7)
    {
        KapsWriteBitsInArray(&data[datalen - startByte], 1, (startBit & 0x7), 0, 0);
        startByte--;
        numOfBytes--;
    }
    if (numOfBytes > 1)
    {
        kaps_memset(&data[datalen - startByte], 0x0, numOfBytes - 1);

        startByte = startByte + 1 - numOfBytes;
    }
    if (endBit < (startByte << 3))
    {
        KapsWriteBitsInArray(data, datalen, (startByte << 3) - 1, endBit, 0);
    }
}

/* Copies the data specified by array(data) to the column
in the array specified by array(o_data) where
startBit indicates start of the column and
endBit indicates end of the column; datalen
gives the size of array in terms of bytes;
Other bits in the array(o_data) remain unchanged */
void
kaps_CopyData(
    uint8_t * o_data,
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit)
{
    uint32_t numOfBytes;
    uint32_t startByte;

    kaps_assert((o_data != NULL), "Null Output Data Pointer !");
    kaps_assert((data != NULL), "Null Data Pointer !");
    kaps_assert(((startBit <= (datalen << 3)) && endBit <= startBit), "Invalid start and ending position value!");

    startByte = (startBit + 8) >> 3;
    numOfBytes = ((startBit - endBit) + 8) >> 3;

    if (numOfBytes == 1)
    {
        KapsWriteBitsInArray(o_data, datalen, startBit, endBit, KapsReadBitsInArrray(data, datalen, startBit, endBit));
        return;
    }

    if ((startBit & 0x7) != 7)
    {
        KapsWriteBitsInArray(&o_data[datalen - startByte], 1, (startBit & 0x7), 0,
                         KapsReadBitsInArrray(&data[datalen - startByte], 1, (startBit & 0x7), 0));
        startByte--;
        numOfBytes--;
    }

    if (numOfBytes > 1)
    {
        kaps_memcpy(&o_data[datalen - startByte], &data[datalen - startByte], numOfBytes - 1);

        startByte = startByte + 1 - numOfBytes;
    }
    if (endBit < (startByte << 3))
    {
        KapsWriteBitsInArray(o_data, datalen, (startByte << 3) - 1, endBit,
                         KapsReadBitsInArrray(data, datalen, (startByte << 3) - 1, endBit));
    }
}
