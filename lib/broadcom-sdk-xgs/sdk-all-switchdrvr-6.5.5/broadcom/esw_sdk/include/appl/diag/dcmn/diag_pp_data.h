/*
 * $Id: diag_pp_data.h,v 1.00 Broadcom SDK $
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
 * File:    diag_pp_data.h
 * Purpose:    Types and structures used import/export PP data
 */

#ifndef __DIAG_PP_DATA_H
#define __DIAG_PP_DATA_H

#include <appl/diag/dcmn/dpp_debug_signals.h>
#include <appl/diag/diag_pp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <shared/utilex/utilex_rhlist.h>

typedef struct {
    char        name[RHNAME_MAX_SIZE];
    int         offset;
    int         size;
    int         header; /* header type for packet or LSB/MSB for signal */
    int         valid;
    uint16      ce;
    int         ce_type;
    char        ce_name[RHNAME_MAX_SIZE];
} qualifier_t;

#define PARSER_PROGRAM_SEGMENT_SIZE 0x40
#define PARSER_PROGRAM_FIN          0x00 /* Execute instruction's command and finish */
#define PARSER_PROGRAM_END          0x7f /* End and do not execute the instruction's command */

#define PP_INVLAID_VALUE            -1

#define CE16_TYPE                   16
#define CE32_TYPE                   32

#define INTERNAL_BUFFER_CE16_SHIFT  16
#define INTERNAL_BUFFER_CE32_SHIFT  32
#define INTERNAL_BUFFER_QUALIFIER   0x8

#define VTT_QUALIFIER_NUM           12
#define FLP_QUALIFIER_NUM           32

#define BOOLSTR(mc_value)         ((mc_value == 0) ? "No" : "Yes")

typedef enum {
    SIG_COMMAND_DEBUG,
    SIG_COMMAND_INTERNAL,
    SIG_COMMAND_STAGE,
    SIG_COMMAND_PARAM,
    SIG_COMMAND_STRUCT,
    SIG_COMMAND_DEBUG_SINGLE
} SIG_COMMAND;

/*
 * Defines
 */
#define DPP_EXPORT_MAX_STAGE_NUM        4
#define DPP_EXPORT_PP_PRESEL_MAX        48
#define DPP_EXPORT_PP_BUNDLE_MAX        DPP_EXPORT_PP_TABLE_MAX
#define DPP_EXPORT_PP_TABLE_MAX         SOC_DPP_DBAL_SW_NOF_TABLES
#define DPP_EXPORT_PP_BUNDLE_TABLE_MAX  32

#define DPP_EXPORT_CHAR_MAX_LENGTH 50

/*
 * Structs
 */
typedef struct dpp_export_pp_table_key_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
} dpp_export_pp_table_key_t;

typedef struct dpp_export_pp_table_action_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
} dpp_export_pp_table_action_t;

typedef struct dpp_export_pp_table_info_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
    char                         database[DPP_EXPORT_CHAR_MAX_LENGTH];
    dpp_export_pp_table_key_t    keys[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    dpp_export_pp_table_action_t actions[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    int                          priority;
    int                          is_static;
} dpp_export_pp_table_info_t;

typedef struct dpp_export_pp_bundle_table_s {
    dpp_export_pp_table_info_t   *table;
    int                          lookup;
    int                          is_static;
} dpp_export_pp_bundle_table_t;

typedef struct dpp_export_pp_bundle_info_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
    dpp_export_pp_bundle_table_t tables[DPP_EXPORT_PP_BUNDLE_TABLE_MAX];
    int                          nof_tables;
    int                          is_static;
    int                          selected;
} dpp_export_pp_bundle_info_t;

typedef struct dpp_presel_qual_info_s {
    rhentry_t   entry;
    uint32      data;
    uint32      mask;
    int         size;
} dpp_presel_qual_info_t;

typedef struct dpp_export_pp_presel_info_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
    char                         app_type[DPP_EXPORT_CHAR_MAX_LENGTH];
    dpp_export_pp_bundle_info_t  *bundle;
    int                          is_static;
    int                          selected;
    rhlist_t                     *qual_list;
} dpp_export_pp_presel_info_t;

typedef struct dpp_export_pp_stage_info_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
    dpp_export_pp_presel_info_t  presels[DPP_EXPORT_PP_PRESEL_MAX];
    dpp_export_pp_bundle_info_t  bundles[DPP_EXPORT_PP_BUNDLE_MAX];
    dpp_export_pp_table_info_t   tables[DPP_EXPORT_PP_TABLE_MAX];
} dpp_export_pp_stage_info_t;

typedef struct dpp_export_pp_info_s {
    dpp_export_pp_stage_info_t   stages[SOC_PPC_NOF_FP_DATABASE_STAGES];
} dpp_export_pp_info_t;

cmd_result_t    cmd_diag_pp_signals_get(int unit, args_t* a);
cmd_result_t    dpp_export_pp(int unit, char *stage, char *filename);
device_t       *appl_dpp_diag_sig_init(int unit);

int dpp_dump_vt_programs(int unit,  int core, dpp_export_pp_stage_info_t *stage);
int dpp_dump_tt_programs(int unit,  int core, dpp_export_pp_stage_info_t *stage);
int dpp_dump_flp_programs(int unit, int core, dpp_export_pp_stage_info_t *stage);

int dpp_dump_vt(int unit,  int core, dpp_export_pp_stage_info_t *stage);
int dpp_dump_tt(int unit,  int core, dpp_export_pp_stage_info_t *stage);
int dpp_dump_flp(int unit, int core, dpp_export_pp_stage_info_t *stage);

#endif /* __DIAG_PP_DATA_H */
