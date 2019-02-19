/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <bcm_int/esw/keygen_md.h>

#define BCMI_KEYGEN_EXT_FINFO_COUNT (1024 * 1)

bcmi_keygen_ext_field_info_t *finfo_free_list[BCM_MAX_NUM_UNITS];

bcmi_keygen_ext_field_info_t finfo_pool[BCM_MAX_NUM_UNITS]
                                 [BCMI_KEYGEN_EXT_FINFO_COUNT];

uint8 debug_enable = 0;
uint16 finfo_free_count = 0;
uint16 finfo_used_count = 0;

#define DUMP_FINFO_COUNT(free) {  \
    if (free) {                   \
        if (debug_enable) {       \
            finfo_free_count++;   \
            finfo_used_count--;   \
        }                         \
    } else {                      \
        if (debug_enable) {       \
            finfo_free_count--;   \
            finfo_used_count++;   \
        }                         \
    }                             \
}

STATIC int
bcmi_keygen_bit_stream_get(int unit,
                     SHR_BITDCL *buffer,
                     uint16 start_bit,
                     uint16 size,
                     uint16  *bs_count,
                     uint16 *bs_offset,
                     uint16 *bs_width)
{
    int offset = -1;
    uint16 bit = 0;
    uint16 width = 0;
    uint16 end_bit = 0;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(buffer, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(bs_count, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(bs_width, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(bs_offset, BCM_E_PARAM);

    width = 0;
    offset = -1;
    *bs_count = 0;
    end_bit = start_bit + size;
    for (bit = start_bit; bit < end_bit; bit++)  {
        if (SHR_BITGET(buffer, bit)) {
           if (offset == -1) {
               offset = bit;
           }
           width++;
           if (bit == end_bit - 1) {
               bs_offset[*bs_count] = offset;
               bs_width[*bs_count] = width;
               (*bs_count)++;
           }
        } else {
           if (offset != -1) {
               bs_offset[*bs_count] = offset;
               bs_width[*bs_count] = width;
               (*bs_count)++;
               offset = -1;
               width = 0;
           }
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

#define ORDER_TYPE_ASCENDING     (1 << 0)
#define ORDER_TYPE_DESCENDING    (1 << 1)
#define ORDER_TYPE_END_OF_LIST   (1 << 2)
#define ORDER_TYPE_START_OF_LIST (1 << 3)

STATIC int
bcmi_keygen_ext_finfo_node_add(int unit,
                  uint8 order_type,
                  bcmi_keygen_ext_field_info_t *in_node,
                  bcmi_keygen_ext_field_info_t **head_node)
{
    bcmi_keygen_ext_field_info_t *temp_node1 = NULL;
    bcmi_keygen_ext_field_info_t *temp_node2 = NULL;


    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(in_node, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(head_node, BCM_E_PARAM);

    in_node->next = NULL;
    if (*head_node == NULL) {
        *head_node = in_node;
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    if(order_type == ORDER_TYPE_ASCENDING) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    } else if(order_type == ORDER_TYPE_DESCENDING) {
        temp_node1 = temp_node2 = *head_node;
        while (temp_node1 != NULL) {
             if (temp_node1->req_bit_count <
                 in_node->req_bit_count) {
                 in_node->next = temp_node1;
                 if (temp_node1 == temp_node2) {
                     *head_node = in_node;
                     break;
                 }
                 temp_node2->next = in_node;
                 break;
             }
             temp_node2 = temp_node1;
             temp_node1 = temp_node1->next;
        }
        if (temp_node1 == NULL) {
            in_node->next = NULL;
            temp_node2->next = in_node;
        }
    } else if (order_type == ORDER_TYPE_END_OF_LIST) {
        temp_node1 = *head_node;
        while (temp_node1->next != NULL) {
            temp_node1 = temp_node1->next;
        }
        temp_node1->next = in_node;
    } else if (order_type == ORDER_TYPE_START_OF_LIST) {
        in_node->next = *head_node;
        *head_node = in_node;
    } else {
        BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC void
bcmi_keygen_ext_finfo_release(int unit,
                             bcmi_keygen_ext_field_info_t *finfo)
{
    bcmi_keygen_ext_field_info_t *temp_finfo = NULL;

    while (finfo != NULL) {
        temp_finfo = finfo;
        finfo = finfo->next;
        sal_memset(temp_finfo, 0, sizeof(bcmi_keygen_ext_field_info_t));
        temp_finfo->next = finfo_free_list[unit];
        finfo_free_list[unit] = temp_finfo;
        DUMP_FINFO_COUNT(1);
    }
}

STATIC int
bcmi_keygen_ext_fields_get_last(int unit,
                              uint8 part,
                              uint8 level,
                              bcmi_keygen_ext_section_t section,
                              uint8 sec_val,
                              bcmi_keygen_ext_ctrl_sel_t ctrl_sel,
                              uint16 ctrl_sel_val,
                              bcmi_keygen_md_t *keygen_md,
                              bcmi_keygen_ext_field_info_t **finfo_list,
                              uint8 *map_index)
{
    uint8 ext_part = 0;
    uint32 ext_idx = 0;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo3 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo4 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo_list = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(finfo_list, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(map_index, BCM_E_PARAM);

    *finfo_list = NULL;
    for (ext_idx = 0;
         ext_idx < keygen_md->ext_cfg_db->conf_size[level];
         ext_idx++) {
         ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
         ext_part = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
        if (ext_part != part || ext_cfg->in_use == FALSE) {
           continue;
        }
        if ((temp_finfo1 = ext_cfg->finfo) == NULL) {
           continue;
        }
        if (section == temp_finfo1->l0_section[0] &&
            sec_val == temp_finfo1->l0_sec_val[0] &&
            ctrl_sel == temp_finfo1->pri_ctrl_sel[0] &&
            ctrl_sel_val == temp_finfo1->pri_ctrl_sel_val[0]) {
            *map_index = 0;
        } else if (section == temp_finfo1->l0_section[1] &&
            sec_val == temp_finfo1->l0_sec_val[1] &&
            ctrl_sel == temp_finfo1->pri_ctrl_sel[1] &&
            ctrl_sel_val == temp_finfo1->pri_ctrl_sel_val[1]) {
            *map_index = 1;
        } else {
            continue;
        }
        temp_finfo2 = finfo_free_list[unit];
        if (temp_finfo2 == NULL)  {
           BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
        }
        finfo_free_list[unit] = temp_finfo2->next;
        DUMP_FINFO_COUNT(0);
        sal_memcpy(temp_finfo2, temp_finfo1,
                   sizeof(bcmi_keygen_ext_field_info_t));
        temp_finfo2->next = NULL;
        /* New node is the first node in the list. */
        if (temp_finfo_list == NULL) {
           temp_finfo_list = temp_finfo2;
           continue;
        }
        /* New node has to be at the head_node of the list. */
        temp_finfo3 = temp_finfo_list;
        if (temp_finfo3->l0_field_offset[*map_index] >
            temp_finfo2->l0_field_offset[*map_index]) {
            temp_finfo2->next = temp_finfo3;
            temp_finfo_list = temp_finfo2;
            continue;
        }
        /* New node has to be some where in the middle of the list. */
        temp_finfo4 = temp_finfo_list;
        while (temp_finfo3 != NULL) {
            if (temp_finfo3->l0_field_offset[*map_index] >
                temp_finfo2->l0_field_offset[*map_index]) {
                temp_finfo4->next = temp_finfo2;
                temp_finfo2->next = temp_finfo3;
                break;
            }
            temp_finfo4 = temp_finfo3;
            temp_finfo3 = temp_finfo3->next;
        }
        /* New node has to be at the end of the list. */
        if (temp_finfo3 == NULL) {
           temp_finfo4->next = temp_finfo2;
        }
    }
    *finfo_list = temp_finfo_list;

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_qual_offset_get(int unit,
                          bcmi_keygen_cfg_t *keygen_cfg,
                          bcmi_keygen_oper_t *keygen_oper,
                          bcmi_keygen_md_t *keygen_md,
                          bcm_field_qualify_t qual_id,
                          bcmi_keygen_qual_cfg_t *qual_cfg)
{
    int bit_count = 0;
    uint8 idx = 0;
    uint8 f_idx = 0;
    uint8 bs_idx = 0;
    uint8 qid_idx = 0;
    uint8 finfo_idx = 0;
    uint8 offset_idx = 0;
    uint8 part = 0;
    uint8 level = 0;
    uint8 sec_val = 0;
    uint8 f_width = 0;
    uint8 q_bit = 0;
    uint8 f_bit = 0;
    uint8 map_index = 0;
    uint16 f_offset = 0;
    uint16 ctrl_sel_val = 0;
    uint16 bs_count = 0;
    uint16 field_offset = 0;
    uint16 map_offset = 0;
    uint16 field_width = 0;
    uint16 bs_offset[BCMI_KEYGEN_EXT_FIELD_OFFSET_COUNT] = { 0 };
    uint16 bs_width[BCMI_KEYGEN_EXT_FIELD_OFFSET_COUNT] = { 0 };
    uint32 req_bmp[BCMI_KEYGEN_EXT_FIELD_WORD_COUNT] = { 0 };
    uint32 ext_bmp[BCMI_KEYGEN_EXT_FIELD_WORD_COUNT] = { 0 };
    uint8 qid_found[BCMI_KEYGEN_ENTRY_PARTS_MAX] = { FALSE };
    bcmi_keygen_ext_section_t section;
    bcmi_keygen_ext_ctrl_sel_t ctrl_sel;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_field_info_t *finfo = NULL;
    bcmi_keygen_ext_field_info_t *finfo_list = NULL;
    bcmi_keygen_qual_offset_t *offset_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(qual_cfg, BCM_E_PARAM);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        if (keygen_cfg->qual_info_arr[idx].qual_id == qual_id) {
            break;
        }
    }
    BCMI_KEYGEN_IF_ERR_EXIT(
        (idx == keygen_cfg->qual_info_count) ? BCM_E_INTERNAL : BCM_E_NONE);
    for (f_idx = 0; f_idx < qual_cfg->num_chunks; f_idx++) {
        section = qual_cfg->e_params[f_idx].section;
        sec_val = qual_cfg->e_params[f_idx].sec_val;
        ctrl_sel = qual_cfg->pri_ctrl_sel;
        ctrl_sel_val = qual_cfg->pri_ctrl_sel_val;
        if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            field_width = qual_cfg->e_params[f_idx].width;
            bit_count = 0;
            SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap,
                               bit_count, field_offset, field_width);
            field_offset += qual_cfg->e_params[f_idx].width;
            if (bit_count == 0) {
                continue;
            }
        }
        for (finfo_idx = 0;
             finfo_idx < BCMI_KEYGEN_EXT_FIELDS_MAX;
             finfo_idx++) {
             if (section == keygen_md->finfo[finfo_idx].l0_section[0] &&
                 sec_val == keygen_md->finfo[finfo_idx].l0_sec_val[0] &&
                 ctrl_sel == keygen_md->finfo[finfo_idx].pri_ctrl_sel[0] &&
                 ctrl_sel_val ==
                 keygen_md->finfo[finfo_idx].pri_ctrl_sel_val[0]) {
                 finfo = &(keygen_md->finfo[finfo_idx]);
                 break;
             }
        }
        BCMI_KEYGEN_IF_ERR_EXIT((finfo == NULL) ? BCM_E_INTERNAL : BCM_E_NONE);
        part = finfo->part;
        qid_idx = keygen_oper->qual_offset_info[part].size;
        offset_info = &(keygen_oper->qual_offset_info[part].offset_arr[qid_idx]);
        keygen_oper->qual_offset_info[part].qid_arr[qid_idx] = qual_id;
        if (qid_found[part] == FALSE) {
            qid_found[part] = TRUE;
        }
        level = keygen_md->num_ext_levels;
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_ext_fields_get_last(unit, part, level,
                                          section, sec_val,
                                          ctrl_sel, ctrl_sel_val,
                                          keygen_md, &finfo_list,
                                          &map_index));
        f_offset = (qual_cfg->e_params[f_idx].offset %
                    qual_cfg->e_params[f_idx].size);
        f_width = qual_cfg->e_params[f_idx].width;
        sal_memset(req_bmp, 0x0, sizeof(req_bmp));
        if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            q_bit = field_offset - f_width;
            for (f_bit = f_offset;
                 f_bit < (f_offset + f_width);
                 f_bit++) {
                if (SHR_BITGET(
                    keygen_cfg->qual_info_arr[idx].bitmap, q_bit)) {
                    SHR_BITSET(req_bmp, f_bit);
                }
                q_bit++;
            }
        } else {
            SHR_BITSET_RANGE(req_bmp, f_offset, f_width);
        }

        finfo = finfo_list;
        while (finfo != NULL) {
            ext_cfg = finfo->ext_cfg;
            if (map_index == 1) {
               SHR_BITCOPY_RANGE(ext_bmp, 0, finfo->req_bmp,
                   finfo->l0_field_offset[map_index], finfo->size);
               map_offset = 0;
            } else {
               map_offset = finfo->l0_field_offset[map_index];
               SHR_BITCOPY_RANGE(ext_bmp,
                                 finfo->l0_field_offset[map_index],
                                 finfo->req_bmp, 0, finfo->size);
            }
            SHR_BITAND_RANGE(ext_bmp, req_bmp, 0,
               sizeof(uint32) * BCMI_KEYGEN_EXT_FIELD_WORD_COUNT * 8, ext_bmp);
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                bcmi_keygen_bit_stream_get(unit, ext_bmp,
                                     map_offset,
                                     finfo->size, &bs_count,
                                     bs_offset, bs_width));
            for (bs_idx = 0; bs_idx < bs_count; bs_idx++) {
                if (map_index == 1) {
                    offset_info->offset[offset_idx] = ext_cfg->offset +
                         bs_offset[bs_idx] + finfo->l0_field_offset[map_index];
                } else {
                    offset_info->offset[offset_idx] = ext_cfg->offset +
                         bs_offset[bs_idx] - finfo->l0_field_offset[map_index];
                }
                offset_info->width[offset_idx] = bs_width[bs_idx];
                offset_info->num_offsets++;
                offset_idx++;
            }
            finfo = finfo->next;
        }
        if (finfo_list != NULL) {
            (void)bcmi_keygen_ext_finfo_release(unit, finfo_list);
            finfo_list = NULL;
        }
    }

    for (part = 0; part < BCMI_KEYGEN_ENTRY_PARTS_MAX; part++) {
        if (qid_found[part] == TRUE) {
             keygen_oper->qual_offset_info[part].size++;
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_qual_id_count_get(int unit,
                            bcmi_keygen_md_t *keygen_md,
                            bcmi_keygen_cfg_t *keygen_cfg,
                            uint8 *qual_id_count)
{
    int bit_count = 0;
    uint8 idx = 0;
    uint8 f_idx = 0;
    uint8 finfo_idx = 0;
    uint8 part = 0;
    uint8 sec_val = 0;
    uint8 qual_cfg_id = 0;
    uint16 ctrl_sel_val = 0;
    uint16 field_offset = 0;
    uint16 field_width = 0;
    bcmi_keygen_ext_section_t section;
    bcmi_keygen_ext_ctrl_sel_t ctrl_sel;
    bcmi_keygen_ext_field_info_t *finfo = NULL;
    bcm_field_qualify_t qual_id = bcmFieldQualifyCount;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmi_keygen_pmux_info_t *pmux_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(qual_id_count, BCM_E_PARAM);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                bcmi_keygen_pmux_info_get(unit,
                                    qual_id,
                                    keygen_md->num_pmux_qual,
                                    keygen_md->pmux_info,
                                    &pmux_info));
            qual_id_count[pmux_info->pmux_part] += 1;
            continue;
        }
        field_offset = 0;
        for (f_idx = 0; f_idx < qual_cfg->num_chunks; f_idx++) {
            section = qual_cfg->e_params[f_idx].section;
            sec_val = qual_cfg->e_params[f_idx].sec_val;
            ctrl_sel = qual_cfg->pri_ctrl_sel;
            ctrl_sel_val = qual_cfg->pri_ctrl_sel_val;
            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                field_width = qual_cfg->e_params[f_idx].width;
                bit_count = 0;
                SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap,
                                   bit_count, field_offset, field_width);
                field_offset += qual_cfg->e_params[f_idx].width;
                if (bit_count == 0) {
                    continue;
                }
            }
            for (finfo_idx = 0;
                finfo_idx < BCMI_KEYGEN_EXT_FIELDS_MAX;
                finfo_idx++) {
                if (section == keygen_md->finfo[finfo_idx].l0_section[0] &&
                    sec_val == keygen_md->finfo[finfo_idx].l0_sec_val[0] &&
                    ctrl_sel == keygen_md->finfo[finfo_idx].pri_ctrl_sel[0] &&
                    ctrl_sel_val ==
                    keygen_md->finfo[finfo_idx].pri_ctrl_sel_val[0]) {
                    finfo = &(keygen_md->finfo[finfo_idx]);
                    break;
                }
            }
            BCMI_KEYGEN_IF_ERR_EXIT((finfo == NULL) ? BCM_E_INTERNAL : BCM_E_NONE);
            part = finfo->part;
            qual_id_count[part]++;
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_qual_offset_create(int unit,
                             bcmi_keygen_cfg_t *keygen_cfg,
                             bcmi_keygen_oper_t *keygen_oper,
                             bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    uint8 part = 0;
    uint8 parts_count = 0;
    uint8 qual_cfg_id = 0;
    uint8 qual_id_count[BCMI_KEYGEN_ENTRY_PARTS_MAX];
    bcm_field_qualify_t *qid_arr = NULL;
    bcmi_keygen_qual_offset_t *offset_arr = NULL;
    bcm_field_qualify_t qual_id = bcmFieldQualifyCount;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmi_keygen_qual_offset_t *qual_offset = NULL;
    bcmi_keygen_qual_offset_info_t *offset_info = NULL;
    bcmi_keygen_pmux_info_t *pmux_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    sal_memset(qual_id_count, 0, sizeof(uint8) * BCMI_KEYGEN_ENTRY_PARTS_MAX);
    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
        bcmi_keygen_qual_id_count_get(unit, keygen_md, keygen_cfg, qual_id_count));
    parts_count = keygen_cfg->num_parts;
    BCMI_KEYGEN_ALLOC(keygen_oper->qual_offset_info,
                      sizeof(bcmi_keygen_qual_offset_info_t) * parts_count,
                      "Keygen operational qualifier offset information");
    for (part = 0; part < parts_count; part++) {
        qid_arr = NULL;
        offset_arr = NULL;
        BCMI_KEYGEN_ALLOC(qid_arr,
                    qual_id_count[part] * sizeof(bcm_field_qualify_t),
                    "Group qual id array in each part");
        keygen_oper->qual_offset_info[part].qid_arr = qid_arr;
        BCMI_KEYGEN_ALLOC(offset_arr,
                    qual_id_count[part] * sizeof(bcmi_keygen_qual_offset_t),
                    "Group qual offset info array in each part");
        keygen_oper->qual_offset_info[part].offset_arr = offset_arr;
    }

    /* Go through all qualifiers */
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                bcmi_keygen_pmux_info_get(unit,
                                    qual_id,
                                    keygen_md->num_pmux_qual,
                                    keygen_md->pmux_info,
                                    &pmux_info));
            offset_info = &(keygen_oper->qual_offset_info[pmux_info->pmux_part]);
            qual_offset =  &(offset_info->offset_arr[offset_info->size]);
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                bcmi_keygen_pmux_qual_offset_get(unit,
                                                 qual_id,
                                                 keygen_md,
                                                 keygen_cfg,
                                                 qual_offset));
            offset_info->qid_arr[offset_info->size] = qual_id;
           offset_info->size++;
        } else {
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                bcmi_keygen_qual_offset_get(unit, keygen_cfg,
                                            keygen_oper, keygen_md,
                                            qual_id, qual_cfg));
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_fields_get_first(int unit,
                               bcmi_keygen_cfg_t *keygen_cfg,
                               bcmi_keygen_md_t *keygen_md)
{
    int bit_count = 0;
    uint8 idx = 0;
    uint8 f_idx = 0;
    uint8 i = 0, j = 0;
    uint8 field_count = 0;
    uint8 finfo_idx = 0;
    uint8 f_width = 0;
    uint8 sec_val = 0;
    uint8 found = FALSE;
    uint8 qual_cfg_id = 0;
    uint8 q_bit = 0;
    uint8 f_bit = 0;
    uint16 f_offset = 0;
    uint16 finfo_size = 0;
    uint16 ctrl_sel_val = 0;
    uint16 field_offset = 0;
    uint16 field_width = 0;
    uint32 qual_flags = 0;
    uint32 finfo_flags = 0;
    bcmi_keygen_ext_section_t section;
    bcmi_keygen_ext_ctrl_sel_t ctrl_sel;
    bcmi_keygen_ext_field_info_t *finfo = NULL;
    bcmi_keygen_ext_field_info_t temp_finfo;
    bcm_field_qualify_t qual_id = bcmFieldQualifyCount;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    finfo_size = sizeof(bcmi_keygen_ext_field_info_t) * BCMI_KEYGEN_EXT_FIELDS_MAX;
    sal_memset(keygen_md->finfo, 0, finfo_size);
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_flags = keygen_cfg->qual_info_arr[idx].flags;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        field_offset = 0;
        for (f_idx = 0; f_idx < qual_cfg->num_chunks; f_idx++) {
            section = qual_cfg->e_params[f_idx].section;
            sec_val = qual_cfg->e_params[f_idx].sec_val;
            ctrl_sel = qual_cfg->pri_ctrl_sel;
            ctrl_sel_val = qual_cfg->pri_ctrl_sel_val;
            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                field_width = qual_cfg->e_params[f_idx].width;
                bit_count = 0;
                SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap,
                                   bit_count, field_offset, field_width);
                field_offset += qual_cfg->e_params[f_idx].width;
                if (bit_count == 0) {
                    continue;
                }
            }
            for (finfo_idx = 0; finfo_idx < BCMI_KEYGEN_EXT_FIELDS_MAX; finfo_idx++) {
                 finfo = &(keygen_md->finfo[finfo_idx]);
                 if ((finfo->section == section &&
                      finfo->sec_val == sec_val &&
                      finfo->pri_ctrl_sel[0] == ctrl_sel &&
                      finfo->pri_ctrl_sel_val[0] == ctrl_sel_val)) {
                      found = TRUE;
                 }
                 /* Ignore 'found' if this flag is set */
                 if (qual_flags & BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW) {
                    found = FALSE;
                 }
                 if ((found == TRUE) ||
                    (finfo->section == BCMI_KEYGEN_EXT_SECTION_DISABLE)) {
                     break;
                 }
                 continue;
            }
            finfo_flags = 0;
            finfo_flags |= (qual_flags & BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16) ?
                                BCMI_KEYGEN_EXT_FIELD_USE_L2_GRAN16: finfo_flags;
            finfo_flags |= (qual_flags & BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN8) ?
                                BCMI_KEYGEN_EXT_FIELD_USE_L2_GRAN8: finfo_flags;
            if (found == FALSE) {
                finfo->flags = finfo_flags;
                finfo->size = qual_cfg->e_params[f_idx].size;
                finfo->section = section;
                finfo->sec_val = sec_val;
                finfo->pri_ctrl_sel[0] = ctrl_sel;
                finfo->pri_ctrl_sel_val[0] = ctrl_sel_val;
                finfo->part = -1;
                finfo->ext_bmp[0] = 0;
                finfo->l0_section[0] = section;
                finfo->l0_sec_val[0] = sec_val;
                finfo->l0_field_offset[0] = 0;
                field_count++;
            }
            f_offset = (qual_cfg->e_params[f_idx].offset %
                        qual_cfg->e_params[f_idx].size);
            f_width = qual_cfg->e_params[f_idx].width;
            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                q_bit = field_offset - f_width;
                for (f_bit = f_offset;
                     f_bit < (f_offset + f_width);
                     f_bit++) {
                    if (SHR_BITGET(
                        keygen_cfg->qual_info_arr[idx].bitmap, q_bit)) {
                        SHR_BITSET(finfo->req_bmp, f_bit);
                    }
                    q_bit++;
                }
            } else {
                SHR_BITSET_RANGE(finfo->req_bmp, (int)f_offset, (int)f_width);
            }
            found = FALSE;
        }
    }

    /* Sort the finfo List */
    for (i = 0; i < field_count; i++) {
        for (j = (i + 1); j < field_count; j++) {
            if (keygen_md->finfo[j].size >
                keygen_md->finfo[i].size) {
                sal_memcpy(&temp_finfo, &keygen_md->finfo[i],
                           sizeof(bcmi_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[i], &keygen_md->finfo[j],
                           sizeof(bcmi_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[j], &temp_finfo,
                           sizeof(bcmi_keygen_ext_field_info_t));
            }
        }
    }

    j = 0;
    field_width = keygen_md->finfo[0].size;
    for (i = 0; i < field_count; i++) {
        if (field_width != keygen_md->finfo[i].size) {
            field_width = keygen_md->finfo[i].size;
            j = i;
        }
        if (keygen_md->finfo[i].pri_ctrl_sel[0] !=
            BCMI_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            if (j != i) {
                sal_memcpy(&temp_finfo, &keygen_md->finfo[j],
                           sizeof(bcmi_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[j], &keygen_md->finfo[i],
                           sizeof(bcmi_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[i], &temp_finfo,
                           sizeof(bcmi_keygen_ext_field_info_t));
            }
            j++;
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_fields_validate(int unit,
                                uint8 part,
                                uint8 level,
                                bcmi_keygen_ext_section_t section,
                                bcmi_keygen_md_t *keygen_md)
{
    uint8 equal = 0;
    uint8 ext_part = 0;
    uint16 ext_idx = 0;
    uint16 bit_count = 0;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo1 = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);
    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);

    bit_count = BCMI_KEYGEN_EXT_FIELD_WORD_COUNT * sizeof(uint32) * 8;
    for (ext_idx = 0;
         ext_idx < keygen_md->ext_cfg_db->conf_size[level];
         ext_idx++) {
         ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
         ext_part =  BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
         if (ext_cfg->out_sec != section ||
             ext_cfg->in_use == FALSE ||
             ext_part != part) {
             continue;
         }
         temp_finfo1 = ext_cfg->finfo;
         if (temp_finfo1 == NULL) {
             if (SHR_BITGET(ext_cfg->ext_attrs.w,
                     BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                 continue;
             } else {
                 BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
             }
         }
         equal = SHR_BITEQ_RANGE(temp_finfo1->req_bmp,
                                 temp_finfo1->ext_bmp,
                                 0, bit_count);
         if (!equal) {
             BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_RESOURCE);
         }
    }
exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Notes:
 * Default behavior of this function is to allow any granularity
 * if finfo flags are not set
 */
STATIC int
bcmi_keygen_finfo_flags_verify(int unit,
                          uint8 level,
                          uint8 gran,
                          bcmi_keygen_ext_field_info_t *finfo)
{
    uint8 g_idx = 0;
    uint8 index = 0;
    uint8 found = TRUE;
    uint8 exp_level = 0;
    uint8 exp_gran[BCMI_KEYGEN_EXT_GRANULAR_ARR_SIZE] = {0};

    if (finfo->flags & BCMI_KEYGEN_EXT_FIELD_USE_L2_GRAN16) {
        exp_level = 1;
        exp_gran[index] = 16;
        index++;
    }
    if (finfo->flags & BCMI_KEYGEN_EXT_FIELD_USE_L2_GRAN8) {
        exp_level = 1;
        exp_gran[index] = 8;
        index++;
    }

    if ((exp_level == level) && (index != 0)) {
        found = FALSE;
        for (g_idx = 0; g_idx < index; g_idx++) {
            if (exp_gran[g_idx] == gran) {
                found = TRUE;
                break;
            }
        }
    }

    return found;
}

STATIC int
bcmi_keygen_section_fields_create(int unit,
                            uint8 part,
                            uint8 level,
                            bcmi_keygen_ext_section_t section,
                            uint8 gran,
                            uint8 num_ext,
                            bcmi_keygen_cfg_t *keygen_cfg,
                            bcmi_keygen_md_t *keygen_md)
{
    int count = 0;
    uint8 idx = 0;
    uint8 chunk = 0;
    uint8 num_chunks = 0;
    uint8 order_type = 0;
    uint8 flags_check = FALSE;
    uint16 ext_idx = 0;
    uint16 ext_offset = 0;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_section_cfg_t *sec_cfg = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo3 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo4 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo_list = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    for (ext_idx = 0;
         ext_idx < keygen_md->ext_cfg_db->conf_size[level];
         ext_idx++) {
         ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
         if (ext_cfg->out_sec != section || ext_cfg->in_use == FALSE) {
             continue;
         }
         temp_finfo1 = ext_cfg->finfo;
         if (temp_finfo1 == NULL) {
             if (SHR_BITGET(ext_cfg->ext_attrs.w,
                     BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                 continue;
             } else {
                 BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
             }
         }
         if (temp_finfo1->size < gran) {
             continue;
         }
         flags_check = bcmi_keygen_finfo_flags_verify(unit,
                                    level, gran, temp_finfo1);
         if (flags_check == FALSE) {
            continue;
         }
         num_chunks = temp_finfo1->size / gran;
         ext_offset = ext_cfg->offset;
         for (chunk = 0; chunk < num_chunks; chunk++) {
             SHR_BITCOUNT_RANGE(temp_finfo1->req_bmp,
                                count, chunk * gran, gran);
             if (count == 0) {
                 continue;
             }
             if (SHR_BITEQ_RANGE(temp_finfo1->req_bmp,
                                 temp_finfo1->ext_bmp,
                                 chunk * gran, gran)) {
                 continue;
             }
             temp_finfo2 = finfo_free_list[unit];
             if (temp_finfo2 == NULL) {
                 BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
             }
             finfo_free_list[unit] = temp_finfo2->next;
             DUMP_FINFO_COUNT(0);
             temp_finfo2->flags = temp_finfo1->flags;
             temp_finfo2->section = section;
             temp_finfo2->sec_val = (ext_offset + (chunk * gran)) / gran;
             temp_finfo2->pri_ctrl_sel[0] = temp_finfo1->pri_ctrl_sel[0];
             temp_finfo2->pri_ctrl_sel_val[0] = temp_finfo1->pri_ctrl_sel_val[0];
             temp_finfo2->size = gran;
             temp_finfo2->extracted = FALSE;
             temp_finfo2->req_bit_count = count;
             temp_finfo2->l0_section[0] = temp_finfo1->l0_section[0];
             temp_finfo2->l0_sec_val[0] = temp_finfo1->l0_sec_val[0];
             temp_finfo2->l0_field_offset[0] =
                           temp_finfo1->l0_field_offset[0] + (chunk * gran);
             SHR_BITCOPY_RANGE(temp_finfo2->req_bmp, 0,
                               temp_finfo1->req_bmp, chunk * gran, gran);
             SHR_BITCOPY_RANGE(temp_finfo1->ext_bmp, chunk * gran,
                               temp_finfo1->req_bmp, chunk * gran, gran);
             temp_finfo2->prev = temp_finfo1;
             temp_finfo2->prev_offset = chunk * gran;
             temp_finfo2->next = NULL;
             /* add the new finfo node to the list in the
              * sorted(descending) order.
              */
             sec_cfg = keygen_md->ext_cfg_db->sec_cfg[section];
             if (sec_cfg->flags & BCMI_KEYGEN_EXT_SECTION_PASS_THRU) {
                 order_type = ORDER_TYPE_END_OF_LIST;
             } else {
                 order_type = ORDER_TYPE_DESCENDING;
             }
             BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                 bcmi_keygen_ext_finfo_node_add(unit, order_type,
                                        temp_finfo2, &temp_finfo_list));
         }
    }

    /* Keep only num_ext number of nodes in the finfo list
     * and add the remaining nodes to free list.
     */
     temp_finfo3 = temp_finfo_list;
     temp_finfo2 = NULL;
     while (temp_finfo3 != NULL) {
         idx++;
         temp_finfo2 = temp_finfo3;
         temp_finfo3 = temp_finfo3->next;
         if (idx == num_ext) {
             break;
         }
     }
     while (temp_finfo3 != NULL) {
         temp_finfo4 = temp_finfo3;
         temp_finfo3 = temp_finfo3->next;
         SHR_BITCLR_RANGE(temp_finfo4->prev->ext_bmp,
                          temp_finfo4->prev_offset,
                          temp_finfo4->size);
         sal_memset(temp_finfo4, 0, sizeof(bcmi_keygen_ext_field_info_t));
         temp_finfo4->next = finfo_free_list[unit];
         finfo_free_list[unit] = temp_finfo4;
         DUMP_FINFO_COUNT(1);
     }

     /* Add the list of chunks to the section in given level and part. */
     if (temp_finfo2 != NULL) {
         temp_finfo2->next = keygen_md->ext_finfo_db[part][level].finfo[section];
         keygen_md->ext_finfo_db[part][level].finfo[section] = temp_finfo_list;
     }

    BCMI_KEYGEN_FUNC_EXIT();
exit:

    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC void
bcmi_keygen_ext_section_finfo_reset(int unit,
                                    uint8 part,
                                    uint8 level,
                                    bcmi_keygen_ext_section_t section,
                                    bcmi_keygen_md_t *keygen_md)
{
    bcmi_keygen_ext_field_info_t *finfo = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo = NULL;

    finfo = keygen_md->ext_finfo_db[part][level].finfo[section];
    while(finfo != NULL) {
        temp_finfo = finfo;
        finfo = finfo->next;

        SHR_BITCLR_RANGE(temp_finfo->prev->ext_bmp,
                temp_finfo->prev_offset,
                temp_finfo->size);
    }

    (void)bcmi_keygen_ext_finfo_release(unit,
            keygen_md->ext_finfo_db[part][level].finfo[section]);

    keygen_md->ext_finfo_db[part][level].finfo[section] = NULL;
}

STATIC int
bcmi_keygen_section_fields_create_type1(int unit,
                                    uint8 part,
                                    uint8 level,
                                    bcmi_keygen_ext_section_t section,
                                    bcmi_keygen_cfg_t *keygen_cfg,
                                    bcmi_keygen_md_t *keygen_md)
{
    int src_offset = 0;
    int dst_offset = 0;
    int num_bits = 0;
    uint8 chunk = 0;
    uint8 gran = 0;
    uint8 map_index = 0;
    uint8 num_chunks = 0;
    uint16 idx = 0;
    uint16 ext_idx = 0;
    uint16 num_extractors = 0;
    uint32 chunk_bmp = 0;
    uint32 section_bmp[20] = { 0 };
    uint8 chunk_offset = 0;
    uint8 required_extractors = 0;
    uint8 required_extractors_gran[4];
    uint8 extractors_1b_count = 0;
    uint8 extractors_2b_count = 0;
    uint8 extractors_4b_count = 0;
    uint16 new_node_offset = 0;
    uint16 old_node_offset = 0;
    uint8 new_node_size = 0;
    uint8 old_node_size = 0;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo_list = NULL;
    bcmi_keygen_ext_section_gran_info_t *section_gran_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    for (ext_idx = 0;
         ext_idx < keygen_md->ext_cfg_db->conf_size[level];
         ext_idx++) {
         ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
         if (ext_cfg->out_sec != section || ext_cfg->in_use == FALSE) {
             continue;
         }
         temp_finfo1 = ext_cfg->finfo;
         if (temp_finfo1 == NULL) {
             if (SHR_BITGET(ext_cfg->ext_attrs.w,
                     BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                 continue;
             } else {
                 BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
             }
         }
         dst_offset = ext_cfg->offset;
         src_offset = 0;
         num_bits = temp_finfo1->size;
         SHR_BITCOPY_RANGE(section_bmp, dst_offset,
                           temp_finfo1->req_bmp,
                           src_offset, num_bits);
    }

    section_gran_info = &(keygen_md->section_gran_info[section]);
    for (idx = 0;
         idx < section_gran_info->out_gran_info_count;
         idx++) {
        gran = section_gran_info->out_gran_info[idx].gran;
        num_extractors = section_gran_info->out_gran_info[idx].num_extractors;
        if (gran == 1) {
            extractors_1b_count = num_extractors;
        } else if (gran == 2) {
            extractors_2b_count = num_extractors;
        } else if (gran == 4) {
            extractors_4b_count = num_extractors;
        } else {
            BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
        }
    }

    num_chunks = (sizeof(uint32) * 8 * 20) / 4;
    for (chunk = 0; chunk < num_chunks; chunk++) {
        chunk_bmp = 0;
        SHR_BITCOPY_RANGE(&chunk_bmp, 0, section_bmp, chunk * 4, 4);
        if ((chunk_bmp & 0xF) == 0) {
            continue;
        }
        switch (chunk_bmp) {
            case 0x1:
            case 0x2:
            case 0x4:
            case 0x8:
                required_extractors = 1;
                if (extractors_1b_count) {
                    required_extractors_gran[0] = 1;
                } else if (extractors_2b_count) {
                    required_extractors_gran[0] = 2;
                } else if (extractors_4b_count) {
                    required_extractors_gran[0] = 4;
                } else {
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_RESOURCE);
                }
                break;
            case 0x3:
            case 0xC:
                required_extractors = 1;
                if (extractors_2b_count) {
                    required_extractors_gran[0] = 2;
                } else if (extractors_1b_count >= 2) {
                    required_extractors = 2;
                    required_extractors_gran[0] = 1;
                    required_extractors_gran[1] = 1;
                } else if (extractors_4b_count) {
                    required_extractors_gran[0] = 4;
                } else {
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_RESOURCE);
                }
                break;
            case 0x5:
            case 0x6:
            case 0x9:
            case 0xA:
                required_extractors = 2;
                if (extractors_1b_count >= 2) {
                    required_extractors_gran[0] = 1;
                    required_extractors_gran[1] = 1;
                } else if (extractors_2b_count >= 2) {
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 2;
                } else if (extractors_4b_count) {
                    required_extractors = 1;
                    required_extractors_gran[0] = 4;
                } else {
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_RESOURCE);
                }
                break;
            case 0x7:
            case 0xB:
                required_extractors = 2;
                if (extractors_2b_count &&
                    extractors_1b_count) {
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 1;
                } else if (extractors_2b_count >= 2) {
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 2;
                } else if (extractors_4b_count) {
                    required_extractors = 1;
                    required_extractors_gran[0] = 4;
                } else {
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_RESOURCE);
                }
                break;
            case 0xD:
            case 0xE:
                required_extractors = 2;
                if (extractors_2b_count &&
                    extractors_1b_count) {
                    required_extractors_gran[0] = 1;
                    required_extractors_gran[1] = 2;
                } else if (extractors_2b_count >= 2) {
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 2;
                } else if (extractors_4b_count) {
                    required_extractors = 1;
                    required_extractors_gran[0] = 4;
                } else {
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_RESOURCE);
                }
                break;
            case 0xF:
                if (extractors_4b_count) {
                    required_extractors = 1;
                    required_extractors_gran[0] = 4;
                } else if (extractors_2b_count >= 2) {
                    required_extractors = 2;
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 2;
                } else if (extractors_2b_count &&
                           extractors_1b_count >= 2) {
                    required_extractors = 3;
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 1;
                    required_extractors_gran[2] = 1;
                } else {
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_RESOURCE);
                }
                break;
            default:
                BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
        }
        for (idx = 0; idx < required_extractors; idx++) {
             gran = required_extractors_gran[idx];
             for (chunk_offset = 0;
                 chunk_offset < (4 / gran);
                 chunk_offset++) {
                 if (chunk_bmp &
                    ((1 << gran) - 1) << (chunk_offset * gran)) {
                     break;
                 }
             }
             if (chunk_offset == (4 / gran)) {
                 BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
             }
             temp_finfo2 = finfo_free_list[unit];
             if (temp_finfo2 == NULL) {
                 BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
             }
             finfo_free_list[unit] = temp_finfo2->next;
             DUMP_FINFO_COUNT(0);
             temp_finfo2->section = section;
             temp_finfo2->sec_val =
                 ((chunk * 4) + (chunk_offset * gran)) / gran;
             temp_finfo2->size = gran;
             temp_finfo2->extracted = FALSE;
             temp_finfo2->prev = NULL;
             temp_finfo2->next = NULL;
             num_bits = 0;
             SHR_BITCOUNT_RANGE(&chunk_bmp, num_bits,
                                chunk_offset * gran, gran);

             temp_finfo2->req_bit_count = num_bits;
             SHR_BITCOPY_RANGE(temp_finfo2->req_bmp, 0,
                               &chunk_bmp, chunk_offset * gran, gran);
             BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
                 (bcmi_keygen_ext_finfo_node_add(unit, ORDER_TYPE_DESCENDING,
                                             temp_finfo2, &temp_finfo_list));
             if (gran == 1) {
                 extractors_1b_count--;
             } else if (gran == 2) {
                 extractors_2b_count--;
             } else if (gran == 4) {
                 extractors_4b_count--;
             } else {
                 BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
             }
             SHR_BITCLR_RANGE(&chunk_bmp, chunk_offset * gran, gran);
        }
    }

    /* Update the l0 section of all new finfo nodes. */
    temp_finfo2 = temp_finfo_list;
    while (temp_finfo2 != NULL) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (ext_cfg->out_sec != section || ext_cfg->in_use == FALSE) {
                continue;
            }
            temp_finfo1 = ext_cfg->finfo;
            if (temp_finfo1 == NULL) {
                if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                    continue;
                } else {
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
                }
            }
            new_node_offset = temp_finfo2->sec_val * temp_finfo2->size;
            new_node_size = temp_finfo2->size;
            old_node_offset = ext_cfg->offset;
            old_node_size = temp_finfo1->size;
            if (((new_node_offset >= old_node_offset) &&
                ((new_node_offset + new_node_size)) <=
                (old_node_offset + old_node_size)) ||
                ((new_node_offset <= old_node_offset) &&
                ((new_node_offset + new_node_size)) >=
                (old_node_offset + old_node_size))) {
                if (temp_finfo2->l0_section[0] == 0) {
                    map_index = 0;
                } else {
                    map_index = 1;
                }
                temp_finfo2->l0_section[map_index] = temp_finfo1->l0_section[0];
                temp_finfo2->l0_sec_val[map_index] = temp_finfo1->l0_sec_val[0];
                temp_finfo2->pri_ctrl_sel[map_index] =
                    temp_finfo1->pri_ctrl_sel[0];
                temp_finfo2->pri_ctrl_sel_val[map_index] =
                    temp_finfo1->pri_ctrl_sel_val[0];
                if (old_node_offset >= new_node_offset) {
                    temp_finfo2->l0_field_offset[map_index] =
                                               temp_finfo1->l0_field_offset[0] +
                                               old_node_offset - new_node_offset;
                } else {
                    temp_finfo2->l0_field_offset[map_index] =
                                               temp_finfo1->l0_field_offset[0] +
                                               new_node_offset - old_node_offset;
                }
            }
        }
        temp_finfo2 = temp_finfo2->next;
    }

    keygen_md->ext_finfo_db[part][level].finfo[section] = temp_finfo_list;

    BCMI_KEYGEN_FUNC_EXIT();

