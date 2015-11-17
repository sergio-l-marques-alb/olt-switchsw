/*
 * $Id: interrupt_handler_corr_act_func.h,v 1.16 Broadcom SDK $
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
 * File:    interrupt_handler_corr_act_func.h
 * Purpose:    Implement Corrective Action function for ARAD interrupts.
 */

#ifndef _DPP_INTERRUPT_HANDLER_CORR_ACT_FUNC_H_
#define _DPP_INTERRUPT_HANDLER_CORR_ACT_FUNC_H_

#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_tcam.h>

/* 
 *  Defines
 */

#define ARAD_INT_FEC_UNCORRECTABLE_THRESHOLD        1
#define ARAD_INT_CRC_MASK                           0xffffffff
#define ARAD_INT_LINK_INVOLVED                      1
#define ARAD_INT_LINK_SHUTDOWN                      0
#define ARAD_INTERRUPT_SPECIAL_MSG_SIZE             256
#define ARAD_INTERRUPT_COR_ACT_MSDG_SIZE            36
#define ARAD_INTERRUPT_PRINT_MSG_SIZE               512
#define ARAD_INT_BIT_PER_MAC_INT_TYPE               4
#define ARAD_INT_BIT_PER_FSRD_QUAD_INT_TYPE         4
#define ARAD_INT_REGS_PER_FSRD_QUAD_INT_TYPE        3
#define ARAD_INTERRUPT_MESSAGE_FOR_CONVERSION       32
#define ARAD_INT_FEC_UNCORRECTABLE_MASK             0x0000ffff
#define ARAD_INTERRUPT_FDT_BLOCK                    6
#define ARAD_INTERRUPT_EPNI_BLOCK                   3                    
#define ARAD_INTERRUPT_EGQ_BLOCK                    2
#define ARAD_INTERRUPT_IPT_BLOCK                    37
#define ARAD_INTERRUPT_IPS_BLOCK                    36
#define ARAD_INTERRUPT_IQM_BLOCK                    38
#define ARAD_INTERRUPT_CFC_BLOCK                    1
#define ARAD_INTERRUPT_NBI_BLOCK                    26
#define ARAD_INT_FDR_NUM_OF_LINKS_IN_LINK_UP_STATUS_REG     18
#define ARAD_INTERRUPT_SCH_BLOCK                            0x37
#define ARAD_INTERRUPT_CRPS_BLOCK                           0x23 
#define ARAD_INTERRUPT_IHP_BLOCK                            0x34                    
#define ARAD_INTERRUPT_IHB_BLOCK                            0x35
#define ARAD_INTERRUPT_OAMP_BLOCK                           56
#define ARAD_INTERRUPT_IDR_BLOCK                            0x32
#define ARAD_INTERRUPT_IRR_BLOCK                            0x33
#define ARAD_INTERRUPT_IRE_BLOCK                            0x31
#define ARAD_INTERRUPT_OLP_BLOCK                            0x30                   
#define ARAD_INTERRUPT_MMU_BLOCK                            0x16
#define ARAD_INTERRUPT_FMAC_FIRST_BLOCK                     0xa
#define ARAD_INTERRUPT_FCR_BLOCK                            0x4

/* Dram Delete buffer corrective action */
#define ARAD_INTERRUPT_IPT_MAX_CRC_DELETED_FIFO_DEPTH 16

/* FCT unreachable destination cell type */
#define ARAD_INTERRUPTS_FCT_UNREACHABLE_DEST_CELL_TYPE_CREDIT 1
/* 
 *  Structs
 */

/* 
 *  Enums
 */
