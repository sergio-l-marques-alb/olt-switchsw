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

#include "kaps_ab.h"
#include "kaps_handle.h"
#include "kaps_resource.h"
#include "kaps_key_internal.h"
#include "kaps_algo_hw.h"

static const char *table_colors[] = { "green", "aquamarine", "blue", "blueviolet",
    "brown", "burlywood", "cadetblue",
    "chartreuse", "chocolate", "coral", "cornflowerblue",
    "cornsilk", "crimson", "cyan",
    "fuchsia",
    "gold", "goldenrod", "aqua",
    "greenyellow", "honeydew", "hotpink", "indigo",
    "ivory", "khaki", "lavender", "lavenderblush", "lawngreen",
    "lemonchiffon", "lime", "limegreen", "linen", "magenta",
    "maroon", "midnightblue", "mintcream", "mistyrose", "moccasin",
    "navy", "oldlace", "olive", "olivedrab", "orange",
    "orchid", "papayawhip", "peachpuff", "peru",
    "pink", "plum", "powderblue", "purple", "red", "rosybrown",
    "royalblue", "saddlebrown", "salmon", "sandybrown", "seagreen",
    "seashell", "sienna", "silver", "skyblue", "snow", "springgreen",
    "steelblue", "tan", "teal", "thistle", "tomato", "turquoise",
    "violet", "wheat", "yellow", "yellowgreen"
};

#define MAX_PRINT_BUF_SIZE 2000
int
kaps_print_to_file(
    struct kaps_device *device,
    FILE * fp,
    const char *fmt,
    ...)
{
    int32_t r;
    va_list ap;
    char buffer[MAX_PRINT_BUF_SIZE];
    int32_t i;
    int32_t pad[] = { 105, 76, 79, 31, 157, 38, 79, 58, 187, 67, 85, 78, 12, 31, 18, 9, 89 };
    int new_val;

    if (!fp)
        return 0;

    if (device->main_dev)
    {
        device = device->main_dev;
    }

    if (device->main_bc_device)
    {
        device = device->main_bc_device;
    }

    if (device->map_print)
    {
        va_start(ap, fmt);
        r = kaps_vsnprintf(buffer, MAX_PRINT_BUF_SIZE, fmt, ap);
        if (r < 1)
            kaps_sassert(0);
        va_end(ap);
        for (i = 0; i < r; i++)
        {
            new_val = buffer[i];
            new_val = (new_val + pad[device->map_print_offset] + 105) % 255;
            kaps_fprintf(fp, "%03d", new_val);
            device->map_print_offset = (device->map_print_offset + 1) % (sizeof(pad) / sizeof(pad[0]));
        }
    }
    else
    {
        va_start(ap, fmt);
        r = kaps_vfprintf(fp, fmt, ap);
        va_end(ap);
    }

    return r;
}

struct memory_map *
resource_kaps_get_memory_map(
    struct kaps_device *device)
{
    int32_t dev_num = device->device_no;

    if (device->main_dev)
        device = device->main_dev;

    return &device->map[dev_num];
}

enum kaps_ad_type
kaps_resource_get_ad_type(
    struct kaps_db *db)
{
    return db->hw_res.db_res->ad_type;
}

void
kaps_resource_set_ad_type(
    struct kaps_db *db,
    enum kaps_ad_type ad_type)
{
    db->hw_res.db_res->ad_type = ad_type;
}

struct kaps_c_list *
kaps_resource_get_ab_list(
    struct kaps_db *db)
{
    if (db->common_info->is_cascaded && db->device->core_id == 1)
        return &db->hw_res.db_res->cas_ab_list;

    return &db->hw_res.db_res->ab_list;
}

kaps_status
kaps_resource_init_module(
    struct kaps_device * device)
{
    device->description = NULL;
    device->next_algo_color = 0;
    device->next_stats_color = 0;
    kaps_c_list_reset(&device->db_list);
    kaps_c_list_reset(&device->inst_list);

    return KAPS_OK;
}

uint32_t
kaps_resource_normalized_ad_width(
    struct kaps_device * device,
    uint32_t ad_width_1)
{
    return ad_width_1;
}


