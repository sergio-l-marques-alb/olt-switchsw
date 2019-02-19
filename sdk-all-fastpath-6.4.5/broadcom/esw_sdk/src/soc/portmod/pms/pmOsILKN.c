/*
 *         
 * $Id:$
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
 *     
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>

        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM_OS_ILKN_SUPPORT

#include <soc/portmod/pmOsILKN.h>

#define OS_ILKN_TOTAL_LANES_PER_CORE                (24)
#define OS_ILKN_LANES_PER_CORE_TWO_ACTIVE_PORTS     (12)
#define OS_ILKN_BURST_256B_VAL                      (3)
#define OS_ILKN_BURST_32B_VAL                       (0)
#define OS_ILKN_METAFRAME_PERIOD_VAL                (64)
#define OS_ILKN_SLE_MAX_NOF_PIPES                   (15)
#define OS_ILKN_MAX_ILKN_AGGREGATED_PMS             (6)
#define OS_ILKN_MAX_NOF_INTERNAL_PORTS              (2)

#define OS_ILKN_CORE_ID_GET(port, pm_info, core_id) \
    core_id = (pm_info->pm_data.pmOsIlkn_db->ports[0] == port) ? 0 : \
              ((pm_info->pm_data.pmOsIlkn_db->ports[1] == port) ? 1 : -1); \
    if (core_id == -1) \
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("port %d is not attched"), port))


typedef struct pmOsIlkn_internal_s{
    soc_pbmp_t phys;
    int is_rx_retransmit;
    int is_tx_retransmit;
    int nof_aggregated_pms;
    pm_info_t *pms;
    int wm_high;
    int wm_low;
    int flags;
} pmOsIlkn_internal_t;

struct pmOsIlkn_s{
    soc_port_t          ports[OS_ILKN_MAX_NOF_INTERNAL_PORTS];
    pmOsIlkn_internal_t ilkn_data[OS_ILKN_MAX_NOF_INTERNAL_PORTS];
};

int pmOsILKN_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
    SOC_INIT_FUNC_DEFS;

    switch(interface){
    case SOC_PORT_IF_ILKN:
        *is_supported = TRUE;
        break;
    default:
        *is_supported = FALSE;
    }

    SOC_FUNC_RETURN;
}

/* Allocate a new pm_info and initialize it with information from pm_add_info */
int pmOsILKN_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_os_ilkn_create_info_internal_t *info = &pm_add_info->pm_specific_info.os_ilkn;
    pmOsIlkn_t pmOsIlkn_data = NULL;
    int i, j;
    SOC_INIT_FUNC_DEFS;

    pm_info->unit = unit;
    pm_info->type = portmodDispatchTypePmOsILKN;
    pm_info->wb_buffer_id = wb_buffer_index;

    pmOsIlkn_data = sal_alloc(sizeof(struct pmOsIlkn_s), "os_ilkn_specific_db");
    SOC_NULL_CHECK(pmOsIlkn_data);
    pm_info->pm_data.pmOsIlkn_db = pmOsIlkn_data;

    for (j = 0; j < OS_ILKN_MAX_NOF_INTERNAL_PORTS; ++j) {
        SOC_PBMP_ASSIGN(pmOsIlkn_data->ilkn_data[j].phys, pm_add_info->phys);

        pmOsIlkn_data->ilkn_data[j].nof_aggregated_pms = info->nof_aggregated_pms;

        pmOsIlkn_data->ilkn_data[j].pms = sal_alloc(sizeof(pm_info_t) * OS_ILKN_MAX_ILKN_AGGREGATED_PMS, "controlled_pms"); 
        SOC_NULL_CHECK(pmOsIlkn_data->ilkn_data[j].pms);
        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            pmOsIlkn_data->ilkn_data[j].pms[i] = info->pms[i];
        }
        pmOsIlkn_data->ilkn_data[j].wm_high = info->wm_high;
        pmOsIlkn_data->ilkn_data[j].wm_low = info->wm_low;

        pmOsIlkn_data->ilkn_data[j].flags = -1;
        pmOsIlkn_data->ilkn_data[j].is_rx_retransmit = -1;
        pmOsIlkn_data->ilkn_data[j].is_tx_retransmit = -1;

        pmOsIlkn_data->ports[j] = -1;
    }

