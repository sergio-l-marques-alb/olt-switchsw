/* $Id: petra_tbl_access.h,v 1.8 Broadcom SDK $
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


#ifndef __SOC_PETRA_TBL_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PETRA_TBL_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>

#ifdef LINK_PB_LIBRARIES
/* 
 * soc_pb_tbl_access uses function defined in soc_petra_tbl_access.h, and vice versa. 
 * In order not to restuct the code, we need to include one from the other. 
 * 'coverity[include_recursion : FALSE]' doesn't work. Also added __SOC_PB_TBL_ACCESS_H_INCLUDED__
 * to ignore the issue. 
 */ 
#ifndef __SOC_PB_TBL_ACCESS_H_INCLUDED__ 
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#endif
#endif
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_OLP_PGE_MEM_TBL_ENTRY_SIZE                                                 1
#define SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE                                            1
#define SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ENTRY_SIZE                                   3
#define SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE                                            1
#define SOC_PETRA_IDR_COMPLETE_PC_TBL_ENTRY_SIZE                                             1
#define SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ENTRY_SIZE                               1
#define SOC_PETRA_IRR_ING_EG_REPLICATION_MULTICAST_DB_TBL_ENTRY_SIZE                         3
#define SOC_PETRA_IRR_MIRROR_TABLE_TBL_ENTRY_SIZE                                            1
#define SOC_PETRA_IRR_SNOOP_TABLE_TBL_ENTRY_SIZE                                             1
#define SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE                                         1
#define SOC_PETRA_IRR_GLAG_MAPPING_TBL_ENTRY_SIZE                                            1
#define SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ENTRY_SIZE                                        1
#define SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ENTRY_SIZE                                        1
#define SOC_PETRA_IHP_PORT_INFO_TBL_ENTRY_SIZE                                               2
#define SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ENTRY_SIZE                                  1
#define SOC_PETRA_IHP_STATIC_HEADER_TBL_ENTRY_SIZE                                           3
#define SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ENTRY_SIZE                               1
#define SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ENTRY_SIZE                                           2
#define SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ENTRY_SIZE                                           2
#define SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ENTRY_SIZE                                     3
/* $Id: petra_tbl_access.h,v 1.8 Broadcom SDK $
 *  The biggest, here according to SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ENTRY_SIZE
 */
#define SOC_PETRA_IHP_KEY_PROGRAM_TBL_ENTRY_SIZE                                             2
#define SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ENTRY_SIZE                                            1
#define SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ENTRY_SIZE                                            1
#define SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ENTRY_SIZE                                            1
#define SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ENTRY_SIZE                                            1
#define SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ENTRY_SIZE                                            2
#define SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ENTRY_SIZE                                        3
#define SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ENTRY_SIZE                                       3
#define SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ENTRY_SIZE                                           1
#define SOC_PETRA_IQM_DYNAMIC_TBL_ENTRY_SIZE                                                 2
#define SOC_PETRA_IQM_STATIC_TBL_ENTRY_SIZE                                                  1
#define SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ENTRY_SIZE                               1
#define SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE                           1
#define SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE                                   1
#define SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ENTRY_SIZE                                  1
#define SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ENTRY_SIZE                        2
#define SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE                       3
#define SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE                          1
#define SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE                              3
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE                       1
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE                       1
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE                       1
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE                       1
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE                                2
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE                                2
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE                                2
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE                                2
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE                        1
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE                        1
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE                        1
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE                        1
#define SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE                 2
#define SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE                      3
#define SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE                      1
#define SOC_PETRA_IQM_SYSTEM_RED_TBL_ENTRY_SIZE                                              6
#define SOC_PETRA_QDR_MEM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE                       1
#define SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE                1
#define SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE                                    1
#define SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE                               1
#define SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE                               2
#define SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                       2
#define SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                   1
#define SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE                        2
#define SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                        1
#define SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ENTRY_SIZE                                  1
#define SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE                                        1
#define SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE                         1
#define SOC_PETRA_IPT_BDQ_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_ENTRY_SIZE                                       2
#define SOC_PETRA_IPT_PCQ_TBL_ENTRY_SIZE                                                     2
#define SOC_PETRA_IPT_SOP_MMU_TBL_ENTRY_SIZE                                                 2
#define SOC_PETRA_IPT_MOP_MMU_TBL_ENTRY_SIZE                                                 1
#define SOC_PETRA_IPT_FDTCTL_TBL_ENTRY_SIZE                                                  1
#define SOC_PETRA_IPT_FDTDATA_TBL_ENTRY_SIZE                                                 16
#define SOC_PETRA_IPT_EGQCTL_TBL_ENTRY_SIZE                                                  1
#define SOC_PETRA_IPT_EGQDATA_TBL_ENTRY_SIZE                                                 16
#define SOC_PETRA_DPI_DLL_RAM_TBL_ENTRY_SIZE                                                 2
#define SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ENTRY_SIZE              3
#define SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ENTRY_SIZE           3
#define SOC_PETRA_EGQ_NIF_SCM_TBL_ENTRY_SIZE                                                 1
#define SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ENTRY_SIZE                                           1
#define SOC_PETRA_EGQ_RCY_SCM_TBL_ENTRY_SIZE                                                 1
#define SOC_PETRA_EGQ_CPU_SCM_TBL_ENTRY_SIZE                                                 1
#define SOC_PETRA_EGQ_CCM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_EGQ_PMC_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_EGQ_CBM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_EGQ_FBM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_EGQ_FDM_TBL_ENTRY_SIZE                                                     2
#define SOC_PETRA_EGQ_DWM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_EGQ_RRDM_TBL_ENTRY_SIZE                                                    2
#define SOC_PETRA_EGQ_RPDM_TBL_ENTRY_SIZE                                                    2
#define SOC_PETRA_EGQ_PCT_TBL_ENTRY_SIZE                                                     3
#define SOC_PETRA_EGQ_PPCT_TBL_ENTRY_SIZE                                                    3
#define SOC_PETRA_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE                                              3
#define SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_ENTRY_SIZE                                        3
#define SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_ENTRY_SIZE                                         2
#define SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_ENTRY_SIZE                                         2
#define SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_ENTRY_SIZE                                         2
#define SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE                              2
#define SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE                              2
#define SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE                       2
#define SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE                        2
#define SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE                              2
#define SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE                         2
#define SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE                          2
#define SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE                       2
#define SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE                        2
#define SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE                          2
#define SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE                           2
#define SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE                        2
#define SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE                         2
#define SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE                      2
#define SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ENTRY_SIZE                    2
#define SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ENTRY_SIZE                   1
#define SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE                        1
#define SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE                        1
#define SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE          1
#define SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE          1
#define SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ENTRY_SIZE                       1
#define SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ENTRY_SIZE                       1
#define SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ENTRY_SIZE                         1
#define SOC_PETRA_SCH_CAL_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_DRM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_DSM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_FDMS_TBL_ENTRY_SIZE                                                    1
#define SOC_PETRA_SCH_SHDS_TBL_ENTRY_SIZE                                                    2
#define SOC_PETRA_SCH_SEM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_FSF_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_FGM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_SHC_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_SCC_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_SCT_TBL_ENTRY_SIZE                                                     2
#define SOC_PETRA_SCH_FQM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_FFM_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_TMC_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_PQS_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE                                          1
#define SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ENTRY_SIZE                                    1
#define SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE                                      16
#define SOC_PETRA_MMU_IDF_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_MMU_FDF_TBL_ENTRY_SIZE                                                     1
#define SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RDF_RADDR_TBL_ENTRY_SIZE                                               1
#define SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_ENTRY_SIZE                                         1
#define SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_ENTRY_SIZE                                         1
#define SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define SOC_PETRA_MMU_RAF_WADDR_TBL_ENTRY_SIZE                                               1
#define SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_ENTRY_SIZE                                       1

#define SOC_PETRA_NUM_OF_INDIRECT_MODULES  21

/*
 *  Indication for invalid block ID
 */

#define SOC_PETRA_IRR_GLAG_MAPPING_LAG_PORT_INDEX_NOF_BITS   4

#define SOC_PETRA_IRR_GLAG_DEVISION_HASH_NOF_BITS  8
/*
 * In the system red mechanism, we divide the queues to 16 ranges,
 * according to the thresholds. and each ranges receives a value of
 * 4 bit (0-15), this represents the queue in the system red test.
 */
#define SOC_PETRA_SYS_RED_NOF_Q_RNGS                                                         16
#define SOC_PETRA_SYS_RED_NOF_Q_RNGS_THS                                                     (SOC_PETRA_SYS_RED_NOF_Q_RNGS-1)

#define SOC_PETRA_IPS_NOF_QUEUE_PRIORITY_MAPS_TABLES 2
/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/*
 *  Set the table to a_tbls
 */
#ifdef LINK_PA_LIBRARIES
  #define SOC_PA_TBL(tbl_ptr)           (tables->a_##tbl_ptr)
  #define SOC_PA_TBL_REF(tbl_ptr)       &(SOC_PA_TBL(tbl_ptr))
#endif

#if (defined(LINK_PA_LIBRARIES) && defined(LINK_PB_LIBRARIES))
  #define SOC_PETRA_TBL(reg_desc) \
    ((SOC_PETRA_IS_DEV_PETRA_A)? SOC_PA_TBL(reg_desc) : SOC_PB_TBL(reg_desc))

  #define SOC_PETRA_TBL_REF(reg_desc) \
    ((SOC_PETRA_IS_DEV_PETRA_A)? SOC_PA_TBL_REF(reg_desc) : SOC_PB_TBL_REF(reg_desc))
#endif /* LINK_PA_LIBRARIES && LINK_PB_LIBRARIES */

