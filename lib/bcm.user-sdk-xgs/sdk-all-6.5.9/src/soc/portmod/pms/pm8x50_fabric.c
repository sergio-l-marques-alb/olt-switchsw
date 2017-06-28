/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/cmic.h>
#include <sal/core/alloc.h>
#include <soc/wb_engine.h>
#include <soc/drv.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/pm8x50_fabric.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#define PM8X50_FABRIC_LEGACY_FIXME_ASSERT  do {\
                                    LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META("Need to validate the function works"))); \
                                    assert(0);\
                                  } while(0)


#define PM8X50_FABRIC_LANES_PER_CORE (8)
#define PM8X50_FABRIC_MAX_PORTS_PER_PM (8)
#define PM8X50_FABRIC_LANES_PER_FMAC (4)

/* WB defines */
#define PM8X50_FABRIC_IS_BYPASSED_SET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_bypassed], &is_bypass)
#define PM8X50_FABRIC_IS_BYPASSED_GET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_bypassed], is_bypass)

#define PM8X50_FABRIC_LANE2PORT_SET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], &port, lane)
#define PM8X50_FABRIC_LANE2PORT_GET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], port, lane)


#ifdef PORTMOD_PM8X50_FABRIC_SUPPORT

/*MAC PRBS Defines*/
#define PM8X50_FABRIC_MAC_PRBS_LOCK_SHIFT  (31)
#define PM8X50_FABRIC_MAC_PRBS_LOCK_MASK   (0x1)
#define PM8X50_FABRIC_MAC_PRBS_CTR_SHIFT   (0)
#define PM8X50_FABRIC_MAC_PRBS_CTR_MASK    (0x7FFFFFFF)

#define PM8X50_FABRIC_FMAC_TX (0)
#define PM8X50_FABRIC_FMAC_RX (1)

#define PM8X50_FABRIC_FMAC_0_LANES_MASK (0xF)
#define PM8X50_FABRIC_FMAC_1_LANES_MASK (0xF0)

#define PM8X50_FABRIC_LANE_INDEX_IN_PM_GET(fmac_index, lane_index_in_fmac) \
    (fmac_index*PM8X50_FABRIC_LANES_PER_FMAC + lane_index_in_fmac)

#define PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(lane_index_in_pm) \
    (lane_index_in_pm % PM8X50_FABRIC_LANES_PER_FMAC)

#define PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(lane_index_in_pm, bitmap) \
    (bitmap & (1 << lane_index_in_pm))

#define PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding) \
        (encoding == PORTMOD_PCS_64B66B_RS_FEC || encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC \
        || encoding == PORTMOD_PCS_64B66B_15T_RS_FEC || encoding == PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC)

#define PM8X50_FABRIC_SPEED_IS_PAM4(speed) \
    ((speed > 28125)? 1 : 0)

#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N1_PAM4 (10)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N2_PAM4 (21)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N3_PAM4 (100)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K1_PAM4 (1)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K2_PAM4 (2)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K3_PAM4 (2)

#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N1_NRZ (1)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N2_NRZ (7)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_N3_NRZ (8)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K1_NRZ (1)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K2_NRZ (7)
#define PM8X50_FABRIC_FEC_SYNC_MACHINE_K3_NRZ (1)

#define PM8X50_FABRIC_FEC_SYNC_MACHINE_IS_TRIPLE_BIT_SLIP(encoding, n1) \
    (encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC && \
            ((n1 % 7 == 0) || (n1 % 7 == 5) || (n1 % 7 == 6)))

/*wb vars in pm8x50_fabric buffer. 
  new value must appended to the end.
  Remove values is disallowed*/
typedef enum pm8x50_fabric_wb_vars {
    wb_is_probed = 0,
    wb_is_initialized = 1,
    wb_ports = 2,
    wb_polarity_rx = 3,
    wb_polarity_tx = 4,
    wb_is_bypassed = 5,
    wb_lane2port_map = 6
} pm8x50_fabric_wb_vars_t;

typedef struct pm8x50_fabric_internal_s{
    pm_info_t *pms;
} pm8x50_fabric_internal_t;

struct pm8x50_fabric_s{
    portmod_pbmp_t phys; 
    phymod_ref_clk_t ref_clk;
    phymod_access_t access;
    phymod_dispatch_type_t core_type;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f fw_loader;
    uint32 fmac_schan_id[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC];
    uint32 fsrd_schan_id;
    int first_phy_offset;
    int core_index;
};

typedef struct pm8x50_fabric_fec_sync_machine_config_s{
    int n1;
    int n2;
    int n3;
    int k1;
    int k2;
    int k3;
    int slip;
} pm8x50_fabric_fec_sync_machine_config_t;

STATIC
int pm8x50_fabric_phy_reset_set(int unit, int port, pm_info_t pm_info, phymod_reset_direction_t reset_direction)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_phy_reset_t phy_reset;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    SOCDNX_IF_ERR_EXIT(phymod_phy_reset_get(&phy_access, &phy_reset));
    phy_reset.tx = reset_direction;
    phy_reset.rx = reset_direction;
    SOCDNX_IF_ERR_EXIT(phymod_phy_reset_set(&phy_access, &phy_reset));


exit:
    SOCDNX_FUNC_RETURN;
}


/* 1. get bitmap of lanes used for the port - the bitmap is of size 8. First 4 bits: for FMAC0, second 4 bits: for FMAC1.
 * 2. get phy_index of first phy in each FMAC used for the port - [0-7] */