exit:
    SOC_FUNC_RETURN;
}

/* Deallocate the pm_info struct- release resources */
int pmOsILKN_pm_destroy(int unit, pm_info_t pm_info)
{
    int i;
    SOC_INIT_FUNC_DEFS;
    
    if(pm_info->pm_data.pmOsIlkn_db != NULL){
        for (i = 0; i < OS_ILKN_MAX_NOF_INTERNAL_PORTS; ++i) {
            if (pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pms != NULL) {
                sal_free(pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pms);
                pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pms = NULL;
            }
        }
        sal_free(pm_info->pm_data.pmOsIlkn_db); 
        pm_info->pm_data.pmOsIlkn_db = NULL;
    }

    SOC_FUNC_RETURN;
}

int pmOsILKN_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

CONST STATIC soc_reg_t remap_lanes_rx_regs[] = {
    ILKN_SLE_RX_REMAP_LANE_4_0r, 
    ILKN_SLE_RX_REMAP_LANE_9_5r, 
    ILKN_SLE_RX_REMAP_LANE_14_10r, 
    ILKN_SLE_RX_REMAP_LANE_19_15r, 
    ILKN_SLE_RX_REMAP_LANE_24_20r
};

CONST STATIC soc_reg_t remap_lanes_tx_regs[] = {
    ILKN_SLE_TX_REMAP_LANE_4_0r, 
    ILKN_SLE_TX_REMAP_LANE_9_5r, 
    ILKN_SLE_TX_REMAP_LANE_14_10r, 
    ILKN_SLE_TX_REMAP_LANE_19_15r, 
    ILKN_SLE_TX_REMAP_LANE_24_20r
};

CONST STATIC soc_field_t remap_lanes_rx_fields[] = {
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_0f,
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_1f,
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_2f,
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_3f,
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_4f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_5f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_6f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_7f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_8f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_9f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_10f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_11f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_12f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_13f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_14f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_15f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_16f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_17f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_18f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_19f,
    SLE_RX_REMAP_LANE_24_20_REMAP_LANE_20f,
    SLE_RX_REMAP_LANE_24_20_REMAP_LANE_21f,
    SLE_RX_REMAP_LANE_24_20_REMAP_LANE_22f,
    SLE_RX_REMAP_LANE_24_20_REMAP_LANE_23f
};

CONST STATIC soc_field_t remap_lanes_tx_fields[] = {
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_0f,
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_1f,
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_2f,
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_3f,
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_4f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_5f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_6f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_7f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_8f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_9f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_10f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_11f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_12f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_13f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_14f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_15f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_16f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_17f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_18f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_19f,
    SLE_TX_REMAP_LANE_24_20_REMAP_LANE_20f,
    SLE_TX_REMAP_LANE_24_20_REMAP_LANE_21f,
    SLE_TX_REMAP_LANE_24_20_REMAP_LANE_22f,
    SLE_TX_REMAP_LANE_24_20_REMAP_LANE_23f
};

CONST STATIC soc_field_t enable_pipes_tx_fields[] = {
    SLE_TX_PIPE_ENABLE_PIPE_0f,
    SLE_TX_PIPE_ENABLE_PIPE_1f,
    SLE_TX_PIPE_ENABLE_PIPE_2f,
    SLE_TX_PIPE_ENABLE_PIPE_3f,
    SLE_TX_PIPE_ENABLE_PIPE_4f,
    SLE_TX_PIPE_ENABLE_PIPE_5f,
    SLE_TX_PIPE_ENABLE_PIPE_6f,
    SLE_TX_PIPE_ENABLE_PIPE_7f,
    SLE_TX_PIPE_ENABLE_PIPE_8f,
    SLE_TX_PIPE_ENABLE_PIPE_9f,
    SLE_TX_PIPE_ENABLE_PIPE_10f,
    SLE_TX_PIPE_ENABLE_PIPE_11f,
    SLE_TX_PIPE_ENABLE_PIPE_12f,
    SLE_TX_PIPE_ENABLE_PIPE_13f,
    SLE_TX_PIPE_ENABLE_PIPE_14f
};