#if (defined(LINK_PA_LIBRARIES) && (!defined(LINK_PB_LIBRARIES)))
  #define SOC_PETRA_TBL(reg_desc)         SOC_PA_TBL(reg_desc)
  #define SOC_PETRA_TBL_REF(fld_desc)     SOC_PA_TBL_REF(fld_desc)
#endif /* LINK_PA_LIBRARIES && !LINK_PB_LIBRARIES */

#if (defined(LINK_PB_LIBRARIES) && (!defined(LINK_PA_LIBRARIES)))
  #define SOC_PETRA_TBL(reg_desc)         SOC_PB_TBL(reg_desc)
  #define SOC_PETRA_TBL_REF(fld_desc)     SOC_PB_TBL_REF(fld_desc)
#endif /* !LINK_PA_LIBRARIES && LINK_PB_LIBRARIES */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
typedef struct
{
  uint32 pge_mem;
} SOC_PETRA_OLP_PGE_MEM_TBL_DATA;

typedef struct
{
  uint32 fap_port;
} SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA;

typedef struct
{
  uint32 contexts_bit_mapping[3];
} SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA;

typedef struct
{
  uint32 fap_port;
} SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA;

typedef struct
{
  uint32 cpu_packet_counter[2];
} SOC_PETRA_IRE_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pcb_pointer;
  uint32 count;
  uint32 ecc;
} SOC_PETRA_IDR_COMPLETE_PC_TBL_DATA;

typedef struct
{
  uint32 is_ingress_replication[SOC_PETRA_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL];
} SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA;

typedef struct
{
  uint32 base_queue[2];
  uint32 is_queue_number[2];
  uint32 out_lif[2];
  uint32 egress_data;
  uint32 link_ptr;
} SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA;

typedef struct
{
  uint32 port[3];
  uint32 out_lif[3];
  uint32 link_ptr;
} SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA;

typedef struct
{
  uint32 mirror_destination;
  uint32 is_queue_number;
  uint32 is_multicast;
  uint32 is_outbound_mirror;
  uint32 mirror_traffic_class;
  uint32 traffic_class_over_write;
  uint32 mirror_drop_precedence;
  uint32 drop_precedence_over_write;
} SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA;

typedef struct
{
  uint32 snoop_destination;
  uint32 is_queue_number;
  uint32 is_multicast;
  uint32 snoop_traffic_class;
  uint32 traffic_class_over_write;
  uint32 snoop_drop_precedence;
  uint32 drop_precedence_over_write;
} SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA;

typedef struct
{
  uint32 glag_to_lag_range;
} SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA;

typedef struct
{
  uint32 smooth_division;
} SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_DATA;

typedef struct
{
  uint32 glag_mapping;
} SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA;

typedef struct
{
  uint32 queue_number;
  uint32 queue_valid;
} SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA;

typedef struct
{
  uint32 offset;
  uint32 rr_lb_mode;
} SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA;

typedef struct
{
  uint32 two_lsb;
  uint32 two_msb;
} SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_DATA;

typedef struct
{
  uint32 port_type;
  uint32 bytes_to_skip;
  uint32 custom_command_select;
  uint32 statistics_profile_extension;
  uint32 port_type_extension;
  uint32 statistics_profile;
  uint32 use_lag_member;
  uint32 has_mirror;
  uint32 mirror_is_mc;
  uint32 bytes_to_remove;
  uint32 header_remove;
  uint32 append_ftmh;
  uint32 append_prog_header;
} SOC_PETRA_IHP_PORT_INFO_TBL_DATA;

typedef struct
{
  uint32 port_to_system_port_id;
} SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA;

typedef struct
{
  uint32 shaping;
  uint32 shaping_itmh;
  uint32 itmh[2];
} SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA;

typedef struct
{
  uint32 system_port_my_port_table[SOC_PETRA_IHP_SYS_MY_PORT_TBL_FLDS];
} SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA;

typedef struct
{
  uint32 ptc_commands[2];
} SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA;

typedef struct
{
  uint32 ptc_key_program_ptr;
  uint32 ptc_key_program_var;
  uint32 ptc_cos_profile;
} SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA;

typedef struct
{
  uint32 select;
  uint32 length;
  uint32 nibble_shift;
  uint32 byte_shift;
  uint32 offset_select;
} SOC_PETRA_IHP_KEY_PROGRAM_TBL_DATA;

typedef struct
{
  uint32 select[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];
  uint32 length[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];
  uint32 nibble_shift[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];
  uint32 byte_shift[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];
  uint32 offset_select[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];
} SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA;

typedef struct
{
  uint32 key_program1;
} SOC_PETRA_IHP_KEY_PROGRAM1_TBL_DATA;

typedef struct
{
  uint32 instruction4;
  uint32 instruction3;
  uint32 exact_match_enable;
  uint32 exact_match_mask_index;
} SOC_PETRA_IHP_KEY_PROGRAM2_TBL_DATA;

typedef struct
{
  uint32 key_program3;
} SOC_PETRA_IHP_KEY_PROGRAM3_TBL_DATA;

typedef struct
{
  uint32 instruction9;
  uint32 instruction10;
  uint32 tcam_match_enable;
  uint32 tcam_match_select;
  uint32 tcam_key_and_value;
  uint32 tcam_key_or_value;
  uint32 bytes_to_remove_hdr_sel;
  uint32 bytes_to_remove_hdr_size;
  uint32 cos_hdr_var_mask_select;
} SOC_PETRA_IHP_KEY_PROGRAM4_TBL_DATA;

typedef struct
{
  uint32 drop_precedence;
  uint32 traffic_class;
} SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_DATA;

typedef struct
{
  uint32 programmable_cos1[3];
} SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_DATA;

typedef struct
{
  uint32 bdb_link_list;
} SOC_PETRA_IQM_BDB_LINK_LIST_TBL_DATA;

typedef struct
{
  uint32 pq_head_ptr;
  uint32 que_not_empty;
  uint32 pq_inst_que_size;
  uint32 pq_avrg_szie;
} SOC_PETRA_IQM_DYNAMIC_TBL_DATA;

typedef struct
{
  uint32 credit_class;
  uint32 rate_class;
  uint32 connection_class;
  uint32 traffic_class;
  uint32 que_signature;
} SOC_PETRA_IQM_STATIC_TBL_DATA;

typedef struct
{
  uint32 tail_ptr;
} SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA;

typedef struct
{
  uint32 pq_weight;
  uint32 avrg_en;
} SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA;

typedef struct
{
  uint32 crdt_disc_val;
  uint32 discnt_sign;
} SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA;

typedef struct
{
  uint32 flus_cnt;
} SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA;

typedef struct
{
  uint32 mn_us_cnt[2];
} SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA;

typedef struct
{
  uint32 pq_max_que_size_mnt;
  uint32 pq_max_que_size_exp;
  uint32 pq_wred_en;
  uint32 pq_c2;
  uint32 pq_c3;
  uint32 pq_c1;
  uint32 pq_avrg_max_th;
  uint32 pq_avrg_min_th;
  uint32 pq_wred_pckt_sz_ignr;
  uint32 addmit_logic;
} SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA;

typedef struct
{
  uint32 tx_pd;
} SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA;

typedef struct
{
  uint32 tx_dscr[3];
} SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA;

typedef struct
{
  uint32 vsq_rc_a;
} SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA;

typedef struct
{
  uint32 vsq_rc_b;
} SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA;

typedef struct
{
  uint32 vsq_rc_c;
} SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA;

typedef struct
{
  uint32 vsq_rc_d;
} SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA;

typedef struct
{
  uint32 vsq_size_wrds;
  uint32 vsq_size_bds;
} SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA;

typedef struct
{
  uint32 vsq_size_wrds;
  uint32 vsq_size_bds;
} SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA;

typedef struct
{
  uint32 vsq_size_wrds;
  uint32 vsq_size_bds;
} SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA;

typedef struct
{
  uint32 vsq_size_wrds;
  uint32 vsq_size_bds;
} SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA;

typedef struct
{
  uint32 vsq_avrg_size;
} SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA;

typedef struct
{
  uint32 vsq_avrg_size;
} SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA;

typedef struct
{
  uint32 vsq_avrg_size;
} SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA;

typedef struct
{
  uint32 vsq_avrg_size;
} SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA;

typedef struct
{
  uint32 wred_en;
  uint32 avrg_size_en;
  uint32 red_weight_q;
  uint32 set_threshold_words_mnt;
  uint32 set_threshold_words_exp;
  uint32 clear_threshold_words_mnt;
  uint32 clear_threshold_words_exp;
  uint32 set_threshold_bd_mnt;
  uint32 set_threshold_bd_exp;
  uint32 clear_threshold_bd_mnt;
  uint32 clear_threshold_bd_exp;
} SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA;

typedef struct
{
  uint32 c2;
  uint32 c3;
  uint32 c1;
  uint32 max_avrg_th;
  uint32 min_avrg_th;
  uint32 vq_wred_pckt_sz_ignr;
  uint32 vq_max_szie_bds_mnt;
  uint32 vq_max_szie_bds_exp;
  uint32 vq_max_size_words_mnt;
  uint32 vq_max_size_words_exp;
} SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA;

typedef struct
{
  uint32 sys_red_en;
  uint32 adm_th;
  uint32 prob_th;
  uint32 drp_th;
  uint32 drp_prob_indx1;
  uint32 drp_prob_indx2;
} SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA;

typedef struct
{
  uint32 drp_prob;
} SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA;

typedef struct
{
  uint32 qsz_rng_th[SOC_PETRA_SYS_RED_NOF_Q_RNGS_THS];
} SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA;

typedef struct
{
  uint32 data;
} SOC_PETRA_QDR_MEM_TBL_DATA;

typedef struct
{
  uint32 qdr_dll_mem;
} __ATTRIBUTE_PACKED__ SOC_PETRA_QDR_QDR_DLL_MEM_TBL_DATA;

typedef struct
{
  uint32 sys_phy_port;
} SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA;

