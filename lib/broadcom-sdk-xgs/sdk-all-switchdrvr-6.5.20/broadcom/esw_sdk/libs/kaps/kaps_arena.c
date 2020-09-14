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

#include <kaps_arena.h>

/*align should be a power of 2. Given a value, the nearest highest power of 2 is returned*/
#define NLM_ALIGN_UP(value, align) (((value) + ((align) - 1)) & ~((align) - 1))

typedef struct kaps_arena
{
    char *m_arenaStart_p;       /* Starting address of the arena */
    char *m_arenaEnd_p;         /* Ending address of the arena */
    uint32_t m_numBytesAvailable;       /* Number of free bytes available in the arena */
    uint32_t m_numAllocedChunks;        /* Number of chunks allocated in the arena */
    uint32_t m_curOffset;       /* Offset of the first free byte in the arena */
    struct kaps_arena *m_next_p;        /* Pointer to the next arena */
    kaps_nlm_allocator *m_alloc_p;      /* Pointer to the Memory Allocator */
} kaps_arena;

typedef struct kaps_arena_info
{

    kaps_arena *g_arenaHead_p;  /* Pointer to the first arena */
    uint32_t g_allowAllocFromArena;     /* If arena is active then g_allowAllocFromArena is set to 1 and memory will be 
                                         * allocated from the arena If arena is inactive then g_allowAllocFromArena is
                                         * set to 0 and memory will NOT be allocated from the arena */
    uint32_t g_numAllocedChunksFromAllArenas;   /* Total number of allocated chunks in all Arenas currently present */
    uint32_t g_numCompletelyFreeArena;  /* Number of completely free arena completely present */
    uint32_t g_arenaSizeInBytes;        /* Size of the arena in bytes. This value should NOT be changed once FibTblMgr
                                         * Configuration is Locked */

} kaps_arena_info;

/*Internal function that allocates space for a new Arena and adds it to the linked list*/
NlmErrNum_t
kaps_arena_add_arena(
    kaps_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason_p)
{
    struct kaps_arena_info *arena_info_p = (struct kaps_arena_info *) alloc_p->m_arena_info;
    kaps_arena *arena_p = NULL;

    /*
     * Allocate space for the linked list node from the system memory 
     */
    arena_p = (kaps_arena *) alloc_p->m_vtbl.m_calloc(alloc_p->m_clientData_p, 1, sizeof(kaps_arena));

    if (!arena_p)
    {
        if (o_reason_p)
            *o_reason_p = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    /*
     * Allocate space for the arena from system memory 
     */
    arena_p->m_arenaStart_p =
        (char *) alloc_p->m_vtbl.m_calloc(alloc_p->m_clientData_p, 1, arena_info_p->g_arenaSizeInBytes);

    if (!arena_p->m_arenaStart_p)
    {
        alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, arena_p);
        if (o_reason_p)
            *o_reason_p = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    arena_p->m_arenaEnd_p = arena_p->m_arenaStart_p + arena_info_p->g_arenaSizeInBytes - 1;
    arena_p->m_numBytesAvailable = arena_info_p->g_arenaSizeInBytes;
    arena_p->m_alloc_p = alloc_p;

    if (!arena_info_p->g_arenaHead_p)
    {
        arena_info_p->g_arenaHead_p = arena_p;
    }
    else
    {
        kaps_arena *curArena_p = arena_info_p->g_arenaHead_p;
        while (curArena_p->m_next_p)
            curArena_p = curArena_p->m_next_p;

        /*
         * Place the new arena at the end of the list 
         */
        curArena_p->m_next_p = arena_p;
    }

    arena_info_p->g_numCompletelyFreeArena++;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_arena_init(
    kaps_nlm_allocator * alloc_p,
    uint32_t arenaSizeInBytes,
    NlmReasonCode * o_reason_p)
{
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_arena_info *arena_info_p;

    alloc_p->m_arena_info =
        (void *) alloc_p->m_vtbl.m_calloc(alloc_p->m_clientData_p, 1, sizeof(struct kaps_arena_info));
    arena_info_p = (struct kaps_arena_info *) alloc_p->m_arena_info;
    arena_info_p->g_arenaSizeInBytes = KAPS_DEFAULT_ARENA_SIZE_IN_BYTES;

    if (arenaSizeInBytes > arena_info_p->g_arenaSizeInBytes)
        arena_info_p->g_arenaSizeInBytes = arenaSizeInBytes;

    errNum = kaps_arena_add_arena(alloc_p, o_reason_p);

    return errNum;
}

NlmErrNum_t
kaps_arena_activate_arena(
    kaps_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason_p)
{
    struct kaps_arena_info *arena_info_p = (struct kaps_arena_info *) alloc_p->m_arena_info;
    NlmErrNum_t errNum = NLMERR_OK;

    if (arena_info_p->g_numCompletelyFreeArena)
    {
        /*
         * Allow memory allocations to happen from the Arena 
         */
        arena_info_p->g_allowAllocFromArena = 1;
        return NLMERR_OK;
    }

    /*
     * If there are no completely free Arenas, then try to allocate a completely new Arena 
     */
    errNum = kaps_arena_add_arena(alloc_p, o_reason_p);

    /*
     * If AddArena is successful, then we will have atleast one completely free Arena. In this case we can allow memory 
     * allocations to take place from the Arena. If AddArena fails then return the failure that AddArena reported 
     */
    if (arena_info_p->g_numCompletelyFreeArena)
        arena_info_p->g_allowAllocFromArena = 1;

    return errNum;
}

NlmErrNum_t
kaps_arena_deactivate_arena(
    kaps_nlm_allocator * api_alloc_p)
{
    struct kaps_arena_info *arena_info_p = (struct kaps_arena_info *) api_alloc_p->m_arena_info;
    /*
     * Don't allow any memory allocations to take place from the Arena until Activate is called again 
     */
    arena_info_p->g_allowAllocFromArena = 0;

    /*
     * If we have 0 or 1 completely free Arenas then return 
     */
    if (arena_info_p->g_numCompletelyFreeArena <= 1)
        return NLMERR_OK;

    {
        kaps_arena *curArena_p = arena_info_p->g_arenaHead_p;
        kaps_arena *prevArena_p = NULL;
        kaps_arena *temp;
        kaps_nlm_allocator *alloc_p;
        int32_t isFirstFullyFreeArenaFound = 0;

        /*
         * If there are 2 or more completely free Arenas, then free up the extra free Arenas to the system memory so
         * that we are left with only one completely free Arena 
         */
        arena_info_p->g_arenaHead_p = NULL;

        while (curArena_p)
        {
            temp = curArena_p->m_next_p;

            if (curArena_p->m_numBytesAvailable == arena_info_p->g_arenaSizeInBytes && isFirstFullyFreeArenaFound)
            {
                if (prevArena_p)
                    prevArena_p->m_next_p = curArena_p->m_next_p;

                alloc_p = curArena_p->m_alloc_p;

                alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, curArena_p->m_arenaStart_p);
                alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, curArena_p);

                --arena_info_p->g_numCompletelyFreeArena;

            }
            else
            {
                if (curArena_p->m_numBytesAvailable == arena_info_p->g_arenaSizeInBytes)
                    isFirstFullyFreeArenaFound = 1;

                if (!arena_info_p->g_arenaHead_p)
                    arena_info_p->g_arenaHead_p = curArena_p;

                prevArena_p = curArena_p;
            }

            curArena_p = temp;
        }

    }

    return NLMERR_OK;
}