CONST STATIC soc_field_t enable_pipes_rx_fields[] = {
    SLE_RX_PIPE_ENABLE_PIPE_0f,
    SLE_RX_PIPE_ENABLE_PIPE_1f,
    SLE_RX_PIPE_ENABLE_PIPE_2f,
    SLE_RX_PIPE_ENABLE_PIPE_3f,
    SLE_RX_PIPE_ENABLE_PIPE_4f,
    SLE_RX_PIPE_ENABLE_PIPE_5f,
    SLE_RX_PIPE_ENABLE_PIPE_6f,
    SLE_RX_PIPE_ENABLE_PIPE_7f,
    SLE_RX_PIPE_ENABLE_PIPE_8f,
    SLE_RX_PIPE_ENABLE_PIPE_9f,
    SLE_RX_PIPE_ENABLE_PIPE_10f,
    SLE_RX_PIPE_ENABLE_PIPE_11f,
    SLE_RX_PIPE_ENABLE_PIPE_12f,
    SLE_RX_PIPE_ENABLE_PIPE_13f,
    SLE_RX_PIPE_ENABLE_PIPE_14f
};


int pmOsILKN_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    pmOsIlkn_internal_t* ilkn_data;
    uint32 reg_val, bmp;
    soc_pbmp_t mirror_bmp;
    int port_i, i, reg_index, field_index, reverse_i = 0;
    int nof_lanes, num_pipes, start, index;
    SOC_INIT_FUNC_DEFS;

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[add_info->ilkn_core_id]);

    pm_info->pm_data.pmOsIlkn_db->ports[add_info->ilkn_core_id] = port;

    ilkn_data->is_rx_retransmit = add_info->rx_retransmit;
    ilkn_data->is_tx_retransmit = add_info->tx_retransmit;
    ilkn_data->flags = add_info->flags;

    SOC_PBMP_ASSIGN(ilkn_data->phys, add_info->phys);
    SOC_PBMP_COUNT(ilkn_data->phys, nof_lanes);
    ilkn_data->nof_aggregated_pms = (nof_lanes + 3) / 4; 
    
    /**** Configure OpenSilicon ILKN core ****/
    /* Lane Disable */
    /* if ILKN core 1,3,5, mirror the bitmap */
    SOC_PBMP_CLEAR(mirror_bmp);
    SOC_PBMP_ITER(ilkn_data->phys, port_i){
        index = (add_info->ilkn_core_id) ? OS_ILKN_TOTAL_LANES_PER_CORE - 1 - ((port_i - 1) % OS_ILKN_TOTAL_LANES_PER_CORE) : ((port_i - 1) % OS_ILKN_TOTAL_LANES_PER_CORE);
        SOC_PBMP_PORT_ADD(mirror_bmp, index);
    }
    bmp = SOC_PBMP_WORD_GET(mirror_bmp, 0);

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_LANEr, &reg_val, SLE_TX_LANE_DISABLEf, ~bmp);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_LANEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_LANEr, &reg_val, SLE_RX_LANE_DISABLEf, ~bmp);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_LANEr(unit, port, reg_val));

    /** Common regs **/
    /* SLE default values depend on LA-mode (on/off). just to make sure, we set these regs to 0, than configure fields. */
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, 0));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_USERf, ilkn_data->is_rx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_USERf, ilkn_data->is_tx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_BURSTr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_SHORTf, OS_ILKN_BURST_32B_VAL);
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_MINf, OS_ILKN_BURST_32B_VAL);
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_MAXf, OS_ILKN_BURST_256B_VAL);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_BURSTr(unit, port, reg_val)); 

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_BURSTr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_SHORTf, OS_ILKN_BURST_32B_VAL);
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_MINf, OS_ILKN_BURST_32B_VAL);
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_MAXf, OS_ILKN_BURST_256B_VAL);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_BURSTr(unit, port, reg_val)); 
    
    /* metaframe period */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_METAFRAMEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_METAFRAMEr, &reg_val, SLE_TX_METAFRAME_PERIODf, OS_ILKN_METAFRAME_PERIOD_VAL); /* minimum val. most sensitive to align errors */
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_METAFRAMEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_METAFRAMEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_METAFRAMEr, &reg_val, SLE_RX_METAFRAME_PERIODf, OS_ILKN_METAFRAME_PERIOD_VAL);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_METAFRAMEr(unit, port, reg_val));

    /* ilkn segment enable */
    /* val =  (ports 1,3,5 || ELK || two_active_cores) ? 0x3 : 0xf; */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_0f, 1);
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_1f, 1);
    if ((add_info->ilkn_core_id == 0) && (nof_lanes > OS_ILKN_LANES_PER_CORE_TWO_ACTIVE_PORTS) && (!PORTMOD_PORT_FLAGS_ELK_GET(ilkn_data->flags))) {
        soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_2f, 1); 
        soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_3f, 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_0f, 1);
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_1f, 1);
    if ((add_info->ilkn_core_id == 0) && (nof_lanes > OS_ILKN_LANES_PER_CORE_TWO_ACTIVE_PORTS) && (!PORTMOD_PORT_FLAGS_ELK_GET(ilkn_data->flags))) {
        soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_2f, 1);
        soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_3f, 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, reg_val));

    /* pipe enable */
    /* enable num_lanes pipes, or all pipes */
    num_pipes = (nof_lanes < OS_ILKN_SLE_MAX_NOF_PIPES) ? nof_lanes : OS_ILKN_SLE_MAX_NOF_PIPES;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_PIPE_ENABLEr(unit, port, &reg_val)); 
    for (i = 0; i < num_pipes; ++i) {
        soc_reg_field_set(unit, ILKN_SLE_TX_PIPE_ENABLEr, &reg_val, enable_pipes_tx_fields[i], 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_PIPE_ENABLEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_PIPE_ENABLEr(unit, port, &reg_val)); 
    for (i = 0; i < num_pipes; ++i) {
        soc_reg_field_set(unit, ILKN_SLE_RX_PIPE_ENABLEr, &reg_val, enable_pipes_rx_fields[i], 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_PIPE_ENABLEr(unit, port, reg_val));

    /* set watermarks, different between PMH/PML */
    /* hard coded: PMH cores should be 14/15 to H/L accordingly, PML core should be 9/10 */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_FIFO_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_FIFO_CFGr, &reg_val, SLE_TX_FIFO_CFG_WM_HIGHf, ilkn_data->wm_high);
    soc_reg_field_set(unit, ILKN_SLE_TX_FIFO_CFGr, &reg_val, SLE_TX_FIFO_CFG_WM_LOWf, ilkn_data->wm_low);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_FIFO_CFGr(unit, port, reg_val));
    
    /* set_ilkn_cfg_inband, set_ilkn_cfg_outband, set_ilkn_cfg_mult_ext*/
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_INBANDf, 1);
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_OUTBANDf, 1);
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_MULT_EXTf, ilkn_data->is_tx_retransmit ? 0 : 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_INBANDf, 1);
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_OUTBANDf, 1);
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_MULT_EXTf, ilkn_data->is_rx_retransmit ? 0 : 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    /* set_ilkn_cal_inband_last */    
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CAL_INBANDr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CAL_INBANDr, &reg_val, SLE_TX_CAL_INBAND_LASTf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CAL_INBANDr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CAL_INBANDr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CAL_INBANDr, &reg_val, SLE_RX_CAL_INBAND_LASTf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CAL_INBANDr(unit, port, reg_val));
    
    /* set_ilkn_cal_outband_last */ /*configure to num of channels - 1*/
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CAL_OUTBANDr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CAL_OUTBANDr, &reg_val, SLE_TX_CAL_OUTBAND_LASTf, nof_lanes - 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CAL_OUTBANDr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CAL_OUTBANDr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CAL_OUTBANDr, &reg_val, SLE_RX_CAL_OUTBAND_LASTf, nof_lanes - 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CAL_OUTBANDr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_OUTBANDf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_OUTBANDf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    /** Lane remap registers (port1, port3 and port5 only) **/
    /* Open-Silicon lanes are in reverse order, so if needed forward ordeed lanes, we need to remap ports 1,3,5 */
    if (add_info->ilkn_core_id) {
        SOC_PBMP_ITER(ilkn_data->phys, port_i){
            reverse_i = (!reverse_i) ? (((port_i / OS_ILKN_TOTAL_LANES_PER_CORE) + 1) * OS_ILKN_TOTAL_LANES_PER_CORE) : reverse_i ; 
            while (!SOC_PBMP_MEMBER(ilkn_data->phys, reverse_i)) {
                --reverse_i;
            }
            field_index = (port_i - 1) % OS_ILKN_TOTAL_LANES_PER_CORE;
            reg_index = field_index / 5;
            _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[reg_index], port, 0, &reg_val));
            soc_reg_field_set(unit, remap_lanes_rx_regs[reg_index], &reg_val, remap_lanes_rx_fields[field_index], reverse_i - 1);
            _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_rx_regs[reg_index], port, 0, reg_val));

            _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_tx_regs[reg_index], port, 0, &reg_val));
            soc_reg_field_set(unit, remap_lanes_tx_regs[reg_index], &reg_val, remap_lanes_tx_fields[field_index], reverse_i - 1);
            _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_tx_regs[reg_index], port, 0, reg_val));
            --reverse_i;
        }
    }

    /* When 1, data pack block is enabled, by default this block is disabled. In the Rx, this bit has no function */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_DATA_PCKf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    /* call PMs */
    start = (add_info->ilkn_core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_pm_bypass_set(unit, ilkn_data->pms[i], 1)); 
        
        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_attach(unit, port, ilkn_data->pms[i], add_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable, i, core_id, start;
    pmOsIlkn_internal_t* ilkn_data;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    _SOC_IF_ERR_EXIT(pmOsILKN_port_enable_get(unit, port, pm_info, 0, &enable));
    if (enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }

    /* call PMs */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_pm_bypass_set(unit, ilkn_data->pms[i], 0));

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_detach(unit, port, ilkn_data->pms[i]));
    }

    /* Disable - clean db */
    ilkn_data->nof_aggregated_pms = OS_ILKN_MAX_ILKN_AGGREGATED_PMS;
    ilkn_data->flags = -1;
    ilkn_data->is_rx_retransmit = -1;
    ilkn_data->is_tx_retransmit = -1;
    SOC_PBMP_CLEAR(ilkn_data->phys);

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /*order is according to arad (serdes vs mac)*/
    if (PORTMOD_PORT_ENABLE_MAC_GET(flags) || flags == 0) {
       
    }

    if (PORTMOD_PORT_ENABLE_PHY_GET(flags) || flags == 0) {
        /* call PMs */
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_enable_set(unit, port, ilkn_data->pms[i], flags, enable));
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    int i, core_id, phy_en = 0, mac_en = 0, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    if (PORTMOD_PORT_ENABLE_MAC_GET(flags) || flags == 0) {
    
    }

    if (PORTMOD_PORT_ENABLE_PHY_GET(flags) || flags == 0) {
        /* call PMs */
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_enable_get(unit, port, ilkn_data->pms[i], flags, &phy_en));
        }
    }
        
    *enable = (phy_en || mac_en); 
    
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_interface_config_set(unit, port, ilkn_data->pms[i], config));
    }
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config)
{
    int core_id, index;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    index = (core_id) ? 3 : 0;

    _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[index]->type]->f_portmod_port_interface_config_get(unit, port, ilkn_data->pms[index], config));
    
    config->interface = SOC_PORT_IF_ILKN;