typedef struct
{
  uint32 dest_port;
  uint32 dest_dev;
} SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA;

typedef struct
{
  uint32 base_flow;
  uint32 sub_flow_mode;
} SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA;

typedef struct
{
  uint32 queue_type_lookup_table;
} SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA;

typedef struct
{
  uint32 queue_priority_map_select;
} SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA;

typedef struct
{
  uint32 queue_priority_maps_table[SOC_PETRA_IPS_NOF_QUEUE_PRIORITY_MAPS_TABLES];
} SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA;

typedef struct
{
  uint32 off_to_slow_msg_th;
  uint32 off_to_norm_msg_th;
  uint32 slow_to_norm_msg_th;
  uint32 norm_to_slow_msg_th;
  uint32 fsm_th_mul;
} SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA;

typedef struct
{
  uint32 backoff_enter_qcr_bal_th;
  uint32 backoff_exit_qcr_bal_th;
  uint32 backlog_enter_qcr_bal_th;
  uint32 backlog_exit_qcr_bal_th;
} SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA;

typedef struct
{
  uint32 empty_qsatisfied_cr_bal;
  uint32 max_empty_qcr_bal;
  uint32 exceed_max_empty_qcr_bal;
} SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA;

typedef struct
{
  uint32 wd_status_msg_gen_period;
  uint32 wd_delete_qth;
} SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA;

typedef struct
{
  uint32 cr_bal;
  uint32 crs;
  uint32 one_pkt_deq;
  uint32 wd_last_cr_time;
  uint32 in_dqcq;
  uint32 wd_delete;
  uint32 fsmrq_ctrl;
} SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA;

typedef struct
{
  uint32 exponent;
  uint32 mantissa;
  uint32 qsize_4b;
} SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_DATA;

typedef struct
{
  uint32 maxqsz;
  uint32 maxqsz_age;
} SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA;
typedef struct
{
  uint32 flow_id;
  uint32 dest_pipe;
  uint32 flow_status;
  uint32 dest_port;
  uint32 queue_size_4b;
  uint32 queue_number;
  uint32 orig_fs;
  uint32 disable_timer;
  uint32 valid;
} SOC_PETRA_IPS_FMS_MSG_MEM_TABLE_TBL_DATA;
typedef struct
{
  uint32 bdq;
} SOC_PETRA_IPT_BDQ_TBL_DATA;

typedef struct
{
  uint32 pcq[2];
} SOC_PETRA_IPT_PCQ_TBL_DATA;

typedef struct
{
  uint32 sop_mmu[2];
} SOC_PETRA_IPT_SOP_MMU_TBL_DATA;

typedef struct
{
  uint32 mop_mmu;
} SOC_PETRA_IPT_MOP_MMU_TBL_DATA;

typedef struct
{
  uint32 fdtctl;
} SOC_PETRA_IPT_FDTCTL_TBL_DATA;

typedef struct
{
  uint32 fdtdata[16];
} SOC_PETRA_IPT_FDTDATA_TBL_DATA;

typedef struct
{
  uint32 egqctl;
} SOC_PETRA_IPT_EGQCTL_TBL_DATA;

typedef struct
{
  uint32 egqdata[16];
} SOC_PETRA_IPT_EGQDATA_TBL_DATA;

typedef struct
{
  uint32 dll_ram[2];
} SOC_PETRA_DPI_DLL_RAM_TBL_DATA;

typedef struct
{
  uint32 unicast_distribution_memory_for_data_cells[3];
} SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA;

typedef struct
{
  uint32 unicast_distribution_memory_for_control_cells[3];
} SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_CPU_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} SOC_PETRA_EGQ_RCY_SCM_TBL_DATA;

typedef struct
{
  uint32 interface_select;
} SOC_PETRA_EGQ_CCM_TBL_DATA;

typedef struct
{
  uint32 port_max_credit;
} SOC_PETRA_EGQ_PMC_TBL_DATA;

typedef struct
{
  uint32 cbm;
} SOC_PETRA_EGQ_CBM_TBL_DATA;

typedef struct
{
  uint32 free_buffer_memory;
} SOC_PETRA_EGQ_FBM_TBL_DATA;

typedef struct
{
  uint32 free_descriptor_memory[2];
} SOC_PETRA_EGQ_FDM_TBL_DATA;

typedef struct
{
  uint32 mc_or_mc_low_queue_weight;
  uint32 uc_or_uc_low_queue_weight;
} SOC_PETRA_EGQ_DWM_TBL_DATA;

typedef struct
{
  uint32 crcremainder;
  uint32 reas_state;
  uint32 eopfrag_num;
  uint32 nxt_frag_number;
  uint32 stored_seg_size;
  uint32 fix129;
} SOC_PETRA_EGQ_RRDM_TBL_DATA;

typedef struct
{
  uint32 packet_start_buffer_pointer;
  uint32 packet_buffer_write_pointer;
  uint32 reas_state;
  uint32 packet_frag_cnt;
} SOC_PETRA_EGQ_RPDM_TBL_DATA;

typedef struct
{
  uint32 outbound_mirr;
  uint32 is_mirroring;
  uint32 otmh_lif_ext_ena;
  uint32 otmh_dest_ext_ena;
  uint32 otmh_src_ext_ena;
  uint32 port_type;
  uint32 port_channel_number;
  uint32 destination_port_extension_id;
  uint32 header_compensation_type;
} SOC_PETRA_EGQ_PCT_TBL_DATA;

typedef struct
{
  uint32 multicast_lag_load_balancing_enable;
  uint32 sys_port_id;
  uint32 glag_member_port_id;
  uint32 glag_port_id;
  uint32 port_is_glag_member;
  uint32 port_type;
} SOC_PETRA_EGQ_PPCT_TBL_DATA;

typedef struct
{
  uint32 vlan_membership[3];
} SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA;


typedef struct
{
  uint32 cfc_flow_control[3];
} SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_DATA;

typedef struct
{
  uint32 nifa_flow_control;
} SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_DATA;

typedef struct
{
  uint32 nifb_flow_control;
} SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_DATA;

typedef struct
{
  uint32 cpu_last_header[2];
} SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_DATA;

typedef struct
{
  uint32 ipt_last_header[2];
} SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_DATA;

typedef struct
{
  uint32 fdr_last_header[2];
} SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_DATA;

typedef struct
{
  uint32 cpu_packet_counter[2];
} SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ipt_packet_counter[2];
} SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 fdr_packet_counter[2];
} SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 rqp_packet_counter[2];
} SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 rqp_discard_packet_counter[2];
} SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ehp_unicast_packet_counter[2];
} SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ehp_multicast_high_packet_counter[2];
} SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ehp_multicast_low_packet_counter[2];
} SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ehp_discard_packet_counter[2];
} SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_unicast_high_packet_counter[2];
} SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_unicast_low_packet_counter[2];
} SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_multicast_high_packet_counter[2];
} SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_multicast_low_packet_counter[2];
} SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_unicast_high_bytes_counter[2];
} SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_unicast_low_bytes_counter[2];
} SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_multicast_high_bytes_counter[2];
} SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_multicast_low_bytes_counter[2];
} SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_discard_unicast_packet_counter[2];
} SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_discard_multicast_packet_counter[2];
} SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 fqp_packet_counter[2];
} SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 egq_ofp_num;
} SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA;

typedef struct
{
  uint32 egq_ofp_num[SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS];
} SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA;

typedef struct
{
  uint32 ofp_hr;
  uint32 lp_ofp_valid;
  uint32 hp_ofp_valid;
} SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA;

typedef struct
{
  uint32 fc_index;
  uint32 fc_dest_sel;
} SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA;

typedef struct
{
  uint32 fc_index;
  uint32 fc_source_sel;
} SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA;

typedef struct
{
  uint32 hrsel;
} SOC_PETRA_SCH_CAL_TBL_DATA;

typedef struct
{
  uint32 device_rate;
} SOC_PETRA_SCH_DRM_TBL_DATA;

typedef struct
{
  uint32 dual_shaper_ena;
} SOC_PETRA_SCH_DSM_TBL_DATA;

typedef struct
{
  uint32 sch_number;
  uint32 cos;
  uint32 hrsel_dual;
} SOC_PETRA_SCH_FDMS_TBL_DATA;

typedef struct
{
  uint32 peak_rate_man_even;
  uint32 peak_rate_exp_even;
  uint32 max_burst_even;
  uint32 slow_rate2_sel_even;
  uint32 peak_rate_man_odd;
  uint32 peak_rate_exp_odd;
  uint32 max_burst_odd;
  uint32 slow_rate2_sel_odd;
  uint32 max_burst_update_even;
  uint32 max_burst_update_odd;
} SOC_PETRA_SCH_SHDS_TBL_DATA;

typedef struct
{
  uint32 sch_enable;
} SOC_PETRA_SCH_SEM_TBL_DATA;

typedef struct
{
  uint32 sfenable;
} SOC_PETRA_SCH_FSF_TBL_DATA;

typedef struct
{
  uint32 flow_group[SOC_PETRA_TBL_FGM_NOF_GROUPS_ONE_LINE];
} SOC_PETRA_SCH_FGM_TBL_DATA;

typedef struct
{
  uint32 hrmode;
  uint32 hrmask_type;
} SOC_PETRA_SCH_SHC_TBL_DATA;

typedef struct
{
  uint32 clsch_type;
} SOC_PETRA_SCH_SCC_TBL_DATA;

typedef struct
{
  uint32 clconfig;
  uint32 af0_inv_weight;
  uint32 af1_inv_weight;
  uint32 af2_inv_weight;
  uint32 af3_inv_weight;
  uint32 wfqmode;
  uint32 enh_clen;
  uint32 enh_clsphigh;
} SOC_PETRA_SCH_SCT_TBL_DATA;

