/*
 * $Id: arl.c,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        arl.c
 *      
 * Provides:
 *      soc_internal_arl_cnt
 *      soc_internal_arl_init
 *      soc_internal_arl_ins
 *      soc_internal_arl_del
 *      soc_internal_arl_lku
 *      
 * Requires:    
 */

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/drv.h>

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"

static int arl_entry_count[3];		/* Entry count per copy */

int soc_internal_arl_cnt(pcid_info_t *pcid_info, int copyno, int cnt)
{
    uint32 tmp[SOC_MAX_MEM_WORDS];

    debugk(DK_VERBOSE,
           "Set ARL_CNT.%d to %d (%d entries)\n",
           copyno, cnt + 1, cnt);

    memset(tmp, 0, sizeof (tmp));
    tmp[0] = cnt + 1;
    soc_internal_write_mem(pcid_info,
			   soc_mem_addr(pcid_info->unit, ARL_CNTm, copyno, 0),
			   tmp);

    return 0;
}

int soc_internal_arl_init(pcid_info_t *pcid_info)
{
    int index, index_min, index_max;
    int copyno;

    debugk(DK_VERBOSE, "ARL Init\n");

    index_min = soc_mem_index_min(pcid_info->unit, ARLm);
    index_max = soc_mem_index_max(pcid_info->unit, ARLm);

    SOC_MEM_BLOCK_ITER(pcid_info->unit, ARLm, copyno) {
        for (index = index_min; index <= index_max; index++) {
            soc_internal_write_mem(pcid_info,
                 soc_mem_addr(pcid_info->unit, ARLm, copyno, index),
                 soc_mem_entry_null(pcid_info->unit, ARLm));
        }

        arl_entry_count[copyno] = 0;

        soc_internal_arl_cnt(pcid_info, copyno, arl_entry_count[copyno]);
    }

    return 0;
}

int soc_internal_arl_ins(pcid_info_t *pcid_info, arl_entry_t *entry)
{
    int index, index_min, index_max;
    int copyno;
    int ins_index, t;
    uint32 tmp[SOC_MAX_MEM_WORDS];

    debugk(DK_VERBOSE, "ARL Insert\n");

    index_min = soc_mem_index_min(pcid_info->unit, ARLm);
    index_max = soc_mem_index_max(pcid_info->unit, ARLm);

    SOC_MEM_BLOCK_ITER(pcid_info->unit, ARLm, copyno) {
        if (arl_entry_count[copyno] == index_max) {
            /*
             * Drop on table full.  This is the behavior when the last
             * entry is static, and having the last entry be static is a
             * workaround requirement for all chips.
             */
            continue;
        }

        /*
         * First a short-cut for insertion at end of table.
         * Otherwise it takes forever to fill it during tests.
         * This does not help if the static FFE entry is there.
         */

        if (arl_entry_count[copyno] > 0) {
            ins_index = index_min + arl_entry_count[copyno];

            soc_internal_read_mem(pcid_info,
              soc_mem_addr(pcid_info->unit, ARLm, copyno, ins_index - 1),
                              tmp);

            if (soc_mem_compare_key(pcid_info->unit, ARLm, entry, tmp)
                > 0) {
                debugk(DK_VERBOSE, "Short cut\n");
                goto found;
            }
        }

        t = 1;

        for (ins_index = index_min;
             ins_index <= index_min + arl_entry_count[copyno] - 1;
             ins_index++) {
            soc_internal_read_mem(pcid_info,
                soc_mem_addr(pcid_info->unit, ARLm, copyno, ins_index),
                              tmp);
            t = soc_mem_compare_key(pcid_info->unit, ARLm, entry, tmp);
            if (t <= 0) {
                break;
            }
        }

        if (t == 0) {
            continue;                   /* Overwrites not implemented */
        }

        /* First unused entry at end of table */
        index = index_min + arl_entry_count[copyno];

        while (index > ins_index) {
            /* Move up one entry */
            soc_internal_read_mem(pcid_info,
                soc_mem_addr(pcid_info->unit, ARLm, copyno, index - 1),
                              tmp);
            soc_internal_write_mem(pcid_info,
                soc_mem_addr(pcid_info->unit, ARLm, copyno, index), tmp);
            index--;
        }

    found:

        soc_internal_write_mem(pcid_info,
            soc_mem_addr(pcid_info->unit, ARLm, copyno, ins_index),
                           (uint32 *) entry);

        arl_entry_count[copyno]++;

        soc_internal_arl_cnt(pcid_info, copyno, arl_entry_count[copyno]);
    }

    return 0;
}