typedef enum {
    ARAD_INT_CORR_ACT_NONE = 0x0,
    ARAD_INT_CORR_ACT_FORCE = 0x1,
    ARAD_INT_CORR_ACT_HARD_RESET = 0x2,
    ARAD_INT_CORR_ACT_SHUTDOWN_FBR_LINKS = 0x3,
    ARAD_INT_CORR_ACT_PRINT = 0x4,
    ARAD_INT_CORR_ACT_SHADOW = 0x5,
    ARAD_INT_CORR_ACT_REPROGRAM_RESOURCE = 0x6,
    ARAD_INT_CORR_ACT_CONFIG_DRAM = 0x7,
    ARAD_INT_CORR_ACT_INGRESS_HARD_RESET = 0x8,
    ARAD_INT_CORR_ACT_ECC_1B_FIX = 0x9,
    ARAD_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO = 0xA,
    ARAD_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO = 0xB,
    ARAD_INT_CORR_ACT_RX_LOS_HANDLE = 0xC,
    ARAD_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION = 0xD,
    ARAD_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO = 0xE,
    ARAD_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC = 0xF,
    ARAD_INT_CORR_ACT_RTP_LINK_MASK_CHANGE = 0x10,
    ARAD_INT_CORR_ACT_CLEAR_CHECK = 0x11,
    ARAD_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB = 0x12
} arad_int_corr_act_type;

typedef struct {
    soc_mem_t mem;
    unsigned array_index;
    int copyno;
    int min_index;
    int max_index;
} arad_interrupt_mem_err_info;

/* 
 *  Structs
  */
typedef struct
{
  int first_link;
  int last_link;
}arad_links ;

typedef struct {
    uint32 links_bitmap[4];
} arad_interrupt_links_t;



/* 
 *  Functions
 */

/* Corrective Action main function */
int arad_interrupt_handles_corrective_action(int unit, int block_instance, arad_interrupt_type interrupt_id, char *msg, arad_int_corr_act_type corr_act, void *param1, void *param2);

/*
 *  Corrective Action functions    
 */
int arad_interrupt_handles_corrective_action_do_nothing (int unit, int block_instance, arad_interrupt_type interrupt_id, char *msg);
int arad_interrupt_handles_corrective_action_shutdown_link(int unit, int block_instance, arad_interrupt_type interrupt_id,uint8* array_links,char* msg);
int arad_interrupt_handles_corrective_action_hard_reset(int unit,int block_instance,arad_interrupt_type interrupt_id,char *msg);
int arad_interrupt_handles_corrective_action_force(int unit, int block_instance, arad_interrupt_type interrupt_id, char *msg);
int arad_interrupt_handles_corrective_action_print(int unit, int block_instance, arad_interrupt_type interrupt_id, char* msg_print, char* msg);
int arad_interrupt_handles_corrective_action_ingress_hard_reset(int unit,int block_instance,arad_interrupt_type interrupt_id,char *msg);
int arad_interrupt_handles_corrective_action_handle_crc_del_buf_fifo(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char* msg);
int arad_interrupt_handles_corrective_action_handle_oamp_event_fifo(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char* msg);
int arad_interrupt_handles_corrective_action_handle_mact_event_fifo(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char* msg);
int arad_interrupt_handles_corrective_action_rtp_link_mask_change(int unit);
int arad_interrupt_handles_corrective_action_tcam_shadow_from_sw_db(int unit, int block_instance, arad_interrupt_type interrupt_id, ARAD_TCAM_LOCATION* location, char* msg);


/*
 *  Corrective Action functions    
 */

/*
*  common function utilities for the same interrupts (Data Collection)
*/

