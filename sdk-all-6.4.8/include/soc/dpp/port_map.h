/*
 * $Id:  $
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
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef SOC_DPP_PORT_MAP_H
#define SOC_DPP_PORT_MAP_H

#define _SOC_DPP_PORT_INVALID 0xffffffff
#define SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit) ((SOC_IS_PETRAB(unit) ? SOC_DPP_PETRA_MAX_LOCAL_PORTS : SOC_MAX_NUM_PORTS))

typedef struct {
  uint32 channel;
  SOC_TMC_INTERFACE_ID nif_id;
  int tm_port;
  int pp_port;
  int if_rate_mbps;
} soc_dpp_port_map_t;

typedef struct {
   uint8 in_use;
} soc_dpp_port_t;

typedef struct {
    int pp_port[SOC_MAX_NUM_PORTS];
    soc_dpp_port_t pp_port_use[SOC_DPP_DEFS_MAX(NOF_PP_PORTS)];
} soc_dpp_pp_port_map;

extern int _dflt_tm_pp_port_map[SOC_MAX_NUM_DEVICES];
extern soc_dpp_port_map_t _port_map[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
extern soc_dpp_port_t *soc_dpp_pp_ports[SOC_MAX_NUM_DEVICES];
extern soc_dpp_port_t *soc_dpp_tm_ports[SOC_MAX_NUM_DEVICES];

#define SOC_DPP_TM_PORT_IN_USE(unit, port)  (soc_dpp_tm_ports[unit][port].in_use == TRUE)
#define SOC_DPP_TM_PORT_RESERVE(unit, port) (soc_dpp_tm_ports[unit][port].in_use = TRUE)
#define SOC_DPP_TM_PORT_FREE(unit, port)    (soc_dpp_tm_ports[unit][port].in_use = FALSE)

#define SOC_DPP_PP_PORT_IN_USE(unit, port)  (soc_dpp_pp_ports[unit][port].in_use == TRUE)
#ifdef BCM_ARAD_SUPPORT
#define SOC_DPP_PP_PORT_RESERVE(unit, port)            \
{                                                      \
    if (SOC_IS_ARAD(unit)) {                           \
        rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.pp_port_use.in_use.set(unit, port, TRUE); \
        SOCDNX_IF_ERR_EXIT(rv);                        \
    }                                                  \
    (soc_dpp_pp_ports[unit][port].in_use = TRUE);      \
}
#define SOC_DPP_PP_PORT_FREE(unit, port)               \
{                                                      \
    if (SOC_IS_ARAD(unit) && !SOC_IS_DETACHING(unit)) {                           \
        rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.pp_port_use.in_use.set(unit, port, FALSE);\
        SOCDNX_IF_ERR_EXIT(rv);                        \
    }                                                  \
    (soc_dpp_pp_ports[unit][port].in_use = FALSE);     \
}
#else
#define SOC_DPP_PP_PORT_RESERVE(unit, port) (soc_dpp_pp_ports[unit][port].in_use = TRUE)
#define SOC_DPP_PP_PORT_FREE(unit, port)    (soc_dpp_pp_ports[unit][port].in_use = FALSE)
#endif

int _soc_dpp_port_map_deinit(int unit);
int _soc_dpp_port_map_init(int unit);
int _soc_dpp_wb_pp_port_restore(int unit);

int petra_soc_dpp_local_to_nif_id_set(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID nif_id, uint32 channel); 
int petra_soc_dpp_local_port_partial(int unit, soc_port_t port);
int petra_soc_dpp_local_to_tm_port_set_internal(int unit, soc_port_t port, int tm_port); 
int petra_soc_dpp_local_to_pp_port_set_internal(int unit, soc_port_t port, int pp_port);
int petra_soc_dpp_local_port_valid(int unit, soc_port_t port); 
int petra_soc_dpp_local_to_nif_id_get(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID *nif_id, uint32 *channel);
int petra_soc_dpp_local_to_pp_port_get(int unit, soc_port_t port, uint32* pp_port, int* core);
int petra_soc_dpp_local_to_tm_port_get(int unit, soc_port_t port, uint32* tm_port, int* core);
int petra_soc_dpp_pp_to_local_port_get(int unit, int core, uint32 pp_port, soc_port_t *port);
int petra_soc_dpp_tm_to_local_port_get(int unit, int core, uint32 tm_port, soc_port_t *port);
int petra_soc_dpp_local_to_tm_port_set(int unit,soc_port_t port, int core, uint32 tm_port);
int petra_soc_dpp_local_to_pp_port_set(int unit, soc_port_t port, uint32 pp_port);

#endif  /* SOC_DPP_PORT_MAP_H */