int soc_internal_arl_del(pcid_info_t *pcid_info, arl_entry_t *entry)
{
    int index, index_min, index_max;
    int copyno;
    int del_index, t;
    uint32 tmp[SOC_MAX_MEM_WORDS];

    debugk(DK_VERBOSE, "ARL Delete\n");

    index_min = soc_mem_index_min(pcid_info->unit, ARLm);
    index_max = soc_mem_index_max(pcid_info->unit, ARLm);

    SOC_MEM_BLOCK_ITER(pcid_info->unit, ARLm, copyno) {
        /*
         * First a short-cut for deletion at end of table.
         * Otherwise it takes forever to clear it during tests.
         * This does not help if the static FFE entry is there.
         */

        if (arl_entry_count[copyno] > 0) {
            index = index_min + arl_entry_count[copyno] - 1;

            soc_internal_read_mem(pcid_info,
                soc_mem_addr(pcid_info->unit, ARLm, copyno, index), tmp);

            if (soc_mem_compare_key(pcid_info->unit, ARLm, entry, tmp)
                == 0) {
                debugk(DK_VERBOSE, "Short cut\n");
                goto found;
            }
        }

        t = 1;

        for (del_index = index_min;
             del_index <= index_min + arl_entry_count[copyno] - 1;
             del_index++) {
            soc_internal_read_mem(pcid_info,
                soc_mem_addr(pcid_info->unit, ARLm, copyno, del_index),
                              tmp);
            t = soc_mem_compare_key(pcid_info->unit, ARLm, entry, tmp);
            if (t <= 0) {
                break;
            }
        }

        if (t != 0) {
            continue;                   /* Entry not found */
        }

        for (index = del_index;
             index < index_min + arl_entry_count[copyno] - 1;
             index++) {
            /* Move down one entry */
            soc_internal_read_mem(pcid_info,
                soc_mem_addr(pcid_info->unit, ARLm, copyno, index + 1),
                              tmp);
            soc_internal_write_mem(pcid_info,
                soc_mem_addr(pcid_info->unit, ARLm, copyno, index), tmp);
        }

    found:

        soc_internal_write_mem(pcid_info,
            soc_mem_addr(pcid_info->unit, ARLm, copyno, index),
                           soc_mem_entry_null(pcid_info->unit, ARLm));

        arl_entry_count[copyno]--;

        soc_internal_arl_cnt(pcid_info, copyno, arl_entry_count[copyno]);
    }

    return 0;
}

int soc_internal_arl_lkup(pcid_info_t *pcid_info, int copyno,
                          arl_entry_t *entry_lookup,
                          arl_entry_t *entry_result)
{
    int index, index_min, index_last;
    uint32 tmp[SOC_MAX_MEM_WORDS];

    debugk(DK_VERBOSE, "ARL Lookup\n");

    if (soc_feature(pcid_info->unit, soc_feature_arl_lookup_retry)) {
        /* Simulate periodic retry reply */
        if (random() % 8 == 0) {
            memset(entry_result, 0, 12);
            debugk(DK_VERBOSE, "Simulate not found\n");
            return 0;
        }
    }

    index_min = soc_mem_index_min(pcid_info->unit, ARLm);
    index_last = index_min + arl_entry_count[0] - 1;

    for (index = index_min; index <= index_last; index++) {
        soc_internal_read_mem(pcid_info,
                          soc_mem_addr(pcid_info->unit, ARLm, 0, index),
                          tmp);

        if (soc_mem_compare_key(pcid_info->unit, ARLm, entry_lookup, tmp)
            == 0) {
            memcpy(entry_result, tmp, 12);
            debugk(DK_VERBOSE, "Found at index %d\n", index);
            return 0;
        }
    }

    debugk(DK_VERBOSE, "Not found\n");

    memset(entry_result, 0xff, 12);

    return 0;
}