STATIC
int pm8x50_fabric_port_phy_index_get(int unit, int port, pm_info_t pm_info, uint32 *phy_index, uint32 *bitmap){
    int i, rv = 0, tmp_port = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *bitmap = 0;
    *phy_index = -1;

    /*set bitmap - all the lanes used for the port, first 4 bits: for FMAC0, second 4 bits: for FMAC1.*/
    for (i = 0; i < PM8X50_FABRIC_MAX_PORTS_PER_PM; i++) {
        rv = PM8X50_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if(tmp_port == port) {
            *phy_index = (*phy_index == -1 ? i : *phy_index);
            SHR_BITSET(bitmap, i);
        }
    }

    if(*phy_index == -1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("port was not found in internal DB %d"), port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/* Get FMAC blk id/s and their amount.
 * fmac_blk_ids is an array of size 2.
 * If only one FMAC (FMAC0/1) is used for the port- its blk_id will be set in fmac_blk_ids[0].
 * If both FMACs are used for the port- their blk_ids will be set in fmac_blk_ids[0] and fmac_blk_ids[1] accordingly.
 * nof_fmac_blks holds count of FMACs used for the port- 1/2. */
STATIC
int pm8x50_fabric_fmac_blk_ids_get(int unit, int port, pm_info_t pm_info, uint32* fmac_blk_ids, int* nof_fmac_blks, int* first_fmac_index) {
    uint32 phy_index, bitmap;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    *first_fmac_index = -1;
    
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    *nof_fmac_blks = 0;
    if (bitmap & PM8X50_FABRIC_FMAC_0_LANES_MASK) { /*at least one lane in 1st FMAC is used for the port*/
        fmac_blk_ids[*nof_fmac_blks] = fabric_data->fmac_schan_id[0] | SOC_REG_ADDR_SCHAN_ID_MASK;
        *first_fmac_index = 0;
        *nof_fmac_blks += 1;
    }
    if (bitmap & PM8X50_FABRIC_FMAC_1_LANES_MASK) { /*at least one lane in 2nd FMAC is used for the port*/
        fmac_blk_ids[*nof_fmac_blks] = fabric_data->fmac_schan_id[1] | SOC_REG_ADDR_SCHAN_ID_MASK;
        if (*first_fmac_index == -1) {
            *first_fmac_index = 1;
        }
        *nof_fmac_blks += 1;
    }

    if(*nof_fmac_blks == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("invalid number of FMACs for port %d"), port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return SOC_E_NONE; 
}


int pm8x50_fabric_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    *enable = TRUE; /* Full Duplex */
    return SOC_E_NONE; 
}


int pm8x50_fabric_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported){
    *is_supported = (interface == SOC_PORT_IF_SFI);
    return SOC_E_NONE;
}

STATIC
int pm8x50_fabric_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    buffer_id = wb_buffer_index;
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm8x50_fabric", NULL, NULL, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_probed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_probed] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_initialized] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(int), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_ports] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "polarity_rx", wb_buffer_index, sizeof(uint32), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_polarity_rx] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "polarity_tx", wb_buffer_index, sizeof(uint32), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_polarity_tx] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_bypassed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_bypassed] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2port_map", wb_buffer_index, sizeof(int), NULL, PM8X50_FABRIC_MAX_PORTS_PER_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_lane2port_map] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    SOCDNX_INIT_FUNC_DEFS;
    PM8X50_FABRIC_LEGACY_FIXME_ASSERT;

    SOC_PBMP_ASSIGN(*phys, fabric_data->phys);
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{   
    const portmod_pm8x50_fabric_create_info_internal_t *info = &pm_add_info->pm_specific_info.pm8x50_fabric;
    pm8x50_fabric_t fabric_data;
    int rv, i, invalid_port = -1;
    int is_phymod_probed=0;
    uint32 rx_polarity, tx_polarity, is_bypassed, is_core_probed;
    SOCDNX_INIT_FUNC_DEFS;

    fabric_data = NULL;
    fabric_data = sal_alloc(sizeof(*(pm_info->pm_data.pm8x50_fabric)), "pm8x50_fabric specific_db");    
    SOCDNX_NULL_CHECK(fabric_data);
    sal_memset(fabric_data, 0, sizeof(*(pm_info->pm_data.pm8x50_fabric)));

    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.pm8x50_fabric = fabric_data;

    pm_info->type = portmodDispatchTypePm8x50_fabric;

    
    fabric_data->core_type = phymodDispatchTypeInvalid; 


    if ((info->ref_clk != phymodRefClk156Mhz) && (info->ref_clk != phymodRefClk125Mhz))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("illegal ref clock")));
    }

    SOCDNX_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(pm_add_info->pm_specific_info.pm8x50_fabric.fw_load_method));

    sal_memcpy(&(fabric_data->access), &info->access, sizeof(phymod_access_t));
    fabric_data->first_phy_offset = pm_add_info->pm_specific_info.pm8x50_fabric.first_phy_offset;
    fabric_data->core_index = pm_add_info->pm_specific_info.pm8x50_fabric.core_index;
    for (i = 0; i < PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC; ++i) {
        fabric_data->fmac_schan_id[i] = pm_add_info->pm_specific_info.pm8x50_fabric.fmac_schan_id[i];
    }
    fabric_data->fsrd_schan_id = pm_add_info->pm_specific_info.pm8x50_fabric.fsrd_schan_id;
    fabric_data->ref_clk = info->ref_clk;
    fabric_data->fw_load_method = pm_add_info->pm_specific_info.pm8x50_fabric.fw_load_method;
    fabric_data->fw_loader = pm_add_info->pm_specific_info.pm8x50_fabric.external_fw_loader;
    PORTMOD_PBMP_ASSIGN(fabric_data->phys, pm_add_info->phys);

    /*init wb buffer*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_wb_buffer_init(unit, wb_buffer_index,  pm_info));
    if (!SOC_WARM_BOOT(unit))
    {
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], -1);
        SOCDNX_IF_ERR_EXIT(rv);
        is_bypassed = 0;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_bypassed], &is_bypassed);
        SOCDNX_IF_ERR_EXIT(rv);
        for (i = 0; i < PM8X50_FABRIC_LANES_PER_CORE; i++)
        {
            rx_polarity = (info->polarity.rx_polarity >> i) & 0x1;
            tx_polarity = (info->polarity.tx_polarity >> i) & 0x1;

            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], &invalid_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } 
    else 
    {
        rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);

        if (is_core_probed)
        {
            /* Probe */
            SOCDNX_IF_ERR_EXIT(phymod_core_probe(&info->access, &(fabric_data->core_type), &is_phymod_probed));
            if (!is_phymod_probed)
            {
                SOCDNX_EXIT_WITH_ERR(PHYMOD_E_INIT, (_BSL_SOCDNX_MSG("failed to probe core")));
            }
        }
    }
exit:
    if (SOC_FUNC_ERROR)
    {
        pm8x50_fabric_pm_destroy(unit, pm_info);
    }
    SOCDNX_FUNC_RETURN;
}



int pm8x50_fabric_pm_destroy(int unit, pm_info_t pm_info)
{   
    if(pm_info->pm_data.pm8x50_fabric != NULL){
        sal_free(pm_info->pm_data.pm8x50_fabric);
        pm_info->pm_data.pm8x50_fabric = NULL;
    }
    
    return SOC_E_NONE;
}

int pm8x50_fabric_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    core_info->ref_clk = pm_info->pm_data.pm8x50_fabric->ref_clk;
    return SOC_E_NONE;
}


STATIC
int pm8x50_fabric_core_init_phase1(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    phymod_core_access_t core_access;
    phymod_core_status_t status;
    phymod_core_init_config_t core_config;
    int rv, fsrd_schan_id;
    portmod_pbmp_t pm_port_phys;
    uint32 is_core_probed;
    soc_reg_above_64_val_t reg_above64_val;
    const portmod_port_interface_config_t* config = &(add_info->interface_config);
    int is_probed;
    int cores_num;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    PORTMOD_PBMP_ASSIGN(pm_port_phys, fabric_data->phys);
    PORTMOD_PBMP_AND(pm_port_phys, add_info->phys);

    fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    if(!PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        /* Get Phy Out Of Reset*/
        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_H_PWRDNf, 0);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_H_PWRDNf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_IDDQf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        sal_usleep(10);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_POR_H_RSTBf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        /* Probe */
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));

         SOCDNX_IF_ERR_EXIT(phymod_core_probe(&core_access.access, &core_access.type, &is_probed));
         if (!is_probed)
         {
             SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("failed to probe the core")));
         }
         fabric_data->core_type = core_access.type;

        /* Mark DB as probed */
        is_core_probed = 1;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        /* Mark DB as fully probed */
        is_core_probed = 2;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    /*core status*/
    rv = phymod_core_status_t_init(&status);
    SOCDNX_IF_ERR_EXIT(rv);
    status.pmd_active = FALSE;

    /*core config*/
    phymod_core_init_config_t_init(&core_config);

    /*lane mapping passed to phymod: blackhawk serdes (represented by index in array) -> {lane rx, lane tx} */
    if (add_info->lane_map.num_of_lanes != PM8X50_FABRIC_LANES_PER_CORE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("add_info->lane_map.num_of_lanes != PM8X50_FABRIC_LANES_PER_CORE")));
    }
    for(i = 0; i < add_info->lane_map.num_of_lanes; ++i)
    {
        core_config.lane_map.lane_map_rx[i] = add_info->lane_map.lane_map_rx[i];
        core_config.lane_map.lane_map_tx[i] = add_info->lane_map.lane_map_tx[i];
    }
    core_config.lane_map.num_of_lanes = add_info->lane_map.num_of_lanes;

    core_config.firmware_load_method = fabric_data->fw_load_method;
    if(fabric_data->fw_load_method == phymodFirmwareLoadMethodExternal){
        /* Fast firmware load */
        core_config.firmware_loader = fabric_data->fw_loader;
    }
    else{
        core_config.firmware_loader = NULL;
    }

    core_config.interface.data_rate = config->speed;
    core_config.interface.interface_modes = config->interface_modes;
    core_config.interface.interface_type = phymodInterfaceBypass;
    core_config.interface.ref_clock = fabric_data->ref_clk;

    if(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_SET(&core_config);
    }
    
    if(PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(&core_config);
    }

    if(PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_config);
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_core_access_get(unit, port, pm_info, 0, 1, &core_access, &cores_num, NULL));
    PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(core_access.device_op_mode);
    SOCDNX_IF_ERR_EXIT(phymod_core_init(&core_access, &core_config, &status));
    PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_CLR(core_access.device_op_mode);

exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC
int pm8x50_fabric_core_init_phase2(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
    int fsrd_schan_id, rv;
    uint32 is_core_initialized;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64_val;
    SOCDNX_INIT_FUNC_DEFS;

    fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_PLL_0_CTRLr(unit, fsrd_schan_id, &reg_val));
    soc_reg_field_set(unit, FSRD_SRD_PLL_0_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_0_DP_H_RSTBf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_PLL_0_CTRLr(unit, fsrd_schan_id, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_PLL_1_CTRLr(unit, fsrd_schan_id, &reg_val));
    soc_reg_field_set(unit, FSRD_SRD_PLL_1_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_1_DP_H_RSTBf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_PLL_1_CTRLr(unit, fsrd_schan_id, reg_val));
    sal_usleep(1);

    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

    soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_H_RSTBf, 0xff);
    soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_H_RSTBf, 0xff);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
    sal_usleep(1);

    soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_DP_H_RSTBf, 0xff);
    soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_DP_H_RSTBf, 0xff);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
    sal_usleep(1);

    /* Mark DB as initialized */
    is_core_initialized = TRUE;
    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN; 
}

int
pm8x50_fabric_interface_config_validate(int unit, pm_info_t pm_info, const portmod_port_interface_config_t* config) 
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    SOCDNX_INIT_FUNC_DEFS;

   if(fabric_data->ref_clk == phymodRefClk156Mhz){
        switch(config->speed){
        case 25781: /*156.25Mhz only*/
        case 21875:
        case 10937:
        case 25000: /*shared speeds*/
        case 20625: 
        case 12500:
        case 10312:
        case 6250:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM , (_BSL_SOCDNX_MSG("unsupported speed %d for 156.25Mhz"), config->speed));
        }
    } else{
        switch(config->speed){
        case 23000: /*125Mhz only*/
        case 22500:
        case 11500:
        case 11250:
        case 5750:
        case 25000: /*shared speeds*/
        case 20625: 
        case 12500:
        case 10312:
        case 6250:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM , (_BSL_SOCDNX_MSG("unsupported speed %d for 125Mhz"), config->speed));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}




int pm8x50_fabric_port_disable_init(int unit, int port, pm_info_t pm_info)
{
    uint32 reg_val;
    uint32 field_val[1];
    uint32 phy_index, bitmap, curr_phy_index;
    int i, j;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    SOCDNX_INIT_FUNC_DEFS;

    /*Port (MAC + PHY) Enable Disable support*/

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));


    for (i = 0; i < nof_fmac_blks; i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[i], &reg_val));
        *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SHR_BITSET(field_val, j);
            }
        }
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *field_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[i], reg_val));
    }

    for (i = 0; i < nof_fmac_blks; i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[i], &reg_val));
        *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SHR_BITSET(field_val, j);
            }
        }
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *field_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[i], reg_val));
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionIn));
    
exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC 
int pm8x50_fabric_power_set(int unit, int port, pm_info_t pm_info, int power)
{
    portmod_access_get_params_t params;
    phymod_phy_power_t phy_power;
    phymod_phy_access_t phy_access;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access)); 

    if (power) {
        phy_power.rx = phymodPowerOn; 
        phy_power.tx = phymodPowerOn;
    } else {
        phy_power.rx = phymodPowerOff; 
        phy_power.tx = phymodPowerOff;
    }
       
    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    SOCDNX_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));

exit:
    SOCDNX_FUNC_RETURN; 
}

int pm8x50_fabric_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int i = 0, j, ii, phy;
    int found = FALSE;
    int rv = 0;
    int ports_db_entry = -1;
    phymod_phy_init_config_t init_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys, phyn;
    portmod_pbmp_t port_phys_in_pm;
    uint32 reg_val, field_val, phy_index, bitmap, curr_phy_index;
    uint64 reg64_val;
    uint32 is_core_probed = 0, is_core_initialized = 0;
    uint32 is_bypass = 0, phys_count;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    uint32 fmac_blk_id;
    int nof_fmac_blks, first_fmac_index;
    uint32 rx_polarity, tx_polarity;
    soc_reg_t tx_lane_map_regs[] = {FMAC_TX_LANE_SWAP_0r, FMAC_TX_LANE_SWAP_1r, FMAC_TX_LANE_SWAP_2r, FMAC_TX_LANE_SWAP_3r};
    soc_field_t tx_lane_map_fields[] = {TX_LANE_SWAP_0f, TX_LANE_SWAP_1f, TX_LANE_SWAP_2f, TX_LANE_SWAP_3f};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));

    rv = PM8X50_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, fabric_data->phys);
    PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);

    ii = 0;
    SOC_PBMP_ITER(fabric_data->phys, phy){
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)){
            rv = PM8X50_FABRIC_LANE2PORT_SET(unit, pm_info, ii, port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        ii++;
    }

    if(!PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        for( i = 0 ; i < PM8X50_FABRIC_MAX_PORTS_PER_PM; i++){
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
            SOCDNX_IF_ERR_EXIT(rv);
            if(ports_db_entry < 0 ){ /*free slot found*/
                rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &port, i);
                SOCDNX_IF_ERR_EXIT(rv);
                found = TRUE;
                break;
            }
        }
        if(!found){
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("no free space in the PM db")));
        }
    }

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
    SOCDNX_IF_ERR_EXIT(rv);
    if (!is_core_probed) {
        /*configure lane swap once on all lanes of PM*/
        for (i = 0; i < PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC; ++i) {
            fmac_blk_id = fabric_data->fmac_schan_id[i] | SOC_REG_ADDR_SCHAN_ID_MASK;
            for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; ++j) {
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, tx_lane_map_regs[j], fmac_blk_id, 0, &reg_val));
                /* Tx mapping: serdes to lane (FMAC lane) */
                field_val = add_info->lane_map.lane_map_tx[PM8X50_FABRIC_LANE_INDEX_IN_PM_GET(i, j)];
                field_val = PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(field_val);
                soc_reg_field_set(unit, tx_lane_map_regs[j], &reg_val, tx_lane_map_fields[j], field_val);

                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, tx_lane_map_regs[j], fmac_blk_id, 0, reg_val));
            }
        }
    }

    if(!is_core_probed || (is_core_probed == 1 && PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info))){ 
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_interface_config_validate(unit, pm_info, &add_info->interface_config));
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_core_init_phase1(unit, port, pm_info, add_info));
    }
    
    if(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        SOC_EXIT;
    }

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized);
    SOCDNX_IF_ERR_EXIT(rv);
    if(!is_core_initialized) {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_core_init_phase2(unit, port, pm_info , &add_info->interface_config));
    }  

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_power_set(unit, port, pm_info, 1));
        
    SOCDNX_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
    init_config.an_en = add_info->autoneg_en;
    init_config.cl72_en = add_info->link_training_en;

    for (i = 0; i < PM8X50_FABRIC_LANES_PER_CORE; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i); 
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);

        init_config.polarity.rx_polarity |= (rx_polarity << i) & 0xff;
        init_config.polarity.tx_polarity |= (tx_polarity << i) & 0xff;
    }

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if (!is_bypass)
    {
        params.phyn = 0;
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
        SOCDNX_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access, phymodMediaTypeChipToChip, init_config.tx));
        SOCDNX_IF_ERR_EXIT(phymod_phy_init(&phy_access, &init_config));
    }
    else {
        params.phyn = 0;
        for (phyn = 0; phyn < PM8X50_FABRIC_LANES_PER_CORE; phyn++) {
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
            SOCDNX_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access, phymodMediaTypeChipToChip, &init_config.tx[phyn]));
        }
        PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(phy_access.device_op_mode);
        SOCDNX_IF_ERR_EXIT(phymod_phy_init(&phy_access, &init_config));
        PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_CLR(phy_access.device_op_mode);
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));
    if (is_bypass) {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

        for(i = 0; i < nof_fmac_blks; i++) {
            for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {

                curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);

                if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_blk_ids[i], j, &reg_val));
                    soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_TX_ILKN_SHARING_ENf, 1);
                    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_blk_ids[i], j, reg_val));

                    SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[i], j, &reg64_val));
                    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_TX_ILKN_ENf, 1);
                    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_RX_ILKN_ENf, 1);
                    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_SLOW_READ_RATEf, 1);
                    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg64_val));
                }
            }
        }
    }

    /*disable phy and mac*/  
    /*SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, 0));*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_disable_init(unit, port, pm_info));
   
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_interface_config_set(unit, port, pm_info , &add_info->interface_config, 0));

exit:
    SOCDNX_FUNC_RETURN; 
}


int pm8x50_fabric_port_detach(int unit, int port, pm_info_t pm_info)
{
    int i = 0;
    int found = FALSE;
    int invalid_port = -1;
    int ports_db_entry = -1;
    int rv = 0;
    int is_last = TRUE;
    int unintialized = 0;
    int fsrd_schan_id;
    uint32 reg_val, tmp_port;
    soc_reg_above_64_val_t reg_above64_val;
    SOCDNX_INIT_FUNC_DEFS;
    PM8X50_FABRIC_LEGACY_FIXME_ASSERT;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_power_set(unit, port, pm_info, 0));
     
    for( i = 0 ; i < PM8X50_FABRIC_MAX_PORTS_PER_PM; i++){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_db_entry == port){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &invalid_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
            found = TRUE;
        } else if(ports_db_entry >= 0){
            is_last = FALSE;
        }
        rv = PM8X50_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if(tmp_port == port){
            rv = PM8X50_FABRIC_LANE2PORT_SET(unit, pm_info, i, invalid_port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }
    if(!found){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("port was not found in the PM db")));
    }
    if (is_last){
        /*Quad in-reset*/
        fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_DP_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        sal_usleep(1);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_DP_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        sal_usleep(1);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_H_RSTBf, 0x0);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        sal_usleep(1);

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_PLL_0_CTRLr(unit, fsrd_schan_id, &reg_val));
        soc_reg_field_set(unit, FSRD_SRD_PLL_0_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_0_DP_H_RSTBf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_PLL_0_CTRLr(unit, fsrd_schan_id, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_PLL_1_CTRLr(unit, fsrd_schan_id, &reg_val));
        soc_reg_field_set(unit, FSRD_SRD_PLL_1_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_1_DP_H_RSTBf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_PLL_1_CTRLr(unit, fsrd_schan_id, reg_val));
        sal_usleep(1);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_POR_H_RSTBf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        sal_usleep(10);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_IDDQf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_RX_H_PWRDNf, 0xff);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_PMD_LN_TX_H_PWRDNf, 0xff);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
        
        sal_usleep(10);

        /*remove from data - base*/  
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &unintialized);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &unintialized);
        SOCDNX_IF_ERR_EXIT(rv);       
        rv = PM8X50_FABRIC_IS_BYPASSED_SET(unit, pm_info, unintialized);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int pm8x50_fabric_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
        
    int i = 0;
    int rv = 0;
    int ports_db_entry;
    uint32 tmp_port;
    SOCDNX_INIT_FUNC_DEFS;
    PM8X50_FABRIC_LEGACY_FIXME_ASSERT;
    
    for (i = 0; i < PM8X50_FABRIC_MAX_PORTS_PER_PM; i++){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_db_entry == port){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &new_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        rv = PM8X50_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = PM8X50_FABRIC_LANE2PORT_SET(unit, pm_info, i, new_port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }
                   
exit:
    SOCDNX_FUNC_RETURN; 
    
}


