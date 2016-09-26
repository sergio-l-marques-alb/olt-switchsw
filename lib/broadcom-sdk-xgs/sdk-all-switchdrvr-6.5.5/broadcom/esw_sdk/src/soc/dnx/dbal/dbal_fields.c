/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal.h"
#include <shared/bsl.h>

/**************************************************DB Initialization ******************************************************************/
static dbal_field_basic_info_t field_basic_info[DBAL_NOF_FIELDS];

/* fields encoding functions generic and specific */

shr_error_e dbal_fields_encode_suffix(int unit, uint32 field_val, uint32 suffix_val, uint32 suffix_offset, uint32* field_val_out)
{
    
    (*field_val_out) = (field_val >> suffix_offset) | (suffix_val);
    return 0;
}

shr_error_e dbal_fields_decode_suffix(int unit, uint32 field_val, uint32 unused, uint32 suffix_offset, uint32* field_val_out)
{
    (*field_val_out) = (field_val << suffix_offset);

    return 0;
}

shr_error_e dbal_fields_encode_prefix(int unit, uint32 field_val, uint32 prefix_val, uint32 prefix_length, uint32* field_val_out)
{
    prefix_val = prefix_val << prefix_length;
    (*field_val_out) = (prefix_val) | (field_val);    
    return 0;
}

shr_error_e dbal_fields_decode_prefix(int unit, uint32 field_val, uint32 unused, uint32 prefix_length, uint32* field_val_out)
{
    uint32 mask = 0;

    mask = (1<<prefix_length) - 1;    
    (*field_val_out) = (field_val & mask);    
    return 0;
}

shr_error_e dbal_fields_encode_bool(int unit, uint32 field_val, uint32 unused1, uint32 unused2, uint32* field_val_out)
{
    if (field_val == 0)
    {
        (*field_val_out ) = 0;
    }else{
        (*field_val_out ) = 1;
    }
    return 0;
}

shr_error_e dbal_fields_decode_bool(int unit, uint32 field_val, uint32 unused1, uint32 unused2, uint32* field_val_out)
{
    if (field_val == 0)
    {
        (*field_val_out ) = 0;
    }else{
        (*field_val_out ) = 1;
    }

    return 0;
}

shr_error_e dbal_fields_encode_subtract(int unit, uint32 field_val, uint32 subtract_val, uint32 unused, uint32* field_val_out)
{
    (*field_val_out) = field_val - subtract_val;
    return 0;
}

shr_error_e dbal_fields_decode_subtract(int unit, uint32 field_val, uint32 subtracted_val, uint32 unused, uint32* field_val_out)
{
    (*field_val_out) = field_val + subtracted_val;
    return 0;
}

