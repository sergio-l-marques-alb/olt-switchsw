/*******************************************************************************
 *
 * Copyright 2014-2019 Broadcom Corporation
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

#include <stdio.h>

#include "kaps_portable.h"
#include "kaps_handle.h"
#include "kaps_key_internal.h"
#include "kaps_ab.h"
#include "kaps_resource.h"

KAPS_INLINE static void
kaps_in_tag(
    FILE * fp,
    uint32_t s,
    char *xml_tag)
{
    kaps_fprintf(fp, "    <%s>%d</%s>\n", xml_tag, s, xml_tag);
}

KAPS_INLINE static void
kaps_in_tag_no_spaces(
    FILE * fp,
    uint32_t s,
    char *xml_tag)
{
    kaps_fprintf(fp, "<%s>%d</%s>\n", xml_tag, s, xml_tag);
}

KAPS_INLINE static void
kaps_start_tag(
    FILE * fp,
    char *s)
{
    kaps_fprintf(fp, "<%s>\n", s);
}

KAPS_INLINE static void
kaps_end_tag(
    FILE * fp,
    char *s)
{
    kaps_fprintf(fp, "</%s>\n", s);
}

KAPS_INLINE static void
kaps_print_spaces(
    FILE * fp)
{
    kaps_fprintf(fp, "    ");
}

KAPS_INLINE static void
print_tabs(
    FILE * fp,
    int32_t num_tabs)
{
    int32_t i;

    for (i = 0; i < num_tabs; i++)
        kaps_fprintf(fp, "    ");
}

KAPS_INLINE static void
print_lines(
    FILE * fp,
    int32_t num_lines)
{
    int32_t i;

    for (i = 0; i < num_lines; i++)
        kaps_fprintf(fp, "\n");
}

KAPS_INLINE static void
self_end(
    FILE * fp,
    char *s)
{
    kaps_fprintf(fp, "<%s/>\n", s);
}

KAPS_INLINE static void
start_comment(
    FILE * fp)
{
    kaps_fprintf(fp, "<!--\n");
}

KAPS_INLINE static void
end_comment(
    FILE * fp)
{
    kaps_fprintf(fp, "-->\n");
}

struct instruction_info
{
    struct kaps_db *db;
    uint16_t result_id;
    uint16_t ad_transmit_start_byte;
    uint16_t ad_size_to_transmit;
};

/*#define KAPS_GET_DB_PARENT(db) ((db->parent) ? (db->parent) : (db))*/

static char *
kaps_get_db_name(
    struct kaps_db *db)
{
    /* (db->common_info->dummy_db)*/
    /* return "dummy";*/
    switch (db->type)
    {
        case KAPS_DB_ACL:
            if (db->common_info->mapped_to_acl)
                return "lpm";
            else
                return "acl";
        case KAPS_DB_LPM:
            return "lpm";
        case KAPS_DB_EM:
            return "em";
        default:
            kaps_sassert(0);
    }

    return NULL;
}

static void
print_key(
    FILE * fp,
    struct kaps_key *key,
    uint8_t in_xml_format)
{
    struct kaps_key_field *field = key->first_field;
    uint32_t fcount = 0, count = 0, next_offset = 0;
    char field_name[1000];

    if (in_xml_format)
    {
        kaps_print_spaces(fp);
        kaps_start_tag(fp, "key");
    }

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_KEY_FIELD_HOLE && !field->is_usr_bmr && !field->is_padding_field)
            continue;
        fcount++;
    }
    do
    {
        for (field = key->first_field; field; field = field->next)
        {
            if (next_offset != field->orig_offset_1)
                continue;
            count++;
            next_offset = next_offset + field->width_1;

            strncpy(field_name, field->name, 999);
            if (1)
            {
                int32_t i;
                for (i = 0; i < strlen(field_name); i++)
                {
                    if (field_name[i] == ':')
                        field_name[i] = '_';
                }
            }

            if (in_xml_format)
            {
                kaps_print_spaces(fp);
                kaps_fprintf(fp, "    <%s type='%s'>%d</%s>\n", field_name, kaps_key_get_type_internal(field->type),
                             field->width_1, field_name);
            }
            else
            {
                if (count == fcount)
                    kaps_fprintf(fp, "(%s,%d,%s)", field_name, field->width_1, kaps_key_get_type_internal(field->type));
                else
                    kaps_fprintf(fp, "(%s,%d,%s) ", field_name, field->width_1,
                                 kaps_key_get_type_internal(field->type));
            }
            break;
        }
    }
    while (count < fcount);

    if (in_xml_format)
    {
        for (field = key->first_overlay_field; field; field = field->next)
        {
            kaps_print_spaces(fp);
            strncpy(field_name, field->name, 999);
            if (1)
            {
                int32_t i;
                for (i = 0; i < strlen(field_name); i++)
                {
                    if (field_name[i] == ':')
                        field_name[i] = '_';
                }
            }
            kaps_fprintf(fp, "    <%s type='%s' offset='%d'>%d</%s>\n", field_name,
                         kaps_key_get_type_internal(field->type), field->offset_1, field->width_1, field_name);
        }
        kaps_print_spaces(fp);
        kaps_end_tag(fp, "key");
    }
    else
    {
        kaps_fprintf(fp, "\n");
    }
}