kaps_status
kaps_resource_add_database(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t id,
    uint32_t capacity,
    uint32_t width_1,
    enum kaps_db_type type)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_device *main_dev, *bc_dev;
    struct kaps_db *p_db = NULL;        /* In case of internal ad_db this points to the original ad db */

    if (device->main_dev)
        main_dev = device->main_dev;
    else
        main_dev = device;

    bc_dev = main_dev;
    if (main_dev->main_bc_device)
        bc_dev = main_dev->main_bc_device;

    /*
     * check for duplicate database ID
     */
    kaps_c_list_iter_init(&main_dev->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(el);
        uint32_t type_match = 0;

        if (tmp->type >= KAPS_DB_AD && type == tmp->type)
            type_match = 1;
        else if (type < KAPS_DB_AD && tmp->type < KAPS_DB_AD)
            type_match = 1;
        if (type_match && tmp->tid == id)
        {
            if (db->is_internal)
            {
                kaps_sassert(type == KAPS_DB_AD);
                kaps_sassert(p_db == NULL);
                p_db = tmp;
            }
            else
            {
                return KAPS_DUPLICATE_DB_ID;
            }
        }
    }

    kaps_c_list_add_tail(&main_dev->db_list, &db->node);

    db->common_info = main_dev->alloc->xcalloc(main_dev->alloc->cookie, 1, sizeof(struct kaps_db_common_info));
    if (db->common_info == NULL)
        return KAPS_OUT_OF_MEMORY;

    db->type = type;
    db->tid = id;
    db->common_info->capacity = capacity;
    db->device = device;
    if (type == KAPS_DB_AD)
    {
        uint32_t norm_width_1 = kaps_resource_normalized_ad_width(device, width_1);

        db->hw_res.ad_res =
            db->device->alloc->xcalloc(db->device->alloc->cookie, 1, sizeof(struct kaps_ad_db_hw_resource));
        if (db->hw_res.ad_res == NULL)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, db->common_info);
            return KAPS_OUT_OF_MEMORY;
        }
        db->width.ad_width_1 = norm_width_1;

    }
    else if (type == KAPS_DB_HB)
    {
        /*
         * Do Nothing 
         */
    }
    else
    {
        /*
         * Always start massively parallel for ACLs, we might negate later
         */
        db->hw_res.db_res =
            db->device->alloc->xcalloc(db->device->alloc->cookie, 1, sizeof(struct kaps_db_hw_resource));
        if (db->hw_res.db_res == NULL)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, db->common_info);
            return KAPS_OUT_OF_MEMORY;
        }

        kaps_resource_set_algorithmic(device, db, 1);

        db->hw_res.db_res->start_mcor = db->hw_res.db_res->end_mcor = -1;
    }

    db->common_info->hw_res_alloc = 1;

    /*
     * Assign colors for HTML printing
     */

    if (type == KAPS_DB_ACL || type == KAPS_DB_LPM || type == KAPS_DB_EM || type == KAPS_DB_DMA)
    {
        if (bc_dev->next_algo_color >= (sizeof(table_colors) / sizeof(char *)))
        {
            /*
             * roll over the color, we will end up re-using the
             * colors, not a big deal
             */
            bc_dev->next_algo_color = 0;
        }

        db->hw_res.db_res->color = table_colors[bc_dev->next_algo_color];
        bc_dev->next_algo_color++;
    }

    return KAPS_OK;
}

void
kaps_resource_free_database(
    struct kaps_device *device,
    struct kaps_db *db)
{
    if (db->common_info->hw_res_alloc)
    {
        if (db->type == KAPS_DB_AD)
        {
            if (db->hw_res.ad_res->hb_res)
            {
                struct kaps_hb_bank_list *bank_list = db->hw_res.ad_res->hb_res->hb_bank_list;
                struct kaps_hb_bank_list *list_next;

                while (bank_list)
                {
                    list_next = bank_list->next;
                    device->alloc->xfree(device->alloc->cookie, bank_list);
                    bank_list = list_next;
                }
                device->alloc->xfree(device->alloc->cookie, db->hw_res.ad_res->hb_res);
            }
            if (!device->nv_ptr)
                device->alloc->xfree(device->alloc->cookie, db->hw_res.ad_res);
        }
        else
        {
            device->alloc->xfree(device->alloc->cookie, db->hw_res.db_res);
        }
    }

    if (db->common_info->index_range_used)
        device->alloc->xfree(device->alloc->cookie, db->common_info->index_range_used);

