/*
 * $Id: fe1600_fabric_multicast.c,v 1.9 Broadcom SDK $
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
 * SOC FE1600 FABRIC MULTICAST
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mem.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <shared/bitop.h>

#if defined(BCM_88750_A0)

#include <soc/dfe/fe1600/fe1600_fabric_multicast.h>

/*
 * Function:
 *      soc_fe1600_fabric_multicast_low_prio_drop_select_priority_set
 * Purpose:
 *      Select the highest priority to be considered as low
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      prio      - (IN)  Highest priority to be considered as low
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_low_prio_drop_select_priority_set(int unit, soc_dfe_fabric_priority_t prio)
{
    uint32 reg_val, low_prio_select;
    SOCDNX_INIT_FUNC_DEFS;
    
    /*Low priority multicast priority mapping.
      bit 4 - mapping of priority 00 - 1 for low priority, 0 for normal priority.
      bit 5 - mapping of priority 01 - 1 for low priority, 0 for normal priority.
      bit 6 - mapping of priority 10 - 1 for low priority, 0 for normal priority.
      bit 7 - mapping of priority 11 - 1 for low priority, 0 for normal priority*/
    switch(prio)
    {
        case soc_dfe_fabric_priority_0:
            low_prio_select = 0x1;
            break;
        case soc_dfe_fabric_priority_1:
            low_prio_select = 0x3;
            break;
        case soc_dfe_fabric_priority_2:
            low_prio_select = 0x7;
            break;
        case soc_dfe_fabric_priority_3:
            low_prio_select = 0xf;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong priority %d"),prio));
    }

    /*Set MC low priority at RTP*/
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MUL_CTRLr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_LOW_PR_MUL_CTRLr, &reg_val, LOW_PR_MUL_PRIOSELf, low_prio_select);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_LOW_PR_MUL_CTRLr(unit, reg_val));

    /*Set MC low priority at DCH*/
#ifdef BCM_88950
    if (SOC_IS_FE3200(unit))
    {
        int blk, nof_dch;

        nof_dch =  SOC_DFE_DEFS_GET(unit, nof_instances_dch);
        for (blk = 0; blk < nof_dch; blk++)
        {
                SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_2r(unit, blk, &reg_val));
                soc_reg_field_set(unit, DCH_DCH_ENABLERS_REGISTER_2r, &reg_val, LOW_PR_DROP_ENf, low_prio_select);
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_2r(unit, blk, reg_val));
        }
    }
