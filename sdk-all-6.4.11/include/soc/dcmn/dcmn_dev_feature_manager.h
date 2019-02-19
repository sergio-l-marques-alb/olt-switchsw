/*
 * $Id$ 
 *
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
#ifndef _DCMN_DEV_FEATURE_MANAGER_H_
#define _DCMN_DEV_FEATURE_MANAGER_H_

#include <soc/error.h>

#define INVALID_DEV_ID -1
typedef enum dcmn_feature_e {
    DCMN_INVALID_FEATURE=-1,
    DCMN_METRO_FEATURE,
    DCMN_DC_FEATURE,
    DCMN_OTN_FEATURE,
    DCMN_NIF_SERDES_FEATURE,
    DCMN_ILKN_PORTS_FEATURE,
    DCMN_FABRIC_ILKN_FEATURE,
    DCMN_FABRIC_MESH_FEATURE,
    DCMN_NUM_OF_FEATURES
} dcmn_feature_t;





typedef struct dcmn_range_s {
    int from;
    int to;
} dcmn_range_t;

typedef soc_error_t (*dcmn_propery_value_valid_cb) (int unit, void  *value,int nof_ranges,void *data);

typedef struct dcmn_feature_cb_s
{
    dcmn_feature_t feature;
    dcmn_propery_value_valid_cb cb;
    int nof_ranges;
    void *data;
} dcmn_feature_cb_t;

typedef struct dcmn_property_features_s
{
    char *property;
    dcmn_feature_cb_t  *features_array;
} dcmn_property_features_t;


soc_error_t dcmn_is_property_value_permited(int unit,const char *property,void *value);
soc_error_t dcmn_property_get(int unit, const char *name, uint32 defl,uint32 *value);
soc_error_t dcmn_property_suffix_num_get(int unit, int num, const char *name, const char *suffix, uint32 defl,uint32 *value);
soc_error_t dcmn_property_get_str(int unit, const char *name,char **value);
uint8       dcmn_device_block_for_feature(int unit,dcmn_feature_t feature);






#endif