exit:
    (void)bcmi_keygen_ext_finfo_release(unit, temp_finfo_list);
    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Note: This function assumes that in a section either single gran
 * extractor or multi-gran extractor are present and not both.
 * Also, It is assumed that only one multi-gran extractor is
 * present in the section. In future, if a section has both types
 * of extractors or more then one multi-gran extractor, then
 * function will be required to enhanced. Calcuate number of
 * combinations of fixed and variable gran extractors and try each
 * combination to extract required bits
 */
STATIC int
bcmi_keygen_ext_section_fields_get(int unit, uint8 part, uint8 level,
                                   bcmi_keygen_ext_section_t section,
                                   bcmi_keygen_cfg_t *keygen_cfg,
                                   bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    uint8 gran = 0;
    uint8 gran_count = 0;
    uint8 num_ext = 0;
    bcmi_keygen_ext_section_gran_t *gran_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    if (keygen_md->ext_cfg_db->sec_cfg[section]->flags &
        BCMI_KEYGEN_EXT_SECTION_MULTI_GRAN_1_2_4) {
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
            (bcmi_keygen_section_fields_create_type1(unit, part, level,
                                  section, keygen_cfg, keygen_md));
    } else {
       gran_count =
           keygen_md->section_gran_info[section].out_gran_info_count;
       gran_info = keygen_md->section_gran_info[section].out_gran_info;
       for (idx = 0; idx < gran_count; idx++) {
           gran = gran_info[idx].gran;
           num_ext = gran_info[idx].num_extractors;
           /* Divide the unextracted bits of the section into chunks of
            * given granularity.
            */
           BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
               (bcmi_keygen_section_fields_create(unit, part, level,
                                             section, gran, num_ext,
                                             keygen_cfg, keygen_md));
        }

        /* Multigran extractors */
        gran_count =
            keygen_md->section_gran_info[section].out_multigran_info_count;
        gran_info = keygen_md->section_gran_info[section].out_multigran_info;
        for (idx = 0; idx < gran_count; idx++) {
            gran = gran_info[idx].gran;
            num_ext = gran_info[idx].num_extractors;
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                bcmi_keygen_section_fields_create(unit, part, level, section,
                                         gran, num_ext, keygen_cfg, keygen_md));
            _func_rv = bcmi_keygen_ext_fields_validate(unit, part, level,
                                                      section, keygen_md);
            if (_func_rv == BCM_E_RESOURCE) {
                /* Reset previous finfo ext bitmap */
                (void) bcmi_keygen_ext_section_finfo_reset(unit, part,
                                           level, section, keygen_md);
            } else {
                break;
            }
        }
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_ext_fields_validate(unit, part, level,
                                         section, keygen_md));
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_fields_get_next(int unit,
                              uint8 part,
                              uint8 level,
                              bcmi_keygen_cfg_t *keygen_cfg,
                              bcmi_keygen_md_t *keygen_md)
{
    uint8 sec_idx = 0;
    bcmi_keygen_ext_section_t section;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    for (sec_idx = 0; sec_idx < BCMI_KEYGEN_EXT_SECTION_COUNT; sec_idx++) {
        section = keygen_md->sections[part][level + 1][sec_idx];
        if (section == BCMI_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
            (bcmi_keygen_ext_section_fields_get(unit, part, level + 1,
                                     section, keygen_cfg, keygen_md));
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_section_validate(int unit,
                                 bcmi_keygen_ext_cfg_t *ext_cfg,
                                 bcmi_keygen_ext_field_info_t *finfo,
                                 bcmi_keygen_md_t *keygen_md)
{
    int gran = 0;
    uint16 fill_bits = 0, parent_fill_bits = 0;
    uint16 drain_bits = 0, parent_drain_bits = 0;
    bcmi_keygen_ext_section_t out_sec;
    bcmi_keygen_ext_section_t parent_sec;

    BCMI_KEYGEN_FUNC_ENTER(unit);
    BCMI_KEYGEN_NULL_CHECK(ext_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(finfo, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);

    gran = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);

    out_sec = ext_cfg->out_sec;
    fill_bits = keygen_md->ext_cfg_db->sec_cfg[out_sec]->fill_bits;
    drain_bits = keygen_md->ext_cfg_db->sec_cfg[out_sec]->drain_bits;

    parent_sec = keygen_md->ext_cfg_db->sec_cfg[out_sec]->parent_sec;
    parent_fill_bits = keygen_md->ext_cfg_db->sec_cfg[parent_sec]->fill_bits;
    parent_drain_bits = keygen_md->ext_cfg_db->sec_cfg[parent_sec]->drain_bits;

    if (((fill_bits + gran) > drain_bits) ||
        ((parent_fill_bits + gran) > parent_drain_bits)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_RESOURCE);
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_alloc_3(int unit,
                      uint8 part,
                      uint8 level,
                      bcmi_keygen_ext_section_t section,
                      bcmi_keygen_cfg_t *keygen_cfg,
                      bcmi_keygen_md_t *keygen_md)
{
    int g = 0, p = 0, l = 0;
    uint16 ext_idx = 0;
    bcmi_keygen_ext_section_t out_sec;
    bcmi_keygen_ext_section_t parent_sec;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_field_info_t *finfo = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    /* Assign extractors to each field of the section. */
    finfo = keygen_md->ext_finfo_db[part][level].finfo[section];
    while (finfo != NULL) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level + 1];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_idx;
            g = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
            p = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            l = BCMI_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
            if ((p != part) ||
                (l != (level + 1)) ||
                (g != finfo->size) ||
                (section != ext_cfg->in_sec)) {
               continue;
            }
            _func_rv = bcmi_keygen_ext_validate(unit, p, keygen_md, ext_cfg);
            if (BCMI_KEYGEN_FUNC_VAL_IS(BCM_E_CONFIG)) {
                continue;
            }
             BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(_func_rv);
            _func_rv = bcmi_keygen_ext_section_validate(unit,
                                            ext_cfg, finfo, keygen_md);
            if (BCMI_KEYGEN_FUNC_VAL_IS(BCM_E_RESOURCE)) {
                continue;
            }
            out_sec = ext_cfg->out_sec;
            parent_sec = keygen_md->ext_cfg_db->sec_cfg[out_sec]->parent_sec;
            keygen_md->ext_cfg_db->sec_cfg[out_sec]->fill_bits += g;
            if (parent_sec != out_sec) {
                keygen_md->ext_cfg_db->sec_cfg[parent_sec]->fill_bits += g;
            }
            ext_cfg->finfo = finfo;
            ext_cfg->in_use = TRUE;
            finfo->extracted = TRUE;
            finfo->ext_cfg = ext_cfg;
            break;
        }

        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            (finfo->extracted == FALSE) ? BCM_E_RESOURCE : BCM_E_NONE);
        finfo = finfo->next;
    }
    BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_alloc_2(int unit,
                      uint8 part,
                      uint8 level,
                      bcmi_keygen_cfg_t *keygen_cfg,
                      bcmi_keygen_md_t *keygen_md)
{
    uint8 sec_idx = 0;
    bcmi_keygen_ext_section_t section;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    for (sec_idx = 0; sec_idx < BCMI_KEYGEN_EXT_SECTION_COUNT; sec_idx++) {
        section = keygen_md->sections[part][level][sec_idx];
        if (section == BCMI_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_ext_alloc_3(unit, part, level,
                                  section, keygen_cfg, keygen_md));
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_finfo_db_free(int unit,
                             uint8 part,
                             uint8 level,
                             bcmi_keygen_md_t *keygen_md)
{
    bcmi_keygen_ext_section_t section;
    bcmi_keygen_ext_field_info_t *finfo = NULL;
    bcmi_keygen_ext_field_info_t *next_finfo = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);

    for (section = 0; section < BCMI_KEYGEN_EXT_SECTION_COUNT; section++) {
        finfo = keygen_md->ext_finfo_db[part][level].finfo[section];
        while (finfo != NULL) {
            next_finfo = finfo->next;
            sal_memset(finfo, 0, sizeof(bcmi_keygen_ext_field_info_t));
            finfo->next = finfo_free_list[unit];
            finfo_free_list[unit] = finfo;
            DUMP_FINFO_COUNT(1);
            finfo = next_finfo;
        }
        keygen_md->ext_finfo_db[part][level].finfo[section] = NULL;
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_finfo_add(int unit,
                        uint8 part,
                        bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    bcmi_keygen_ext_section_t section;
    bcmi_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmi_keygen_ext_field_info_t *temp_finfo3 = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);

    for (idx = 0; idx < BCMI_KEYGEN_EXT_FIELDS_MAX; idx++) {
       temp_finfo1 = &(keygen_md->finfo[idx]);
       section = temp_finfo1->section;
       if (section == BCMI_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (temp_finfo1->part != part)  {
           continue;
       }
       temp_finfo2 = finfo_free_list[unit];
       BCMI_KEYGEN_IF_ERR_EXIT((temp_finfo2 == NULL) ? BCM_E_INTERNAL : BCM_E_NONE);
       finfo_free_list[unit] = finfo_free_list[unit]->next;
       DUMP_FINFO_COUNT(0);
       sal_memcpy(temp_finfo2, temp_finfo1, sizeof(bcmi_keygen_ext_field_info_t));
       temp_finfo2->next = NULL;
       if (keygen_md->ext_finfo_db[part][0].finfo[section] == NULL) {
           keygen_md->ext_finfo_db[part][0].finfo[section] = temp_finfo2;
       } else {
           temp_finfo3 = keygen_md->ext_finfo_db[part][0].finfo[section];
           while (temp_finfo3->next != NULL) {
               temp_finfo3 = temp_finfo3->next;
           }
           temp_finfo3->next = temp_finfo2;
       }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_alloc_1(int unit,
                      uint8 part,
                      bcmi_keygen_cfg_t *keygen_cfg,
                      bcmi_keygen_md_t *keygen_md)
{
    uint8 level = 0;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_ext_finfo_db_free(unit, part, level, keygen_md));
    }
    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(bcmi_keygen_ext_cfg_reset_part(unit, part, keygen_cfg, keygen_md));
    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(bcmi_keygen_ext_finfo_add(unit, part, keygen_md));
    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(bcmi_keygen_ext_codes_validate(unit, part, keygen_cfg, keygen_md));
    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_ext_alloc_2(unit, part, level, keygen_cfg, keygen_md));
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_ext_fields_get_next(unit, part, level, keygen_cfg, keygen_md));
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC void
bcmi_keygen_ext_finfo_list_init(int unit)
{
    uint16 idx = 0;

    sal_memset(&finfo_pool[unit], 0,
        sizeof(bcmi_keygen_ext_field_info_t) * BCMI_KEYGEN_EXT_FINFO_COUNT);
    for (idx = 0; idx < (BCMI_KEYGEN_EXT_FINFO_COUNT - 1); idx++) {
         finfo_pool[unit][idx].next = &(finfo_pool[unit][idx + 1]);
    }
    finfo_pool[unit][BCMI_KEYGEN_EXT_FINFO_COUNT - 1].next = NULL;
    finfo_free_list[unit] = &(finfo_pool[unit][0]);
    finfo_free_count = 1024;
    finfo_used_count = 0;
}

STATIC int
bcmi_keygen_ext_finfo_list_verify(int unit)
{
    uint16 idx = 0;
    bcmi_keygen_ext_field_info_t *finfo = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    finfo = finfo_free_list[unit];
    while (finfo != NULL) {
        idx++;
        finfo = finfo->next;
    }

    if (idx != BCMI_KEYGEN_EXT_FINFO_COUNT) {
        BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_validate(int unit,
                       uint8 part,
                       bcmi_keygen_md_t *keygen_md,
                       bcmi_keygen_ext_cfg_t *ext_cfg)
{
    uint8 p = part;
    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(ext_cfg, BCM_E_PARAM);

    if (ext_cfg->in_use == TRUE) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_MODE_HALF) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_MODE_SINGLE_ASET_WIDE) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_IPBM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0_HM)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1_HM)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB_HM)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_A) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_B) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_DROP) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_HIT)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_NEEDED)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_DST_REALM_ID)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_PKT_RESOLUTION) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_C) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_D) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_INT_PRI) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_IFP_DROP_VECTOR) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMI_KEYGEN_ACL_ATTR_PMUX_MH_OPCODE) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_CONFIG);
    }
exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_alloc(int unit,
                    bcmi_keygen_cfg_t *keygen_cfg,
                    bcmi_keygen_oper_t *keygen_oper,
                    bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    uint8 part = 0;
    uint8 level = 0;
    uint16 size = 0;
    bcm_error_t rv = BCM_E_NONE;
    uint8 field_extracted = FALSE;
    bcmi_keygen_ext_section_t section;
    bcmi_keygen_ext_field_info_t *finfo = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    bcmi_keygen_ext_finfo_list_init(unit);

    size = (sizeof(uint32) * BCMI_KEYGEN_ENTRY_PARTS_MAX *
                               BCMI_KEYGEN_EXT_GRAN_COUNT);
    sal_memset(keygen_md->bits_extracted, 0, size);
    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
        bcmi_keygen_ext_fields_get_first(unit, keygen_cfg, keygen_md));

    if (keygen_cfg->num_parts == 1) {
        for (idx = 0; idx < BCMI_KEYGEN_EXT_FIELDS_MAX; idx++) {
            finfo = &(keygen_md->finfo[idx]);
            section = finfo->section;
            if (section == BCMI_KEYGEN_EXT_SECTION_DISABLE) {
                break;
            }
            finfo->part = 0;
        }
    } else {
        for (idx = 0; idx < BCMI_KEYGEN_EXT_FIELDS_MAX; idx++) {
            finfo = &(keygen_md->finfo[idx]);
            field_extracted = FALSE;
            section = finfo->section;
            if (section == BCMI_KEYGEN_EXT_SECTION_DISABLE) {
                break;
            }
            keygen_md->tried_part_bmp[0] = 0;
            for (part = 0; part < keygen_cfg->num_parts; part++) {
                BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                    bcmi_keygen_ext_part_get(unit, keygen_cfg,
                                             keygen_md, finfo));
                if (finfo->part < 0) {
                    BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_INTERNAL);
                }
                _func_rv = bcmi_keygen_ext_alloc_1(unit, finfo->part,
                                                   keygen_cfg, keygen_md);
                if (BCMI_KEYGEN_FUNC_VAL_IS(BCM_E_RESOURCE)) {
                   finfo->part = -1;
                   continue;
                } else if (BCMI_KEYGEN_FAILURE(_func_rv)) {
                    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(_func_rv);
                } else {
                   field_extracted = TRUE;
                   if (finfo->part < 0) {
                       BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_INTERNAL);
                   }
                   keygen_md->bits_extracted[finfo->part][0] += finfo->size;
                   break;
                }
            }
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                (field_extracted == FALSE) ? BCM_E_RESOURCE : BCM_E_NONE);
        }
    }

    for (part = 0; part < keygen_cfg->num_parts; part++) {
         BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
             bcmi_keygen_ext_alloc_1(unit, part, keygen_cfg, keygen_md));
    }
    /* Assign the offsets in the final key for all the qualifiers. */
    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
        bcmi_keygen_qual_offset_create(unit, keygen_cfg, keygen_oper, keygen_md));
    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
        bcmi_keygen_ext_codes_create(unit, keygen_cfg, keygen_md, keygen_oper));
exit:
    for (part = 0; part < keygen_cfg->num_parts; part++) {
         for (level = 0; level < keygen_md->num_ext_levels; level++) {
             (void)bcmi_keygen_ext_finfo_db_free(unit, part, level, keygen_md);
         }
         (void)bcmi_keygen_ext_cfg_reset_part(unit, part, keygen_cfg, keygen_md);
    }
    rv = bcmi_keygen_ext_finfo_list_verify(unit);
    if (BCM_FAILURE(rv)) {
        _func_rv = rv;
    }
    BCMI_KEYGEN_FUNC_EXIT();
}