int arad_interrupt_data_collection_for_mac_rxcrcerrn_int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, uint8* array_links, char* special_msg);
int arad_interrupt_data_collection_for_mac_wrongsize_int(int unit,int block_instance, arad_interrupt_type en_arad_interrupt,char* special_msg);
int arad_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(int unit,int block_instance, arad_interrupt_type en_arad_interrupt,char* special_msg ,uint8* array_links);
int arad_interrupt_data_collection_for_mac_oof_int( int unit,int block_instance,arad_interrupt_type en_arad_inter, ARAD_PORT_PCS* p_pcs,char* special_msg);
int arad_interrupt_data_collection_for_recurring_mac_oof_int( int unit, int block_instance, arad_interrupt_type en_arad_interrupt, ARAD_PORT_PCS* p_pcs, arad_int_corr_act_type* p_corrective_action,char* special_msg, uint8* array_links);
int arad_interrupt_data_collection_for_mac_decerr_int( int unit,int block_instance, arad_interrupt_type en_arad_interrupt, int is_recurring, arad_int_corr_act_type* p_corrective_action, char* special_msg, uint8* array_links);
int arad_interrupt_data_collection_for_mac_transmit_err_int( int unit,int block_instance,arad_interrupt_type en_arad_interrupt, char* special_msg, arad_int_corr_act_type* p_corrective_action, uint8* array_links);
int arad_interrupt_data_collection_for_mac_lnklvlagen_int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char* special_msg);
int arad_interrupt_data_collection_for_mac_lnklvlagectxbn_int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char* special_msg);
int arad_interrupt_data_collection_for_mac_los_int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char* special_msg);
int arad_interrupt_data_collection_for_mac_rxlostofsync_int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, soc_port_t *link, char* special_msg);
int arad_interrupt_data_collection_for_fdrprmifmbfob(int unit,int block_instance, arad_int_corr_act_type *corr_act , soc_reg_t reg, soc_field_t field, char* special_msg);
int arad_interrupt_data_collection_for_recuring_action_fdr_alto(int unit,int block_instance,arad_interrupt_type en_arad_interrupt,arad_int_corr_act_type * p_corrective_action,uint8*  array_links,char* special_msg);
int arad_interrupt_data_collection_for_shadowing(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char* special_msg, arad_int_corr_act_type* p_corrective_action, arad_interrupt_mem_err_info* shadow_correct_info);
int arad_interrupt_data_collection_for_ecc_1b(int unit, int block_instance, int en_arad_interrupt, char* special_msg, arad_int_corr_act_type* p_corrective_action, arad_interrupt_mem_err_info* ecc_1b_correct_info);
int arad_interrupt_data_collection_for_nbi_link_status_change(int unit,  int block_instance, int en_arad_interrupt, char *msg, arad_int_corr_act_type *corrective_action, soc_port_t *port);
int arad_interrupt_data_collection_for_recurring_drc_err(int unit, int block_instance, int en_arad_interrupt, char* special_msg, arad_int_corr_act_type* p_corrective_action);
int arad_interrupt_data_collection_for_fctunrchdestevent(int unit, int block_instance, arad_interrupt_type interrupt_id, uint32* fap_id, uint32* cell_type, arad_int_corr_act_type* p_corrective_action);
int arad_interrupt_data_collection_for_fsrd_syncstatuschanged(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, soc_port_t *link, char* special_msg);
int arad_interrupt_data_collection_for_port_mib(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char* special_msg, arad_int_corr_act_type* p_corrective_action);
int arad_interrupt_data_collection_for_ilkn_rx_status_change(int unit, int block_instance, int en_arad_interrupt, char* special_msg, arad_int_corr_act_type *corrective_action, soc_port_t *port);
int arad_interrupt_data_collection_for_ihbtcamprotectionerror(int unit, int block_instance, arad_interrupt_type interrupt_id, ARAD_TCAM_LOCATION *location, arad_int_corr_act_type* corrective_action);

 /*    Standard Function Utilities 
 */
int arad_interrupt_fmac_link_get(int unit,int fmac_block_instance,int bit_in_field, int *p_link);
int arad_interrupt_fdr_links_get(int unit, int arad_block_instance, arad_links *links);

/* print function */
int arad_interrupt_print_info(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, int recurring_action, arad_int_corr_act_type corr_act, char *general_msg);

#endif /* _DPP_INTERRUPT_HANDLER_CORR_ACT_FUNC_H_ */