exit:
    SOC_FUNC_RETURN; 
}


STATIC int _pmOsILKN_port_loopback_set(int unit, int port, int enable)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_DATA_TX_TO_RX_LBf, enable);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_DATA_TX_TO_RX_LBf, enable);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN; 
}

STATIC int _pmOsILKN_port_loopback_get(int unit, int port, int *enable)
{
    uint32 reg_val, is_tx, is_rx;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    is_tx = soc_reg_field_get(unit, ILKN_SLE_TX_CFGr, reg_val, SLE_TX_CFG_DATA_TX_TO_RX_LBf);

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    is_rx = soc_reg_field_get(unit, ILKN_SLE_RX_CFGr, reg_val, SLE_RX_CFG_DATA_TX_TO_RX_LBf);

    *enable = (is_tx && is_rx) ? 1 : 0;

exit:
    SOC_FUNC_RETURN; 
}


int pmOsILKN_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int i, core_id, rv, start;
    portmod_dispatch_type_t type;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    switch (loopback_type) {
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacCore:
    case portmodLoopbackMacPCS:
    case portmodLoopbackMacAsyncFifo:
        _SOC_IF_ERR_EXIT(_pmOsILKN_port_loopback_set(unit, port, enable));
        break;
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD:
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {
            type = ilkn_data->pms[i]->type;
            if ((type != portmodDispatchTypePmOsILKN) &&
                (NULL != __portmod__dispatch__[type]->f_portmod_port_loopback_set)){
                rv = __portmod__dispatch__[type]->f_portmod_port_loopback_set(unit, port, ilkn_data->pms[i], loopback_type, enable);
                _SOC_IF_ERR_EXIT(rv);
            }
        }
        break;
    case portmodLoopbackPhyRloopPCS:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Rloop PCS is not supported.")));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }

exit:
    SOC_FUNC_RETURN; 
    
}

