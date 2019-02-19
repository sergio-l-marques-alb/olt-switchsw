/*
* $Id$
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
* ARAD FABRIC CELL H
*/

#ifndef _SOC_ARAD_FABRIC_CELL_H_

#define _SOC_ARAD_FABRIC_CELL_H_

/* Relevant only for Flow Status Message cell */
#define ARAD_VSC256_CONTROL_CELL_FSM_DEST_PORT_START  0
#define ARAD_VSC256_CONTROL_CELL_FSM_DEST_PORT_LENGTH  8
#define ARAD_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_START  16
#define ARAD_VSC256_CONTROL_CELL_FSM_SRC_QUEUE_NUM_LENGTH  20
#define ARAD_VSC256_CONTROL_CELL_FSM_FLOW_ID_START  36
#define ARAD_VSC256_CONTROL_CELL_FSM_FLOW_ID_LENGTH  20

/* Relevant only for Credit Message cell */
#define ARAD_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_START  16
#define ARAD_VSC256_CONTROL_CELL_CREDIT_DEST_Q_NUM_LENGTH  20
#define ARAD_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_START  36
#define ARAD_VSC256_CONTROL_CELL_CREDIT_SUB_FLOW_ID_LENGTH  2
#define ARAD_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_START  38
#define ARAD_VSC256_CONTROL_CELL_CREDIT_FLOW_ID_LENGTH  18

/* Relevant only for Reachability cell */
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_START  0
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_BITMAP_LENGTH  32
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_START  64
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_BASE_ID_LENGTH  6
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_START  79
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_SRC_LINK_NUM_LENGTH  8

#define ARAD_VSC256_CONTROL_CELL_SOURCE_ID_START  85
#define ARAD_VSC256_CONTROL_CELL_SOURCE_ID_LENGTH  11
#define ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_START  96
#define ARAD_VSC256_CONTROL_CELL_DEST_DEVICE_LENGTH  11
#define ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_START  107
#define ARAD_VSC256_CONTROL_CELL_CONTROL_TYPE_LENGTH  3
#define ARAD_VSC256_CONTROL_CELL_REACHABILITY_SOURCE_ID_START  93

#define SR_CELL_TYPE 01

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dpp/dpp_fabric_cell.h>

soc_error_t soc_arad_cell_filter_set(int unit, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val);
soc_error_t soc_arad_cell_filter_clear(int unit);
soc_error_t soc_arad_cell_filter_receive(int unit,  dcmn_captured_cell_t* data_out);
soc_error_t soc_arad_control_cell_filter_set(int unit, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* filter_type_arr, uint32* filter_type_val);
soc_error_t soc_arad_control_cell_filter_clear(int unit);
soc_error_t soc_arad_control_cell_filter_receive(int unit,  soc_dcmn_captured_control_cell_t* data_out);

#endif /* _SOC_ARAD_FABRIC_CELL_H_ */