#endif /*BCM_88950*/

      
exit:
    SOCDNX_FUNC_RETURN;
  
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_low_prio_drop_select_priority_get
 * Purpose:
 *      Get the highest priority to be considered as low
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      prio      - (OUT) Highest priority to be considered as low
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_low_prio_drop_select_priority_get(int unit, soc_dfe_fabric_priority_t* prio)
{
    uint32 reg_val, low_prio_select;
    SOCDNX_INIT_FUNC_DEFS; 
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MUL_CTRLr(unit, &reg_val));
      
    low_prio_select = soc_reg_field_get(unit, RTP_LOW_PR_MUL_CTRLr, reg_val, LOW_PR_MUL_PRIOSELf);
    
    switch(low_prio_select)
    {
        case 0x1:
            *prio = soc_dfe_fabric_priority_0;
            break;
        case 0x3:
            *prio = soc_dfe_fabric_priority_1;
            break;
        case 0x7:
            *prio = soc_dfe_fabric_priority_2;
            break; 
        case 0xf:
            *prio = soc_dfe_fabric_priority_3;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Unrecognized priority %d"),low_prio_select));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
  
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_low_prio_threshold_validate
 * Purpose:
 *      Validate thresholds for low priority multicast drop
 * Parameters:
 *      unit      - (IN) Unit number.
 *      type      - (IN) bcmFabricMcLowPrioDropThUp or bcmFabricMcLowPrioDropThDown
 *      arg       - (IN) Threshold
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_low_prio_threshold_validate(int unit, bcm_fabric_control_t type, int arg)
{
    uint64  val64;
    SOCDNX_INIT_FUNC_DEFS; 
      
    COMPILER_64_SET(val64, 0, arg);
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            if(SOC_FAILURE(soc_reg64_field_validate(unit, RTP_LOW_PR_MULTHr, LOW_PR_MUL_DROP_TH_HIGHf, val64))) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Argument is too big %d"),arg));
            }
            break;
        case bcmFabricMcLowPrioDropThDown:
            if(SOC_FAILURE(soc_reg64_field_validate(unit, RTP_LOW_PR_MULTHr, LOW_PR_MUL_DROP_TH_LOWf, val64))) {
                 SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Argument is too big %d"),arg));
            }
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
     
exit:
    SOCDNX_FUNC_RETURN;
  
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_low_prio_threshold_set
 * Purpose:
 *      Set thresholds for low priority multicast drop
 * Parameters:
 *      unit      - (IN) Unit number.
 *      type      - (IN) bcmFabricMcLowPrioDropThUp or bcmFabricMcLowPrioDropThDown
 *      arg       - (IN) Threshold
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_low_prio_threshold_set(int unit, bcm_fabric_control_t type, int arg)
{
    uint64 reg_val, val64;
    SOCDNX_INIT_FUNC_DEFS; 
	
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MULTHr(unit, &reg_val));
    COMPILER_64_SET(val64, 0, arg);
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            soc_reg64_field_set(unit, RTP_LOW_PR_MULTHr, &reg_val, LOW_PR_MUL_DROP_TH_HIGHf, val64);
            break;
        case bcmFabricMcLowPrioDropThDown:
            soc_reg64_field_set(unit, RTP_LOW_PR_MULTHr, &reg_val, LOW_PR_MUL_DROP_TH_LOWf, val64);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
    
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_LOW_PR_MULTHr(unit, reg_val));
      
