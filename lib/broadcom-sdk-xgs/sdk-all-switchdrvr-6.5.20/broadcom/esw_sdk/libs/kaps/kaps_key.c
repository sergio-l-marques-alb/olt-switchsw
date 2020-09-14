/*
 **************************************************************************************
 Copyright 2012-2019 Broadcom Corporation

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

#include "kaps_ab.h"
#include "kaps_key_internal.h"
#include "kaps_device_internal.h"
#include "kaps_device_wb.h"

/**

   @addtogroup KEY

   <B> \e \#include \e "key.h", \e libkbp.a </B>

   The key-processing unit (KPU) in hardware provides users flexibility in generating search keys
   for various databases participating in an instruction through key manipulation and
   preprocessing. The KPU offers ability for users to create unique
   keys for each database search that is part of the instruction. The control-plane-key APIs,
   in conjunction with the database and instruction APIs, allow logical expression of
   the elements in the master search key and the databases. The KPU assembles
   four unique keys using up to 640 bits of data from the context buffer. Byte-level parsing is
   used across the entire 640 bits of data. Up to 10 segments from the context buffer can be used
   to generate search keys for compare operations. Each of the four independent KPU pipelines
   in the processor is capable of generating search keys up to 640 bits wide.

   @image html KPU_example.png "KPU Processing Example"
   @image latex KPU_example.png "KPU Processing Example" width=15cm
   @image rtf KPU_example.png "KPU Processing Example"

   The key APIs in software provide abstraction to the KPUs and allow users to intuitively
   define and describe the key fields, and provide for easy detection of errors. Construction
   of the key starts by first calling the API kaps_key_init().

   Individual key fields can then be stacked from MSB to LSB to construct the
   full-width key using the API kaps_key_add_field().

   Finally, keys are associated with ACL, LPM, or EM databases as local search
   keys using the API kaps_db_set_key().

   For instructions, the key can be set as the master search key using the API kaps_instruction_set_key().

   The example in the diagram below shows a sample database key layout for an ACL database. The code following the diagram
   demonstrates how the key is constructed and associated with the database.

   @image html db_key.png "ACL Database Key Layout"
   @image latex db_key.png "ACL Database Key Layout" width=10cm
   @image rtf db_key.png "ACL Database Key Layout"

   The following ACL initialization code is from the packaged example simple_acl.c.

   @snippet simple_acl.c acl db init example

   Similarly an LPM database might describe a key layout as shown in the diagram below.

   @image html lpm_db_key.png "LPM Database Key Layout"
   @image latex lpm_db_key.png "LPM Database Key Layout" width=8cm
   @image rtf lpm_db_key.png "LPM Database Key Layout"

   Finally the ASIC/NPU may transmit the following master key in one
   of the search scenarios, where an instruction would be
   created with the master key as shown in the diagram below.

   @image html master_key.png  "Sample Master Key for ACL and LPM Combined"
   @image latex master_key.png "Sample Master Key for ACL and LPM Combined" width=15cm
   @image rtf master_key.png  "Sample Master Key for ACL and LPM Combined"

   @code

   @comment Initialize the key
   kaps_key *key;
   kaps_key_init(device, &key);

   @comment Describe the fields in order starting from MSB
   kaps_key_add_field(key, "DIP", 128, KAPS_KEY_FIELD_PREFIX);
   kaps_key_add_field(key, "SIP", 128, KAPS_KEY_FIELD_PREFIX);
   kaps_key_add_field(key, "SPORT", 16, KAPS_KEY_FIELD_RANGE);
   kaps_key_add_field(key, "DPORT", 16, KAPS_KEY_FIELD_RANGE);
   kaps_key_add_field(key, "FLAGS", 16, KAPS_KEY_FIELD_TERNARY);
   kaps_key_add_field(key, "TID", 8, KAPS_KEY_FIELD_EM);
   kaps_key_add_field(key, "VRF", 16, KAPS_KEY_FIELD_EM);

   @comment Add the key to an instruction. One key can be associated with a single database or instruction
   kaps_instruction_add_key(instruction, key);

   @comment key extraction gets defined as databases get added to instruction
   kaps_instruction_add_db(instruction, lpm_db, 0);
   kaps_instruction_add_db(instruction, acl_db, 1);
   kaps_instruction_install(instruction);

   @endcode

   The KPUs are automatically programmed by the SDK to generate
   two sub keys from the master key for the specific instruction.

   @image html key_extraction.png "Key Extraction Example"
   @image latex key_extraction.png "Key Extraction Example" width=15cm
   @image rtf key_extraction.png "Key Extraction Example"

   The utility function kaps_key_verify() is provided to
   allow the user to check if a specific database key can be derived
   from a master key. This condition is implicitly checked for all
   databases and master keys when the kaps_instruction_install()
   API is called.

   In addition print utility kaps_key_print() is available to pretty-print the key.
*/

