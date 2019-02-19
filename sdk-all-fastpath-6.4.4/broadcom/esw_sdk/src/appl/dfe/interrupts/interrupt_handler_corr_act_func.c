/*
 * $Id: interrupt_handler_cb_func.c,v 1.12 Broadcom SDK $
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
 */


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_INTR

#include <shared/bsl.h>

/* 
 *  include  
 */ 
#include <sal/core/time.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm_int/dfe/fabric.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#include <soc/dfe/fe1600/fe1600_port.h>


#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/dfe_dispatch.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>

#include <appl/dfe/interrupts/interrupt_handler.h>
#include <appl/dfe/interrupts/interrupt_handler_cb_func.h>
#include <appl/dfe/interrupts/interrupt_handler_corr_act_func.h>

#include <appl/dcmn/interrupts/interrupt_handler.h>

#include <appl/dcmn/rx_los/rx_los.h>


/*
 *  Corrective Action Utility functions
 */


/*
 *  Corrective Action functions    
 */
int
fe1600_interrupt_handles_corrective_action_do_nothing (
  int unit,
  int block_instance,
  fe1600_interrupt_type interrupt_id,
  char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;
    

    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_handles_corrective_action_force(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "%s(): unit=%d, block_instance=%d, interrupt_id=%d\n"), FUNCTION_NAME(), unit, block_instance, interrupt_id));
