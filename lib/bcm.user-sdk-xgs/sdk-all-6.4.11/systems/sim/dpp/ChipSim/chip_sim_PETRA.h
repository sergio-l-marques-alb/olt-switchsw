/* $Id: chip_sim_PETRA.h,v 1.5 Broadcom SDK $
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




#ifndef __CHIP_SIM_PETRA_H_INCLUDED__
/* { */
#define __CHIP_SIM_PETRA_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * INCLUDES  *
 *************/
/* { */
#include "chip_sim.h"
#include "chip_sim_counter.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_indirect.h"
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* table index definitions { */
#define SOC_PETRA_OLP_PGE_MEM_TBL_ID                                          0
#define SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID                                     1
#define SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID                            2
#define SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID                                     3
#define SOC_PETRA_IDR_COMPLETE_PC_TBL_ID                                      4
#define SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID                        5
#define SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID                                     6
#define SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID                                      7
#define SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID                                8
#define SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID                                  9
#define SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID                                     10
#define SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID                                11
#define SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID                                 12
#define SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID                                 13
#define SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID                         14
#define SOC_PETRA_IHP_PORT_INFO_TBL_ID                                        15
#define SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID                                   16
#define SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID                                   17
#define SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID                           18
#define SOC_PETRA_IHP_STATIC_HEADER_TBL_ID                                    19
#define SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID                        20
#define SOC_PETRA_IHP_CID_INFO_TBL_ID                                         21
#define SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID                                  22
#define SOC_PETRA_IHP_STP_TABLE_TBL_ID                                        23
#define SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID                   24
#define SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID                                 25
#define SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID              26
#define SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID                      27
#define SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID                              28
#define SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID                                    29
#define SOC_PETRA_IHP_TOS_TO_TC_TBL_ID                                        30
#define SOC_PETRA_IHP_EVENT_FIFO_TBL_ID                                       31
#define SOC_PETRA_IHP_FEC_TABLE_TBL_ID                                        32
#define SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID                                  33
#define SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID                            34
#define SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID                            35
#define SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID                            36
#define SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID                               37
#define SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID                               38
#define SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID                               39
#define SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID                               40
#define SOC_PETRA_IHP_TOS_TO_COS_TBL_ID                                       41
#define SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID                               42
#define SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID                                  43
#define SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID                                      44
#define SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID                                      45
#define SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID                                      46
#define SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID                                      47
#define SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID                                   48
#define SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID                                    49
#define SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID                                    50
#define SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID                              51
#define SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID                                     52
#define SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID                                     53
#define SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID                                     54
#define SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID                                     55
#define SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID                                     56
#define SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID                                 57
#define SOC_PETRA_IHP_CID_SUBNET_TBL_ID                                       58
#define SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID                  59
#define SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID                                60
#define SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID                                    61
#define SOC_PETRA_IQM_DYNAMIC_TBL_ID                                          62
#define SOC_PETRA_IQM_STATIC_TBL_ID                                           63
#define SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID                        64
#define SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID                    65
#define SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID                            66
#define SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID                           67
#define SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID                 68
#define SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID                69
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID                70
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID                71
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID                72
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID                73
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID                         74
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID                         75
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID                         76
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID                         77
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID                 78
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID                 79
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID                 80
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID                 81
#define SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID        82
#define SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID        83
#define SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID        84
#define SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID        85
#define SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID               86
#define SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID               87
#define SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID               88
#define SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID               89
#define SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID                      90
#define SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID               91
#define SOC_PETRA_IQM_SYSTEM_RED_TBL_ID                                       92
#define SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID                93
#define SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID         94
#define SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID                             95
#define SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID                          96
#define SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID                        97
#define SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID                        98
#define SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID                99
#define SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID            100
#define SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID                 101
#define SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID                 102
#define SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID                           103
#define SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID                                 104
#define SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID                  105
#define SOC_PETRA_DPI_DLL_RAM_TBL_A_ID                                        106
#define SOC_PETRA_DPI_DLL_RAM_TBL_B_ID                                        107
#define SOC_PETRA_DPI_DLL_RAM_TBL_C_ID                                        108
#define SOC_PETRA_DPI_DLL_RAM_TBL_D_ID                                        109
#define SOC_PETRA_DPI_DLL_RAM_TBL_E_ID                                        110
#define SOC_PETRA_DPI_DLL_RAM_TBL_F_ID                                        111
#define SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID       112
#define SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID    113
#define SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID                                     114
#define SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID                                     115
#define SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID                                     116
#define SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID                                     117
#define SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID                                     118
#define SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID                                     119
#define SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID                                     120
#define SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID                                     121
#define SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID                                    122
#define SOC_PETRA_EGQ_RCY_SCM_TBL_ID                                          123
#define SOC_PETRA_EGQ_CPU_SCM_TBL_ID                                          124
#define SOC_PETRA_EGQ_CCM_TBL_ID                                              125
#define SOC_PETRA_EGQ_PMC_TBL_ID                                              126
#define SOC_PETRA_EGQ_CBM_TBL_ID                                              127
#define SOC_PETRA_EGQ_FBM_TBL_ID                                              128
#define SOC_PETRA_EGQ_FDM_TBL_ID                                              129
#define SOC_PETRA_EGQ_DWM_TBL_ID                                              130
#define SOC_PETRA_EGQ_RRDM_TBL_ID                                             131
#define SOC_PETRA_EGQ_RPDM_TBL_ID                                             132
#define SOC_PETRA_EGQ_PCT_TBL_ID                                              133
#define SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID                                       134
#define SOC_PETRA_EGQ_PPCT_TBL_ID                                             135
#define SOC_PETRA_EGQ_STP_TBL_ID                                              136
#define SOC_PETRA_EPNI_ARP_TBL_ID                                             137
#define SOC_PETRA_EPNI_PTT_TBL_ID                                             138
#define SOC_PETRA_EPNI_PCP_ENC_TBL_ID                                         139
#define SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID                              140
#define SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID                               141
#define SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID            142
#define SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID                 143
#define SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID                 144
#define SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID   145
#define SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID   146
#define SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID                147
#define SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID                148
#define SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID                  149
#define SOC_PETRA_SCH_CAL_TBL_ID                                              150
#define SOC_PETRA_SCH_DRM_TBL_ID                                              151
#define SOC_PETRA_SCH_DSM_TBL_ID                                              152
#define SOC_PETRA_SCH_FDMS_TBL_ID                                             153
#define SOC_PETRA_SCH_SHDS_TBL_ID                                             154
#define SOC_PETRA_SCH_SEM_TBL_ID                                              155
#define SOC_PETRA_SCH_FSF_TBL_ID                                              156
#define SOC_PETRA_SCH_FGM_TBL_ID                                              157
#define SOC_PETRA_SCH_SHC_TBL_ID                                              158
#define SOC_PETRA_SCH_SCC_TBL_ID                                              159
#define SOC_PETRA_SCH_SCT_TBL_ID                                              160
#define SOC_PETRA_SCH_FQM_TBL_ID                                              161
#define SOC_PETRA_SCH_FFM_TBL_ID                                              162
#define SOC_PETRA_SCH_TMC_TBL_ID                                              163
#define SOC_PETRA_SCH_PQS_TBL_ID                                              164
#define SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID                                   165
#define SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID                             166
#define SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID                 167
#define SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID                  168
#define SOC_PETRA_IPT_BDQ_TBL_ID                                              169
#define SOC_PETRA_IPT_PCQ_TBL_ID                                              170
#define SOC_PETRA_IPT_SOP_MMU_TBL_ID                                          171
#define SOC_PETRA_IPT_MOP_MMU_TBL_ID                                          172
#define SOC_PETRA_IPT_FDTCTL_TBL_ID                                           173
#define SOC_PETRA_IPT_FDTDATA_TBL_ID                                          174
#define SOC_PETRA_IPT_EGQCTL_TBL_ID                                           175
#define SOC_PETRA_IPT_EGQDATA_TBL_ID                                          176
#define SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID                               177
#define SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID                               178
#define SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID                              179
#define SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID                              180
#define SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID                               181
#define SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID                                      182

/*
 *	Must be the last.
 *  All tables below
 *  are not supported in chipsim
 */
#define SOC_PETRA_TBL_ID_LAST                                                 183

#define SOC_PETRA_IHP_FEC_UC_TBL_ID                                           183
#define SOC_PETRA_IHP_FEC_MC_TBL_ID                                           184
#define SOC_PETRA_IHP_FEC_PROG_PORT_TBL_ID                                    185
#define SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ID                               186
#define SOC_PETRA_QDR_QDR_MEM_TBL_ID                                          187

/* table index definitions } */
/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
extern
  CHIP_SIM_INDIRECT_BLOCK
    Soc_petra_indirect_blocks[];
extern
  CHIP_SIM_COUNTER
    Soc_petra_counters[];
extern
  CHIP_SIM_INTERRUPT
    Soc_petra_interrupts[];
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*****************************************************
*NAME
* soc_petra_indirect_init
*TYPE:
*  PROC
*DATE:
*  10/24/07
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_petra_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
  soc_petra_indirect_init(
  );

void
  soc_petra_initialize_device_values(
    SOC_SAND_OUT uint32   *base_ptr
  );

/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __CHIP_SIM_PETRA_INCLUDED__*/
#endif