static void
kaps_print_mask(
    FILE * fp,
    uint32_t key_width,
    uint32_t width,
    uint32_t len)
{
    uint8_t i = 0, mask[80] = { 0, };

    len = (width * KAPS_BITS_IN_BYTE) - len;
    for (i = width - ((len + 7) / 8); i < width; i++)
    {
        if (!len)
            mask[i] = 0x00;
        else if (!(len % 8))
            mask[i] = 0xff;
        else
        {
            mask[i] = 0xff >> (8 - (len % 8));
            len -= len % 8;
            continue;
        }
        if (len)
            len -= 8;
    }
    for (i = 0; i < width; i++)
    {
        if (i == width - 1)
            kaps_fprintf(fp, "%.2x", mask[i]);
        else
            kaps_fprintf(fp, "%.2x.", mask[i]);
    }
}

static void
kaps_print_field(
    FILE * fp,
    struct kaps_db *db,
    struct kaps_entry *e,
    struct kaps_key_field *field,
    uint8_t * data,
    uint8_t * mask,
    uint16_t length)
{
    uint32_t width_8 = 0, prev_offset = 0, i = 0;
    uint32_t key_width = db->width.key_width_1 / KAPS_BITS_IN_BYTE;
    char *bits[] = { "0", "1", "*" };
    int nbits = field->width_1;
    int start = 0;

    if (field->offset_1)
        prev_offset = (field->offset_1 / KAPS_BITS_IN_BYTE);

    switch (field->type)
    {
        case KAPS_KEY_FIELD_TABLE_ID:
            for (i = 0; i < field->width_1 / KAPS_BITS_IN_BYTE; i++)
                kaps_fprintf(fp, "%.2x", data[prev_offset + i]);
            break;
        case KAPS_KEY_FIELD_EM:
        case KAPS_KEY_FIELD_PREFIX:
            width_8 = field->width_1 / KAPS_BITS_IN_BYTE;
            length -= (prev_offset * KAPS_BITS_IN_BYTE);

            /*
             * Data 
             */
            for (i = 0; i < width_8; i++)
            {
                if (i == width_8 - 1)
                    kaps_fprintf(fp, "%.2x", data[prev_offset + i]);
                else
                    kaps_fprintf(fp, "%.2x.", data[prev_offset + i]);
            }

            kaps_fprintf(fp, "/");

            /*
             * Mask
             */
            if (mask)
            {
                for (i = 0; i < field->width_1 / KAPS_BITS_IN_BYTE; i++)
                {
                    if (i == width_8 - 1)
                        kaps_fprintf(fp, "%.2x", mask[prev_offset + i]);
                    else
                        kaps_fprintf(fp, "%.2x.", mask[prev_offset + i]);
                }
            }
            else
            {
                kaps_print_mask(fp, key_width, width_8, length);
            }
            break;
        case KAPS_KEY_FIELD_TERNARY:
            start = prev_offset * KAPS_BITS_IN_BYTE;
            for (i = start; i < (start + nbits); i++)
            {
                if (!(i % 4))
                    kaps_fprintf(fp, " ");
                kaps_fprintf(fp, "%s", bits[kaps_array_get_bit(data, mask, i)]);
            }
            break;
        default:
            return;
    }
}

static void
kaps_print_lpm_entry(
    FILE * f,
    struct kaps_db *db,
    struct kaps_entry *e)
{
    struct kaps_key_field *db_f = db->key->first_field;
    struct kaps_entry_info info;
    uint32_t fcount = 0, count = 0, next_offset = 0;

    if (db->device->flags & KAPS_DEVICE_ISSU)
        kaps_entry_get_info(db, KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, e->user_handle), &info);
    else
        kaps_entry_get_info(db, e, &info);

    for (; db_f; db_f = db_f->next)
    {
        if (db_f->type == KAPS_KEY_FIELD_HOLE)
            continue;
        fcount++;
    }

    do
    {
        for (db_f = db->key->first_field; db_f; db_f = db_f->next)
        {
            if (next_offset != db_f->orig_offset_1)
                continue;
            count++;
            next_offset = next_offset + db_f->width_1;
            kaps_print_field(f, db, e, db_f, info.data, NULL, info.prio_len);
            kaps_fprintf(f, " ");
            break;
        }
    }
    while (count < fcount);
    kaps_fprintf(f, "\n");
}