#endif

    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_handles_corrective_action_hard_reset(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    char *msg)
{
    int rc = 0;
    bcm_pbmp_t rx_los_pbmp;
    int rx_los_enable;
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "Decision: Hard reset!\n")));
#endif


    /*RX LOS App disable - required in order to run soft reset without traffic loss*/
    rc = rx_los_enable_get(unit, &rx_los_pbmp, &rx_los_enable);
    SOCDNX_IF_ERR_EXIT(rc);

    if (rx_los_enable) {
        rc = rx_los_unit_detach(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }

    rc = soc_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    /*RX LOS App enable - without resetting RX LOS - required in order to run soft reset without traffic loss*/
    if (rx_los_enable) {
        rc = rx_los_unit_attach(unit, rx_los_pbmp, 1);
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_handles_corrective_action_shutdown_link(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    uint8* array_links,
    char* msg)
{
    int rc ;
    int link_number ;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(array_links);

    for(link_number = 0 ;link_number < SOC_FE1600_NOF_LINKS ;link_number++) {
        if(FE1600_INT_LINK_SHUTDOWN == array_links[link_number]) {
            if (IS_SFI_PORT(unit, link_number)) {
                rc = bcm_port_enable_set(unit, link_number, FE1600_INT_LINK_SHUTDOWN);
                SOCDNX_IF_ERR_EXIT(rc);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}

int
fe1600_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    char* msg_print,
    char* msg)
{
    soc_interrupt_db_t* interrupt;
    char print_msg[FE1600_INTERRUPT_PRINT_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(msg_print);

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);

    /* prepare string for print */
#if defined(SOC_NO_NAMES)
    sal_sprintf(print_msg,"id=%d, index=%d, block=%d, unit=%d | %s\n ", 
                interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#else
    sal_sprintf(print_msg,"name=%s, id=%d, index=%d, block=%d, unit=%d | %s\n ", 
                interrupt->name, interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#endif

    LOG_ERROR(BSL_LS_BCM_INTR,
              (BSL_META_U(unit,
                          "%s"), print_msg));
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_handles_corrective_action_mc_rtp_correct(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    fe1600_interrupt_mc_rtp_table_correct_info* mc_rtp_table_correct_info_p,
    char* msg)
{

    uint32 data[SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(mc_rtp_table_correct_info_p);

    /* read valid ndx */
    SOCDNX_IF_ERR_EXIT(READ_RTP_MCTm(unit, mc_rtp_table_correct_info_p->valid_table_id, MEM_BLOCK_ANY, mc_rtp_table_correct_info_p->multicast_id, data));

    /* update for all copies */
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ALL, mc_rtp_table_correct_info_p->multicast_id, data));


exit:
    SOCDNX_FUNC_RETURN;

}

int
fe1600_interrupt_handles_corrective_action_uc_rtp_correct(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    int* route_update_dirty_index,
    char* msg)
{
    uint64 rtp_unicast_route_update_dirty_r;
    uint64 add_index;
    uint32 low_reg ; 
    uint32 high_reg ;
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(route_update_dirty_index);

    if((*route_update_dirty_index) >= SOC_REG_INFO(unit, RTP_UNICAST_ROUTE_UPDATE_DIRTYr).fields[0].len ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOC_MSG("Update Dirty Index is out of range"))); 
    }

    low_reg = 0;
    high_reg = 0;
    
    if((*route_update_dirty_index) < 32) {
        SHR_BITSET((&low_reg), (*route_update_dirty_index));
    } 
    else {
        SHR_BITSET((&high_reg), ((*route_update_dirty_index) - 32));
    }

    COMPILER_64_SET(add_index, high_reg, low_reg);

    rc = READ_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, &rtp_unicast_route_update_dirty_r);
    SOCDNX_IF_ERR_EXIT(rc);

    COMPILER_64_OR(rtp_unicast_route_update_dirty_r, add_index);

    rc = WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, rtp_unicast_route_update_dirty_r);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int 
fe1600_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    fe1600_interrupt_shadow_correct_info* shadow_correct_info_p,
    char* msg)
{
    int rc;
    uint32* data_entry;
    uint8 orig_read_through_flag;
    int entry_dw;
    int current_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(shadow_correct_info_p);
 
    if (soc_dfe_tbl_is_dynamic(unit, shadow_correct_info_p->mem)) {
        SOC_EXIT;
    }

    if (!soc_mem_cache_get(unit, shadow_correct_info_p->mem, shadow_correct_info_p->copyno)) {
         SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
 
    entry_dw = soc_mem_entry_words(unit, shadow_correct_info_p->mem);
    data_entry = sal_alloc((entry_dw * 4), "FE1600_INTERRUPT Shadow data entry allocation");

    if (NULL == data_entry) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    
    orig_read_through_flag = SOC_MEM_FORCE_READ_THROUGH(unit);
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0); /* read from cache */
    for (current_index = shadow_correct_info_p->min_index ; current_index <= shadow_correct_info_p->max_index ; current_index++) {
        
         rc = soc_mem_array_read(unit, shadow_correct_info_p->mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, data_entry);
         SOCDNX_IF_ERR_EXIT(rc);

         rc = soc_mem_array_write(unit, shadow_correct_info_p->mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, data_entry);
         SOCDNX_IF_ERR_EXIT(rc);

    }
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_through_flag);
    
    if(data_entry != NULL) {
        sal_free(data_entry);
    }

exit:
    SOCDNX_FUNC_RETURN;

}

/* Corrective Action main function */
int
fe1600_interrupt_handles_corrective_action_all_reachable_fix(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    soc_reg_above_64_val_t* integrity_vec,
    char* msg)
{
    int i;
    uint32 link_bit_map[SOC_FE1600_NOF_LINKS/32] = {0};
    soc_reg_above_64_val_t rmhmt_entry;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(*integrity_vec);
    SOC_REG_ABOVE_64_CLEAR(rmhmt_entry);
    for (i = 0 ; i < soc_mem_index_count(unit, RTP_RMHMTm) ; i++) {
        SOCDNX_IF_ERR_EXIT(READ_RTP_RMHMTm(unit, MEM_BLOCK_ANY, i, rmhmt_entry));
        SHR_BITAND_RANGE(rmhmt_entry, *integrity_vec, 0, SOC_FE1600_NOF_LINKS, link_bit_map);
        soc_mem_field_set(unit, RTP_RMHMTm, rmhmt_entry, LINK_BIT_MAPf, link_bit_map);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RMHMTm(unit, MEM_BLOCK_ANY, i, rmhmt_entry));
    }
exit:
    SOCDNX_FUNC_RETURN;
}
int
fe1600_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    fe1600_interrupt_type interrupt_id,
    char *msg,
    fe1600_int_corr_act_type corr_act,
    void *param1, 
    void *param2)
{
    int rc;
    uint32 flags;
    soc_port_t link;

    SOCDNX_INIT_FUNC_DEFS;
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    BCMDNX_IF_ERR_EXIT(rc);
    
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = FE1600_INT_CORR_ACT_NONE;
    }
    
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = FE1600_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
        case FE1600_INT_CORR_ACT_NONE:
            rc = fe1600_interrupt_handles_corrective_action_do_nothing(unit, block_instance, interrupt_id, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_CORR_ACT_FORCE:
            rc = fe1600_interrupt_handles_corrective_action_force(unit, block_instance, interrupt_id, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_CORR_ACT_HARD_RESET:
            rc = fe1600_interrupt_handles_corrective_action_hard_reset(unit, block_instance, interrupt_id, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_CORR_ACT_SHUTDOWN_LINKS:
            rc = fe1600_interrupt_handles_corrective_action_shutdown_link(unit, block_instance, interrupt_id,(uint8*)param1, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_CORR_ACT_PRINT:
            rc = fe1600_interrupt_handles_corrective_action_print(unit, block_instance, interrupt_id,(char*)param1, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_CORR_ACT_MC_RTP_CORRECT:
            rc = fe1600_interrupt_handles_corrective_action_mc_rtp_correct(unit, block_instance, interrupt_id,(fe1600_interrupt_mc_rtp_table_correct_info*)param1, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_CORR_ACT_UC_RTP_CORRECT:
            rc = fe1600_interrupt_handles_corrective_action_uc_rtp_correct(unit, block_instance, interrupt_id,(int*)param1, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_CORR_ACT_SHADOW:
            rc = fe1600_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(fe1600_interrupt_shadow_correct_info*)param1, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_CORR_ACT_RX_LOS_HANDLE:
            /* get link number */
            SOCDNX_NULL_CHECK(param1);
            link = *((soc_port_t *)param1);
            if (rx_loss_appl_enable(unit)) {
                rc = rx_los_handle(unit,link);
                SOCDNX_IF_ERR_EXIT(rc);
            }
            break;
        case FE1600_INT_CORR_ACT_ALL_REACHABLE_FIX:
            rc = fe1600_interrupt_handles_corrective_action_all_reachable_fix(unit, block_instance, interrupt_id,(soc_reg_above_64_val_t*)param1, msg);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("UnKnown corrective action")));

  }
    
exit:
  SOCDNX_FUNC_RETURN;
}


/*
*  common utilities for the same interrupts
*/

int
fe1600_interrupt_data_collection_for_recuring_action_dch_alto(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    fe1600_int_corr_act_type * p_corrective_action,
    uint8*  array_links,
    char* special_msg)
{

    int rc  ;
    uint32 data[1];
    fe1600_links links;
    int link_number, str_pos;
    int link_status_changed;
    char temp_buff[10];
    char shutdown_links_num[200];
 
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(array_links);
    SOCDNX_NULL_CHECK(special_msg);

    *p_corrective_action = FE1600_INT_CORR_ACT_NONE;

    switch (en_fe1600_interrupt) {
        case FE1600_INT_DCH_ALTOPINT:
            /*read Alto BmpP*/
            rc = READ_DCH_REG_010Dr(unit, block_instance, data);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case FE1600_INT_DCH_ALTOSINT:
            /*read Alto BmpS*/
            rc = READ_DCH_REG_010Er(unit, block_instance, data);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));
    }

    /* get links for current DCH block */
    fe1600_interrupt_dch_links_get(unit, block_instance, &links);  
  
    /* get problem links */
    str_pos = 0;
    for(link_number = 0; link_number < FE1600_INT_LINKS_NUM_IN_DCH_BLOCK; link_number++) {
        link_status_changed  = SHR_BITGET(data, link_number);
        if(link_status_changed) {
            *p_corrective_action = FE1600_INT_CORR_ACT_SHUTDOWN_LINKS ;
            /* add link to shutdown table*/
            array_links[(links.first_link) + link_number] = FE1600_INT_LINK_SHUTDOWN;
            /* add corrupted link to message */
            sal_sprintf(temp_buff, "%d ", link_number);
            sal_strncpy((&shutdown_links_num[str_pos]), temp_buff, sizeof(shutdown_links_num) - str_pos);
            str_pos+=sal_strlen(temp_buff);
            sal_strncpy((&shutdown_links_num[str_pos]), " ", sizeof(shutdown_links_num) - str_pos);
            str_pos++;
        }/*if link status changed*/
    } /*end for*/

    sal_sprintf(special_msg, "corrupted links: %s",shutdown_links_num);

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_handle_dch_ifm(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    fe1600_int_corr_act_type * p_corrective_action,
    char* special_msg)
{
    
    int rc;
    soc_reg_t reg;
    soc_field_t field;
    uint32 data, regval;
    char mode_string[FE1600_INTERRUPT_PRIVATE_STRING_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(special_msg);

    /* update default corrective action */
    *p_corrective_action = FE1600_INT_CORR_ACT_NONE;
    
    switch (en_fe1600_interrupt) {
    case FE1600_INT_DCH_IFMAFOPINT:
        reg = DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr;
        field = MAX_IFMAFPf;
        sal_strncpy(mode_string, "MAX_IFMAFPf=", sizeof(mode_string));
        break;
    case FE1600_INT_DCH_IFMBFOPINT:
        reg = DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr;
        field = MAX_IFMBFPf;
        sal_strncpy(mode_string, "MAX_IFMBFPf=", sizeof(mode_string));
        break;
    case FE1600_INT_DCH_IFMAFOSINT:
        reg = DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr;
        field = MAX_IFMAFSf;
        sal_strncpy(mode_string, "MAX_IFMAFSf=", sizeof(mode_string));
        break;
    case FE1600_INT_DCH_IFMBFOSINT:
        reg = DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr;
        field = MAX_IFMBFSf;
        sal_strncpy(mode_string, "MAX_IFMBFSf=", sizeof(mode_string));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));
    }
    
    rc = soc_reg32_get(unit, reg, block_instance, 0, &regval);
    SOCDNX_IF_ERR_EXIT(rc);

    data = soc_reg_field_get(unit, reg, regval, field);
    
    if (data > 250) { /*bigger then 250 is illegal*/
        *p_corrective_action = FE1600_INT_CORR_ACT_HARD_RESET ;
    }
    
    sal_sprintf(special_msg, "%s 0x%02x", mode_string, data);

exit:
    SOCDNX_FUNC_RETURN;
}

int fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    uint8*  array_links,
    char* special_msg)

{
    int rc;
    int link;
    uint64 value, val64;
    uint32 flags;
    int interval;
    pbmp_t pbmp;
    char str_value[FE1600_INTERRUPT_MESSAGE_FOR_CONVERSION];
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(array_links);
    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    /* get link number */
    rc = fe1600_interrupt_fmac_link_get(unit, block_instance ,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);   
    
    /* get soc counter status */
    rc = soc_counter_status(unit, &flags, &interval, &pbmp);
    SOCDNX_IF_ERR_EXIT(rc);

    /* read statistic counter */
    if(interval != 0)
    {
        rc = bcm_stat_sync(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    
    rc = bcm_stat_get(unit, link, snmpBcmRxCrcErrors, &value);
    SOCDNX_IF_ERR_EXIT(rc);

    array_links[link] = FE1600_INT_LINK_SHUTDOWN;
    COMPILER_64_SET(val64, 0, FE1600_INT_CRC_MASK );
    COMPILER_64_AND(value, val64);
    format_uint64(str_value, value);
    sal_sprintf(special_msg, "link=%d, fmal_statistic_output_control_crc=%s", link, str_value);

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_handle_mac_wrongsize_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    int link;
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);
    sal_sprintf(special_msg, "link=%d ",link);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_handle_mac_los_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    int link;
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);

    sal_sprintf(special_msg, "link=%d ",link);
    
exit:
    SOCDNX_FUNC_RETURN;
}



