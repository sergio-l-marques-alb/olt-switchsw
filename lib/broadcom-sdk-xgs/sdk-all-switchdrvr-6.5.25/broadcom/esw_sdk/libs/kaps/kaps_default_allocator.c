/*******************************************************************************
 *
 * Copyright 2012-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#include "kaps_default_allocator.h"
#include "kaps_portable.h"

struct kaps_default_allocator_handle
{
    struct kaps_default_allocator_stats stats;
    uint64_t nbytes;            /* active bytes to compute peak usage */
};

struct kaps_default_allocator_header
{
    uint64_t nbytes;
};

static void *
default_malloc(
    void *cookie,
    uint32_t size)
{
    struct kaps_default_allocator_header *hdr = NULL;

    if (size == 0)
        kaps_assert(0, "malloc of size zero is invalid");

    hdr = kaps_sysmalloc(size + sizeof(struct kaps_default_allocator_header));
    if (hdr)
    {
        struct kaps_default_allocator_handle *handle = (struct kaps_default_allocator_handle *) cookie;
        handle->stats.nallocs++;
        handle->stats.cumulative_bytes += size;
        if ((handle->nbytes + size) > handle->stats.peak_bytes)
            handle->stats.peak_bytes = handle->nbytes + size;
        hdr->nbytes = size;
        handle->nbytes += size;
        return hdr + 1;
    }
    return NULL;
}

static void *
default_calloc(
    void *cookie,
    uint32_t nelem,
    uint32_t size)
{
    uint32_t tot_size = nelem * size;
    struct kaps_default_allocator_header *hdr = NULL;

    if (tot_size == 0)
        kaps_assert(0, "calloc of size zero is invalid");

    hdr = kaps_syscalloc(1, tot_size + sizeof(struct kaps_default_allocator_header));
    if (hdr)
    {
        struct kaps_default_allocator_handle *handle = (struct kaps_default_allocator_handle *) cookie;
        handle->stats.nallocs++;
        handle->stats.cumulative_bytes += (nelem * size);
        if ((handle->nbytes + tot_size) > handle->stats.peak_bytes)
            handle->stats.peak_bytes = handle->nbytes + tot_size;
        hdr->nbytes = tot_size;
        handle->nbytes += tot_size;
        return hdr + 1;
    }
    return NULL;
}

static void
default_free(
    void *cookie,
    void *ptr)
{
    if (ptr)
    {
        struct kaps_default_allocator_handle *handle = (struct kaps_default_allocator_handle *) cookie;
        struct kaps_default_allocator_header *hdr =
            (struct kaps_default_allocator_header *) ((uint8_t *) ptr - sizeof(struct kaps_default_allocator_header));
        kaps_sassert(handle->nbytes >= hdr->nbytes);
        handle->nbytes -= hdr->nbytes;
        handle->stats.nfrees++;
        kaps_sysfree(hdr);
    }
}

kaps_status
kaps_default_allocator_create(
    struct kaps_allocator **alloc)
{
    struct kaps_allocator *ret;
    struct kaps_default_allocator_handle *handle;

    if (!alloc)
        return KAPS_INVALID_ARGUMENT;

    ret = kaps_sysmalloc(sizeof(*ret));
    if (!ret)
    	return KAPS_OUT_OF_MEMORY;

    handle = kaps_syscalloc(1, sizeof(*handle));

    if (!handle) {
    	kaps_sysfree(ret);
        return KAPS_OUT_OF_MEMORY;
    }

    ret->cookie = handle;
    ret->xmalloc = default_malloc;
    handle->nbytes = 0;
    handle->stats.nallocs = 0;
    handle->stats.nfrees = 0;
    handle->stats.cumulative_bytes = 0;
    handle->stats.peak_bytes = 0;;
    ret->xfree = default_free;
    ret->xcalloc = default_calloc;

    *alloc = ret;
    return KAPS_OK;
}

kaps_status
kaps_default_allocator_destroy(
    struct kaps_allocator * alloc)
{
    if (!alloc)
        return KAPS_INVALID_ARGUMENT;

    kaps_sysfree(alloc->cookie);
    kaps_sysfree(alloc);
    return KAPS_OK;
}

kaps_status
kaps_default_allocator_get_stats(
    struct kaps_allocator * alloc,
    struct kaps_default_allocator_stats * stats)
{
    struct kaps_default_allocator_handle *handle;

    if (!alloc || !stats)
        return KAPS_INVALID_ARGUMENT;

    handle = (struct kaps_default_allocator_handle *) alloc->cookie;
    kaps_memcpy(stats, &handle->stats, sizeof(*stats));
    return KAPS_OK;
}