soc_error_t pm8x50_fabric_fmac_enable_set(int unit, int port, pm_info_t pm_info, int rx_tx, int enable)
{
    soc_field_t field;
    uint32 field_val[1];
    uint32 reg_val, phy_index, bitmap, curr_phy_index;
    int i, j;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    field = ((rx_tx == PM8X50_FABRIC_FMAC_TX) ? FMAC_TX_RST_Nf : FMAC_RX_RST_Nf);

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    for(i = 0 ; i < nof_fmac_blks ; i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[i], &reg_val));
        *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, field);
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++)
        {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                if (enable){
                    SHR_BITCLR(field_val, j);
                }else{
                    SHR_BITSET(field_val, j);
                }
            }
        }
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, field, *field_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[i], reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    uint32 flags_mask = flags;
    uint32 is_bypass;
    int cur_enable, rv;
    SOCDNX_INIT_FUNC_DEFS;

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(flags_mask);
        PORTMOD_PORT_ENABLE_TX_SET(flags_mask);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(flags_mask);
        PORTMOD_PORT_ENABLE_MAC_SET(flags_mask);
    }

    rv = PM8X50_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) && 
             PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        /*Port (MAC + PHY) Enable Disable support*/

        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, flags, &cur_enable)); 
        if ((cur_enable ? 1 : 0) == (enable ? 1 : 0))
        {
            SOC_EXIT;
        }

        if(enable){

            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionOut));

            /* Enable FMAC Tx*/
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));

            /* Enable FMAC Rx*/
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));

        } 
        else
        {
            /* Disable FMAC Rx*/
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));

            /* Disable FMAC Tx*/
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));

            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionIn));
        }
    }
    else if ((PORTMOD_PORT_ENABLE_RX_GET(flags)) && ((PORTMOD_PORT_ENABLE_MAC_GET(flags)) || is_bypass))
    {
        /*MAC RX Enable/Disable support*/
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));
    } 
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (!is_bypass){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Fabric doesn't support only MAC option. (Only ILKN over fabric supports that mode).\n")));
        }
        if(enable){
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));
        }else{
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_RX, enable));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));
        }
    }
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            !PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (!is_bypass){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Fabric doesn't support only Tx MAC option. (Only ILKN over fabric supports that mode).\n")));
        }
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_enable_set(unit, port, pm_info, PM8X50_FABRIC_FMAC_TX, enable));
    }
    else if (!PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) &&
             PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (!is_bypass){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Fabric doesn't support only Phy option. (Only ILKN over fabric supports that mode).\n")));
        }


        if(enable){

            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionOut));
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionIn));
        }
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Supported options are only: (1) all, (2) MAC + RX \n (3) MAC (but for ILKN over fabric only) (4) Tx MAC (but for ILKN over fabric only)\n (5) Phy (but for ILKN over fabric only)\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int *enable)
{
    uint32 reg_val, flags_mask = flags;
    uint32 field_val[1];
    uint32 is_bypass, mac_reset, tx_mac_reset, bitmap;
    uint32 phy_index;
    phymod_phy_access_t phy_access;
    phymod_phy_reset_t phy_reset;
    int rx_enable, tx_enable;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    portmod_access_get_params_t params;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));    
    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    /*if the first phy in the first FMAC is enabled - assuming the whole port is enabled*/
    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_ids[0], &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    mac_reset = SHR_BITGET(field_val, PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    tx_mac_reset = SHR_BITGET(field_val, PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index));

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(flags_mask);
        PORTMOD_PORT_ENABLE_TX_SET(flags_mask);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(flags_mask);
        PORTMOD_PORT_ENABLE_MAC_SET(flags_mask);
    }
    
    if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) && 
         PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        SOCDNX_IF_ERR_EXIT(phymod_phy_reset_get(&phy_access, &phy_reset)); 
         rx_enable = ((!mac_reset) && (phy_reset.rx == phymodResetDirectionOut));
         tx_enable = ((!mac_reset) && (phy_reset.tx == phymodResetDirectionOut));
         *enable = rx_enable || tx_enable;
    } 
    else if ((PORTMOD_PORT_ENABLE_RX_GET(flags)) && ((PORTMOD_PORT_ENABLE_MAC_GET(flags)) || is_bypass))
    {
        *enable = !mac_reset;
    } 
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            !PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        *enable = !tx_mac_reset;
    }
    else 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Supported options are only (1) Rx MAC (2) Tx MAC or (3) all - MAC, PHY, TX, RX")));
    }


exit:
    SOCDNX_FUNC_RETURN;    
}

