/*
 * $Id: profile_mem.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 *
 * Provides generic routines for managing HW profile tables.
 */

#include <soc/profile_mem.h>
#include <soc/error.h>
#include <soc/drv.h>

/*
 * Function:
 *      soc_profile_mem_t_init
 * Purpose:
 *      Initialize a soc_profile_mem_t structure.
 *
 * Parameters:
 *      profile_mem - (IN) Pointer to profile memory structure
 * Returns:
 *      void
 */
void
soc_profile_mem_t_init(soc_profile_mem_t *profile_mem)
{
    if (NULL != profile_mem) {
        sal_memset(profile_mem, 0, sizeof(soc_profile_mem_t));
    }
}

/*
 * Function:
 *      soc_profile_mem_create
 * Purpose:
 *      Create a shadow copy and refcounts of a profile table.
 *      If called during WARM BOOT, the shadow copy is populated with
 *      the HW contents, otherwise, both the shadow copy and the
 *      HW entries are cleared.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      mems        - (IN) Pointer to memory id array
 *      mems_count  - (IN) Number of entries in memory id array
 *      profile_mem - (IN) Pointer to profile memory structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_create(int unit,
                       soc_mem_t *mems,
                       int mems_count,
                       soc_profile_mem_t *profile_mem)
{
    soc_mem_t mem;
    int rv;
    int alloc_size;
    int num_entries, index_min, cache_words, mem_idx, i;
    uint32 *cache_p;

    if (profile_mem == NULL) {
        return SOC_E_INIT;
    }

    if (mems == NULL || !mems_count) {
        return SOC_E_PARAM;
    }

    num_entries = soc_mem_index_count(unit, mems[0]);
    if (num_entries <= 0) {
        return SOC_E_BADID;
    }
    index_min = soc_mem_index_min(unit, mems[0]);
    cache_words = soc_mem_entry_words(unit, mems[0]);
    for (mem_idx = 1; mem_idx < mems_count; mem_idx++) {
        mem = mems[mem_idx];
        if (soc_mem_index_count(unit, mem) != num_entries ||
            soc_mem_index_min(unit, mem) != index_min) {
            return SOC_E_PARAM;
        }
        cache_words += soc_mem_entry_words(unit, mem);
    }

    alloc_size = num_entries *                                             \
        (sizeof(soc_profile_mem_entry_t) + cache_words * sizeof(uint32)) + \
         mems_count * sizeof(soc_mem_t);

    /* If profile_mem->mems is NULL, init the profile_mem for the first
     * time, otherwise simply check for null pointers */
    if (profile_mem->mems != NULL) {
        if (profile_mem->entries == NULL) {
            return SOC_E_INTERNAL;
        }
    } else {
        profile_mem->entries = sal_alloc(alloc_size,  "Profile Mem Entries");
        if (profile_mem->entries == NULL) {
            return SOC_E_MEMORY;
        }
    }
    sal_memset(profile_mem->entries, 0, alloc_size);

    cache_p = (uint32 *)&profile_mem->entries[num_entries];
    for (i = 0; i < num_entries; i++) {
        profile_mem->entries[i].cache_p = cache_p;
        cache_p += cache_words;
    }
    profile_mem->mems = (soc_mem_t *)cache_p;
    for (mem_idx = 0; mem_idx < mems_count; mem_idx++) {
        profile_mem->mems[mem_idx] = mems[mem_idx];
    }
    profile_mem->mems_count = mems_count;

    if (SOC_WARM_BOOT(unit)) {
        for (i = 0; i < num_entries; i++) {
            cache_p = profile_mem->entries[i].cache_p;
            for (mem_idx = 0; mem_idx < profile_mem->mems_count; mem_idx++) {
                mem = profile_mem->mems[mem_idx];
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                  soc_mem_index_min(unit, mem) + i, cache_p);
                if (rv < 0) {
                    sal_free(profile_mem->entries);
                    return rv;
                }
                cache_p += soc_mem_entry_words(unit, mem);
            }
        }
    } else {
        /* Clear HW memory */
        for (mem_idx = 0; mem_idx < profile_mem->mems_count; mem_idx++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, profile_mem->mems[mem_idx],
                               COPYNO_ALL, TRUE));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_mem_destroy
 * Purpose:
 *      Destroy the shadow copy and refcounts of a profile table.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile_mem - (IN) Pointer to profile memory structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_destroy(int unit,
                        soc_profile_mem_t *profile_mem)
{
    if (profile_mem != NULL) {
        if (profile_mem->entries != NULL) {
            /* release profile_mem->entries also release the storage hold by
             * profile_mem->entries[i].cache_p */
            sal_free(profile_mem->entries);
        }
        profile_mem->mems = NULL;
        return SOC_E_NONE;
    }
    return SOC_E_PARAM;
}

