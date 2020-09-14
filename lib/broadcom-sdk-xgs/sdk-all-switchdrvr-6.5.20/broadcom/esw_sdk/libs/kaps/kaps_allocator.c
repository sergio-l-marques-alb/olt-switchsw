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

#include "kaps_legacy.h"
#include "kaps_arena.h"
#include "kaps_portable.h"
#include "kaps_utility.h"

/* forward declarations */

#include <kaps_externcstart.h>

static void *kaps_nlm_allocator_malloc_body(
    kaps_nlm_allocator * self,
    size_t size);

static void *kaps_nlm_allocator_sys_malloc_body(
    size_t size);

static void *kaps_nlm_allocator_calloc_body(
    kaps_nlm_allocator * self,
    size_t cnt,
    size_t size);

static void *kaps_nlm_allocator_resize_body(
    kaps_nlm_allocator * self,
    void *memblk,
    size_t newSize,
    size_t oldSize);

static void kaps_nlm_allocator_free_body(
    kaps_nlm_allocator * self,
    void *memblk);

static void kaps_nlm_allocator_sys_free_body(
    void *memblk);

/*
 * The virtual table for the kaps_nlm_allocator
 */
static kaps_nlm_allocator_vtbl defaultVtbl = {
    "kaps_nlm_allocator_default",
    kaps_nlm_allocator_malloc_body,
    kaps_nlm_allocator_calloc_body,
    kaps_nlm_allocator_resize_body,
    kaps_nlm_allocator_free_body,
    kaps_nlm_allocator_dtor_body,
    "kaps_nlm_allocator_default",
    kaps_nlm_allocator_sys_malloc_body,
    kaps_nlm_allocator_sys_free_body,
};

kaps_nlm_allocator *
kaps_nlm_allocator_ctor(
    kaps_nlm_allocator * self)
{
    self->m_vtbl_p = &(self->m_vtbl);
    kaps_memcpy(self->m_vtbl_p, &defaultVtbl, sizeof(kaps_nlm_allocator_vtbl));
    self->m_clientData_p = self;
    self->m_arena_info = NULL;

    return self;
}

void
kaps_nlm_allocator_dtor_body(
    kaps_nlm_allocator * self)
{
    (void) self;
}

void
kaps_nlm_allocator_config(
    kaps_nlm_allocator * self,
    kaps_nlm_allocator_vtbl * vtbl)
{
    kaps_memcpy(&(self->m_vtbl), vtbl, sizeof(kaps_nlm_allocator_vtbl));
}

void *
kaps_nlm_allocator_malloc_body(
    kaps_nlm_allocator * self,
    size_t size)
{
    void *ptr;

    (void) self;

    ptr = kaps_sysmalloc(size);

    return ptr;
}

void *
kaps_nlm_allocator_sys_malloc_body(
    size_t size)
{
    return kaps_sysmalloc(size);
}

void *
kaps_nlm_allocator_calloc_body(
    kaps_nlm_allocator * self,
    size_t cnt,
    size_t size)
{
    /*
     * NOTE: Optimization. The Win32/Debug implementation is sub-optimal. It uses a loop involving pointer comparision, 
     * increment, dereference, and assignment to zero out the memory. The system kaps_memset implementation is much
     * more efficient at zeroing out memory. 
     */
    void *ptr;
    size_t tot_size;

    (void) self;

    tot_size = cnt * size;

    ptr = kaps_sysmalloc(tot_size);
    if (ptr)
    {
        kaps_memset(ptr, 0, tot_size);
    }

    return ptr;
}

void *
kaps_nlm_allocator_resize_body(
    kaps_nlm_allocator * self,
    void *memblk,
    size_t newSize,
    size_t oldSize)
{
    void *newblk = 0;

    if (!memblk)
    {
        kaps_assert(0 == oldSize, "old size must be 0 if memblk is NULL");
        return kaps_nlm_allocator_malloc(self, newSize);
    }
    else if (newSize == oldSize)
        return memblk;

    /*
     * If we fail to get a new chunk, leave the original chunk untouched. 
     */

    if (0 != (newblk = kaps_nlm_allocator_malloc(self, newSize)))
    {
        kaps_memcpy(newblk, memblk, KAPS_MIN(newSize, oldSize));
        kaps_nlm_allocator_free(self, memblk);
    }
    else if (newSize < oldSize)
        return memblk;

    return newblk;
}

void
kaps_nlm_allocator_free_body(
    kaps_nlm_allocator * self,
    void *memblk)
{
    (void) self;
    /*
     * Avoid attempting to free 0; while this will be treated as a no-op by stdlib (as documented on Linux, but not
     * Solaris), not all allocators support it by default (eg, dmalloc). Be robust, and handle it here. 
     */

    if (memblk)
        kaps_sysfree(memblk);
}

void
kaps_nlm_allocator_sys_free_body(
    void *memblk)
{
    /*
     * Avoid attempting to free 0; while this will be treated as a no-op by stdlib (as documented on Linux, but not
     * Solaris), not all allocators support it by default (eg, dmalloc). Be robust, and handle it here. 
     */
    if (memblk)
        kaps_sysfree(memblk);

}

void *
kaps_nlm_allocator_malloc(
    kaps_nlm_allocator * self,
    size_t size)
{
    void *ptr;

    ptr = self->m_vtbl.m_malloc((kaps_nlm_allocator *) self->m_clientData_p, size);

    if (!ptr)
        ptr = kaps_arena_allocate(self, size);

    return ptr;
}

void *
kaps_nlm_allocator_calloc(
    kaps_nlm_allocator * self,
    size_t cnt,
    size_t size)
{
    void *ptr;
    size_t tot_size;

    tot_size = cnt * size;

    ptr = self->m_vtbl.m_calloc((kaps_nlm_allocator *) self->m_clientData_p, cnt, size);
    if (!ptr)
    {
        ptr = kaps_arena_allocate(self, tot_size);
        if (ptr)
        {
            kaps_memset(ptr, 0, tot_size);
        }
    }

    return ptr;
}

void *
kaps_nlm_allocator_resize(
    kaps_nlm_allocator * self,
    void *memblk,
    size_t newSize,
    size_t oldSize)
{
    void *newblk = 0;

    newblk = self->m_vtbl.m_resize((kaps_nlm_allocator *) self, memblk, newSize, oldSize);
    if (!newblk)
    {
        newblk = kaps_arena_allocate(self, newSize);
        if (newblk && memblk)
        {
            kaps_memcpy(newblk, memblk, KAPS_MIN(newSize, oldSize));
            kaps_nlm_allocator_free(self, memblk);
        }
    }

    return newblk;
}

void
kaps_nlm_allocator_free(
    kaps_nlm_allocator * self,
    void *memblk)
{
    if (memblk)
    {
        if (!kaps_arena_free_if_arena_memory(self, memblk))
            self->m_vtbl.m_free((kaps_nlm_allocator *) self->m_clientData_p, memblk);
    }
}

#include <kaps_externcend.h>

/*[]*/
