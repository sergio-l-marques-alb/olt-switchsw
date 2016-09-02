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
 * File: qax_cnt.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_CNT

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/QAX/qax_cnt.h>
#include <soc/dpp/drv.h>



/*********************************************************************
* NAME:
 *   qax_cnt_get_field_name_per_src_type
 * TYPE:
 *   PROC
 * DESCRIPTION: get the fields name for qax for the following registers:
 *          CRPS_CRPS_SRC_MASK, CRPS_CRPS_CNT_SRC_CFG, CRPS_CRPS_CNT_SRC_GROUP_SIZES
 * INPUT:
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
 *********************************************************************/

int qax_cnt_get_field_name_per_src_type(int unit, SOC_TMC_CNT_SRC_TYPE src_type, int command_id, soc_field_t * src_mask_field_name, soc_field_t * src_cfg_field_name, soc_field_t * group_size_field_name)
{

    SOCDNX_INIT_FUNC_DEFS

    switch(src_type)
    {
    case SOC_TMC_CNT_SRC_TYPE_ING_PP:
        * src_mask_field_name = (command_id == 0) ? CRPS_N_IRPP_A_MASKf : CRPS_N_IRPP_B_MASKf;
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_CGMf; /* IRPP use the CGM bubble */
        * group_size_field_name = (command_id == 0) ? IRPP_A_GROUP_SIZEf : IRPP_B_GROUP_SIZEf;
        break;
    case SOC_TMC_CNT_SRC_TYPE_VOQ:
    case SOC_TMC_CNT_SRC_TYPE_STAG:
    case SOC_TMC_CNT_SRC_TYPE_VSQ:
    case SOC_TMC_CNT_SRC_TYPE_CNM_ID:    
        * src_mask_field_name = (command_id == 0) ? CRPS_N_CGM_A_MASKf : CRPS_N_CGM_B_MASKf;
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_CGMf;
        * group_size_field_name = (command_id == 0) ? CGM_A_GROUP_SIZEf : CGM_B_GROUP_SIZEf;                
        break;
    case SOC_TMC_CNT_SRC_TYPE_EGR_PP:     
        * src_mask_field_name = (command_id == 0) ? CRPS_N_EGQ_A_MASKf : CRPS_N_EGQ_B_MASKf;     
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_EGQf;      
        * group_size_field_name = (command_id == 0) ? EGQ_A_GROUP_SIZEf : EGQ_B_GROUP_SIZEf;        
        break;
    case SOC_TMC_CNT_SRC_TYPE_EPNI:
        * src_mask_field_name = (command_id == 0) ? CRPS_N_EPNI_A_MASKf : CRPS_N_EPNI_B_MASKf;   
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_EPNIf;        
        * group_size_field_name = (command_id == 0) ? EPNI_A_GROUP_SIZEf : EPNI_B_GROUP_SIZEf;                
        break;
    case SOC_TMC_CNT_SRC_TYPE_OAM:   
        * src_mask_field_name = (command_id == 0) ? CRPS_N_IHP_A_MASKf : CRPS_N_IHP_B_MASKf;  
        * src_cfg_field_name = CRPS_N_BUBBLE_REQ_IHPf;        
        * group_size_field_name = INVALIDf;                
        break;
    case SOC_TMC_CNT_SRC_TYPES_EGQ_TM:
        * src_mask_field_name = INVALIDf;
        * src_cfg_field_name = INVALIDf;   
        * group_size_field_name = EGQ_TM_GROUP_SIZEf;                
        break;  
    case SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY:
        * src_mask_field_name = INVALIDf;
        * src_cfg_field_name = INVALIDf;        
        * group_size_field_name = INVALIDf;                
        break;                     
    default:
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "src_type=%d is invalid\n"), src_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);        
    }

exit:
    SOCDNX_FUNC_RETURN  
}

/*********************************************************************
* NAME:
 *   qax_cnt_counters_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the Counter Processor: its counting mode and
 *   its counting source.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx -
 *     Counter processor index
 *   SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO        *info -
 *     Counter info of the counter processor
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
 *********************************************************************/