kaps_status
kaps_key_init(
    struct kaps_device *device,
    struct kaps_key **key)
{
    struct kaps_key *new_key;

    KAPS_TRACE_IN("%p %p\n", device, key);
    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    if (!key)
        return KAPS_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    new_key = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_key));
    if (!new_key)
    {
        return KAPS_OUT_OF_MEMORY;
    }

    new_key->device = device;
    *key = new_key;
    KAPS_TRACE_OUT("%p\n", *key);

    return KAPS_OK;
}

void
kaps_key_destroy_internal(
    struct kaps_key *key)
{
    struct kaps_key_field *f;
    if (!key)
        return;

    f = key->first_field;
    while (f)
    {
        struct kaps_key_field *tmp = f->next;

        key->device->alloc->xfree(key->device->alloc->cookie, f);
        f = tmp;
    }

    f = key->first_overlay_field;
    while (f)
    {
        struct kaps_key_field *tmp = f->next;

        key->device->alloc->xfree(key->device->alloc->cookie, f);
        f = tmp;
    }

    key->device->alloc->xfree(key->device->alloc->cookie, key);
}

void
kaps_key_adjust_offsets(
    struct kaps_key *key)
{
    struct kaps_key_field *tmp;
    uint32_t cur_offset_1;

    /*
     * Re-adjust all field offsets and key size
     */
    cur_offset_1 = 0;
    for (tmp = key->first_field; tmp; tmp = tmp->next)
    {
        tmp->offset_1 = cur_offset_1;
        cur_offset_1 += tmp->width_1;
    }
    key->width_1 = cur_offset_1;
}

/*
    This function moves the field to the specified offset.
    And the offset_1 must match with any of the field's offset.
 */
void
kaps_key_move_internal(
    struct kaps_key *key,
    struct kaps_key_field *field,
    uint32_t offset_1)
{
    struct kaps_key_field *tmp;
    uint32_t found = 0;

    kaps_sassert(field);

    if (field->offset_1 == offset_1)
        return;

    /*
     * isolate the field from the key
     */
    if (field->prev)
        field->prev->next = field->next;
    if (field->next)
        field->next->prev = field->prev;

    /*
     * update the first and last field if if is changed, after separating the field
     */
    if (key->first_field == field)
        key->first_field = field->next;
    if (key->last_field == field)
        key->last_field = field->prev;

    for (tmp = key->first_field; tmp; tmp = tmp->next)
    {
        if (tmp->offset_1 == offset_1)
        {
            found = 1;
            break;
        }
    }
    kaps_sassert(found);

    if (offset_1 < field->offset_1)
    {
        /*
         * moving it in backward direction
         */
        field->next = tmp;
        field->prev = tmp->prev;
        if (tmp->prev)
            tmp->prev->next = field;
        tmp->prev = field;
        if (offset_1 == 0)
            key->first_field = field;
    }
    else
    {
        /*
         * moving it in forward direction
         */
        if (tmp->next)
            tmp->next->prev = field;
        field->next = tmp->next;
        field->prev = tmp;
        tmp->next = field;
        if (key->last_field == tmp)
            key->last_field = field;
    }

    kaps_key_adjust_offsets(key);
}

