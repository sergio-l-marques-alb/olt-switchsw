/* 
 * $Id: diag.h,v 1.4 Broadcom SDK $
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
 * File:        diag.h
 * Purpose:     Device diagnostics commands.
 *
 */


#ifndef _DIAG_H_INCLUDED_
#define _DIAG_H_INCLUDED_

#include <soc/chip.h>
#include <appl/diag/diag.h>


/* 
 *DNX Diag pack infrastructure 
 */
#define DNX_DIAG_ID_UNAVAIL     (-1)

typedef cmd_result_t (*diag_dnx_action_func_ptr_t)(int unit, args_t* a);
typedef void (*diag_dnx_usage_func_ptr_t)(int unit);
typedef int (*diag_dnx_diag_id_supported_func_ptr_t)(int unit, int diag_id);

typedef struct diag_dnx_table_s {
    char*                       module_char;    /*Command name*/
    diag_dnx_action_func_ptr_t  action_func;    /*Callback to command fuction*/
    diag_dnx_usage_func_ptr_t   usage_func;     /*Callback to command usage function*/
    int                         diag_id;        /*diag id - used in order to make sure this command is supported by the device*/
} diag_dnx_table_t;

/*
 * Function: 
 *      diag_dnx_usage_print 
 * Purpose: 
 *      print the usage of all the supported commands by this unit. 
 * Parameters: 
 *      unit            - (IN) Unit number.
 *      diag_pack       - (IN) Diag pack table.
 *      diag_id_support - (IN) Callback to a function that checks if diag_id supported.
 */
void diag_dnx_usage_print(int unit,
                          const diag_dnx_table_t *diag_pack, 
                          diag_dnx_diag_id_supported_func_ptr_t diag_id_support);

/*
 * Function: 
 *      diag_dnx_usage_print 
 * Purpose: 
 *      Dispatch according to command name and if the diag_id is supported. 
 * Parameters: 
 *      unit            - (IN) Unit number.
 *      diag_pack       - (IN) Diag pack table.
 *      diag_id_support - (IN) Callback to a function that checks if diag_id supported.
 *      cmd_name        - (IN) Command full name.
 *      args            - (IN) args required by the command.
 */
cmd_result_t diag_dnx_dispatch(int unit,
                  const diag_dnx_table_t *diag_pack,
                  diag_dnx_diag_id_supported_func_ptr_t diag_id_support,
                  char *cmd_name,
                  args_t *args);

int diag_dcmn_phy_measure_port(int unit, bcm_port_t port, bcm_stat_val_t type,uint32* rate_int, uint32* rate_remainder);

#define DIAG_COUNTERS_F_ARADPLUS_ONLY 0x1

typedef struct diag_counter_data_s {
  char*             reg_name;
  soc_block_type_t  block;
  soc_reg_t         reg;
  unsigned          reg_index; /* register index, should be 0 when not a register array. */
  soc_field_t       cnt_field;
  soc_field_t       overflow_field;
  uint32            flags;
} diag_counter_data_t;