int
fe1600_interrupt_data_collection_for_handle_mac_rxlostofsync_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    soc_port_t *link,
    char* special_msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, link);
    SOCDNX_IF_ERR_EXIT(rc);

    sal_sprintf(special_msg, "link=%d, diagnose SerDes ",*link);
    
exit:
    SOCDNX_FUNC_RETURN;
}


int
fe1600_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg,
    uint8*  array_links)
{
    int link;
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(array_links);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);
    array_links[link] = FE1600_INT_LINK_SHUTDOWN;
    sal_sprintf(special_msg, "link = %d ",link);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_handle_mac_oof_int( 
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    soc_dcmn_port_pcs_t* p_pcs,
    char* special_msg)
{
    int rc;
    int link;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_pcs);
    SOCDNX_NULL_CHECK(special_msg);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);

    rc = soc_fe1600_port_control_pcs_get( unit, link, p_pcs);
    SOCDNX_IF_ERR_EXIT(rc);

    if(soc_dcmn_port_pcs_8_10 != (*p_pcs))
    {
        sal_sprintf(special_msg, "The receiver frame decoder lost of frame alignment for link %d", link);
    }
    else{
        sal_sprintf(special_msg, "Out of frame interrupt received for link %d for for encoding type 8/10, for this mode interrupt should be masked ",link);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_recurring_mac_oof_int( 
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    soc_dcmn_port_pcs_t* p_pcs,
    fe1600_int_corr_act_type* p_corrective_action,
    char* special_msg,
    uint8*  array_links)
{
    int rc;
    int link;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_pcs);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(array_links);

    *p_corrective_action = FE1600_INT_CORR_ACT_NONE ;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);

    rc = soc_fe1600_port_control_pcs_get( unit, block_instance ,p_pcs);
    SOCDNX_IF_ERR_EXIT(rc);

    if(soc_dcmn_port_pcs_8_10 != (*p_pcs))
    {
        sal_sprintf(special_msg, "The receiver frame decoder lost of frame alignment for link %d", link);
    }
    else{
        sal_sprintf(special_msg, "Out of frame interrupt received for link %d for for encoding type 8/10, for this mode interrupt should be masked ",link);
    }

    if(soc_dcmn_port_pcs_8_10 != *p_pcs){
    /* shutdown  link */
        array_links[link] = FE1600_INT_LINK_SHUTDOWN;
        *p_corrective_action = FE1600_INT_CORR_ACT_SHUTDOWN_LINKS ;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int fe1600_interrupt_data_collection_for_handle_mac_decerr_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    soc_dcmn_port_pcs_t* p_pcs,
    fe1600_int_corr_act_type* p_corrective_action,
    char* special_msg,
    uint8*  array_links)

{
    int rc;
    int link;
    uint64 value, val64;
    uint32 flags;
    int interval;
    pbmp_t pbmp;
    bcm_stat_val_t error_type ;
    char mode_string[FE1600_INTERRUPT_MESSAGE_FOR_CONVERSION];
    char str_value[FE1600_INTERRUPT_MESSAGE_FOR_CONVERSION];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_pcs);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(array_links);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    /* update default corrective action */
    *p_corrective_action = FE1600_INT_CORR_ACT_NONE;
    
    /* get link number */
    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);   
    
    rc = soc_fe1600_port_control_pcs_get( unit, block_instance ,p_pcs);
    SOCDNX_IF_ERR_EXIT(rc);

    switch(*p_pcs) {
        case soc_dcmn_port_pcs_8_9_legacy_fec:
            sal_strncpy(mode_string, "8_9_legacy_fec", sizeof(mode_string));
            error_type = snmpBcmRxFecUncorrectable ;
            break;
        case soc_dcmn_port_pcs_64_66_fec:
            sal_strncpy(mode_string, "64_66_fec", sizeof(mode_string));
            error_type = snmpBcmRxFecUncorrectable ;
            break;
        case soc_dcmn_port_pcs_64_66_bec :
            sal_strncpy(mode_string, "64_66_bec", sizeof(mode_string));
            error_type = snmpBcmRxBecRxFault;
            break;
        case soc_dcmn_port_pcs_8_10 :
            sal_strncpy(mode_string, "8_10", sizeof(mode_string));
            error_type = snmpBcmRxCodeErrors;
            break;
        default:
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
        
    /* get soc counter status */
    rc = soc_counter_status(unit, &flags, &interval, &pbmp);
    SOCDNX_IF_ERR_EXIT(rc);

    if(interval!= 0){
        /* read statistic counter */
        rc = bcm_stat_sync(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    
    rc = bcm_stat_get(unit, link, error_type, &value);
    SOCDNX_IF_ERR_EXIT(rc);

    /* define corrective action  and fill shutdown link structure   */
    COMPILER_64_SET(val64, 0, FE1600_INT_FEC_UNCORRECTABLE_MASK);
    COMPILER_64_AND(value, val64);
    COMPILER_64_SET(val64, 0, FE1600_INT_FEC_UNCORRECTABLE_THRESHOLD);
    if(COMPILER_64_GT(value, val64)) {
        array_links[link] = FE1600_INT_LINK_SHUTDOWN;
        *p_corrective_action = FE1600_INT_CORR_ACT_SHUTDOWN_LINKS;
    }
  
    format_uint64(str_value, value);
    sal_sprintf(special_msg, "link=%d, pcs mode=%s, fmal_statistic_output_control=%s", link, mode_string, str_value);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int fe1600_interrupt_data_collection_for_mac_transmit_err_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg,
    fe1600_int_corr_act_type* p_corrective_action,
    uint8* array_links)
{
    int rc;
    int link;
    soc_dcmn_port_pcs_t pcs;
    char mode_string[20];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(array_links);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

     /* update default corrective action */
    *p_corrective_action = FE1600_INT_CORR_ACT_NONE;

    /* get link number */
    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);
       
    rc = soc_fe1600_port_control_pcs_get( unit, block_instance ,&pcs);
    SOCDNX_IF_ERR_EXIT(rc);

    /* perform mode string and select shut down link for 8_10 mode */
    switch(pcs) {
    case soc_dcmn_port_pcs_8_9_legacy_fec:
        sal_strncpy(mode_string, "8_9_legacy_fec", sizeof(mode_string));
        break;
    case soc_dcmn_port_pcs_64_66_fec:
        sal_strncpy(mode_string, "64_66_fec", sizeof(mode_string));
        break;
    case soc_dcmn_port_pcs_64_66_bec:
        sal_strncpy(mode_string, "64_66_bec", sizeof(mode_string));
        break;
    case soc_dcmn_port_pcs_8_10:
        sal_strncpy(mode_string, "8_10", sizeof(mode_string));
        *p_corrective_action = FE1600_INT_CORR_ACT_SHUTDOWN_LINKS;
        break;
    default:
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    sal_sprintf(special_msg, "link=%d, pcs mode=%s", link, mode_string);

exit:
    SOCDNX_FUNC_RETURN;
}

int fe1600_interrupt_data_collection_for_mac_ecc_1berr_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg,
    fe1600_int_corr_act_type* p_corrective_action,
    uint8* array_links)
{
    int rc;
    int link;
    soc_dcmn_port_pcs_t pcs;
    char mode_string[20];
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(array_links);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    /* update default corrective action */
    *p_corrective_action = FE1600_INT_CORR_ACT_NONE;

    /* get link number */
    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);   

    rc = soc_fe1600_port_control_pcs_get(unit, block_instance ,&pcs);
    SOCDNX_IF_ERR_EXIT(rc);

    /* perform mode string and select shut down link for 8_10 mode */
    switch(pcs) {
    case soc_dcmn_port_pcs_8_9_legacy_fec:
        sal_strncpy(mode_string, "8_9_legacy_fec", sizeof(mode_string));
        break;
    case soc_dcmn_port_pcs_64_66_fec:
        sal_strncpy(mode_string, "64_66_fec", sizeof(mode_string));
        break;
    case soc_dcmn_port_pcs_64_66_bec :
        sal_strncpy(mode_string, "64_66_bec", sizeof(mode_string));
        break;
    case soc_dcmn_port_pcs_8_10 :
        sal_strncpy(mode_string, "8_10", sizeof(mode_string));
        *p_corrective_action = FE1600_INT_CORR_ACT_SHUTDOWN_LINKS;
        break;
    default:
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    sal_sprintf(special_msg, "link=%d, pcs mode =%s", link, mode_string);
exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_mac_lnklvlagen_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    uint32 lnk_lvl_flow_control_conf_reg;
    uint32 lnk_lvl_age_prd;
    int link;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);  
    
    rc = soc_reg32_get(unit, BRDC_FMACH_LINK_LEVEL_FLOW_CONTROL_CONFIGURATION_REGISTERr, block_instance, 0, &lnk_lvl_flow_control_conf_reg);
    SOCDNX_IF_ERR_EXIT(rc);
      
    lnk_lvl_age_prd = soc_reg_field_get(unit, BRDC_FMACH_LINK_LEVEL_FLOW_CONTROL_CONFIGURATION_REGISTERr, lnk_lvl_flow_control_conf_reg, LNK_LVL_AGE_PRDf);
    sal_sprintf(special_msg,"link=%d, LnkLvlAgePrd=0x%01x " ,link, lnk_lvl_age_prd );

exit:
    SOCDNX_FUNC_RETURN;
}
int
fe1600_interrupt_data_collection_for_mac_lnklvlagectxbn_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    uint32 lnk_lvl_flow_control_conf_reg;
    uint32 lnk_lvl_age_prd;
    int link;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = fe1600_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);  
    
    rc = soc_reg32_get(unit, BRDC_FMACH_LINK_LEVEL_FLOW_CONTROL_CONFIGURATION_REGISTERr, block_instance, 0, &lnk_lvl_flow_control_conf_reg );
    SOCDNX_IF_ERR_EXIT(rc);  

    lnk_lvl_age_prd = soc_reg_field_get(unit, BRDC_FMACH_LINK_LEVEL_FLOW_CONTROL_CONFIGURATION_REGISTERr, lnk_lvl_flow_control_conf_reg, LNK_LVL_AGE_PRDf);
    sal_sprintf(special_msg,"link = %d, LnkLvlAgePrd = 0x%01x " ,link, lnk_lvl_age_prd );

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_handle_ccs_cpucaptcellfneint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    soc_reg_above_64_val_t  reg_val, field_val;
    char rval_str[256];
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    rc = READ_CCS_CAPTURED_CELLr(unit, block_instance, reg_val);
    SOCDNX_IF_ERR_EXIT(rc);
    
    soc_reg_above_64_field_get(unit, CCS_CAPTURED_CELLr, reg_val, CAPTURED_CELLf, field_val);
    format_long_integer(rval_str, field_val, 4);
                
    sal_sprintf(special_msg,"captured cell data = %s",rval_str);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