int qax_cnt_counters_set(SOC_SAND_IN  int unit, SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID processor_ndx, SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info)
{
    int command_id;
    uint32 proc_id, group_size_val=0, nof_counters = 0, one_entry_mode_cnt = 0;
    uint32 res = SOC_SAND_OK;
    ARAD_CNT_CRPS_IQM_CMD crps_iqm_cmd;
    soc_field_t group_size_field = INVALIDf;
    /*uint32 format_field_val; */
    soc_field_t src_field_name = INVALIDf;
    soc_field_t src_bubble_field_name;

    SOCDNX_INIT_FUNC_DEFS

    sal_memset(&crps_iqm_cmd, 0, sizeof(ARAD_CNT_CRPS_IQM_CMD));
    
    res = arad_cnt_get_processor_id(unit, processor_ndx, &proc_id);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    command_id = info->command_id;

    /* get the fields name for the registers that are going to be set */
    SOCDNX_IF_ERR_EXIT( qax_cnt_get_field_name_per_src_type(unit, info->src_type, info->command_id, &src_field_name, &src_bubble_field_name, &group_size_field));
    SOCDNX_SAND_IF_ERR_EXIT(arad_cnt_base_val_set(unit, proc_id, info));
    if(src_field_name != INVALIDf)
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, CRPS_CRPS_SRC_MASKr, REG_PORT_ANY, proc_id, src_field_name, TRUE));    
    }
    if(src_bubble_field_name != INVALIDf)
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_CFGr, REG_PORT_ANY, proc_id, src_bubble_field_name, TRUE));    
    }
        
    /*EPNI - T.B.D */
    /*SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR, arad_cnt_base_val_set(unit, proc_id, info)); */

    res = arad_cnt_ingress_params_get(unit, info, &group_size_val, &nof_counters, &one_entry_mode_cnt);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (group_size_field != INVALIDf) 
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_GROUP_SIZESr, REG_PORT_ANY, 0, group_size_field, group_size_val));
    }

    if ((info->src_type == ARAD_CNT_SRC_TYPE_VOQ) || (info->src_type == ARAD_CNT_SRC_TYPE_STAG) || 
      (info->src_type == ARAD_CNT_SRC_TYPE_VSQ) || (info->src_type == ARAD_CNT_SRC_TYPE_CNM_ID))
    {
      res = arad_cnt_crps_iqm_cmd_get(unit, proc_id, command_id, &crps_iqm_cmd);
      SOCDNX_SAND_IF_ERR_EXIT(res);
      SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, crps_iqm_cmd.src_cfg_r, REG_PORT_ANY, 0, crps_iqm_cmd.src_type_f,  info->src_type - ARAD_CNT_SRC_TYPE_VOQ));
    }

exit:
    SOCDNX_FUNC_RETURN
}



