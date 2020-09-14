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

/*@@kaps_pfx_bundle Module

   Summary
   This module creates a prefix bundle factory.

   Description
   A prefix bundle is essentially the essential parts of a prefix (see <LINK kaps_prefix Module>),
   its index, and an optional associated data value. The size of the associated data is
   specified during contruction and should not be changed afterwards!!!

   A prefix bundle (kaps_pfx_bundle) is always associated with a storage factory. You cannot
   access the contents of the prefix-bundle with its associated factory. This module is
   designed assuming that a factory is used to create a variety of prefix-bundles.

   To store a 32 bit prefix with no associated data, using this factory, you would take
   (4 + 4) bytes rounded to 8 byte increments gives you 8 bytes of storage with almost no
   malloc overhead (usually an additional 8 bytes). ALSO READ DOCUMENTATION IN
   kaps_nlm_allocator FOR MORE DETAILS.

   If you do not use this factory then the storage needed is 16 bytes for a ptr to the prefix
   itself and an additional 12 bytes for the index/prefix/assoc data structure (assuming that
   you are not allocating individual bundles and putting the bundles in an array of structs
   rather than an array of ptrs which would require an additional 12 bytes). If the assoc data
   is larger than 4 bytes, then the exact amount needed by the associated data size (rounded up
   to 2 bytes) whereas with the other scheme, we may need an additional 12 bytes per prefix.

   If you have a million prefixes, this translates into several megabytes of savings. If your
   malloc overhead is 8 bytes (which on most systems it is), your savings can be as high as 50%
   and if your assoc data is larger than 4-bytes or <= 2 bytes, then the savings are even higher.

   WE DO NOT RECOMMEND USING kaps_pfx_bundle's IF THE NUMBER OF PREFIXES YOU ARE DEALING WITH IS
   SMALL (say less than 100,000). IMPROPER USE CAN ACTUALLY LEAD TO WASTING MORE MEMORY
   THAN USING STRAIGHT MALLOC/FREE.

   Notes:
   The storage factory supplied to kaps_nlm_allocator can be used to allocate other types of
   objects in the same factory as well. You can safely do this as long as the size of other
   stuff is either fixed, or you have a large number of them or if your sizes are comparable to
   the size of the kaps_pfx_bundle (index + assoc + pfx-size + pfx-data).

   When creating the storage factory, it is safe to use '4' for alignment as this will create an
   efficient (both memory and performance) prefix bundle. Recommended chunk size is about 1024
   bytes.
*/

#ifndef INCLUDED_KAPS_PFXBUNDLE_H
#define INCLUDED_KAPS_PFXBUNDLE_H

#include <kaps_fib_cmn_prefix.h>
#include "kaps_utility.h"

#include <kaps_externcstart.h>

#define KAPS_LSN_NEW_INDEX                ((uint32_t)0xffffffff)

/* Storage for associated data and the prefix itself is created by overallocating from the
   factory. The associated data is also stored at the end of the prefix data (it is two byte)
   aligned.
*/
typedef struct kaps_pfx_bundle
{
    struct kaps_lpm_entry *m_backPtr;
    uint32_t m_nIndex;
#ifdef KAPS_LPM_DEBUG
    uint32_t m_nSeqNum:27;
#endif
    uint32_t reconcile_used:1;   /**< Entry has been marked as used by user during reconcile phase */
    uint32_t m_status:1;        /* Indicates if the pfx bundle is pending or committed */
    uint32_t m_isAptLmpsofarPfx:1;
    uint32_t m_isPfxCopy:1;     /* Set to 1 if this is a prefix copy of an RPT entry */
    uint32_t m_nPfxSize:8;
    uint8_t m_data[2];          /* DO NOT ACCESS THIS */
} kaps_pfx_bundle;

extern kaps_pfx_bundle *kaps_pfx_bundle_create(
    kaps_nlm_allocator *,       /* What factory does this bundle belong to? */
    kaps_prefix * prefix,       /* Prefix to use to create bundle */
    uint32_t ix,                /* Index to set */
    uint32_t assocSize,         /* number of bytes for assoc data */
    uint32_t seqNum);           /* Sequence number */

#define kaps_pfx_bundle_destroy(self,alloc) kaps_nlm_allocator_free(alloc,self)

extern kaps_pfx_bundle *kaps_pfx_bundle_create_from_string(
    kaps_nlm_allocator *,       /* What factory does this bundle belong to? */
    const uint8_t * prefix,     /* Prefix to use to create bundle, Can be NULL */
    uint32_t numbits,           /* Number of bits to use */
    uint32_t ix,                /* Index to set */
    uint32_t assocSize,         /* number of bytes for assoc data */
    uint8_t addExtraFirstByte); /* Should an extra zero byte be added or not */

kaps_pfx_bundle *kaps_pfx_bundle_create_from_pfx_bundle(
    kaps_nlm_allocator * alloc,
    kaps_pfx_bundle * oldPfxBundle,
    uint32_t assocSize,
    uint8_t copyAssoc);

/* Compare two prefixes based on prefix information only. Use NlmCmPfxBundle__IsEqual if all
   you want is an equality check because it is much faster than a full blown Compare

   Returns
   0, <0 or >0 depending on whether self==other, self<other or self>other
*/
extern int kaps_pfx_bundle_compare(
    const kaps_pfx_bundle * self,
    const kaps_pfx_bundle * other);

extern void kaps_pfx_bundle_print(
    kaps_pfx_bundle * self,
    FILE * fp);

#define kaps_pfx_bundle_assert_valid(self)       \
    NlmCmPrefix__AssertTrailingBits(            \
        KAPS_PFX_BUNDLE_GET_PFX_DATA(self),       \
    KAPS_PFX_BUNDLE_GET_PFX_SIZE(self),       \
    8*KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES((self)->m_nPfxSize))

/* Returns the number of prefix bytes used to store the prefix. */
#define KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(x)   (((uint32_t)(x) + 7) >> 3)
#define KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES2(x)  ((KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(x) + 1) & ~(uint32_t)1)

/* Returns the index of the Prefix Bundle. */
#define KAPS_PFX_BUNDLE_GET_INDEX(self)      (self)->m_nIndex

/* Returns the pointer to the associated data. The returned data is a uint8 * but
   it is guaranteed to be 2-byte aligned.
 */
#define KAPS_PFX_BUNDLE_GET_ASSOC_PTR(SELF)   (kaps_assert_((SELF)==(SELF), "side-effect") \
                                             ((uint8_t *)(SELF)->m_data + KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES2((SELF)->m_nPfxSize)))

/* Given a Prefix bundle and the factory from which it came from, returns the size of
   the prefix.
*/
#define KAPS_PFX_BUNDLE_GET_PFX_SIZE(self)        (self)->m_nPfxSize

/* Given a Prefix bundle and the factory from which it came from, returns the data
   for the prefix
*/
#define KAPS_PFX_BUNDLE_GET_PFX_DATA(self)        ((uint8_t *)(self)->m_data)

extern void kaps_pfx_bundle_set_bit(
    kaps_pfx_bundle * self,
    uint32_t ix,
    int32_t bit);               /* Must be zero or non-zero, non-zero treated as one */

#include <kaps_externcend.h>

#endif