fe1600_interrupt_data_collection_for_handle_dcl_ccpsrcdcnglink_int(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    int rc;
    uint32 cnc_tvt_links_evt_reg;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    rc = soc_reg32_get(unit, DCL_CONNECTIVITY_LINKS_EVENTSr, block_instance, 0, &cnc_tvt_links_evt_reg);
    SOCDNX_IF_ERR_EXIT(rc); 

    sal_sprintf(special_msg,"DCL_CONNECTIVITY_LINKS_EVENTS_REG= 0x%08X", cnc_tvt_links_evt_reg);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    int rc;
    uint32 global_reg_data;
    uint32 dual_pipe_en;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    rc = soc_reg32_get(unit, ECI_GLOBAL_1r, block_instance, 0, &global_reg_data);
    SOCDNX_IF_ERR_EXIT(rc);

    dual_pipe_en = soc_reg_field_get(unit, ECI_GLOBAL_1r, global_reg_data, DUAL_PIPE_ENf);
    sal_sprintf(special_msg,"RTP_DUAL_PIPE_EN= %01d", dual_pipe_en);

exit:
    SOCDNX_FUNC_RETURN;

}

int
fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationabovemaxbaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    int rc;
    uint64 global_reg_data;
    uint64 max_base_index;
    char str_value[FE1600_INTERRUPT_MESSAGE_FOR_CONVERSION];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    rc = soc_reg64_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, block_instance, 0, &global_reg_data);
    SOCDNX_IF_ERR_EXIT(rc);

    max_base_index = soc_reg64_field_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, global_reg_data, MAX_BASE_INDEXf);

    format_uint64(str_value,  max_base_index);
    sal_sprintf(special_msg,"RTP_MAX_BASE_INDEX= %s",str_value);

exit:
    SOCDNX_FUNC_RETURN;

}

int
fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    int rc;
    uint64 global_reg_data;
    uint64 update_base_index;
    char str_value[FE1600_INTERRUPT_MESSAGE_FOR_CONVERSION];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    rc = soc_reg64_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, block_instance, 0, &global_reg_data);
    SOCDNX_IF_ERR_EXIT(rc);

    update_base_index = soc_reg64_field_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, global_reg_data, UPDATE_BASE_INDEXf);

    format_uint64(str_value, update_base_index);
    sal_sprintf(special_msg,"RTP_UPDATE_BASE_INDEX= %s",str_value);

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_handle_rtp_drhp_queryunreachablemulticast(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    int rc;
    uint32 global_reg_data, drhp_unreach_mc_dest_id, drhp_unreach_mc_id;
    int array_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    switch(en_fe1600_interrupt) {
    case FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT:
        array_index = 0;
        break;
    case FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT:
        array_index = 1;
        break;
    case FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT:
        array_index = 2;
        break;
    case FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT:
        array_index = 3;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));
    }

    rc = READ_RTP_DRHP_UNREACHABLEMULTICASTINFOr(unit,array_index, &global_reg_data);
    SOCDNX_IF_ERR_EXIT(rc);

    drhp_unreach_mc_dest_id = soc_reg_field_get(unit, RTP_DRHP_UNREACHABLEMULTICASTINFOr, global_reg_data, DRHP_N_UNREACH_MC_DEST_IDf);
    drhp_unreach_mc_id = soc_reg_field_get(unit, RTP_DRHP_UNREACHABLEMULTICASTINFOr, global_reg_data, DRHP_N_UNREACH_MC_IDf);

    sal_sprintf(special_msg,"DRHP_UNREACH_MC_DEST_ID=0x%03x, DRHP_UNREACH_MC_ID=0x%05x", drhp_unreach_mc_dest_id, drhp_unreach_mc_id);

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_data_collection_for_handle_rtp_drhs_queryunreachablemulticast(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg)
{
    int rc;
    uint32 global_reg_data, drhs_unreach_mc_dest_id, drhs_unreach_mc_id;
    int array_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    switch(en_fe1600_interrupt) {
    case FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT:
        array_index = 0;
        break;
    case FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT:
        array_index = 1;
        break;
    case FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT:
        array_index = 2;
        break;
    case FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT:
        array_index = 3;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));
    }

    rc = READ_RTP_DRHS_UNREACHABLEMULTICASTINFOr(unit,array_index, &global_reg_data);
    SOCDNX_IF_ERR_EXIT(rc);

    drhs_unreach_mc_dest_id = soc_reg_field_get(unit, RTP_DRHS_UNREACHABLEMULTICASTINFOr, global_reg_data, DRHS_N_UNREACH_MC_DEST_IDf);
    drhs_unreach_mc_id = soc_reg_field_get(unit, RTP_DRHS_UNREACHABLEMULTICASTINFOr, global_reg_data, DRHS_N_UNREACH_MC_IDf);

    sal_sprintf(special_msg,"DRHP_UNREACH_MC_DEST_ID=0x%03x, DRHP_UNREACH_MC_ID=0x%05x", drhs_unreach_mc_dest_id, drhs_unreach_mc_id);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