/*********************************************************************************
 * Function:
 *      qax_cnt_filter_config_ingress_set_get
 * Purpose:
 *      set and get filters criteria configurations in ingress,
 *      eccentialy decides which filters criteria the CRPS will filter in or out
 * Parameters:
 *      unit -  unit number
 *      relevant_bit - the bit to set/reset in the register
 *      command_id - the CRPS support few types of command, 0,1, and special cases which has a uniqe command id, here only 0 and 1 are supported.
 *      get - decides if get or set
 *      value - value to set, or returned value from get
 * Returns:
 *      SOC_E_XXX
**********************************************************************************/
int qax_cnt_filter_config_ingress_set_get(int unit, int relevant_bit, int command_id, int get, int* value)
{
    soc_reg_above_64_val_t reg_above_64;
    uint32 mask;
    uint32 field32_val;

    SOCDNX_INIT_FUNC_DEFS;

    /* create a mask for relevant bit */
    mask = 1U << relevant_bit;

    SOCDNX_IF_ERR_EXIT(READ_CGM_TM_CTR_PTR_SETTINGSr(unit, reg_above_64));    
    /* handle each command ID */
    if (command_id == 0) {
        field32_val = soc_reg_above_64_field32_get(unit, CGM_TM_CTR_PTR_SETTINGSr, reg_above_64, TM_CTR_A_RJCT_FILTER_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_above_64_field32_set(unit, CGM_TM_CTR_PTR_SETTINGSr, reg_above_64, TM_CTR_A_RJCT_FILTER_MASKf, field32_val);
        }
    } else if (command_id == 1) {
        field32_val = soc_reg_above_64_field32_get(unit, CGM_TM_CTR_PTR_SETTINGSr, reg_above_64, TM_CTR_B_RJCT_FILTER_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_above_64_field32_set(unit, CGM_TM_CTR_PTR_SETTINGSr, reg_above_64, TM_CTR_B_RJCT_FILTER_MASKf, field32_val);
        }        
    } else {
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "ingress counter doesn't support command id %d\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_TM_CTR_PTR_SETTINGSr(unit, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}




uint32
  _qax_cnt_counter_bmap_mem_by_src_type_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                proc_id,
    SOC_SAND_IN  int                   src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
    SOC_SAND_IN  int                   command_id,
    SOC_SAND_OUT soc_reg_t             *reg, 
    SOC_SAND_OUT soc_field_t           *fields)
{
    soc_reg_t
        iqm_counter_bmap_reg[] =  {CRPS_CGM_OFFSET_BMAP_Ar, CRPS_CGM_OFFSET_BMAP_Br},
        egq_counter_bmap_reg[] =  {CRPS_EGQ_OFFSET_BMAP_Ar, CRPS_EGQ_OFFSET_BMAP_Br},
        irpp_counter_bmap_reg[] = {CRPS_IRPP_OFFSET_BMAP_Ar, CRPS_IRPP_OFFSET_BMAP_Br},
        epni_counter_bmap_reg[] = {CRPS_EPNI_OFFSET_BMAP_Ar, CRPS_EPNI_OFFSET_BMAP_Br},
        egq_tm_counter_bmap_reg[] =  {CRPS_EGQ_TM_OFFSET_BMAPr},
        counter_bmap_reg = INVALIDf;
   int index, field_offset = 0;
    soc_field_t
       iqm_counter_bmap_fields[] =  {_BMAP_FIELDS_BLOCK_CMD_ID(CGM, A), _BMAP_FIELDS_BLOCK_CMD_ID(CGM, B)},
       egq_counter_bmap_fields[] =  {_BMAP_FIELDS_BLOCK_CMD_ID(EGQ, A), _BMAP_FIELDS_BLOCK_CMD_ID(EGQ, B)},
       irpp_counter_bmap_fields[] = {_BMAP_FIELDS_BLOCK_CMD_ID(IRPP, A), _BMAP_FIELDS_BLOCK_CMD_ID(IRPP, B)},
       epni_counter_bmap_fields[] = {_BMAP_FIELDS_BLOCK_CMD_ID(EPNI, A), _BMAP_FIELDS_BLOCK_CMD_ID(EPNI, B)},
       egq_tm_counter_bmap_fields[] = {_BMAP_FIELDS_BLOCK_CMD_ID(EGQ, _TM)},
       *counter_bmap_fields = NULL;
   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   switch (src_type) {
    case ARAD_CNT_SRC_TYPE_ING_PP:
        counter_bmap_reg = irpp_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = irpp_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
        break;      
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
        counter_bmap_reg = iqm_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = iqm_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
        break;  
    case ARAD_CNT_SRC_TYPE_EGR_PP:
        counter_bmap_reg = egq_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = egq_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
        break;
    case ARAD_CNT_SRC_TYPE_OAM:
    /* Always 1 count for OAM */
      break;
    case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
    /* Always 1 count for IPT latency*/
       break;
    case ARAD_CNT_SRC_TYPE_EPNI:
        counter_bmap_reg = epni_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = epni_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
        break;
   case ARAD_CNT_SRC_TYPES_EGQ_TM:
       counter_bmap_reg = egq_tm_counter_bmap_reg[src_core];
       counter_bmap_fields = egq_tm_counter_bmap_fields;
       field_offset = src_core * SOC_TMC_CNT_BMAP_OFFSET_COUNT;
       break;
   default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  *reg = counter_bmap_reg;
  if (counter_bmap_reg != INVALIDf && counter_bmap_fields) {
       for (index = 0; index  < SOC_TMC_CNT_BMAP_OFFSET_COUNT; index++) {
           fields[index] = counter_bmap_fields[field_offset + index];
       }
   }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _qax_cnt_counter_bmap_mem_by_src_type_get()", 0, 0);
}





/**************************************************************
 * Function:
 *      qax_cnt_epni_counter_base_set
 * Purpose:
 *      set the counter base for epni
 * Parameters:
 * int src_core
 * int base_offset_field_val
 * int command_id
 * SOC_TMC_CNT_MODE_EG_TYPE type
 * SOC_TMC_CNT_SRC_TYPE source
 *
 * Returns:
 *      SOC_E_xxx
 **************************************************************/

int qax_cnt_epni_counter_base_set(int unit, int src_core, int base_offset_field_val, int command_id, SOC_TMC_CNT_MODE_EG_TYPE type, SOC_TMC_CNT_SRC_TYPE source)
{
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS

    switch (type) 
    {
    case SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT:
    case SOC_TMC_CNT_MODE_EG_TYPE_TM:
        rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTERS_BASEr, src_core, 0, PORT_COUNTER_BASE_POINTERf, 0); /*ignore base for TM commands*/
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case ARAD_CNT_MODE_EG_TYPE_VSI:
        if((base_offset_field_val % QAX_VSI_OFFSET_RESOLUTION) != 0) /* offset must be 2K resolution */
        {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "base_offset_field_val=%d, value must be in 2K resolution for EGRESS_VSI.\n"), base_offset_field_val));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);            
        }
        else
        {
            base_offset_field_val = base_offset_field_val/QAX_VSI_OFFSET_RESOLUTION;
            rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTERS_BASEr, src_core, 0, VSI_COUNTER_BASE_POINTERf, base_offset_field_val);
            SOCDNX_IF_ERR_EXIT(rv);            
        }
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid mode %s for source %s.\n"), SOC_TMC_CNT_MODE_EG_TYPE_to_string(type), SOC_TMC_CNT_SRC_TYPE_to_string(source)));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    
exit:
    SOCDNX_FUNC_RETURN;        
}


