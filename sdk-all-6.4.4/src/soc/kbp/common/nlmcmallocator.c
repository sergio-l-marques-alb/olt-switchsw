/*
 * $Id: nlmcmallocator.c,v 1.2.8.2 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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


 
#include <nlmcmbasic.h>
#include <nlmcmallocator.h>
#include <nlmcmstring.h>
#include <nlmcmarena.h>


/* forward declarations */

#include <nlmcmexterncstart.h>





static void* 
NlmCmAllocator__malloc_body(
    NlmCmAllocator* self,
    size_t size) ;

static void* 
NlmCmAllocator__sysMalloc_body(
    size_t size) ;


static void* 
NlmCmAllocator__calloc_body(
    NlmCmAllocator* self,
    size_t cnt, 
    size_t size) ;

static void* 
NlmCmAllocator__resize_body(
    NlmCmAllocator* self,
    void* memblk,
    size_t newSize,
        size_t oldSize) ;

static void
NlmCmAllocator__free_body(
    NlmCmAllocator* self,
    void* memblk) ;

static void
NlmCmAllocator__sysFree_body(
    void* memblk) ;

/*
 * The virtual table for the NlmCmAllocator
 */
static NlmCmAllocator__vtbl defaultVtbl = 
{
    "NlmCmAllocator__default",
    NlmCmAllocator__malloc_body,
    NlmCmAllocator__calloc_body,
    NlmCmAllocator__resize_body,
    NlmCmAllocator__free_body,
    NlmCmAllocator__dtor_body,
    "NlmCmAllocator__default",
    NlmCmAllocator__sysMalloc_body,
    NlmCmAllocator__sysFree_body,
} ;

NlmCmAllocator*
NlmCmAllocator__ctor(
    NlmCmAllocator *self)
{
    self->m_vtbl_p = &(self->m_vtbl) ;
    NlmCm__memcpy(self->m_vtbl_p, &defaultVtbl, sizeof(NlmCmAllocator__vtbl)) ;
    self->m_clientData_p = NULL ;

    return self ;
}

void
NlmCmAllocator__dtor_body(
    NlmCmAllocator *self)
{
    (void)self ;
}

void
NlmCmAllocator__config(
    NlmCmAllocator* self,
    NlmCmAllocator__vtbl* vtbl)
{
    NlmCm__memcpy(&(self->m_vtbl), vtbl, sizeof(NlmCmAllocator__vtbl)) ;
}

void* 
NlmCmAllocator__malloc_body(
    NlmCmAllocator* self,
    size_t size)
{
    void *ptr;

    (void)self ;

    ptr = NlmCm__sysmalloc(size) ;

    return ptr;
}

void* 
NlmCmAllocator__sysMalloc_body(
    size_t size)
{
    return NlmCm__sysmalloc(size) ;
}

void* 
NlmCmAllocator__calloc_body(
    NlmCmAllocator* self,
    size_t cnt,
    size_t size)
{
    /* NOTE: Optimization. The Win32/Debug implementation is sub-optimal.
     * It uses a loop involving pointer comparision, increment, dereference,
     * and assignment to zero out the memory. The system memset implementation
     * is much more efficient at zeroing out memory.
     */
    void* ptr ;
    size_t tot_size ;

    (void)self ;

    tot_size = cnt * size ;

    ptr = NlmCm__sysmalloc(tot_size) ;
    if( ptr )
    {
        NlmCm__memset(ptr, 0, tot_size) ;
    }

    return ptr ;
}
void* 
NlmCmAllocator__resize_body(
    NlmCmAllocator* self,
        void* memblk,
    size_t newSize,
        size_t oldSize)
{
    void* newblk = 0;

    if (!memblk) {
    NlmCmAssert(0 == oldSize, "old size must be 0 if memblk is NULL") ;
    return NlmCmAllocator__malloc(self, newSize) ;
    } else if (newSize == oldSize)
    return memblk ;

    /* If we fail to get a new chunk, leave the original chunk untouched. */

    if(0 != (newblk = NlmCmAllocator__malloc(self, newSize))) {
        NlmCm__memcpy(newblk, memblk, NLMCMMIN(newSize, oldSize));
        NlmCmAllocator__free(self, memblk);
    } else if (newSize < oldSize)
    return memblk ;

    return newblk;
}

void
NlmCmAllocator__free_body(
    NlmCmAllocator* self,
    void* memblk) 
{
    (void)self ;
    /* Avoid attempting to free 0; while this will be treated as
       a no-op by stdlib (as documented on Linux, but not Solaris),
       not all allocators support it by default (eg, dmalloc).
       Be robust, and handle it here.
    */

   if (memblk) NlmCm__sysfree(memblk) ;
}


void
NlmCmAllocator__sysFree_body(
    void* memblk) 
{
    /* Avoid attempting to free 0; while this will be treated as
       a no-op by stdlib (as documented on Linux, but not Solaris),
       not all allocators support it by default (eg, dmalloc).
       Be robust, and handle it here.
    */
    if (memblk) NlmCm__sysfree(memblk) ;    

}


void* 
NlmCmAllocator__malloc(
    NlmCmAllocator* self,
    size_t size)
{
    void *ptr;

    ptr = self->m_vtbl.m_malloc(self, size);

    if(!ptr)
        ptr = NlmCmArena__Allocate(size);

    return ptr;
}

void* 
NlmCmAllocator__calloc(
    NlmCmAllocator* self,
    size_t cnt,
    size_t size)
{
    void* ptr ;
    size_t tot_size ;

    tot_size = cnt * size ;

    ptr = self->m_vtbl.m_calloc(self, cnt, size);
    if(!ptr)
    {
        ptr = NlmCmArena__Allocate(tot_size);
        if(ptr)
        {
            NlmCm__memset(ptr, 0, tot_size) ;
        }
    }

    return ptr ;
}


void* 
NlmCmAllocator__resize(
    NlmCmAllocator* self,
    void* memblk,
    size_t newSize,
    size_t oldSize)
{
    void* newblk = 0;


    newblk = self->m_vtbl.m_resize(self, memblk, newSize, oldSize);
    if(!newblk)
    {
        newblk = NlmCmArena__Allocate(newSize);
        if(newblk && memblk)
        {
            NlmCm__memcpy(newblk, memblk, NLMCMMIN(newSize, oldSize));
            NlmCmAllocator__free(self, memblk);
        }
    } 

    return newblk;
}


void
NlmCmAllocator__free(
    NlmCmAllocator* self,
    void* memblk) 
{
    if (memblk) 
    {
        if(!NlmCmArena__FreeIfArenaMemory(memblk))
            self->m_vtbl.m_free(self, memblk);
    }
}



#include <nlmcmexterncend.h>

/*[]*/