fe1600_interrupt_data_collection_for_srd_fsrd_syncstatuschanged( 
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    soc_port_t *link,
    char* special_msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }
    
    /* get link number */
    rc = fe1600_interrupt_fsrd_link_get(unit, 
                                        block_instance, 
                                        SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].bit_in_field,
                                        SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt].reg_index, 
                                        link);
    SOCDNX_IF_ERR_EXIT(rc);
    
    sal_sprintf(special_msg, "link=%d, diagnose SerDes", *link);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_get_unreachable_p_dest_info(
    int unit,
    int block_instance,
    fe1600_interrupt_unreachable_dest_info* unreachable_dest_info_p)
{
    int rc;
    uint64 un_reach_desp_0_p_p;
    uint32 un_reach_dest = 0;
    
    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_64_ZERO(un_reach_desp_0_p_p);
    SOCDNX_NULL_CHECK(unreachable_dest_info_p);

    /* read unrteachable destination primary register */
    rc = READ_DCH_UN_REACH_DEST_0_Pr(unit, block_instance, &un_reach_desp_0_p_p);
    SOCDNX_IF_ERR_EXIT(rc);

    /* get unreachable destination counter */
    unreachable_dest_info_p->un_reach_dest_cnt = soc_reg64_field32_get(unit, DCH_UN_REACH_DEST_0_Pr, un_reach_desp_0_p_p, UNREACH_DEST_CNT_Pf);

    /* get bit overflow of destination counter */
    unreachable_dest_info_p->un_reach_dest_cnt_o = soc_reg64_field32_get(unit, DCH_UN_REACH_DEST_0_Pr, un_reach_desp_0_p_p, UNREACH_DEST_CNT_OPf);

    /* check dest mode and dest id */
    un_reach_dest = soc_reg64_field32_get(unit, DCH_UN_REACH_DEST_0_Pr, un_reach_desp_0_p_p, UNREACH_DEST_Pf);
    if(SHR_BITGET(&(un_reach_dest), FE1600_INTERRUPT_MULTICAST_BIT)!= 0) {
        unreachable_dest_info_p->dest_mode = FE1600_INT_MUTICAST_DEST_MODE;
    }
    else {
        unreachable_dest_info_p->dest_mode = FE1600_INT_UNICAST_DEST_MODE;
    }
    unreachable_dest_info_p->dest_id = ((un_reach_dest)&(FE1600_INTERRUPT_DESTINATION_ID_MASK)); 
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_get_unreachable_s_dest_info(
    int unit,
    int block_instance,
    fe1600_interrupt_unreachable_dest_info* unreachable_dest_info_p)
{
    int rc;
    uint64 un_reach_desp_0_s_p;
    uint32 un_reach_dest = 0;
    
    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_64_ZERO(un_reach_desp_0_s_p);

    SOCDNX_NULL_CHECK(unreachable_dest_info_p);

    /* read unrteachable destination primary register */
    rc = READ_DCH_UN_REACH_DEST_0_Sr(unit, block_instance, &un_reach_desp_0_s_p);
    SOCDNX_IF_ERR_EXIT(rc);

    /* get unreachable destination counter */
    unreachable_dest_info_p->un_reach_dest_cnt = soc_reg64_field32_get(unit, DCH_UN_REACH_DEST_0_Sr, un_reach_desp_0_s_p, UNREACH_DEST_CNT_Sf);

    /* get bit overflow of destination counter */
    unreachable_dest_info_p->un_reach_dest_cnt_o = soc_reg64_field32_get(unit, DCH_UN_REACH_DEST_0_Sr, un_reach_desp_0_s_p, UNREACH_DEST_CNT_OSf);

    /* check dest mode and dest id */
    un_reach_dest = soc_reg64_field32_get(unit, DCH_UN_REACH_DEST_0_Sr, un_reach_desp_0_s_p, UNREACH_DEST_Sf);
    if(SHR_BITGET(&(un_reach_dest), FE1600_INTERRUPT_MULTICAST_BIT)!= 0) {
        unreachable_dest_info_p->dest_mode = FE1600_INT_MUTICAST_DEST_MODE;
    } else {
        unreachable_dest_info_p->dest_mode = FE1600_INT_UNICAST_DEST_MODE;
    }
    unreachable_dest_info_p->dest_id = ((un_reach_dest)&(FE1600_INTERRUPT_DESTINATION_ID_MASK)); 
exit:
    SOCDNX_FUNC_RETURN;
}


int fe1600_interrupt_data_collection_for_handle_unreachable_destination(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg,
    fe1600_int_corr_act_type* p_corrective_action,
    int* corr_act_unicast_index,
    fe1600_interrupt_mc_rtp_table_correct_info* mc_rtp_table_correct_info_p)
{
    int rc;
    int is_free;
    int tables_num;
    fe1600_interrupt_unreachable_dest_info unreachable_dest_info;
    fe1600_interrupt_multicast_tables_info multicast_tables_info;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(corr_act_unicast_index);
    SOCDNX_NULL_CHECK(mc_rtp_table_correct_info_p);

    *p_corrective_action = FE1600_INT_CORR_ACT_NONE;

    /* get unreachable destination info */
    switch(en_fe1600_interrupt) {
    case FE1600_INT_DCH_UNREACHDESTEVPINT:
        rc = fe1600_interrupt_get_unreachable_p_dest_info(unit, block_instance, &unreachable_dest_info);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case FE1600_INT_DCH_UNREACHDESTEVSINT:
        rc = fe1600_interrupt_get_unreachable_s_dest_info(unit, block_instance, &unreachable_dest_info);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));
    }
    

    if(FE1600_INT_MUTICAST_DEST_MODE == (unreachable_dest_info.dest_mode)){
    
        /* multicast mode */

        /* Check configuration problem */
        rc = bcm_dfe_multicst_id_map_is_free_id(unit, unreachable_dest_info.dest_id, &is_free);
        SOCDNX_IF_ERR_EXIT(rc);

        if(!is_free) {

#if 1
            tables_num = FE1600_INTERRUPT_DEFAULT_TABLES_NUM;
#endif
            if(tables_num > FE1600_INTERRUPT_MAX_TABLES_FOR_RTP_TABLE_NON_REPAIR){
                rc = fe1600_interrupt_rtp_table_inconsistent_data_collection(unit, block_instance, tables_num, unreachable_dest_info.dest_id, &multicast_tables_info);
                SOCDNX_IF_ERR_EXIT(rc);

            }
            if(FE1600_INT_MULTICAST_TABLES_SHOULD_BE_CORRECTED == multicast_tables_info.multicast_tables_status) {
                /* for unreachdestevent scrub_shadow_write corrective action */
                
                /* prepare message */
                sal_sprintf(special_msg, "Dch_un_reach_dest_cnt_p=0x%04x, Dch_un_reach_dest_cnt_o_p=0x%01x, Dch_multicast_id=0x%05x, reference table id=0%01d",
                            unreachable_dest_info.un_reach_dest_cnt, unreachable_dest_info.un_reach_dest_cnt_o, unreachable_dest_info.dest_id, multicast_tables_info.valid_table_id);

                /* set value for rtp table correct structure*/
                mc_rtp_table_correct_info_p -> multicast_id   = unreachable_dest_info.dest_id;
                mc_rtp_table_correct_info_p -> tables_num     = tables_num;
                mc_rtp_table_correct_info_p -> valid_table_id = multicast_tables_info.valid_table_id;

                /* set corrective action */
                *p_corrective_action = FE1600_INT_CORR_ACT_MC_RTP_CORRECT;
            
            }
            else if (FE1600_INT_MULTICAST_TABLES_TOO_MANY_TRIALS_WITH_INCORRECT_DIFFERENT_TABLES == multicast_tables_info.multicast_tables_status) {
                sal_sprintf(special_msg, "Dch_un_reach_dest_cnt_p=0x%04x , Dch_un_reach_dest_cnt_o_p=0x%01x, multicast group=0x%05x  Inconsistency in more than one entry",
                            unreachable_dest_info.un_reach_dest_cnt, unreachable_dest_info.un_reach_dest_cnt_o, unreachable_dest_info.dest_id);

            }

            else {
                sal_sprintf(special_msg, "Dch_un_reach_dest_cnt_p=0x%04x, Dch_un_reach_dest_cnt_o_p=0x%01x, multicast group=0x%05x",
                            unreachable_dest_info.un_reach_dest_cnt, unreachable_dest_info.un_reach_dest_cnt_o, unreachable_dest_info.dest_id);
            }

        } 
        /* multicast id is free */
        else {
            sal_sprintf(special_msg, "Dch_un_reach_dest_cnt_p=0x%04x, Dch_un_reach_dest_cnt_o_p=0x%01x, Packet MC destination for undefined MC group=0x%05x is free",
                            unreachable_dest_info.un_reach_dest_cnt, unreachable_dest_info.un_reach_dest_cnt_o, unreachable_dest_info.dest_id);
        }
    }
    else {

        /* check if unicast id is valid */
        if((SOC_DFE_CONFIG(unit).is_dual_mode) && (unreachable_dest_info.dest_id > SOC_MEM_INFO(unit, RTP_DUCTPm).index_max)){
            sal_sprintf(special_msg, "Unicast group is not valid for dual mode Dch_un_reach_dest_cnt_p=0x%04x, Dch_un_reach_dest_cnt_o_p=0x%01x, Unicast group=0x%05x",
                        unreachable_dest_info.un_reach_dest_cnt, unreachable_dest_info.un_reach_dest_cnt_o, unreachable_dest_info.dest_id);
            *p_corrective_action = FE1600_INT_CORR_ACT_NONE;

        }
      
        else if ((!(SOC_DFE_CONFIG(unit).is_dual_mode)) && (unreachable_dest_info.dest_id > SOC_MEM_INFO(unit, RTP_CUCTm).index_max)){{
            sal_sprintf(special_msg, "Unicast group is not valid for dual mode Dch_un_reach_dest_cnt_p=0x%04x, Dch_un_reach_dest_cnt_o_p=0x%01x, Unicast group=0x%05x",
                        unreachable_dest_info.un_reach_dest_cnt, unreachable_dest_info.un_reach_dest_cnt_o, unreachable_dest_info.dest_id);

            }
        }
       
        else {       
       
            /* unicast mode */
            sal_sprintf(special_msg, "Dch_un_reach_dest_cnt_p=0x%04x, Dch_un_reach_dest_cnt_o_p=0x%01x, Unicast group=0x%05x",
                        unreachable_dest_info.un_reach_dest_cnt, unreachable_dest_info.un_reach_dest_cnt_o, unreachable_dest_info.dest_id);

            /* set corrective action */
            *p_corrective_action = FE1600_INT_CORR_ACT_UC_RTP_CORRECT;
            *corr_act_unicast_index = unreachable_dest_info.dest_id/FE1600_INTERRUPT_DIRTY_BIT_VECTOR_SIZE_TO_OVERRIDE_UNICAST_TABLE;
      
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int fe1600_interrupt_data_collection_for_handle_rtp_err(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char* special_msg,
    fe1600_int_corr_act_type* p_corrective_action,
    int* corr_act_unicast_index,
    fe1600_interrupt_mc_rtp_table_correct_info* mc_rtp_table_correct_info_p,
    fe1600_interrupt_shadow_correct_info*  shadow_correct_info)
{
    
    int rc;
    uint64 counter;
    uint32 cntf, cnt_overflowf, addrf, addr_validf, converted_addrf;
    int index, tables_num;
    unsigned numels;
    int is_free;
    soc_mem_t mem;
    char* memory_name; 
    fe1600_interrupt_multicast_tables_info multicast_tables_info;
    int cached_flag;
    
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(corr_act_unicast_index);
    SOCDNX_NULL_CHECK(mc_rtp_table_correct_info_p);

    *p_corrective_action = FE1600_INT_CORR_ACT_NONE;

    switch(en_fe1600_interrupt) {
    case FE1600_INT_RTP_ECC_2BERRINT:
         rc = READ_RTP_ECC_2B_ERR_CNTr(unit, &counter);
         SOCDNX_IF_ERR_EXIT(rc);

         /* get address validity bit */
         addr_validf = soc_reg64_field32_get(unit, RTP_ECC_2B_ERR_CNTr, counter, ECC_2B_ERR_ADDR_VALIDf);

         /* get memory address bit */
         addrf = soc_reg64_field32_get(unit, RTP_ECC_2B_ERR_CNTr, counter, ECC_2B_ERR_ADDRf);

         /* get counter overflow indication  */
         cnt_overflowf = soc_reg64_field32_get(unit, RTP_ECC_2B_ERR_CNTr, counter, ECC_2B_ERR_CNT_OVERFLOWf);

         /* get counter value of 2 bit error */
         cntf = soc_reg64_field32_get(unit, RTP_ECC_2B_ERR_CNTr, counter, ECC_2B_ERR_CNTf);
         break;

    case FE1600_INT_RTP_PARITYERRINT:
        rc = READ_RTP_PARITY_ERR_CNTr(unit, &counter);
        SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg64_field32_get(unit, RTP_PARITY_ERR_CNTr, counter, PARITY_ERR_ADDR_VALIDf);

        /* get memory address bit */
        addrf = soc_reg64_field32_get(unit, RTP_PARITY_ERR_CNTr, counter, PARITY_ERR_ADDRf);

        /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, RTP_PARITY_ERR_CNTr, counter, PARITY_ERR_CNT_OVERFLOWf);

        /* get counter value of 2 bit error */
        cntf = soc_reg64_field32_get(unit, RTP_PARITY_ERR_CNTr, counter, PARITY_ERR_CNTf);
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));
    }

    if (addr_validf != 0) {

        rc = fe1600_interrupt_convert_memory_address(unit, FE1600_INTERRUPT_RTP_BLOCK, addrf, &converted_addrf);
        SOCDNX_IF_ERR_EXIT(rc);

        mem = soc_addr_to_mem_extended(unit, FE1600_INTERRUPT_RTP_BLOCK , 0xff, converted_addrf);

        if(mem!= INVALIDm) {
            memory_name = SOC_MEM_NAME(unit, mem);
        } else {
            memory_name = NULL;
        }

        switch(mem) {
        case RTP_MCTm:
            rc = fe1600_interrupt_mct_addr_to_array_element_and_index(unit, addrf, &tables_num, &index);
            SOCDNX_IF_ERR_EXIT(rc);

            /* Check configuration problem */
            rc = bcm_dfe_multicst_id_map_is_free_id(unit, index, &is_free);
            SOCDNX_IF_ERR_EXIT(rc);

            if(!is_free && tables_num > FE1600_INTERRUPT_MAX_TABLES_FOR_RTP_TABLE_NON_REPAIR) {
                
                rc = fe1600_interrupt_rtp_table_inconsistent_data_collection(unit, block_instance, tables_num, index, &multicast_tables_info);
                SOCDNX_IF_ERR_EXIT(rc);                

                if (FE1600_INT_MULTICAST_TABLES_SHOULD_BE_CORRECTED == multicast_tables_info.multicast_tables_status) {
                    /* for unreachdestevent scrub_shadow_write corrective action */
                
                    /* prepare message */
                    sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x, memory=%s, multicast group=%d, reference table id=0%01d",
                                cntf, cnt_overflowf, addrf, memory_name, index, multicast_tables_info.valid_table_id);
                

                    /* set value for rtp table correct structure*/
                    mc_rtp_table_correct_info_p -> multicast_id   = index;
                    mc_rtp_table_correct_info_p -> tables_num     = tables_num;
                    mc_rtp_table_correct_info_p -> valid_table_id = multicast_tables_info.valid_table_id;

                    /* set corrective action */
                    *p_corrective_action = FE1600_INT_CORR_ACT_MC_RTP_CORRECT;
            
                } else if (FE1600_INT_MULTICAST_TABLES_TOO_MANY_TRIALS_WITH_INCORRECT_DIFFERENT_TABLES == multicast_tables_info.multicast_tables_status) {

                    sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x, memory=%s, index=%u, multicast group=0x%05x, Inconsistency in more than one entry",
                                cntf, cnt_overflowf, addrf, memory_name, index, multicast_tables_info.valid_table_id);

                } else {

                    sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x, memory=%s, index=%u, multicast group=0x%05x",
                                cntf, cnt_overflowf, addrf, memory_name, index, multicast_tables_info.valid_table_id);
      
                }
            } else {
                /* multicast id is free */
                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x, memory=%s, multicast group=0x%05x, is free ",
                            cntf, cnt_overflowf, addrf, memory_name, index);
            }
            break;
        case RTP_DUCTPm:
        case RTP_DUCTSm:
        case RTP_CUCTm:
            rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
            SOCDNX_IF_ERR_EXIT(rc);

            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x memory=%s, array element=%d, index=%d",
                    cntf, cnt_overflowf, addrf, memory_name, numels, index);

            /* set corrective action */
            *p_corrective_action = FE1600_INT_CORR_ACT_UC_RTP_CORRECT;
            if(!(SOC_DFE_CONFIG(unit).is_dual_mode) && (RTP_DUCTSm == mem)) {
                *corr_act_unicast_index = SOC_REG_INFO(unit, RTP_UNICAST_ROUTE_UPDATE_DIRTYr).fields[0].len/2 + index/FE1600_INTERRUPT_DIRTY_BIT_VECTOR_SIZE_TO_OVERRIDE_UNICAST_TABLE;
            } 
            else {
                *corr_act_unicast_index = index/FE1600_INTERRUPT_DIRTY_BIT_VECTOR_SIZE_TO_OVERRIDE_UNICAST_TABLE;
            }
            break;
       
        case INVALIDm:
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x memory does not exist",
                    cntf, cnt_overflowf, addrf);
            break;

        default:
            /* support for shadow memories*/
            rc = interrupt_memory_cached(unit, mem, block_instance, &cached_flag);
            if(rc != SOC_E_NONE)
            {
                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x memory does not exist",
                            cntf, cnt_overflowf, addrf);
   
            }
            else if (TRUE == cached_flag) {
            
                if((RTP_SLSCTm == mem) && (soc_dfe_load_balancing_mode_destination_unreachable == SOC_DFE_CONFIG(unit).fabric_load_balancing_mode)) {
                    sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x memory=%s",
                                cntf, cnt_overflowf, addrf, memory_name);
                } else {
                    rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
                    SOCDNX_IF_ERR_EXIT(rc);

                    sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x memory=%s, array element=%d, index=%d",
                                cntf, cnt_overflowf, addrf, memory_name, numels, index);
                    /* set corrective action */
                    *p_corrective_action = FE1600_INT_CORR_ACT_SHADOW;
                    shadow_correct_info->array_index = numels;
                    shadow_correct_info->copyno = COPYNO_ALL;
                    shadow_correct_info->min_index = index;
                    shadow_correct_info->max_index = index;
                    shadow_correct_info->mem = mem;
                }

            } else {
                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x memory=%s",
                            cntf, cnt_overflowf, addrf, memory_name);

            }
      } 
      
      /* for invalid received address */
    }  else {
        sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, block RTP memory address=0x%08x address is not valid",
                    cntf, cnt_overflowf, addrf);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