typedef enum jericho_graphic_counters {

    EGQ_EHP_DISCARD_PACKET_COUNTER, 	
    EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER, 	
    EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER, 	
    EGQ_EHP_UNICAST_PACKET_COUNTER, 	
    EGQ_FQP_PACKET_COUNTER, 	
    EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER, 	
    EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER, 	
    EGQ_PQP_MULTICAST_BYTES_COUNTER, 	
    EGQ_PQP_MULTICAST_PACKET_COUNTER, 	
    EGQ_PQP_UNICAST_BYTES_COUNTER, 	
    EGQ_PQP_UNICAST_PACKET_COUNTER, 
    EPNI_EPE_BYTES_COUNTER, 	
    EPNI_EPE_DISCARDED_PACKETS_COUNTER, 	
    EPNI_EPE_PACKET_COUNTER,
    FDA_EGQ_N_CELLS_IN_CNT_IPT0,
    FDA_EGQ_N_CELLS_IN_CNT_IPT1,
    FDA_EGQ_N_CELLS_IN_CNT_MESHMC0,
    FDA_EGQ_N_CELLS_IN_CNT_MESHMC1,
    FDA_EGQ_N_CELLS_IN_CNT_TDM0,
    FDA_EGQ_N_CELLS_IN_CNT_TDM1,
    FDA_EGQ_N_CELLS_OUT_CNT_IPT0,
    FDA_EGQ_N_CELLS_OUT_CNT_IPT1,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_10,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_11,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_00,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_01,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_20,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_21,
    FDA_EGQ_N_CELLS_OUT_CNT_TDM0,
    FDA_EGQ_N_CELLS_OUT_CNT_TDM1,
    FDA_EGQ_N_CELLS_OUT_CNT_MESHMC0,
    FDA_EGQ_N_CELLS_OUT_CNT_MESHMC1,
    FDA_EGQ_N_FABFIF_0_PRIO_1_DROP_CNT0,
    FDA_EGQ_N_FABFIF_0_PRIO_1_DROP_CNT1,
    FDA_EGQ_N_FABFIF_0_PRIO_0_DROP_CNT0,
    FDA_EGQ_N_FABFIF_0_PRIO_0_DROP_CNT1,
    FDA_EGQ_N_FABFIF_0_PRIO_2_DROP_CNT0,
    FDA_EGQ_N_FABFIF_0_PRIO_2_DROP_CNT1,
    FDA_EGQ_N_FABFIF_1_PRIO_0_DROP_CNT0,
    FDA_EGQ_N_FABFIF_1_PRIO_0_DROP_CNT1,
    FDA_EGQ_N_FABFIF_1_PRIO_1_DROP_CNT0,
    FDA_EGQ_N_FABFIF_1_PRIO_1_DROP_CNT1,
    FDA_EGQ_N_FABFIF_1_PRIO_2_DROP_CNT0,
    FDA_EGQ_N_FABFIF_1_PRIO_2_DROP_CNT1,
    FDA_EGQ_N_FABFIF_2_PRIO_1_DROP_CNT0,
    FDA_EGQ_N_FABFIF_2_PRIO_1_DROP_CNT1,
    FDA_EGQ_N_FABFIF_2_PRIO_0_DROP_CNT0,
    FDA_EGQ_N_FABFIF_2_PRIO_0_DROP_CNT1,
    FDA_EGQ_N_FABFIF_2_PRIO_2_DROP_CNT0,
    FDA_EGQ_N_FABFIF_2_PRIO_2_DROP_CNT1,
    FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT0,
    FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT1,
    FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT0,
    FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT1,
    FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT0,
    FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT1,
    FDA_EGQ_N_TDM_OVF_DROP_CNT0,
    FDA_EGQ_N_TDM_OVF_DROP_CNT1,
    FDA_FAB_36_SCH_CELLS_IN_CNT_P_1,
    FDA_FAB_36_SCH_CELLS_IN_CNT_P_2,
    FDA_FAB_36_SCH_CELLS_IN_CNT_P_3,
    FDR_CELL_IN_CNT,
    FDR_P_1_CELL_IN_CNT,
    FDR_P_2_CELL_IN_CNT,
    FDR_P_3_CELL_IN_CNT,
    FDT_DESC_CELL_CNT,
    FDT_IRE_DESC_CELL_CNT,
    FDT_DATA_CELL_CNT,
    IPT_CRC_ERR_CNT,
    IPT_EGQ_0_PKT_CNT,
    IPT_EGQ_1_PKT_CNT,
    IPT_ENQ_0_PKT_CNT,
    IPT_ENQ_1_PKT_CNT,
    IPT_FDT_0_PKT_CNT,
    IPT_FDT_1_PKT_CNT,
    IQM_DEQUEUE_PACKET_COUNTER,
    IQM_ENQUEUE_PACKET_COUNTER,
    IQM_QUEUE_DELETED_PACKET_COUNTER,
    IRE_CPU_PACKET_COUNTER,
    IRE_NIF_N_PACKET_COUNTER0,
    IRE_NIF_N_PACKET_COUNTER1,
    IRE_OAMP_PACKET_COUNTER,
    IRE_OLP_PACKET_COUNTER,
    IRE_RCY_N_PACKET_COUNTER0,
    IRE_RCY_N_PACKET_COUNTER1,
    IRE_RCY_N_PACKET_COUNTER2,
    IRE_RCY_N_PACKET_COUNTER3,
    NBIH_RX_TOTAL_BYTE_COUNTER,
    NBIH_RX_TOTAL_PKT_COUNTER,
    NBIH_RX_NUM_TOTAL_DROPPED_EOPS,
    NBIH_TX_TOTAL_BYTE_COUNTER,
    NBIH_TX_TOTAL_PKT_COUNTER,
    NBIL_RX_NUM_TOTAL_DROPPED_EOPS,
    END

} jericho_graphic_counters_e;

#endif /*_DIAG_H_INCLUDED_*/