/*
 * pm8x50_fabric_port_fec_sync_machine_config
 * This function configures RS FEC sync machine (relevant only to RS FEC).
 * The sync machine synchronizes on the start of a frame.
 * It chooses a bit as a candidate, then looks for K valid frames out of N frames in 3 stages,
 * to determine if a synchronization on the start of the frame is successful and this is indeed the first bit.
 * The configuration depends both on FEC type and on PAM4/NRZ.
 * Thus this function needs to be called when setting the encoding and when setting the speed.
*/
static
int pm8x50_fabric_port_fec_sync_machine_config(int unit, int port, pm_info_t pm_info)
{
    uint32 properties = 0;
    portmod_port_pcs_t pcs;
    portmod_port_interface_config_t interface_config;
    pm8x50_fabric_fec_sync_machine_config_t sync_machine_config;
    uint32 phy_index, bitmap, curr_phy_index;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int i, j, nof_fmac_blks, first_fmac_index;
    soc_reg_above_64_val_t reg_above64;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_encoding_get(unit, port, pm_info, &properties, &pcs));
    if (!PM8X50_FABRIC_ENCODING_IS_RS_FEC(pcs)) {
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_interface_config_get(unit, port, pm_info, &interface_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));

    if (PM8X50_FABRIC_SPEED_IS_PAM4(interface_config.speed)) {
        sync_machine_config.n1 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N1_PAM4;
        sync_machine_config.n2 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N2_PAM4;
        sync_machine_config.n3 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N3_PAM4;
        sync_machine_config.k1 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K1_PAM4;
        sync_machine_config.k2 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K2_PAM4;
        sync_machine_config.k3 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K3_PAM4;
    } else { /*NRZ*/
        sync_machine_config.n1 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N1_NRZ;
        sync_machine_config.n2 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N2_NRZ;
        sync_machine_config.n3 = PM8X50_FABRIC_FEC_SYNC_MACHINE_N3_NRZ;
        sync_machine_config.k1 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K1_NRZ;
        sync_machine_config.k2 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K2_NRZ;
        sync_machine_config.k3 = PM8X50_FABRIC_FEC_SYNC_MACHINE_K3_NRZ;
    }

    if (PM8X50_FABRIC_FEC_SYNC_MACHINE_IS_TRIPLE_BIT_SLIP(pcs, sync_machine_config.n1)) {
        sync_machine_config.slip = 1;
    } else {
        sync_machine_config.slip = 0;
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    for(i = 0 ; i < nof_fmac_blks ; i++) {
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg_above64));
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_N_1f, sync_machine_config.n1);
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_N_2f, sync_machine_config.n2);
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_N_3f, sync_machine_config.n3);
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_K_1f, sync_machine_config.k1);
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_K_2f, sync_machine_config.k2);
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_SYNC_K_3f, sync_machine_config.k3);
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, AUTO_DOC_NAME_98f, sync_machine_config.slip);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg_above64));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int phy_init_flags)
{
    phymod_phy_inf_config_t interface_config;
    phymod_phy_access_t phy;
    int nof_phys;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    portmod_access_get_params_t params;
    uint32 flags;
    int enabled = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /*nothing to do for warm boot*/
    if (SOC_WARM_BOOT(unit))
    {
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_interface_config_validate(unit, pm_info, config));

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    SOCDNX_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&interface_config));

    interface_config.data_rate = config->speed;
    interface_config.ref_clock = fabric_data->ref_clk;
    interface_config.interface_type = phymodInterfaceBypass;
    interface_config.interface_modes = config->interface_modes;

    /*Disable ports*/
    flags = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));

    /*disable phy and mac*/  
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, 0));

    params.phyn = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy, &nof_phys, NULL));
    SOCDNX_IF_ERR_EXIT(phymod_phy_interface_config_set(&phy, config->flags, &interface_config));

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_fec_sync_machine_config(unit, port, pm_info)); /*needs to be called when changing the speed*/

    /*restore MAC and PHY */
    flags = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, enabled));

exit:
    SOCDNX_FUNC_RETURN; 
}


int pm8x50_fabric_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{

    portmod_access_get_params_t params;
    phymod_phy_access_t phy;
    int phys_num;
    phymod_phy_inf_config_t phy_config;
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pm_info);

    portmod_access_get_params_t_init(unit, &params);
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1 , &phy , &phys_num, NULL));
    if(phys_num != 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL , (_BSL_SOCDNX_MSG("unexpected phys num %d"), phys_num));
    }
        
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, config));
    SOCDNX_IF_ERR_EXIT(phymod_phy_interface_config_get(&phy, config->flags, fabric_data->ref_clk, &phy_config));

    config->speed = phy_config.data_rate;
    config->interface_modes = phy_config.interface_modes;
    config->interface = SOC_PORT_IF_SFI;

exit:
    SOCDNX_FUNC_RETURN; 
    
}

int pm8x50_fabric_port_default_interface_get(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, soc_port_if_t* interface)
{
    *interface = SOC_PORT_IF_NULL;
    return SOC_E_NONE;
}

int pm8x50_fabric_port_encoding_set(int unit, int port, pm_info_t pm_info, uint32 properties, portmod_port_pcs_t encoding)
{
    int fec_enable = 1;
    int encoding_type = 0;
    int slow_read_rate = 0;
    int rx_ctrl_bypass = 0;
    int flags = 0;
    int enabled = 0;
    int llfc_cig = 0;
    int low_latency_llfc = 0;
    int rsf_err_mark = 0;
    int fec_err_mark = 0;
    int cig_ignore = 0;
    int llfc_after_fec = 0;
    int fmal_width = 0;
    int rsf_frame_type = 3; /*same as HW default, stands for 15T_RS_FEC*/
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    uint32 phy_index, bitmap, curr_phy_index;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64;
    uint64 reg64_val;
    int i , j;
    SOCDNX_INIT_FUNC_DEFS;

    switch(encoding){
    case PORTMOD_PCS_64B66B_FEC:
        encoding_type = 2;
        slow_read_rate = 3;
        fmal_width = 2;
        break;
    case PORTMOD_PCS_64B66B:
        encoding_type = 2;
        slow_read_rate = 1;
        fec_enable = 0;
        fmal_width = 2;
        break;
    case PORTMOD_PCS_64B66B_RS_FEC:
        encoding_type = 4;
        slow_read_rate = 4;
        rx_ctrl_bypass = 3;
        low_latency_llfc = 1;
        llfc_cig = 1;
        fmal_width = 0;
        rsf_frame_type = 1;
        break;
    case PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC:
        encoding_type = 4;
        slow_read_rate = 4;
        rx_ctrl_bypass = 3;
        low_latency_llfc = 1;
        llfc_cig = 1;
        fmal_width = 0;
        rsf_frame_type = 0;
        break;
    case PORTMOD_PCS_64B66B_15T_RS_FEC:
        encoding_type = 4;
        slow_read_rate = 4;
        rx_ctrl_bypass = 3;
        low_latency_llfc = 1;
        llfc_cig = 1;
        fmal_width = 0;
        rsf_frame_type = 3;
        break;
    case PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC:
        encoding_type = 4;
        slow_read_rate = 4;
        rx_ctrl_bypass = 3;
        low_latency_llfc = 1;
        llfc_cig = 1;
        fmal_width = 0;
        rsf_frame_type = 2;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported pcs type %d"), encoding));
    }

    /* handle properties*/
    /*Low latency llfc*/
    if(PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("low latency llfc is not supported for 64/66 pcs")));
    }
    if(!PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("RS_FEC pcs: low latency llfc is always enabled")));
    }
    if(PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties)){
        low_latency_llfc = 1;
    }

    /*error detect*/
    if(PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("error detect is not supported for 64/66 pcs")));
    }
    if(encoding == PORTMOD_PCS_64B66B_FEC){
        fec_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    } else if(PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        rsf_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    }

    /*extract cig from llfc cells*/
    if(PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties) && (encoding != PORTMOD_PCS_64B66B_FEC)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("extract CIG indications from LLFC cells is supported just for 64/66 kr fec")));
    }
    cig_ignore = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties) ? 0 : 1;

    /*do FEC on LLFC and congestion indication (get llfc/cig values after error correction)*/
    if(PORTMOD_ENCODING_LLFC_AFTER_FEC_GET(properties) && !PM8X50_FABRIC_ENCODING_IS_RS_FEC(encoding)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("llfc after FEC is supported only for rs fecs")));
    }
    llfc_after_fec = PORTMOD_ENCODING_LLFC_AFTER_FEC_GET(properties);

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    flags = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));
    /*disable phy and mac*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, 0));

    for(i = 0 ; i < nof_fmac_blks ; i++) {
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[i], j, &reg_val));
                /*encoding type set*/
                soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_ENCODING_TYPEf, encoding_type);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg_val));
            }
        }
    }

    for(i = 0 ; i < nof_fmac_blks ; i++) {
         for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
             curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
             if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                 SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[i], j, &reg64_val));
                 soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_SLOW_READ_RATEf, slow_read_rate);
                 soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_WIDTHf, fmal_width);
                 soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_TX_WIDTHf, fmal_width);
                 SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg64_val));
             }
         }
     }

    for(i = 0 ; i < nof_fmac_blks ; i++) {
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_ids[i], j, &reg_val));
                /*enable/disable FEC for 64\66 and 64\66 KR*/
                soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr, &reg_val, FPS_N_RX_FEC_FEC_ENf, fec_enable);
                /*error mark en - relevant only for KR FEC(64B66B_FEC)*/
                soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr, &reg_val, FPS_N_RX_FEC_ERR_MARK_ENf, fec_err_mark);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg_val));
            }
        }
    }

    for(i = 0 ; i < nof_fmac_blks ; i++) {
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_TX_CONFIGURATIONr(unit, fmac_blk_ids[i], j, &reg_val));
                soc_reg_field_set(unit, FMAC_FPS_TX_CONFIGURATIONr, &reg_val, FPS_N_TX_FEC_ENf, fec_enable);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_TX_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg_val));
            }
        }
    }

    for(i = 0 ; i < nof_fmac_blks ; i++) {
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[i], j, &reg_val));
                /*set llfc low latency and enable cig*/
                soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_LOW_LATENCY_LLFCf, low_latency_llfc);
                soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_LOW_LATENCY_CIGf, llfc_cig);
                soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_IGNORE_CIG_LLFC_CELLf, cig_ignore);
                soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_RX_CTRL_BYPf, rx_ctrl_bypass);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg_val));
            }
        }
    }

    /* RS-FEC configurations */
    for(i = 0 ; i < nof_fmac_blks ; i++) {
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg_above64));
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_ERR_MARK_ENf, rsf_err_mark);

                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_TX_FRAME_TYPEf, rsf_frame_type);
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_TYPEf, rsf_frame_type);

                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_TX_SCRAMBLE_LLFC_BITS_ENf, 0x1);
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_SCRAMBLE_LLFC_BITS_ENf, 0x1);

                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_LLFC_AFTER_FECf, llfc_after_fec);

                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_ids[i], j, reg_above64));
            }
        }
    }

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_fec_sync_machine_config(unit, port, pm_info));

    /*restore MAC and PHY */
    flags = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, enabled));