int pmOsILKN_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    int i, core_id, rv, start;
    portmod_dispatch_type_t type;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    switch (loopback_type) {
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacCore:
    case portmodLoopbackMacPCS:
    case portmodLoopbackMacAsyncFifo:
        _SOC_IF_ERR_EXIT(_pmOsILKN_port_loopback_get(unit, port, enable));
        break;
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD:
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {
            type = ilkn_data->pms[i]->type;
            if ((type != portmodDispatchTypePmOsILKN) &&
                (NULL != __portmod__dispatch__[type]->f_portmod_port_loopback_get)){
                rv = __portmod__dispatch__[type]->f_portmod_port_loopback_get(unit, port, ilkn_data->pms[i], loopback_type, enable);
                _SOC_IF_ERR_EXIT(rv);
            }
        }
        break;
    case portmodLoopbackPhyRloopPCS:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Rloop PCS is not supported.")));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, 
                                  phymod_core_access_t* core_access_arr, int* nof_cores, int* is_most_ext)
{
    int i, core_id, arr_inx, rv, start;
    pmOsIlkn_internal_t *ilkn_data ;
    int is_most_ext_i;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    if (is_most_ext) {
        *is_most_ext = 0;
    }

    *nof_cores = arr_inx = 0;
    /* call PMs below - aggregate here */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        rv = __portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_core_access_get(unit, port, 
                                            ilkn_data->pms[i], phyn, max_cores - arr_inx, core_access_arr + arr_inx, nof_cores, &is_most_ext_i);
        _SOC_IF_ERR_EXIT(rv);

        arr_inx += *nof_cores;
        if (arr_inx >= max_cores) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("core_access_arr size %d isn't big enough"), max_cores));
        }

        if (is_most_ext) {
            *is_most_ext = (*is_most_ext | is_most_ext_i ? 1 : 0);
        }
    }

