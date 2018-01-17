/* $Id: qax_cnt.h
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
*/


#ifndef __QAX_CNT_H__
#define __QAX_CNT_H__

/*************
 * INCLUDES  *
 *************/
#include <soc/dpp/ARAD/arad_api_cnt.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>
#include <soc/dpp/ARAD/arad_cnt.h>



/*************
 * DEFINES   *
 *************/

#define QAX_CNT_MODE_EG_TYPE_TM_FLD_VAL                           (0x2)
#define QAX_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL                       (0x8)
#define QAX_CNT_MODE_EG_TYPE_VSI_FLD_VAL                          (0x4)
#define QAX_CNT_MODE_EG_TYPE_LATENCY_FLD_VAL                      (0x1)
#define QAX_VSI_OFFSET_RESOLUTION                                 (0x800)


/*************
 * MACROS    *
 *************/
#define _BMAP_FIELDS_BLOCK_CMD_ID(BLOCK, CMD_ID)                              \
        BLOCK ## CMD_ID ## _GREEN_ADMITf,                   \
        BLOCK ## CMD_ID ## _GREEN_DISCARDf,                 \
        BLOCK ## CMD_ID ## _YELLOW_1_ADMITf,                \
        BLOCK ## CMD_ID ## _YELLOW_1_DISCARDf,              \
        BLOCK ## CMD_ID ## _YELLOW_2_ADMITf,                \
        BLOCK ## CMD_ID ## _YELLOW_2_DISCARDf,              \
        BLOCK ## CMD_ID ## _RED_ADMITf,                     \
        BLOCK ## CMD_ID ## _RED_DISCARDf                    \

/*************
 * TYPE DEFS *
 *************/






/*************
 * GLOBALS   *
 *************/




/*************
 * FUNCTIONS *
 *************/
/* { */
int qax_cnt_counters_set(SOC_SAND_IN int unit, SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID processor_ndx, SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info);
int qax_cnt_get_field_name_per_src_type(int unit, SOC_TMC_CNT_SRC_TYPE src_type, int command_id, soc_field_t * src_mask_field_name, soc_field_t * src_cfg_field_name, soc_field_t * group_size_field_name);
int qax_cnt_filter_config_ingress_set_get(int unit, int relevant_bit, int command_id, int get, int* value);
uint32
  _qax_cnt_counter_bmap_mem_by_src_type_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                proc_id,
    SOC_SAND_IN  int                   src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
    SOC_SAND_IN  int                   command_id,
    SOC_SAND_OUT soc_reg_t             *reg, 
    SOC_SAND_OUT soc_field_t           *fields);
int qax_cnt_epni_counter_base_set(int unit, int src_core, int base_offset_field_val, int command_id, SOC_TMC_CNT_MODE_EG_TYPE type, SOC_TMC_CNT_SRC_TYPE source);
uint32 qax_cnt_crps_cgm_cmd_get(int unit, int proc_id, int command_id, SOC_SAND_OUT ARAD_CNT_CRPS_IQM_CMD *crps_iqm_cmd);
uint32 qax_cnt_do_not_count_field_by_src_type_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 proc_id,
    SOC_SAND_IN  int                    src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                    command_id,
    SOC_SAND_OUT soc_field_t            *field
    );
int qax_filter_config_egress_receive_set_get(int unit, int relevant_bit, int command_id, int get, int* value);




#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* __QAX_CNT_H__ */