typedef struct
{
  uint32 base_queue_num;
  uint32 sub_flow_mode;
  uint32 flow_slow_enable;
} SOC_PETRA_SCH_FQM_TBL_DATA;

typedef struct
{
  uint32 device_number;
} SOC_PETRA_SCH_FFM_TBL_DATA;

typedef struct
{
  uint32 token_count;
  uint32 slow_status;
} SOC_PETRA_SCH_TMC_TBL_DATA;

typedef struct
{
  uint32 max_qsz;
  uint32 flow_id;
  uint32 aging_bit;
} SOC_PETRA_SCH_PQS_TBL_DATA;

typedef struct
{
  uint32 schinit;
} SOC_PETRA_SCH_SCHEDULER_INIT_TBL_DATA;

typedef struct
{
  uint32 message_flow_id;
  uint32 message_type;
} SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_DATA;
typedef struct
{
  uint32 data[16];
} SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_DATA;

typedef struct
{
  uint32 data;
} SOC_PETRA_MMU_IDF_TBL_DATA;

typedef struct
{
  uint32 data;
} SOC_PETRA_MMU_FDF_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
} SOC_PETRA_MMU_RDF_RADDR_TBL_DATA;

typedef struct
{
  uint32 waddr_half_a;
} SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_DATA;

typedef struct
{
  uint32 waddr_half_b;
} SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
} SOC_PETRA_MMU_RAF_WADDR_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 select_source_sum[SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_ENTRY_SIZE];
} SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_DATA;

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

void
  soc_petra_tbls_nof_repetitions_clear(void);

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_olp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_ire_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndidrctCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_idr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_irr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_ihp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_qdr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_iqm_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_ips_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

uint32
  soc_petra_ipt_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

uint32
  soc_petra_dpi_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32  block_id,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_rtp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_egq_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_cfc_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_sch_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 *  Set number of repetitions for table write.
 *  Each write command is executed
 *  IndirectCommandCount number of times. The address is
 *  advanced by one for each write command. If set to 0,
 *  only one operation is performed.
 */
uint32
  soc_petra_epni_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

/*
 * Read indirect table pge_mem_tbl from block OLP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_olp_pge_mem_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_OLP_PGE_MEM_TBL_DATA* OLP_pge_mem_tbl_data
  );

/*
 * Write indirect table pge_mem_tbl from block OLP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_olp_pge_mem_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_OLP_PGE_MEM_TBL_DATA* OLP_pge_mem_tbl_data
  );

/*
 * Read indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ire_nif_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA* IRE_nif_ctxt_map_tbl_data
  );

/*
 * Write indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ire_nif_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA* IRE_nif_ctxt_map_tbl_data
  );

/*
 * Read indirect table nif_port2ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ire_nif_port2ctxt_bit_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA* IRE_nif_port2ctxt_bit_map_tbl_data
  );

/*
 * Write indirect table nif_port2ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ire_nif_port2ctxt_bit_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA* IRE_nif_port2ctxt_bit_map_tbl_data
  );

/*
 * Read indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ire_rcy_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  );

/*
 * Write indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ire_rcy_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  );

/*
 * Read indirect table complete_pc_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_idr_complete_pc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IDR_COMPLETE_PC_TBL_DATA* IDR_complete_pc_tbl_data
  );

/*
 * Write indirect table complete_pc_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_idr_complete_pc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IDR_COMPLETE_PC_TBL_DATA* IDR_complete_pc_tbl_data
  );

/*
 * Read indirect table is_ingress_replication_db_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_is_ingress_replication_db_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA* IRR_is_ingress_replication_db_tbl_data
  );

/*
 * Write indirect table is_ingress_replication_db_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_is_ingress_replication_db_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA* IRR_is_ingress_replication_db_tbl_data
  );


/*
 * Read indirect table egress_replication_multicast_db_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA* IRR_egress_replication_multicast_db_tbl_data
  );

/*
 * Write indirect table egress_replication_multicast_db_tbl from block IRR,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_irr_egress_replication_multicast_db_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA* IRR_egress_replication_multicast_db_tbl_data
  );


/*
 * Read indirect table ingress_replication_multicast_db_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA* IRR_ingress_replication_multicast_db_tbl_data
  );

/*
 * Write indirect table ingress_replication_multicast_db_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA* IRR_ingress_replication_multicast_db_tbl_data
  );

/*
 * Read indirect table mirror_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_mirror_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA* IRR_mirror_table_tbl_data
  );

/*
 * Write indirect table mirror_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_mirror_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA* IRR_mirror_table_tbl_data
  );

/*
 * Read indirect table snoop_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_snoop_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA* IRR_snoop_table_tbl_data
  );

/*
 * Write indirect table snoop_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_snoop_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA* IRR_snoop_table_tbl_data
  );

/*
 * Read indirect table glag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_glag_to_lag_range_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA* IRR_glag_to_lag_range_tbl_data
  );

/*
 * Write indirect table glag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_glag_to_lag_range_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA* IRR_glag_to_lag_range_tbl_data
  );

/*
 * Read indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_smooth_division_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_indx,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_OUT  SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_DATA* IRR_smooth_division_tbl_data
  );

/*
 * Write indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_smooth_division_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_IN   SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_DATA* IRR_smooth_division_tbl_data
  );

/*
 * Read indirect table glag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_glag_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_OUT  SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA* IRR_glag_mapping_tbl_data
  );

/*
 * Write indirect table glag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_glag_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_IN   SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA* IRR_glag_mapping_tbl_data
  );

/*
 * Read indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_destination_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA* IRR_destination_table_tbl_data
  );

/*
 * Write indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_destination_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA* IRR_destination_table_tbl_data
  );

/*
 * Read indirect table glag_next_member_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_glag_next_member_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA* IRR_glag_next_member_tbl_data
  );

/*
 * Write indirect table glag_next_member_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_glag_next_member_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA* IRR_glag_next_member_tbl_data
  );

/*
 * Read indirect table rlag_next_member_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_rlag_next_member_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_DATA* IRR_rlag_next_member_tbl_data
  );

/*
 * Write indirect table rlag_next_member_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_irr_rlag_next_member_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_DATA* IRR_rlag_next_member_tbl_data
  );

/*
 * Read indirect table port_info_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_port_info_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PORT_INFO_TBL_DATA* IHP_port_info_tbl_data
  );

/*
 * Write indirect table port_info_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_port_info_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PORT_INFO_TBL_DATA* IHP_port_info_tbl_data
  );

/*
 * Read indirect table port_to_system_port_id_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_port_to_system_port_id_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA* IHP_port_to_system_port_id_tbl_data
  );

/*
 * Write indirect table port_to_system_port_id_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA* IHP_port_to_system_port_id_tbl_data
  );

/*
 * Read indirect table static_header_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_static_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA* IHP_static_header_tbl_data
  );

/*
 * Write indirect table static_header_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_static_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA* IHP_static_header_tbl_data
  );

/*
 * Read indirect table system_port_my_port_table_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_system_port_my_port_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA* IHP_system_port_my_port_table_tbl_data
  );

/*
 * Write indirect table system_port_my_port_table_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_system_port_my_port_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA* IHP_system_port_my_port_table_tbl_data
  );

/*
 * Read indirect table ptc_commands1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_ptc_commands1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands1_tbl_data
  );

/*
 * Write indirect table ptc_commands1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_ptc_commands1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands1_tbl_data
  );

/*
 * Read indirect table ptc_commands2_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_ptc_commands2_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands2_tbl_data
  );

/*
 * Write indirect table ptc_commands2_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_ptc_commands2_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands2_tbl_data
  );

/*
 * Read indirect table ptc_key_program_lut_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_ptc_key_program_lut_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA* IHP_ptc_key_program_lut_tbl_data
  );

/*
 * Write indirect table ptc_key_program_lut_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_ptc_key_program_lut_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA* IHP_ptc_key_program_lut_tbl_data
  );

/*
 * This is a generic function for key program tables 0 - 4
 * Read indirect table key_program_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             program_ndx,
    SOC_SAND_IN   uint32             instr_ndx,
    SOC_SAND_OUT   SOC_PETRA_IHP_KEY_PROGRAM_TBL_DATA* IHP_key_program_tbl_data
  );

/*
 * This is a generic function for key program tables 0 - 4
 * Write indirect table key_program_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_ihp_key_program_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             program_ndx,
    SOC_SAND_IN   uint32             instr_ndx,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM_TBL_DATA* IHP_key_program_tbl_data
  );

/*
 * Read indirect table key_program0_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program0_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA* IHP_key_program0_tbl_data
  );

/*
 * Write indirect table key_program0_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program0_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA* IHP_key_program0_tbl_data
  );

/*
 * Read indirect table key_program1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM1_TBL_DATA* IHP_key_program1_tbl_data
  );

/*
 * Write indirect table key_program1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM1_TBL_DATA* IHP_key_program1_tbl_data
  );

/*
 * Read indirect table key_program2_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program2_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM2_TBL_DATA* IHP_key_program2_tbl_data
  );

/*
 * Write indirect table key_program2_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program2_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM2_TBL_DATA* IHP_key_program2_tbl_data
  );

/*
 * Read indirect table key_program3_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program3_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM3_TBL_DATA* IHP_key_program3_tbl_data
  );

/*
 * Write indirect table key_program3_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program3_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM3_TBL_DATA* IHP_key_program3_tbl_data
  );

/*
 * Read indirect table key_program4_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program4_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_KEY_PROGRAM4_TBL_DATA* IHP_key_program4_tbl_data
  );

/*
 * Write indirect table key_program4_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_key_program4_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_KEY_PROGRAM4_TBL_DATA* IHP_key_program4_tbl_data
  );

/*
 * Read indirect table programmable_cos_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 * Each entry contains 16 set. Use set_ndx for the requested set
 */