    /*
     * release common_info 
     */
    device->alloc->xfree(device->alloc->cookie, db->common_info);
    if (db->description)
        device->alloc->xfree(device->alloc->cookie, db->description);
}

kaps_status
kaps_resource_db_set_key(
    struct kaps_device *device,
    struct kaps_db *db,
    struct kaps_key *key)
{
    if (db->common_info->treat_holes_as_pad)
    {
        struct kaps_key_field *f = key->first_field;

        for (; f; f = f->next)
        {
            if (f->type == KAPS_KEY_FIELD_HOLE)
            {
                f->is_usr_bmr = 0;
                f->is_padding_field = 1;
            }
        }
        key->has_user_bmr = 0;
    }

    db->key = key;
    return KAPS_OK;
}

kaps_status
kaps_resource_db_add_ad(
    struct kaps_device * device,
    struct kaps_db * db,
    struct kaps_db * ad)
{
   
    {
        /*
        * We can have only max one AD database
        */
        if (db->common_info->ad_info.ad)
            return KAPS_TOO_MANY_AD;
    }   

    /*
     * If the AD database has already been associated with
     * another database, prevent it for now.
     */
    if (ad->common_info->ad_info.db)
        return KAPS_ONE_ADDB_PER_DB;

    if (ad->common_info->capacity == 0)
        ad->common_info->capacity = db->common_info->capacity;


    ad->hw_res.ad_res->ad_type = KAPS_AD_TYPE_INPLACE;
    db->hw_res.db_res->ad_type = KAPS_AD_TYPE_INPLACE;


        
    
    if (!db->common_info->ad_info.ad)
    {
        db->common_info->ad_info.ad = ad;
        ad->common_info->ad_info.db = db;
    }
    else 
    {
        struct kaps_ad_db *tmp, *ad_db;

        /*Initialize tmp to the first ad db associated with the LPM database*/
        tmp = (struct kaps_ad_db *) (db->common_info->ad_info.ad);
        ad_db = (struct kaps_ad_db *) ad;

        KAPS_WB_HANDLE_TABLE_DESTROY(&(ad_db->db_info));
        ad->common_info->user_hdl_table_size = tmp->db_info.common_info->user_hdl_table_size;
        ad->common_info->user_hdl_to_entry = tmp->db_info.common_info->user_hdl_to_entry;

        /*Go to the last ad_db in the linked list*/
        while (tmp->next)
            tmp = tmp->next;

        /*connect the current ad_db to the end of the linked list*/
        tmp->next = ad_db;
        ad->common_info->ad_info.db = db;
    }

    return KAPS_OK;

}

kaps_status
kaps_resource_db_add_hb(
    struct kaps_device * device,
    struct kaps_db * db,
    struct kaps_db * hb_db)
{
    uint32_t is_supported;

    is_supported = 0;
    if (device->type == KAPS_DEVICE_KAPS && device->num_hb_blocks > 0)
    {
        is_supported = 1;
    }

    if (!is_supported)
        return KAPS_UNSUPPORTED;

    if ((db->type != KAPS_DB_LPM) && (db->type != KAPS_DB_ACL))
        return KAPS_UNSUPPORTED;

    if (db->common_info->hb_info.hb)
        return KAPS_TOO_MANY_HB;

    if (hb_db->common_info->hb_info.db)
        return KAPS_ONE_HB_DB_PER_DB;

    if (!db->common_info->ad_info.ad)
        return KAPS_HB_NO_AD;

    db->common_info->hb_info.hb = hb_db;
    hb_db->common_info->hb_info.db = db;

    if (device->type == KAPS_DEVICE_KAPS)
        return KAPS_OK;

    return KAPS_OK;
}







void
kaps_resource_print_db_capacity(
    FILE * fp,
    struct kaps_db *db)
{
    kaps_print_to_file(db->device, fp, " d%d c%u", db->tid, db->common_info->cur_capacity);
}

void
kaps_resource_print_ab_release_info(
    struct kaps_db *db,
    struct kaps_ab_info *ab)
{
    FILE *fp = db->device->dynamic_alloc_fp;

    if (!fp)
        return;

    kaps_print_to_file(db->device, fp, "D_1:");
    kaps_resource_print_db_capacity(fp, db);
    kaps_print_to_file(db->device, fp, " a%d\n", ab->ab_num);
}