exit:
    SOCDNX_FUNC_RETURN;
  
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_low_prio_threshold_get
 * Purpose:
 *      Get thresholds for low priority multicast drop
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      type      - (IN)  bcmFabricMcLowPrioDropThUp or bcmFabricMcLowPrioDropThDown
 *      arg       - (OUT) Threshold
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_low_prio_threshold_get(int unit, bcm_fabric_control_t type, int* arg)
{
    uint64 reg_val;
    SOCDNX_INIT_FUNC_DEFS; 
      
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MULTHr(unit, &reg_val));
      
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            *arg = soc_reg_field_get(unit, RTP_LOW_PR_MULTHr, COMPILER_64_LO(reg_val), LOW_PR_MUL_DROP_TH_HIGHf);
            break;
        case bcmFabricMcLowPrioDropThDown:
            *arg = soc_reg_field_get(unit, RTP_LOW_PR_MULTHr, COMPILER_64_LO(reg_val), LOW_PR_MUL_DROP_TH_LOWf);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_low_priority_drop_enable_set
 * Purpose:
 *      Enable / Disable multicast low priority drop
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      arg      - (IN)  Enable (1) or Disable (0) multicast law priority drop
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_low_priority_drop_enable_set(int unit, int arg)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS; 
     
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MUL_CTRLr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_LOW_PR_MUL_CTRLr, &reg_val, LOW_PR_MUL_ENf, (arg ? 1 : 0));
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_LOW_PR_MUL_CTRLr(unit, reg_val));
    
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_low_priority_drop_enable_get
 * Purpose:
 *      Get multicast low priority drop state (Enabled / Disabled)
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      arg      - (OUT) Enable (1) or Disable (0) multicast law priority drop
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_low_priority_drop_enable_get(int unit, int* arg)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS; 
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MUL_CTRLr(unit, &reg_val));
    *arg = soc_reg_field_get(unit, RTP_LOW_PR_MUL_CTRLr, reg_val, LOW_PR_MUL_ENf);
  
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_priority_range_validate
 * Purpose:
 *      Validate multicast priority ranges
 * Parameters:
 *      unit    - (IN) Unit number.
 *      type    - (IN) Range to set (Low / Mid x Min / Max)
  *     arg     - (IN) Multicast group number
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_priority_range_validate(int unit, bcm_fabric_control_t type, int arg)
{
    uint32 max_id ;
    SOCDNX_INIT_FUNC_DEFS; 
    
    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &max_id)));

     if(arg < 0 || arg >= max_id) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("arg %d is invalid for type %d, outside of multicast id range"),arg, type));
     }
    
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_priority_range_set
 * Purpose:
 *      Set multicast priority ranges
 * Parameters:
 *      unit    - (IN) Unit number.
 *      type    - (IN) Range to set (Low / Mid x Min / Max)
  *     arg     - (IN) Multicast group number
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_priority_range_set(int unit, bcm_fabric_control_t type, int arg)
{
    uint32 reg_val = 0;
    SOCDNX_INIT_FUNC_DEFS; 
    
    switch(type)
    {    
        case bcmFabricMcLowPrioMin:
            soc_reg_field_set(unit, DCH_LOW_PR_MUL_0r, &reg_val, LOW_PR_MUL_QUE_LOWf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_0r(unit, 0, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_0r(unit, 1, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_0r(unit, 2, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_0r(unit, 3, reg_val));
            break;
        case bcmFabricMcLowPrioMax:
            soc_reg_field_set(unit, DCH_LOW_PR_MUL_1r, &reg_val, LOW_PR_MUL_QUE_HIGHf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_1r(unit, 0, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_1r(unit, 1, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_1r(unit, 2, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_1r(unit, 3, reg_val));
            break;
        case bcmFabricMcMidPrioMin:
            soc_reg_field_set(unit, DCH_MID_PR_MUL_0r, &reg_val, MID_PR_MUL_QUE_LOWf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_0r(unit, 0, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_0r(unit, 1, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_0r(unit, 2, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_0r(unit, 3, reg_val));
            break;
        case bcmFabricMcMidPrioMax:
            soc_reg_field_set(unit, DCH_MID_PR_MUL_1r, &reg_val, MID_PR_MUL_QUE_HIGHf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_1r(unit, 0, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_1r(unit, 1, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_1r(unit, 2, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_1r(unit, 3, reg_val));
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe1600_fabric_multicast_priority_range_get
 * Purpose:
 *      Get multicast priority ranges
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      type    - (IN)  Range to get (Low / Mid x Min / Max)
  *     arg     - (OUT) Multicast group number
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe1600_fabric_multicast_priority_range_get(int unit, bcm_fabric_control_t type, int* arg)
{
    uint32 reg_val = 0;
    SOCDNX_INIT_FUNC_DEFS;
    
    switch(type)
    {    
        case bcmFabricMcLowPrioMin:
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOW_PR_MUL_0r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_LOW_PR_MUL_0r, reg_val, LOW_PR_MUL_QUE_LOWf);
            break;
        case bcmFabricMcLowPrioMax:
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOW_PR_MUL_1r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_LOW_PR_MUL_1r, reg_val, LOW_PR_MUL_QUE_HIGHf);
            break;
        case bcmFabricMcMidPrioMin:
            SOCDNX_IF_ERR_EXIT(READ_DCH_MID_PR_MUL_0r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_MID_PR_MUL_0r, reg_val, MID_PR_MUL_QUE_LOWf);
            break;
        case bcmFabricMcMidPrioMax:
            SOCDNX_IF_ERR_EXIT(READ_DCH_MID_PR_MUL_1r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_MID_PR_MUL_1r, reg_val, MID_PR_MUL_QUE_HIGHf);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_fe1600_fabric_multicast_multi_set(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    bcm_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array) 
{
    int rv, i;
    uint32 *entry_array, *entry;
    int size;
    int index_max, index_min;
    bcm_fabric_module_vector_t *cur_dest_array;
    bcm_multicast_t group;
    int entry_words;
    uint32 mem_flags;

    SOCDNX_INIT_FUNC_DEFS; 
    SOC_FE1600_ONLY(unit);

    size = SOC_MEM_TABLE_BYTES(unit, RTP_MCTm);
    index_max = soc_mem_index_max(unit, RTP_MCTm);
    index_min = soc_mem_index_min(unit, RTP_MCTm);
    entry_array = soc_cm_salloc(unit, size, "RTP_MCTm");
    if (entry_array == NULL) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("failed to allocate memory")));
    }
    rv = soc_mem_array_read_range(unit, RTP_MCTm, 0, MEM_BLOCK_ANY, index_min, index_max, entry_array);
    SOCDNX_IF_ERR_EXIT(rv);

    
    entry_words = soc_mem_entry_words(unit, RTP_MCTm) - 1; /* -1 for ignoring parity byte*/
    for (i=0, cur_dest_array=dest_array; i<ngroups; i++, cur_dest_array++) {
        group = groups[i];
        entry = entry_array + (entry_words * group);
        soc_mem_field_set(unit, RTP_MCTm, entry, LINK_BIT_MAPf, *cur_dest_array /*value to set*/);
    }
    if (flags & _SHR_FABRIC_MULTICAST_COMMIT_ONLY) {
        mem_flags = SOC_MEM_WRITE_COMMIT_ONLY;
    } else if (flags & _SHR_FABRIC_MULTICAST_SET_ONLY) {
        mem_flags = SOC_MEM_WRITE_SET_ONLY;
    } else if (flags & _SHR_FABRIC_MULTICAST_STATUS_ONLY) {
        mem_flags = SOC_MEM_WRITE_STATUS_ONLY;
    } else {
        mem_flags = 0;
    }

    rv = soc_mem_array_write_range(unit, mem_flags, RTP_MULTI_CAST_TABLE_UPDATEm, 0, MEM_BLOCK_ALL, index_min, index_max, entry_array);
    SOCDNX_IF_ERR_EXIT(rv);


    exit:
    if (entry_array != NULL) {
        soc_cm_sfree(unit, entry_array);
    }
    SOCDNX_FUNC_RETURN;
}

int 
soc_fe1600_fabric_multicast_multi_get(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    bcm_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array) 
{
    int rv,i;
    uint32 *entry_array, *entry;
    int size;
    int index_max, index_min;
    bcm_fabric_module_vector_t *cur_dest_array;
    bcm_multicast_t group;
    int entry_words;
    SOCDNX_INIT_FUNC_DEFS; 
	SOC_FE1600_ONLY(unit);

    size = SOC_MEM_TABLE_BYTES(unit, RTP_MCTm);
    index_max = soc_mem_index_max(unit, RTP_MCTm);
    index_min = soc_mem_index_min(unit, RTP_MCTm);
    entry_array = soc_cm_salloc(unit, size, "RTP_MCTm");
    if (entry_array == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("failed to allocate memory")));
    }
    rv = soc_mem_array_read_range(unit, RTP_MCTm, 0, MEM_BLOCK_ANY, index_min, index_max, entry_array);
    SOCDNX_IF_ERR_EXIT(rv);

    entry_words = soc_mem_entry_words(unit, RTP_MCTm);
    for (i=0, cur_dest_array=dest_array; i<ngroups; i++, cur_dest_array++) {
        group = groups[i];
        entry = entry_array + (entry_words * group);
        soc_mem_field_get(unit, RTP_MCTm, entry, LINK_BIT_MAPf, *cur_dest_array);
    }

    exit:
    if (entry_array != NULL) {
        soc_cm_sfree(unit, entry_array);
    }
    SOCDNX_FUNC_RETURN;
}



#endif /*defined(BCM_88750_A0)*/

#undef _ERR_MSG_MODULE_NAME