kaps_status
kaps_key_add_field_internal(
    struct kaps_key *key,
    char *name,
    uint32_t width_1,
    enum kaps_key_field_type type,
    uint32_t is_user_bmr)
{
    struct kaps_key_field *field;
    uint32_t size8;

    if (!name || !key)
        return KAPS_INVALID_ARGUMENT;

    if ((width_1 == 0) || (width_1 > KAPS_HW_MAX_DBA_WIDTH_1))
        return KAPS_INVALID_KEY_WIDTH;

    if ((key->device->type != KAPS_DEVICE_KAPS) && (width_1 % 8 != 0))
        return KAPS_INVALID_KEY_WIDTH;

    if (type >= KAPS_KEY_FIELD_INVALID)
        return KAPS_INVALID_KEY_TYPE;

    if (key->device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_KEY_FIELD_HOLE)
            continue;
        if (strcmp(field->name, name) == 0)
            return KAPS_DUPLICATE_KEY_FIELD;
    }

    if (type == KAPS_KEY_FIELD_HOLE && is_user_bmr)
        key->has_user_bmr = 1;

    size8 = sizeof(*field) + strlen(name) + 1;
    field = key->device->alloc->xcalloc(key->device->alloc->cookie, 1, size8);
    if (!field)
    {
        return KAPS_OUT_OF_MEMORY;
    }

    field->name = (char *) field + sizeof(*field);
    strcpy(field->name, name);
    field->width_1 = width_1;
    field->type = type;
    field->next = NULL;
    field->prev = NULL;
    key->width_1 += width_1;
    field->rinfo = NULL;
    field->do_not_bmr = 0;
    field->is_usr_bmr = 0;
    field->is_padding_field = 0;
    if (type == KAPS_KEY_FIELD_HOLE && is_user_bmr)
        field->is_usr_bmr = 1;

    if (!key->first_field)
    {
        /*
         * This is the first field being added
         */
        field->offset_1 = 0;
        key->first_field = field;
    }
    else
    {
        
        field->offset_1 = key->last_field->offset_1 + key->last_field->width_1;
        key->last_field->next = field;
        field->prev = key->last_field;
    }

    field->orig_offset_1 = field->offset_1;
    if (type == KAPS_KEY_FIELD_RANGE)
        key->nranges++;
    if (type == KAPS_KEY_FIELD_DUMMY_FILL)
        key->has_dummy_fill_field = 1;
    key->last_field = field;
    return KAPS_OK;
}

kaps_status
kaps_key_add_field(
    struct kaps_key * key,
    char *name,
    uint32_t width_1,
    enum kaps_key_field_type type)
{
    uint32_t is_user_bmr = 0;

    KAPS_TRACE_IN("%p %s %u %d\n", key, name, width_1, type);
    if (type == KAPS_KEY_FIELD_HOLE)
        is_user_bmr = 1;

    return kaps_key_add_field_internal(key, name, width_1, type, is_user_bmr);
}

kaps_status
kaps_key_set_critical_field(
    struct kaps_key * key,
    char *name)
{
    struct kaps_key_field *field;

    KAPS_TRACE_IN("%p %s\n", key, name);
    if (!name || !key)
        return KAPS_INVALID_ARGUMENT;

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_KEY_FIELD_HOLE)
            continue;
        if (field->type == KAPS_KEY_FIELD_RANGE)
            continue;
        if (field->type == KAPS_KEY_FIELD_DUMMY_FILL)
            continue;
        if (strcmp(field->name, name) == 0)
        {
            break;
        }
    }

    if (!field)
        return KAPS_INVALID_ARGUMENT;

    if (field->field_prio)
        return KAPS_INVALID_ARGUMENT;

    key->critical_field_count++;
    field->field_prio = key->critical_field_count;

    return KAPS_OK;
}

kaps_status
kaps_key_overlay_field(
    struct kaps_key * key,
    char *name,
    uint32_t width_1,
    enum kaps_key_field_type type,
    uint32_t offset_1)
{
    struct kaps_key_field *field;
    uint32_t size8;

    KAPS_TRACE_IN("%p %s %u %d %u\n", key, name, width_1, type, offset_1);
    if (!name || !key)
        return KAPS_INVALID_ARGUMENT;

    if (key->device->type == KAPS_DEVICE_KAPS)
        return KAPS_UNSUPPORTED;

    if (width_1 % 8 != 0)
        return KAPS_INVALID_KEY_WIDTH;

    if ((width_1 == 0) || (width_1 >= key->width_1))
        return KAPS_INVALID_KEY_WIDTH;

    if (offset_1 % 8 != 0)
        return KAPS_INVALID_ARGUMENT;

    if ((offset_1 + width_1) > key->width_1)
        return KAPS_INVALID_ARGUMENT;

    if (type >= KAPS_KEY_FIELD_INVALID)
        return KAPS_INVALID_KEY_TYPE;

    if (key->device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    if (type == KAPS_KEY_FIELD_HOLE)
        return KAPS_INVALID_ARGUMENT;

    for (field = key->first_overlay_field; field; field = field->next)
    {
        if (strcmp(field->name, name) == 0)
            return KAPS_DUPLICATE_KEY_FIELD;
    }

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_KEY_FIELD_HOLE)
            continue;
        if (strcmp(field->name, name) == 0)
            return KAPS_DUPLICATE_KEY_FIELD;
    }

    size8 = sizeof(*field) + strlen(name) + 1;
    field = key->device->alloc->xcalloc(key->device->alloc->cookie, 1, size8);
    if (!field)
    {
        return KAPS_OUT_OF_MEMORY;
    }

    field->name = (char *) field + sizeof(*field);
    strcpy(field->name, name);
    field->width_1 = width_1;
    field->type = type;
    field->next = NULL;
    field->prev = NULL;

    field->rinfo = NULL;
    field->do_not_bmr = 0;
    field->is_usr_bmr = 0;
    field->is_padding_field = 0;

    field->offset_1 = offset_1;

    if (!key->first_overlay_field)
    {
        key->first_overlay_field = field;
    }
    else
    {
        key->last_overlay_field->next = field;
        field->prev = key->last_overlay_field;
    }

    key->last_overlay_field = field;

    return KAPS_OK;
}

