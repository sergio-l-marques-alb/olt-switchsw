/** \file jer2_ingress_packet_queuing.c
 *
 * Functions for handling Ingress Packet Queuing (IPQ).
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INGRESS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/JER2/jer2_ingress_packet_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/legacy/TMC/tmc_api_general.h>
#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>
#include <soc/dnx/legacy/TMC/tmc_api_egr_queuing.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/mbcm.h>
/* } */

/* 
 * DEFINEs
 * { 
 */

/** Invalid queue definition */
#define JER2_IPQ_INVALID_QUEUE(unit)  (dnx_data_ipq.queues.nof_queues_get(unit) - 1)

/** Number of iterations to check for empty queue */
#define JER2_IPQ_QUEUE_EMPTY_ITERATIONS (5)

/* 
 * If destination is system port then is_flow_or_multicast==0
 * If is_flow then is_flow_or_multicast==1
 * If is_multicast then is_flow_or_multicast==2
 *  
 * The traffic class is mapped according to packet's source and destination for pipe 0/1.
 * Table is accessed with the following key key_msb,ingress_shape,orig_tc, where:
 * key_msb (4 bits):
 *    - 2'b00,TC-Mapping-Profile if Destination is System-Port-ID
 *    - 2'b01,Flow-Profile if Destination is Flow
 *    - 2'b10,2'b00 if Destination is fabric or egress multicast
 * orig_tc (3 bits) is the original traffic class
 */
#define _JER2_IPQ_TC_CLS_OFF 0
#define _JER2_IPQ_TC_CLS_NOF_BITS 3
#define _JER2_IPQ_TC_CLS_MASK ((1 << _JER2_IPQ_TC_CLS_NOF_BITS) - 1)

#define _JER2_IPQ_TC_SHAPE_OFF(unit) (_JER2_IPQ_TC_CLS_NOF_BITS + _JER2_IPQ_TC_CLS_OFF)

#define _JER2_IPQ_TC_PROF_OFF(unit) (_JER2_IPQ_TC_SHAPE_OFF(unit))
#define _JER2_IPQ_TC_PROF_NOF_BITS 2
#define _JER2_IPQ_TC_PROF_MASK ((1 << _JER2_IPQ_TC_PROF_NOF_BITS) - 1)

#define _JER2_IPQ_TC_FLOW_OFF(unit) (_JER2_IPQ_TC_PROF_NOF_BITS + _JER2_IPQ_TC_PROF_OFF(unit))
#define _JER2_IPQ_TC_FLOW_NOF_BITS 2
#define _JER2_IPQ_TC_FLOW_MASK ((1 << _JER2_IPQ_TC_FLOW_NOF_BITS) - 1)

#define JER2_IPQ_TC_ENTRY(unit, is_flow_or_multicast, profile_ndx, tr_cls_ndx) \
    (\
    ((is_flow_or_multicast & _JER2_IPQ_TC_FLOW_MASK)        << _JER2_IPQ_TC_FLOW_OFF(unit))   | \
    ((profile_ndx          & _JER2_IPQ_TC_PROF_MASK)        << _JER2_IPQ_TC_PROF_OFF(unit))   | \
    ((tr_cls_ndx           & _JER2_IPQ_TC_CLS_MASK)         << _JER2_IPQ_TC_CLS_OFF)            \
    )

/* 
 * } 
 */

/*************
 * FUNCTIONS *
 *************/
/* { */


/*
 * See .h file.
 */
int
  jer2_ipq_init(
    DNX_SAND_IN  int unit
  ) 
{
    uint64 reg64;
    soc_reg_above_64_val_t reg_above_64;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg64);
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    /* 
     * Set general IPS configurations 
     */
    SHR_IF_ERR_EXIT(soc_reg_get(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, SOC_CORE_ALL, 0, &reg64));
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISCARD_ALL_QM_MSGf, 0);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISCARD_ALL_CRDTf, 0);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DISABLE_STATUS_MSG_GENf, 0);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, DIS_DEQ_CMDSf, 0);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, UPDATE_MAX_QSIZE_FROM_LOCALf, 1);
    soc_reg64_field32_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg64, SEND_FSM_FOR_BIGGER_MAX_QUEUE_SIZEf, 1);
    SHR_IF_ERR_EXIT(soc_reg_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, SOC_CORE_ALL, 0, reg64));

    /* configure credit deq command to support full rate */
    
    SHR_IF_ERR_EXIT(READ_IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr(unit, SOC_CORE_ALL, reg_above_64));
    soc_reg_above_64_field32_set(unit, IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr, reg_above_64, DEQ_CMD_SRAM_TO_FABRIC_CRDT_LFSR_TH_Af, 0x70);
    soc_reg_above_64_field32_set(unit, IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr, reg_above_64, DEQ_CMD_SRAM_TO_FABRIC_CRDT_LFSR_MASK_Af, 0x3);
    soc_reg_above_64_field32_set(unit, IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr, reg_above_64, DEQ_CMD_SRAM_TO_FABRIC_CRDT_LFSR_TH_Bf, 0x70);
    soc_reg_above_64_field32_set(unit, IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr, reg_above_64, DEQ_CMD_SRAM_TO_FABRIC_CRDT_LFSR_MASK_Bf, 0x3);
    soc_reg_above_64_field32_set(unit, IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr, reg_above_64, DEQ_CMD_SRAM_TO_FABRIC_CRDT_LFSR_TH_Cf, 0x70);
    soc_reg_above_64_field32_set(unit, IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr, reg_above_64, DEQ_CMD_SRAM_TO_FABRIC_CRDT_LFSR_MASK_Cf, 0x3);
    soc_reg_above_64_field32_set(unit, IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr, reg_above_64, DEQ_CMD_SRAM_TO_FABRIC_CRDT_LFSR_TH_Df, 0x70);
    soc_reg_above_64_field32_set(unit, IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr, reg_above_64, DEQ_CMD_SRAM_TO_FABRIC_CRDT_LFSR_MASK_Df, 0x3);
    SHR_IF_ERR_EXIT(WRITE_IPS_DEQ_CMD_SRAM_TO_FABRIC_CRDT_ADJUSTr(unit, SOC_CORE_ALL, reg_above_64));

exit:
    SHR_FUNC_EXIT;
}

/* } */

#undef BSL_LOG_MODULE