static void
kaps_print_entry(
    FILE * f,
    struct kaps_db *db,
    struct kaps_entry *e)
{
    if (db->type == KAPS_DB_LPM)
    {
        kaps_print_lpm_entry(f, db, e);
    }
}

static kaps_status
kaps_write_entries(
    struct kaps_db *db,
    FILE * f)
{
    struct kaps_entry *e = NULL;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    kaps_fprintf(f, "+KBP-STANDARD-FORMAT\n");
    print_key(f, db->key, 0);

    kaps_c_list_iter_init(&db->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        e = KAPS_DBLIST_TO_KAPS_ENTRY(el);
        if (e == NULL)
            break;
        kaps_print_entry(f, db, e);
        kaps_fprintf(f, "\n");
    }

    if (kaps_c_list_count(&db->db_pending_list) == 0)
        return KAPS_OK;

    kaps_fprintf(f, "-----------------Pending------------\n");
    kaps_c_list_iter_init(&db->db_pending_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        e = KAPS_DBLIST_TO_KAPS_ENTRY(el);

        if (e == NULL)
            break;

        kaps_print_entry(f, db, e);
        kaps_fprintf(f, "\n");
    }

    return KAPS_OK;
}

static void
kaps_print_ad_db_info(
    FILE * fp,
    struct kaps_ad_db *ad_db)
{
    kaps_print_spaces(fp);
    kaps_fprintf(fp, "<ad id='%d'>\n", ad_db->db_info.tid);
    kaps_print_spaces(fp);
    kaps_in_tag(fp, ad_db->db_info.common_info->capacity, "capacity");
    kaps_print_spaces(fp);
    kaps_in_tag(fp, ad_db->user_width_1, "width");
    kaps_print_spaces(fp);
    kaps_end_tag(fp, "ad");
    fflush(fp);
}

static void
kaps_print_xml_clone(
    FILE * fp,
    struct kaps_db *db)
{
    kaps_fprintf(fp, "<clone id='%d'>\n", db->tid);

    if (db->key)
        print_key(fp, db->key, 1);
    kaps_print_spaces(fp);
    kaps_end_tag(fp, "clone");
    fflush(fp);
}

static void
kaps_print_xml_table(
    FILE * fp,
    struct kaps_db *db,
    uint32_t dump_entries_to_file)
{
    char dbfilename[1024];
    struct kaps_db *tmp = NULL, *parent = db->parent;

    kaps_print_spaces(fp);
    kaps_fprintf(fp, "<table id='%d'>\n", db->tid);
    kaps_sprintf(dbfilename, "%s-%d-table-%d.txt", kaps_get_db_name(db), db->parent->tid, db->tid);
    kaps_fprintf(fp, "\t<input>%s</input>\n", dbfilename);

    if (dump_entries_to_file)
    {
        FILE *f = kaps_fopen(dbfilename, "w+");

        if (f == NULL)
        {
            kaps_printf("UNABLE TO OPEN THE FILE %s\n", dbfilename);
            return;
        }

        kaps_write_entries(db, f);
        kaps_fclose(f);
    }

    if (db->key)
        print_key(fp, db->key, 1);
    kaps_print_spaces(fp);
    kaps_print_spaces(fp);

    for (tmp = parent->next_tab; tmp; tmp = tmp->next_tab)
    {
        if (tmp->is_clone && tmp->clone_of == db)
            kaps_print_xml_clone(fp, tmp);
    }
    kaps_end_tag(fp, "table");
}

