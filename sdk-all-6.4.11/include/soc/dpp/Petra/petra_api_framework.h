/* $Id: petra_api_framework.h,v 1.9 Broadcom SDK $
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

#ifndef __SOC_PETRA_API_FRAMEWORK_H_INCLUDED__
/* { */
#define __SOC_PETRA_API_FRAMEWORK_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_api_framework.h,v 1.9 Broadcom SDK $
 * Procedure identifiers.
 * {
 */

#define SOC_PETRA_REGISTER_DEVICE                                                       (   0|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_UNREGISTER_DEVICE                                                     (   1|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY                                        (   2|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_FDR_ENABLERS_VAL_GET                                             (   3|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CREDIT_WORTH_SET                                                 (   7|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CREDIT_WORTH_GET                                                 (   8|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CREDIT_WORTH_VERIFY                                              (   9|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CREDIT_WORTH_SET_UNSAFE                                          (  20|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CREDIT_WORTH_GET_UNSAFE                                          (  21|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE1                                             (  22|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_FAP_ID_SET                                                       (  23|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_FAP_ID_GET                                                       (  24|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_FAP_ID_VERIFY                                                    (  25|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_FAP_ID_SET_UNSAFE                                                (  26|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_FAP_ID_GET_UNSAFE                                                (  27|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE2                                             (  28|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_INTERFACES_SET                                                (  29|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_INTERFACES_GET                                                (  40|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_INTERFACES_VERIFY                                             (  41|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_INTERFACES_SET_UNSAFE                                         (  42|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_INTERFACES_GET_UNSAFE                                         (  43|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_SET                                        (  44|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_GET                                        (  45|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_VERIFY                                     (  46|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_SET_UNSAFE                                 (  47|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_GET_UNSAFE                                 (  48|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ENABLE_TRAFFIC_SET                                               (  49|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ENABLE_TRAFFIC_GET                                               (  50|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CHIP_TIME_TO_TICKS                                                    (  51|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CHIP_TICKS_TO_TIME                                                    (  52|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_DEVICE_INIT                                                      (  53|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ENABLE_TRAFFIC_VERIFY                                            (  61|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ENABLE_TRAFFIC_SET_UNSAFE                                        (  62|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ENABLE_TRAFFIC_GET_UNSAFE                                        (  63|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SERDES_LINKS_PARAM_SET                                            (  64|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SERDES_LINKS_PARAM_GET                                            (  65|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SERDES_LINKS_PARAM_VERIFY                                         (  66|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SERDES_LINKS_PARAM_SET_UNSAFE                                     (  67|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SERDES_LINKS_PARAM_GET_UNSAFE                                     (  68|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SERDES_LINKS_STATUS_GET                                           (  69|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_CONF_SET                                                    (  80|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_CONF_GET                                                    (  81|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_CONF_VERIFY                                                 (  82|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_CONF_SET_UNSAFE                                             (  83|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_CONF_GET_UNSAFE                                             (  84|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_XAUI_CONF_SET                                                     (  85|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_XAUI_CONF_GET                                                     (  86|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_XAUI_CONF_VERIFY                                                  (  87|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_XAUI_CONF_SET_UNSAFE                                              (  88|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_XAUI_CONF_GET_UNSAFE                                              (  89|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SPAUI_CONF_SET                                                    ( 100|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SPAUI_CONF_GET                                                    ( 101|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SPAUI_CONF_VERIFY                                                 ( 102|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SPAUI_CONF_SET_UNSAFE                                             ( 103|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SPAUI_CONF_GET_UNSAFE                                             ( 104|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CONF_SET                                                    ( 105|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CONF_GET                                                    ( 106|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CONF_VERIFY                                                 ( 107|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CONF_SET_UNSAFE                                             ( 108|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CONF_GET_UNSAFE                                             ( 109|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_OC768C_CONF_SET                                                   ( 120|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_OC768C_CONF_GET                                                   ( 121|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_OC768C_CONF_VERIFY                                                ( 122|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_OC768C_CONF_SET_UNSAFE                                            ( 123|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_OC768C_CONF_GET_UNSAFE                                            ( 124|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FLOW_CONTROL_CONF_SET                                             ( 125|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FLOW_CONTROL_CONF_GET                                             ( 126|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FLOW_CONTROL_CONF_VERIFY                                          ( 127|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FLOW_CONTROL_CONF_SET_UNSAFE                                      ( 128|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FLOW_CONTROL_CONF_GET_UNSAFE                                      ( 129|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_ENABLE_SET                                                  ( 140|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_ENABLE_GET                                                  ( 141|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_ENABLE_VERIFY                                               ( 142|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_ENABLE_SET_UNSAFE                                           ( 143|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINKS_ENABLE_GET_UNSAFE                                           ( 144|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_DIAG_LAST_PACKET_GET                                              ( 146|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_NIF_PORTS_SET                                        ( 147|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_NIF_PORTS_GET                                        ( 148|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_NIF_PORTS_VERIFY                                     ( 149|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_DIAG_LAST_PACKET_GET_UNSAFE                                       ( 150|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_NIF_PORTS_SET_UNSAFE                                 ( 160|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_NIF_PORTS_GET_UNSAFE                                 ( 161|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_RCY_PORTS_SET                                        ( 162|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_RCY_PORTS_GET                                        ( 163|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_RCY_PORTS_VERIFY                                     ( 164|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_RCY_PORTS_SET_UNSAFE                                 ( 165|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_RCY_PORTS_GET_UNSAFE                                 ( 166|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_CPU_PORTS_SET                                        ( 167|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_CPU_PORTS_GET                                        ( 168|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_CPU_PORTS_VERIFY                                     ( 169|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_CPU_PORTS_SET_UNSAFE                                 ( 180|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAP_PORTS_TO_CPU_PORTS_GET_UNSAFE                                 ( 181|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_OPTIONS_SET                                                      ( 182|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_OPTIONS_GET                                                      ( 183|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_OPTIONS_VERIFY                                                   ( 184|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_OPTIONS_SET_UNSAFE                                               ( 185|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_OPTIONS_GET_UNSAFE                                               ( 186|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_RAW_SWITCHING_CONF_SET                                           ( 187|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_RAW_SWITCHING_CONF_GET                                           ( 188|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_RAW_SWITCHING_CONF_VERIFY                                        ( 189|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_RAW_SWITCHING_CONF_SET_UNSAFE                                    ( 200|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_RAW_SWITCHING_CONF_GET_UNSAFE                                    ( 201|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_STATISTICS_TAG_CONF_SET                                          ( 202|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_STATISTICS_TAG_CONF_GET                                          ( 203|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_STATISTICS_TAG_CONF_VERIFY                                       ( 204|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_STATISTICS_TAG_CONF_SET_UNSAFE                                   ( 205|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_STATISTICS_TAG_CONF_GET_UNSAFE                                   ( 206|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU_IF_PORT_CONF_SET                                                  ( 207|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU_IF_PORT_CONF_GET                                                  ( 208|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU_IF_PORT_CONF_VERIFY                                               ( 209|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU_IF_PORT_CONF_SET_UNSAFE                                           ( 220|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU_IF_PORT_CONF_GET_UNSAFE                                           ( 221|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_HEADER_SET                                                     ( 228|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_HEADER_GET                                                     ( 229|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_HEADER_VERIFY                                                  ( 240|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_HEADER_SET_UNSAFE                                              ( 241|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_HEADER_GET_UNSAFE                                              ( 242|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_INGRESS_PORT_SET                                               ( 243|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_INGRESS_PORT_GET                                               ( 244|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_INGRESS_PORT_VERIFY                                            ( 245|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_INGRESS_PORT_SET_UNSAFE                                        ( 246|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_INGRESS_PORT_GET_UNSAFE                                        ( 247|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_EGRESS_PORT_SET                                                ( 248|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_EGRESS_PORT_GET                                                ( 249|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_EGRESS_PORT_VERIFY                                             ( 260|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_EGRESS_PORT_SET_UNSAFE                                         ( 261|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MIRROR_EGRESS_PORT_GET_UNSAFE                                         ( 262|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET                                    ( 263|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET                                    ( 264|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_VERIFY                                 ( 265|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_UNSAFE                             ( 266|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET_UNSAFE                             ( 267|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_SET                                             ( 268|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_GET                                             ( 269|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_VERIFY                                          ( 280|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_SET_UNSAFE                                      ( 281|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_GET_UNSAFE                                      ( 282|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET                          ( 283|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET                          ( 284|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_VERIFY                       ( 285|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_UNSAFE                   ( 286|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_UNSAFE                   ( 287|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_SET                                     ( 288|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_GET                                     ( 289|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_VERIFY                                  ( 300|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_SET_UNSAFE                              ( 301|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_GET_UNSAFE                              ( 302|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_SET                                         ( 303|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_GET                                         ( 304|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_VERIFY                                      ( 305|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_SET_UNSAFE                                  ( 306|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_GET_UNSAFE                                  ( 307|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUARTET_RESET                                                     ( 308|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUARTET_RESET_VERIFY                                              ( 309|SOC_PETRA_PROC_BITS)
#define SOC_PA_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_UNSAFE                                ( 310|SOC_PETRA_PROC_BITS)
#define SOC_PA_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET_UNSAFE                                ( 311|SOC_PETRA_PROC_BITS)
#define SOC_PA_IPQ_TRAFFIC_CLASS_MAP_SET_UNSAFE                                         ( 312|SOC_PETRA_PROC_BITS)
#define SOC_PA_IPQ_TRAFFIC_CLASS_MAP_GET_UNSAFE                                         ( 313|SOC_PETRA_PROC_BITS)
#define SOC_PA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_UNSAFE                      ( 314|SOC_PETRA_PROC_BITS)
#define SOC_PA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_UNSAFE                      ( 315|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_IPQ_QUARTET_RESET_UNSAFE                                              ( 329|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_IPQ_SYS_PHYSICAL_TO_DEST_PORT_SET                                     ( 339|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_SYS_PHYSICAL_TO_DEST_PORT_GET                                     ( 340|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_SYS_PHYSICAL_TO_DEST_PORT_VERIFY                                  ( 341|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_SYS_PHYSICAL_TO_DEST_PORT_SET_UNSAFE                              ( 342|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_SYS_PHYSICAL_TO_DEST_PORT_GET_UNSAFE                              ( 343|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LOCAL_TO_SYSTEM_PORT_SET                                          ( 344|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LOCAL_TO_SYSTEM_PORT_GET                                          ( 345|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LOCAL_TO_SYSTEM_PORT_VERIFY                                       ( 346|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LOCAL_TO_SYSTEM_PORT_SET_UNSAFE                                   ( 347|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LOCAL_TO_SYSTEM_PORT_GET_UNSAFE                                   ( 348|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LAG_GLOBAL_INFO_SET                                               ( 349|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LAG_GLOBAL_INFO_GET                                               ( 360|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LAG_GLOBAL_INFO_VERIFY                                            ( 361|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LAG_GLOBAL_INFO_SET_UNSAFE                                        ( 362|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_LAG_GLOBAL_INFO_GET_UNSAFE                                        ( 363|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_SET                                          ( 369|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_GET                                          ( 380|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_VERIFY                                       ( 381|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_SET_UNSAFE                                   ( 382|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_GET_UNSAFE                                   ( 383|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_ITM_CATEGORY_RNGS_SET                                                 ( 389|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CATEGORY_RNGS_GET                                                 ( 400|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CATEGORY_RNGS_VERIFY                                              ( 401|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CATEGORY_RNGS_SET_UNSAFE                                          ( 402|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CATEGORY_RNGS_GET_UNSAFE                                          ( 403|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ADMIT_TEST_TMPLT_SET                                              ( 404|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ADMIT_TEST_TMPLT_GET                                              ( 405|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ADMIT_TEST_TMPLT_VERIFY                                           ( 406|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ADMIT_TEST_TMPLT_SET_UNSAFE                                       ( 407|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ADMIT_TEST_TMPLT_GET_UNSAFE                                       ( 408|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_SET                                                    ( 409|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_GET                                                    ( 420|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_VERIFY                                                 ( 421|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_SET_UNSAFE                                             ( 422|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_GET_UNSAFE                                             ( 423|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_DISCOUNT_SET                                                   ( 424|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_DISCOUNT_GET                                                   ( 425|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_DISCOUNT_VERIFY                                                ( 426|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_DISCOUNT_SET_UNSAFE                                            ( 427|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_DISCOUNT_GET_UNSAFE                                            ( 428|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_EXP_WQ_SET                                                   ( 429|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_EXP_WQ_GET                                                   ( 430|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_EXP_WQ_SET_UNSAFE                                            ( 431|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_EXP_WQ_VERIFY                                                ( 432|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_EXP_WQ_GET_UNSAFE                                            ( 433|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_TAIL_DROP_SET                                                     ( 434|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_TAIL_DROP_GET                                                     ( 435|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_TAIL_DROP_SET_UNSAFE                                              ( 436|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_TAIL_DROP_GET_UNSAFE                                              ( 437|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_TAIL_DROP_VERIFY                                                  ( 438|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_ITM_WRED_SET                                                          ( 444|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_GET                                                          ( 445|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_SET_UNSAFE                                                   ( 446|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_VERIFY                                                       ( 447|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_GET_UNSAFE                                                   ( 448|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_WD_SET                                                         ( 449|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_WD_GET                                                         ( 460|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_WD_VERIFY                                                      ( 461|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_WD_SET_UNSAFE                                                  ( 462|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_WD_GET_UNSAFE                                                  ( 463|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_QT_RT_CLS_SET                                                 ( 464|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_QT_RT_CLS_GET                                                 ( 465|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_QT_RT_CLS_VERIFY                                              ( 466|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_QT_RT_CLS_SET_UNSAFE                                          ( 467|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_QT_RT_CLS_GET_UNSAFE                                          ( 468|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_FC_SET                                                        ( 469|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_FC_GET                                                        ( 480|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_FC_VERIFY                                                     ( 481|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_FC_SET_UNSAFE                                                 ( 482|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_FC_GET_UNSAFE                                                 ( 483|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_SET                                                      ( 484|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_GET                                                      ( 485|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_VERIFY                                                   ( 486|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_SET_UNSAFE                                               ( 487|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_GET_UNSAFE                                               ( 488|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_STAG_SET                                                          ( 489|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_STAG_GET                                                          ( 500|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_STAG_VERIFY                                                       ( 501|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_STAG_SET_UNSAFE                                                   ( 502|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_STAG_GET_UNSAFE                                                   ( 503|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_INFO_SET                                                    ( 504|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_INFO_GET                                                    ( 505|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_INFO_VERIFY                                                 ( 506|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_INFO_SET_UNSAFE                                             ( 507|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_INFO_GET_UNSAFE                                             ( 508|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_INGRESS_SHAPE_SET                                                 ( 509|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_INGRESS_SHAPE_GET                                                 ( 520|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_INGRESS_SHAPE_VERIFY                                              ( 521|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_INGRESS_SHAPE_SET_UNSAFE                                          ( 522|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_INGRESS_SHAPE_GET_UNSAFE                                          ( 523|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SET                                            ( 524|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_GET                                            ( 525|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_VERIFY                                         ( 526|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SET_UNSAFE                                     ( 527|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_GET_UNSAFE                                     ( 528|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SELECT_SET                                     ( 529|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SELECT_GET                                     ( 540|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SELECT_VERIFY                                  ( 541|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_UNSAFE                              ( 542|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_UNSAFE                              ( 543|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_DROP_PROB_SET                                             ( 544|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_DROP_PROB_GET                                             ( 545|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_DROP_PROB_VERIFY                                          ( 546|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_DROP_PROB_SET_UNSAFE                                      ( 547|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_DROP_PROB_GET_UNSAFE                                      ( 548|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET                                 ( 549|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET                                 ( 558|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_VERIFY                              ( 559|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_UNSAFE                          ( 560|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_UNSAFE                          ( 561|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_Q_BASED_SET                                               ( 562|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_Q_BASED_GET                                               ( 563|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_Q_BASED_VERIFY                                            ( 564|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_Q_BASED_SET_UNSAFE                                        ( 565|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_Q_BASED_GET_UNSAFE                                        ( 566|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_EG_SET                                                    ( 567|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_EG_GET                                                    ( 568|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_EG_VERIFY                                                 ( 569|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_EG_SET_UNSAFE                                             ( 570|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_EG_GET_UNSAFE                                             ( 571|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_SET                                              ( 572|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_GET                                              ( 573|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_VERIFY                                           ( 574|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_SET_UNSAFE                                       ( 575|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_GET_UNSAFE                                       ( 576|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_TEST_TMPLT_SET                                              ( 577|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_TEST_TMPLT_GET                                              ( 578|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_TEST_TMPLT_SET_UNSAFE                                       ( 579|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_TEST_TMPLT_VERIFY                                           ( 580|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_QUEUE_TEST_TMPLT_GET_UNSAFE                                       ( 581|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_TAIL_DROP_SET                                                 ( 582|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_TAIL_DROP_GET                                                 ( 583|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_TAIL_DROP_SET_UNSAFE                                          ( 585|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_TAIL_DROP_GET_UNSAFE                                          ( 586|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_TAIL_DROP_VERIFY                                              ( 587|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_TAIL_DROP_GET_DEFAULT_UNSAFE                                  ( 588|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_GEN_SET                                                  ( 589|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_GEN_GET                                                  ( 590|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_GEN_SET_UNSAFE                                           ( 591|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_GEN_VERIFY                                               ( 592|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_GEN_GET_UNSAFE                                           ( 593|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_FC_SET                                                   ( 594|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_FC_GET                                                   ( 595|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_DROP_SET                                                 ( 596|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_DROP_GET                                                 ( 597|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_FC_SET_UNSAFE                                            ( 598|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_FC_VERIFY                                                ( 599|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_FC_GET_UNSAFE                                            ( 600|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_DROP_SET_UNSAFE                                          ( 601|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_DROP_VERIFY                                              ( 602|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_GLOB_RCS_DROP_GET_UNSAFE                                          ( 603|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_MESH_SET                                            ( 611|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_MESH_GET                                            ( 612|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_MESH_VERIFY                                         ( 613|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_MESH_SET_UNSAFE                                     ( 614|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_MESH_GET_UNSAFE                                     ( 615|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_SET                                            ( 616|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_GET                                            ( 617|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_VERIFY                                         ( 618|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_SET_UNSAFE                                     ( 619|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_GET_UNSAFE                                     ( 620|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_SCH_QUEUE_TYPE_SET                                             ( 621|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_SCH_QUEUE_TYPE_GET                                             ( 622|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_SCH_QUEUE_TYPE_VERIFY                                          ( 623|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_SCH_QUEUE_TYPE_SET_UNSAFE                                      ( 624|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_SCH_QUEUE_TYPE_GET_UNSAFE                                      ( 625|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_QUEUES_CONF_SET                                                ( 626|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_QUEUES_CONF_GET                                                ( 627|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_QUEUES_CONF_VERIFY                                             ( 628|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_QUEUES_CONF_SET_UNSAFE                                         ( 629|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGRESS_QUEUES_CONF_GET_UNSAFE                                         ( 630|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_RATE_ENTRY_SET                                             ( 631|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_RATE_ENTRY_GET                                             ( 640|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_RATE_ENTRY_VERIFY                                          ( 641|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_RATE_ENTRY_SET_UNSAFE                                      ( 642|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_RATE_ENTRY_GET_UNSAFE                                      ( 643|SOC_PETRA_PROC_BITS)
#define SOC_PA_SCH_MAL_RATE_GET_UNSAFE                                                  ( 644|SOC_PETRA_PROC_BITS)
#define SOC_PA_SCH_MAL_RATE_SET_UNSAFE                                                  ( 645|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SCH_EGRESS_PORTS_QOS_SET                                              ( 669|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_EGRESS_PORTS_QOS_GET                                              ( 680|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_EGRESS_PORTS_QOS_VERIFY                                           ( 681|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_EGRESS_PORTS_QOS_SET_UNSAFE                                       ( 682|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_EGRESS_PORTS_QOS_GET_UNSAFE                                       ( 683|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_ONE_EGRESS_PORT_QOS_SET                                           ( 684|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_ONE_EGRESS_PORT_QOS_GET                                           ( 685|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_ONE_EGRESS_PORT_QOS_VERIFY                                        ( 686|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_ONE_EGRESS_PORT_QOS_SET_UNSAFE                                    ( 687|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_ONE_EGRESS_PORT_QOS_GET_UNSAFE                                    ( 688|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_SET                                             ( 689|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_GET                                             ( 700|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_VERIFY                                          ( 701|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_SET_UNSAFE                                      ( 702|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_GET_UNSAFE                                      ( 703|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_TABLE_SET                                       ( 704|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_TABLE_GET                                       ( 705|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_TABLE_VERIFY                                    ( 706|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_TABLE_SET_UNSAFE                                ( 707|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CLASS_TYPE_PARAMS_TABLE_GET_UNSAFE                                ( 708|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SLOW_MAX_RATES_SET                                                ( 709|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SLOW_MAX_RATES_GET                                                ( 720|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SLOW_MAX_RATES_VERIFY                                             ( 721|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SLOW_MAX_RATES_SET_UNSAFE                                         ( 722|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SLOW_MAX_RATES_GET_UNSAFE                                         ( 723|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_SET                                                          ( 724|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_GET                                                          ( 725|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_VERIFY                                                       ( 726|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_SET_UNSAFE                                                   ( 727|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_GET_UNSAFE                                                   ( 728|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_SCHED_VERIFY                                                 ( 729|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_SCHED_SET_UNSAFE                                             ( 730|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_SCHED_GET_UNSAFE                                             ( 731|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SCH_AGGREGATE_SET                                                     ( 749|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_AGGREGATE_GROUP_SET                                               ( 750|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_AGGREGATE_GET                                                     ( 760|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_AGGREGATE_VERIFY                                                  ( 761|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_AGGREGATE_SET_UNSAFE                                              ( 762|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_AGGREGATE_GET_UNSAFE                                              ( 763|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_DELETE_UNSAFE                                                ( 764|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_DELETE                                                       ( 765|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_AGGREGATE_GROUP_SET_UNSAFE                                        ( 766|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_ING_TRAFFIC_CLASS_MAP_SET                                        ( 800|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_TRAFFIC_CLASS_MAP_GET                                        ( 801|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_TRAFFIC_CLASS_MAP_VERIFY                                     ( 802|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_TRAFFIC_CLASS_MAP_SET_UNSAFE                                 ( 803|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_TRAFFIC_CLASS_MAP_GET_UNSAFE                                 ( 804|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_OPEN                                                   ( 805|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_UPDATE                                                 ( 806|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_CLOSE                                                  ( 807|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_DESTINATION_ADD                                              ( 808|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_DESTINATION_REMOVE                                           ( 809|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_SIZE_GET                                               ( 810|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_GET                                                    ( 811|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_ALL_GROUPS_CLOSE                                             ( 812|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_ING_GROUP_OPEN_UNSAFE                                            ( 820|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_UPDATE_UNSAFE                                          ( 821|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_CLOSE_UNSAFE                                           ( 822|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_DESTINATION_ADD_UNSAFE                                       ( 823|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_DESTINATION_REMOVE_UNSAFE                                    ( 824|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_SIZE_GET_UNSAFE                                        ( 825|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_GET_UNSAFE                                             ( 826|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_ALL_GROUPS_CLOSE_UNSAFE                                      ( 827|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_MULTICAST_ID_NDX_VERIFY                       ( 828|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET                               ( 829|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_GET                               ( 830|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_VERIFY                            ( 831|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_UNSAFE                        ( 832|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_GET_UNSAFE                        ( 833|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_OPEN                                                    ( 839|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_UPDATE                                                  ( 840|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_CLOSE                                                   ( 841|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_PORT_ADD                                                      ( 842|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_PORT_REMOVE                                                   ( 843|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_SIZE_GET                                                ( 844|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_GET                                                     ( 845|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_ALL_GROUPS_CLOSE                                              ( 846|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN                                    ( 847|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE                                  ( 848|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE                                   ( 849|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD                                      ( 850|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE                                   ( 851|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET                                     ( 852|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_ALL_GROUPS_CLOSE                              ( 853|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_EG_GROUP_OPEN_UNSAFE                                             ( 854|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_UPDATE_UNSAFE                                           ( 855|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_CLOSE_UNSAFE                                            ( 856|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_PORT_ADD_UNSAFE                                               ( 857|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_PORT_REMOVE_UNSAFE                                            ( 858|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_SIZE_GET_UNSAFE                                         ( 859|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_GET_UNSAFE                                              ( 860|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_ALL_GROUPS_CLOSE_UNSAFE                                       ( 861|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_UNSAFE                             ( 862|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_UNSAFE                           ( 863|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_UNSAFE                            ( 864|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE                               ( 865|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE                            ( 866|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_UNSAFE                              ( 867|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_ALL_GROUPS_CLOSE_UNSAFE                       ( 868|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET                    ( 869|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET                    ( 870|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_VERIFY                 ( 871|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_UNSAFE             ( 872|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_UNSAFE             ( 873|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_BASE_QUEUE_SET                                            ( 874|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_BASE_QUEUE_GET                                            ( 875|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_BASE_QUEUE_VERIFY                                         ( 876|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_BASE_QUEUE_SET_UNSAFE                                     ( 877|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_BASE_QUEUE_GET_UNSAFE                                     ( 878|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_SET                                         ( 879|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_GET                                         ( 880|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_SET_UNSAFE                                  ( 881|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_GET_UNSAFE                                  ( 882|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_SET                                          ( 883|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_GET                                          ( 884|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_FABRIC_LINKS_SERDES_PARAM_SET                                         ( 885|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_SERDES_PARAM_GET                                         ( 886|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_SERDES_PARAM_VERIFY                                      ( 887|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_SERDES_PARAM_SET_UNSAFE                                  ( 888|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_SERDES_PARAM_GET_UNSAFE                                  ( 889|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_LOGIC_CONF_SET                                           ( 890|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_LOGIC_CONF_GET                                           ( 891|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_LOGIC_CONF_VERIFY                                        ( 892|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_LOGIC_CONF_SET_UNSAFE                                    ( 893|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINKS_LOGIC_CONF_GET_UNSAFE                                    ( 894|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_MODE_SET                                                       ( 895|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_MODE_GET                                                       ( 896|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_MODE_VERIFY                                                    ( 897|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_MODE_SET_UNSAFE                                                ( 898|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_MODE_GET_UNSAFE                                                ( 899|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_SET                                       ( 900|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_GET                                       ( 920|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_VERIFY                                    ( 921|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_SET_UNSAFE                                ( 922|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_GET_UNSAFE                                ( 923|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_DETECT                                    ( 924|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_HANDLER                                                     ( 925|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STATISTICS_IF_INFO_SET                                                ( 926|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STATISTICS_IF_INFO_GET                                                ( 927|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STATISTICS_IF_INFO_VERIFY                                             ( 928|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STATISTICS_IF_INFO_SET_UNSAFE                                         ( 929|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STATISTICS_IF_INFO_GET_UNSAFE                                         ( 940|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SEND_SR_DATA_CELL                                                     ( 941|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_RECV_SR_DATA_CELL                                                     ( 942|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SEND_CONTROL_CELL                                                     ( 943|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PACKET_SEND                                                           ( 944|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PACKET_RECV                                                           ( 945|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_GET_BUFF_SIZE                                                     ( 949|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_TO_BUFF                                                           ( 960|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_FROM_BUFF                                                         ( 961|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_REGISTER_CALLBACK_FUNCTION                                            ( 962|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_UNREGISTER_CALLBACK_FUNCTION                                          ( 963|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_UNREGISTER_ALL_CALLBACK_FUNCTIONS                                     ( 964|SOC_PETRA_PROC_BITS)

#define SOC_PA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_UNSAFE                (1152|SOC_PETRA_PROC_BITS)
#define SOC_PA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_UNSAFE                (1153|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_VERIFY_ALL                                                        (1154|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_STATUS_GET_UNSAFE                                            (1158|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_SET_UNSAFE                                                (1159|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_VERIFY                                                    (1160|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_GET_UNSAFE                                                (1161|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POLARITY_SET_UNSAFE                                          (1162|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POLARITY_VERIFY                                              (1163|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POLARITY_GET_UNSAFE                                          (1164|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_DIVISOR_VERIFY                                               (1165|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_CMU_SET_UNSAFE                                            (1166|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_CMU_VERIFY                                                (1167|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_CMU_GET_UNSAFE                                            (1168|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_STATUS_INFO_GET                                              (1169|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POLARITY_SET                                                 (1171|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POLARITY_GET                                                 (1172|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_SET_UNSAFE                                                (1173|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_VERIFY                                                    (1174|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_GET_UNSAFE                                                (1175|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_SET_ALL_UNSAFE                                                    (1176|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_GET_ALL_UNSAFE                                                    (1177|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRINT_ALL_UNSAFE                                                  (1178|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_SYNC_UNSAFE                                                  (1179|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REF_CLOCK_SET                                                     (1180|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REF_CLOCK_GET                                                     (1181|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_CMU_SET                                                   (1182|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_CMU_GET                                                   (1183|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_SET_ALL                                                           (1184|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_GET_ALL                                                           (1185|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRINT_ALL                                                         (1186|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_SYNC                                                         (1187|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DEST_SYS_PORT_INFO_VERIFY                                             (1188|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DEST_INFO_VERIFY                                                      (1189|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_SHAPER_RATE_SET                                                (1190|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_SHAPER_RATE_GET                                                (1191|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_IDX_SET                                          (1192|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_IDX_GET                                          (1193|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_WEIGHT_CONF_SET                                                (1194|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_WEIGHT_CONF_GET                                                (1195|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_SHAPER_RATE_VERIFY                                             (1196|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_IDX_SET_UNSAFE                                   (1197|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_IDX_VERIFY                                       (1198|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_IDX_GET_UNSAFE                                   (1199|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_WEIGHT_CONF_SET_UNSAFE                                         (1200|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_WEIGHT_CONF_VERIFY                                             (1201|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_WEIGHT_CONF_GET_UNSAFE                                         (1202|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERFACE_ID_VERIFY                                                   (1203|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_DEVICE_SCH_INITIALIZE                                           (1204|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_DEVICE_INIT                                                     (1205|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_DEVICE_CLOSE                                                    (1206|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_INIT                                                            (1207|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FAP_PORT_ID_VERIFY                                                    (1208|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DROP_PRECEDENCE_VERIFY                                                (1209|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TRAFFIC_CLASS_VERIFY                                                  (1210|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_DEV_EGR_PORTS_INITIALIZE                                        (1211|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_MULTICAST_INITIALIZE                                            (1212|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SRD_LANE_REG_WRITE                                                    (1213|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_REG_WRITE_UNSAFE                                             (1214|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_REG_READ                                                     (1215|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_REG_READ_UNSAFE                                              (1216|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_RATE_FACTORS_SET                                          (1217|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_RATE_FACTORS_SET_UNSAFE                                   (1218|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_RATE_FACTORS_GET                                          (1219|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_RATE_FACTORS_VERIFY                                       (1220|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QUARTET_RATE_FACTORS_GET_UNSAFE                                   (1221|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_STATE_SET                                                    (1222|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_STATE_SET_UNSAFE                                             (1223|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_STATE_GET                                                    (1224|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_STATE_VERIFY                                                 (1225|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_STATE_GET_UNSAFE                                             (1226|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_DIVISOR_SET                                                  (1227|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_DIVISOR_SET_UNSAFE                                           (1228|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_DIVISOR_GET                                                  (1229|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_DIVISOR_VERIFY                                               (1230|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_DIVISOR_GET_UNSAFE                                           (1231|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_SET                                          (1232|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_SET_UNSAFE                                   (1233|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_GET                                          (1234|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_VERIFY                                       (1235|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_GET_UNSAFE                                   (1236|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_SET                                                       (1237|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_GET                                                       (1238|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_REG_WRITE                                                     (1239|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_REG_WRITE_UNSAFE                                              (1240|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_REG_READ                                                      (1241|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_REG_READ_UNSAFE                                               (1242|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_INIT                                                          (1243|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_INIT_UNSAFE                                                   (1244|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_CMU_REG_WRITE                                                     (1245|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_CMU_REG_WRITE_UNSAFE                                              (1246|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_CMU_REG_READ                                                      (1247|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_CMU_REG_READ_UNSAFE                                               (1248|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_FACTORS_CALCULATE                                            (1249|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_FACTORS_CALCULATE_UNSAFE                                     (1250|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_INFO_CALCULATE                                      (1251|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_INFO_CALCULATE_UNSAFE                               (1252|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LINK_RX_EYE_MONITOR                                               (1253|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LINK_RX_EYE_MONITOR_UNSAFE                                        (1254|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_LOOPBACK_MODE_SET                                            (1255|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_LOOPBACK_MODE_SET_UNSAFE                                     (1256|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_LOOPBACK_MODE_GET                                            (1257|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_LOOPBACK_MODE_VERIFY                                         (1258|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_LOOPBACK_MODE_GET_UNSAFE                                     (1259|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_SET                                                          (1262|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_SET_UNSAFE                                                   (1263|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_GET                                                          (1264|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_GET_UNSAFE                                                   (1265|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_VERIFY                                                       (1266|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SRD_REGS_FIELD_FROM_REG_SET                                           (1270|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_FIELD_FROM_REG_GET                                           (1271|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_WRITE_REG_UNSAFE                                             (1272|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_WRITE_REG                                                    (1273|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_READ_REG                                                     (1274|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_WRITE_FLD                                                    (1275|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_READ_FLD                                                     (1276|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_SET_DO_PRINTS                                                (1277|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_SCIF_CNTRL                                                   (1278|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SRD_LLA_RDWR_VERIFY                                                   (1279|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_SET_DO_PRINTS                                                 (1280|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_WRITE                                                         (1281|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_READ                                                          (1282|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_EPB_BITSTREAM_TO_EPB_CMD                                      (1283|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_EPB_EPB_CMD_TO_BITSTREAM                                      (1284|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_EPB_CMD_WRITE                                                 (1285|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_EPB_CMD_READ                                                  (1286|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_EPB_WRITE                                                     (1287|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LLA_EPB_READ                                                      (1288|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_ACTUAL_ID_GET                                                (1289|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGGS_DEFAULT_GET                                                 (1290|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_INFO_ITEM_SET                                                (1291|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_INFO_ITEM_GET                                                (1292|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGGS_ACTUAL_IDS_SET                                              (1293|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_ALL_AGGREGATES_OPEN_UNSAFE                                       (1294|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_CLOSE_PORT_AGGS                                                  (1295|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_UPDATE_PORT_AGGS                                                 (1296|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_PORT_SCHEME_AGGREGATES_PRINT                                 (1297|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_FLOWS_GET_DEFAULT                                                (1298|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_FLOW_SET_PHYSICAL_IDS                                            (1299|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_FLOW_PORT_FLOWS_OPEN                                             (1301|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_CLOSE_PORT_FLOWS                                                 (1302|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_UPDATE_PORT_FLOWS                                                (1303|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_ALL_FLOWS_OPEN_UNSAFE                                            (1304|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_API_AUTO_PRINT_PORT_SCHEME_FLOWS                                      (1305|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_SYSTEM_INFO_SAVE                                                 (1306|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_SYSTEM_INFO_DEFAULTS_GET                                         (1307|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_ALL_PORTS_OPEN                                                   (1309|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_ALL_AGGREGATES_OPEN                                              (1310|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_ALL_QUEUES_OPEN                                                  (1311|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_ALL_FLOWS_OPEN                                                   (1312|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_OPEN                                                        (1313|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_UPDATE                                                      (1314|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_CLOSE                                                       (1315|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_PORT_AGGS_OPEN                                               (1316|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_PORT_AGGS_CLOSE                                              (1317|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_PORT_AGGS_UPDATE                                             (1318|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_PORT_SINGLE_AGGS_UPDATE                                      (1319|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_QUEUES_OPEN                                                      (1320|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_QUEUES_UPDATE                                                    (1321|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_QUEUES_CLOSE                                                     (1322|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_FLOW_OPEN                                                        (1323|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_FLOW_UPDATE                                                      (1324|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_FLOW_CLOSE                                                       (1325|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_SYSTEM_INFO_SAVE_UNSAFE                                          (1326|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_FIRST_RELATIVE_ID_GET                                       (1327|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_ACTUAL2RELATIVE_GET                                         (1328|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_QUEUE_INFO_DEFAULT_GET                                           (1329|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_RELATIVE2ACTUAL_GET                                         (1330|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_RELATIVE_ID_GET                                              (1331|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_QUEUE_ID_GET                                                     (1332|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_FLOW_ID_GET                                                      (1333|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_SYSTEM_PHYSICAL_PORT_ID_GET                                      (1334|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_DESTINATION_ID_GET                                               (1335|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_AGG_BASE_IDS_GET                                                 (1336|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_QUEUE_PORT_SCHEME_QUEUES_PRINT                                   (1337|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_ALL_QUEUES_OPEN_UNSAFE                                           (1338|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_OPEN_PORT_AGGS                                                   (1339|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_DEFAULT_GET                                                 (1340|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_SCHEDULER_PORT_DEFAULT_GET                                  (1341|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_ALL_PORTS_OPEN_UNSAFE                                            (1342|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_UPDATE_UNSAFE                                               (1343|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_PORT_PORT_SCHEME_PORT_PRINT                                      (1344|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_GET_CREDIT_SOURCES                                               (1345|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_NOF_RELATIVE_PORT_GET                                            (1346|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SW_DB_AUTO_SCHEME_INITIALIZE                                          (1350|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_SEND_UNSAFE                                                (1351|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_RECV_UNSAFE                                                (1352|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_SEND                                                       (1353|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_RECV                                                       (1354|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_TX_LOAD_MEM                                                (1355|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_TX_LOAD_MEM_UNSAFE                                         (1356|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_PKT_COUNTERS_COLLECT                                             (1357|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_COUNTER_GET                                                      (1358|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_ALL_COUNTERS_GET                                                 (1359|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_PKT_COUNTERS_COLLECT_UNSAFE                                      (1362|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_COUNTER_GET_UNSAFE                                               (1363|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_ALL_COUNTERS_GET_UNSAFE                                          (1364|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_COUNTER_TO_DEVICE_COUNTERS_ADD                                        (1367|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_STATISTICS_MODULE_INITIALIZE                                     (1368|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_STATISTICS_DEVICE_INITIALIZE                                     (1369|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_STATISTICS_DEFERRED_COUNTER_CLEAR                                (1370|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_COUNTER_INFO_GET                                                 (1371|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_PKT_CNT_CALLBACK_GET                                             (1372|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TODO_ERR_DEF                                                          (1399|SOC_PETRA_PROC_BITS)
/*
 * soc_petra_reg_access.c
 * soc_petra_tbl_access.c
 */
#define SOC_PETRA_READ_FLD                                                              (1400|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_WRITE_FLD                                                             (1401|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_READ_REG                                                              (1402|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_WRITE_REG                                                             (1403|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_READ_FLD_UNSAFE                                                       (1404|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_WRITE_FLD_UNSAFE                                                      (1405|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_READ_REG_UNSAFE                                                       (1406|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_WRITE_REG_UNSAFE                                                      (1407|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_GET_BLKN_TBLN_ENTRY                                                   (1408|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_GET_BLKN_TBLN_ENTRY_UNSAFE                                            (1409|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_WRITE_REG_BUFFER_UNSAFE                                               (1410|SOC_PETRA_PROC_BITS)

/*
 * soc_petra_chip_regs.c
 * soc_petra_chip_tbls.c
 */
#define SOC_PETRA_REGS_GET                                                              (1420|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_REGS_INIT                                                             (1421|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TBLS_GET                                                              (1422|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TBLS_INIT                                                             (1423|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_WRITE_ARRAY_OF_FLDS                                                   (1424|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_READ_ARRAY_OF_FLDS                                                    (1425|SOC_PETRA_PROC_BITS)

/*
 * soc_petra_tbl_access.c {
 */
#define SOC_PETRA_FIELD_IN_PLACE_SET                                                    (1489|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FIELD_IN_PLACE_GET                                                    (1490|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MGMT_INDIRECT_MEMORY_MAP_GET                                          (1491|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INDIRECT_MEMORY_MAP_INIT                                         (1492|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INDIRECT_TABLE_MAP_GET                                           (1493|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INDIRECT_TABLE_MAP_INIT                                          (1494|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INDIRECT_MODULE_INFO_GET                                         (1495|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INDIRECT_MODULE_INFO_INIT                                        (1496|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INDIRECT_MODULE_INIT                                             (1497|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ACCESS_DB_INIT                                                        (1498|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_OLP_PGE_MEM_TBL_GET_UNSAFE                                            (1501|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OLP_PGE_MEM_TBL_SET_UNSAFE                                            (1503|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_GET_UNSAFE                                       (1505|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_SET_UNSAFE                                       (1507|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_GET_UNSAFE                              (1509|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_SET_UNSAFE                              (1511|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_GET_UNSAFE                                       (1513|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_SET_UNSAFE                                       (1515|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IDR_COMPLETE_PC_TBL_GET_UNSAFE                                        (1517|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IDR_COMPLETE_PC_TBL_SET_UNSAFE                                        (1519|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_GET_UNSAFE                          (1521|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_SET_UNSAFE                          (1523|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_MIRROR_TABLE_TBL_GET_UNSAFE                                       (1525|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_MIRROR_TABLE_TBL_SET_UNSAFE                                       (1527|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_SNOOP_TABLE_TBL_GET_UNSAFE                                        (1529|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_SNOOP_TABLE_TBL_SET_UNSAFE                                        (1531|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_GET_UNSAFE                                  (1533|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_SET_UNSAFE                                  (1535|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_GET_UNSAFE                                    (1537|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_SET_UNSAFE                                    (1539|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_GLAG_MAPPING_TBL_GET_UNSAFE                                       (1541|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_GLAG_MAPPING_TBL_SET_UNSAFE                                       (1543|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_DESTINATION_TABLE_TBL_GET_UNSAFE                                  (1545|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_DESTINATION_TABLE_TBL_SET_UNSAFE                                  (1547|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_GET_UNSAFE                                   (1549|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_SET_UNSAFE                                   (1551|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_GET_UNSAFE                                   (1553|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_SET_UNSAFE                                   (1555|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_INFO_TBL_GET_UNSAFE                                          (1557|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_INFO_TBL_SET_UNSAFE                                          (1559|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_GET_UNSAFE                             (1561|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_SET_UNSAFE                             (1563|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_STATIC_HEADER_TBL_GET_UNSAFE                                      (1565|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_STATIC_HEADER_TBL_SET_UNSAFE                                      (1567|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_GET_UNSAFE                          (1569|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_SET_UNSAFE                          (1571|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PTC_COMMANDS1_TBL_GET_UNSAFE                                      (1573|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PTC_COMMANDS1_TBL_SET_UNSAFE                                      (1575|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PTC_COMMANDS2_TBL_GET_UNSAFE                                      (1577|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PTC_COMMANDS2_TBL_SET_UNSAFE                                      (1579|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_GET_UNSAFE                                (1581|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_SET_UNSAFE                                (1583|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM0_TBL_GET_UNSAFE                                       (1585|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM0_TBL_SET_UNSAFE                                       (1587|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM1_TBL_GET_UNSAFE                                       (1589|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM1_TBL_SET_UNSAFE                                       (1591|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM2_TBL_GET_UNSAFE                                       (1593|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM2_TBL_SET_UNSAFE                                       (1595|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM3_TBL_GET_UNSAFE                                       (1597|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM3_TBL_SET_UNSAFE                                       (1599|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM4_TBL_GET_UNSAFE                                       (1601|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM4_TBL_SET_UNSAFE                                       (1603|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_GET_UNSAFE                                   (1605|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_SET_UNSAFE                                   (1607|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_GET_UNSAFE                                  (1609|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_SET_UNSAFE                                  (1611|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_EYE_SCAN_RUN                                                      (1612|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_EYE_SCAN_RUN_MULTIPLE_ALLOC                                       (1613|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_EYE_SCAN_RUN_MULTIPLE_ALLOC_UNSAFE                                (1614|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_IQM_BDB_LINK_LIST_TBL_GET_UNSAFE                                      (1621|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_BDB_LINK_LIST_TBL_SET_UNSAFE                                      (1623|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_DYNAMIC_TBL_GET_UNSAFE                                            (1625|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_DYNAMIC_TBL_SET_UNSAFE                                            (1627|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_STATIC_TBL_GET_UNSAFE                                             (1629|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_STATIC_TBL_SET_UNSAFE                                             (1631|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_GET_UNSAFE                          (1633|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_SET_UNSAFE                          (1635|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_GET_UNSAFE                      (1637|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_SET_UNSAFE                      (1639|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_GET_UNSAFE                              (1641|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_SET_UNSAFE                              (1643|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_GET_UNSAFE                             (1645|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_SET_UNSAFE                             (1647|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_GET_UNSAFE                   (1649|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_SET_UNSAFE                   (1651|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_GET_UNSAFE                  (1653|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE                  (1655|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_GET_UNSAFE                  (1657|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_SET_UNSAFE                  (1659|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_GET_UNSAFE                  (1661|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_SET_UNSAFE                  (1663|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_GET_UNSAFE                  (1665|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_SET_UNSAFE                  (1667|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_GET_UNSAFE                  (1669|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_SET_UNSAFE                  (1671|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_GET_UNSAFE                           (1673|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_SET_UNSAFE                           (1675|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_GET_UNSAFE                           (1677|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_SET_UNSAFE                           (1679|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_GET_UNSAFE                           (1681|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_SET_UNSAFE                           (1683|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_GET_UNSAFE                           (1685|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_SET_UNSAFE                           (1687|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_GET_UNSAFE                   (1689|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_SET_UNSAFE                   (1691|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_GET_UNSAFE                   (1693|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_SET_UNSAFE                   (1695|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_GET_UNSAFE                   (1697|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_SET_UNSAFE                   (1699|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_GET_UNSAFE                   (1701|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_SET_UNSAFE                   (1703|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_GET_UNSAFE            (1705|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE            (1707|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_GET_UNSAFE                   (1721|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE                   (1723|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_GET_UNSAFE                        (1737|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE                        (1739|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_GET_UNSAFE                 (1741|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_SET_UNSAFE                 (1743|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_SYSTEM_RED_TBL_GET_UNSAFE                                         (1745|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_SYSTEM_RED_TBL_SET_UNSAFE                                         (1746|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_QDR_MEMORY_TBL_GET_UNSAFE                                             (1747|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_QDR_MEMORY_TBL_SET_UNSAFE                                             (1748|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_GET_UNSAFE                  (1749|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE                  (1751|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_GET_UNSAFE           (1753|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE           (1755|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_GET_UNSAFE                               (1757|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_UNSAFE                               (1759|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_GET_UNSAFE                            (1761|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_UNSAFE                            (1763|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_GET_UNSAFE                          (1765|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_UNSAFE                          (1767|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_GET_UNSAFE                          (1769|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_UNSAFE                          (1771|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_GET_UNSAFE                  (1773|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_UNSAFE                  (1775|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_GET_UNSAFE              (1777|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_UNSAFE              (1779|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_GET_UNSAFE                   (1781|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_UNSAFE                   (1783|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_GET_UNSAFE                   (1785|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_UNSAFE                   (1787|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_GET_UNSAFE                             (1789|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_UNSAFE                             (1791|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_GET_UNSAFE                                   (1793|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_SET_UNSAFE                                   (1795|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_GET_UNSAFE                    (1797|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_UNSAFE                    (1799|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DPI_DLL_RAM_TBL_GET_UNSAFE                                            (1805|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DPI_DLL_RAM_TBL_SET_UNSAFE                                            (1807|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_GET_UNSAFE         (1809|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_UNSAFE         (1811|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_GET_UNSAFE      (1813|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_UNSAFE      (1815|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_GET_UNSAFE                                       (1817|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_SET_UNSAFE                                       (1819|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_GET_UNSAFE                                       (1821|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_SET_UNSAFE                                       (1823|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_GET_UNSAFE                                       (1825|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_SET_UNSAFE                                       (1827|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_GET_UNSAFE                                       (1829|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_SET_UNSAFE                                       (1831|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_GET_UNSAFE                                       (1833|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_SET_UNSAFE                                       (1835|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_GET_UNSAFE                                       (1837|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_SET_UNSAFE                                       (1839|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_GET_UNSAFE                                       (1841|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_SET_UNSAFE                                       (1843|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_GET_UNSAFE                                       (1845|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_SET_UNSAFE                                       (1847|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_GET_UNSAFE                                      (1849|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_SET_UNSAFE                                      (1851|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RCY_SCM_TBL_GET_UNSAFE                                            (1853|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RCY_SCM_TBL_SET_UNSAFE                                            (1855|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CPU_SCM_TBL_GET_UNSAFE                                            (1857|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CPU_SCM_TBL_SET_UNSAFE                                            (1859|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CCM_TBL_GET_UNSAFE                                                (1861|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CCM_TBL_SET_UNSAFE                                                (1863|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PMC_TBL_GET_UNSAFE                                                (1865|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PMC_TBL_SET_UNSAFE                                                (1867|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FBM_TBL_GET_UNSAFE                                                (1869|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FBM_TBL_SET_UNSAFE                                                (1871|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FDM_TBL_GET_UNSAFE                                                (1873|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FDM_TBL_SET_UNSAFE                                                (1875|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_DWM_TBL_GET_UNSAFE                                                (1877|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_DWM_TBL_SET_UNSAFE                                                (1879|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RRDM_TBL_GET_UNSAFE                                               (1881|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RRDM_TBL_SET_UNSAFE                                               (1883|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RPDM_TBL_GET_UNSAFE                                               (1885|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RPDM_TBL_SET_UNSAFE                                               (1887|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PCT_TBL_GET_UNSAFE                                                (1889|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PCT_TBL_SET_UNSAFE                                                (1891|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_VLAN_TABLE_TBL_GET_UNSAFE                                         (1893|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_VLAN_TABLE_TBL_SET_UNSAFE                                         (1895|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_TDMMCID_TBL_GET_UNSAFE                                            (1897|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_TDMMCID_TBL_SET_UNSAFE                                            (1899|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFA_CH0_SCM_TBL_GET_UNSAFE                                      (1901|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFA_CH0_SCM_TBL_SET_UNSAFE                                      (1903|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFA_CH1_SCM_TBL_GET_UNSAFE                                      (1905|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFA_CH1_SCM_TBL_SET_UNSAFE                                      (1907|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFA_CH2_SCM_TBL_GET_UNSAFE                                      (1909|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFA_CH2_SCM_TBL_SET_UNSAFE                                      (1911|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFA_CH3_SCM_TBL_GET_UNSAFE                                      (1913|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFA_CH3_SCM_TBL_SET_UNSAFE                                      (1915|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFB_CH0_SCM_TBL_GET_UNSAFE                                      (1917|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFB_CH0_SCM_TBL_SET_UNSAFE                                      (1919|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFB_CH1_SCM_TBL_GET_UNSAFE                                      (1921|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFB_CH1_SCM_TBL_SET_UNSAFE                                      (1923|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFB_CH2_SCM_TBL_GET_UNSAFE                                      (1925|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFB_CH2_SCM_TBL_SET_UNSAFE                                      (1927|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFB_CH3_SCM_TBL_GET_UNSAFE                                      (1929|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFB_CH3_SCM_TBL_SET_UNSAFE                                      (1931|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFAB_NCH_SCM_TBL_GET_UNSAFE                                     (1933|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_NIFAB_NCH_SCM_TBL_SET_UNSAFE                                     (1935|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_RCY_SCM_TBL_GET_UNSAFE                                           (1937|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_RCY_SCM_TBL_SET_UNSAFE                                           (1939|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_CPU_SCM_TBL_GET_UNSAFE                                           (1941|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_CPU_SCM_TBL_SET_UNSAFE                                           (1943|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_CCM_TBL_GET_UNSAFE                                               (1945|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_CCM_TBL_SET_UNSAFE                                               (1947|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_PMC_TBL_GET_UNSAFE                                               (1949|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_PMC_TBL_SET_UNSAFE                                               (1951|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_CBM_TBL_GET_UNSAFE                                               (1953|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_CBM_TBL_SET_UNSAFE                                               (1955|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_FBM_TBL_GET_UNSAFE                                               (1957|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_FBM_TBL_SET_UNSAFE                                               (1959|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_FDM_TBL_GET_UNSAFE                                               (1961|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_FDM_TBL_SET_UNSAFE                                               (1963|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_DWM_TBL_GET_UNSAFE                                               (1965|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_DWM_TBL_SET_UNSAFE                                               (1967|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_RRDM_TBL_GET_UNSAFE                                              (1969|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_RRDM_TBL_SET_UNSAFE                                              (1971|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_RPDM_TBL_GET_UNSAFE                                              (1973|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_RPDM_TBL_SET_UNSAFE                                              (1975|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_PCT_TBL_GET_UNSAFE                                               (1977|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_PCT_TBL_SET_UNSAFE                                               (1979|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_VLAN_TABLE_TBL_GET_UNSAFE                                        (1981|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_VLAN_TABLE_TBL_SET_UNSAFE                                        (1983|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_GET_UNSAFE              (1985|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_SET_UNSAFE              (1987|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_GET_UNSAFE                   (1989|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_SET_UNSAFE                   (1991|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_GET_UNSAFE                   (1993|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_SET_UNSAFE                   (1995|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_GET_UNSAFE     (1997|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_SET_UNSAFE     (1999|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_GET_UNSAFE     (2001|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_SET_UNSAFE     (2003|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_GET_UNSAFE                  (2005|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_SET_UNSAFE                  (2007|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_GET_UNSAFE                  (2009|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_SET_UNSAFE                  (2011|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_GET_UNSAFE                    (2013|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_SET_UNSAFE                    (2015|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CAL_TBL_GET_UNSAFE                                                (2017|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_CAL_TBL_SET_UNSAFE                                                (2019|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DRM_TBL_GET_UNSAFE                                                (2021|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DRM_TBL_SET_UNSAFE                                                (2023|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DSM_TBL_GET_UNSAFE                                                (2025|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DSM_TBL_SET_UNSAFE                                                (2027|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FDMS_TBL_GET_UNSAFE                                               (2029|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FDMS_TBL_SET_UNSAFE                                               (2031|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SHDS_TBL_GET_UNSAFE                                               (2033|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SHDS_TBL_SET_UNSAFE                                               (2035|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SEM_TBL_GET_UNSAFE                                                (2037|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SEM_TBL_SET_UNSAFE                                                (2039|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FSF_TBL_GET_UNSAFE                                                (2041|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FSF_TBL_SET_UNSAFE                                                (2043|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FGM_TBL_GET_UNSAFE                                                (2045|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FGM_TBL_SET_UNSAFE                                                (2047|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SHC_TBL_GET_UNSAFE                                                (2049|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SHC_TBL_SET_UNSAFE                                                (2051|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SCC_TBL_GET_UNSAFE                                                (2053|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SCC_TBL_SET_UNSAFE                                                (2055|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SCT_TBL_GET_UNSAFE                                                (2057|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SCT_TBL_SET_UNSAFE                                                (2059|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FQM_TBL_GET_UNSAFE                                                (2061|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FQM_TBL_SET_UNSAFE                                                (2063|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FFM_TBL_GET_UNSAFE                                                (2065|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FFM_TBL_SET_UNSAFE                                                (2067|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_TMC_TBL_GET_UNSAFE                                                (2069|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_TMC_TBL_SET_UNSAFE                                                (2071|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PQS_TBL_GET_UNSAFE                                                (2073|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PQS_TBL_SET_UNSAFE                                                (2075|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SCHEDULER_INIT_TBL_GET_UNSAFE                                     (2077|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SCHEDULER_INIT_TBL_SET_UNSAFE                                     (2079|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_GET_UNSAFE                               (2081|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_SET_UNSAFE                               (2083|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CBM_TBL_GET_UNSAFE                                                (2085|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CBM_TBL_SET_UNSAFE                                                (2087|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_GET_UNSAFE                   (2088|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_GET_UNSAFE                    (2089|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_BDQ_TBL_GET_UNSAFE                                                (2091|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_BDQ_TBL_SET_UNSAFE                                                (2093|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_PCQ_TBL_GET_UNSAFE                                                (2095|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_PCQ_TBL_SET_UNSAFE                                                (2097|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_SOP_MMU_TBL_GET_UNSAFE                                            (2099|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_SOP_MMU_TBL_SET_UNSAFE                                            (2101|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_MOP_MMU_TBL_GET_UNSAFE                                            (2103|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_MOP_MMU_TBL_SET_UNSAFE                                            (2105|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_FDTCTL_TBL_GET_UNSAFE                                             (2107|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_FDTCTL_TBL_SET_UNSAFE                                             (2109|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_FDTDATA_TBL_GET_UNSAFE                                            (2111|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_FDTDATA_TBL_SET_UNSAFE                                            (2113|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_EGQCTL_TBL_GET_UNSAFE                                             (2115|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_EGQCTL_TBL_SET_UNSAFE                                             (2117|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_EGQDATA_TBL_GET_UNSAFE                                            (2119|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_EGQDATA_TBL_SET_UNSAFE                                            (2121|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_GET_UNSAFE                                   (2123|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL_SET_UNSAFE                                   (2125|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_GET_UNSAFE                                  (2127|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL_SET_UNSAFE                                  (2129|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_GET_UNSAFE                                  (2131|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL_SET_UNSAFE                                  (2133|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_GET_UNSAFE                                    (2135|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL_SET_UNSAFE                                    (2137|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_GET_UNSAFE                                    (2139|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL_SET_UNSAFE                                    (2141|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_GET_UNSAFE                                    (2143|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL_SET_UNSAFE                                    (2145|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2147|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2149|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2151|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2153|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2155|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2157|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2159|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2161|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_GET_UNSAFE                         (2163|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_UNSAFE                         (2165|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_GET_UNSAFE                         (2167|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_UNSAFE                         (2169|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE                  (2171|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE                  (2173|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE                   (2175|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE                   (2177|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_GET_UNSAFE                         (2179|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_UNSAFE                         (2181|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE                    (2183|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE                    (2185|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE                     (2187|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE                     (2189|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE                  (2191|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE                  (2193|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE                   (2195|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE                   (2197|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_GET_UNSAFE                     (2199|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_UNSAFE                     (2201|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_GET_UNSAFE                      (2203|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_UNSAFE                      (2205|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_GET_UNSAFE                   (2207|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_UNSAFE                   (2209|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_GET_UNSAFE                    (2211|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_UNSAFE                    (2213|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_GET_UNSAFE                 (2215|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_UNSAFE                 (2217|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_GET_UNSAFE               (2219|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_UNSAFE               (2221|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2223|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2225|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_GET_UNSAFE                                 (2231|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_SET_UNSAFE                                 (2233|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_INFO_GET_UNSAFE                                (2234|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_IDF_TBL_GET_UNSAFE                                                (2235|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_IDF_TBL_SET_UNSAFE                                                (2237|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_FDF_TBL_GET_UNSAFE                                                (2239|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_FDF_TBL_SET_UNSAFE                                                (2241|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_GET_UNSAFE                                  (2243|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL_SET_UNSAFE                                  (2245|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_GET_UNSAFE                                  (2247|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL_SET_UNSAFE                                  (2249|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_GET_UNSAFE                                  (2251|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL_SET_UNSAFE                                  (2253|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_GET_UNSAFE                                  (2255|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL_SET_UNSAFE                                  (2257|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_GET_UNSAFE                                  (2259|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL_SET_UNSAFE                                  (2261|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_GET_UNSAFE                                  (2263|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL_SET_UNSAFE                                  (2265|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDF_RADDR_TBL_GET_UNSAFE                                          (2267|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RDF_RADDR_TBL_SET_UNSAFE                                          (2269|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_GET_UNSAFE                                    (2271|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL_SET_UNSAFE                                    (2273|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_GET_UNSAFE                                    (2275|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL_SET_UNSAFE                                    (2277|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2279|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2281|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2283|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2285|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2287|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2289|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2291|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2293|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2295|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2297|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2299|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2301|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2303|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2305|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2307|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2309|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2311|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2313|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2315|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2317|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2319|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2321|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2323|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2325|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAF_WADDR_TBL_GET_UNSAFE                                          (2327|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAF_WADDR_TBL_SET_UNSAFE                                          (2329|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_GET_UNSAFE                                  (2331|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL_SET_UNSAFE                                  (2333|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_GET_UNSAFE                                  (2335|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL_SET_UNSAFE                                  (2337|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_GET_UNSAFE                                  (2339|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL_SET_UNSAFE                                  (2341|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_GET_UNSAFE                                  (2343|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL_SET_UNSAFE                                  (2345|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_GET_UNSAFE                                  (2347|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL_SET_UNSAFE                                  (2349|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_GET_UNSAFE                                  (2351|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL_SET_UNSAFE                                  (2353|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_SET_UNSAFE                                  (2354|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_GET_UNSAFE                     (2355|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_SET_UNSAFE                     (2356|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_GET_UNSAFE                         (2357|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_SET_UNSAFE                         (2358|SOC_PETRA_PROC_BITS)

/*
 * soc_petra_tbl_access.c }
 */

/*
 * access tables
 */
#define SOC_PETRA_TBL_READ_LOW                                                          (2426|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TBL_WRITE_LOW                                                         (2427|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TBL_READ_LOW_UNSAFE                                                   (2428|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TBL_WRITE_LOW_UNSAFE                                                  (2429|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TBL_FIELD_MAX_VALUE_GET                                               (2430|SOC_PETRA_PROC_BITS)

/*
 * soc_petra_mgmt {
 */
#define SOC_PETRA_MGMT_SYSTEM_FAP_ID_GET                                                (2431|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_SYSTEM_FAP_ID_GET_UNSAFE                                         (2432|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_SYSTEM_FAP_ID_SET                                                (2433|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_SYSTEM_FAP_ID_SET_UNSAFE                                         (2434|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_SYSTEM_FAP_ID_VERIFY                                             (2435|SOC_PETRA_PROC_BITS)
/*
 * soc_petra_mgmt }
 */

/*
 * soc_petra_api_framework.c
 */
#define SOC_PETRA_GET_ERR_TEXT                                                          (2445|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PROC_ID_TO_STRING                                                     (2446|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_VERIFY                                      (2450|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_SET_UNSAFE                   (2451|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_SET_UNSAFE                    (2452|SOC_PETRA_PROC_BITS)

/*
 * soc_petra_sw_db {
 */

/*
 * soc_petra_sw_db }
 */

#define SOC_PETRA_INTERNAL_FUNCS_BASE                                                   ((2500))

#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_WEIGHTS_SET                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +    1)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_GLOBAL_SHAPERS_SET                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +    2)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_HP_SHAPERS_SET                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +    3)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_SHAPER_VALUES_SET                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +    4)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_WEIGHT_SET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +    5)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_RATE_TO_DELAY_CAL_FORM                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +    6)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_MESH_REG_FLDS_DB_GET                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +    7)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_WEIGHT_GET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +    8)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_SHAPER_VALUES_GET                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +    9)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_DELAY_CAL_TO_MAX_RATE_FORM                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +   10)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_WEIGHTS_GET                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +   11)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_GLOBAL_SHAPERS_GET                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +   12)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_CLOS_HP_SHAPERS_GET                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +   13)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SCH_PORT_SCHED_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +   16)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_HP_CLASS_CONF_SET                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +   17)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_SCHED_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +   18)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_SET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +   19)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_GET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +   20)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_STATUS_SET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +   21)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PER1K_INFO_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +   22)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PER1K_INFO_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +   23)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_SET                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +   24)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_GET                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +   25)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SIMPLE_FLOW_ID_VERIFY_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +   26)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_ID_VERIFY_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +   27)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_ID_VERIFY_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +   28)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_K_FLOW_ID_VERIFY_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +   29)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_QUARTET_ID_VERIFY_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +   30)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SCH_FLOW_SLOW_ENABLE_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +   33)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_SHAPER_RATE_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +   34)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IF_SHAPER_RATE_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +   35)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +   36)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_GET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +   37)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_NOF_SUBFLOWS_GET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +   38)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_NOF_SUBFLOWS_SET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +   39)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_SET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +   40)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_SLOW_ENABLE_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +   41)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_STATUS_SET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +   42)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_STATUS_VERIFY                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +   43)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_SUBFLOW_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +   44)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_SUBFLOW_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +   45)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_GET_UNSAFE                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +   46)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_SET_UNSAFE                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +   47)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_VERIFY                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +   48)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_VERIFY_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +   49)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FROM_INTERNAL_CL_SUBFLOW_CONVERT                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +   50)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FROM_INTERNAL_CL_WEIGHT_CONVERT                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +   51)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FROM_INTERNAL_HR_SUBFLOW_CONVERT                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +   52)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FROM_INTERNAL_HR_WEIGHT_CONVERT                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +   53)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_GROUP_TO_PORT_ASSIGN                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +   54)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_GROUP_TO_SE_ASSIGN                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +   55)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_HR_LOWEST_HP_CLASS_SELECT_GET                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   56)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_HR_LOWEST_HP_CLASS_SELECT_SET                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   57)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_INTERNAL_HR_MODE_TO_HR_MODE_CONVERT                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +   58)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_INTERNAL_SUB_FLOW_TO_SUB_FLOW_CONVERT                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +   59)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IS_SUBFLOW_VALID                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +   60)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_NOF_QUARTETS_TO_MAP_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +   61)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PER1K_INFO_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +   62)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PER1K_INFO_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +   63)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PER1K_INFO_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +   64)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_HP_CLASS_CONF_GET_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   65)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_HP_CLASS_CONF_SET_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   66)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_HP_CLASS_CONF_VERIFY                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +   67)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SCH_SE_CONFIG_GET                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   71)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_CONFIG_SET                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   72)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_DUAL_SHAPER_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +   73)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_GET_UNSAFE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   74)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_GROUP_GET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +   75)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_GROUP_SET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +   76)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_ID_AND_TYPE_MATCH_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +   77)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_SET_UNSAFE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   78)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_STATE_GET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +   79)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_STATE_SET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +   80)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SE_VERIFY_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +   81)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SUB_FLOW_TO_INTERNAL_SUB_FLOW_CONVERT                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +   82)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SUBFLOWS_VERIFY_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +   83)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_TO_INTERNAL_CL_SUBFLOW_CONVERT                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +   84)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_TO_INTERNAL_CL_WEIGHT_CONVERT                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   85)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_TO_INTERNAL_HR_WEIGHT_CONVERT                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +   86)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_TO_INTERNAL_SUBFLOW_SHAPER_CONVERT                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +   87)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_DEVICE_IF_WEIGHT_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +   88)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_INTERN_RATE2CLOCK                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  121)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERN_CLOCK2RATE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  122)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_PORT_SET                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  127)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_PORT_GET                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  128)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_SHAPER_SET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  129)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_SHAPER_GET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  130)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_BE_WFQ_SET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  131)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_BE_WFQ_GET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  132)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_BE_SET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  133)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_BE_GET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  134)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_GU_SET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  135)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_GU_GET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  136)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  137)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_SET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  138)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_GET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  139)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_MULTICAST_GROUP_INPUT                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  140)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_MULTICAST_GROUP_ENTRY_TO_TBL                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  141)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_MULTICAST_ASSERT_INGRESS_REPLICATION_HW_TABLE                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  142)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_MULTICAST_ID_RELOCATION                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  143)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ENTRY_CONTENT_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  144)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_ENTRY_CONTENT_GET                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  145)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_ENTRY_CONTENT_GET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  146)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_PTR_TO_OLD_LIST_GET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  147)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_GROUP_UPDATE_UNSAFE_JOINT                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  149)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_LINK_LIST_PTR_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  150)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ERASE_MULTICAST_GROUP                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  151)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ERASE_ONE_ENTRY                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  152)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_GROUP_CLOSE_UNSAFE_JOINT                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  153)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_FILL_IN_LAST_ENTRY                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  154)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_FILL_IN_LAST_ENTRY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  155)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_DESTINATION_ADD_UNSAFE_INNER                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  156)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_LAST_ENTRY_IN_LIST_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  157)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ADD_ENTRY_IN_END_OF_LINK_LIST                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  158)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FILL_IN_LAST_ENTRY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  159)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  160)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  161)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_GROUP_SIZE_GET_UNSAFE_INNER                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  162)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  163)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_NIF_MAL_BASIC_CONF_SET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  164)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  165)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_TOPOLOGY_VALIDATE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  166)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_ENABLE_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  167)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_ENABLE_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  168)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MIN_PACKET_SIZE_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  169)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MIN_PACKET_SIZE_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  170)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  171)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_TOPOLOGY_VALIDATE_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  172)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_ENABLE_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  173)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_ENABLE_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  174)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_ENABLE_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  175)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MIN_PACKET_SIZE_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  176)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MIN_PACKET_SIZE_VERIFY                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  177)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MIN_PACKET_SIZE_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  178)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  179)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_VERIFY                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  180)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_XAUI_SET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  181)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_SGMII_SET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  182)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SPAUI_DEFAULTS_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  183)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SPAUI_DEFAULTS_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  184)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAT_PIPE_SET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  185)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAT_PIPE_GET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  186)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CLASS_BASED_FC_SET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  187)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CLASS_BASED_FC_GET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  188)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_INBOUND_FC_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  189)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_INBOUND_FC_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  190)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAT_PIPE_SET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  191)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAT_PIPE_VERIFY                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  192)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAT_PIPE_GET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  193)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CLASS_BASED_FC_SET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  194)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CLASS_BASED_FC_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  195)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_CLASS_BASED_FC_GET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  196)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_INBOUND_FC_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  197)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_INBOUND_FC_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  198)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_INBOUND_FC_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  199)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MDIO22_WRITE                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  200)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MDIO22_READ                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  201)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MDIO45_WRITE                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  202)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MDIO45_READ                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  203)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MDIO22_WRITE_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  204)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MDIO22_READ_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  205)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MDIO45_WRITE_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  206)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MDIO45_READ_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  207)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_TYPE_GET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  213)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_TYPE_GET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  214)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_TYPE_FROM_EGQ_GET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  215)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_INGRESS_MAP                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  216)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FC_ENABLE_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  217)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FC_ENABLE_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  218)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CELL_FORMAT_SET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  219)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CELL_FORMAT_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  220)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_COEXIST_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  221)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_COEXIST_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  222)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CONNECTIVITY_MAP_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  223)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_STANDALONE_FAP_MODE_DETECT                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  224)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CONNECT_MODE_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  225)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CONNECT_MODE_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  226)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FAP20_MAP_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  227)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FAP20_MAP_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  228)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FC_ENABLE_SET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  229)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FC_ENABLE_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  230)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FC_ENABLE_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  231)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CELL_FORMAT_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  232)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CELL_FORMAT_VERIFY                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  233)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CELL_FORMAT_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  234)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_COEXIST_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  235)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_COEXIST_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  236)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_COEXIST_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  237)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CONNECTIVITY_MAP_GET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  238)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_STANDALONE_FAP_MODE_DETECT_UNSAFE                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  239)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CONNECT_MODE_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  240)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CONNECT_MODE_VERIFY                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  241)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_CONNECT_MODE_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  242)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FAP20_MAP_SET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  243)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FAP20_MAP_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  244)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_FAP20_MAP_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  245)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MULT_EG_MULTICAST_GROUP_ENTRY_TO_TBL                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  246)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_MULTICAST_GROUP_ENTRY_TO_TBL                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  247)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_NEXT_LINK_LIST_PTR_SET                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  249)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_ING_NEXT_LINK_LIST_PTR_GET                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  250)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_RPLCT_TBL_ENTRY_OCCUPIED_BUT_EMPTY_SET                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  251)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  252)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  253)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_FABRIC_LINK_STATUS_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  254)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_LINK_STATUS_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  255)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_ITM_VSQ_CONVERT_GROUP_INDX_TO_GLOBAL_INDX                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  257)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_SET_FC_INFO                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  258)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_GET_FC_INFO                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  259)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CONVERT_ADMIT_ONE_TEST_TMPLT_TO_U32                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  260)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CONVERT_U32_TO_ADMIT_ONE_TEST_TMPLT                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  261)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_SET                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  262)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_SET                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  263)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_MAN_EXP_BUFFER_SET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  264)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_GET                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  265)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_GET                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  266)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_MAN_EXP_BUFFER_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  267)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_QT_DP_INFO_TO_WRED_TBL_DATA                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  268)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_WRED_TBL_DATA_TO_WRED_QT_DP_INFO                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  269)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_CONVERT_GLOBAL_INDX_TO_CTGRY_INDX                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  270)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_A_SET_RT_CLASS                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  271)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_B_SET_RT_CLASS                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  272)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_C_SET_RT_CLASS                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  273)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_D_SET_RT_CLASS                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  274)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_A_GET_RT_CLASS                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  275)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_B_GET_RT_CLASS                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  276)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_C_GET_RT_CLASS                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  277)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_GROUP_D_GET_RT_CLASS                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  278)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_QT_DP_INFO_TO_WRED_TBL_DATA                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  279)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_TBL_DATA_TO_WRED_QT_DP_INFO                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  280)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_WRED_GROUP_SET_INFO                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  281)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SCH_SE_DUAL_SHAPER_SET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  282)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FIELD_FROM_REG_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  283)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FIELD_FROM_REG_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  284)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_THRESH_TYPE_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  285)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_THRESH_TYPE_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  286)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_SCHED_DROP_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  287)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_SCHED_DROP_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  288)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  289)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  290)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_DEV_FC_SET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  291)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_DEV_FC_GET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  292)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_CHNIF_FC_SET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  293)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_CHNIF_FC_GET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  294)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_FC_SET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  295)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_FC_GET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  296)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_SET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  297)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_GET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  298)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_ENABLE_SET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  299)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_ENABLE_GET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  300)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCH_MODE_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  301)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCH_MODE_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  302)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCHEDULING_SET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  303)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCHEDULING_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  304)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_Q_PRIO_SET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  305)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_Q_PRIO_GET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  306)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_THRESH_TYPE_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  314)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_THRESH_TYPE_VERIFY                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  315)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_THRESH_TYPE_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  316)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_SCHED_DROP_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  317)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_SCHED_DROP_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  318)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_SCHED_DROP_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  319)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_SET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  320)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  321)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  322)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_DEV_FC_SET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  323)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_DEV_FC_VERIFY                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  324)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_DEV_FC_GET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  325)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_CHNIF_FC_SET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  326)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_CHNIF_FC_VERIFY                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  327)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_CHNIF_FC_GET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  328)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_FC_SET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  329)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_FC_VERIFY                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  330)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_FC_GET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  331)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_SET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  332)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_VERIFY                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  333)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_GET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  334)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_ENABLE_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  335)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_ENABLE_VERIFY                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  336)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MCI_FC_ENABLE_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  337)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCH_MODE_SET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  338)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCH_MODE_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  339)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCH_MODE_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  340)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCHEDULING_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  341)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCHEDULING_VERIFY                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  342)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_OFP_SCHEDULING_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  343)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_Q_PRIO_SET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  344)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_Q_PRIO_VERIFY                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  345)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_Q_PRIO_GET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  346)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_THRESH_TO_MNT_EXP                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  347)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MNT_EXP_TO_THRESH                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  348)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_THRESH_FLD_TO_MNT_EXP                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  349)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_MNT_EXP_TO_THRESH_FLD                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  350)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_NIF_PAUSE_QUANTA_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  352)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_QUANTA_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  353)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_SET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  354)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_GET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  355)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_QUANTA_SET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  356)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_QUANTA_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  357)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_QUANTA_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  358)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_SET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  359)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  360)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_GET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  361)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_SET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  362)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  363)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_SET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  364)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_GET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  365)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_SET                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  366)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_GET                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  367)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_OOB_SET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  368)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_OOB_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  369)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_VIA_NIF_SET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  371)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_VIA_NIF_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  372)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_OOB_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  373)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_OOB_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  374)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_VSQ_OFP_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  376)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_VSQ_OFP_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  377)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_GLB_OFP_HR_SET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  378)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_GLB_OFP_HR_GET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  379)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_HR_ENABLE_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  380)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_HR_ENABLE_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  381)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_SET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  384)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  385)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_GET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  386)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_SET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  387)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_VERIFY                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  388)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_GET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  389)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_SET_UNSAFE                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  390)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_VERIFY                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  391)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_GET_UNSAFE                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  392)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_OOB_SET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  393)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_OOB_VERIFY                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  394)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_INGR_GEN_OOB_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  395)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_VIA_NIF_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  397)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_VIA_NIF_VERIFY                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  398)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_VIA_NIF_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  399)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_OOB_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  400)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_OOB_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  401)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_OOB_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  402)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_VSQ_OFP_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  404)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_VSQ_OFP_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  405)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_VSQ_OFP_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  406)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_GLB_OFP_HR_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  407)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_GLB_OFP_HR_VERIFY                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  408)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_GLB_OFP_HR_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  409)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_HR_ENABLE_SET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  410)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_HR_ENABLE_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  411)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_HR_ENABLE_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  412)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_ON_GLB_RCS_OVERRIDE_SET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  413)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_ON_GLB_RCS_OVERRIDE_GET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  414)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_ON_GLB_RCS_OVERRIDE_SET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  415)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_ON_GLB_RCS_OVERRIDE_VERIFY                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  416)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_RCY_ON_GLB_RCS_OVERRIDE_GET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  417)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FC_SET                                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  418)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FC_GET                                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  419)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FC_SET_UNSAFE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  420)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FC_LL_VERIFY                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  421)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FC_GET_UNSAFE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  422)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PROG_N00_LOAD_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  423)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PROG_N00_LOAD                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  424)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_SET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  425)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_VERIFY                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  426)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_GET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  427)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_SET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  428)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PORT_GET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  429)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_PROG_PTC_CMD_SET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  430)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_SHAPING_SET_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  431)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_SHAPING_GET_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  432)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_COUNTER_GET                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  433)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_COUNTER_GET_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  434)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ALL_COUNTERS_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  435)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ALL_COUNTERS_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  436)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINK_STATUS_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  437)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LINK_STATUS_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  438)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_INTERFACE_VERIFY                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  439)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_FOR_FDR_SET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  440)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_FOR_HDR_SET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  441)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_FOR_QDR_SET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  442)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_FOR_FDR_GET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  443)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_FOR_HDR_GET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  444)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYSICAL_PARAMS_FOR_QDR_GET                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  445)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LINK_RX_EYE_MONITOR_MODE_SET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  449)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LINK_RX_EYE_MONITOR_MODE_GET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  450)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LINK_RX_EYE_MONITOR_RUN                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  451)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LINK_RX_EYE_MONITOR_HEIGHT_GET                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  452)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_MEM_LOAD                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  453)|SOC_PETRA_PROC_BITS)

/*
 *  soc_petra_mgmt
 */
#define SOC_PETRA_MGMT_OPERATION_MODE_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  454)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_OPERATION_MODE_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  455)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_REGISTER_DEVICE_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  456)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_OPERATION_MODE_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  457)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_OPERATION_MODE_VERIFY                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  458)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_OPERATION_MODE_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  459)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_OPERATION_MODE_DEVICE_TYPE_SET                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  460)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_OPERATION_MODE_DEVICE_TYPE_GET                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  461)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_REVISION_INIT                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  462)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_MODULE_INIT                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  463)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_IHP_STAG_HDR_DATA_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  464)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_STAG_HDR_DATA_VERIFY                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  465)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_STAG_HDR_DATA_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  466)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_TMLAG_HUSH_FIELD_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  467)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_TMLAG_HUSH_FIELD_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  468)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_TMLAG_HUSH_FIELD_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  469)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_STAG_HDR_DATA_SET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  470)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_STAG_HDR_DATA_GET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  471)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_TMLAG_HUSH_FIELD_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  472)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_TMLAG_HUSH_FIELD_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  473)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DEVICE_CLOSE                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  474)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_UNREGISTER_DEVICE_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  475)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  476)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_GET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  477)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_LOCAL_TO_SYS_PHYS_PORT_MAP_GET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  478)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_TO_INTERFACE_MAP_SET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  479)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_TO_INTERFACE_MAP_GET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  480)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  481)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_GET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  482)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_HEADER_TYPE_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  483)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_HEADER_TYPE_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  484)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_INBOUND_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  485)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_INBOUND_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  486)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_OUTBOUND_SET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  487)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_OUTBOUND_GET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  488)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SNOOP_SET                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  489)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SNOOP_GET                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  490)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  491)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_VERIFY                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  492)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  493)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  494)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_TO_INTERFACE_MAP_SET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  495)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_TO_INTERFACE_MAP_VERIFY                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  496)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_TO_INTERFACE_MAP_GET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  497)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  498)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_VERIFY                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  499)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_GET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  500)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_HEADER_TYPE_SET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  501)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_HEADER_TYPE_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  502)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_HEADER_TYPE_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  503)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_INBOUND_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  504)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_INBOUND_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  505)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_INBOUND_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  506)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_OUTBOUND_SET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  507)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_OUTBOUND_VERIFY                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  508)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_MIRROR_OUTBOUND_GET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  509)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SNOOP_SET_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  510)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SNOOP_VERIFY                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  511)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SNOOP_GET_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  512)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PPCT_TBL_GET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  514)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_PPCT_TBL_SET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  516)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT2IF_INGR_MAP_NEW_UNMAP_OLD                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  517)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT2IF_EGR_MAP_NEW_UNMAP_OLD                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  518)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_EGR_MAP_WRITE_VAL                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  519)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LOGICAL_SYS_ID_BUILD                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  520)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LOGICAL_SYS_ID_PARSE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  521)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_ITMH_EXTENSION_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  522)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_ITMH_EXTENSION_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  523)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SHAPING_HEADER_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  524)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SHAPING_HEADER_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  525)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FORWARDING_HEADER_SET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  526)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FORWARDING_HEADER_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  527)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_STAG_FIELDS_SET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  528)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_STAG_FIELDS_GET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  529)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FTMH_EXTENSION_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  530)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FTMH_EXTENSION_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  531)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_OTMH_EXTENSION_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  532)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_OTMH_EXTENSION_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  533)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_STAG_FIELDS_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  534)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_STAG_FIELDS_VERIFY                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  535)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_STAG_FIELDS_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  536)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FTMH_EXTENSION_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  537)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FTMH_EXTENSION_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  538)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FTMH_EXTENSION_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  539)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_OTMH_EXTENSION_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  540)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_OTMH_EXTENSION_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  541)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_OTMH_EXTENSION_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  542)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FORWARDING_HEADER_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  543)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FORWARDING_HEADER_GET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  544)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_ITMH_EXTENSION_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  545)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_ITMH_EXTENSION_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  546)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_ITMH_EXTENSION_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  547)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SHAPING_HEADER_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  548)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SHAPING_HEADER_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  549)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_SHAPING_HEADER_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  550)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FORWARDING_HEADER_SET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  551)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_ITMH_BUILD_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  560)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_ITMH_BUILD                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  561)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_ITMH_PARSE                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  562)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_FTMH_BUILD_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  563)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_FTMH_BUILD                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  564)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_FTMH_PARSE                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  565)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_OTMH_BUILD_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  566)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_OTMH_BUILD                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  567)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_HPU_OTMH_PARSE                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  568)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_DRAM_BUFFS_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  569)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_DRAM_BUFFS_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  570)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_WAIT_FOR_INIT                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  571)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_DRAM_BUFFS_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  573)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_DRAM_BUFFS_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  574)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_DRAM_BUFFS_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  575)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_BLOCKS_INIT_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  576)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_QUEUING_REGS_INIT                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  577)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_QUEUING_INIT                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  578)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_REGS_INIT                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  579)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_INIT                                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  580)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FLOW_CONTROL_REGS_INIT                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  581)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FLOW_CONTROL_INIT                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  582)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_HEADER_PARSING_REGS_INIT                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  583)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_HEADER_PARSING_INIT                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  584)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_REGS_INIT                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  585)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INGRESS_SCHEDULER_INIT                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  586)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_REGS_INIT                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  587)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_INIT                                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  588)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_REGS_INIT                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  589)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_INIT                                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  590)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCHEDULER_END2END_REGS_INIT                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  591)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCHEDULER_END2END_INIT                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  592)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SERDES_REGS_INIT                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  593)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SERDES_INIT                                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  594)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_TBLS_INIT                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  595)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_FUNCTIONAL_INIT                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  596)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_SET_DEFAULTS                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  597)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OLP_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  598)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRE_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  599)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IDR_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  600)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  601)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  602)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  603)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  604)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  605)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DPI_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  606)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_RTP_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  611)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  612)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  613)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SET_REPS_FOR_TBL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  614)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_GEN_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  615)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OLP_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  616)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRE_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  617)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IDR_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  618)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IRR_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  619)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  620)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IQM_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  621)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPS_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  622)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPT_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  623)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DPI_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  624)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_RTP_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  629)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  630)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CFC_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  631)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  632)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_SET_REPS_FOR_TBL                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  633)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EPNI_SET_REPS_FOR_TBL_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  634)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_OLP_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  635)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_IRE_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  636)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_IDR_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  637)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_IRR_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  638)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_IHP_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  639)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_IQM_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  640)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_IPS_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  641)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_DPI_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  642)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_RTP_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  643)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_EGQ_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  644)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CFC_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  645)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_SCH_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  646)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_IPT_TBLS_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  647)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SYS_PORT_ADD_VERIFY                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  648)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SYS_PORT_ADD_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  649)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SYS_PORT_ADD                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  650)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SYS_PORT_REMOVE_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  651)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SYS_PORT_REMOVE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  652)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_HEADER_TYPE_SET_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  653)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_HEADER_TYPE_GET_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  654)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SYS_PORT_INFO_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  655)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_SYS_PORT_INFO_GET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  656)|SOC_PETRA_PROC_BITS)
#define SOC_PA_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  657)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORTS_ITMH_EXTENSION_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  658)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORTS_ITMH_EXTENSION_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  659)|SOC_PETRA_PROC_BITS)
#define SOC_PA_MAL_EQUIVALENT_ID_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  660)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_MGMT_HW_ADJUST_DDR                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  666)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_ADJUST_DDR_INIT                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  667)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_ADJUST_QDR                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  668)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_ADJUST_SERDES                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  669)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_ADJUST_CPU                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  670)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_HW_ADJUST_FABRIC                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  671)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ALL_CTRL_CELLS_FCT_DISABLE_POLLING                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  672)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_WRITE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  673)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_ALL_CTRL_CELLS_FCT_ENABLE_POLLING                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  674)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE1_VERIFY                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  675)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE2_VERIFY                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  676)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_FAP_AND_NIF_TYPE_MATCH_VERIFY                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  677)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_SET_ALL                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  678)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_GET_ALL                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  679)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_SET_ALL                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  680)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_GET_ALL                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  681)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_VERIFY_ALL                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  682)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_SET_ALL_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  683)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_MAL_BASIC_CONF_GET_ALL_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  684)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_VERIFY_ALL                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  685)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_SET_ALL_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  686)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_GET_ALL_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  687)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_SET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  688)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_GET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  689)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_UPDATE                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  690)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_SINGLE_PORT_GET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  691)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_MAL_SHAPER_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  692)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_MAL_SHAPER_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  693)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_SET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  694)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_VERIFY                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  695)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_GET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  696)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_UPDATE_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  697)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_SINGLE_PORT_GET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  698)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_MAL_SHAPER_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  699)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_MAL_SHAPER_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  700)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_MAL_SHAPER_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  701)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MAL_EQUIVALENT_ID_VERIFY                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  702)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_UPDATE_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  703)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_CAL_PER_LEN_BUILD                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  704)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_CAL_LEN_CALCULATE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  707)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_FIXED_LEN_CAL_BUILD                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  708)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_DEVICE_INTERFACE_RATE_SET                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  710)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_IS_CHANNELIZED_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  711)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_IS_CHANNELIZED                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  712)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_FROM_SCH_PORT_RATES_TO_CALENDAR                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  713)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_FILL_SHAPER_CALENDAR_CREDITS                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  714)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_FROM_EGQ_PORT_RATES_TO_CALENDAR                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  715)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_FROM_CALENDAR_TO_PORTS_SCH_RATE                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  716)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_FROM_CALENDAR_TO_PORTS_EGQ_RATE                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  718)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIF_SCM_TBL_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  720)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGQ_NIF_SCM_TBL_SET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  722)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_ACTIVE_CALENDARS_CONFIG                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  723)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_RATE_BELOW_3_2_PARAMS_TO_DEVICE_CALC                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  724)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_RATE_ABOVE_3_2_PARAMS_TO_DEVICE_CALC                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  725)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_RATE_BELOW_3_2_PARAMS_FROM_DEVICE_CALC                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  726)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_TX_RATE_ABOVE_3_2_PARAMS_FROM_DEVICE_CALC                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  727)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_INIT                                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  728)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_WAIT_FOR_INIT                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  729)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_PORT_HP_CLASS_CONF_GET                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  730)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_UNBIND                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  731)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_UNBIND_VERIFY                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  732)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_IS_RESET_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  733)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FLOW_STATUS_PRINT                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  734)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AGG_STATUS_PRINT                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  735)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FLOW_AND_UP_PRINT_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  736)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IS_HR_SUBFLOW_VALID                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  737)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_IS_CL_SUBFLOW_VALID                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  738)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_HR_TO_PORT_ASSIGN_SET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  739)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_HR_TO_PORT_ASSIGN_GET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  740)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_EGQ_SHAPER_RATE_TO_INTERNAL                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  741)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_SW_DB_WRITE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  742)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_EGQ_SHAPER_CONFIG                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  743)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_EGQ_SHAPER_RATE_FROM_INTERNAL                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  744)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_EXACT_TBL_INIT                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  745)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_TABLE_CONSTRUCT                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  746)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_ACTIVE_CALENDARS_RETRIEVE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  747)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_EGQ_SHAPER_RETRIEVE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  748)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_AQFM_MAL_DFLT_SHAPER_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  749)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_OFP_MAL_GET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  750)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_ACTIVE_MAL_BUILD                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  751)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_EGR_PORTS_ACTIVE_MALS_GET                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  752)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_FLD_FROM_REG_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  753)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_FLD_FROM_REG_SET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  754)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_EPB_STATUS_PARSE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  755)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_EPB_CMD_BUILD                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  756)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_ACCESS_VERIFY                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  757)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REG_READ_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  758)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_FLD_WRITE_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  759)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_FLD_READ_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  760)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REG_WRITE                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  761)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REG_READ                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  762)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_FLD_WRITE                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  763)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_FLD_READ                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  764)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_PHYS_PARAMS_CONFIG                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  765)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_REGS_INIT_REGS                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  767)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_CONFIG                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  768)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_INIT_ALL_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  769)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_BEFORE_BLOCKS_OOR                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  770)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_AFTER_BLOCKS_OOR                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  771)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INIT_DRAM_FBC_BUFFS_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  772)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INIT_DRAM_MAX_WITHOUT_FBC_GET                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  773)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_REGS_INIT                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  774)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_INIT                                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  775)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_ENABLE_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  776)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IPU_ENABLE_ALL_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  777)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_IDDR_SET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  779)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_IDDR_GET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  780)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_REGS_DUMP                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  781)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_TBLS_DUMP                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  782)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_PACKET_WALKTROUGH_GET                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  783)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DEV_TBLS_DUMP                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  784)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_ATTACHED_FLOW_PORT_GET                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  786)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_TBLS_DUMP_TABLES_GET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  787)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_IDDR_SET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  788)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_IDDR_GET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  789)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_READ_REG_BUFFER_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  790)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_ATTACHED_FLOW_PORT_GET_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  791)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_QDR_BIST_TEST_START                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  792)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_QDR_BIST_TEST_START_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  793)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_QDR_BIST_TEST_RESULT_GET                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  794)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_QDR_BIST_TEST_RESULT_GET_UNSAFE                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  795)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_BIST_TEST_START                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  796)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_BIST_TEST_START_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  797)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_BIST_TEST_RESULT_GET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  798)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_BIST_TEST_RESULT_GET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  799)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_QDR_BIST_TEST_START_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  800)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_QDR_BIST_TEST_COUNTERS_GET_UNSAFE                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  801)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_STATUS_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  802)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_STATUS_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  803)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_BIST_TEST_COUNTERS_GET_UNSAFE                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  804)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_DBUFF_SIZE2INTERNAL                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  805)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_DBUFF_INTERNAL2SIZE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  806)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_FAT_PIPE_ENABLE_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  807)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_FAT_PIPE_INITIALIZE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  808)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_OP_MODE_RELATED_INIT                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  809)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_OFP_MAL_GET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  810)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_IS_FAT_PIPE_PORT                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  811)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_IS_FAT_PIPE_MAL                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  812)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_TBLS_DUMP_ALL                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  813)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_FAT_PIPE_RATE_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  814)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_FAT_PIPE_RATE_VERIFY                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  815)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_FAT_PIPE_RATE_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  816)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_FAT_PIPE_RATE_SET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  817)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_FAT_PIPE_RATE_GET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  818)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MC_REGS_INIT                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  819)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM_TBL_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  820)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IHP_KEY_PROGRAM_TBL_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  821)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_SET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  822)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_GET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  823)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_TX_PHYS_PARAMS_SET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  824)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_TX_PHYS_PARAMS_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  825)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POWER_STATE_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  826)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POWER_STATE_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  827)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_STAR_SET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  828)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_STAR_GET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  829)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_ALL_SET                                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  830)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_ALL_GET                                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  831)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_SET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  832)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_VERIFY                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  833)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_GET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  834)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_TX_PHYS_PARAMS_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  835)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_TX_PHYS_PARAMS_VERIFY                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  836)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_TX_PHYS_PARAMS_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  837)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POWER_STATE_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  838)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POWER_STATE_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  839)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_LANE_POWER_STATE_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  840)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_STAR_SET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  841)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_STAR_VERIFY                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  842)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_STAR_GET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  843)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_ALL_SET_UNSAFE                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  844)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_ALL_VERIFY                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  845)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_ALL_GET_UNSAFE                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  846)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_INTERNAL_RATE_CALC                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  847)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_DEVICE_SRD_INIT                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  848)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_INTERNAL_RATE_SET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  849)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_INTERNAL_RATE_GET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  850)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RATE_GET_AND_VALIDATE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  851)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_TX_PHYS_PARAMS_CALCULATE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  852)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_TX_PHYS_EXPLICIT_TO_INTERN                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  853)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_CMU_TRIM                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  854)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_SW_DB_READ                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  855)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_MAC_RATE_SET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  856)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_AUTO_EQUALIZE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  859)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_AUTO_EQUALIZE_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  860)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RX_PHYS_PARAMS_SET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  861)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RX_PHYS_PARAMS_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  862)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RX_PHYS_PARAMS_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  863)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RX_PHYS_PARAMS_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  864)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RX_PHYS_PARAMS_VERIFY                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  865)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SW_DB_DEVICE_LBG_INIT                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  866)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_MODE_VERIFY                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  867)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_MODE_GET_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  868)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_START_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  869)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_STOP_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  870)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_GET_AND_CLEAR_STAT_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  871)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_MODE_SET                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  872)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_MODE_GET                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  873)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_START                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  874)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_STOP                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  875)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_GET_AND_CLEAR_STAT                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  876)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_CLEAR_STATUSES                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  877)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LPM_PORT_STATE_SAVE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  878)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LPM_PORT_STATE_LOAD_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  879)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LBG_CONF_SET                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  880)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LBG_CONF_SET_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  881)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LBG_TRAFFIC_SEND                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  882)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LBG_TRAFFIC_SEND_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  883)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LBG_RESULT_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  884)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LBG_RESULT_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  885)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LBG_CLOSE                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  886)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_LBG_CLOSE_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  887)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ALL_NIFS_ALL_COUNTERS_GET                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  888)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ALL_NIFS_ALL_COUNTERS_GET_UNSAFE                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  889)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INIT_DRAM_NOF_BUFFS_CALC                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  890)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INIT_DRAM_BUFF_BOUDARIES_CALC                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  891)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_ID_VERIFY                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  892)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_RATE_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  893)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_RATE_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  894)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_RATE_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  895)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_RATE_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  896)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_RATE_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  897)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_STATUS_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  898)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_STATUS_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  899)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_ALL_OFP_RATES_GET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  900)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_ALL_OFP_RATES_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  901)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_SPECIAL_ID_DETECT                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  902)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_MODULE_INITIALIZE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  903)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_INFO_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  904)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_MASK_ALL_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  905)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_UNMASK_ALL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  906)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_MASK_CLEAR_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  907)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_IS_ALL_MASKED_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  908)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MONITOR_START_UNSAFE                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  909)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MONITOR_STOP_UNSAFE                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  910)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_SET_UNSAFE                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  911)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_GET_UNSAFE                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  912)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_MASK_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  913)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_LEVEL_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  914)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_INITIAL_MASK_LIFT                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  915)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SINGLE_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  916)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SINGLE_INFO_READ                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  917)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_INFO_READ                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  918)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_GET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  919)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MONITOR_START                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  920)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MONITOR_STOP                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  921)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_SET                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  922)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_GET                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  923)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_MASK_ALL                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  924)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_UNMASK_ALL                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  925)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_MASK_CLEAR                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  926)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_HANDLER_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  927)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CALLBACK_ALL_FUNCTIONS_UNREGISTER_UNSAFE                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  928)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CALLBACK_FUNCTION_REGISTER_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  929)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CALLBACK_USER_CALLBACK_FUNCTION                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  930)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_CLEAR_ALL                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  931)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_PRIO_SET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  932)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_PRIO_GET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  933)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_PRIO_SET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  934)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_PRIO_VERIFY                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  935)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_EGR_UNSCHED_DROP_PRIO_GET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  936)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_BASE_Q_DFLT_INVALID_SET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  937)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_BASE_Q_IS_VALID_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  938)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_INIT                                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  939)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_FORBIDDEN_VER_SIZE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  940)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_FORBIDDEN_VER_TRANS                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  941)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_MAX_SW_DB_SIZE_GET                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  942)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_SW_DB_SIZE_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  943)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_CFG_VERSION_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  944)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_TRANSFORM_DB2CURR                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  945)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_DATA_LOAD                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  946)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_DATA_SAVE                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  947)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_BUFFER_SAVE                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  948)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_BUFFER_LOAD                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  949)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_BUFF_SIZE_GET                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE +  950)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOOPBACK_SET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  951)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOOPBACK_GET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  952)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOOPBACK_SET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  953)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOOPBACK_VERIFY                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  954)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOOPBACK_GET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  955)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_PCKT_SIZE_RANGE_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  956)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_PCKT_SIZE_RANGE_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  957)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_PCKT_SIZE_RANGE_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  958)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_PCKT_SIZE_RANGE_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  959)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_PCKT_SIZE_RANGE_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  960)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TEXT_ERR_GET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  961)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_STAR_RESET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  962)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TOPOLOGY_STATUS_CONNECTIVITY_GET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  963)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TOPOLOGY_STATUS_CONNECTIVITY_GET_UNSAFE                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  964)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TOPOLOGY_STATUS_CONNECTIVITY_PRINT                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  965)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_LINKS_STATUS_GET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  966)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_TRANSACTION_WITH_FE600                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  967)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_READ_FROM_FE600_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  968)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_WRITE_FROM_FE600_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE +  969)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INDIRECT_READ_FROM_FE600_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  970)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INDIRECT_WRITE_FROM_FE600_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  971)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SR_SEND_CELL                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE +  972)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SR_RCV_CELL                                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  973)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SR_SEND_AND_WAIT_ACK                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  974)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_READ_FROM_FE600                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE +  975)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_WRITE_FROM_FE600                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  976)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INDIRECT_READ_FROM_FE600                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE +  977)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INDIRECT_WRITE_FROM_FE600                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  978)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_WRITE_INDIRECT_OFFSET_TO_RTP_TABLE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  979)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_IVCDL_SET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  980)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_ROUTE_FORCE_SET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  981)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_ROUTE_FORCE_SET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  982)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_ROUTE_FORCE_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE +  983)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_ROUTE_FORCE_VERIFY                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE +  984)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_ROUTE_FORCE_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE +  985)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_AUTOCREDIT_SET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  986)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_AUTOCREDIT_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  987)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_AUTOCREDIT_GET                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  988)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_AUTOCREDIT_VERIFY                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE +  989)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_AUTOCREDIT_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE +  990)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_SET                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  991)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_GET                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE +  992)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_VERIFY                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE +  993)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_GET_UNSAFE                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE +  994)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_SET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  995)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_SET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  996)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE +  997)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE +  998)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_GET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE +  999)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_PORT_MAL_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1000)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_RATE2AUTOCREDIT_RATE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1001)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_AUTOCREDIT_RATE2RATE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1002)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_SET_UNSAFE                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1003)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_VERIFY                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1004)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_GET_UNSAFE                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1005)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_TYPE_SET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1006)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_VERIFY                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1007)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_TYPE_GET_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1008)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_SET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1009)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_GET                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1010)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_TYPE_SET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1011)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORT_EGR_HDR_DISCOUNT_TYPE_GET                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1012)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_OOB_STAT_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1013)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_OOB_STAT_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1014)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU2CPU_WITH_FE600_WRITE_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1015)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU2CPU_WITH_FE600_WRITE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1016)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_BUILD_DATA_CELL_FOR_FE600                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1017)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU2CPU_WITH_FE600_READ_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1018)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CPU2CPU_WITH_FE600_READ                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1019)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_INVERT_POLARITY_FROM_ORIG                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1020)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INIT_MEM_CORRECTION_ENABLE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1021)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOCAL_FAULT_OVRD_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1022)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOCAL_FAULT_OVRD_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1023)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOCAL_FAULT_OVRD_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1024)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOCAL_FAULT_OVRD_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1025)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_LOCAL_FAULT_OVRD_VERIFY                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1026)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_EYE_SCAN_CALC_OPTIMUM                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1027)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_ENABLE_STATE_SET                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1028)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_SGMII_ENABLE_STATE_GET                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1029)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_EGQ_CALENDAR_VALIDATE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1030)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_EGQ_CALENDAR_VALIDATE_UNSAFE                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1031)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_TEST_SETTINGS                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1032)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_TEST_SETTINGS_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1033)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_TEST_RANDOM                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1034)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_TEST_RANDOM_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1035)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STATUS_FLD_POLL                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1036)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STATUS_FLD_POLL_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1037)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_INFO_SET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1038)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_INFO_SET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1039)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_INFO_GET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1040)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_INFO_VERIFY                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1041)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_INFO_GET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1042)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FC_EGR_REC_OOB_TABLES_SET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1043)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_LINK_ON_OFF_SET                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1044)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_LINK_ON_OFF_GET                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1045)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_LINK_ON_OFF_SET_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1046)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_LINK_ON_OFF_VERIFY                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1047)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_LINK_ON_OFF_GET_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1048)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_INIT                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1049)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_PRBS_MODE_SET_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1050)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_BIST_BASIC_WRITE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1051)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_BIST_PRESETTINGS_SET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1052)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_BIST_BASIC_READ_AND_COMPARE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1053)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_BIST_WRITE                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1054)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_BIST_WRITE_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1055)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_BIST_READ_AND_COMPARE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1056)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_BIST_READ_AND_COMPARE_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1057)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_INFO_SET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1058)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_INFO_SET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1059)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_INFO_GET                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1060)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_INFO_VERIFY                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1061)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_INFO_GET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1062)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_REPORT_INFO_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1063)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_REPORT_INFO_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1064)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_REPORT_INFO_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1065)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_REPORT_INFO_VERIFY                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1066)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IF_REPORT_INFO_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1067)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_INIT                                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1068)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_REGS_INIT                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1069)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_OFFSET2BIST_ADDR_CONVERT                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1070)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_SET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1071)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_VERIFY                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1072)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_GET_UNSAFE                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1073)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_SET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1074)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_GET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1075)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_SCIF_ENABLE_SET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1076)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_SCIF_ENABLE_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1077)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_SCIF_ENABLE_SET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1078)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_SCIF_ENABLE_VERIFY                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1079)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_SCIF_ENABLE_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1080)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_RESET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1081)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_RESET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1082)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_VALIDATE_AND_RELOCK                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1083)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_IDX_VERIFY                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1084)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_COUNTER_SET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1085)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_COUNTER_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1086)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_COUNTER_READ                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1087)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_COUNTER_VERIFY                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1088)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_COUNTER_SET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1089)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_COUNTER_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1090)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_VSQ_COUNTER_READ_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1091)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_CALLBACK_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1092)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_CALLBACK_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1094)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_CALLBACK_VERIFY                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1095)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_CALLBACK_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1096)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_INIT                                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1097)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_ASYNC_INTERFACE_PACKET_SEND                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1098)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_ASYNC_INTERFACE_PACKET_RECV                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1099)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_CALLBACK_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1100)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DLL_STATUS_GET                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1101)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DLL_STATUS_GET_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1102)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_STATUS_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1103)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_STATUS_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1104)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_STATUS_FLD_POLL_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1105)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_SRD_QRTT_RESET                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1106)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ON_OFF_SET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1107)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ON_OFF_GET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1108)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_FABRIC_SRD_QRTT_RESET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1109)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ON_OFF_SET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1110)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ON_OFF_VERIFY                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1111)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_NIF_ON_OFF_GET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1112)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_TX_PHYS_EXPLICITE_TO_INTERN                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1113)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CTRL_CELLS_COUNTER_CLEAR                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1114)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_MEMBER_ADD                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1115)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_LAG_MEMBER_ADD_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1116)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_QUEUE_FLUSH                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1117)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_QUEUE_FLUSH_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1118)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_QUEUE_FLUSH_ALL                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1119)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_QUEUE_FLUSH_ALL_UNSAFE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1120)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_RECEIVE_MODE_SET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1121)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_RECEIVE_MODE_SET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1122)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_RECEIVE_MODE_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1123)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_RECEIVE_MODE_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1124)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PKT_PACKET_RECEIVE_MODE_GET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1125)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_UPDATE_DEVICE_SET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1126)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_UPDATE_DEVICE_SET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1127)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_UPDATE_DEVICE_GET                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1128)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_UPDATE_DEVICE_GET_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1129)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_QUEUE_FLUSH_INTERNAL_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1130)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_INGR_RESET                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1131)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_INGR_RESET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1132)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RELOCK                                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1133)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_RELOCK_UNSAFE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1134)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ENHANCED_SET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1135)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ENHANCED_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1136)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ENHANCED_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1137)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ENHANCED_SET_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1138)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_FABRIC_ENHANCED_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1139)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_DIAGNOSTIC_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1140)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_DIAGNOSTIC_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1141)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_DRAM_DIAGNOSTIC_GET_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1142)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_RPLCT_TBL_ENTRY_UNOCCUPIED_SET                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1143)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_RPLCT_TBL_ENTRY_UNOCCUPIED_SET_ALL                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1144)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_RPLCT_TBL_ENTRY_IS_OCCUPIED                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1145)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_RPLCT_TBL_ENTRY_IS_EMPTY_BUT_OCCUPIED                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1146)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_MAL_RATE_SET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1147)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_MAL_RATE_GET_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1148)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_IS_DEVICE_INIT_DONE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1149)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SSR_IS_DEVICE_INIT_DONE_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1150)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_ECI_ACCESS_TST                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1151)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_ECI_ACCESS_TST_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1152)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_QRTT_UNMAP                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1153)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IPQ_QUEUE_QRTT_UNMAP_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1154)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_WRITE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1155)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_WRITE_PRINT                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1156)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_WRITE_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1157)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_WRITE_VERIFY                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1158)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_READ                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1159)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_READ_PRINT                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1160)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_READ_UNSAFE                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1161)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_READ_VERIFY                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1162)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_MC_TBL_INBAND_BUILD                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1164)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CELL_ACK_GET                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1165)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IFP_CNT_SELECT_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1166)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IFP_CNT_SELECT_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1167)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IFP_CNT_SELECT_GET_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1168)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IFP_CNT_SELECT_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1169)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IFP_CNT_SELECT_SET_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1170)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IFP_CNT_SELECT_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1171)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VOQ_CNT_SELECT_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1172)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VOQ_CNT_SELECT_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1173)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VOQ_CNT_SELECT_GET_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1174)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VOQ_CNT_SELECT_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1175)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VOQ_CNT_SELECT_SET_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1176)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VOQ_CNT_SELECT_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1177)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VSQ_CNT_SELECT_SET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1178)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VSQ_CNT_SELECT_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1179)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VSQ_CNT_SELECT_GET_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1180)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VSQ_CNT_SELECT_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1181)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VSQ_CNT_SELECT_SET_VERIFY                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1182)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_VSQ_CNT_SELECT_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1183)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CORE_FREQUENCY_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1184)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CORE_FREQUENCY_GET_PRINT                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1185)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_CORE_FREQUENCY_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1186)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_WINDOW_VALIDITY_GET                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1187)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_WINDOW_VALIDITY_GET_PRINT                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1188)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_WINDOW_VALIDITY_GET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1189)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_WINDOW_VALIDITY_GET_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1190)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_EPB_ACCESS_VALIDATE_DONE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1191)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PA_EGR_UNSCHED_DROP_SET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1192)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PA_EGR_UNSCHED_DROP_GET_UNSAFE                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1193)|SOC_PETRA_PROC_BITS)
#define SOC_PA_MGMT_PCKT_SIZE_RANGE_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1194)|SOC_PETRA_PROC_BITS)
#define SOC_PA_MGMT_PCKT_SIZE_RANGE_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1195)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1196)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_STAT_IS_RELEVANT_COUNTER                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1197)|SOC_PETRA_PROC_BITS)
#define SOC_PA_DIAG_TBLS_DUMP                                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1198)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_TO_INTERFACE_MAP_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1199)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_TO_INTERFACE_MAP_VERIFY                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1200)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_TO_INTERFACE_MAP_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1201)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORTS_FTMH_EXTENSION_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1202)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORTS_FTMH_EXTENSION_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1203)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORTS_OTMH_EXTENSION_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1204)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORTS_OTMH_EXTENSION_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1205)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_EGR_HDR_DISCOUNT_SELECT_GET_UNSAFE                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1206)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORT_EGR_HDR_DISCOUNT_SELECT_SET_UNSAFE                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1207)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_ALL_OFP_RATES_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1208)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_RATES_SINGLE_PORT_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1209)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_OFP_RATES_SINGLE_PORT_SET_UNSAFE                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1210)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_RATES_SINGLE_PORT_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1211)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_RATES_MAL_SHAPER_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1212)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_FAT_PIPE_RATE_SET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1213)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_FAT_PIPE_RATE_GET_UNSAFE                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1214)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_RATES_EGQ_CALENDAR_VALIDATE_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1215)|SOC_PETRA_PROC_BITS)
#define SOC_PA_INTERFACE_ID_VERIFY                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1216)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_RATES_UPDATE_DEVICE_SET_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1217)|SOC_PETRA_PROC_BITS)
#define SOC_PA_OFP_RATES_INIT                                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1218)|SOC_PETRA_PROC_BITS)
#define SOC_PA_DIAG_LBG_CONF_SET_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1219)|SOC_PETRA_PROC_BITS)
#define SOC_PA_DIAG_LBG_CLOSE_UNSAFE                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1220)|SOC_PETRA_PROC_BITS)
#define SOC_PA_INTERRUPT_HANDLER_UNSAFE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1221)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_SRD_QRTT_STATUS_PRINT                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1250)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MC_INIT                                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1251)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_SET_UNSAFE                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1252)|SOC_PETRA_PROC_BITS)

#define SOC_PETRA_INTERRUPT_INIT                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1253)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_ALL_INTERRUPTS_AND_INDICATIONS_CLEAR_UNSAFE                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1254)|SOC_PETRA_PROC_BITS)
#define SOC_PA_INTERRUPT_ALL_INTERRUPTS_AND_INDICATIONS_CLEAR_UNSAFE                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1255)|SOC_PETRA_PROC_BITS)
#define SOC_PA_INTERRUPT_MASK_ALL_UNSAFE                                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1256)|SOC_PETRA_PROC_BITS)
#define SOC_PA_INTERRUPT_UNMASK_ALL_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1257)|SOC_PETRA_PROC_BITS)
#define SOC_PA_INTERRUPT_MASK_CLEAR_UNSAFE                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1258)|SOC_PETRA_PROC_BITS)
#define SOC_PA_INTERRUPT_ALL_INTERRUPTS_PRINT_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1259)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_ALL_INTERRUPTS_PRINT_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1260)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_QRTT_SYNC_FIFO_EN                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1261)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SRD_INIT                                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1262)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_GROUP_ENTRIES_SET                                             ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1263)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_TBL_ENTRY_FORMAT_GET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1264)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MULT_EG_PROGRESS_INDEX_GET                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1265)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_IF_TYPE_FROM_ID                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1266)|SOC_PETRA_PROC_BITS)
#define SOC_PA_IF_TYPE_FROM_ID                                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1267)|SOC_PETRA_PROC_BITS)
#define SOC_PA_SCH_DEVICE_IF_WEIGHT_SET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1268)|SOC_PETRA_PROC_BITS)
#define SOC_PA_SCH_DEVICE_IF_WEIGHT_GET_UNSAFE                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1269)|SOC_PETRA_PROC_BITS)
#define SOC_PA_EGR_CHNIF_FC_GET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1270)|SOC_PETRA_PROC_BITS)
#define SOC_PA_EGR_CHNIF_FC_SET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1271)|SOC_PETRA_PROC_BITS)
#define SOC_PA_EGR_CHNIF_FC_VERIFY                                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1272)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_QDR_WINDOW_VALIDITY_GET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1273)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORTS_MIRROR_OUTBOUND_SET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1274)|SOC_PETRA_PROC_BITS)
#define SOC_PA_PORTS_MIRROR_OUTBOUND_GET_UNSAFE                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1275)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_PCM_READINGS_GET                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1276)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_PCM_READINGS_GET_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1277)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_RESET                                                         ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1280)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_RESET_UNSAFE                                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1281)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_SUSPECT_SPR_DETECT                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1282)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_SUSPECT_SPR_DETECT_PRINT                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1283)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_SUSPECT_SPR_DETECT_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1284)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_SUSPECT_SPR_DETECT_VERIFY                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1285)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_SUSPECT_SPR_CONFIRM                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1286)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_SUSPECT_SPR_CONFIRM_UNSAFE                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1287)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_SCH_SUSPECT_SPR_CONFIRM_VERIFY                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1288)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_SET_PRINT																		((SOC_PETRA_INTERNAL_FUNCS_BASE + 1289)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_SET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1290)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_SET_VERIFY                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1291)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_GET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1292)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_GET_PRINT                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1293)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_GET_UNSAFE                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1295)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_SET                                          ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1296)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DEBUG_GET_PROCS_PTR                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1297)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DEBUG_GET_ERRS_PTR                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1298)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_SOFT_ERROR_TEST_START                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1299)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_SOFT_ERROR_TEST_START_PRINT                                      ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1300)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_SOFT_ERROR_TEST_START_UNSAFE                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1301)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_SOFT_ERROR_TEST_START_VERIFY                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1302)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_SOFT_ERROR_TEST_RESULT_GET                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1303)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_SOFT_ERROR_TEST_RESULT_GET_PRINT                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1304)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_SOFT_ERROR_TEST_RESULT_GET_UNSAFE                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1305)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_SOFT_ERROR_TEST_RESULT_GET_VERIFY																((SOC_PETRA_INTERNAL_FUNCS_BASE + 1306)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_DEV_RESET																													((SOC_PETRA_INTERNAL_FUNCS_BASE + 1307)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_DEV_RESET_PRINT																										((SOC_PETRA_INTERNAL_FUNCS_BASE + 1308)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_DEV_RESET_UNSAFE																									((SOC_PETRA_INTERNAL_FUNCS_BASE + 1309)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_DPRC_RESET_UNSAFE																									((SOC_PETRA_INTERNAL_FUNCS_BASE + 1310)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_DEV_RESET_VERIFY																									((SOC_PETRA_INTERNAL_FUNCS_BASE + 1311)|SOC_PETRA_PROC_BITS)
#define SOC_PA_CFC_REGS_MIRROR_RETRIEVE																									((SOC_PETRA_INTERNAL_FUNCS_BASE + 1312)|SOC_PETRA_PROC_BITS)
#define SOC_PA_CFC_REGS_MIRROR_LOAD 																									  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1313)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_CTRL_CELLS_EN_FAST 																									  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1314)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_INDIRECT_INIT																											((SOC_PETRA_INTERNAL_FUNCS_BASE + 1315)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_INDIRECT_CLOSE                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1316)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_INDIRECT_FREE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1317)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_INDIRECT_MALLOC                                                   ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1318)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_INDIRECT_WRITE                                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1319)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_INDIRECT_OP_WRITE                                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1320)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_ENTRY_READ                                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1321)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_ENTRY_COMPARE                                                     ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1322)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_ENTRY_WRITE                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1323)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_INDIRECT_RANGE_COMPARE                                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1324)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_SHD_SCRUB_RANGE                                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1325)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY_UNSAFE                                 ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1326)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_PCKT_FROM_BUFF_READ                                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1327)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_PCKT_FROM_BUFF_READ_UNSAFE                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1328)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DRAM_PCKT_FROM_BUFF_READ_VERIFY                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1329)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_QDR_SET_REPS_FOR_TBL_UNSAFE                                           ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1330)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_FAPS_WITH_NO_MESH_LINKS_GET                                       ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1331)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DBG_FAPS_WITH_NO_MESH_LINKS_GET_UNSAFE                                ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1332)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_MBIST_RUN								                                        ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1333)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_DIAG_MBIST_RUN_UNSAFE								                                  ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1334)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ECN_ENABLE_SET								                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1335)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ECN_ENABLE_SET_UNSAFE								                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1336)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ECN_ENABLE_SET_VERIFY								                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1337)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ECN_ENABLE_GET								                                    ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1338)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_ITM_ECN_ENABLE_GET_UNSAFE								                              ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1339)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_PORTS_EXPECTED_CHAN_GET                                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1340)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SINGLE_CLEAR_WITH_INSTANCE_UNSAFE                               ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1342)|SOC_PETRA_PROC_BITS)
#define SOC_PETRA_INTERRUPT_SPECIFIC_CAUSE_NOF_INSTANCES_GET                            ((SOC_PETRA_INTERNAL_FUNCS_BASE + 1343)|SOC_PETRA_PROC_BITS)

/*
 * Procedure identifiers.
 * }

 */

/*
 * Error codes
 * {
 */
typedef enum SOC_PETRA_ERR_LIST
{
/*
 * Start Error number for specific errors in the SOC_PETRA driver.
 */
  SOC_PETRA_START_ERR_LIST_NUMBER      = SOC_SAND_PETRA_START_ERR_NUMBER,
  SOC_PETRA_DO_NO_USE_0001,
  SOC_PETRA_DO_NO_USE_0002,
  SOC_PETRA_REGS_NOT_INITIALIZED,
  SOC_PETRA_TBLS_NOT_INITIALIZED,
  SOC_PETRA_FLD_OUT_OF_RANGE,
/*
 * Procedure soc_petra_get_err_text() reports:
 * Input variable 'err_id' out of range (unknown value).
 */
  SOC_PETRA_GET_ERR_TEXT_001,
/*
 * Procedure soc_petra_proc_id_to_string() reports:
 * Input variable 'proc_id' out of range (unknown value).
 */
  SOC_PETRA_PROC_ID_TO_STRING_001,

  /*
   divide by zero error
  */
  SOC_PETRA_DIVISION_BY_ZERO_ERR,

  /*
    The weight value is out of range
   */
  SOC_PETRA_ING_SCH_WEIGHT_OUT_OF_RANGE_ERR,
  /*
    Max Credit (max_burst) Value is out of range
   */
  SOC_PETRA_ING_SCH_MAX_CREDIT_OUT_OF_RANGE_ERR,
  /*
    Id of context in the context array of the mesh info structure is out of range
   */
  SOC_PETRA_ING_SCH_MESH_ID_OUT_OF_RANGE_ERR,
  /*
    nof_entries of the mesh info structure is out of range
   */
  SOC_PETRA_ING_SCH_MESH_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  /*
    exact cal value calculation exceeds maximum cal value
  */
  SOC_PETRA_ING_SCH_EXACT_CAL_LARGER_THAN_MAXIMUM_VALUE_ERR,
  /*
    Soc_petra ITM credit request hungry threshold is out of range
   */
  SOC_PETRA_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR,
  /*
    Soc_petra ITM credit request hungry multiplier is out of range
   */
  SOC_PETRA_ITM_CR_REQUEST_HUNGRY_TH_MULTIPLIER_OUT_OF_RANGE_ERR,
  /*
    Soc_petra ITM credit request satisfied (man & exp) threshold is out of range
  */
  SOC_PETRA_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR,
  /*
    Soc_petra ITM credit request satisfied empty queues threshold is out of range
  */
  SOC_PETRA_ITM_CR_REQUEST_SATISFIED_EMPTY_Q_TH_OUT_OF_RANGE_ERR,
  /*
    Soc_petra ITM credit request watchdog threshold is out of range
  */
  SOC_PETRA_ITM_CR_REQUEST_WD_TH_OUT_OF_RANGE_ERR,
  /*
    Soc_petra ITM credit discount value is out of range
  */
  SOC_PETRA_ITM_CR_DISCOUNT_OUT_OF_RANGE_ERR,
  /*
    Soc_petra ITM credit class index is out of range
  */
  SOC_PETRA_ITM_CR_CLS_OUT_OF_RANGE_ERR,
  /*
    Soc_petra ITM wred exponential weight parameter is out of range
  */
  SOC_PETRA_ITM_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_CATEGORY_END_OUT_OF_ORDER_ERR,
  SOC_PETRA_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR,
  /*
    Soc_petra ITM admit test index is out of range
  */
  SOC_PETRA_ITM_ADMT_TEST_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR,
  SOC_PETRA_DRPP_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_ADMT_TEST_ID_OUT_OF_RANGE_ERR,


  SOC_PETRA_ITM_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR,
  SOC_PETRA_ITM_WRED_MIN_AVRG_TH_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_WRED_MAX_AVRG_TH_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_WRED_PROB_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR,
  SOC_PETRA_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_MAX_INST_Q_SIZ_PARAMETER_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR,
  SOC_PETRA_ITM_VSQ_MIN_AVRG_TH_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_MAX_AVRG_TH_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_LBL_IN_STAT_TAG_LSB_LARGER_THAN_MSB_ERR,
  SOC_PETRA_ITM_VSQ_LBL_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_LBL_DP_LSB_OUT_OF_RANGE_ERR,
  SOC_PETRA_SIGNATURE_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_ING_SHAPE_SCH_PORT_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_ING_SHAPE_SCH_RATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR,
  SOC_PETRA_MULTI_FABRIC_QUEUE_ORDER_ERR,
  SOC_PETRA_ITM_ING_SHAPE_Q_LOW_ABOVE_HIGH_NUM_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_PRIORITY_MAP_SEGMENT_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_SYS_RED_DRP_PROB_INDEX_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_SYS_RED_EG_INFO_AGING_TIMER_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_SYS_RED_GLOB_RCS_RNG_THS_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SNOOP_COMMAND_INDEX_OUT_OF_RANGE_ERR,
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_BASE_QUEUE_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_TR_CLS_OUT_OF_RANGE_ERR,
  SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORTS_MIRROR_PORT_INDEX_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORTS_MIRROR_DROP_PRCDNC_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORTS_MIRROR_TR_CLS_OUT_OF_RANGE_ERR,
  SOC_PETRA_SYS_PHYSICAL_PORT_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_IPQ_K_QUEUE_INDEX_OUT_OF_RANGE_ERR,
  SOC_PETRA_FLOW_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_IPQ_BASE_FLOW_FOR_INTERDIGIT_QUEUE_QUARTET_TOO_LOW_ERR,
  SOC_PETRA_IPQ_BASE_FLOW_ALREADY_MAPPED_BY_PREVIOUS_QUEUE_QUARTET_ERR,
  SOC_PETRA_IPQ_BASE_FLOW_QUARTET_NOT_EVEN_ERR,
  SOC_PETRA_IPQ_BASE_FLOW_QUARTET_NOT_MULTIPLY_OF_FOUR_ERR,
  SOC_PETRA_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR,
  SOC_PETRA_MULT_FABRIC_ILLEGAL_NUMBER_OF_QUEUE_ERR,
  SOC_PETRA_MULT_FABRIC_ILLEGAL_CONF_ERR,
  SOC_PETRA_MULT_FABRIC_ILLEGAL_NOF_LINKS,
  SOC_PETRA_MULT_EG_ILLEGAL_GROUP_RANG_CONFIG_ERR,
  SOC_PETRA_MULT_EG_ILLEGAL_MULT_ID_ERR,
  SOC_PETRA_MULT_EG_ILLEGAL_NOF_REPLICATIONS_CONFIG_ERR,
  SOC_PETRA_MULT_EG_MULTICAST_ID_NOT_IN_VLAN_MEMBERSHIP_RNG_ERR,
  SOC_PETRA_MULT_EG_MULTICAST_ID_IS_IN_VLAN_MEMBERSHIP_RNG_ERR,
  SOC_PETRA_MULT_EG_ILLEGAL_VLAN_PTR_ENTRY_IN_LINK_LIST_ERR,
  SOC_PETRA_MULT_ENTRY_DOES_NOT_EXIST,
  SOC_PETRA_MULT_MC_GROUP_REAL_SIZE_LARGER_THAN_GIVEN_SIZE_ERR,
  SOC_PETRA_MULT_LL_ILLEGAL_ENTRY_FOR_RELOCATION_ERR,
  SOC_PETRA_MULT_LL_ENTRY_NULL_CANT_BE_IN_MIDDLE_OF_LINK_LIST_ERR,
  SOC_PETRA_MULT_LL_ILLEGAL_ENTRY_ERR,
  SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_MULT_REMOVE_ELEMENT_DID_NOT_SUCCEED_ERROR,
  SOC_PETRA_PACKET_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_REG_ADDR_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_EPB_TIMEOUT_ERR,
  SOC_PETRA_DIAG_MBIST_POLL_TIMEOUT_ERR,
  SOC_PETRA_SRD_LLA_EPB_CMD_READ_ERR,
  SOC_PETRA_SRD_LANE_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_STAR_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_QRTT_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_LANE_LOOPBACK_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_LANE_EQ_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_LANE_STATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_RATE_DIVISOR_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_AMP_VAL_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_MEDIA_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_EXPLCT_PRE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_EXPLCT_POST_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_EXPLCT_SWING_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_ATTEN_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_MAIN_VAL_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_PRE_EMPHASIS_VAL_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_POST_EMPHASIS_VAL_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_UNEXPECTED_LOOPBACK_MODE_ERR,
  SOC_PETRA_SRD_MORE_THAN_ONE_LOOPBACK_MODE_DETECTED_ERR,
  SOC_PETRA_SRD_LOOPBACK_ENABLED_BUT_NO_MODE_ERR,
  SOC_PETRA_SRD_EQ_MORE_THAN_ONE_MODE_DETECTED_ERR,
  SOC_PETRA_SRD_UNEXPECTED_LANE_EQ_MODE_ERR,
  SOC_PETRA_SRD_CMU_ELEMENT_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_LANE_ELEMENT_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_EYE_SCAN_RESOLUTION_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_EYE_SCAN_PRBS_DURATION_MIN_SEC_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_EYE_SCAN_RANGE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_ELEMENT_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_INVALID_SIMPLE_FLOW_ID_ERR,
  SOC_PETRA_SCH_INVALID_FLOW_ID_ERR,
  SOC_PETRA_SCH_INVALID_SE_ID_ERR,
  SOC_PETRA_SCH_INVALID_PORT_ID_ERR,
  SOC_PETRA_SCH_INVALID_K_FLOW_ID_ERR,
  SOC_PETRA_SCH_INVALID_QUARTET_ID_ERR,
  SOC_PETRA_SCH_AGGR_SE_AND_FLOW_ID_MISMATCH_ERR,
  SOC_PETRA_SCH_CL_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_CL_CLASS_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_DESCRETE_WEIGHT_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_CL_CLASS_WEIGHTS_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_CL_ENHANCED_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_DEVICE_IF_WEIGHT_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_FABRIC_LINK_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_FLOW_HR_AND_SCHEDULER_MODE_MISMATCH_ERR,
  SOC_PETRA_SCH_FLOW_HR_CLASS_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_FLOW_ID_IS_SECOND_SUB_FLOW_ERR,
  SOC_PETRA_SCH_FLOW_STATUS_NOT_ON_OFF_ERR,
  SOC_PETRA_SCH_FLOW_TO_FIP_SECOND_QUARTET_MISMATCH_ERR,
  SOC_PETRA_SCH_FLOW_TO_Q_INVALID_GLOBAL_CONF_ERR,
  SOC_PETRA_SCH_FLOW_TO_Q_NOF_QUARTETS_MISMATCH_ERR,
  SOC_PETRA_SCH_FLOW_TO_Q_ODD_EVEN_IS_FALSE_ERR,
  SOC_PETRA_SCH_FLOW_TYPE_UNDEFINED_ERR,
  SOC_PETRA_SCH_GAP_IN_SUB_FLOW_ERR,
  SOC_PETRA_SCH_GROUP_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_GRP_AND_PORT_RATE_MISMATCH_ERR,
  SOC_PETRA_SCH_NOF_CLASS_TYPES_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_HP_CLASS_IDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_HP_CLASS_NOT_AVAILABLE_ERR,
  SOC_PETRA_SCH_HP_CLASS_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_HP_CLASS_VAL_INVALID_ERR,
  SOC_PETRA_SCH_HR_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_CLCONFIG_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_ENH_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_INTERFACE_IS_SINGLE_PORT_ERR,
  SOC_PETRA_SCH_INVALID_IF_TYPE_ERR,
  SOC_PETRA_SCH_INTERFACE_RATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_INTERFACE_WEIGHT_INDEX_OUT_OF_RANGE_ERR,
  SOC_PETRA_INTERFACE_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_INTERFACE_INDEX_OUT_OF_RANGE_ERR,
  SOC_PETRA_INVALID_CH_NIF_INDEX_ERR,
  SOC_PETRA_INVALID_PORT_NIF_INDEX_ERR,
  SOC_PETRA_SCH_INVALID_PORT_GROUP_ERR,
  SOC_PETRA_SCH_SE_ID_AND_TYPE_MISMATCH_ERR,
  SOC_PETRA_SCH_SE_PORT_RATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_SE_PORT_SE_TYPE_NOT_HR_ERR,
  SOC_PETRA_SCH_SE_STATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_SE_TYPE_SE_CONFIG_MISMATCH_ERR,
  SOC_PETRA_SCH_SE_TYPE_UNDEFINED_ERR,
  SOC_PETRA_SCH_SLOW_RATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR,
  SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_TYPE_MISMATCH_ERR,
  SOC_PETRA_SCH_SUB_FLOW_ATTACHED_TO_DISABLED_SCHEDULER_ERR,
  SOC_PETRA_SCH_SUB_FLOW_CLASS_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_SUB_FLOW_ENHANCED_SP_MODE_MISMATCH_ERR,
  SOC_PETRA_SCH_SUB_FLOW_ID_MISMATCH_WITH_FLOW_ID_ERR,
  SOC_PETRA_SCH_SUB_FLOW_SE_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SCH_SUB_FLOW_WEIGHT_OUT_OF_RANGE_ERR,
  SOC_PETRA_FAP_PORT_ID_INVALID_ERR,
  SOC_PETRA_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR,
  SOC_PETRA_REGS_FIELD_VAL_OUT_OF_RANGE_ERR,
  SOC_PETRA_DROP_PRECEDENCE_OUT_OF_RANGE_ERR,
  SOC_PETRA_TRAFFIC_CLASS_OUT_OF_RANGE_ERR,
  SOC_PETRA_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR,
  SOC_PETRA_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR,
  SOC_PETRA_EGR_MANTISSA_OUT_OF_RANGE_ERR,
  SOC_PETRA_EGR_EXPONENT_OUT_OF_RANGE_ERR,
  SOC_PETRA_EGR_MNT_EXP_FLD_OUT_OF_RANGE_ERR,
  SOC_PETRA_NOT_A_CHANNELIZED_IF_ERR,
  SOC_PETRA_EGR_MCI_PRIO_OUT_OF_RANGE_ERR,
  SOC_PETRA_EGR_MCI_PRIO_AND_ID_MISMATCH_ERR,
  SOC_PETRA_EGR_MCI_ERP_AND_MCI_ENABLE_ERR,
  SOC_PETRA_EGR_OFP_SCH_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_EGR_OFP_CHNIF_PRIO_OUT_OF_RANGE_ERR,
  SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR,
  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORTS_INVALID_ECI_PORT_IDX_ERR,
  SOC_PETRA_NIF_INDEX_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_SGMII_INTERFACE_NOF_SINGLE_RATE_ERR,
  SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_DIRECTION_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_INVALID_TYPE_ERR,
  SOC_PETRA_NIF_IPG_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_PREAMBLE_AND_BCT_CONF_CONFLICT_ERR,
  SOC_PETRA_NIF_BCT_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_BCT_CHANNEL_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_MAL_INVALID_CONFIG_STATUS_ON_ERR,
  SOC_PETRA_NIF_TWO_LAST_MALS_SERDES_OVERLAP_ERR,
  SOC_PETRA_NIF_INCOMPATIBLE_TYPE_ERR,
  SOC_PETRA_NIF_MDIO_LESS_THEN_ONE_WORD_DATA_ERR,
  SOC_PETRA_NIF_COUNTER_OUT_OF_RANGE_ERR,
  SOC_PETRA_IPQ_NIF_ID_NOT_FIRST_IN_MAL_ERR,
  SOC_PETRA_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR,
  SOC_PETRA_DBG_FORCE_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DBG_Q_FLUSH_ALL_TIMEOUT_ERR,
  SOC_PETRA_DBG_Q_FLUSH_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DBG_FORCE_MODE_FLD_OUT_OF_RANGE_ERR,
  SOC_PETRA_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR,
  SOC_PETRA_FABRIC_FAP20_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_FC_INGR_NOF_GEN_NIF_LL_OUT_OF_RANGE_ERR,
  SOC_PETRA_FC_INGR_NOF_GEN_NIF_CB_OUT_OF_RANGE_ERR,
  SOC_PETRA_FC_NIF_CB_CLASSES_OUT_OF_RANGE_ERR,
  SOC_PETRA_FC_NIF_CB_SGMII_INVALID_CONF_ERR,
  SOC_PETRA_FC_RCY_CONNECTION_CLS_OUT_OF_RANGE_ERR,
  SOC_PETRA_FC_NOF_RCY_OFP_HANDLERS_OUT_OF_RANGE_ERR,
  SOC_PETRA_FC_RCY_ON_GLB_RCS_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_FC_OOB_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_FC_OOB_CAL_REP_OF_RANGE_ERR,
  SOC_PETRA_FC_OOB_CAL_LEN_OF_RANGE_ERR,
  SOC_PETRA_FC_OOB_CAL_SRC_TYPE_INVALID_ERR,
  SOC_PETRA_FC_OOB_CAL_SRC_ID_OF_RANGE_ERR,
  SOC_PETRA_FC_OOB_CAL_DEST_TYPE_INVALID_ERR,
  SOC_PETRA_FC_OOB_CAL_EXCESSIVE_NOF_SCH_OFP_HRS_ERR,
  SOC_PETRA_FC_OOB_CAL_DEST_ID_OF_RANGE_ERR,
  SOC_PETRA_IHP_STAG_OFFSET_OUT_OF_RANGE_ERR,
  SOC_PETRA_IHP_TMLAG_OFFSET_BASE_OUT_OF_RANGE_ERR,
  SOC_PETRA_IHP_TMLAG_OFFSET_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_IHP_SOP2HEADER_OFFSET_OUT_OF_RANGE_ERR,
  SOC_PETRA_IHP_STRIP_FROM_SOP_OUT_OF_RANGE_ERR,
  SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR,
  SOC_PETRA_REVISION_SUB_TYPE_OUT_OF_LIMIT_ERR,
  SOC_PETRA_CHIP_TYPE_UNKNOWN_ERR,
  SOC_PETRA_CREDIT_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_FAP_FABRIC_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_PP_ENABLE_NOT_SUPPORTED_ERR,
  SOC_PETRA_SYSTEM_PHYSICAL_PORT_OUT_OF_RANGE_ERR,
  SOC_PETRA_SYSTEM_PORT_OUT_OF_RANGE_ERR,
  SOC_PETRA_CUD_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_DEVICE_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORT_DIRECTION_OUT_OF_RANGE_ERR,
  SOC_PETRA_IF_CHANNEL_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORT_LAG_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORT_LAG_NOF_MEMBERS_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORT_LAG_SYS_PORT_ALREADY_MEMBER_ERR,
  SOC_PETRA_DEVICE_ID_ABOVE_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORTS_OTMH_OUTLIF_EXT_PERMISSION_ERR,
  SOC_PETRA_CELL_VARIABLE_IN_FAP20_SYSTEM_ERR,
  SOC_PETRA_CELL_VAR_SIZE_IN_FE200_SYSTEM_ERR,
  SOC_PETRA_HW_DRAM_CONF_LEN_OUT_OF_RANGE_ERR,
  SOC_PETRA_HW_DRAM_NOF_INTERFACES_OUT_OF_RANGE_ERR,
  SOC_PETRA_HW_QDR_PROTECT_TYPE_INVALID_ERR,
  SOC_PETRA_HW_QDR_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_MGMT_EGQ_INIT_FAILS_ERR,
  SOC_PETRA_NIF_NOF_PORTS_IN_FAT_PIPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_LAG_GROUP_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_LAG_ENTRY_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_DEST_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DEST_SYS_PORT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR,
  SOC_PETRA_OUTBND_MIRR_IFP_NOT_MAPPED_TO_RCY_IF_ERR,
  SOC_PETRA_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_AQFM_CREDIT_SOURCE_ID_ERR,
  SOC_PETRA_AQFM_SCH_SUB_FLOW_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_AQFM_CANT_ALLOC_AGG_ID_ERR,
  SOC_PETRA_AQFM_INVALID_PORT_ID_ERR,
  SOC_PETRA_SRD_TX_LANE_DATA_RATE_DIVISOR_INVALID_ERR,
  SOC_PETRA_SRD_CMU_REF_CLK_OUT_OF_RANGE_ERR,
  SOC_PETRA_MGMT_DRAM_INIT_FAILS_ERR,
  SOC_PETRA_PORTS_IF_EXCEEDED_MAX_ITERATIONS_ERR,
  SOC_PETRA_OFP_SHPR_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_OFP_MAL_INDEX_MISMATCH_ERR,
  SOC_PETRA_NIF_FC_LL_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_ENTITY_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_FLD_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_MGMT_BASIC_CONF_NULL_AT_INIT_ERR,
  SOC_PETRA_MGMT_BASIC_CONF_NOT_SUPPLIED_ERR,
  SOC_PETRA_FABRIC_CONNECT_MESH_MODE_CHANGE_ERR,
  SOC_PETRA_FTMH_EXTENSION_OUT_OF_RANGE_ERR,
  SOC_PETRA_MGMT_QDR_NOT_READY_ERR,
  SOC_PETRA_MGMT_QDR_INIT_BIST_DID_NOT_FINISH_ERR,
  SOC_PETRA_MGMT_QDRC_NOT_LOCKED_ERR,
  SOC_PETRA_MGMT_QDR_TRAINING_REPLY_FAIL_ERR,
  SOC_PETRA_INIT_QDR_PLL_NOT_LOCKED_ERR,
  SOC_PETRA_INIT_DDR_PLL_NOT_LOCKED_ERR,
  SOC_PETRA_INIT_CORE_PLL_NOT_LOCKED_ERR,
  SOC_PETRA_SRD_8051_CHECKSUM_STILL_RUNNING_ERR,
  SOC_PETRA_SRD_8051_CHECKSUM_FAIL_ERR,
  SOC_PETRA_SRD_8051_CHECKSUM_DID_NOT_COMPLETE_ERR,
  SOC_PETRA_SRD_TRIM_FAILED_ERR,
  SOC_PETRA_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR,
  SOC_PETRA_HW_INVALID_NOF_BANKS_FOR_DRAM_TYPE_ERR,
  SOC_PETRA_HW_INVALID_NOF_BANKS_ERR,
  SOC_PETRA_HW_NOF_COLUMNS_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_TX_TRIMMING_DID_NOT_END_ERR,
  SOC_PETRA_SRD_PLL_RESET_DID_NOT_END_ERR,
  SOC_PETRA_SRD_LN_RX_RESET_DID_NOT_END_ERR,
  SOC_PETRA_NIF_SRD_RATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_MAL_IS_FABRIC_NOT_NIF_ERR,
  SOC_PETRA_STAT_NO_SUCH_COUNTER_ERR,
  SOC_PETRA_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR,
  SOC_PETRA_SRD_PLL_CONF_NOT_FOUND_ERR,
  SOC_PETRA_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAG_PATTERN_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAG_BIST_ADDRESS_ILLEGAL_RANGE_ERR,
  SOC_PETRA_DIAG_DRAM_OFFSET_ILLEGAL_RANGE_ERR,
  SOC_PETRA_DIAG_INCONSISTENT_DRAM_CONFIG_ERR,
  SOC_PETRA_DIAG_BIST_DATA_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAG_DUMP_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAG_BIST_DATA_PATERN_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAG_DRAM_ACCESS_TIMEOUT_ERR,
  SOC_PETRA_DIAG_DRAM_ACC_NOF_COLUMNS_INVALID_ERR,
  SOC_PETRA_SRD_ACCESS_INTERNAL_REG_WITH_EXTERNAL_CONF_ERR,
  SOC_PETRA_SRD_CMU_VER_MISMATCH_ERR,
  SOC_PETRA_DBUFF_SIZE_INVALID_ERR,
  SOC_PETRA_PKT_TX_CPU_PACKET_BYTE_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_PKT_ASYNC_MODE_CONFIG_ERR,
  SOC_PETRA_FAT_PIPE_NO_SEQ_HDR_ERR,
  SOC_PETRA_PORT_ID_CONSUMED_BY_FAT_PIPE_ERR,
  SOC_PETRA_FAT_PIPE_NOT_SET_ERR,
  SOC_PETRA_FAT_PIPE_MULTIPLE_PORTS_ERR,
  SOC_PETRA_FAT_PIPE_MUST_BE_FIRST_ERR,
  SOC_PETRA_FAT_PIPE_INVALID_PORT_ID_ERR,
  SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_TBL_PROG_IDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_TBL_INSTR_IDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_TBL_PRGR_COS_SET_IDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_INVALID_REF_CLOCK_ERR,
  SOC_PETRA_SRD_TX_CONF_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_INVALID_RATE_ERR,
  SOC_PETRA_SRD_MISCONFIGURED_RATE_ON_POWERUP_ERR,
  SOC_PETRA_SRD_MISCONFIGURED_RATE_ERR,
  SOC_PETRA_UNSUPPORTED_ERR,
  SOC_PETRA_UNSUPPORTED_FOR_DEVICE_ERR,
  SOC_PETRA_SRD_PLL_NOT_LOCKED_ERR,
  SOC_PETRA_SRD_INVALID_RATE_FOR_FABRIC_MAC_ERR,
  SOC_PETRA_FBR_LINK_INVALID_ERR,
  SOC_PETRA_SRD_AEQ_MODE_INVALID_ERR,
  SOC_PETRA_FAP_PORT_ID_INVALID_WHEN_FAT_PIPE_ENABLED_ERR,
  SOC_PETRA_SRD_PRBS_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DRAM_BUFFERS_UC_FBC_OVERFLOW_ERR,
  SOC_PETRA_DRAM_BUFFERS_FBC_OVERFLOW_ERR,
  SOC_PETRA_SCH_SLOW_RATE_INDEX_INVALID_ERR,
  SOC_PETRA_SCH_INTERNAL_SLOW_RATE_INDEX_INVALID_ERR,
  SOC_PETRA_IPQ_INVALID_QUEUE_ID_ERR,
  SOC_PETRA_NIF_SGMII_RATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_FBR_LINK_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_FBR_LINK_ON_OFF_STATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_1000BASE_X_INVALID_RATE_ERR,
  SOC_PETRA_CALLBACK_ALREADY_REGISTERED_ERR,
  SOC_PETRA_ITM_INGRESS_SHAPING_LOW_BELOW_HIGH_ERR,
  SOC_PETRA_BASE_Q_NOT_SET_ERR,
  SOC_PETRA_SSR_FORBIDDEN_FUNCTION_CALL_ERR,
  SOC_PETRA_SSR_INCOMPATIBLE_SRC_VERSION_ERR,
  SOC_PETRA_SW_DB_BUFF_SIZE_MISMATCH_ERR,
  SOC_PETRA_FC_INGR_GEN_GLB_HP_INVALID_ERR,
  SOC_PETRA_NIF_FC_RX_DISABLE_ERR,
  SOC_PETRA_SRD_INIT_TRIM_ERR,
  SOC_PETRA_NIF_SRD_LPBCK_MODE_INCONSISTENT_ERR,
  SOC_PETRA_INCOMPATABLE_NIF_ID_ERR,
  SOC_PETRA_PCKT_SIZE_VSC_BELOW_MIN_ERR,
  SOC_PETRA_PCKT_SIZE_FSC_BELOW_MIN_ERR,
  SOC_PETRA_PCKT_SIZE_VSC_ABOVE_MAX_ERR,
  SOC_PETRA_PCKT_SIZE_FSC_ABOVE_MAX_ERR,
  SOC_PETRA_PCKT_SIZE_MIN_EXCEEDS_MAX_ERR,
  SOC_PETRA_MIN_PCKT_SIZE_INCONSISTENT_ERR,
  SOC_PETRA_TEXT_NO_ERR_TXT_FOUND_ERR,
  SOC_PETRA_SRD_EPB_ACCESS_ERR,
  SOC_PETRA_ECI_ACCESS_ERR,
  SOC_PETRA_SRD_LN_TRIM_DONE_IS_DOWN_ERR,
  SOC_PETRA_SRD_LN_CLCK_RLS_IS_DOWN_ERR,
  SOC_PETRA_SRD_LN_CLCK_RXTDACDONE_IS_DOWN_ERR,
  SOC_PETRA_OP_MODE_A1_OR_BELOW_LOCAL_MISMATCH_ERR,
  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_Q_ID_WITH_BASE_Q_MISMATCH_ERR,
  SOC_PETRA_MEM_CORRECTION_RM_BIT_INCONSISTENT_ERR,
  SOC_PETRA_MEM_CORRECTION_ERR,
  SOC_PETRA_MEM_BIST_ERR,
  SOC_PETRA_CELL_DIFFERENT_CELL_IDENT_ERR,
  SOC_PETRA_CELL_NO_RECEIVED_CELL_ERR,
  SOC_PETRA_CELL_WRITE_OUT_OF_BOUNDARY,
  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR,
  SOC_PETRA_SCH_SUB_FLOW_INVALID_ERR,
  SOC_PETRA_SRD_VALIDATE_AND_RELOCK_FAILED_ERR,
  SOC_PETRA_STAG_ENABLE_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_QUEUE_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_IPU_IRQ_ACK_DOWN_ERR,
  SOC_PETRA_NIF_ON_OFF_STATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_NIF_SGMII_0_OFF_INVALID_ERR,
  SOC_PETRA_SRD_NOT_FABRIC_QUARTET_ERR,
  SOC_PETRA_SRD_TX_EXPLCT_FAIL_ERR,
  SOC_PETRA_SRD_TX_ATTEN_FAIL_ERR,
  SOC_PETRA_SRD_TX_ATTEN_ABOVE_3_125_FAIL_ERR,
  SOC_PETRA_MDIO_OP_TIMEOUT_ERR,
  SOC_PETRA_LBG_PATTERN_DATA_SIZE_OUT_OF_RANGE_ERR,
  SOC_PETRA_ALLOC_TO_NON_NULL_ERR,
  SOC_PETRA_DRAM_CONF_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_CR_WD_DEL_TH_OUT_OF_RANGE,
  SOC_PETRA_SCH_FLOW_AND_SE_TYPE_MISMATCH_ERR,
  SOC_PETRA_PORT_LAG_MEMBER_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_FEATURE_NOT_SUPPORTED_AT_REVISION_ERR,
  SOC_PETRA_ITM_CR_REQ_TYPE_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_CREDIT_CLS_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_RATE_CLS_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_CONNECTION_CLS_OUT_OF_RANGE_ERR,
  SOC_PETRA_ITM_VSQ_TRAFFIC_CLS_OUT_OF_RANGE_ERR,
  SOC_PETRA_UNSCHED_PRIO_OUT_OF_RANGE_ERR,
  SOC_PETRA_EGR_MC_16K_ENABLE_INCOMPATIBLE_ERR,
  SOC_PETRA_INGR_RST_INTERN_BLOCK_INIT_ERR,
  SOC_PETRA_SCH_ILLEGAL_COMPOSITE_AGGREGATE_ERR,
  SOC_PETRA_SCH_COMPOSITE_AGGREGATE_DUAL_SHAPER_ERR,
  SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR,
  SOC_PETRA_OFP_RATES_TBL_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PETRA_OFP_RATES_SCH_RATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_OFP_RATES_EGQ_RATE_OUT_OF_RANGE_ERR,
  SOC_PETRA_OFP_RATES_BURST_OUT_OF_RANGE_ERR,
  SOC_PETRA_OFP_RATES_PORTS_FROM_DIFFERENT_MALS_ERR,
  SOC_PETRA_OFP_RATES_CAL_LEN_INVALID_ERR,
  SOC_PETRA_OFP_RATES_CAL_NO_SLOTS_AVAILABLE_ERR,
  SOC_PETRA_OFP_RATES_CAL_ALLOCATED_AND_REQUESTED_LEN_MISMATCH_ERR,
  SOC_PETRA_OFP_RATES_FAP_PORTS_OUT_OF_RANGE_ERR,
  SOC_PETRA_OFP_RATES_SCH_PORT_ID_OUT_OF_RANGE_ERR,
  SOC_PETRA_OFP_RATES_OFP_INDEX_MISMATCH_ERR,
  SOC_PETRA_OFP_RATES_NOF_ITERATIONS_EXCEEDS_LIMITS_ERR,
  SOC_PETRA_OFP_RATES_PORT_HAS_NO_IF_ERR,
  SOC_PETRA_OFP_RATES_CONSECUTIVE_CREDIT_DISTRIBUTION_ERR,
  SOC_PETRA_OFP_RATES_ACTUAL_AND_EXPECTED_RATE_DIFFERENCE_ERR,
  SOC_PETRA_NON_MESH_CONF_ERR,
  SOC_PETRA_FULL_PCKT_MODE_IN_MESH_CONF_ERR,
  SOC_PETRA_DRAM_AUTO_MODE_IS_NOT_SUPPORTED_ERR,
  SOC_PETRA_DRAM_INVALID_DRAM_TYPE_ERR,
  SOC_PETRA_DRAM_DPRC_POLL_FAIL_ERR,
  SOC_PETRA_FABRIC_CRC_NOT_SUPPORTED_ERR,
  SOC_PETRA_CELL_DATA_OUT_OF_RANGE_ERR,
  SOC_PETRA_CELL_FE_LOCATION_OUT_OF_RANGE_ERR,
  SOC_PETRA_CELL_PATH_LINKS_OUT_OF_RANGE_ERR,
  SOC_PETRA_MC_ID_NDX_OUT_OF_RANGE_ERR,
  SOC_PETRA_REG_ACCESS_NO_PA_ERR,
  SOC_PETRA_REG_ACCESS_NO_PB_ERR,
  SOC_PETRA_REG_ACCESS_UNKNOWN_DEVICE_ERR,
  SOC_PETRA_STAT_MULT_ID_UNSUPPORTED_ERR,
  SOC_PETRA_STAT_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_LANE_AND_QRTT_ENABLE_MISMATCH_ERR,
  SOC_PETRA_NIF_MAL_SGMII_CONF_MISMATCH_ERR,
  SOC_PETRA_NIF_SPAUI_INTRLV_BCT_SIZE_ERR,
  SOC_PETRA_SRD_EYE_SCAN_CRC_NOT_FABRIC_ERR,
  SOC_PETRA_SRD_EYE_SCAN_FEC_NOT_CONFIGURED_ERR,
  SOC_PETRA_SRD_CNT_SRC_OUT_OF_RANGE_ERR,
  SOC_PETRA_SRD_TRAFFIC_SRC_OUT_OF_RANGE_ERR,
  SOC_PETRA_CORE_FREQ_OUT_OF_RANGE_ERR,
  SOC_PETRA_CR_WD_DELETE_BEFORE_STATUS_MSG_ERR,
  SOC_PETRA_CR_WD_DEL_TH_UNSUPPORTED_ERR,
  SOC_PETRA_RATE_CONF_MODE_INCONSISTENT_ERR,
  SOC_PETRA_FUNC_CALL_NO_PA_ERR,
  SOC_PETRA_FUNC_CALL_NO_PB_ERR,
  SOC_PETRA_FUNC_CALL_UNKNOWN_DEVICE_ERR,
  SOC_PETRA_SRD_REF_CLK_OF_RANGE_ERR,
  SOC_PETRA_NIF_SPAUI_DS_CONF_NON_DS_BUS_ERR,
  SOC_PETRA_NIF_SPAUI_DS_SOP_ODD_EVEN_ERR,
  SOC_PETRA_PORTS_NON_CH_IF_ERR,
  SOC_PETRA_DRAM_NOF_BANKS_OUT_OF_RANGE_ERR,
  SOC_PETRA_DRAM_NOF_COLS_OUT_OF_RANGE_ERR,
  SOC_PETRA_DRAM_CL_OUT_OF_RANGE_ERR,
  SOC_PETRA_DRAM_WL_OUT_OF_RANGE_ERR,
  SOC_PETRA_DRAM_WR_OUT_OF_RANGE_ERR,
  SOC_PETRA_DRAM_BL_OUT_OF_RANGE_ERR,
  SOC_PETRA_INTERRUPT_DEVICE_BETWEEN_ISR_TO_TCM_ERR,
  SOC_PETRA_INTERRUPT_ILLEGAL_CAUSE_NUMBER_ERR,
  SOC_PETRA_INTERRUPT_INSUFFICIENT_MEMORY_ERR,
  SOC_PETRA_DRAM_NOF_CONF_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_PORTS_EG_TM_PROFILE_FULL_ERR,
  SOC_PETRA_SRD_RATE_UNKNOWN_ERR,
  SOC_PETRA_DIAG_QDR_REPLY_LOCK_CANNOT_LOCK_ERR,
  SOC_PETRA_DIAG_QDR_DLL_NOT_READY_ERR,
  SOC_PETRA_DIAG_QDR_TRAINING_FAIL_ERR,
  SOC_PETRA_DIAG_QDR_CANNOT_ACCESS_ERR,
  SOC_PETRA_DEPRICATED_ERR,
  SOC_PETRA_DBG_PCM_COUNTER_NOT_EXPIRED_ERR,
  SOC_PETRA_END2END_SCHEDULER_MODE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAGNOSTICS_COUNT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAGNOSTICS_PATTERN_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAGNOSTICS_SMS_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAGNOSTICS_ERR_SP_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAGNOSTICS_ERR_DP_OUT_OF_RANGE_ERR,
  SOC_PETRA_DIAGNOSTICS_ERR_RF_OUT_OF_RANGE_ERR,
  SOC_PETRA_INGR_SHP_Q_ABOVE_MAX_ERR,
  SOC_PETRA_DEBUG_RST_DOMAIN_OUT_OF_RANGE_ERR,
  SOC_PETRA_DBG_CFC_DB_CORRUPT_ERR,
  SOC_PETRA_MGMT_DRAM_INIT_RND_TRIP_FAILS_ERR,
  SOC_PETRA_API_NOT_FUNCTIONAL_ERR,
  SOC_PETRA_SRD_RATE_VCO_BELOW_MIN_ERR,
  SOC_PETRA_SRD_RATE_VCO_ABOVE_MAX_ERR,
  SOC_PETRA_DBG_STAT_IF_ENABLED_ERR,
  SOC_PETRA_SHD_NOT_INITIALIZED_ERR,
  SOC_PETRA_INTERRUPT_INVALID_MONITORED_CAUSE_ERR,
  SOC_PETRA_INT_INSTANCE_CLEAR_NOT_APPLICABLE_ERR,
  SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR,
  SOC_PETRA_WARMBOOT_PP_SCACHE_SIZE_TOO_SMALL_ERR,
 SOC_PETRA_LAST_ERR
} SOC_PETRA_ERR;

/*
 * Error codes
 * }
 */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PETRA_FLD_SIZE_BITS(msb, lsb) (msb - lsb + 1)

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* soc_petra_add_petra_errors
*TYPE:
*  PROC
*DATE:
*  08/14/2007
*FUNCTION:
*  Add the pool of SOC_PETRA errors to the all-system
*  sorted pool.
*CALLING SEQUENCE:
*  soc_petra_add_petra_errors()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error
*  SOC_SAND_INDIRECT:
*    Updated all-system pool of sorted error descriptors.
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_add_petra_errors(void);

/*****************************************************
*NAME
* soc_petra_add_petra_procedure_desc
*TYPE:
*  PROC
*DATE:
*  08/14/2007
*FUNCTION:
*  Add the pool of soc_petra procedure descriptors to
*  the all-system sorted pool.
*CALLING SEQUENCE:
* soc_petra_add_petra_procedure_desc()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error
*  SOC_SAND_INDIRECT:
*    Updated all-system pool of sorted procedure
*    descriptors.
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_add_petra_procedure_desc(void);

/* } */

/*****************************************************
*NAME
*  soc_petra_get_err_text
*TYPE:
*  PROC
*DATE:
*  25/SEP/2007
*FUNCTION:
*  This procedure finds the matching text describing
*  input error identifier.
*CALLING SEQUENCE:
*  soc_petra_get_err_text(err_id,err_name,err_text)
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32 err_id -
*      Identifier of the error for which description
*      is required. See list of ERROR RETURN VALUES above.
*    char         **err_name -
*      This procedure loads pointed memory with
*      pointer to null terminated string containing
*      error name. Caller may NOT change the
*      contents of this buffer.
*    char         **err_text -
*      This procedure loads pointed memory with
*      pointer to null terminated string containing
*      error description. Caller may NOT change the
*      contents of this buffer.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*      See formatting rules in SOC_PETRA_ERROR RETURN VALUES above.
*      If error code is not soc_petra_NO_ERR then
*        specific error codes:
*          SOC_PETRA_GET_ERR_TEXT_001 -
*            Input variable 'err_id' has unknown value
*            (could not be translated, as required).
*      Otherwise, no error has been detected and text
*        has been retrieved.
*  SOC_SAND_INDIRECT:
*    err_text
*REMARKS:
*  Note that text returned by this procedure does not
*  change during runtime and may only be changed via a
*  new compilation.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_get_err_text(
    uint32  err_id,
    char           **err_name,
    char           **err_text
  ) ;
/*****************************************************
*NAME
* soc_petra_proc_id_to_string
*TYPE:
*  PROC
*DATE:
*  17/FEB/2003
*FUNCTION:
*  Get ASCII names of module and procedure from input
*  procedure id.
*CALLING SEQUENCE:
*  soc_petra_proc_id_to_string(
*        proc_id,out_module_name,out_proc_name)
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32  proc_id -
*      Procedure id to locate name and module of.
*    char           **out_module_name -
*      This procedure loads pointed memory with
*      pointer to null terminated string containing
*      the name of the module.
*    char           **out_proc_name -
*      This procedure loads pointed memory with
*      pointer to null terminated string containing
*      the name of the procedure.
*  SOC_SAND_INDIRECT:
*    All-system procedure descriptor pools
*    (e.g. Soc_petra_procedure_desc_element).
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error has occurred and
*      procedure string has not been located.
*  SOC_SAND_INDIRECT:
*    See out_module_name, out_module_name.
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_proc_id_to_string(
    uint32 in_proc_id,
    char          **out_module_name,
    char          **out_proc_name
  ) ;

/*****************************************************
*NAME
* soc_petra_errors_ptr_get
*TYPE:
*  PROC
*DATE:
*  10/OCT/2007
*FUNCTION:
*  Get the pointer to the list of errors of the 'Soc_petra'
*  module.
*CALLING SEQUENCE:
*  soc_petra_errors_ptr_get()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    list of Soc_petra errors: Soc_petra_error_desc_element.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_ERROR_DESC_ELEMENT * -
*      Pointer to the static list of Soc_petra errors.
*  SOC_SAND_INDIRECT:
*    .
*REMARKS:
*  This utility is mainly for external users (to the 'Soc_petra'
*  module) such as 'soc_sand module'.
*SEE ALSO:
*
*****************************************************/
SOC_ERROR_DESC_ELEMENT
  *soc_petra_errors_ptr_get(
    void
  );

/*****************************************************
*NAME
* soc_petra_errors_add
*TYPE:
*  PROC
*DATE:
*  08/14/2007
*FUNCTION:
*  Add the pool of SOC_PETRA errors to the all-system
*  sorted pool.
*CALLING SEQUENCE:
*  soc_petra_errors_add()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error
*  SOC_SAND_INDIRECT:
*    Updated all-system pool of sorted error descriptors.
*REMARKS:
*SEE ALSO:
*****************************************************/

uint32
  soc_petra_errors_add(void);

/*****************************************************
*NAME
* soc_petra_procedure_desc_add
*TYPE:
*  PROC
*DATE:
*  08/14/2007
*FUNCTION:
*  Add the pool of soc_petra procedure descriptors to
*  the all-system sorted pool.
*CALLING SEQUENCE:
* soc_petra_procedure_desc_add()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error
*  SOC_SAND_INDIRECT:
*    Updated all-system pool of sorted procedure
*    descriptors.
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_procedure_desc_add(void);

uint32
  soc_petra_err_text_get(
    SOC_SAND_IN  uint32  err_id,
    SOC_SAND_OUT char      **err_name,
    SOC_SAND_OUT char      **err_text
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_FRAMEWORK_H_INCLUDED__*/
#endif

/* } */