uint32
  soc_petra_ihp_programmable_cos_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             set_ndx,
    SOC_SAND_OUT  SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_DATA* IHP_programmable_cos_tbl_data
  );

/*
 * Write indirect table programmable_cos_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 * Each entry contains 16 set. Use set_ndx for the requested set
 */

uint32
  soc_petra_ihp_programmable_cos_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             set_ndx,
    SOC_SAND_IN   SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_DATA* IHP_programmable_cos_tbl_data
  );

/*
 * Read indirect table programmable_cos1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_programmable_cos1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_DATA* IHP_programmable_cos1_tbl_data
  );

/*
 * Write indirect table programmable_cos1_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ihp_programmable_cos1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_DATA* IHP_programmable_cos1_tbl_data
  );

/*
 * Read indirect table bdb_link_list_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_bdb_link_list_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_BDB_LINK_LIST_TBL_DATA* IQM_bdb_link_list_tbl_data
  );

/*
 * Write indirect table bdb_link_list_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_bdb_link_list_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_BDB_LINK_LIST_TBL_DATA* IQM_bdb_link_list_tbl_data
  );

/*
 * Read indirect table dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_dynamic_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  );

/*
 * Write indirect table dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_dynamic_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  );

/*
 * Read indirect table static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_static_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  );

/*
 * Write indirect table static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_static_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  );

/*
 * Read indirect table packet_queue_tail_pointer_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_packet_queue_tail_pointer_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA* IQM_packet_queue_tail_pointer_tbl_data
  );

/*
 * Write indirect table packet_queue_tail_pointer_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_packet_queue_tail_pointer_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA* IQM_packet_queue_tail_pointer_tbl_data
  );

/*
 * Read indirect table packet_queue_red_weight_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA* IQM_packet_queue_red_weight_table_tbl_data
  );

/*
 * Write indirect table packet_queue_red_weight_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_packet_queue_red_weight_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA* IQM_packet_queue_red_weight_table_tbl_data
  );

/*
 * Read indirect table credit_discount_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_credit_discount_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA* IQM_credit_discount_table_tbl_data
  );

/*
 * Write indirect table credit_discount_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_credit_discount_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA* IQM_credit_discount_table_tbl_data
  );

/*
 * Read indirect table full_user_count_memory_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_full_user_count_memory_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA* IQM_full_user_count_memory_tbl_data
  );

/*
 * Write indirect table full_user_count_memory_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_full_user_count_memory_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA* IQM_full_user_count_memory_tbl_data
  );

/*
 * Read indirect table mini_multicast_user_count_memory_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_mini_multicast_user_count_memory_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA* IQM_mini_multicast_user_count_memory_tbl_data
  );

/*
 * Write indirect table mini_multicast_user_count_memory_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_mini_multicast_user_count_memory_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA* IQM_mini_multicast_user_count_memory_tbl_data
  );

/*
 * Read indirect table packet_queue_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   uint32                                              rt_cls_ndx,
    SOC_SAND_IN   uint32                                             drop_precedence_ndx,
    SOC_SAND_OUT  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  );

/*
 * Write indirect table packet_queue_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   uint32                                              rt_cls_ndx,
    SOC_SAND_IN   uint32                                             drop_precedence_ndx,
    SOC_SAND_IN   SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  );

uint32
  soc_petra_IQM_packet_descriptor_fifos_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_petra_IQM_packet_descriptor_fifos_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_petra_IQM_tx_descriptor_fifos_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_petra_IQM_tx_descriptor_fifos_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table vsq_descriptor_rate_class_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA* IQM_vsq_descriptor_rate_class_group_a_tbl_data
  );

/*
 * Write indirect table vsq_descriptor_rate_class_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA* IQM_vsq_descriptor_rate_class_group_a_tbl_data
  );

/*
 * Read indirect table vsq_descriptor_rate_class_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA* IQM_vsq_descriptor_rate_class_group_b_tbl_data
  );

/*
 * Write indirect table vsq_descriptor_rate_class_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA* IQM_vsq_descriptor_rate_class_group_b_tbl_data
  );

/*
 * Read indirect table vsq_descriptor_rate_class_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA* IQM_vsq_descriptor_rate_class_group_c_tbl_data
  );

/*
 * Write indirect table vsq_descriptor_rate_class_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA* IQM_vsq_descriptor_rate_class_group_c_tbl_data
  );

/*
 * Read indirect table vsq_descriptor_rate_class_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA* IQM_vsq_descriptor_rate_class_group_d_tbl_data
  );

/*
 * Write indirect table vsq_descriptor_rate_class_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA* IQM_vsq_descriptor_rate_class_group_d_tbl_data
  );

/*
 * Read indirect table vsq_qsize_memory_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_qsize_memory_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_qsize_memory_group_a_tbl_data
  );

/*
 * Write indirect table vsq_qsize_memory_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_qsize_memory_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_qsize_memory_group_a_tbl_data
  );

/*
 * Read indirect table vsq_qsize_memory_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_qsize_memory_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_qsize_memory_group_b_tbl_data
  );

/*
 * Write indirect table vsq_qsize_memory_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_qsize_memory_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_qsize_memory_group_b_tbl_data
  );

/*
 * Read indirect table vsq_qsize_memory_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_qsize_memory_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_qsize_memory_group_c_tbl_data
  );

/*
 * Write indirect table vsq_qsize_memory_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_qsize_memory_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_qsize_memory_group_c_tbl_data
  );

/*
 * Read indirect table vsq_qsize_memory_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_qsize_memory_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_qsize_memory_group_d_tbl_data
  );

/*
 * Write indirect table vsq_qsize_memory_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_qsize_memory_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_qsize_memory_group_d_tbl_data
  );

/*
 * Read indirect table vsq_average_qsize_memory_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_average_qsize_memory_group_a_tbl_data
  );

/*
 * Write indirect table vsq_average_qsize_memory_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_average_qsize_memory_group_a_tbl_data
  );

/*
 * Read indirect table vsq_average_qsize_memory_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_average_qsize_memory_group_b_tbl_data
  );

/*
 * Write indirect table vsq_average_qsize_memory_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_average_qsize_memory_group_b_tbl_data
  );

/*
 * Read indirect table vsq_average_qsize_memory_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_average_qsize_memory_group_c_tbl_data
  );

/*
 * Write indirect table vsq_average_qsize_memory_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_average_qsize_memory_group_c_tbl_data
  );

/*
 * Read indirect table vsq_average_qsize_memory_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_average_qsize_memory_group_d_tbl_data
  );

/*
 * Write indirect table vsq_average_qsize_memory_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_average_qsize_memory_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_average_qsize_memory_group_d_tbl_data
  );

/*
 * Read indirect table vsq_flow_control_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_flow_control_parameters_table_group_tbl_data
  );

/*
 * Write indirect table vsq_flow_control_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_flow_control_parameters_table_group_tbl_data
  );

/*
 * Read indirect table vsq_queue_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_OUT  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  );

/*
 * Write indirect table vsq_queue_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_IN   SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  );



/*
 * Read indirect table system_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_system_red_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN   int                                       unit,
    SOC_SAND_IN   uint32                                        rt_cls_ndx,
    SOC_SAND_IN   uint32                                       drop_precedence_ndx,
    SOC_SAND_OUT  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA* IQM_system_red_parameters_table_tbl_data
  );

/*
 * Write indirect table system_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_system_red_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN   int                                       unit,
    SOC_SAND_IN   uint32                                        rt_cls_ndx,
    SOC_SAND_IN   uint32                                       drop_precedence_ndx,
    SOC_SAND_IN   SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA* IQM_system_red_parameters_table_tbl_data
  );

/*
 * Read indirect table system_red_drop_probability_values_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_system_red_drop_probability_values_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA* IQM_system_red_drop_probability_values_tbl_data
  );

/*
 * Write indirect table system_red_drop_probability_values_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_system_red_drop_probability_values_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA* IQM_system_red_drop_probability_values_tbl_data
  );

/*
 * Read indirect table system_red_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_system_red_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA* IQM_system_red_tbl_data
  );

/*
 * Write indirect table system_red_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_iqm_system_red_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA* IQM_system_red_tbl_data
  );



/*
 * Read indirect table qdr_memory from block QDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_qdr_memory_tbl_get_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   uint32                  entry_offset,
    SOC_SAND_OUT  SOC_PETRA_QDR_MEM_TBL_DATA*   QDR_memory_tbl_data
  );

/*
 * Write indirect table qdr_memory from block QDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_qdr_memory_tbl_set_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   uint32                  entry_offset,
    SOC_SAND_IN  SOC_PETRA_QDR_MEM_TBL_DATA*   QDR_memory_tbl_data
  );

/*
 * Read indirect table qdr_dll_mem_tbl from block QDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_qdr_qdr_dll_mem_tbl_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       entry_offset,
    SOC_SAND_OUT SOC_PETRA_QDR_QDR_DLL_MEM_TBL_DATA *tbl_data
  );

/*
 * Write indirect table qdr_dll_mem_tbl from block QDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_qdr_qdr_dll_mem_tbl_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       entry_offset,
    SOC_SAND_IN  SOC_PETRA_QDR_QDR_DLL_MEM_TBL_DATA *tbl_data
  );

/*
 * Read indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_system_physical_port_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA* IPS_system_physical_port_lookup_table_tbl_data
  );

/*
 * Write indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_system_physical_port_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA* IPS_system_physical_port_lookup_table_tbl_data
  );

/*
 * Read indirect table destination_device_and_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_destination_device_and_port_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA* IPS_destination_device_and_port_lookup_table_tbl_data
  );

/*
 * Write indirect table destination_device_and_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_destination_device_and_port_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA* IPS_destination_device_and_port_lookup_table_tbl_data
  );

/*
 * Read indirect table flow_id_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_flow_id_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  );

/*
 * Write indirect table flow_id_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_flow_id_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  );

/*
 * Read indirect table queue_type_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_type_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA* IPS_queue_type_lookup_table_tbl_data
  );

/*
 * Write indirect table queue_type_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_type_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA* IPS_queue_type_lookup_table_tbl_data
  );

/*
 * Read indirect table queue_priority_map_select_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_priority_map_select_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA* IPS_queue_priority_map_select_tbl_data
  );

/*
 * Write indirect table queue_priority_map_select_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_priority_map_select_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA* IPS_queue_priority_map_select_tbl_data
  );

/*
 * Read indirect table queue_priority_maps_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_priority_maps_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA* IPS_queue_priority_maps_table_tbl_data
  );

/*
 * Write indirect table queue_priority_maps_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_priority_maps_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA* IPS_queue_priority_maps_table_tbl_data
  );

/*
 * Read indirect table queue_size_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_queue_size_based_thresholds_table_tbl_data
  );

/*
 * Write indirect table queue_size_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_size_based_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_queue_size_based_thresholds_table_tbl_data
  );

/*
 * Read indirect table credit_balance_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_balance_based_thresholds_table_tbl_data
  );

/*
 * Write indirect table credit_balance_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_credit_balance_based_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_balance_based_thresholds_table_tbl_data
  );

/*
 * Read indirect table empty_queue_credit_balance_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA* IPS_empty_queue_credit_balance_table_tbl_data
  );

/*
 * Write indirect table empty_queue_credit_balance_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_empty_queue_credit_balance_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA* IPS_empty_queue_credit_balance_table_tbl_data
  );

/*
 * Read indirect table credit_watchdog_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_credit_watchdog_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_watchdog_thresholds_table_tbl_data
  );

/*
 * Write indirect table credit_watchdog_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_credit_watchdog_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_watchdog_thresholds_table_tbl_data
  );

/*
 * Read indirect table queue_descriptor_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_descriptor_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA* IPS_queue_descriptor_table_tbl_data
  );

/*
 * Write indirect table queue_descriptor_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_descriptor_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA* IPS_queue_descriptor_table_tbl_data
  );

/*
 * Read indirect table queue_size_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_size_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_DATA* IPS_queue_size_table_tbl_data
  );

/*
 * Write indirect table queue_size_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_queue_size_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_DATA* IPS_queue_size_table_tbl_data
  );

/*
 * Read indirect table system_red_max_queue_size_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_system_red_max_queue_size_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA* IPS_system_red_max_queue_size_table_tbl_data
  );

/*
 * Write indirect table system_red_max_queue_size_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ips_system_red_max_queue_size_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA* IPS_system_red_max_queue_size_table_tbl_data
  );

/*
 * Read indirect table bdq_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_bdq_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_BDQ_TBL_DATA* IPT_bdq_tbl_data
  );

/*
 * Write indirect table bdq_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_bdq_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_BDQ_TBL_DATA* IPT_bdq_tbl_data
  );

/*
 * Read indirect table pcq_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_pcq_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_PCQ_TBL_DATA* IPT_pcq_tbl_data
  );

/*
 * Write indirect table pcq_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_pcq_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_PCQ_TBL_DATA* IPT_pcq_tbl_data
  );

/*
 * Read indirect table sop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_sop_mmu_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_SOP_MMU_TBL_DATA* IPT_sop_mmu_tbl_data
  );

/*
 * Write indirect table sop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_sop_mmu_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_SOP_MMU_TBL_DATA* IPT_sop_mmu_tbl_data
  );

/*
 * Read indirect table mop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_mop_mmu_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_MOP_MMU_TBL_DATA* IPT_mop_mmu_tbl_data
  );

/*
 * Write indirect table mop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_mop_mmu_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_MOP_MMU_TBL_DATA* IPT_mop_mmu_tbl_data
  );

/*
 * Read indirect table fdtctl_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_fdtctl_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_FDTCTL_TBL_DATA* IPT_fdtctl_tbl_data
  );

/*
 * Write indirect table fdtctl_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_fdtctl_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_FDTCTL_TBL_DATA* IPT_fdtctl_tbl_data
  );

/*
 * Read indirect table fdtdata_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_fdtdata_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_FDTDATA_TBL_DATA* IPT_fdtdata_tbl_data
  );

/*
 * Write indirect table fdtdata_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_fdtdata_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_FDTDATA_TBL_DATA* IPT_fdtdata_tbl_data
  );

/*
 * Read indirect table egqctl_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_egqctl_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_EGQCTL_TBL_DATA* IPT_egqctl_tbl_data
  );

/*
 * Write indirect table egqctl_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_egqctl_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_EGQCTL_TBL_DATA* IPT_egqctl_tbl_data
  );

/*
 * Read indirect table egqdata_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_egqdata_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_IPT_EGQDATA_TBL_DATA* IPT_egqdata_tbl_data
  );

/*
 * Write indirect table egqdata_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_ipt_egqdata_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_IPT_EGQDATA_TBL_DATA* IPT_egqdata_tbl_data
  );

/*
 * Read indirect table dll_ram_tbl from block DPI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_dpi_dll_ram_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             dram_ndx,
    SOC_SAND_OUT  SOC_PETRA_DPI_DLL_RAM_TBL_DATA* DPI_dll_ram_tbl_data
  );

/*
 * Write indirect table dll_ram_tbl from block DPI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_dpi_dll_ram_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DPI_DLL_RAM_TBL_DATA* DPI_dll_ram_tbl_data
  );

/*
 * Read indirect table unicast_distribution_memory_for_data_cells_tbl from block RTP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_data_cells_tbl_data
  );

/*
 * Write indirect table unicast_distribution_memory_for_data_cells_tbl from block RTP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_data_cells_tbl_data
  );

/*
 * Read indirect table unicast_distribution_memory_for_control_cells_tbl from block RTP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_rtp_unicast_distribution_memory_for_control_cells_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_control_cells_tbl_data
  );

/*
 * Write indirect table unicast_distribution_memory_for_control_cells_tbl from block RTP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_rtp_unicast_distribution_memory_for_control_cells_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_control_cells_tbl_data
  );

/*
 * Read indirect table nif_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_nif_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32             mal_ndx,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_nif_scm_tbl_data
  );

/*
 * Write indirect table nif_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_nif_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32             mal_ndx,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_nif_scm_tbl_data
  );

/*
 * Read indirect table nifab_nch_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_nifab_nch_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_DATA* EGQ_nifab_nch_scm_tbl_data
  );

/*
 * Write indirect table nifab_nch_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_nifab_nch_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_DATA* EGQ_nifab_nch_scm_tbl_data
  );

/*
 * Read indirect table rcy_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rcy_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_rcy_scm_tbl_data
  );

/*
 * Write indirect table rcy_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rcy_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_rcy_scm_tbl_data
  );

/*
 * Read indirect table cpu_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cpu_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_cpu_scm_tbl_data
  );

/*
 * Write indirect table cpu_scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cpu_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_SCM_TBL_DATA* EGQ_cpu_scm_tbl_data
  );

/*
 * Read indirect table ccm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ccm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CCM_TBL_DATA* EGQ_ccm_tbl_data
  );

/*
 * Write indirect table ccm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ccm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CCM_TBL_DATA* EGQ_ccm_tbl_data
  );

/*
 * Read indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pmc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  );

/*
 * Write indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pmc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  );

/*
 * Read indirect table cbm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cbm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CBM_TBL_DATA* EGQ_cbm_tbl_data
  );

/*
 * Write indirect table cbm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cbm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CBM_TBL_DATA* EGQ_cbm_tbl_data
  );

/*
 * Read indirect table fbm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fbm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FBM_TBL_DATA* EGQ_fbm_tbl_data
  );

/*
 * Write indirect table fbm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fbm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FBM_TBL_DATA* EGQ_fbm_tbl_data
  );

/*
 * Read indirect table fdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FDM_TBL_DATA* EGQ_fdm_tbl_data
  );

/*
 * Write indirect table fdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FDM_TBL_DATA* EGQ_fdm_tbl_data
  );

/*
 * Read indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_dwm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_DWM_TBL_DATA* EGQ_dwm_tbl_data
  );

/*
 * Write indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_dwm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_DWM_TBL_DATA* EGQ_dwm_tbl_data
  );

/*
 * Read indirect table rrdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rrdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_RRDM_TBL_DATA* EGQ_rrdm_tbl_data
  );

/*
 * Write indirect table rrdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rrdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_RRDM_TBL_DATA* EGQ_rrdm_tbl_data
  );

/*
 * Read indirect table rpdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rpdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_RPDM_TBL_DATA* EGQ_rpdm_tbl_data
  );

/*
 * Write indirect table rpdm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rpdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_RPDM_TBL_DATA* EGQ_rpdm_tbl_data
  );

/*
 * Read indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pct_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PCT_TBL_DATA* EGQ_pct_tbl_data
  );

/*
 * Write indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pct_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PCT_TBL_DATA* EGQ_pct_tbl_data
  );

/*
 * Read indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ppct_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PPCT_TBL_DATA* EGQ_ppct_tbl_data
  );

/*
 * Write indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_petra_egq_ppct_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PPCT_TBL_DATA* EGQ_ppct_tbl_data
  );

/*
 * Read indirect table vlan_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_vlan_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA* EGQ_vlan_table_tbl_data
  );

/*
 * Write indirect table vlan_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_vlan_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA* EGQ_vlan_table_tbl_data
  );


/*
 * Read indirect table cfc_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cfc_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_DATA* EGQ_cfc_flow_control_tbl_data
  );

/*
 * Write indirect table cfc_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cfc_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_DATA* EGQ_cfc_flow_control_tbl_data
  );

/*
 * Read indirect table nifa_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_nifa_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_DATA* EGQ_nifa_flow_control_tbl_data
  );

/*
 * Write indirect table nifa_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_nifa_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_DATA* EGQ_nifa_flow_control_tbl_data
  );

/*
 * Read indirect table nifb_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_nifb_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_DATA* EGQ_nifb_flow_control_tbl_data
  );

/*
 * Write indirect table nifb_flow_control_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_nifb_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_DATA* EGQ_nifb_flow_control_tbl_data
  );

/*
 * Read indirect table cpu_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cpu_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_DATA* EGQ_cpu_last_header_tbl_data
  );

/*
 * Write indirect table cpu_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cpu_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_DATA* EGQ_cpu_last_header_tbl_data
  );

/*
 * Read indirect table ipt_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ipt_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_DATA* EGQ_ipt_last_header_tbl_data
  );

/*
 * Write indirect table ipt_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ipt_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_DATA* EGQ_ipt_last_header_tbl_data
  );

/*
 * Read indirect table fdr_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fdr_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_DATA* EGQ_fdr_last_header_tbl_data
  );

/*
 * Write indirect table fdr_last_header_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fdr_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_DATA* EGQ_fdr_last_header_tbl_data
  );

/*
 * Read indirect table cpu_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cpu_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_DATA* EGQ_cpu_packet_counter_tbl_data
  );

/*
 * Write indirect table cpu_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_cpu_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_DATA* EGQ_cpu_packet_counter_tbl_data
  );

/*
 * Read indirect table ipt_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ipt_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_DATA* EGQ_ipt_packet_counter_tbl_data
  );

/*
 * Write indirect table ipt_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ipt_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_DATA* EGQ_ipt_packet_counter_tbl_data
  );

/*
 * Read indirect table fdr_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fdr_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_DATA* EGQ_fdr_packet_counter_tbl_data
  );

/*
 * Write indirect table fdr_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fdr_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_DATA* EGQ_fdr_packet_counter_tbl_data
  );

/*
 * Read indirect table rqp_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rqp_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_DATA* EGQ_rqp_packet_counter_tbl_data
  );

/*
 * Write indirect table rqp_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rqp_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_DATA* EGQ_rqp_packet_counter_tbl_data
  );

/*
 * Read indirect table rqp_discard_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rqp_discard_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_rqp_discard_packet_counter_tbl_data
  );

/*
 * Write indirect table rqp_discard_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_rqp_discard_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_rqp_discard_packet_counter_tbl_data
  );

/*
 * Read indirect table ehp_unicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ehp_unicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_ehp_unicast_packet_counter_tbl_data
  );

/*
 * Write indirect table ehp_unicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ehp_unicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_ehp_unicast_packet_counter_tbl_data
  );

/*
 * Read indirect table ehp_multicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ehp_multicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_high_packet_counter_tbl_data
  );

/*
 * Write indirect table ehp_multicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ehp_multicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_high_packet_counter_tbl_data
  );

/*
 * Read indirect table ehp_multicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ehp_multicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_low_packet_counter_tbl_data
  );

/*
 * Write indirect table ehp_multicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ehp_multicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_low_packet_counter_tbl_data
  );

/*
 * Read indirect table ehp_discard_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ehp_discard_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_ehp_discard_packet_counter_tbl_data
  );

/*
 * Write indirect table ehp_discard_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_ehp_discard_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_ehp_discard_packet_counter_tbl_data
  );

/*
 * Read indirect table pqp_unicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_unicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_packet_counter_tbl_data
  );

/*
 * Write indirect table pqp_unicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_unicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_packet_counter_tbl_data
  );

/*
 * Read indirect table pqp_unicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_unicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_packet_counter_tbl_data
  );

/*
 * Write indirect table pqp_unicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_unicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_packet_counter_tbl_data
  );

/*
 * Read indirect table pqp_multicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_multicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_packet_counter_tbl_data
  );

/*
 * Write indirect table pqp_multicast_high_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_multicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_packet_counter_tbl_data
  );

/*
 * Read indirect table pqp_multicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_multicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_packet_counter_tbl_data
  );

/*
 * Write indirect table pqp_multicast_low_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_multicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_packet_counter_tbl_data
  );

/*
 * Read indirect table pqp_unicast_high_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_unicast_high_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_bytes_counter_tbl_data
  );

/*
 * Write indirect table pqp_unicast_high_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_unicast_high_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_bytes_counter_tbl_data
  );

/*
 * Read indirect table pqp_unicast_low_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_unicast_low_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_bytes_counter_tbl_data
  );

/*
 * Write indirect table pqp_unicast_low_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_unicast_low_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_bytes_counter_tbl_data
  );

/*
 * Read indirect table pqp_multicast_high_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_multicast_high_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_bytes_counter_tbl_data
  );

/*
 * Write indirect table pqp_multicast_high_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_multicast_high_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_bytes_counter_tbl_data
  );

/*
 * Read indirect table pqp_multicast_low_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_multicast_low_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_bytes_counter_tbl_data
  );

/*
 * Write indirect table pqp_multicast_low_bytes_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_multicast_low_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_bytes_counter_tbl_data
  );

/*
 * Read indirect table pqp_discard_unicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_discard_unicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_unicast_packet_counter_tbl_data
  );

/*
 * Write indirect table pqp_discard_unicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_discard_unicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_unicast_packet_counter_tbl_data
  );

/*
 * Read indirect table pqp_discard_multicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_discard_multicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_multicast_packet_counter_tbl_data
  );

/*
 * Write indirect table pqp_discard_multicast_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_pqp_discard_multicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_multicast_packet_counter_tbl_data
  );

/*
 * Read indirect table fqp_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fqp_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_DATA* EGQ_fqp_packet_counter_tbl_data
  );

/*
 * Write indirect table fqp_packet_counter_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_egq_fqp_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_DATA* EGQ_fqp_packet_counter_tbl_data
  );


/*
 * Read indirect table recycle_to_out_going_fap_port_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA* CFC_recycle_to_out_going_fap_port_mapping_tbl_data
  );

/*
 * Write indirect table recycle_to_out_going_fap_port_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA* CFC_recycle_to_out_going_fap_port_mapping_tbl_data
  );

/*
 * Read indirect table nif_a_class_based_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_nif_a_class_based_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_a_class_based_to_ofp_mapping_tbl_data
  );

/*
 * Write indirect table nif_a_class_based_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_nif_a_class_based_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_a_class_based_to_ofp_mapping_tbl_data
  );

/*
 * Read indirect table nif_b_class_based_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_nif_b_class_based_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_b_class_based_to_ofp_mapping_tbl_data
  );

/*
 * Write indirect table nif_b_class_based_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_nif_b_class_based_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_b_class_based_to_ofp_mapping_tbl_data
  );

/*
 * Read indirect table a_schedulers_based_flow_control_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  );

/*
 * Write indirect table a_schedulers_based_flow_control_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  );

/*
 * Read indirect table b_schedulers_based_flow_control_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  );

/*
 * Write indirect table b_schedulers_based_flow_control_to_ofp_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  );

/*
 * Read indirect table out_of_band_rx_a_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_a_calendar_mapping_tbl_data
  );

/*
 * Write indirect table out_of_band_rx_a_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_a_calendar_mapping_tbl_data
  );

/*
 * Read indirect table out_of_band_rx_b_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_b_calendar_mapping_tbl_data
  );

/*
 * Write indirect table out_of_band_rx_b_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_b_calendar_mapping_tbl_data
  );

/*
 * Read indirect table out_of_band_tx_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_tx_calendar_mapping_tbl_data
  );

/*
 * Write indirect table out_of_band_tx_calendar_mapping_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_tx_calendar_mapping_tbl_data
  );

/*
 * Read indirect table cal_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_cal_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_CAL_TBL_DATA* SCH_cal_tbl_data
  );

/*
 * Write indirect table cal_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_cal_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_CAL_TBL_DATA* SCH_cal_tbl_data
  );

/*
 * Read indirect table drm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_drm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  );

/*
 * Write indirect table drm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_drm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  );

/*
 * Read indirect table dsm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_dsm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_DSM_TBL_DATA* SCH_dsm_tbl_data
  );

/*
 * Write indirect table dsm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_dsm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_DSM_TBL_DATA* SCH_dsm_tbl_data
  );

/*
 * Read indirect table fdms_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_fdms_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FDMS_TBL_DATA* SCH_fdms_tbl_data
  );

/*
 * Write indirect table fdms_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_fdms_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FDMS_TBL_DATA* SCH_fdms_tbl_data
  );

/*
 * Read indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_shds_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  );

/*
 * Write indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_shds_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  );

/*
 * Read indirect table sem_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_sem_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SEM_TBL_DATA* SCH_sem_tbl_data
  );

/*
 * Write indirect table sem_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_sem_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SEM_TBL_DATA* SCH_sem_tbl_data
  );

/*
 * Read indirect table fsf_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_fsf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FSF_TBL_DATA* SCH_fsf_tbl_data
  );

/*
 * Write indirect table fsf_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_fsf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FSF_TBL_DATA* SCH_fsf_tbl_data
  );

/*
 * Read indirect table fgm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_fgm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FGM_TBL_DATA* SCH_fgm_tbl_data
  );

/*
 * Write indirect table fgm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_fgm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FGM_TBL_DATA* SCH_fgm_tbl_data
  );

/*
 * Read indirect table shc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_shc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SHC_TBL_DATA* SCH_shc_tbl_data
  );

/*
 * Write indirect table shc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_shc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SHC_TBL_DATA* SCH_shc_tbl_data
  );

/*
 * Read indirect table scc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_scc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SCC_TBL_DATA* SCH_scc_tbl_data
  );

/*
 * Write indirect table scc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_scc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SCC_TBL_DATA* SCH_scc_tbl_data
  );

/*
 * Read indirect table sct_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_sct_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_SCT_TBL_DATA* SCH_sct_tbl_data
  );

/*
 * Write indirect table sct_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_sct_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SCT_TBL_DATA* SCH_sct_tbl_data
  );

/*
 * Read indirect table fqm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_fqm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FQM_TBL_DATA* SCH_fqm_tbl_data
  );

/*
 * Write indirect table fqm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_fqm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FQM_TBL_DATA* SCH_fqm_tbl_data
  );

/*
 * Read indirect table ffm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_ffm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_FFM_TBL_DATA* SCH_ffm_tbl_data
  );

/*
 * Write indirect table ffm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_ffm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FFM_TBL_DATA* SCH_ffm_tbl_data
  );

/*
 * Read indirect table soc_tmctbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_tmc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_TMC_TBL_DATA* SCH_tmc_tbl_data
  );

/*
 * Write indirect table soc_tmctbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_tmc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_TMC_TBL_DATA* SCH_tmc_tbl_data
  );

/*
 * Write indirect table pqs_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_pqs_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_PQS_TBL_DATA* SCH_pqs_tbl_data
  );

uint32
  soc_petra_sch_pqs_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_SCH_PQS_TBL_DATA* SCH_pqs_tbl_data
  );

/*
 * Write indirect table scheduler_init_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_scheduler_init_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_SCHEDULER_INIT_TBL_DATA* SCH_scheduler_init_tbl_data
  );

/*
 * Write indirect table force_status_message_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_sch_force_status_message_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_DATA* SCH_force_status_message_tbl_data
  );


/*
 * Get the number of banks and number of Drams
 */