/**************************************************************
 * Function:
 *      qax_cnt_crps_cgm_cmd_get
 * Purpose:
 *      get the cgmregister and fields name for  counting
 * Parameters:
 * int unit
 * int proc_id
 * int command_id
 * SOC_SAND_OUT ARAD_CNT_CRPS_IQM_CMD *crps_iqm_cmd
 *
 * Returns:
 *      SOC_E_xxx
 **************************************************************/

uint32 qax_cnt_crps_cgm_cmd_get(int unit, int proc_id, int command_id, SOC_SAND_OUT ARAD_CNT_CRPS_IQM_CMD *crps_iqm_cmd)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(crps_iqm_cmd);

    if (command_id == 0) {
        crps_iqm_cmd->cfg_1_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->cfg_2_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->src_cfg_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->src_type_f = TM_CTR_A_PTR_SELf;
        crps_iqm_cmd->queue_shift_f = TM_CTR_A_QNUM_SHIFTf;
        crps_iqm_cmd->base_q_f = TM_CTR_A_QNUM_LOWf;
        crps_iqm_cmd->top_q_f = TM_CTR_A_QNUM_HIGHf;
    } else if (command_id == 1) {
        crps_iqm_cmd->cfg_1_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->cfg_2_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->src_cfg_r = CGM_TM_CTR_PTR_SETTINGSr;
        crps_iqm_cmd->src_type_f = TM_CTR_B_PTR_SELf;
        crps_iqm_cmd->queue_shift_f = TM_CTR_B_QNUM_SHIFTf;
        crps_iqm_cmd->base_q_f = TM_CTR_B_QNUM_LOWf;
        crps_iqm_cmd->top_q_f = TM_CTR_B_QNUM_HIGHf;
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid command ID %d\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }        
exit:
  SOCDNX_FUNC_RETURN;
}

/**************************************************************
 * Function:
 *      qax_cnt_do_not_count_field_by_src_type_get
 * Purpose:
 *     get register and field name
 * Parameters:
 *  SOC_SAND_IN  int                    unit,
 *  SOC_SAND_IN  uint32                 proc_id,
 *  SOC_SAND_IN  int                    src_core,
 *  SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
 *  SOC_SAND_IN  int                    command_id,
 *  SOC_SAND_OUT soc_field_t            *field
 *
 *
 * Returns:
 *      SOC_E_xxx
 **************************************************************/