/*
 * Function:
 *      soc_profile_mem_add
 * Purpose:
 *      Add a set of entries (one or more enrties) to a profile table. This
 *      routine searches for a matching set in the profile table. If a matching
 *      set is found, the ref count for that entry is incremented and
 *      its base index is returned. If a matching set is not found and a free
 *      set is found, the HW table is updated, the ref count is incremented,
 *      and the base index of the set is returned. If no free set is found, an
 *      error is returned
 *
 * Parameters:
 *      unit        - (IN)  Unit
 *      profile_mem - (IN)  Pointer to profile memory structure
 *      entries     - (IN)  Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index       - (OUT) Base index for the entires allocated in HW
 * Returns:
 *      SOC_E_XXX
 *
 * Notes:
 * For example
 * Usually a profile structure is formed by single memory table. However if a
 * profile structure is formed by the combination of 2 tables (table1 and
 * table2 in this example). Each entry in table1 is 2 words long, and each
 * entry in table2 is 3 words long. Argument entries_per_set is uniform on all
 * tables (4 in this example).
 *   +------------+     +--------------------------+
 *   | entries[0] |---> |  table1[0], 2 words long | 
 *   +------------+     +--------------------------+
 *   | entries[1] |-+   |  table1[1]               |
 *   +------------+ |   +--------------------------+
 *                  |   |  table1[2]               |
 *                  |   +--------------------------+
 *                  |   |  table1[3]               |
 *                  |   +--------------------------+
 *                  |   +---------------------------------+
 *                  +-> |  table2[0], 3 words long        |
 *                      +---------------------------------+
 *                      |  table2[1]                      |
 *                      +---------------------------------+
 *                      |  table2[2]                      |
 *                      +---------------------------------+
 *                      |  table2[3]                      |
 *                      +---------------------------------+
 *
 * The code for above example may look like:
 * {
 *     void *entries[2];
 *     table1_entry_t table1[4];
 *     table2_entry_t table2[4];
 *     uint32 index;
 *
 *     fill table1[0], table1[1], table1[2], table1[3]
 *     fill table2[0], table2[1], table2[2], table2[3]
 *     entries[0] = &table1;
 *     entries[1] = &table2;
 *     soc_profile_mem_add(unit, profile_mem, &entries, 4, &index);
 * }
 */