exit:    
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, 
                                      int max_phys, phymod_phy_access_t* access, int* nof_phys, int* is_most_ext)
{
    int i, core_id, arr_inx, rv, start;
    pmOsIlkn_internal_t *ilkn_data ;
    int is_most_ext_i;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    if (is_most_ext) {
        *is_most_ext = 0;
    }

    *nof_phys = arr_inx = 0;
    /* call PMs below - aggregate here */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        if (arr_inx >= max_phys) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("access size %d isn't big enough"), max_phys));
        }
        rv = __portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_phy_lane_access_get(unit, port, 
                                                                                 ilkn_data->pms[i], params, max_phys - arr_inx, access + arr_inx, nof_phys, &is_most_ext_i);
        _SOC_IF_ERR_EXIT(rv);

        arr_inx += *nof_phys;

        if (is_most_ext) {
            *is_most_ext = (*is_most_ext | is_most_ext_i ? 1 : 0);
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    uint32 reg_val;
    int is_aligned, is_err;    
    SOC_INIT_FUNC_DEFS;
    
    /* link up indecation - in ILKN require interface will be in alignment */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_ALIGNr(unit, port, &reg_val));
    is_aligned = soc_reg_field_get(unit, ILKN_SLE_RX_ALIGNr, reg_val, SLE_RX_ALIGN_GOODf);
    is_err = soc_reg_field_get(unit, ILKN_SLE_RX_ALIGNr, reg_val, SLE_RX_ALIGN_ERRf);

    *link = (is_aligned && !is_err);

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_nof_lanes_set(int unit, int port, pm_info_t pm_info, int nof_lanes)
{
    SOC_INIT_FUNC_DEFS;
    
    /*TBD*/

    SOC_FUNC_RETURN;
}