static kaps_status
kaps_print_all_db_properties(
    struct kaps_db *db,
    FILE * fp)
{
    kaps_in_tag(fp, db->common_info->capacity, "capacity");
    if (db->common_info->capacity < kaps_c_list_count(&db->db_list))
        kaps_in_tag(fp, kaps_c_list_count(&db->db_list), "parse_only");

    kaps_in_tag(fp, kaps_db_get_algorithmic(db), "kaps_prop_algorithmic");

    if (db->common_info->user_specified)
    {
        if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
        {
            kaps_in_tag(fp, db->common_info->num_ab, "dba");
            kaps_in_tag(fp, db->common_info->uda_mb, "uda");
        }
        else if (db->type == KAPS_DB_AD)
        {
            kaps_in_tag(fp, db->common_info->uda_mb, "uda");
        }
    }

    if (db->common_info->is_xl_db)
        kaps_in_tag(fp, db->common_info->is_xl_db, "kaps_prop_xl_db");
    if (db->common_info->locality)
        kaps_in_tag(fp, db->common_info->locality, "kaps_prop_locality");

    if (db->common_info->custom_index)
        kaps_fprintf(fp, "    <kaps_prop_index_range min='%d' max='%d'/>\n",
                     db->common_info->index_range_min, db->common_info->index_range_max);

    if (db->common_info->defer_deletes_to_install)
        kaps_in_tag(fp, db->common_info->defer_deletes_to_install, "kaps_prop_defer_deletes");
    if (db->common_info->enable_db_compaction)
        kaps_in_tag(fp, db->common_info->enable_db_compaction, "kaps_prop_enable_db_compaction");
    if (db->has_counters)
        kaps_in_tag(fp, db->has_counters, "kaps_prop_enable_counters");
    if (db->common_info->is_replicate_db_user_specified)
        kaps_in_tag(fp, db->common_info->replicate_db, "kaps_prop_replicate_db");
    if (db->common_info->multicast_db)
        kaps_in_tag(fp, db->common_info->multicast_db, "kaps_prop_mc_db");
    if (db->common_info->meta_priority)
        kaps_in_tag(fp, db->common_info->meta_priority, "kaps_db_priority");

    if (db->type == KAPS_DB_HB)
    {
        struct kaps_hb_db *hb_db = (struct kaps_hb_db *) db;
        if (hb_db->age_count)
            kaps_in_tag(fp, hb_db->age_count, "age_count");
    }

    if (db->common_info->send_priority_over_ad)
        kaps_fprintf(fp, "    <!-- KAPS_PROP_SEND_PRIORITY_OVER_AD is set -->\n");

    if (db->common_info->user_specified_range)
        kaps_in_tag(fp, db->common_info->user_specified_range, "kaps_prop_use_mcor");
    if (db->common_info->estimated_min_priority_user_specified_value)
        kaps_in_tag(fp, db->common_info->estimated_min_priority_user_specified_value, "kaps_prop_min_priority");
    if (db->common_info->estimated_max_priority_user_specified_value)
        kaps_in_tag(fp, db->common_info->estimated_max_priority_user_specified_value, "kaps_prop_max_priority");
    if (db->common_info->max_capacity > 0)
        kaps_in_tag(fp, db->common_info->max_capacity, "kaps_prop_max_capacity");

    return KAPS_OK;
}

static kaps_status
kaps_print_db_xml_format(
    struct kaps_db *db,
    uint32_t dump_entries_to_file,
    FILE * fp)
{
    struct kaps_db *next_tab = NULL;
    struct kaps_ad_db *ad_db = NULL;
    char dbfilename[512];

    kaps_fprintf(fp, "<%s id='%d'>\n", kaps_get_db_name(db), db->tid);
    if (db->common_info->dummy_db)
    {
        kaps_fprintf(fp, "    <!-- dummy db -->\n");
    }

    kaps_print_all_db_properties(db, fp);

    kaps_sprintf(dbfilename, "%s-%d.txt", kaps_get_db_name(db), db->tid);

    kaps_fprintf(fp, "    <input>%s</input>\n", dbfilename);

    if (dump_entries_to_file)
    {
        FILE *f = kaps_fopen(dbfilename, "w+");

        if (f == NULL)
        {
            kaps_printf("UNABLE TO OPEN THE FILE %s\n", dbfilename);
            return KAPS_OK;
        }

        kaps_write_entries(db, f);
        kaps_fclose(f);
    }

    if (db->key)
        print_key(fp, db->key, 1);

    ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;
    while (ad_db)
    {
        kaps_print_ad_db_info(fp, ad_db);
        ad_db = ad_db->next;
    }

    for (next_tab = db->next_tab; next_tab; next_tab = next_tab->next_tab)
    {
        if (next_tab->is_clone)
            continue;
        kaps_print_xml_table(fp, next_tab, dump_entries_to_file);
    }

    for (next_tab = db->next_tab; next_tab; next_tab = next_tab->next_tab)
    {
        if (next_tab->is_clone && next_tab->clone_of == db)
            kaps_print_xml_clone(fp, next_tab);
    }

    kaps_fprintf(fp, "    <!-- thread %d -->\n", 2 * db->device->smt_no + db->device->core_id);

    kaps_end_tag(fp, kaps_get_db_name(db));
    return KAPS_OK;
}

static void
kaps_kaps_print_db_name_for_search(
    struct instruction_info inst_info,
    FILE * f)
{
    struct kaps_db *db = inst_info.db;
    uint32_t result_port = inst_info.result_id;

    if (!db)
    {
        kaps_fprintf(f, "    <empty/>\n");
        return;
    }
    if (db->parent)
    {
        if (db->is_clone)
        {
            if (!db->clone_of->parent)
                kaps_fprintf(f, "    <%s id='%d' clone='%d' result='%d'", kaps_get_db_name(db->parent), db->clone_of->tid,
                             db->tid, result_port);
            else
                kaps_fprintf(f, "    <%s id='%d' table='%d' clone='%d' result='%d'",
                             kaps_get_db_name(db->clone_of->parent), db->clone_of->parent->tid,
                             db->clone_of->tid, db->tid, result_port);
        }
        else
        {
            kaps_fprintf(f, "    <%s id='%d' table='%d' result='%d'", kaps_get_db_name(db->parent), db->parent->tid,
                         db->tid, result_port);
        }
    }
    else
    {
        kaps_fprintf(f, "    <%s id='%d' result='%d'", kaps_get_db_name(db), db->tid, result_port);
    }
    if (inst_info.ad_size_to_transmit)
    {
        kaps_fprintf(f, " ad_start_byte='%d' ad_num_bytes='%d'/>\n", inst_info.ad_transmit_start_byte,
                     inst_info.ad_size_to_transmit);
    }
    else
    {
        kaps_fprintf(f, "/>\n");
    }
}

