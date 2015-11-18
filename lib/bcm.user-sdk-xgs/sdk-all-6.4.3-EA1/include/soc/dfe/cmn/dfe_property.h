/*
 * $Id: dfe_property.h,v 1.3 Broadcom SDK $
 *
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
 *
 * DFE PROPERTY H
 */
 
#ifndef _SOC_DFE_PROPERTY_H_
#define _SOC_DFE_PROPERTY_H_

/**********************************************************/
/*                  Includes                              */
/**********************************************************/
#include <soc/error.h>
#include <soc/drv.h>

/**********************************************************/
/*                  Structures                            */
/**********************************************************/

typedef struct soc_dfe_property_info_s {
    char* str;
    char* def_str;
    int def_int;
    int def_type; /* 1 for string, 2 for int */
} soc_dfe_property_info_t ;

typedef struct soc_dfe_property_str_enum_s {
    int enum_val;
    char *str;
} soc_dfe_property_str_enum_t ;

/**********************************************************/
/*                  Defines                               */
/**********************************************************/

#define SOC_DFE_PROPERTY_DEFAULT_TYPE_STRING 1
#define SOC_DFE_PROPERTY_DEFAULT_TYPE_INT 2
#define SOC_DFE_PROPERTY_UNAVAIL -1

/**********************************************************/
/*                  Functions                             */
/**********************************************************/

int soc_dfe_property_suffix_num_get(int unit,int num, char* soc_property_name,const char* suffix, int force_use_default, int soc_prop_default);
int soc_dfe_property_suffix_num_get_only_suffix(int unit, int num, char* soc_propert_name, const char* suffix, int force_use_default, int soc_prop_default);
int soc_dfe_property_get(int unit,char* soc_property_name,int force_use_default, int soc_prop_default);
char* soc_dfe_property_get_str(int unit,char* soc_property_name,int force_use_default, char* soc_prop_default);
char* soc_dfe_property_port_get_str(int unit, char* soc_property_name, soc_port_t port, int force_use_default, char* soc_prop_default);
int soc_dfe_property_port_get(int unit, char* soc_property_name, soc_port_t port, int force_use_default, int soc_prop_default);

void soc_dfe_check_soc_property(int unit,char* soc_property_name,int* is_supported,soc_dfe_property_info_t* soc_property_info);

/* 
 *Property string values to enum
 *Both function assumes that the last entry is NULL.
 */
soc_error_t soc_dfe_property_str_to_enum(int unit, char *soc_property_name, const soc_dfe_property_str_enum_t *property_info, char *str_val, int *int_val);
soc_error_t soc_dfe_property_enum_to_str(int unit, char *soc_property_name, const soc_dfe_property_str_enum_t *property_info, int int_val, char **str_val);

/**********************************************************/
/*                  Constant                              */
/**********************************************************/
/*Soc property: fabric_device_mode - available values*/
extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_device_mode[];
/*Soc property: fabric_multicast_mode - available values*/
extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_multicast_mode[];
/*Soc property: fabric_load_balancing_mode - available values*/
extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_load_balancing_mode[];
/*Soc property: fabric_cell_format - available values*/
extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_cell_format[];
/*Soc property: fabric_tdm_priority_min - available values*/
extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_tdm_priority_min[];
/*Soc property: fe_mc_id_range - available values*/
extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fe_mc_id_range[];
/*Soc property: backplane_serdes_encoding - available values*/
extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_backplane_serdes_encoding[];

#endif /* SOC_DFE_PROPERTY_H */
