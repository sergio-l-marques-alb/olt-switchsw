/*
 * $Id: fe3200_fabric_links.h,v 1.8.60.1 Broadcom SDK $
 *
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * FE3200 FABRIC LINKS H
 */
 
#ifndef _SOC_FE3200_FABRIC_LINKS_H_
#define _SOC_FE3200_FABRIC_LINKS_H_

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/error.h>
#include <soc/types.h>

soc_error_t soc_fe3200_fabric_links_cell_format_verify(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val);
soc_error_t soc_fe3200_fabric_links_flow_status_control_cell_format_set(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val);
soc_error_t soc_fe3200_fabric_links_flow_status_control_cell_format_get(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t *val);
soc_error_t soc_fe3200_fabric_links_weight_validate(int unit, int val);
soc_error_t soc_fe3200_fabric_links_weight_set(int unit, soc_port_t link, int is_prim, int val);
soc_error_t soc_fe3200_fabric_links_weight_get(int unit, soc_port_t link, int is_prim, int *val);
soc_error_t soc_fe3200_fabric_links_secondary_only_set(int unit, soc_port_t link, int val);
soc_error_t soc_fe3200_fabric_links_secondary_only_get(int unit, soc_port_t link, int *val);
soc_error_t soc_fe3200_fabric_link_repeater_enable_set(int unit, soc_port_t port, int enable, int empty_cell_size);
soc_error_t soc_fe3200_fabric_link_repeater_enable_get(int unit, soc_port_t port, int *enable, int *empty_cell_size);
soc_error_t soc_fe3200_fabric_links_pcp_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe3200_fabric_links_pcp_enable_get(int unit, soc_port_t port, int *enable);
soc_error_t soc_fe3200_fabric_links_pipe_map_set(int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t pipe_map);
soc_error_t soc_fe3200_fabric_links_pipe_map_get(int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t *pipe_map);
soc_error_t soc_fe3200_fabric_links_repeater_nof_remote_pipe_set(int unit, int port, uint32 nof_remote_pipe);
soc_error_t soc_fe3200_fabric_links_repeater_nof_remote_pipe_get(int unit, int port, uint32 *nof_remote_pipe);
soc_error_t soc_fe3200_fabric_links_cell_format_get(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t *val);

#define SOC_FE3200_ASYMMETRICAL_FE13_QUAD_0 (26)
#define SOC_FE3200_ASYMMETRICAL_FE13_QUAD_1 (35)
#define SOC_FE3200_WFQ_WEIGHT_MAX      (127)

#endif