static kaps_status
kaps_print_instruction_xml_format(
    struct kaps_instruction *instruction,
    FILE * fp)
{
    uint32_t i, j, num_searches, cmp3_ltr = 0, count = 0;
    struct instruction_info *desc = NULL;
    struct kaps_instruction *cmp3_pair = NULL;

    num_searches = instruction->num_searches;

    if (num_searches == 0)
    {
        return KAPS_OK;
    }

    if (instruction->type == INSTR_COMPARE3)
    {
        struct kaps_c_list_iter it;
        struct kaps_list_node *el;

        cmp3_ltr = instruction->ltr + 1;

        if (instruction->device->flags & KAPS_DEVICE_SMT)
        {
            if (instruction->device->smt_no == 0)
            {
                if (cmp3_ltr >= KAPS_HW_MAX_LTR_DUAL_PORT)
                    cmp3_ltr -= KAPS_HW_MAX_LTR_DUAL_PORT;
            }
            else
            {
                if (cmp3_ltr >= KAPS_HW_MAX_LTRS)
                    cmp3_ltr -= KAPS_HW_MAX_LTR_DUAL_PORT;
            }
        }
        else
        {
            if (cmp3_ltr >= KAPS_HW_MAX_LTRS)
                cmp3_ltr -= KAPS_HW_MAX_LTRS;
        }

        if (instruction->device->smt_no == 1)
        {
            kaps_c_list_iter_init(&instruction->device->smt->inst_list, &it);
        }
        else
        {
            kaps_c_list_iter_init(&instruction->device->inst_list, &it);
        }

        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_instruction *in = KAPS_INSTLIST_TO_ENTRY(el);

            if (in->device != instruction->device)
                continue;
            if (in->ltr == cmp3_ltr)
            {
                cmp3_pair = in;
                num_searches += in->num_searches;
            }
        }
    }
    /*
     * assert (num_searches);
     */

    kaps_fprintf(fp, "<instruction id='%d'>\n", instruction->id);

    if (instruction->master_key)
        print_key(fp, instruction->master_key, 1);

    kaps_in_tag(fp, instruction->ltr, "ltr");

    num_searches = 0;
    count = 0;
    if (cmp3_pair)
    {
        for (i = 0; i < instruction->num_searches; i++)
        {
            for (j = 0; j < cmp3_pair->num_searches; j++)
            {
                if (cmp3_pair->desc[j].db == instruction->desc[i].db)
                {
                    break;
                }
            }
            if (j != cmp3_pair->num_searches)
                count++;
        }
        num_searches = instruction->num_searches + cmp3_pair->num_searches - count;
    }
    else
    {
        num_searches = instruction->num_searches;
    }

    desc =
        instruction->device->alloc->xcalloc(instruction->device->alloc->cookie, num_searches,
                                            sizeof(struct instruction_info));
    if (!desc)
        return KAPS_OUT_OF_MEMORY;

    if (cmp3_pair)
    {
        for (i = 0; i < cmp3_pair->num_searches; i++)
        {
            desc[i].result_id = cmp3_pair->desc[i].result_id + 2;
            desc[i].db = cmp3_pair->desc[i].db;
            if (instruction->is_prop_set_xml_dump[desc[i].result_id])
            {
                desc[i].ad_transmit_start_byte = instruction->prop_byte_offset[desc[i].result_id];
                desc[i].ad_size_to_transmit = instruction->prop_num_bytes[desc[i].result_id];
            }
        }

        count = cmp3_pair->num_searches;
        for (i = 0; i < instruction->num_searches; i++)
        {
            for (j = 0; j < num_searches; j++)
            {
                if (instruction->desc[i].db == desc[j].db)
                    break;
            }
            if (j == num_searches)
            {
                desc[count].result_id = instruction->desc[i].result_id;
                desc[count].db = instruction->desc[i].db;
                if (instruction->is_prop_set_xml_dump[desc[count].result_id])
                {
                    desc[count].ad_transmit_start_byte = instruction->prop_byte_offset[desc[count].result_id];
                    desc[count].ad_size_to_transmit = instruction->prop_num_bytes[desc[count].result_id];
                }
                count++;
            }
        }
    }
    else
    {

        count = instruction->num_searches;
        if (instruction->device->type == KAPS_DEVICE_KAPS)
        {
            uint32_t srch_no = 0;

            for (i = 0; i < instruction->device->max_num_searches; i++)
            {

                if (instruction->desc[i].db)
                {
                    desc[srch_no].result_id = instruction->desc[i].result_id;
                    desc[srch_no].db = instruction->desc[i].db;
                    if (instruction->is_prop_set_xml_dump[desc[srch_no].result_id])
                    {
                        desc[srch_no].ad_transmit_start_byte = instruction->prop_byte_offset[desc[srch_no].result_id];
                        desc[srch_no].ad_size_to_transmit = instruction->prop_num_bytes[desc[srch_no].result_id];
                    }
                    srch_no++;
                }
            }
            kaps_sassert(srch_no == count);

        }
        else
        {
            for (i = 0; i < instruction->num_searches; i++)
            {

                desc[i].result_id = instruction->desc[i].result_id;
                desc[i].db = instruction->desc[i].db;
                if (instruction->is_prop_set_xml_dump[desc[i].result_id])
                {
                    desc[i].ad_transmit_start_byte = instruction->prop_byte_offset[desc[i].result_id];
                    desc[i].ad_size_to_transmit = instruction->prop_num_bytes[desc[i].result_id];
                }
            }
        }
    }

    for (i = 0; i < num_searches; i++)
    {
        struct instruction_info search_info = desc[i];

        kaps_kaps_print_db_name_for_search(search_info, fp);
    }

    if (instruction->device->flags & KAPS_DEVICE_SMT)
        kaps_in_tag(fp, instruction->device->smt_no, "thread");

    kaps_end_tag(fp, "instruction");
    instruction->device->alloc->xfree(instruction->device->alloc->cookie, desc);
    return KAPS_OK;
}