fe1600_interrupt_rtp_table_inconsistent_data_collection(
    int unit,
    int block_instance,
    int tables_num,
     uint32 multicast_id,
    fe1600_interrupt_multicast_tables_info* multicast_tables_info)
{
  
    uint32 nof_loops_done = 0;
    uint32 nof_diff_tbls = 0;
    uint32 first_diff_table_ndx = 0;
    uint32 mc_tbl_ndx;
    uint32 is_3_diff_tbl = 0;
    int is_another_loop_to_do = 0;
    int is_not_equal;
    int is_different_value_found = 0;
    int rc;
    uint32 i,j;
    int flag_end = 0;
    
    uint32 data[FE1600_INTERRUPT_DEFAULT_TABLES_NUM][SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(multicast_tables_info);
    
    multicast_tables_info -> multicast_tables_status = FE1600_INT_MULTICAST_TABLES_EQUAL;
  
    for( i = 0; i < FE1600_INTERRUPT_DEFAULT_TABLES_NUM; i++  ) {
        for( j = 0; j < SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32; j ++) {
            data[i][j] = 0;
        }
    }

    while(0 == flag_end) {

        /* 
        * 0. See if another loop must be done on the previous Multicast-ID
        *    in case exactly two tables were found different from two others tables
        */

        if (is_another_loop_to_do == TRUE)
        {
          is_another_loop_to_do = FALSE;
        }
        else
        {
          nof_loops_done = 0;
        }

        /* get the result from the 4 MC tables */
        for (mc_tbl_ndx = 0; mc_tbl_ndx < tables_num; mc_tbl_ndx++)
        {

            rc = READ_RTP_MCTm(unit, mc_tbl_ndx, MEM_BLOCK_ANY, multicast_id, &(data[mc_tbl_ndx][0]));
            SOCDNX_IF_ERR_EXIT(rc);
        }

        /*  
        * Compare the results 
        *    The table 0 is the reference, the first table found different is the second reference
        *    If a third table is found different, do not synchronize
        *    Synchronize only if a single table is different (either 0, or the first found different to 0)
        */
        nof_diff_tbls = 0;
        is_different_value_found = FALSE;
        for (mc_tbl_ndx = 1; mc_tbl_ndx < tables_num; mc_tbl_ndx++)
        {
            is_not_equal = sal_memcmp(&(data[0][0]), &(data[mc_tbl_ndx][0]), (sizeof(SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32)*sizeof(uint32)));
            if(is_not_equal) {
                nof_diff_tbls ++;
                if (is_different_value_found == FALSE) {
                    is_different_value_found = TRUE;
                    first_diff_table_ndx = mc_tbl_ndx;
                } 
                else {
                    is_not_equal = sal_memcmp(&(data[mc_tbl_ndx][0]), &(data[first_diff_table_ndx][0]), (sizeof(SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32)*sizeof(uint32)));
                    if (is_not_equal) {
                        /* 
                        * 3 different values at all - no sync in this loop
                        */
                        is_3_diff_tbl = TRUE;
                    }
                }
            } 
        }
    

        /* 
        * 3. Copy the table if needed 
        */
        if ((is_3_diff_tbl == TRUE) || (nof_diff_tbls == (tables_num/2)))
        {
            is_3_diff_tbl = FALSE;

            /* 
            * Retry a limited number of time 
            */
            if (nof_loops_done < (tables_num + 1)) {
                is_another_loop_to_do = TRUE;
                nof_loops_done ++;
            } 
            else {
                multicast_tables_info -> multicast_tables_status = FE1600_INT_MULTICAST_TABLES_TOO_MANY_TRIALS_WITH_INCORRECT_DIFFERENT_TABLES;
                flag_end = 1;;
            }
        } 
        else if (nof_diff_tbls > 0) {
            if (nof_diff_tbls == 1) {
                /* 
                * Take Multicast-Table 0 as the reference
                */
                multicast_tables_info -> multicast_tables_status = FE1600_INT_MULTICAST_TABLES_SHOULD_BE_CORRECTED;
                multicast_tables_info -> valid_table_id = 0;
            } 
            else /* nof_diff_tbls = 3 */ {
                /* 
                * Take Multicast-Table 1 as the reference 
                * (the first one to be different from 0)
                */
                multicast_tables_info -> multicast_tables_status = FE1600_INT_MULTICAST_TABLES_SHOULD_BE_CORRECTED;
                multicast_tables_info -> valid_table_id = first_diff_table_ndx;
            }
            flag_end = 1;
        }
        else {
            /* 
            * Take Multicast-Table 0 as the reference
            */
            multicast_tables_info -> multicast_tables_status = FE1600_INT_MULTICAST_TABLES_EQUAL;
            multicast_tables_info -> valid_table_id = 0;
            flag_end = 1;
        }
  }

exit:
    SOCDNX_FUNC_RETURN;

}

/* standard utilities */

int fe1600_interrupt_fmac_link_get(
    int unit,
    int fmac_block_instance,
    int bit_in_field,
    int *p_link    )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_link);

    *p_link = fmac_block_instance * FE1600_INT_BIT_PER_MAC_INT_TYPE  + (bit_in_field % FE1600_INT_BIT_PER_MAC_INT_TYPE);

