/*
 * $Id: soc_dcmn_intr_handler.h, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implement header for soc interrupt handler.
 */

#ifndef _DCMN_INTR_HANDLER_H_
#define _DCMN_INTR_HANDLER_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/defs.h>
#include <soc/intr.h>
#include <soc/dcmn/error.h>

/*************
 * FUNCTIONS *
 *************/

typedef enum dcmn_memory_dc_e
{
    DCMN_INVALID_DC=-1,
    DCMN_ECC_ECC2B_DC,
    DCMN_ECC_ECC1B_DC,
    DCMN_P_1_ECC_ECC2B_DC,
    DCMN_P_1_ECC_ECC1B_DC,
    DCMN_P_2_ECC_ECC2B_DC,
    DCMN_P_2_ECC_ECC1B_DC,
    DCMN_P_3_ECC_ECC2B_DC,
    DCMN_P_3_ECC_ECC1B_DC,
    DCMN_ECC_PARITY_DC

} dcmn_memory_dc_t;

typedef enum dcmn_intr_reset_callback_e
{
    ASIC_RESET_NONE,
    ASIC_PON_RESET,
    ASIC_HARD_RESET,
    ASIC_SOFT_RESET_BLOCKS,
    ASIC_SOFT_RESET_BLOCKS_FABRIC
} dcmn_intr_reset_callback_t;


typedef struct dcmn_block_control_info_s
{
    int ecc1_int;
    int ecc2_int;
    int parity_int;
    soc_reg_t gmo_reg;   /*  global mem option reg*/
    soc_reg_t ecc1_monitor_mem_reg;   /*  global mem option reg*/

} dcmn_block_control_info_t;

typedef struct dcmn_block_control_info_main_s
{
    dcmn_block_control_info_t *map;
    dcmn_block_control_info_t *config;

} dcmn_block_control_info_main_t;


void dcmn_collect_blocks_control_info(int unit,dcmn_block_control_info_t *map, dcmn_block_control_info_t *config);
 
int dcmn_disable_block_ecc_check(int unit, int interrupt_id, soc_mem_t mem, int copyno, soc_reg_above_64_val_t value, soc_reg_above_64_val_t orig_value);

int dcmn_num_of_ints(int unit);

void dcmn_intr_switch_event_cb(
    int unit, 
    soc_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata);
int dcmn_intr_add_handler(int unit, int en_inter, int occurrences, int timeCycle, soc_handle_interrupt_func inter_action, soc_handle_interrupt_func inter_recurring_action);
int dcmn_intr_add_handler_ext(int unit, int en_inter, int occurrences, int timeCycle, soc_handle_interrupt_func inter_action, soc_handle_interrupt_func inter_recurring_action,char *msg);
int dcmn_intr_add_clear_ext(int unit,int en_inter,clear_func inter_action);
int dcmn_intr_handler_deinit(int unit);
int dcmn_intr_handler_short_init(int unit);
int dcmn_intr_handler_init(int unit);
int dcmn_get_cnt_reg_values(int unit,dcmn_memory_dc_t type,soc_reg_t cnt_reg,int copyno, uint32 *cntf,uint32 *cnt_overflowf, uint32 *addrf,uint32 *addr_validf);
dcmn_memory_dc_t get_cnt_reg_type(int unit,soc_reg_t cnt_reg);
int dcmn_blktype_to_index(soc_block_t blktype);

int dcmn_get_ser_entry_from_cache(int unit,  soc_mem_t mem, int copyno, int array_index, int index, uint32 *data_entr);
int dcmn_int_name_to_id(int unit, char *name);
int dcmn_fdt_unreachdest_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_emptysdqcqwrite_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_emptyddqcqwrite_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_queueentereddel_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_creditlost_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_creditoverflow_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_sdqcqoverflow_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_ddqcqoverflow_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_pushqueueactive_interrupt_clear(int unit, int block_instance, int interrupt_id);

int dcmn_interrupt_handle_IHBInvalidDestinationValid(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_IHBKapsDirectLookupErr(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_IHBIsemErrorTableCoherency(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_IHBIsemErrorCamTableFull(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IHBIsemErrorDeleteUnknownKey(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IHBIsemErrorReachedMaxEntryLimit(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IHBIsemWarningInsertedExisting(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IHBIsemManagementUnitFailureValid(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_MRPSMcdaWrap(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_MRPSMcdbWrap(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_MTRPS_EMMcdaWrap(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_MTRPS_EMMcdbWrap(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_SCHActFlowBadParams(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_SCHShpFlowBadParams(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHRestartFlowEvent(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHSmpThrowSclMsg(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHSclGroupChanged(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHSmpFabricMsgsFifoFull(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHSmpBadMsg(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_FDTBurstTooLarge(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACLOS_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACRxLostOfSync(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACOOF_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACDecErr_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACTransmitErr_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_PPDB_BMactErrorTableCoherency_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_PPDB_BMactManagementUnitFailureValid_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_FCRCpuCntCellFne(int unit,int block_instance,uint32 en_interrupt,char * msg);

int dcmn_interrupt_handle_IPTCrcErrPkt_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTCrcDeletedBuffersFifoFull_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTCrcDeletedBuffersFifoNotEmpty_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTOcbOnlyDataOverflow_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTOcbMix0DataOverflow_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTOcbMix1DataOverflow_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_OAMPRxStatsDone(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_OAMPRmapemManagementUnitFailureValid(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_IDRErrorMiniMulticast0ExtOverflow(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IDRErrorFullMulticast0ExtOverflow(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IDRErrorMiniMulticast1ExtOverflow(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IDRErrorFullMulticast1ExtOverflow(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_IRRErrIsMaxReplication(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IRRErrIsReplicationEmpty(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IRRErrMaxReplication(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IRRErrReplicationEmpty(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_IQMFreeBdbOvf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMFreeBdbUnf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMDeqComEmptyQ(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMFullUscntOvf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMMiniUscntOvf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMUpdtFifoOvf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMIngressReset(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMFreeBdbProtErr(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IngressSoftReset(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FDRIFMFifoOverflow_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_fsrd_fsrd_SyncStatusChanged(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_fsrd_fsrd_TxpllLockChanged(int unit, int block_instance, uint32 en_interrupt, char *msg);
#endif /*_DCMN_INTR_HANDLER_H_ */