static
shr_error_e dbal_fields_validation(int unit)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < DBAL_NOF_FIELDS; i++) {        
        if (field_basic_info[i].type == DBAL_FIELD_TYPE_ARRAY8 && field_basic_info[i].max_size > DBAL_FIELD_ARRAY_MAX_SIZE*8) {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,"Error. array size too long %d \n",field_basic_info[i].max_size);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* this function run over all existing fields and assign the proper encoding and decoding functions*/
shr_error_e dbal_fields_init(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_db_init_fields_set_default(unit, field_basic_info))
    SHR_IF_ERR_EXIT(dbal_db_init_fields(unit, DBAL_INIT_FLAGS_NONE, field_basic_info));
    SHR_IF_ERR_EXIT(dbal_fields_validation(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dbal_fields_dump_all(int unit, dbal_labels_e label)
{
    int i,j;

    SHR_FUNC_INIT_VARS(unit);

    if ((label == DBAL_NOF_LABEL_TYPES) || (label == DBAL_LABEL_NONE ))
    {
        LOG_CLI((BSL_META("\nDumping all fields\n")));
        LOG_CLI((BSL_META("==================\n\n")));
    }else{
        LOG_CLI((BSL_META("\nDumping all fields related to label %s\n"), dbal_label_to_string(unit, label)));
        LOG_CLI((BSL_META("===============================================\n\n")));
    }

    LOG_CLI((BSL_META("Field name          |Bits size|Encode type|Sub field      |default value  |Related labels\n")));
    LOG_CLI((BSL_META("-----------------------------------------------------------------------------------------\n")));

    for (i = 1; i < DBAL_NOF_FIELDS; i++)
    {
        if (label == DBAL_LABEL_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_info_print(unit, i));            
        }else{
            for(j = 0; j < DBAL_NOF_LABEL_TYPES; j++)
                if( (field_basic_info[i].labels[j] == label) || (field_basic_info[i].labels[j] == DBAL_NOF_LABEL_TYPES)){
                    SHR_IF_ERR_EXIT(dbal_fields_field_info_print(unit, i));                    
                    break;
                }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dbal_fields_field_info_get(int unit, dbal_fields_e field_id, dbal_field_basic_info_t** field_info)
{
    (*field_info) = &field_basic_info[field_id];

    return 0;
}

shr_error_e dbal_fields_field32_encode(int unit, dbal_fields_e field_id, uint32 field_val, uint32* field_value_out)
{
    SHR_FUNC_INIT_VARS(unit);

    if (field_basic_info[field_id].field_32_encoding_func != NULL)
    {
        SHR_IF_ERR_EXIT(field_basic_info[field_id].field_32_encoding_func(unit, field_val, field_basic_info[field_id].value_input_param1, field_basic_info[field_id].value_input_param2, field_value_out));
    }else{
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "no encoding func for field %s\n", dbal_field_to_string(unit, field_id));
    }


exit:
    SHR_FUNC_EXIT;
}

shr_error_e dbal_fields_field32_decode(int unit, dbal_fields_e field_id, uint32 field_val, uint32* field_value_out)
{
    SHR_FUNC_INIT_VARS(unit);

    if (field_basic_info[field_id].field_32_decoding_func != NULL)
    {
        SHR_IF_ERR_EXIT(field_basic_info[field_id].field_32_decoding_func(unit, field_val, field_basic_info[field_id].value_input_param1, field_basic_info[field_id].value_input_param2, field_value_out));
    }else{
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "no decoding func for field %s\n", dbal_field_to_string(unit, field_id));
    }


exit:
    SHR_FUNC_EXIT;
}

shr_error_e dbal_fields_is_field_encoded(int unit, dbal_fields_e field_id)
{
    switch (field_basic_info[field_id].encode_type)
    {
    case DBAL_VALUE_ENCODE_NONE:
    case DBAL_VALUE_ENCODE_PARENT_FIELD:/* parent field no need encoding */
    case DBAL_VALUE_ENCODE_PARTIAL_KEY:/* No need encoding */
        return 0;
        break;

    default:
        return 1;
        break;
    }

    return 0;
}

shr_error_e dbal_fields_parent_field_id_get(int unit, dbal_fields_e field_id, dbal_fields_e* parent_field_id)
{
    (*parent_field_id) = field_basic_info[field_id].parent_field_id;

    return 0;
}


shr_error_e dbal_fields_max_size_get(int unit, dbal_fields_e field_id, uint32* field_size)
{
    (*field_size) = field_basic_info[field_id].max_size;

    return 0;
}
  
shr_error_e dbal_fields_type_get(int unit, dbal_fields_e field_id, dbal_field_type_e* field_type)
{
    (*field_type) = field_basic_info[field_id].type;

    return 0;
}
 
shr_error_e dbal_fields_encode_type_get(int unit, dbal_fields_e field_id, dbal_value_encode_types_e* encode_type)
{
    (*encode_type) = field_basic_info[field_id].encode_type;

    return 0;
}
 
shr_error_e dbal_fields_field_info_print(int unit, dbal_fields_e field_id)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (sal_strcmp(field_basic_info[field_id].name,"") == 0) {
        SHR_FUNC_EXIT;
    }

    /*Name*/
    LOG_CLI((BSL_META("%-20s| "),field_basic_info[field_id].name));

    /*Size+Encoding*/
    LOG_CLI((BSL_META("%-8d|%-11s|"), field_basic_info[field_id].max_size, dbal_encode_type_to_string(unit, field_basic_info[field_id].encode_type)));

    if (0) {
        /*Type*/
        LOG_CLI((BSL_META("%-15s|"), dbal_field_type_to_string(unit, field_basic_info[field_id].type)));
    }

    /*Parent*/
    if (field_basic_info[field_id].parent_field_id != DBAL_NOF_FIELDS){
        LOG_CLI((BSL_META("%-15s|"), field_basic_info[field_basic_info[field_id].parent_field_id].name));
    }else{
        LOG_CLI((BSL_META("%-15s|"), " - "));
    }

    /*default Value*/
    if (field_basic_info[field_id].is_default_value_valid){
        LOG_CLI((BSL_META("%-15d|"), field_basic_info[field_id].default_value));
    }else{
        LOG_CLI((BSL_META("%-15s|"), " - "));
    }

    /*LABELS*/
    for(i = 0; i < DBAL_NOF_LABEL_TYPES; i++) {
        if ((field_basic_info[field_id].labels[i] == DBAL_LABEL_NONE)&&(i != 0)) {
            break;
        }
        if (i == 0) {
            LOG_CLI((BSL_META("%s"), dbal_label_to_string(unit, field_basic_info[field_id].labels[i])));
        }else{
            LOG_CLI((BSL_META(",%s"), dbal_label_to_string(unit, field_basic_info[field_id].labels[i])));
        }
    }

    LOG_CLI((BSL_META("\n")));
    
    SHR_FUNC_EXIT;
}