exit:
    SOCDNX_FUNC_RETURN; 
}

int pm8x50_fabric_port_encoding_get(int unit, int port, pm_info_t pm_info, uint32 *properties, portmod_port_pcs_t *encoding)
{
    int encoding_type = 0;
    int rsf_frame_type = 0;
    int low_latency_llfc = 0;
    int err_mark = 0;
    int cig_ignore = 0;
    int llfc_after_fec = 0;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC];
    int nof_fmac_blks, first_fmac_index;
    uint32 phy_index, bitmap;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64;
    int fec_enable;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
 
    *properties = 0;
    encoding_type = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_ENCODING_TYPEf);
    switch(encoding_type){
    case 2:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        fec_enable = soc_reg_field_get(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr, reg_val,FPS_N_RX_FEC_FEC_ENf); 
        *encoding = fec_enable ? PORTMOD_PCS_64B66B_FEC : PORTMOD_PCS_64B66B;
        break;
    case 4:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_above64));
        rsf_frame_type = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_FRAME_TYPEf);
        switch(rsf_frame_type) {
        case 0:
            *encoding = PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC;
            break;
        case 1:
            *encoding = PORTMOD_PCS_64B66B_RS_FEC;
            break;
        case 2:
            *encoding = PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC;
            break;
        case 3:
            *encoding = PORTMOD_PCS_64B66B_15T_RS_FEC;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unknown RS-FEC type")));
        }
        break;
    case 0:
        *encoding = PORTMOD_PCS_UNKNOWN;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unknown pcs type %d"), encoding_type));
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
    /*set llfc low latency and enable cig*/
    low_latency_llfc = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val ,FMAL_N_LOW_LATENCY_LLFCf);
    cig_ignore = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_IGNORE_CIG_LLFC_CELLf);
    if (*encoding == PORTMOD_PCS_64B66B_FEC)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        err_mark = soc_reg_field_get(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr, reg_val, FPS_N_RX_FEC_ERR_MARK_ALLf); 
        llfc_after_fec = 0;
    } else if (PM8X50_FABRIC_ENCODING_IS_RS_FEC(*encoding))
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_above64));
        err_mark = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_ERR_MARK_ENf);
        llfc_after_fec = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_LLFC_AFTER_FECf);
    } else {
        err_mark = 0;
        llfc_after_fec = 0;
    }

    if(low_latency_llfc){
         PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(*properties);
    }
    if(!cig_ignore){
         PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(*properties);
    }
    if(err_mark){
         PORTMOD_ENCODING_FEC_ERROR_DETECT_SET(*properties);
    }
    if(llfc_after_fec){
        PORTMOD_ENCODING_LLFC_AFTER_FEC_SET(*properties);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


/* SRD_N_FORCE_SIGNAL_DETECT and FMAL_N_TX_SRD_BACKPRESSURE_EN configurations are irrelevant for ILKN-over-fabric */
STATIC int
_pm8x50_fabric_loopback_serdes_overrides_set(int unit, int port, pm_info_t pm_info)
{
    int i, port_index;
    int rv = 0;
    soc_reg_above_64_val_t reg_above64_val;
    int ports_db_entry = -1;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    portmod_loopback_mode_t mac_loopbacks[]= { portmodLoopbackMacAsyncFifo, portmodLoopbackMacOuter, portmodLoopbackMacPCS, portmodLoopbackPhyGloopPMD,portmodLoopbackPhyRloopPMD};
    int lb_enable = FALSE;
    uint32 fsrd_schan_id;
    uint32 phy_index, bitmap;
    int backpressure_en=1;
    uint32 reg_val = 0, field_val = 0;
    uint32 force_signal_detect_set = 0;
    SOCDNX_INIT_FUNC_DEFS;

    for(port_index = 0 ; port_index < PM8X50_FABRIC_MAX_PORTS_PER_PM; port_index++){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, port_index);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_db_entry < 0){
            continue;
        }
        for( i = 0 ; i < sizeof(mac_loopbacks)/sizeof(mac_loopbacks[0]); i++){
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_loopback_get(unit, ports_db_entry, pm_info, mac_loopbacks[i], &lb_enable));
            if(lb_enable){
                force_signal_detect_set = 1;
                if (mac_loopbacks[i] == portmodLoopbackMacAsyncFifo || mac_loopbacks[i] == portmodLoopbackMacPCS) {
                    backpressure_en = 0;
                }
                break;
            }
        }
    }

    /*Ignore analog signals from SerDes in case of loopback*/
    fsrd_schan_id = pm_info->pm_data.pm8x50_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));
    field_val = soc_reg_above_64_field32_get(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_FORCE_SIGNAL_DETECTf);
    force_signal_detect_set = force_signal_detect_set << phy_index;
    field_val |= force_signal_detect_set;
    soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above64_val, SRD_N_FORCE_SIGNAL_DETECTf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_CTRLr(unit, fsrd_schan_id, reg_above64_val));

    /*back-pressure from SerDes Tx should be disabled for portmodLoopbackMacAsyncFifo and portmodLoopbackMacPCS LBs - to ignore flow control from SerDes*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_TX_SRD_BACKPRESSURE_ENf, backpressure_en);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int pm8x50_fabric_mac_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    uint32 reg_val = 0;
    uint32 field[1] = {0};
    uint32 phy_index, bitmap;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    switch(loopback_type){
    case portmodLoopbackMacOuter:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_ids[0], &reg_val));
        *field = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
        if(enable) {
            SHR_BITSET(field, PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index));
        } else {
            SHR_BITCLR(field, PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index));
        }
        soc_reg_field_set(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, &reg_val, LCL_LPBK_ONf, *field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_ids[0], reg_val));
        if (enable)
        {
            LOG_WARN(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "Warning: MAC outer loopback is supported over rates 20.625 and higher\n")));

        }
        break;

    case portmodLoopbackMacAsyncFifo:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMALN_CORE_LOOPBACKf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CNTRL_BURST_PERIODf, enable ? 0xc : 8);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));
        break;

    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        soc_reg_field_set(unit, FMAC_KPCS_CONFIGURATIONr, &reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CNTRL_BURST_PERIODf, enable ? 0xd : 8);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("should not reach here")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Checks per a given port if lanes are swapped.
 * Done by checking FMAC_TX_LANE_SWAP_ register with the lane index of the given port:
 * if lanes aren't swapped the mapping of this register (serdes->lane) will be 1:1.
 */
