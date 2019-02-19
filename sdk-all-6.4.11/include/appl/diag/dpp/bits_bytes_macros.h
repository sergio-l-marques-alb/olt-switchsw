/* $Id$
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
#ifndef BITS_BYTES_MACROS_INCLUDED
/* { */
#define BITS_BYTES_MACROS_INCLUDED



#define BITS_IN_BYTE    8
#define BITS_IN_LONG    32


#ifndef __KERNEL__ /* for kernel, already defined in bitops.h */
#define BIT(x)          (1<<(x))
#endif
#define RBIT(x)         (~(1<<(x)))

#define VAL_MIN(x,y) ( (x)<(y)? (x) : (y) )

/*******************************************
 * SET ALL BITS, BEGINNING AT BIT X, TO 1  *
 *******************************************/
#define FROM_BIT(x)     (RBIT(x) + 1)

/*******************************************
 * SET ALL BITS UP TO BIT X (NOT INCLUDING *
 * BIT X) TO 1.                            *
 *******************************************/
#define UPTO_BIT(x)     (~(FROM_BIT(x)))

/***************************************************
* MACROS FOR ACCESSING SPECIFIC BYTES              *
****************************************************/

/***************************************************
 * MACROS TO COMPUTE THE UI BITS              *
 ****************************************************/
/* All the bits up until bit x with a verification if x > 31 */
#define UNTIL_BIT(x)               (((x)>= (BITS_IN_LONG-1))? 0xffffffff : ((unsigned  long) (1<<((VAL_MIN((x)%BITS_IN_LONG, 30))+1))-1))
/* All the bits from y to x */
#define BIT_UP_RANGE(x,y)          (UNTIL_BIT(x)-UNTIL_BIT(y))

/* For the UI, the right output in the bit entry for a global number num */
#define BIT_N(num, bit) \
  ((num >= (BITS_IN_LONG * (bit+1))) || (num < (BITS_IN_LONG * bit))) ? 0 : BIT(num % BITS_IN_LONG) 

/* The 16-entry array for the UI function or parameter*/
#define BIT_UI_16(num) \
 {BIT_N(num, 0), BIT_N(num, 1), BIT_N(num, 2), BIT_N(num, 3), BIT_N(num, 4), BIT_N(num, 5), BIT_N(num, 6), BIT_N(num, 7), BIT_N(num, 8), BIT_N(num, 9), BIT_N(num, 10), BIT_N(num, 11), BIT_N(num, 12), BIT_N(num, 13), BIT_N(num, 14), BIT_N(num, 15)}

#define BIT_N_RNG(nmin, nmax, bit) \
  (nmax < (BITS_IN_LONG * bit)) ? 0 : ((nmin >= (BITS_IN_LONG * (bit+1))) ? 0 : ((nmin < (BITS_IN_LONG * bit))? UNTIL_BIT((nmax>=(BITS_IN_LONG * (bit+1)))? BITS_IN_LONG:(nmax%BITS_IN_LONG)) : BIT_UP_RANGE(((nmax>=(BITS_IN_LONG * (bit+1)))? BITS_IN_LONG:(nmax%BITS_IN_LONG)),(nmin%BITS_IN_LONG))))

#define BIT_UI_16_RNG(nmin, nmax) \
  {BIT_N_RNG(nmin, nmax, 0), BIT_N_RNG(nmin, nmax, 1), BIT_N_RNG(nmin, nmax, 2), BIT_N_RNG(nmin, nmax, 3), BIT_N_RNG(nmin, nmax, 4), BIT_N_RNG(nmin, nmax, 5), BIT_N_RNG(nmin, nmax, 6), BIT_N_RNG(nmin, nmax, 7), BIT_N_RNG(nmin, nmax, 8), BIT_N_RNG(nmin, nmax, 9), BIT_N_RNG(nmin, nmax, 10), BIT_N_RNG(nmin, nmax, 11), BIT_N_RNG(nmin, nmax, 12), BIT_N_RNG(nmin, nmax, 13), BIT_N_RNG(nmin, nmax, 14), BIT_N_RNG(nmin, nmax, 15)}

/**************************************************************
 * MACRO TO GET THE OFFSET OF AN ELEMENT IN A                 *
 * GIVEN STRUCTURE TYPE.                                      *
 * 'X' IS THE TYPE OF THE STRUCTURE.                          *
 * 'Y' IS THE THE ELEMENT (IN THE STRUCTURE) WHOSE            *
 *     OFFSET WE NEED.                                        *
 * THE OFFSET RETURNS AS 'unsigned long'.                     *
 **************************************************************/
#define OFFSETOF(x,y) ((unsigned long)(&(((x *)0)->y)))
/*
 * Macros related to handling of 32 bits registers.
 * {
 */
/*
 * Take value and put it in its proper location within a 'long'
 * register (and make sure it does not effect bits outside its
 * predefined mask).
 */
/*
 * Take value and add it in its proper location within a 'long'
 * register ('target') and make sure it does not effect bits outside its
 * predefined mask.
 */
/*
 * Get a value out of location within a 'long' register (and make sure it
 * is not effected by bits outside its predefined mask).
 */
/*
 * }
 */
/**************************************************************
 * MACRO TO GET THE SIZE OF AN ELEMENT IN A                   *
 * GIVEN STRUCTURE TYPE.                                      *
 * 'X' IS THE TYPE OF THE STRUCTURE.                          *
 * 'Y' IS THE  ELEMENT (IN THE STRUCTURE) WHOSE               *
 *     SIZE (IN BYTES) WE NEED.                               *
 * THE SIZE RETURNS AS 'unsigned long'.                       *
 **************************************************************/
#define SIZEOF(x,y) ((unsigned long)(sizeof(((x *)0)->y)))
/**************************************************************
 * MACRO TO CONVERT BIG ENDIAN LONG VARIABLE TO LITTLE ENDIAN *
 * 'X' IS THE INPUT VARIABLE.                                 *
 * 'Y' IS THE OUTPUT VARIABLE                                 *
 **************************************************************/

/**************************************************************
 * MACRO TO CONVERT littel ENDIAN LONG VARIABLE TO BIG ENDIAN *
 * 'X' IS THE INPUT VARIABLE.                                 *
 * 'Y' IS THE OUTPUT VARIABLE                                 *
 **************************************************************/

/* } */
#endif