int pmOsILKN_port_nof_lanes_get(int unit, int port, pm_info_t pm_info, int* nof_lanes)
{
    int core_id;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* pbmp count */
    SOC_PBMP_COUNT(ilkn_data->phys, *nof_lanes);

exit:        
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_firmware_mode_set(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t fw_mode)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_firmware_mode_set(unit, port, ilkn_data->pms[i], fw_mode));
    }
        
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_firmware_mode_get(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t* fw_mode)
{
    int core_id, index;
    pmOsIlkn_internal_t *ilkn_data ;        
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    index = (core_id) ? 3 : 0;

    /* assume all PMs are equal */
    _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[index]->type]->f_portmod_port_firmware_mode_get(unit, port, ilkn_data->pms[index], fw_mode));

exit:
    SOC_FUNC_RETURN;    
}


int pmOsILKN_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_update(unit, port, ilkn_data->pms[i], update_control));
    }
        
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int flags, int reg_addr, uint32* value)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_phy_reg_read(unit, port, ilkn_data->pms[i], flags, reg_addr, value));
    }
    
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int flags, int reg_addr, uint32 value)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_phy_reg_write(unit, port, ilkn_data->pms[i], flags, reg_addr, value));
    }

exit:
    SOC_FUNC_RETURN;    
}


int pmOsILKN_port_reset_set(int unit, int port, pm_info_t pm_info, int mode, int opcode, int value)
{
    SOC_INIT_FUNC_DEFS;
    
    /*TBD*/

    SOC_FUNC_RETURN;
}


int pmOsILKN_port_reset_get(int unit, int port, pm_info_t pm_info, int mode, int opcode, int* value)
{        
    SOC_INIT_FUNC_DEFS;

    /* TBD */

    SOC_FUNC_RETURN;     
}


int pmOsILKN_port_ref_clk_get(int unit, int port, pm_info_t pm_info, int* ref_clk)
{
    int core_id, index;    
    pmOsIlkn_internal_t *ilkn_data ;        
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    index = (core_id) ? 3 : 0;

    /* assume all PMs are equal */
    _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[index]->type]->f_portmod_port_ref_clk_get(unit, port, ilkn_data->pms[index], ref_clk));

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{   
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    start = (core_id) ? 3 : 0;
    for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_soft_reset_toggle(unit, port, ilkn_data->pms[i], idx));
    }
    
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_prbs_config_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    
    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_config_set(unit, port, ilkn_data->pms[i], mode, flags, config));
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }
    
exit:
    SOC_FUNC_RETURN;   
}


int pmOsILKN_port_prbs_config_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{            
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    
    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_config_get(unit, port, ilkn_data->pms[i], mode, flags, config));
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }

exit:
    SOC_FUNC_RETURN;    
}


int pmOsILKN_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int enable)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_enable_set(unit, port, ilkn_data->pms[i], mode, flags, enable));
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int* enable)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    
    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_enable_get(unit, port, ilkn_data->pms[i], mode, flags, enable));
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }

exit:
    SOC_FUNC_RETURN;    
}


int pmOsILKN_port_prbs_status_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    int i, core_id, start;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
       
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    
    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        start = (core_id) ? 3 : 0;
        for (i = start; i < start + ilkn_data->nof_aggregated_pms; ++i) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_status_get(unit, port, ilkn_data->pms[i], mode, flags, status));
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_mode_set (int unit, soc_port_t port, 
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("mode set isn't supported")));

exit:
    SOC_FUNC_RETURN;  
}

int pmOsILKN_port_mode_get (int unit, soc_port_t port, 
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    int nof_lanes, core_id;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    if (core_id == -1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("port %d is not attched"), port));
    }

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    SOC_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    mode->lanes = nof_lanes;
    mode->cur_mode = portmodPortModeQuad;
    
exit:
    SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM_OS_ILKN_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