int
soc_profile_mem_add(int unit,
                    soc_profile_mem_t *profile_mem,
                    void **entries,
                    int entries_per_set,
                    uint32 *index)
{
    soc_mem_t mem;
    int base, free_index, cache_index;
    int num_entries, entry_words, offset, mem_idx, i;
    uint32 *cache_p, *entry_p;

    if (profile_mem == NULL || profile_mem->mems == NULL ||
        profile_mem->entries == NULL) {
        return SOC_E_INIT;
    }

    num_entries = soc_mem_index_count(unit, profile_mem->mems[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if (entries == NULL || index == NULL ||
        entries_per_set < 1 || entries_per_set > num_entries) {
        return SOC_E_PARAM;
    }

    /*
     * Search for an existing entry that has the same configuration.
     */
    free_index = -1;
    for (base = 0; base < num_entries; base += entries_per_set) {
        /* Skip unused entries. */
        if (!profile_mem->entries[base].ref_count) {
            if (free_index == -1) {
                /* Preserve location of free slot. */
                free_index = base;
                for (i = 1; i < entries_per_set; i++) {
                    if (profile_mem->entries[base + i].ref_count) {
                        free_index = -1;
                        break;
                    }
                }
            }
            continue;
        }

        for (i = 0; i < entries_per_set; i++) {
            if (profile_mem->entries[base + i].entries_per_set !=
                entries_per_set) {
                break;
            }
            offset = 0;
            for (mem_idx = 0; mem_idx < profile_mem->mems_count; mem_idx++) {
                mem = profile_mem->mems[mem_idx];
                entry_words = soc_mem_entry_words(unit, mem);
                entry_p = (uint32 *)entries[mem_idx] + i * entry_words;
                cache_p = profile_mem->entries[base + i].cache_p + offset;
                if (soc_mem_compare_entry(unit, mem, cache_p, entry_p)) {
                    break;
                }
                offset += entry_words;
            }
            if (mem_idx != profile_mem->mems_count) {
                break;
            }
        }
        if (i == entries_per_set) {
            for (i = 0; i < entries_per_set; i++) {
                profile_mem->entries[base + i].ref_count++;
            }
            *index = base;
            return SOC_E_NONE;
        }
        if (profile_mem->entries[base].entries_per_set > entries_per_set) {
            base += profile_mem->entries[base].entries_per_set -
                    entries_per_set;
        }
    }

    if (free_index == -1) {
        return SOC_E_RESOURCE;
    }

    cache_index = free_index;
    free_index += soc_mem_index_min(unit, profile_mem->mems[0]);
    for (i = 0; i < entries_per_set; i++) {
        offset = 0;
        for (mem_idx = 0; mem_idx < profile_mem->mems_count; mem_idx++) {
            mem = profile_mem->mems[mem_idx];
            entry_words = soc_mem_entry_words(unit, mem);
            entry_p = (uint32 *)entries[mem_idx] + i * entry_words;
            cache_p = profile_mem->entries[cache_index + i].cache_p + offset;

            /* Insert the new entries into profile table */
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                              free_index + i, entry_p));

            /* Copy entry into the software cache. */
            sal_memcpy(cache_p, entry_p, entry_words * sizeof(uint32));
            offset += entry_words;
        }
        profile_mem->entries[cache_index + i].ref_count++;
        profile_mem->entries[cache_index + i].entries_per_set =
            entries_per_set;
    }
    *index = free_index;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_mem_delete
 * Purpose:
 *      Delete the reference to the cached entry at the specified index.
 *
 * Parameters:
 *      unit        - (IN)  Unit
 *      profile_mem - (IN)  Pointer to profile memory structure
 *      index       - (IN)  Base index for the entires allocated in HW
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_delete(int unit,
                       soc_profile_mem_t *profile_mem,
                       uint32 index)
{
    soc_mem_t mem;
    int cache_index, num_entries, entries_per_set, mem_idx, i;

    if (profile_mem == NULL || profile_mem->mems == NULL ||
        profile_mem->entries == NULL) {
        return SOC_E_INIT;
    }

    num_entries = soc_mem_index_count(unit, profile_mem->mems[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if (0 == soc_mem_index_valid(unit, profile_mem->mems[0], (int)index)) {
        return SOC_E_PARAM;
    }

    cache_index = index - soc_mem_index_min(unit, profile_mem->mems[0]);
    entries_per_set = profile_mem->entries[cache_index].entries_per_set;
    if (index % entries_per_set) {
        return SOC_E_INTERNAL;
    }

    profile_mem->entries[cache_index].ref_count--;
    for (i = 1; i < entries_per_set; i++) {
        profile_mem->entries[cache_index + i].ref_count--;
        if (profile_mem->entries[cache_index + i].ref_count !=
            profile_mem->entries[cache_index].ref_count) {
            return SOC_E_INTERNAL;
        }
    }

    if (profile_mem->entries[cache_index].ref_count) {
        return SOC_E_NONE;
    }

    for (i = 0; i < entries_per_set; i++) {
        for (mem_idx = 0; mem_idx < profile_mem->mems_count; mem_idx++) {
            /* Clear the entry in the HW */
            mem = profile_mem->mems[mem_idx];
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                index + i, soc_mem_entry_null(unit, mem)));
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_profile_mem_ref_count_get
 * Purpose:
 *      Get the reference count of the cached entry at the specified index.
 *
 * Parameters:
 *      unit        - (IN)  Unit
 *      profile_mem - (IN)  Pointer to profile memory structure
 *      index       - (IN)  Base index for the entires allocated in HW
 *      ref_count   - (OUT) Reference count.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_ref_count_get(int unit,
                              soc_profile_mem_t *profile_mem,
                              uint32 index, int *ref_count)
{
    int cache_index, num_entries, entries_per_set;

    if (profile_mem == NULL || profile_mem->mems == NULL ||
        profile_mem->entries == NULL || ref_count == NULL) {
        return SOC_E_INIT;
    }

    num_entries = soc_mem_index_count(unit, profile_mem->mems[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if (0 == soc_mem_index_valid(unit, profile_mem->mems[0], (int)index)) {
        return SOC_E_PARAM;
    }

    cache_index = index - soc_mem_index_min(unit, profile_mem->mems[0]);
    entries_per_set = profile_mem->entries[cache_index].entries_per_set;
    if (index % entries_per_set) {
        return SOC_E_INTERNAL;
    }

    *ref_count = profile_mem->entries[cache_index].ref_count;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_profile_reg_t_init
 * Purpose:
 *      Initialize a soc_profile_reg_t structure.
 *
 * Parameters:
 *      profile_reg - (IN) Pointer to profile register structure
 * Returns:
 *      void
 */
void
soc_profile_reg_t_init(soc_profile_reg_t *profile_reg)
{
    if (NULL != profile_reg) {
        sal_memset(profile_reg, 0, sizeof(soc_profile_reg_t));
    }
}

/*
 * Function:
 *      soc_profile_reg_create
 * Purpose:
 *      Create a shadow copy and refcounts of a profile table.
 *      If called during WARM BOOT, the shadow copy is populated with
 *      the HW contents, otherwise, both the shadow copy and the
 *      HW entries are cleared.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      regs        - (IN) Pointer to register id array
 *      regs_count  - (IN) Number of entries in register id array
 *      profile_reg - (IN) Pointer to profile register structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_create(int unit,
                       soc_reg_t *regs,
                       int regs_count,
                       soc_profile_reg_t *profile_reg)
{
    soc_reg_t reg;
    int rv;
    int num_entries, reg_idx, i;
    int alloc_size;
    uint64 *cache_p;
    uint32 addr;
    uint64 rval;

    if (profile_reg == NULL) {
        return SOC_E_INIT;
    }

    if (regs == NULL || !regs_count) {
        return SOC_E_PARAM;
    }

    num_entries = SOC_REG_NUMELS(unit, regs[0]);
    if (num_entries <= 0) {
        return SOC_E_BADID;
    }
    alloc_size = num_entries * \
        (sizeof(soc_profile_reg_entry_t) + regs_count * sizeof(uint64)) +
         regs_count * sizeof(soc_reg_t); 

    /* If profile_reg->regs is NULL, init the profile_reg for the first
     * time, otherwise simply check for null pointers */
    if (profile_reg->regs != NULL) {
        if (profile_reg->entries == NULL) {
            return SOC_E_INTERNAL;
        }
    } else {
        profile_reg->entries = sal_alloc(alloc_size,  "Profile Reg Entries");
        if (profile_reg->entries == NULL) {
            return SOC_E_MEMORY;
        }
    }
    sal_memset(profile_reg->entries, 0, alloc_size);

    cache_p = (uint64 *)&profile_reg->entries[num_entries];
    for (i = 0; i < num_entries; i++) {
        profile_reg->entries[i].cache_p = cache_p;
        cache_p += regs_count;
    }
    profile_reg->regs = (soc_reg_t *)cache_p;
    for (reg_idx = 0; reg_idx < regs_count; reg_idx++) {
        profile_reg->regs[reg_idx] = regs[reg_idx];
    }
    profile_reg->regs_count = regs_count;

    if (SOC_WARM_BOOT(unit)) {
        for (i = 0; i < num_entries; i++) {
            cache_p = profile_reg->entries[i].cache_p;
            for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
                reg = profile_reg->regs[reg_idx];
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, i);
                rv = soc_reg_read(unit, reg, addr, cache_p);
                if (rv < 0) {
                    sal_free(profile_reg->entries);
                    return rv;
                }
                cache_p++;
            }
        }
    } else {
        /* Clear HW memory */
        COMPILER_64_ZERO(rval);
        for (i = 0; i < num_entries; i++) {
            for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
                reg = profile_reg->regs[reg_idx];
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, i);
                rv = soc_reg_write(unit, reg, addr, rval);
                if (rv < 0) {
                    sal_free(profile_reg->entries);
                    return rv;
                }
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_reg_destroy
 * Purpose:
 *      Destroy the shadow copy and refcounts of a profile table.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile_reg - (IN) Pointer to profile register structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_destroy(int unit,
                        soc_profile_reg_t *profile_reg)
{
    if (profile_reg != NULL) {
        if (profile_reg->entries != NULL) {
            sal_free(profile_reg->entries);
        }
        profile_reg->regs = NULL;
        return SOC_E_NONE;
    }
    return SOC_E_PARAM;
}

/*
 * Function:
 *      soc_profile_reg_add
 * Purpose:
 *      Add a set of entries (one or more enrties) to a profile table. This
 *      routine searches for a matching set in the profile table. If a matching
 *      set is found, the ref count for that entry is incremented and
 *      its base index is returned. If a matching set is not found and a free
 *      set is found, the HW table is updated, the ref count is incremented,
 *      and the base index of the set is returned. If no free set is found, an
 *      error is returned
 *
 * Parameters:
 *      unit        - (IN)  Unit
 *      profile_reg - (IN)  Pointer to profile register structure
 *      entries     - (IN)  Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index       - (OUT) Base index for the entires allocated in HW
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_add(int unit,
                    soc_profile_reg_t *profile_reg,
                    uint64 **entries,
                    int entries_per_set,
                    uint32 *index)
{
    soc_reg_t reg;
    int base, free_index;
    int num_entries, reg_idx, i;
    uint32 addr;
    uint64 *cache_p, *entry_p;

    if (profile_reg == NULL || profile_reg->regs == NULL ||
        profile_reg->entries == NULL) {
        return SOC_E_INIT;
    }

    num_entries = SOC_REG_NUMELS(unit, profile_reg->regs[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if (entries == NULL || index == NULL ||
        entries_per_set < 1 || entries_per_set > num_entries) {
        return SOC_E_PARAM;
    }

    /*
     * Search for an existing entry that has the same configuration.
     */
    free_index = -1;
    for (base = 0; base < num_entries; base += entries_per_set) {
        /* Skip unused entries. */
        if (!profile_reg->entries[base].ref_count) {
            if (free_index == -1) {
                /* Preserve location of free slot. */
                free_index = base;
                for (i = 1; i < entries_per_set; i++) {
                    if (profile_reg->entries[base + i].ref_count) {
                        free_index = -1;
                        break;
                    }
                }
            }
            continue;
        }

        for (i = 0; i < entries_per_set; i++) {
            if (profile_reg->entries[base + i].entries_per_set !=
                entries_per_set) {
                break;
            }
            for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
                reg = profile_reg->regs[reg_idx];
                entry_p = &entries[reg_idx][i];
                cache_p = &profile_reg->entries[base + i].cache_p[reg_idx];
                if (COMPILER_64_NE(*cache_p, *entry_p)) {
                    break;
                }
            }
            if (reg_idx != profile_reg->regs_count) {
                break;
            }
        }
        if (i == entries_per_set) {
            for (i = 0; i < entries_per_set; i++) {
                profile_reg->entries[base + i].ref_count++;
            }
            *index = base;
            return SOC_E_NONE;
        }
        if (profile_reg->entries[base].entries_per_set > entries_per_set) {
            base += profile_reg->entries[base].entries_per_set -
                    entries_per_set;
        }
    }

    if (free_index == -1) {
        return SOC_E_RESOURCE;
    }

    for (i = 0; i < entries_per_set; i++) {
        for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
            reg = profile_reg->regs[reg_idx];
            entry_p = &entries[reg_idx][i];
            cache_p = &profile_reg->entries[free_index + i].cache_p[reg_idx];

            /* Insert the new entries into profile table */
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, free_index + i);
            SOC_IF_ERROR_RETURN(soc_reg_write(unit, reg, addr, *entry_p));

            /* Copy entry into the software cache. */
            *cache_p = *entry_p;
        }
        profile_reg->entries[free_index + i].ref_count++;
        profile_reg->entries[free_index + i].entries_per_set = entries_per_set;
    }
    *index = free_index;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_reg_delete
 * Purpose:
 *      Delete the reference to the cached entry at the specified index.
 *
 * Parameters:
 *      unit        - (IN)  Unit
 *      profile_reg - (IN)  Pointer to profile register structure
 *      index       - (IN)  Base index for the entires allocated in HW
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_delete(int unit,
                       soc_profile_reg_t *profile_reg,
                       uint32 index)
{
    soc_reg_t reg;
    int entries_per_set, reg_idx, i;
    uint32 addr, num_entries;
    uint64 rval;

    if (profile_reg == NULL || profile_reg->regs == NULL ||
        profile_reg->entries == NULL) {
        return SOC_E_INIT;
    }

    num_entries = SOC_REG_NUMELS(unit, profile_reg->regs[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if (index >= num_entries) {
        return SOC_E_PARAM;
    }

    entries_per_set = profile_reg->entries[index].entries_per_set;
    if (index % entries_per_set) {
        return SOC_E_INTERNAL;
    }

    profile_reg->entries[index].ref_count--;
    for (i = 1; i < entries_per_set; i++) {
        profile_reg->entries[index + i].ref_count--;
        if (profile_reg->entries[index + i].ref_count !=
            profile_reg->entries[index].ref_count) {
            return SOC_E_INTERNAL;
        }
    }

    if (profile_reg->entries[index].ref_count) {
        return SOC_E_NONE;
    }

    COMPILER_64_ZERO(rval);
    for (i = 0; i < entries_per_set; i++) {
        for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
            /* Clear the entry in the HW */
            reg = profile_reg->regs[reg_idx];
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, index + i);
            SOC_IF_ERROR_RETURN(soc_reg_write(unit, reg, addr, rval));
        }
    }

    return SOC_E_NONE;
}