uint32
  soc_petra_mmu_dram_address_space_info_get_unsafe(
    SOC_SAND_IN    int                               unit,
    SOC_SAND_OUT   uint32                                *nof_drams,
    SOC_SAND_OUT   uint32                                *nof_banks,
    SOC_SAND_OUT   uint32                                *nof_cols
  );

uint32
  soc_petra_mmu_dram_address_space_tbl_verify(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx
  );

/*
 * col_ndx has to be multiply of 16.
 * proper configuration should be for the
 * DRAM, before access it. (number of drams columns etc.)
 */
uint32
  soc_petra_mmu_dram_address_space_tbl_get_unsafe(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx,
    SOC_SAND_OUT  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_DATA*  MMU_dram_address_space_tbl_data
  );

/*
 * Write indirect table dram_address_space_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_dram_address_space_tbl_set_unsafe(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx,
    SOC_SAND_IN  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_DATA*  MMU_dram_address_space_tbl_data
  );

/*
 * Read indirect table idf_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_idf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_IDF_TBL_DATA* MMU_idf_tbl_data
  );

/*
 * Write indirect table idf_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_idf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_IDF_TBL_DATA* MMU_idf_tbl_data
  );

/*
 * Read indirect table fdf_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_fdf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_FDF_TBL_DATA* MMU_fdf_tbl_data
  );

/*
 * Write indirect table fdf_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_fdf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_FDF_TBL_DATA* MMU_fdf_tbl_data
  );

/*
 * Read indirect table rdfa_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfa_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_DATA* MMU_rdfa_waddr_status_tbl_data
  );

/*
 * Write indirect table rdfa_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfa_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_DATA* MMU_rdfa_waddr_status_tbl_data
  );

/*
 * Read indirect table rdfb_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfb_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_DATA* MMU_rdfb_waddr_status_tbl_data
  );

/*
 * Write indirect table rdfb_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfb_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_DATA* MMU_rdfb_waddr_status_tbl_data
  );

/*
 * Read indirect table rdfc_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfc_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_DATA* MMU_rdfc_waddr_status_tbl_data
  );

/*
 * Write indirect table rdfc_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfc_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_DATA* MMU_rdfc_waddr_status_tbl_data
  );

/*
 * Read indirect table rdfd_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfd_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_DATA* MMU_rdfd_waddr_status_tbl_data
  );

/*
 * Write indirect table rdfd_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfd_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_DATA* MMU_rdfd_waddr_status_tbl_data
  );

/*
 * Read indirect table rdfe_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfe_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_DATA* MMU_rdfe_waddr_status_tbl_data
  );

/*
 * Write indirect table rdfe_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdfe_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_DATA* MMU_rdfe_waddr_status_tbl_data
  );

/*
 * Read indirect table rdff_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdff_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_DATA* MMU_rdff_waddr_status_tbl_data
  );

/*
 * Write indirect table rdff_waddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdff_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_DATA* MMU_rdff_waddr_status_tbl_data
  );

/*
 * Read indirect table rdf_raddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdf_raddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RDF_RADDR_TBL_DATA* MMU_rdf_raddr_tbl_data
  );

/*
 * Write indirect table rdf_raddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rdf_raddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RDF_RADDR_TBL_DATA* MMU_rdf_raddr_tbl_data
  );

/*
 * Read indirect table waf_halfa_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_waf_halfa_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_DATA* MMU_waf_halfa_waddr_tbl_data
  );

/*
 * Write indirect table waf_halfa_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_waf_halfa_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_DATA* MMU_waf_halfa_waddr_tbl_data
  );

/*
 * Read indirect table waf_halfb_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_waf_halfb_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_DATA* MMU_waf_halfb_waddr_tbl_data
  );

/*
 * Write indirect table waf_halfb_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_waf_halfb_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_DATA* MMU_waf_halfb_waddr_tbl_data
  );

/*
 * Read indirect table wafa_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafa_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafa_halfa_raddr_status_tbl_data
  );

/*
 * Write indirect table wafa_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafa_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafa_halfa_raddr_status_tbl_data
  );

/*
 * Read indirect table wafb_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafb_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafb_halfa_raddr_status_tbl_data
  );

/*
 * Write indirect table wafb_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafb_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafb_halfa_raddr_status_tbl_data
  );

/*
 * Read indirect table wafc_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafc_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafc_halfa_raddr_status_tbl_data
  );

/*
 * Write indirect table wafc_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafc_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafc_halfa_raddr_status_tbl_data
  );

/*
 * Read indirect table wafd_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafd_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafd_halfa_raddr_status_tbl_data
  );

/*
 * Write indirect table wafd_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafd_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafd_halfa_raddr_status_tbl_data
  );

/*
 * Read indirect table wafe_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafe_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafe_halfa_raddr_status_tbl_data
  );

/*
 * Write indirect table wafe_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafe_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafe_halfa_raddr_status_tbl_data
  );

/*
 * Read indirect table waff_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_waff_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA* MMU_waff_halfa_raddr_status_tbl_data
  );

/*
 * Write indirect table waff_halfa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_waff_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA* MMU_waff_halfa_raddr_status_tbl_data
  );

/*
 * Read indirect table wafa_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafa_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafa_halfb_raddr_status_tbl_data
  );

/*
 * Write indirect table wafa_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafa_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafa_halfb_raddr_status_tbl_data
  );

/*
 * Read indirect table wafb_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafb_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafb_halfb_raddr_status_tbl_data
  );

/*
 * Write indirect table wafb_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafb_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafb_halfb_raddr_status_tbl_data
  );

/*
 * Read indirect table wafc_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafc_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafc_halfb_raddr_status_tbl_data
  );

/*
 * Write indirect table wafc_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafc_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafc_halfb_raddr_status_tbl_data
  );

/*
 * Read indirect table wafd_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafd_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafd_halfb_raddr_status_tbl_data
  );

/*
 * Write indirect table wafd_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafd_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafd_halfb_raddr_status_tbl_data
  );

/*
 * Read indirect table wafe_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafe_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafe_halfb_raddr_status_tbl_data
  );

/*
 * Write indirect table wafe_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_wafe_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafe_halfb_raddr_status_tbl_data
  );

/*
 * Read indirect table waff_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_waff_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA* MMU_waff_halfb_raddr_status_tbl_data
  );

/*
 * Write indirect table waff_halfb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_waff_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA* MMU_waff_halfb_raddr_status_tbl_data
  );

/*
 * Read indirect table raf_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_raf_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAF_WADDR_TBL_DATA* MMU_raf_waddr_tbl_data
  );

/*
 * Write indirect table raf_waddr_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_raf_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAF_WADDR_TBL_DATA* MMU_raf_waddr_tbl_data
  );

/*
 * Read indirect table rafa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_DATA* MMU_rafa_raddr_status_tbl_data
  );

/*
 * Write indirect table rafa_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_DATA* MMU_rafa_raddr_status_tbl_data
  );

/*
 * Read indirect table rafb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_DATA* MMU_rafb_raddr_status_tbl_data
  );

/*
 * Write indirect table rafb_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_DATA* MMU_rafb_raddr_status_tbl_data
  );

/*
 * Read indirect table rafc_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafc_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_DATA* MMU_rafc_raddr_status_tbl_data
  );

/*
 * Write indirect table rafc_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafc_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_DATA* MMU_rafc_raddr_status_tbl_data
  );

/*
 * Read indirect table rafd_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafd_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_DATA* MMU_rafd_raddr_status_tbl_data
  );

/*
 * Write indirect table rafd_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafd_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_DATA* MMU_rafd_raddr_status_tbl_data
  );

/*
 * Read indirect table rafe_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafe_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_DATA* MMU_rafe_raddr_status_tbl_data
  );

/*
 * Write indirect table rafe_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_rafe_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_DATA* MMU_rafe_raddr_status_tbl_data
  );

/*
 * Read indirect table raff_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_raff_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_DATA* MMU_raff_raddr_status_tbl_data
  );

/*
 * Write indirect table raff_raddr_status_tbl from block MMU,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_petra_mmu_raff_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_DATA* MMU_raff_raddr_status_tbl_data
  );


uint32
  soc_petra_ipt_select_source_sum_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_OUT  SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_DATA* IPT_select_source_sum_tbl_data
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_TBL_ACCESS_H_INCLUDED__*/
#endif
