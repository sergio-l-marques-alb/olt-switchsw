/*
 * $Id: $
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
 *
 * File: qax_nif.h
 */

#ifndef __QAX_NIF_INCLUDED__

#define __QAX_NIF_INCLUDED__

#include <soc/portmod/portmod.h>


#define QAX_NIF_PHY_SIF_PORT_NBIL0 (64)
#define QAX_NIF_PHY_SIF_PORT_NBIL1 (116)

/* Add QSGMIIs offset to phy port
   Input:  Physical lane in the range of 1-72
   Output: Physical port in the range of 1-144*/
int soc_qax_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy);

/* Remove QSGMIIs offset from phy port
   Input:  Physical port in the range of 1-144
   Output: Physical lane in the range of 1-72*/
int soc_qax_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy);

int soc_qax_port_sch_config(int unit, soc_port_t port);

int soc_qax_port_open_fab_o_nif_path(int unit, int port);

int soc_qax_port_fabric_o_nif_bypass_interface_enable(int unit, int port, int enable);

int soc_qax_port_open_ilkn_path(int unit, int port);

int soc_qax_port_close_ilkn_path(int unit, int port);

int soc_qax_pm_instances_get(int unit, portmod_pm_instances_t **pm_instances, int *pms_instances_arr_len);

int soc_qax_pml_table_get(int unit, soc_dpp_pm_entry_t **soc_pml_table);

int soc_qax_wait_gtimer_trigger(int unit);

int soc_qax_port_ilkn_init(int unit);

int soc_qax_nif_ilkn_pbmp_get(int unit, soc_port_t port, uint32 ilkn_id, soc_pbmp_t* phys, soc_pbmp_t* src_pbmp);

int soc_qax_nif_qsgmii_pbmp_get(int unit, soc_port_t port, uint32 id, soc_pbmp_t *phy_pbmp);

int soc_qax_port_ilkn_bypass_interface_enable(int unit, int port, int enable);
int soc_qax_nif_sif_set(int unit, uint32 first_phy);

#endif /*__QAX_NIF_INCLUDED__*/