kaps_status
kaps_key_verify(
    struct kaps_key * master_key,
    struct kaps_key * db_key,
    char **error_field_name)
{
    int32_t start = -1, end = -1;
    struct kaps_key_field *db_key_field;

    KAPS_TRACE_IN("%p %p %p\n", master_key, db_key, error_field_name);
    if (!db_key)
        return KAPS_INVALID_KEY;
    if (!master_key)
        return KAPS_INVALID_MASTER_KEY;
    if (master_key->device->type == KAPS_DEVICE_KAPS)
        return KAPS_OK;

    db_key_field = db_key->first_field;

    while (db_key_field)
    {
        struct kaps_key_field *master_key_field = master_key->first_field;

        if (db_key_field->type == KAPS_KEY_FIELD_HOLE)
        {
            db_key_field = db_key_field->next;
            continue;
        }
        while (master_key_field)
        {
            if (strcmp(master_key_field->name, db_key_field->name) == 0)
            {
                if (master_key_field->width_1 == db_key_field->width_1)
                {
                    if (master_key_field->type != KAPS_KEY_FIELD_DUMMY_FILL)
                    {
                        if (master_key_field->type == db_key_field->type)
                            break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            master_key_field = master_key_field->next;
        }

        if (master_key_field == NULL && master_key->first_overlay_field)
        {
            master_key_field = master_key->first_overlay_field;
            while (master_key_field)
            {
                if (strcmp(master_key_field->name, db_key_field->name) == 0)
                {
                    if (master_key_field->width_1 == db_key_field->width_1)
                    {
                        if (master_key_field->type != KAPS_KEY_FIELD_DUMMY_FILL)
                        {
                            if (master_key_field->type == db_key_field->type)
                                break;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                master_key_field = master_key_field->next;
            }
        }

        if (master_key_field == NULL)
        {

            if (error_field_name)
            {
                *error_field_name = db_key_field->name;
                KAPS_TRACE_OUT("%s\n", *error_field_name);
            }
            return KAPS_KEY_FIELD_MISSING;
        }

        if (start == -1)
        {
            kaps_sassert(end == -1);
            start = master_key_field->offset_1;
            end = start + master_key_field->width_1;
        }
        else if (master_key_field->offset_1 == end)
        {
            end = end + master_key_field->width_1;
        }
        else
        {
            if (start & 0x7 || end & 0x7)
            {
                return KAPS_KEY_GRAN_ERROR;
            }
            start = master_key_field->offset_1;
            end = start + master_key_field->width_1;
        }

        db_key_field = db_key_field->next;
    }
    if (start != -1)
    {
        if (start & 0x7)
        {
            return KAPS_KEY_GRAN_ERROR;
        }
    }
    return KAPS_OK;
}

char *
kaps_key_get_type_internal(
    enum kaps_key_field_type type)
{
    if (type == KAPS_KEY_FIELD_TERNARY)
        return "ternary";
    else if (type == KAPS_KEY_FIELD_PREFIX)
        return "prefix";
    else if (type == KAPS_KEY_FIELD_EM)
        return "em";
    else if (type == KAPS_KEY_FIELD_RANGE)
        return "range";
    else if (type == KAPS_KEY_FIELD_TABLE_ID)
        return "tid";
    else if (type == KAPS_KEY_FIELD_DUMMY_FILL)
        return "dummyfill";
    else
        return "hole";
}

kaps_status
kaps_key_print(
    struct kaps_key * key,
    FILE * file)
{
    struct kaps_key_field *field;
    uint32_t fcount = 0, count = 0, next_offset = 0;

    KAPS_TRACE_IN("%p %p\n", key, file);
    if (!key || !file)
        return KAPS_INVALID_ARGUMENT;

    if (key->device->flags & KAPS_DEVICE_ISSU)
    {
        if (key->device->issu_status != KAPS_ISSU_INIT)
        {
            return KAPS_ISSU_IN_PROGRESS;
        }
    }

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_KEY_FIELD_HOLE && !field->is_usr_bmr)
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
            kaps_fprintf(file, "(%s,%d,%s) ", field->name, field->width_1, kaps_key_get_type_internal(field->type));
            break;
        }
    }
    while (count < fcount);

    return KAPS_OK;
}

kaps_status
kaps_key_print_internal(
    struct kaps_key * key,
    FILE * file)
{
    struct kaps_key_field *field;

    if (!key || !file)
        return KAPS_INVALID_ARGUMENT;

    if (key->device->flags & KAPS_DEVICE_ISSU)
    {
        if (key->device->issu_status != KAPS_ISSU_INIT)
        {
            return KAPS_ISSU_IN_PROGRESS;
        }
    }

    for (field = key->first_field; field; field = field->next)
    {
        kaps_fprintf(file, "(%s,%d,%s)", field->name, field->width_1, kaps_key_get_type_internal(field->type));
    }
    kaps_fprintf(file, "\n");
    return KAPS_OK;
}

kaps_status
kaps_key_clone(
    struct kaps_device * device,
    struct kaps_key * key,
    struct kaps_key ** result)
{
    struct kaps_key *new_key;
    struct kaps_key_field *field;
    int32_t count, fcount;
    uint32_t next_offset;
    kaps_status status;

    if (device == NULL)
        return KAPS_INVALID_DEVICE_PTR;

    if (key == NULL || result == NULL)
        return KAPS_INVALID_ARGUMENT;

    status = kaps_key_init(device, &new_key);
    if (status != KAPS_OK)
        return status;

    /*
     * Key elements may have been shuffled, we need to
     * ensure its copied in the correct original order
     */

    count = fcount = 0;
    next_offset = 0;
    for (field = key->first_field; field; field = field->next)
    {
        fcount++;
    }

    status = KAPS_OK;
    do
    {
        for (field = key->first_field; field; field = field->next)
        {
            if (next_offset != field->orig_offset_1)
                continue;
            count++;
            next_offset = next_offset + field->width_1;
            status = kaps_key_add_field_internal(new_key, field->name, field->width_1, field->type, field->is_usr_bmr);
            break;
        }
        if (status != KAPS_OK)
            break;
    }
    while (count < fcount);

    for (field = key->first_overlay_field; field; field = field->next)
    {
        status = kaps_key_overlay_field(new_key, field->name, field->width_1, field->type, field->offset_1);
        if (status != KAPS_OK)
            break;
    }

    if (status != KAPS_OK)
    {
        kaps_key_destroy_internal(new_key);
        new_key = NULL;
    }

    *result = new_key;
    return status;
}

kaps_status
kaps_key_validate_internal(
    struct kaps_db * db,
    struct kaps_key * key)
{
    if (!db || !key)
        return KAPS_INVALID_ARGUMENT;

    if (db->is_clone)
    {
        /*
         * If we are adding the key to the clone, there is
         * no requirement of having table ID similar to
         * tables within database. We only ensure the
         * key fields are exactly the same type, width
         * and in same order for the two clones
         */

        struct kaps_key_field *f1, *f2;

        if (db->clone_of->key == NULL)
            return KAPS_CLONE_PARENT_KEY_MISSING;

        f1 = key->first_field;
        f2 = db->clone_of->key->first_field;
        for (; f1 && f2; f2 = f2->next, f1 = f1->next)
        {
            /*
             * for LPM wont support the holes, support for lpm-mapped-acl
             */
            if (f1->type == KAPS_KEY_FIELD_HOLE && db->type == KAPS_DB_LPM && !db->common_info->mapped_to_acl)
                return KAPS_LPM_KEY_WITH_HOLE_UNSUPPORTED;

            /*
             * Range fields cannot BMR out, as of now not supported
             */
            if (f1->type == KAPS_KEY_FIELD_HOLE && f2->type == KAPS_KEY_FIELD_RANGE)
                return KAPS_RANGE_FIELDS_AS_HOLE_UNSUPPORTED;

            /*
             * Table id field cannot BMR out, as of now not supported
             */
            if (f1->type == KAPS_KEY_FIELD_HOLE && f2->type == KAPS_KEY_FIELD_TABLE_ID)
                return KAPS_TABLE_ID_AS_HOLE_UNSUPPORTED;

            if ((f1->width_1 != f2->width_1 || f1->type != f2->type)
                && (f1->type != KAPS_KEY_FIELD_HOLE && f2->type != KAPS_KEY_FIELD_HOLE))
                return KAPS_CLONE_KEY_FORMAT_MISMATCH;

            f1->p_field = f2;
        }
        if (f1 != NULL || f2 != NULL)
            return KAPS_CLONE_KEY_FORMAT_MISMATCH;
    }
    else if (db->parent || db->has_tables)
    {
        /*
         * If we are adding tables, ensure the key
         * has the table ID key segment as part of it
         */
        struct kaps_db *cur_db;
        int32_t table_id_offset = -1, tid_width = -1;

        if (db->parent)
            cur_db = db->parent;
        else
            cur_db = db;

        for (; cur_db; cur_db = cur_db->next_tab)
        {
            struct kaps_key *cur_db_key = cur_db->key;
            struct kaps_key_field *f;
            int32_t num_tid = 0;

            if ((struct kaps_db *) cur_db == db)
                cur_db_key = key;

            if (!cur_db_key || cur_db->is_clone)
                continue;

            for (f = cur_db_key->first_field; f; f = f->next)
            {
                if (f->type == KAPS_KEY_FIELD_TABLE_ID)
                {
                    num_tid++;

                    if (table_id_offset == -1)
                    {
                        table_id_offset = f->offset_1;
                        tid_width = f->width_1;
                    }
                    else if (f->offset_1 != table_id_offset)
                    {
                        return KAPS_INCONSISTENT_TABLE_ID_OFFSET;
                    }
                    else if (f->width_1 != tid_width)
                    {
                        return KAPS_INCONSISTENT_TABLE_ID_WIDTH;
                    }
                }
            }

            if (num_tid != 1)
                return KAPS_MISSING_TABLE_ID_FIELD;
        }
    }

    return KAPS_OK;
}

/**
 * Warmboot container structure for Key
 */
struct kaps_key_wb_info
{
    uint32_t fcount;      /**< Number of fields */
    uint32_t f_name_len;  /**< Length of key field name in terms of bits */
    uint32_t ftype;       /**< Field type */
    uint32_t width_1;     /**< width of key field in bits */
    uint8_t fname[];     /**< Name of key field */
};

struct kaps_key_overlay_wb_info
{
    uint32_t f_name_len;  /**< Length of key field name in terms of bits */
    uint32_t ftype;       /**< Field type */
    uint32_t width_1;     /**< width of key field in bits */
    uint32_t offset_1;    /**< offset of key field in bits */
    uint8_t fname[];     /**< Name of key field */
};

kaps_status
kaps_key_wb_save(
    struct kaps_key *key,
    struct kaps_wb_cb_functions *wb_fun)
{
    int32_t count, fcount = 0;
    uint32_t next_offset;
    uint32_t overlay_field_cnt = 0;
    struct kaps_key_field *field;

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_KEY_FIELD_HOLE && !field->is_usr_bmr && !field->is_padding_field)
            continue;
        fcount++;
    }

    /*
     * Key elements may have been shuffled, we need to
     * ensure its copied in the correct original order
     */

    count = 0;
    next_offset = 0;

    do
    {
        for (field = key->first_field; field; field = field->next)
        {
            struct kaps_key_wb_info *wb_info_ptr;
            uint32_t total_size;

            if (next_offset != field->orig_offset_1)
                continue;

            count++;
            next_offset = next_offset + field->width_1;
            total_size = sizeof(*wb_info_ptr) + strlen(field->name) + 1;
            if (wb_fun->nv_ptr == NULL)
            {
                wb_info_ptr = key->device->alloc->xmalloc(key->device->alloc->cookie, total_size);
                if (wb_info_ptr == NULL)
                    return KAPS_OUT_OF_MEMORY;
            }
            else
            {
                wb_info_ptr = (struct kaps_key_wb_info *) wb_fun->nv_ptr;
            }

            kaps_memset(wb_info_ptr, 0, total_size);
            wb_info_ptr->fcount = fcount;
            wb_info_ptr->f_name_len = strlen(field->name) + 1;
            wb_info_ptr->ftype = field->type;
            wb_info_ptr->width_1 = field->width_1;
            kaps_memcpy(wb_info_ptr->fname, field->name, wb_info_ptr->f_name_len);

            if (wb_fun->write_fn)
            {
                if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, total_size, *wb_fun->nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;
            }
            *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
            if (wb_fun->nv_ptr)
            {
                wb_fun->nv_ptr += total_size;
            }
            else
            {
                key->device->alloc->xfree(key->device->alloc->cookie, wb_info_ptr);
            }
            break;
        }
    }
    while (count < fcount);

    /*
     * Save overlay fields
     */
    overlay_field_cnt = 0;
    for (field = key->first_overlay_field; field; field = field->next)
    {
        overlay_field_cnt++;
    }

    if (wb_fun->write_fn)
    {
        if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & overlay_field_cnt,
                                  sizeof(overlay_field_cnt), *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
    }
    else
    {
        *((uint32_t *) wb_fun->nv_ptr) = overlay_field_cnt;
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(overlay_field_cnt);
    if (wb_fun->nv_ptr)
        wb_fun->nv_ptr += sizeof(overlay_field_cnt);

    for (field = key->first_overlay_field; field; field = field->next)
    {
        uint32_t total_size;
        struct kaps_key_overlay_wb_info *wb_info_ptr;

        total_size = sizeof(*wb_info_ptr) + strlen(field->name) + 1;
        if (wb_fun->nv_ptr == NULL)
        {
            wb_info_ptr = key->device->alloc->xmalloc(key->device->alloc->cookie, total_size);
            if (wb_info_ptr == NULL)
                return KAPS_OUT_OF_MEMORY;
        }
        else
        {
            wb_info_ptr = (struct kaps_key_overlay_wb_info *) wb_fun->nv_ptr;
        }

        kaps_memset(wb_info_ptr, 0, total_size);
        wb_info_ptr->f_name_len = strlen(field->name) + 1;
        wb_info_ptr->ftype = field->type;
        wb_info_ptr->width_1 = field->width_1;
        wb_info_ptr->offset_1 = field->offset_1;
        kaps_memcpy(wb_info_ptr->fname, field->name, wb_info_ptr->f_name_len);

        if (wb_fun->write_fn)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, total_size, *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
        if (wb_fun->nv_ptr)
        {
            wb_fun->nv_ptr += total_size;
        }
        else
        {
            key->device->alloc->xfree(key->device->alloc->cookie, wb_info_ptr);
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_key_wb_restore(
    struct kaps_key * key,
    struct kaps_wb_cb_functions * wb_fun)
{
    int32_t f_no, fcount = 0;
    kaps_status status = KAPS_OK;
    struct kaps_key_wb_info wb_info, *wb_info_ptr;
    uint32_t overlay_field_cnt = 0;

    if (wb_fun->nv_ptr == NULL)
    {
        wb_info_ptr = &wb_info;
    }
    else
    {
        wb_info_ptr = (struct kaps_key_wb_info *) wb_fun->nv_ptr;
    }

    /*
     * Read once to get the number of fields
     */
    if (wb_fun->read_fn != NULL)
    {
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
    }

    fcount = wb_info_ptr->fcount;
    for (f_no = 0; f_no < fcount; f_no++)
    {
        struct kaps_key_wb_info *act_info = NULL;
        uint32_t total_size;

        if (wb_fun->nv_ptr)
            wb_info_ptr = (struct kaps_key_wb_info *) wb_fun->nv_ptr;

        /*
         * Read only the header first to get the length of name field
         */
        if (wb_fun->read_fn != NULL)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }

        total_size = wb_info_ptr->f_name_len + sizeof(*wb_info_ptr);

        if (wb_fun->read_fn != NULL)
        {
            act_info = key->device->alloc->xcalloc(key->device->alloc->cookie, 1, total_size);
            if (act_info == NULL)
                return KAPS_OUT_OF_MEMORY;
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) act_info, total_size, *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        else
        {
            act_info = (struct kaps_key_wb_info *) wb_fun->nv_ptr;
        }

        kaps_sassert(act_info);
        status = kaps_key_add_field(key, (char *) act_info->fname, act_info->width_1, act_info->ftype);

        if (wb_fun->read_fn != NULL)
        {
            key->device->alloc->xfree(key->device->alloc->cookie, act_info);
        }

        if (status != KAPS_OK)
            return status;
        *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
        if (wb_fun->nv_ptr)
            wb_fun->nv_ptr += total_size;
    }

    /*
     * Restore overlay fields
     */

    if (wb_fun->read_fn)
    {
        if (0 !=
            wb_fun->read_fn(wb_fun->handle, (uint8_t *) & overlay_field_cnt, sizeof(overlay_field_cnt),
                            *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
    }
    else
    {
        overlay_field_cnt = *((uint32_t *) wb_fun->nv_ptr);
        wb_fun->nv_ptr += sizeof(overlay_field_cnt);
    }
    *wb_fun->nv_offset += sizeof(overlay_field_cnt);

    for (f_no = 0; f_no < overlay_field_cnt; f_no++)
    {
        struct kaps_key_overlay_wb_info *act_info = NULL;
        struct kaps_key_overlay_wb_info wb_info, *wb_info_ptr;
        uint32_t total_size;

        if (wb_fun->nv_ptr)
            wb_info_ptr = (struct kaps_key_overlay_wb_info *) wb_fun->nv_ptr;
        else
            wb_info_ptr = &wb_info;

        /*
         * Read only the header first to get the length of name field
         */
        if (wb_fun->read_fn != NULL)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }

        total_size = wb_info_ptr->f_name_len + sizeof(*wb_info_ptr);

        if (wb_fun->read_fn != NULL)
        {
            act_info = key->device->alloc->xcalloc(key->device->alloc->cookie, 1, total_size);
            if (act_info == NULL)
                return KAPS_OUT_OF_MEMORY;
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) act_info, total_size, *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        else
        {
            act_info = (struct kaps_key_overlay_wb_info *) wb_fun->nv_ptr;
        }

        kaps_sassert(act_info);
        status = kaps_key_overlay_field(key, (char *) act_info->fname, act_info->width_1,
                                        act_info->ftype, act_info->offset_1);

        if (wb_fun->read_fn != NULL)
        {
            key->device->alloc->xfree(key->device->alloc->cookie, act_info);
        }

        if (status != KAPS_OK)
            return status;
        *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
        if (wb_fun->nv_ptr)
            wb_fun->nv_ptr += total_size;
    }

    return KAPS_OK;
}

kaps_status
kaps_key_wb_read(
    struct kaps_print_key ** key,
    FILE * bin_fp,
    uint32_t * nv_offset)
{
    int32_t f_no, fcount = 0;
    uint32_t f_name_len = 0;
    enum kaps_key_field_type f_type;
    uint16_t f_width;
    struct kaps_print_key *tmp;

    *key = (struct kaps_print_key *) kaps_sysmalloc(sizeof(struct kaps_print_key));

    tmp = *key;

    if (0 != kaps_device_wb_file_read(bin_fp, (uint8_t *) & fcount, sizeof(fcount), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(fcount);

    for (f_no = 0; f_no < fcount; f_no++)
    {
        if (0 != kaps_device_wb_file_read(bin_fp, (uint8_t *) & f_name_len, sizeof(f_name_len), *nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
        *nv_offset += sizeof(f_name_len);

        tmp->fname = (char *) kaps_sysmalloc(f_name_len * sizeof(char));

        if (!tmp->fname)
            return KAPS_OUT_OF_MEMORY;

        if (0 != kaps_device_wb_file_read(bin_fp, (uint8_t *) tmp->fname, f_name_len, *nv_offset))
        {
            kaps_sysfree(tmp->fname);
            kaps_sysfree(tmp);
            return KAPS_NV_READ_WRITE_FAILED;
        }
        *nv_offset += f_name_len;

        if (0 != kaps_device_wb_file_read(bin_fp, (uint8_t *) & f_type, sizeof(f_type), *nv_offset))
        {
            kaps_sysfree(tmp->fname);
            kaps_sysfree(tmp);
            return KAPS_NV_READ_WRITE_FAILED;
        }
        *nv_offset += sizeof(f_type);

        if (0 != kaps_device_wb_file_read(bin_fp, (uint8_t *) & f_width, sizeof(f_width), *nv_offset))
        {
            kaps_sysfree(tmp->fname);
            kaps_sysfree(tmp);
            return KAPS_NV_READ_WRITE_FAILED;
        }
        *nv_offset += sizeof(f_width);

        tmp->type = f_type;
        tmp->width = f_width;

        if (f_no < fcount - 1)
        {
            tmp->next = (struct kaps_print_key *) kaps_sysmalloc(sizeof(struct kaps_print_key));
            tmp = tmp->next;

            if (!tmp)
                return KAPS_OUT_OF_MEMORY;
        }
    }

    tmp->next = NULL;

    return KAPS_OK;
}

void
kaps_key_wb_print(
    struct kaps_print_key *key,
    FILE * txt_fp)
{
    if (!key)
        return;

    kaps_fprintf(txt_fp, " (%s,%d,%s) ", key->fname, key->width, kaps_key_get_type_internal(key->type));
    kaps_key_wb_print(key->next, txt_fp);
}

void
kaps_key_wb_free(
    struct kaps_print_key *key)
{
    if (!key)
        return;
    kaps_key_wb_free(key->next);
    kaps_sysfree(key->fname);
    kaps_sysfree(key);
}