STATIC
int pm8x50_fabric_port_are_lanes_swapped(int unit, int port, pm_info_t pm_info, int* lanes_are_swapped)
{
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    uint32 phy_index, bitmap, curr_phy_index;
    int i, j;
    soc_reg_t tx_lane_map_regs[] = {FMAC_TX_LANE_SWAP_0r, FMAC_TX_LANE_SWAP_1r, FMAC_TX_LANE_SWAP_2r, FMAC_TX_LANE_SWAP_3r};
    soc_field_t tx_lane_map_fields[] = {TX_LANE_SWAP_0f, TX_LANE_SWAP_1f, TX_LANE_SWAP_2f, TX_LANE_SWAP_3f};
    uint32 reg_val, lane;
    SOCDNX_INIT_FUNC_DEFS;

    *lanes_are_swapped = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    for(i = 0 ; i < nof_fmac_blks ; i++) {
        for (j = 0; j < PM8X50_FABRIC_LANES_PER_FMAC; j++) {
            curr_phy_index = PM8X50_FABRIC_LANE_INDEX_IN_PM_GET((first_fmac_index+i), j);
            if (PM8X50_FABRIC_IS_LANE_USED_FOR_PORT(curr_phy_index, bitmap)) {
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, tx_lane_map_regs[j], fmac_blk_ids[i], 0, &reg_val));
                lane = soc_reg_field_get(unit, tx_lane_map_regs[j], reg_val, tx_lane_map_fields[j]);
                if(lane != j)
                {
                    *lanes_are_swapped = 1;
                    SOC_EXIT;
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int is_phy_loopback = FALSE;
    int lb_enable = 0;
    uint32 encoding_properties;
    portmod_port_pcs_t encoding = 0;
    uint32 flags;
    int enabled;
    uint32 is_bypass;
    SOCDNX_INIT_FUNC_DEFS;

    /*Get phy and mac reset state*/
    flags = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));

    /*Disable phy and mac*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, 0));    

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacAsyncFifo:
    case portmodLoopbackMacOuter:
        break;
    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_encoding_get(unit, port, pm_info, &encoding_properties ,&encoding));
        if((encoding != PORTMOD_PCS_64B66B_FEC) && (encoding != PORTMOD_PCS_64B66B)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported encoding type %d for MAC PCS loopback"), encoding));
        }
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
        is_phy_loopback = TRUE;
        break;
    /*PHY PCS modes and MAC core are not supported*/
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported loopback type %d"), loopback_type));
    }

    /*check if not already defined*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_loopback_get(unit, port, pm_info, loopback_type, &lb_enable));
    if(enable == lb_enable){
        SOC_EXIT;
    }

    /*loopback set*/
    if(is_phy_loopback){
        int lanes_are_swapped = 0;
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_are_lanes_swapped(unit, port, pm_info, &lanes_are_swapped));
        if(enable && lanes_are_swapped)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("configuring phy loopback on a lane-swapped port is not supported, align lane-swap to be one-to-one or configure mac loopback")));
        }
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_loopback_set(unit, port, pm_info,loopback_type, enable));
    } else { /*MAC looopback*/
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_mac_loopback_set(unit, port, pm_info, loopback_type, enable));
    }

    SOCDNX_IF_ERR_EXIT(PM8X50_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass));
    if(!is_bypass) {
        SOCDNX_IF_ERR_EXIT(_pm8x50_fabric_loopback_serdes_overrides_set(unit, port, pm_info));
    }
    /*restore MAC and PHY */
    flags = 0;
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_set(unit, port, pm_info, flags, enabled));

exit:
    SOCDNX_FUNC_RETURN; 
}


int pm8x50_fabric_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    uint32 reg_val = 0;
    uint32 field[1] = {0};
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    uint32 phy_index, bitmap;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    switch(loopback_type){
    case portmodLoopbackMacAsyncFifo:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        *enable = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMALN_CORE_LOOPBACKf);
        break;
    case portmodLoopbackMacOuter:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_ids[0], &reg_val));
        *field = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
        *enable = SHR_BITGET(field, PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index));
        break;
    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        *enable = soc_reg_field_get(unit, FMAC_KPCS_CONFIGURATIONr, reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf);
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_loopback_get(unit, port, pm_info, loopback_type, enable));
        break;
    /*PHY pcs modes and MAC core are not supported*/
    default:
        *enable = 0; /*not supported - no loopback*/
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


int pm8x50_fabric_port_link_get(int unit, int port, pm_info_t pm_info, int flag, int* link)
{
    int enable,
        rv;
    uint32 phy_index, bitmap;
    uint32 
        is_down=0,
        reg_val = 0,
        fld_val[1] = {0},
        rx_pmd_locked;
    phymod_phy_access_t phy_access[1];
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    portmod_access_get_params_t params;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    /*link is down unless we find it's up*/
    *link = 0;

    /*check if port is enabled*/
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_enable_get(unit, port, pm_info, 0, &enable));
    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    if (enable) {
        /*clear rxlos interrupt*/
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        reg_val = 0;
        SHR_BITSET(fld_val, PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index));
        soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit, fmac_blk_ids[0], reg_val));
        /*check if Serdes is locked*/
        SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));
        params.phyn = 0;
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, phy_access, &nof_phys, NULL));
        rv = phymod_phy_rx_pmd_locked_get(phy_access, &rx_pmd_locked);
        if (rv != PHYMOD_E_NONE) {
            return rv;
        }

        if(rx_pmd_locked == 1)
        {
            /*check if FMAC is synced - check rxlos interrupt*/
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit, fmac_blk_ids[0], &reg_val));

            *fld_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
            is_down = SHR_BITGET(fld_val, PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index));
            *link = (is_down == 0);
        }
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}

int pm8x50_fabric_port_prbs_config_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;
    uint32 poly_val = 0;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_config_set(unit, port, pm_info, flags, config));
    }
    else{
        switch(config->poly){
        case phymodPrbsPoly31:
            poly_val = 1;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported polynomial for MAC PRBS %d"), config->poly));
        }
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, &reg_val, KPCS_N_TST_RX_PTRN_SELf, poly_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, &reg_val, KPCS_N_TST_TX_PTRN_SELf, poly_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));
        }
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}

int pm8x50_fabric_port_prbs_config_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    uint32 reg_val;
    uint32 tx_poly;
    uint32 poly_val = 0; 
    uint32 phy_index, bitmap;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_config_get(unit, port, pm_info, flags, config));
    }
    else{
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
            poly_val = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_PTRN_SELf);
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
            tx_poly = soc_reg_field_get(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, reg_val, KPCS_N_TST_TX_PTRN_SELf);
            if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
                if(poly_val != tx_poly){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("rx and tx are not configured the same")));
                }
            }
            poly_val = tx_poly;
        }
        switch(poly_val){
        case 1:
            config->poly = phymodPrbsPoly31;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unknown polynomial %u"), poly_val));
        }
        /*no meaning for invert in MAC PRBS*/
        config->invert = 0;
    }

exit:
    SOCDNX_FUNC_RETURN; 
    
}


int pm8x50_fabric_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int enable)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_enable_set(unit, port, pm_info, flags, enable));
    }
    else{
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, &reg_val, KPCS_N_TST_RX_ENf, enable ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, &reg_val, KPCS_N_TST_TX_ENf, enable ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int* enable)
{
    uint32 reg_val;
    int tx_enable;
    uint32 phy_index, bitmap;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_enable_get(unit, port, pm_info, flags, enable));
    }
    else{
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
            *enable = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_ENf);
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
            tx_enable = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_ENf);
            /*validate rx and tx are configured the same*/
            if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
                if(*enable != tx_enable){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("rx and tx are not configured the same")));
                }
            }
            *enable = tx_enable;
        }
    }        
exit:
    SOCDNX_FUNC_RETURN;
}