void *
kaps_arena_allocate(
    kaps_nlm_allocator * alloc_p,
    uint32_t size)
{
    struct kaps_arena_info *arena_info_p = (struct kaps_arena_info *) alloc_p->m_arena_info;
    /*
     * If the Arena is not active or the memory size to be requested is 0, then return NULL 
     */
    if (!arena_info_p || !arena_info_p->g_allowAllocFromArena || size == 0)
        return NULL;

    {
        uint32_t actual_size = NLM_ALIGN_UP(size, sizeof(char *));      /* Ensure that the address returned from Arena
                                                                         * is word aligned */
        kaps_arena *curArena_p = arena_info_p->g_arenaHead_p;
        void *result = NULL;

        /*
         * Iterate through the Arenas and allocate the requested memory from the first Arena that can satisfy the
         * request 
         */
        while (curArena_p)
        {
            if (curArena_p->m_numBytesAvailable >= actual_size)
            {
                result = &curArena_p->m_arenaStart_p[curArena_p->m_curOffset];

                if (curArena_p->m_numBytesAvailable == arena_info_p->g_arenaSizeInBytes)
                    arena_info_p->g_numCompletelyFreeArena--;
                curArena_p->m_numBytesAvailable -= actual_size;
                curArena_p->m_curOffset += actual_size;
                curArena_p->m_numAllocedChunks++;

                arena_info_p->g_numAllocedChunksFromAllArenas++;

                break;
            }
            curArena_p = curArena_p->m_next_p;
        }

        return result;
    }

}

int32_t
kaps_arena_free_if_arena_memory(
    kaps_nlm_allocator * alloc_p,
    void *ptr)
{
    struct kaps_arena_info *arena_info_p = (struct kaps_arena_info *) alloc_p->m_arena_info;
    /*
     * If there are no chunks allocated from any of the Arenas or ptr is NULL, then return 0 indicating that the memory 
     * is not Arena memory 
     */
    if (!arena_info_p || !arena_info_p->g_numAllocedChunksFromAllArenas || !ptr)
        return 0;

    {
        kaps_arena *curArena_p = arena_info_p->g_arenaHead_p;
        int32_t result = 0;

        while (curArena_p)
        {
            if (ptr >= (void *) curArena_p->m_arenaStart_p && ptr <= (void *) curArena_p->m_arenaEnd_p)
            {
                /*
                 * The memory has been allocated from the Arena. So return it back to the arena 
                 */
                result = 1;

                if (curArena_p->m_numAllocedChunks)
                    curArena_p->m_numAllocedChunks--;

                if (arena_info_p->g_numAllocedChunksFromAllArenas)
                    arena_info_p->g_numAllocedChunksFromAllArenas--;

                if (curArena_p->m_numAllocedChunks == 0)
                {
                    /*
                     * The Arena is completely free 
                     */
                    curArena_p->m_curOffset = 0;
                    curArena_p->m_numBytesAvailable = arena_info_p->g_arenaSizeInBytes;
                    arena_info_p->g_numCompletelyFreeArena++;
                }
                break;
            }
            curArena_p = curArena_p->m_next_p;
        }

        return result;
    }
}

void
kaps_arena_destroy(
    kaps_nlm_allocator * alloc)
{
    struct kaps_arena_info *arena_info_p = (struct kaps_arena_info *) alloc->m_arena_info;
    kaps_arena *curArena_p = arena_info_p->g_arenaHead_p;
    kaps_arena *temp;
    kaps_nlm_allocator *alloc_p = alloc;

    /*
     * Iterate through all the Arenas. Release the memory for the linked list node and the actual Arena 
     */
    while (curArena_p)
    {
        temp = curArena_p->m_next_p;
        alloc_p = curArena_p->m_alloc_p;

        alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, curArena_p->m_arenaStart_p);
        alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, curArena_p);

        curArena_p = temp;
    }
    alloc_p->m_vtbl.m_free(alloc_p->m_clientData_p, alloc_p->m_arena_info);
    alloc_p->m_arena_info = NULL;
}