uint32 qax_cnt_do_not_count_field_by_src_type_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 proc_id,
    SOC_SAND_IN  int                    src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                    command_id,
    SOC_SAND_OUT soc_field_t            *field
    )
{
  soc_field_t       
      qax_cgm_do_not_count_entryf[] =      {CGM_A_DO_NOT_COUNT_OFFSETf,  CGM_B_DO_NOT_COUNT_OFFSETf},
      qax_egq_do_not_count_entryf[] =      {EGQ_A_DO_NOT_COUNT_OFFSETf,  EGQ_B_DO_NOT_COUNT_OFFSETf},
      qax_irpp_do_not_count_entryf[] =     {IRPP_A_DO_NOT_COUNT_OFFSETf, IRPP_B_DO_NOT_COUNT_OFFSETf},
      qax_epni_do_not_count_entryf[] =     {EPNI_A_DO_NOT_COUNT_OFFSETf, EPNI_B_DO_NOT_COUNT_OFFSETf},
      qax_egq_tm_do_not_count_entryf[] =   {EGQ_TM_DO_NOT_COUNT_OFFSETf},      
      do_not_count_fld = INVALIDf;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    switch (src_type) {
    case ARAD_CNT_SRC_TYPE_ING_PP:
      do_not_count_fld = qax_irpp_do_not_count_entryf[command_id];
      break;      
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
      do_not_count_fld = qax_cgm_do_not_count_entryf[command_id];
      break;  
    case ARAD_CNT_SRC_TYPE_EGR_PP:
      do_not_count_fld = qax_egq_do_not_count_entryf[command_id];
      break;
    case ARAD_CNT_SRC_TYPE_OAM:
    /* Always 1 count for OAM */
      break;
    case ARAD_CNT_SRC_TYPE_EPNI:
      do_not_count_fld = qax_epni_do_not_count_entryf[command_id];
      break;
    case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
      /* Always 1 count for IPT latency*/
      break;
    case ARAD_CNT_SRC_TYPES_EGQ_TM:
      do_not_count_fld = qax_egq_tm_do_not_count_entryf[0];
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
    } 
  *field = do_not_count_fld;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in qax_cnt_do_not_count_field_by_src_type_get()", 0, 0);
}



/***********************************************************************************************************************
 * Function:
 *      soc_jer_filter_config_egress_receive_set_get
 * Purpose:
 *      set and get filters criteria configurations in egress receive,
 *      eccentialy decides which filters criteria the CRPS will filter in and out
 * Parameters:
 *      unit -  unit number
 *      relevant_bit - the bit to set/reset in the register
 *      command_id - the CRPS support few types of command: 0,1, and special cases which has a uniqe command id. In the egress receive 0,1 and BCM_STAT_COUNTER_TM_COMMAND command IDs are supported.
 *      get - decides if get or set value
 *      value - value to set or returned value from get
 * Returns:
 *      SOC_E_XXX
 ***********************************************************************************************************************/
int qax_filter_config_egress_receive_set_get(int unit, int relevant_bit, int command_id, int get, int* value)
{
    uint32 reg32_val;
    uint32 mask;
    uint32 field32_val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(value);

    /* create a mask for relevant bit */
    mask = 1U << relevant_bit;

    /* handle each command ID */
    if (command_id == BCM_STAT_COUNTER_TM_COMMAND) {
        SOCDNX_IF_ERR_EXIT(READ_ECGM_CRPS_TM_PQP_DROP_REASONS_MASKr(unit, &reg32_val));
        field32_val = soc_reg_field_get(unit, ECGM_CRPS_TM_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_TM_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, ECGM_CRPS_TM_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_TM_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CRPS_TM_PQP_DROP_REASONS_MASKr(unit, reg32_val));
        }        
    } else if (command_id == 0) {
        SOCDNX_IF_ERR_EXIT(READ_ECGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr(unit, &reg32_val));
        field32_val = soc_reg_field_get(unit, ECGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_PP_1_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, ECGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_PP_1_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr(unit, reg32_val));
        }
    } else if (command_id == 1) {
        SOCDNX_IF_ERR_EXIT(READ_ECGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr(unit, &reg32_val));
        field32_val = soc_reg_field_get(unit, ECGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_PP_2_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, ECGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_PP_2_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr(unit, reg32_val));
        }        
    } else {
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "egress receive counter doesn't support command id %d\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