exit:
    SOCDNX_FUNC_RETURN;
}

int fe1600_interrupt_fsrd_link_get(int unit, 
                                int block_instance, 
                                int bit_in_field, 
                                int reg_index, 
                                int *p_link)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_link);

    *p_link = block_instance * FE1600_INT_BIT_PER_FSRD_QUAD_INT_TYPE * FE1600_INT_REGS_PER_FSRD_QUAD_INT_TYPE + reg_index * FE1600_INT_BIT_PER_FSRD_QUAD_INT_TYPE  + bit_in_field ;
    assert(*p_link >= 0 && *p_link < SOC_FE1600_NOF_LINKS);

exit:
    SOCDNX_FUNC_RETURN;  
}


int fe1600_interrupt_dch_links_get(
    int unit,
    int dch_block_instance,
    fe1600_links *links)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(links);

    links ->first_link =  dch_block_instance * FE1600_INTERRUPT_LINKS_NUM_PER_DCH_BLOCK ;
    links ->last_link  =  links -> first_link + ( FE1600_INTERRUPT_LINKS_NUM_PER_DCH_BLOCK - 1);

exit:
    SOCDNX_FUNC_RETURN;

}

/* conversion memory address according to cmic requirement */
int
fe1600_interrupt_convert_memory_address(
    int unit,
    uint32 block,
    uint32 address,
    uint32* converted_address
)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(converted_address);

    if((block > FE1600_INTERRUPT_MAX_BLOCK_NUM ) || (address > FE1600_INTERRUPT_MAX_MEMORY_ADDRESS)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOC_MSG("invalid parameter" )));

    }

    *converted_address = ((((address & 0xFFFFF) | ((address & 0x3F00000 ) << 4))) | (((block << 20) | (((block << 20) & 0x3000000) << 6)) & 0xC0F00000));
 