static kaps_status
kaps_dump_device_details(
    struct kaps_device *device,
    FILE * fp)
{
    kaps_fprintf(fp, "\n<!--  Device Info: ");

    if (device->type == KAPS_DEVICE_KAPS)
    {
        kaps_fprintf(fp, "KAPS ");

        if (device->id == KAPS_DEFAULT_DEVICE_ID)
        {
            kaps_fprintf(fp, "Jericho ");
        }
        else if (device->id == KAPS_QUMRAN_DEVICE_ID)
        {
            kaps_fprintf(fp, "Qumran ");
        }
        else if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
        {
            kaps_fprintf(fp, "JR+ ");
        }
        else if (device->id == KAPS_QUX_DEVICE_ID)
        {
            kaps_fprintf(fp, "QUX ");
        }
        else if (device->id == KAPS_JERICHO_2_DEVICE_ID)
        {
            kaps_fprintf(fp, "JR2 ");
        }

        kaps_fprintf(fp, ", Subtype = %d", device->silicon_sub_type);
    }

    kaps_fprintf(fp, " -->\n");

    return KAPS_OK;
}

static kaps_status
kaps_print_all_device_properties(
    struct kaps_device *device,
    FILE * fp)
{
    if (device->is_counter_scrub_thread)
        kaps_in_tag(fp, device->is_counter_scrub_thread, "kaps_device_prop_counter_compression");

    if (device->prop.instruction_latency)
        kaps_in_tag(fp, device->prop.instruction_latency, "kaps_device_prop_inst_latency");

    if (device->prop.is_broadcast_at_xpt)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_BROADCAST_AT_XPT is set as: %d -->\n", device->prop.is_broadcast_at_xpt);

    if (device->nv_ptr)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_CRASH_RECOVERY is set -->\n");

    if (device->handle_interface_errors)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_HANDLE_INTERFACE_ERRORS is set as: %d -->\n",
                     device->handle_interface_errors);

    if (device->is_ltr_sm_update_needed)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_LTR_SM_UPDATE is set as: %d -->\n",
                     device->is_ltr_sm_update_needed);

    if (device->interrupt_enable)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_INTERRUPT is set as: %d -->\n", device->interrupt_enable);

    if (device->dba_err_threshold != 10)        /* Default value is 10 */
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_INTERRUPT is set as: %d -->\n", device->dba_err_threshold);

    if (device->inject_parity_err)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_INTERRUPT is set as: %d -->\n", device->inject_parity_err);

    /* if (device->prop.read_type)*/
    kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_READ_TYPE is set as: %d -->\n", device->prop.read_type);

    if (device->debug_level)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_DEBUG_LEVEL is set as: %d -->\n", device->debug_level);

    /* if (device->prop.early_block_clear)*/
    kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PRE_CLEAR_ABS is set as: %d -->\n", device->prop.early_block_clear);

    if (device->defer_ltr_writes)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_DEFER_LTR_WRITES is set as: %d -->\n", device->defer_ltr_writes);

    if (device->clear_ESR_err_bits)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_CLR_DEV_ERR_BITS is set as: %d -->\n", device->clear_ESR_err_bits);

    if (device->dump_on_assert)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_DUMP_ON_ASSERT is set as: %d -->\n", device->dump_on_assert);

    if (device->op2_clock_rate)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_CLOCK_RATE is set as: %d -->\n", device->op2_clock_rate);

    if (device->prop.enable_adv_uda_write)
        kaps_in_tag(fp, device->prop.enable_adv_uda_write, "kaps_device_prop_adv_uda_write");

    if (device->prop.return_error_on_asserts)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_RETURN_ERROR_ON_ASSERTS is set as: %d -->\n",
                     device->prop.return_error_on_asserts);

    if (device->prop.min_rxc)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_MIN_RESOURCE is set as: %d -->\n", device->prop.min_rxc);

    if (device->magic_db_entry_invalidate)
        kaps_fprintf(fp, "    <!-- KAPS_DEVICE_PROP_FORCE_INVALIDATE_LOC is set as: %d -->\n",
                     device->magic_db_entry_invalidate);

    return KAPS_OK;
}

kaps_status
kaps_device_dump(
    struct kaps_device * device,
    uint32_t dump_xml_data_with_entries,
    FILE * fp)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t db_id;

    KAPS_TRACE_IN("%p %u %p\n", device, dump_xml_data_with_entries, fp);

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    if (!fp)
        return KAPS_INVALID_ARGUMENT;

    if (device->main_dev)
        device = device->main_dev;

    if (device->main_bc_device)
        device = device->main_bc_device;

    /*
     * dont dump from if non main device /core-1x pointers are passed 
     */
    if (device->core_id % 2)
        return KAPS_OK;

    kaps_fprintf(fp, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
    kaps_dump_device_details(device, fp);
    kaps_start_tag(fp, "scenario");

    kaps_print_all_device_properties(device, fp);

    /*
     * dump databases Information 
     */
    kaps_fprintf(fp, "\n<!-- Database information -->\n");

    {
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM || db->type == KAPS_DB_EM)
            {
                kaps_print_db_xml_format(db, device->dump_xml_data_with_entries, fp);
                /* kaps_db_print(db, stdout);*/
                fflush(fp);
            }
        }
    }

    /*
     * dump instructions Information 
     */
    kaps_fprintf(fp, "\n<!-- Instruction information -->\n");
    kaps_c_list_iter_init(&device->inst_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_instruction *instruction = KAPS_INSTLIST_TO_ENTRY(el);

        if (instruction->is_cmp3_pair)
            continue;

        kaps_print_instruction_xml_format(instruction, fp);

        fflush(fp);
    }

    if (device->other_core)
    {
        kaps_c_list_iter_init(&device->other_core->inst_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_instruction *instruction = KAPS_INSTLIST_TO_ENTRY(el);

            if (instruction->is_cmp3_pair)
                continue;

            kaps_print_instruction_xml_format(instruction, fp);

            fflush(fp);
        }
    }

    kaps_end_tag(fp, "scenario");

    /*
     * log the database entries (installed) to the file if dump_xml_data_with_entries set 
     */
    if (dump_xml_data_with_entries != 0)
    {
        struct kaps_c_list_iter it_db;
        struct kaps_list_node *el_db;
        uint32_t count = 0;
        struct kaps_device *dev = device;

        kaps_fprintf(fp, "\n\n<!-- Database entries information -->\n");

        kaps_fprintf(fp, "\n<!--\n");

        /* iterate through all the databases in this device*/
        do
        {
            kaps_c_list_iter_init(&dev->db_list, &it_db);
            while ((el_db = kaps_c_list_iter_next(&it_db)) != NULL)
            {
                struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el_db);
                struct kaps_db *next_tab;

                if (db->type != KAPS_DB_ACL && db->type != KAPS_DB_LPM && db->type != KAPS_DB_EM)
                {
                    /* kaps_printf("Not an ACL / LPM / EM database. Skipping...\n");*/
                    continue;
                }
                if (db->is_bc_required && !db->is_main_bc_db)
                    continue;   /* dont print the broadcasted dbs content*/

                db_id = db->tid;
                if (db->has_tables == 0)
                {
                    /* kaps_printf("no tables in this db. id = %d\n", db_id);*/
                    ;
                }

                /* iterate through all the tables in this database*/
                for (next_tab = db; next_tab; next_tab = next_tab->next_tab)
                {
                    struct kaps_c_list_iter it_entry;
                    struct kaps_list_node *el_entry;
                    struct kaps_ad_db *ad_db = (struct kaps_ad_db *) next_tab->common_info->ad_info.ad;

                    if (next_tab->is_clone == 1)
                        continue;

                    /* iterate through all the entries in this table*/
                    if (next_tab->key)
                    {
                        kaps_fprintf(fp, "\n");
                        switch (next_tab->type)
                        {
                            case KAPS_DB_EM:
                                kaps_fprintf(fp, "file_begin: %s-%d.txt <em-entries id='%d' tid='%d'> \n",
                                             kaps_get_db_name(db), db_id, db_id, next_tab->tid);
                                kaps_fprintf(fp, "+KBP-STANDARD-FORMAT-1\n");
                                kaps_fprintf(fp, "#_EM_WITH_MASK\n");
                                break;
                            case KAPS_DB_LPM:
                                kaps_fprintf(fp, "file_begin: %s-%d.txt <lpm-entries id='%d' tid='%d'> \n",
                                             kaps_get_db_name(db), db_id, db_id, next_tab->tid);
                                kaps_fprintf(fp, "+KBP-STANDARD-FORMAT-1\n");
                                kaps_fprintf(fp, "#_EM_WITH_MASK\n");
                                break;
                            case KAPS_DB_ACL:
                                if (next_tab->common_info->mapped_to_acl == 0)
                                {
                                    kaps_fprintf(fp, "file_begin: %s-%d.txt <acl-entries id='%d' tid='%d'> \n",
                                                 kaps_get_db_name(db), db_id, db_id, next_tab->tid);
                                    kaps_fprintf(fp, "+KBP-STANDARD-FORMAT-2\n");
                                    kaps_fprintf(fp, "#_EM_WITH_MASK\n");
                                    kaps_fprintf(fp, "(PRIORITY,24,priority) ");
                                    break;
                                }
                                else
                                {
                                    kaps_fprintf(fp, "file_begin: %s-%d.txt <lpm-entries id='%d' tid='%d'> \n",
                                                 kaps_get_db_name(db), db_id, db_id, next_tab->tid);
                                    kaps_fprintf(fp, "+KBP-STANDARD-FORMAT-1\n");
                                    kaps_fprintf(fp, "#_EM_WITH_MASK\n");
                                    break;
                                }
                            default:
                                kaps_printf("Not ACL / LPM / EM  ?!\n");
                                return KAPS_OK;
                        }

                        kaps_key_print(next_tab->key, fp);

                        if (0 && ad_db != NULL) /* dont print ad unless support added to handle multi-ad dbs/ db*/
                            /* [aligned]*/
                            kaps_fprintf(fp, "(AD,%d,ad)\n", ad_db->user_width_1);
                        else
                            kaps_fprintf(fp, "\n");
                    }

                    kaps_c_list_iter_init(&next_tab->db_list, &it_entry);
                    while ((el_entry = kaps_c_list_iter_next(&it_entry)) != NULL)
                    {
                        struct kaps_entry *entry = KAPS_DBLIST_TO_KAPS_ENTRY(el_entry);
                        struct kaps_entry *entry_index;
                        struct kaps_ad *ad_index;

                        entry_index = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, entry->user_handle);

                        /* print the entry and its priority*/
                        KAPS_TRY(kaps_entry_print(next_tab, entry_index, fp));

                        /* print the entry's AD data*/
                        if (0 && entry->ad_handle != NULL)
                        {       /* dont print ad unless support added to handle multi-ad dbs/ db [aligned]*/
                            ad_index = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_ad, entry->ad_handle->user_handle);
                            kaps_ad_entry_print(ad_db, ad_index, fp);
                        }
                        kaps_fprintf(fp, "\n");
                    }

                    kaps_fprintf(fp, "file_end:\n");

                    if (next_tab->key)
                    {
                        switch (next_tab->type)
                        {
                            case KAPS_DB_EM:
                                kaps_end_tag(fp, "em-entries");
                                break;
                            case KAPS_DB_LPM:
                                kaps_end_tag(fp, "lpm-entries");
                                break;
                            case KAPS_DB_ACL:
                                if (next_tab->common_info->mapped_to_acl == 0)
                                {
                                    kaps_end_tag(fp, "acl-entries");
                                }
                                else
                                {
                                    if (next_tab->common_info->is_em == 1)
                                    {
                                        kaps_end_tag(fp, "em-entries");
                                    }
                                    else
                                    {
                                        kaps_end_tag(fp, "lpm-entries");
                                    }
                                }
                                break;
                            default:
                                return KAPS_OK;
                        }
                    }
                }       /* End: for (next_tab = db; next_tab; next_tab = next_tab->next_tab) */
            }   /* End: while ((el_db = kaps_c_list_iter_next(&it_db)) != NULL) */

            count++;
            if (count > 1)
            {
                break;
            }
            dev = dev->other_core;
        }
        while (dev);

        kaps_fprintf(fp, "\n-->\n");

    }   /* End: if (dump_xml_data_with_entries != 0) */

    return KAPS_OK;
}