int pm8x50_fabric_port_prbs_status_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;
    uint32 fmac_blk_ids[PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC] = {0};
    int nof_fmac_blks, first_fmac_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_fmac_blk_ids_get(unit, port, pm_info, fmac_blk_ids, &nof_fmac_blks, &first_fmac_index));

    if(nof_fmac_blks > 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                (_BSL_SOCDNX_MSG("More than one FMAC per fabric port"), port));
    }

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_status_get(unit, port, pm_info, flags, status));
    } else {
        SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_STATUSr(unit, fmac_blk_ids[0], PM8X50_FABRIC_LANE_INDEX_IN_FMAC_GET(phy_index), &reg_val));
        status->prbs_lock = (reg_val >>  PM8X50_FABRIC_MAC_PRBS_LOCK_SHIFT) & PM8X50_FABRIC_MAC_PRBS_LOCK_MASK;
        status->prbs_lock_loss = 0; /*Not supported*/
        if (status->prbs_lock) {
            status->error_count = (reg_val >>  PM8X50_FABRIC_MAC_PRBS_CTR_SHIFT) & PM8X50_FABRIC_MAC_PRBS_CTR_MASK;
        } else {
            status->error_count = -1;
        }
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int
pm8x50_fabric_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, 
                                    phymod_phy_access_t* phy_access_arr, int* nof_phys, int* is_most_ext)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    int rv;
    uint32 phy_index, is_bypass, bitmap;
    SOCDNX_INIT_FUNC_DEFS;

    rv = PM8X50_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    if(max_phys < 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("array should be at size 1 at least")));
    }
    if(params->lane != 0 && params->lane != -1 && !is_bypass){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("fabric ports are single lane only")));
    }
    
    if(params->phyn != 0 && params->phyn != -1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("external phy is not supported")));
    }
    
    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access_arr[0]));

    *nof_phys = 1;

    if (is_bypass) {
        sal_memcpy(&phy_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access)); 
        if (params->lane == PORTMOD_ALL_LANES_ARE_ACTIVE) {
            phy_access_arr[0].access.lane_mask = 0xff & bitmap;
        } else {
            phy_access_arr[0].access.lane_mask = 0xff & (1 << (params->lane + phy_index));
            if (phy_access_arr[0].access.lane_mask == 0) {
                *nof_phys = 0;
            }
        }
        phy_access_arr[0].type = fabric_data->core_type;
    } else {
        sal_memcpy(&phy_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access)); 
        SHR_BITSET(&phy_access_arr[0].access.lane_mask, phy_index);
        phy_access_arr[0].type = fabric_data->core_type;
    }

    if(is_most_ext) {
        *is_most_ext = 1;
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


int pm8x50_fabric_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    SOCDNX_INIT_FUNC_DEFS;
    /*Do nothing*/
    SOCDNX_FUNC_RETURN;
}

int
pm8x50_fabric_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, 
                                phymod_core_access_t* core_access_arr, int* cores_num, int* is_most_ext)
{
    pm8x50_fabric_t fabric_data = pm_info->pm_data.pm8x50_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    if(max_cores < 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("array should be at size 1 at least")));
    }
    if(phyn != 0 && phyn != -1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("external phy is not supported")));
    }
    
    SOCDNX_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));
    sal_memcpy(&core_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access));
    core_access_arr[0].type = fabric_data->core_type;
    *cores_num = 1;

    if(is_most_ext) {
        *is_most_ext = 1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_mode_set (int unit, soc_port_t port, 
                     pm_info_t pm_info, const portmod_port_mode_info_t *mode)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("mode set isn't supported")));

exit:
    SOCDNX_FUNC_RETURN;  
}

int pm8x50_fabric_port_mode_get (int unit, soc_port_t port, 
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    SOCDNX_INIT_FUNC_DEFS;
    PM8X50_FABRIC_LEGACY_FIXME_ASSERT;

    mode->lanes = 1;
    mode->cur_mode = portmodPortModeSingle;
    
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    phymod_phy_access_t phy_access[1];
    int chain_length = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,
                                                        1, &chain_length));
    SOCDNX_IF_ERR_EXIT(portmod_port_phychain_cl72_set(unit, port, phy_access, chain_length, enable));

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    phymod_phy_access_t phy_access[1];
    int chain_length = 0;
    uint32_t tmp_enable;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,
                                                        1, &chain_length));
    SOCDNX_IF_ERR_EXIT(portmod_port_phychain_cl72_get(unit, phy_access, chain_length, &tmp_enable));
    *enable = tmp_enable;

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,uint32 inst, int op_type, 
    int op_cmd, const void *arg) 
{
    phymod_phy_access_t phy_access[1];
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_tx_t  ln_txparam[PHYMOD_MAX_LANES_PER_CORE];
    SOCDNX_INIT_FUNC_DEFS;
    PM8X50_FABRIC_LEGACY_FIXME_ASSERT;

    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));  
    params.phyn = 0;

    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,(BSL_META_U(unit, 
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"), unit, 
                port, PHY_DIAG_CTRL_DSC));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(phymod_phy_pmd_info_dump(phy_access, (void*)arg));
            }
            break;

        case PHY_DIAG_CTRL_PCS:
            LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"), unit, 
                port, PHY_DIAG_CTRL_PCS));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(phymod_phy_pcs_info_dump(phy_access, (void*)arg));
            }
            break;

        case PHY_DIAG_CTRL_LINKMON_MODE:
            LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_LINKMON_MODE  0x%x\n"), unit, port, 
                PHY_DIAG_CTRL_LINKMON_MODE));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(portmod_pm_phy_link_mon_enable_set(phy_access, nof_phys, PTR_TO_INT(arg)));
            }
            break;

       case PHY_DIAG_CTRL_LINKMON_STATUS:
            LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_LINKMON_STATUS 0x%x\n"), unit, 
                port, PHY_DIAG_CTRL_LINKMON_STATUS));
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(portmod_pm_phy_link_mon_status_get(phy_access, nof_phys));
            }
            break;

       default:
            SOCDNX_IF_ERR_EXIT(pm8x50_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            
            if(op_type == PHY_DIAG_CTRL_SET) {
                LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                    "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_SET 0x%x\n"),
                    unit, port, PHY_DIAG_CTRL_SET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !((*phy_access).access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_set(phy_access, nof_phys, 
                            op_cmd, ln_txparam, (*phy_access).access.lane_mask, PTR_TO_INT(arg)));
                    }
                }
            } else if(op_type == PHY_DIAG_CTRL_GET) {
                LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                    "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_GET 0x%x\n"),
                    unit, port, PHY_DIAG_CTRL_GET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !((*phy_access).access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_get(phy_access,nof_phys,
                            op_cmd, ln_txparam, (*phy_access).access.lane_mask, (uint32 *)arg));
                    } else {
                        *(uint32 *)arg = 0;
                    }
                }
            } else {
                SOC_IF_ERROR_RETURN(SOC_E_UNAVAIL);
            }

            break;
    }

exit:
    SOCDNX_FUNC_RETURN;

}

int pm8x50_fabric_pm_bypass_set(int unit, pm_info_t pm_info, int bypass_enable)
{
    int     rv = 0;
    uint32  pm_is_active = 0, bypass_enable_get;

    SOCDNX_INIT_FUNC_DEFS;
    PM8X50_FABRIC_LEGACY_FIXME_ASSERT;

    rv = PM8X50_FABRIC_IS_BYPASSED_GET(unit, pm_info, &bypass_enable_get);
    SOCDNX_IF_ERR_EXIT(rv);

    if (bypass_enable_get != bypass_enable) {
        rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &pm_is_active);
        SOCDNX_IF_ERR_EXIT(rv);

        if(pm_is_active) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("can't chenge bypass mode for active pm")));
        }

        rv = PM8X50_FABRIC_IS_BYPASSED_SET(unit, pm_info, bypass_enable);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_polarity_set(int unit, int port, pm_info_t pm_info, const phymod_polarity_t* polarity)
{
    int rv, i, rx_polarity, tx_polarity;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < PM8X50_FABRIC_LANES_PER_CORE; i++) {
        rx_polarity = (polarity->rx_polarity >> i) & 0x1;
        tx_polarity = (polarity->tx_polarity >> i) & 0x1;

        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int pm8x50_fabric_port_polarity_get(int unit, int port, pm_info_t pm_info, phymod_polarity_t* polarity)
{
    int rv, i, rx_polarity, tx_polarity;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(polarity));

    for (i = 0; i < PM8X50_FABRIC_LANES_PER_CORE; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i); 
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);

        polarity->rx_polarity |= rx_polarity << i; 
        polarity->tx_polarity |= tx_polarity << i; 
    }

exit:
    SOCDNX_FUNC_RETURN;
}


#endif /* PORTMOD_PM8X50_FABRIC_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

