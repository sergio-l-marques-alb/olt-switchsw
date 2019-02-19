/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *
 */

#ifndef _PM_OS_ILKN_SHARED_H_
#define _PM_OS_ILKN_SHARED_H_

#define OS_ILKN_TOTAL_LANES_PER_CORE                (24)
#define OS_ILKN_MAX_NOF_INTERNAL_PORTS              (2)
#define OS_ILKN_MAX_ILKN_AGGREGATED_PMS             (6)
#define OS_ILKN_MAX_NOF_CHANNELS                    (256)

typedef struct pmOsIlkn_internal_s{
    portmod_pbmp_t phys;
    pm_info_t *pms; /*pm_info_t is also a pointer (on pm_info_s) so pms is actually pointer to an array of OS_ILKN_MAX_ILKN_AGGREGATED_PMS pointers*/
    int wm_high;
    int wm_low;
    uint32 core_clk_khz;
} pmOsIlkn_internal_t;

struct pmOsIlkn_s{
    int                 is_over_fabric_en;
    pmOsIlkn_internal_t ilkn_data[OS_ILKN_MAX_NOF_INTERNAL_PORTS];
};

typedef enum pmOsIlkn_wb_vars{ /*No need for isActive, isInitialized and isBypassed since they are treated from the scope of the relevant associated pms (pm4x25 / pm4x10 / pm4x10q etc).*/
    ports = 0,
    isPortOverFabric,
    physAligned,
    isPmAggregated,
    ilknData
}pmOsIlkn_wb_vars_t;

/* WB defines */
#define PM_ILKN_PORT_SET(unit, pm_info, ilkn_port, ilkn_core_id) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &ilkn_port, ilkn_core_id);
#define PM_ILKN_PORT_GET(unit, pm_info, ilkn_port, ilkn_core_id) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], ilkn_port, ilkn_core_id)

#define PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, is_port_over_fabric, ilkn_core_id) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isPortOverFabric], &is_port_over_fabric, ilkn_core_id);
#define PM_ILKN_IS_PORT_OVER_FABRIC_GET(unit, pm_info, is_port_over_fabric, ilkn_core_id) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isPortOverFabric], is_port_over_fabric, ilkn_core_id)

#define PM_ILKN_PHYS_ALIGNED_SET(unit, pm_info, phys_aligned, ilkn_core_id) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[physAligned], &phys_aligned, ilkn_core_id);
#define PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, phys_aligned, ilkn_core_id) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[physAligned], phys_aligned, ilkn_core_id)

#define PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, is_pm_aggregated, ilkn_core_id, agg_pm) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isPmAggregated], &is_pm_aggregated, ((ilkn_core_id*(OS_ILKN_MAX_ILKN_AGGREGATED_PMS))+agg_pm));
#define PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, is_pm_aggregated, ilkn_core_id, agg_pm) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isPmAggregated], is_pm_aggregated, ((ilkn_core_id*(OS_ILKN_MAX_ILKN_AGGREGATED_PMS))+agg_pm))

#define OS_ILKN_CORE_ID_GET(port, pm_info, core_id)                                           \
    do {                                                                                      \
        int port0, port1;                                                                     \
        _SOC_IF_ERR_EXIT(PM_ILKN_PORT_GET(unit, pm_info, &port0, 0));                         \
        _SOC_IF_ERR_EXIT(PM_ILKN_PORT_GET(unit, pm_info, &port1, 1));                         \
        core_id = (port0 == port) ? 0 : ((port1 == port) ? 1 : -1);                           \
        if (core_id == -1) {                                                                  \
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("port %d is not attached"), port));     \
            SOC_EXIT;                                                                         \
        }                                                                                     \
    } while(0)

#define OS_ILKN_IS_PM_SHARED_GET(port, pm_info, core_id, pm, is_other_pm_aggregated)                                         \
    do {                                                                                                                     \
        int other_ilkn_core_id, other_port;                                                                                  \
        *is_other_pm_aggregated = 0;                                                                                         \
        other_ilkn_core_id = (~core_id) & 0x1;                                                                               \
        _SOC_IF_ERR_EXIT(PM_ILKN_PORT_GET(unit, pm_info, &other_port, other_ilkn_core_id));                                  \
        if (other_port != -1){                                                                                               \
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, is_other_pm_aggregated, other_ilkn_core_id, pm));   \
        }                                                                                                                    \
    } while(0)

#endif /*_PM_OS_ILKN_SHARED_H_*/

