#ifndef _SAND_TYPES_H_
#define _SAND_TYPES_H_
/* --------------------------------------------------------------------------
**
** $Id: sbTypes.h,v 1.16 Broadcom SDK $
**
** $Copyright: Copyright 2012 Broadcom Corporation.
** This program is the proprietary software of Broadcom Corporation
** and/or its licensors, and may only be used, duplicated, modified
** or distributed pursuant to the terms and conditions of a separate,
** written license agreement executed between you and Broadcom
** (an "Authorized License").  Except as set forth in an Authorized
** License, Broadcom grants no license (express or implied), right
** to use, or waiver of any kind with respect to the Software, and
** Broadcom expressly reserves all rights in and to the Software
** and all intellectual property rights therein.  IF YOU HAVE
** NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
** IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
** ALL USE OF THE SOFTWARE.  
**  
** Except as expressly set forth in the Authorized License,
**  
** 1.     This program, including its structure, sequence and organization,
** constitutes the valuable trade secrets of Broadcom, and you shall use
** all reasonable efforts to protect the confidentiality thereof,
** and to use this information only in connection with your use of
** Broadcom integrated circuit products.
**  
** 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
** PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
** REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
** OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
** DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
** NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
** ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
** CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
** OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
** 
** 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
** BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
** INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
** ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
** TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
** THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
** WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
** ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
**
** sbTypes.h: basic types on which all Sandburst code depends
**
** --------------------------------------------------------------------------*/
#ifdef BCM_SBX_SUPPORT
#include <soc/sbx/sbTypesGlue.h>
#endif

#if defined(SAND_CHIP_USER) || defined(SAND_CHIP_USER_PATH)
#ifndef SAND_CHIP_USER_PATH
#define SAND_CHIP_USER_PATH <chip_user.h>
#endif
#include SAND_CHIP_USER_PATH
#endif

#ifndef SAND_CHIP_USER_INT_TYPES
#if !defined(__NetBSD__)
#if !defined(__linux__)
#if !defined(__sun__) && !defined(VXWORKS)
#if !defined(__uint8_defined)
typedef unsigned char uint8;
#endif
#if !defined(__uint16_defined)
typedef unsigned short uint16;
#endif
#if !defined(__uint32_defined)
typedef unsigned int uint32;
#endif
#endif /* __!defined(__sun__) && !defined(__VXWORKS__) */
#if !defined(__uint64_defined)
#ifndef VXWORKS
typedef unsigned long long uint64;
#endif /* !defined(VXWORKS) */
#endif
#endif /* !defined(__linux__) */
#endif /* !defined(__NetBSD__) */
#if defined(__NetBSD__) && !defined(uint8)
#define uint8 u_int8
#define uint16 u_int16
#define uint32 u_int32
#define uint64 u_int64
#endif /* defined(__NetBSD__) && !defined(uint8) */

#if defined(__linux__) && !defined(uint8)
#define uint8 u_int8
#define uint16 u_int16
#define uint32 u_int32
#define uint64 u_int64
#endif /* defined(__linux__) && !defined(uint8) */

#ifdef VXWORKS
#include "types/vxTypesOld.h"
#else  /* VXWORKS */
#ifndef ZDT_64
typedef uint32 UINT;
#endif
#endif /* VXWORKS */
#ifndef uint
#define uint unsigned int
#endif /* uint */
#endif /* SAND_CHIP_USER_INT_TYPES */

#if defined(uint8)
#define __uint8_defined
#endif
#if defined(uint16)
#define __uint16_defined
#endif
#if defined(uint32)
#define __uint32_defined
#endif
#if defined(uint64)
#define __uint64_defined
#endif
#if defined(int8)
#define __int8_defined
#endif
#if defined(int16)
#define __int16_defined
#endif
#if defined(int32)
#define __int32_defined
#endif
#if defined(int64)
#define __int64_defined
#endif

#ifdef BE_HOST
#define SAND_BIG_ENDIAN_HOST
#endif

#ifdef SAND_BIG_ENDIAN_HOST
#define SAND_HOST_TO_FE_16(x) SAND_SWAP_16(x)
#define SAND_HOST_TO_FE_32(x) SAND_SWAP_32(x)
#define SAND_FE_TO_HOST_16(x) SAND_SWAP_16(x)
#define SAND_FE_TO_HOST_32(x) SAND_SWAP_32(x)
#else
#define SAND_HOST_TO_FE_16(x) (x)
#define SAND_HOST_TO_FE_32(x) (x)
#define SAND_FE_TO_HOST_16(x) (x)
#define SAND_FE_TO_HOST_32(x) (x)
#endif

#ifndef SAND_SWAP_16
#define sandSwap16(x) ((((uint16)(x) >> 8) & 0xff) | (((uint16)(x) & 0xff) << 8))
#define SAND_SWAP_16(x) sandSwap16((x))
#endif

#ifndef SAND_SWAP_32
#define sandSwap32(x) ((((uint32)(x) >> 24) & 0xff)  | (((uint32)(x) >> 8) & 0xff00) \
		       | (((uint32)(x) & 0xff00) << 8) | (((uint32)(x) & 0xff) << 24))
#define SAND_SWAP_32(x) sandSwap32((x))
#endif

#ifndef NULL
#define NULL 0
#endif

/**
 * When a MAC address is added, a client supplies a handle that identifies
 * the address later if other entries collide, or the entry becomes too old.
 */

/**
 * Sandburst Library Version Structure
 */
typedef struct sbSwLibVersion_s
{
    uint32 productCode;      /**< Sandburst Product Code */
    uint32 majorVersion;     /**< Major Version of the library */
    uint32 minorVersion;     /**< Minor Version of the library */
    uint32 patchLevel;       /**< Patch Level of the library */
    char     description[80];  /**< Description of the library */
} sbSwLibVersion_t, *sbSwLibVersion_p_t;

/**
 * Sandburst Microcode Version Structure
 */
typedef struct sbUcodeVersion_s
{
    uint32 productCode;            /**< Sandburst Product Code of parent library */
    uint32 libMajorVersion;        /**< Major Version of the parent library */
    uint32 libMinorVersion;        /**< Minor Version of the parent library */
    uint32 libPatchLevel;          /**< Patch Level of the parent library */
    uint32 ucodeMajorVersion;      /**< Major Version of the microcode */
    uint32 ucodeMinorVersion;      /**< Minor Version of the microcode */
    uint32 ucodePatchLevel;        /**< Patch Level of the microcode */
    char     packageDescription[80]; /**< Description of the library */
} sbUcodeVersion_t, *sbUcodeVersion_p_t;

#endif