uint32_t
kaps_resource_get_ab_usage(
    struct kaps_device * this_device,
    struct kaps_db * db)
{
    struct memory_map *mem_map = resource_kaps_get_memory_map(this_device);
    uint32_t i, num_ab;
    uint32_t db_group_id = db->db_group_id;

    if (!this_device->is_config_locked)
    {
        return db->common_info->num_ab;
    }

    num_ab = 0;
    for (i = 0; i < this_device->num_ab[db_group_id]; ++i)
    {
        if (mem_map->ab_memory[i].db == db)
        {
            num_ab += 2;
        }
    }

    return (num_ab + 1) / 2;
}



kaps_status
kaps_jr1_resource_assign_rpbs(
    struct kaps_device * device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    int32_t i, j, count;
    struct kaps_db *cur_db;
    struct kaps_ab_info *prev_ab_info;

    if (device->issu_in_progress)
    {
        for (i = 0; i < device->num_ab[0]; i++)
        {
            if (device->map->ab_memory[i].db != NULL)
            {
                cur_db = device->map->ab_memory[i].db;
               
                if (!device->map->ab_memory[i].is_dup_ab)
                    kaps_c_list_add_tail(kaps_resource_get_ab_list(cur_db), &device->map->ab_memory[i].ab_node);

                cur_db->common_info->num_ab = 1;
            }
        }
        return KAPS_OK;
    }

    /*
     * For the first time assign the AB config information to the
     * AB structure
     */
    for (i = 0; i < device->num_ab[0]; i++)
    {
        struct kaps_ab_info *ab = &device->map->ab_memory[i];
        ab->conf = KAPS_NORMAL_160;
        ab->num_slots = ab->device->hw_res->num_rows_in_rpb[i];
    }

    /*
     * Make sure that the same db is being searched at the current position in all instructions
     */
    for (i = 0; i < device->max_num_searches; i++)
    {

        /*
         * Iterate through the instructions
         */
        cur_db = NULL;
        kaps_c_list_iter_init(&device->inst_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {

            struct kaps_instruction *instr = KAPS_INSTLIST_TO_ENTRY(el);

            if (instr->desc[i].db == NULL)
                continue;

            if (cur_db == NULL)
            {
                cur_db = kaps_db_get_main_db(instr->desc[i].db);
                continue;
            }

            if (cur_db != kaps_db_get_main_db(instr->desc[i].db))
                return KAPS_CONFLICTING_SEARCH_PORTS;
        }

        if (cur_db)
        {
            /*
             * Assign AB to database 
             */
            
            if (kaps_c_list_count(&cur_db->hw_res.db_res->ab_list) == 0)
                kaps_c_list_add_tail(kaps_resource_get_ab_list(cur_db), &device->map->ab_memory[i].ab_node);
        

            device->map->ab_memory[i].db = cur_db;
            cur_db->common_info->num_ab = 1;

            cur_db->rpb_id = i;
            
        }
    }

    for (i = 0; i < device->max_num_searches; i++)
    {
        cur_db = device->map->ab_memory[i].db;
        if (!cur_db)
            continue;

        count = 1;

        for (j = 0; j < device->max_num_searches; j++)
        {
            if (!device->map->ab_memory[j].db)
                continue;

            if (i == j)
                continue;

            if (device->map->ab_memory[j].db == cur_db)
                count++;

            if (count > device->max_num_clone_parallel_lookups)
                return KAPS_EXCEEDED_MAX_PARALLEL_SEARCHES;
        }
    }

    /*
     * Assign clone ab pointers 
     */
    for (i = 0; i < device->max_num_searches; i++)
    {

        cur_db = device->map->ab_memory[i].db;
        if (!cur_db)
            continue;

        prev_ab_info = &device->map->ab_memory[i];

        for (j = i + 1; j < device->max_num_searches; j++)
        {
            if (!device->map->ab_memory[j].db)
                continue;

            if (cur_db == device->map->ab_memory[j].db)
            {
                if (!prev_ab_info->dup_ab)
                    prev_ab_info->dup_ab = &device->map->ab_memory[j];

                device->map->ab_memory[j].is_dup_ab = 1;
                cur_db->has_dup_ad = 1;
                device->map->ab_memory[j].db->has_dup_ad = 1;

                prev_ab_info = &device->map->ab_memory[j];
            }
        }
    }

    return KAPS_OK;
}