exit:
    SOCDNX_FUNC_RETURN;
}

int
fe1600_interrupt_mct_addr_to_array_element_and_index(
    int unit,
    uint32 address,
    int* numels,
    int* index
)
{
    int tables_num;
    int max_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(numels);
    SOCDNX_NULL_CHECK(index);

    /* get tables number */
#if 1
    tables_num = FE1600_INTERRUPT_DEFAULT_TABLES_NUM;
#endif

    max_index = SOC_MEM_INFO(unit, RTP_MCTm).base * (FE1600_INTERRUPT_DEFAULT_TABLES_NUM / tables_num);
    *numels = (address - SOC_MEM_INFO(unit, RTP_MCTm).base) / max_index;
    *index  = (address - (SOC_MEM_INFO(unit, RTP_MCTm).base + SOC_MEM_INFO(unit, RTP_MCTm).index_min)) % tables_num;

exit:
    SOCDNX_FUNC_RETURN;    
}


int
fe1600_interrupt_print_info(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    int recurring_action,
    fe1600_int_corr_act_type corr_act,
    char *special_msg)
{
    soc_interrupt_db_t* interrupt;
    uint32 flags; 

    char cur_special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    char cur_corr_act_msg[FE1600_INTERRUPT_COR_ACT_MSDG_SIZE];
    char print_msg[FE1600_INTERRUPT_PRINT_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;
 
    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe1600_interrupt]);

    BCMDNX_IF_ERR_EXIT(soc_interrupt_flags_get(unit, en_fe1600_interrupt, &flags));

    if (special_msg == NULL) {
        sal_snprintf(cur_special_msg, sizeof(cur_special_msg), "None");
    } else {
        sal_strncpy(cur_special_msg, special_msg, sizeof(cur_special_msg) - 1);
    }
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = FE1600_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
        case FE1600_INT_CORR_ACT_NONE:
            sal_sprintf(cur_corr_act_msg, "None");
            break;
        case FE1600_INT_CORR_ACT_FORCE:
            sal_sprintf(cur_corr_act_msg, "Force");
            break;
        case FE1600_INT_CORR_ACT_HARD_RESET:
            sal_sprintf(cur_corr_act_msg, "Hard Reset");
            break;
        case FE1600_INT_CORR_ACT_SHUTDOWN_LINKS:
            sal_sprintf(cur_corr_act_msg, "Shutdown Links");
            break;
        case FE1600_INT_CORR_ACT_PRINT:
            sal_sprintf(cur_corr_act_msg, "Print");
            break;
        case FE1600_INT_CORR_ACT_MC_RTP_CORRECT:
            sal_sprintf(cur_corr_act_msg, "MC RTP Correct");
            break;
        case FE1600_INT_CORR_ACT_UC_RTP_CORRECT:
            sal_sprintf(cur_corr_act_msg, "UC RTP Correct");
            break;
         case FE1600_INT_CORR_ACT_SHADOW:
            sal_sprintf(cur_corr_act_msg, "SHADOW Correct");
            break;
        case FE1600_INT_CORR_ACT_RX_LOS_HANDLE:
            sal_sprintf(cur_corr_act_msg, "RX los Correct");
            break;

        default:
            sal_sprintf(cur_corr_act_msg, "Unknown");
    }

    /* prepare string for print */
#if defined(SOC_NO_NAMES)
    sal_sprintf(print_msg,"id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", 
                en_fe1600_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#else
    sal_sprintf(print_msg,"name=%s, id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", 
                interrupt->name, en_fe1600_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#endif

    /* Print per interrupt mechanism */
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE)) {
        /* print to screen */
        LOG_ERROR(BSL_LS_BCM_INTR,
                  (BSL_META_U(unit,
                              "%s"), print_msg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME


