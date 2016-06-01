/*
 * $Id: $
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
 * File: qax_ingress_packet_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/QAX/qax_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>

int
  qax_iqm_dynamic_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          entry_offset,
    SOC_SAND_OUT  ARAD_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  )
{
    soc_reg_above_64_val_t data_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_SIZEm(unit, CGM_BLOCK(unit, core), entry_offset, &data_above_64));

    IQM_dynamic_tbl_data->pq_inst_que_size = soc_CGM_VOQ_SIZEm_field32_get(unit, &data_above_64, WORDS_SIZEf);
    IQM_dynamic_tbl_data->pq_avrg_szie = soc_CGM_VOQ_SIZEm_field32_get(unit, &data_above_64, AVRG_SIZEf);

exit:
    SOCDNX_FUNC_RETURN;
}

int
qax_ipq_explicit_mapping_mode_info_get(
   SOC_SAND_IN  int                            unit,
   SOC_SAND_OUT ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
   ) 
{

    uint32 fld_val;

    SOCDNX_INIT_FUNC_DEFS;

    arad_ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(info);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IRR_FLOW_BASE_QUEUEr, REG_PORT_ANY,  0, FLOW_BASE_QUEUE_0f, &fld_val));

    info->base_queue_id = fld_val;
    info->queue_id_add_not_decrement = TRUE;

exit:
    SOCDNX_FUNC_RETURN;
}

int
qax_ipq_explicit_mapping_mode_info_set(
   SOC_SAND_IN  int                            unit,
   SOC_SAND_IN ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
   ) 
{

    uint32 fld_val, res;
    int profile_id;

    SOCDNX_INIT_FUNC_DEFS;
    
    res = arad_ipq_explicit_mapping_mode_info_verify(unit, info);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    
    
   /*
    * All the Base_Q Flow is implemented trough the TAR table:
    * See Arch-PP-Spec figure 15: per flow, a profile is got to set its TC-mapping and its Base-Flow
    * No reason for a Base-Flow per Flow-Id, so a global one is set
    */
    fld_val = info->base_queue_id;
    for (profile_id = 0; profile_id < ARAD_NOF_INGRESS_FLOW_TC_MAPPING_PROFILES; ++profile_id) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, TAR_FLOW_BASE_QUEUEr, REG_PORT_ANY,  0, FLOW_BASE_QUEUE_0f + profile_id,  fld_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
qax_ipq_default_invalid_queue_set(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            core,
   SOC_SAND_IN  uint32         queue_id,
   SOC_SAND_IN  int            enable) 
{
    SOCDNX_INIT_FUNC_DEFS;

    if (enable) {
        SOCDNX_IF_ERR_EXIT(WRITE_TAR_INVALID_DESTINATION_QUEUEr(unit, queue_id));
    } else { /* mark as invalid */
        SOCDNX_IF_ERR_EXIT(WRITE_TAR_INVALID_DESTINATION_QUEUEr(unit, ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit)));
    }
            
exit:
    SOCDNX_FUNC_RETURN;
}

int
qax_ipq_default_invalid_queue_get(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            core,
   SOC_SAND_OUT uint32         *queue_id,
   SOC_SAND_OUT int            *enable) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_TAR_INVALID_DESTINATION_QUEUEr(unit, queue_id));
    
    if (*queue_id != ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit)) {
        *enable = 1;
    } else {
        *enable = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

