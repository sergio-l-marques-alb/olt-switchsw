/*
 * $Id:  $
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
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef SOC_DPP_PETRA_DRV_H
#define SOC_DPP_PETRA_DRV_H

/*Defines*/
#define SOC_DPP_PETRA_MAX_LOCAL_PORTS (80)

int soc_petra_allocate_tm_port_in_range_and_recycle_channel(int unit, int core, int max_port, int *tm_port, uint32 *channel);
int soc_petra_free_tm_port_and_recycle_channel(int unit, int port);
int soc_petra_info_config_device_ports(int unit);
int soc_petra_is_olp(int unit, soc_port_t port, uint32* is_olp);
int soc_petra_is_oamp(int unit, soc_port_t port, uint32* is_oamp);
int soc_petra_validate_fabric_mode(int unit, SOC_TMC_FABRIC_CONNECT_MODE *fabric_connect_mode);
int soc_petra_prop_fap_device_mode_get(int unit, uint8 *pp_enable, SOC_TMC_MGMT_TDM_MODE *tdm_mode);
int soc_petra_fc_oob_mode_validate(int unit, int port);
int soc_petra_default_ofp_rates_set(int unit);

int soc_petra_init(int unit, int reset);
int soc_petra_deinit(int unit);
int soc_petra_attach(int unit);

SOC_SAND_RET
  soc_dpp_sand_write_cb_chipsim_pb(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 );

SOC_SAND_RET
  soc_dpp_sand_read_cb_chipsim_pb(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 );

#endif  /* SOC_DPP_PETRA_DRV_H */
