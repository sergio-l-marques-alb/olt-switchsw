/*
 *
 * $Id:$
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>
#include <soc/portmod/portmod_legacy_phy.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM8X50_SUPPORT

#include <soc/portmod/cdmac.h>
#include <soc/portmod/pm8x50.h>
#include <soc/portmod/pm8x50_shared.h>


#define PM_8x50_INFO(pm_info) ((pm_info)->pm_data.pm8x50_db)

/* Warmboot variable defines - start */

#define PM8x50_WB_BUFFER_VERSION        (1)

#define PM8x50_IS_CORE_INITIALIZED_SET(unit, pm_info, is_core_initialized) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isCoreInitialized],         \
                          &is_core_initialized)
#define PM8x50_IS_CORE_INITIALIZED_GET(unit, pm_info, is_core_initialized) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[isCoreInitialized],         \
                          is_core_initialized)

#define PM8x50_IS_ACTIVE_SET(unit, pm_info, is_active)   \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isActive],\
                         &is_active)
#define PM8x50_IS_ACTIE_GET(unit, pm_info, is_active)    \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,   \
                          pm_info->wb_vars_ids[isActive],\
                          is_active)

#define PM8x50_IS_BYPASSED_SET(unit, pm_info, is_bypassed)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,      \
                          pm_info->wb_vars_ids[isBypassed], \
                          &is_bypassed)
#define PM8x50_IS_BYPASSED_GET(unit, pm_info, is_bypassed)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,      \
                          pm_info->wb_vars_ids[isBypassed], \
                          is_bypassed)

#define PM8x50_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll0ActiveLaneBitmap],             \
                          &pll0_active_lane_bitmap)
#define PM8x50_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, pll0_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll0ActiveLaneBitmap],             \
                          pll0_active_lane_bitmap)

#define PM8x50_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll1ActiveLaneBitmap],             \
                          &pll1_active_lane_bitmap)
#define PM8x50_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, pll1_active_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                            \
                          pm_info->wb_vars_ids[pll1ActiveLaneBitmap],             \
                          pll1_active_lane_bitmap)

#define PM8x50_PLL0_ADV_LANE_BITMAP_SET(unit, pm_info, pll0_adv_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[pll0AdvLaneBitmap],          \
                          &pll0_adv_lane_bitmap)
#define PM8x50_PLL0_ADV_LANE_BITMAP_GET(unit, pm_info, pll0_adv_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[pll0AdvLaneBitmap],          \
                          pll0_adv_lane_bitmap)

#define PM8x50_PLL1_ADV_LANE_BITMAP_SET(unit, pm_info, pll1_adv_lane_bitmap)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[pll1AdvLaneBitmap],          \
                          &pll1_adv_lane_bitmap)
#define PM8x50_PLL1_ADV_LANE_BITMAP_GET(unit, pm_info, pll1_adv_lane_bitmap)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[pll1AdvLaneBitmap],          \
                          pll1_adv_lane_bitmap)

#define PM8x50_LANE2PORT_SET(unit, pm_info, lane, port)                  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                   \
                          pm_info->wb_vars_ids[lane2portMap], &port, lane)
#define PM8x50_LANE2PORT_GET(unit, pm_info, lane, port)                 \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,                  \
                          pm_info->wb_vars_ids[lane2portMap], port, lane)

#define PM8x50_MAX_SPEED_SET(unit, pm_info, max_speed, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                          \
                          pm_info->wb_vars_ids[maxSpeed], &max_speed, port_index)
#define PM8x50_MAX_SPEED_GET(unit, pm_info, max_speed, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                          pm_info->wb_vars_ids[maxSpeed], max_speed, port_index)

#define PM8x50_AN_MODE_SET(unit, pm_info, an_mode, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[anMode], &an_mode, port_index)
#define PM8x50_AN_MODE_GET(unit, pm_info, an_mode, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[anMode], an_mode, port_index)

#define PM8x50_AN_FEC_SET(unit, pm_info, an_fec, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                    \
                          pm_info->wb_vars_ids[anFec], &an_fec, port_index)
#define PM8x50_AN_FEC_GET(unit, pm_info, an_fec, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                   \
                          pm_info->wb_vars_ids[anFec], an_fec, port_index)

#define PM8x50_FS_CL72_SET(unit, pm_info, fs_cl72, port_index)              \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                      \
                          pm_info->wb_vars_ids[fsCl72], &fs_cl72, port_index)
#define PM8x50_FS_CL72_GET(unit, pm_info, fs_cl72, port_index)             \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                     \
                          pm_info->wb_vars_ids[fsCl72], fs_cl72, port_index)

typedef enum pm8x50_wb_vars{
    isCoreInitialized,
    isActive,
    isBypassed,
    pll0ActiveLaneBitmap,
    pll1ActiveLaneBitmap,
    pll0AdvLaneBitmap,
    pll1AdvLaneBitmap,
    lane2portMap,
    maxSpeed,
    anMode,
    anFec,
    fsCl72
}pm8x50_wb_vars_t;

/* Warmboot variable defines - end */


STATIC
int _pm8x50_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
#if 0
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < MAX_PORTS_PER_PM8X50; i++){
       rv = PM8x50_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
       _SOC_IF_ERR_EXIT(rv);

       if(tmp_port == port){
           *first_index = (*first_index == -1 ? i : *first_index);
           SHR_BITSET(bitmap, i);
       }
   }

   if(*first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("port was not found in internal DB %d"), port));
   }

exit:
    SOC_FUNC_RETURN;
#endif
    return 0;
}
/*Get whether the inerface type is supported by PM */
int pm8x50_pm_interface_type_is_supported(int unit,
                                          soc_port_if_t interface,
                                          int* is_supported)
{

    *is_supported = TRUE;
    return (SOC_E_NONE);
}

STATIC
int
pm8x50_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr,
                         uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CDPORT_TSC_UCMEM_DATAm, user_acc,
                                             core_addr, reg_addr, val);
}

STATIC
int
pm8x50_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr,
                        uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CDPORT_TSC_UCMEM_DATAm, user_acc,
                                            core_addr, reg_addr, val);
}

/*
 * Function:
 *      portmod_pm8x50_wb_upgrade_func
 * Purpose:
 *      This function will take care of the warmboot variable manipulation
 *      in case of upgrade case, when variable definition got change or
 *      unspoorted varaible from previous version.
 * Parameters:
 *      unit              -(IN) Device unit number .
 *      arg               -(IN) Generic pointer for a specific module to be used
 *      recovered_version -(IN) Warmboot version of the existing data.
 *      new_version       -(IN) Warmboot version of new data.
 * Returns:
 *      Status
 */

int portmod_pm8x50_wb_upgrade_func(int unit, void *arg, int recovered_version,
                                   int new_version)
{
    return (SOC_E_NONE);
}

/*
 * Initialize the buffer to support warmboot
 * The state of warmboot is store in the variables like
 * isInitialized, isActive, isBypassed, ports.. etc.,
 * All of these variables need to be added to warmboot
 * any variables added to save the state of warmboot should be
 * included here.
 */
STATIC
int pm8x50_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm8x50",
                           portmod_pm8x50_wb_upgrade_func, pm_info,
                           PM8x50_WB_BUFFER_VERSION, 1,
                           SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                         "is_core_initialized", wb_buffer_index, sizeof(int),
                          NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isCoreInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active",
                          wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_bypassed",
                          wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isBypassed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll0_active_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll0ActiveLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll1_active_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll1ActiveLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll0_adv_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll0AdvLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll1_adv_lane_bitmap",
                          wb_buffer_index, sizeof(uint8), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll1AdvLaneBitmap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2portMap", 
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2portMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "max_speed", 
                          wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[maxSpeed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_mode",
                          wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_fec",
                          wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anFec] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "fs_cl72",
                          wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM8X50, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[fsCl72] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD,
                                               wb_buffer_index, FALSE));

exit:
    SOC_FUNC_RETURN;
}

phymod_bus_t pm8x50_default_bus = {
    "PM8x50 Bus",
    pm8x50_default_bus_read,
    pm8x50_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

/*Add new pm.*/
int pm8x50_pm_init(int unit,
                   const portmod_pm_create_info_internal_t* pm_add_info,
                   int wb_buffer_index,
                   pm_info_t pm_info)
{
    const portmod_pm8x50_create_info_t *info =
                &pm_add_info->pm_specific_info.pm8x50;
    pm8x50_t pm8x50_data = NULL;
    int i, rv;
    int bypass_enable;
    int is_core_initialized;
    SOC_INIT_FUNC_DEFS;

    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    /* PM8x50 specific info */
    pm8x50_data = sal_alloc(sizeof(struct pm8x50_s), "specific_db");
    SOC_NULL_CHECK(pm8x50_data);
    pm_info->pm_data.pm8x50_db = pm8x50_data;

    PORTMOD_PBMP_ASSIGN(pm8x50_data->phys, pm_add_info->phys);
    pm8x50_data->int_core_access.type = phymodDispatchTypeCount;
    pm8x50_data->first_phy = -1;
    pm8x50_data->warmboot_skip_db_restore = TRUE;

    for(i = 0 ; i < PM8X50_LANES_PER_CORE; i++){
        pm8x50_data->nof_phys[i] = 0;
    }

    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm8x50_data->int_core_access);

    sal_memcpy(&pm8x50_data->polarity, &info->polarity,
               sizeof(phymod_polarity_t));
    sal_memcpy(&(pm8x50_data->int_core_access.access), &info->access,
                sizeof(phymod_access_t));

    sal_memcpy(&pm8x50_data->lane_map, &info->lane_map,
                sizeof(pm8x50_data->lane_map));
    pm8x50_data->ref_clk = info->ref_clk;
    pm8x50_data->fw_load_method = info->fw_load_method;
    pm8x50_data->external_fw_loader = info->external_fw_loader;

    if(info->access.access.bus == NULL) {
        /* if null - use default */
        pm8x50_data->int_core_access.access.bus = &pm8x50_default_bus;
    }

#ifdef FIXME
    if(pm8x50_data->external_fw_loader == NULL) {
        /* if null - use default */
        pm8x50_data->external_fw_loader = pm8x50_default_fw_loader;
    }
#endif

    for(i = 0 ; i < PM8X50_LANES_PER_CORE; i++){
        pm8x50_data->nof_phys[i] = 1; /* Internal Phy */
    }

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm8x50_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if(!SOC_WARM_BOOT(unit)){
        is_core_initialized = 0;
        rv = PM8x50_IS_CORE_INITIALIZED_SET(unit, pm_info,
                                            is_core_initialized);
        _SOC_IF_ERR_EXIT(rv);

        bypass_enable = 0;
        rv = PM8x50_IS_BYPASSED_SET(unit, pm_info, bypass_enable);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    if(SOC_FUNC_ERROR){
        pm8x50_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

/*Release PM resources*/
int pm8x50_pm_destroy(int unit, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    if(pm_info->pm_data.pm8x50_db != NULL){
        sal_free(pm_info->pm_data.pm8x50_db);
        pm_info->pm_data.pm8x50_db = NULL;
    }

    SOC_FUNC_RETURN;
}

/*Get PM8x50 specific capabilites*/
int pm8x50_pm_specific_capability_get(int unit,
                                      portmod_pm_capability_t* pm_cap)
{
    SOC_INIT_FUNC_DEFS;

    if(pm_cap == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("pm_cap NULL paramaeter")));
    }

    portmod_pm8x50_capability_t_init(unit,
                                     &pm_cap->pm_capability.pm8x50_cap);
    pm_cap->pm_capability.pm8x50_cap.vcos[0] = portmodVCO20G;
    pm_cap->pm_capability.pm8x50_cap.vcos[1] = portmodVCO25G;
    pm_cap->pm_capability.pm8x50_cap.vcos[2] = portmodVCO26G;

exit:
    SOC_FUNC_RETURN;
}

/*Get the VCO values for the list of speed-ids that need to be supported*/
int pm8x50_pm_specific_vcos_get(int unit, const portmod_pm_capability_t* pm_cap, portmod_pm_vco_setting_t* vco_select)
{
    SOC_INIT_FUNC_DEFS;

    if(vco_select == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("vco_select NULL paramaeter")));
    }

    /* TBD - to parse the speed list and return the tvco/ovco */
    vco_select->tvco = portmodVCO26G;
    vco_select->ovco = portmodVCO25G;

exit:
    SOC_FUNC_RETURN;
}

/*Enable port macro.*/

int pm8x50_pm_enable(int unit,
                     int port,
                     pm_info_t pm_info, 
                     int port_mode, 
                     const portmod_port_add_info_t* add_info, 
                     int enable)
{
    return (SOC_E_NONE);
}

/*PM Core init routine*/
int pm8x50_port_pm_core_init(int unit,
                             pm_info_t pm_info,
                             int pport,
                             int lport,
                             uint32 flags,
                             const portmod_port_add_info_t* add_info)
{
    uint32 rval;
    int is_core_initialized = 0;
    int rv;
    SOC_INIT_FUNC_DEFS;
    /* TBD - cdport power save disable */

    /*
     * bring both the iinstances of CDMAC out of reset
     * TBD - using default settngs for now
     */
    _SOC_IF_ERR_EXIT(READ_CDPORT_MAC_CONTROLr(unit, lport, &rval));
    soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &rval, CDMAC0_RESETf, 0);
    soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &rval, CDMAC1_RESETf, 0);
    _SOC_IF_ERR_EXIT(WRITE_CDPORT_MAC_CONTROLr(unit, lport, rval));

    /*
     * TBD
     * Bring TSC out of reset.
     * call phymod_core_init
     */
   is_core_initialized = 1;
   rv = PM8x50_IS_CORE_INITIALIZED_SET(unit, pm_info, is_core_initialized);
   _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

/*Add new port*/
int pm8x50_port_attach(int unit, int port, pm_info_t pm_info,
                       const portmod_port_add_info_t* add_info)
{
    uint32 flags = CDMAC_NULL_FLAGS;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_init(unit, port, flags));

exit:
    SOC_FUNC_RETURN;
}

/*Validate a set of speed config within a port macro.*/
int pm8x50_pm_speed_config_validate(int unit,
                                    int port,
                                    pm_info_t pm_info,
                                    const portmod_pbmp_t* phys,
                                    int flag,
                                    portmod_pm_vco_setting_t* vco_setting)
{
    return (SOC_E_NONE);
}

/*Port speed validation.*/
int pm8x50_port_speed_config_validate(int unit,
                                      int port,
                                      const portmod_speed_config_t* speed_config,
                                      portmod_pbmp_t* affected_pbmp)
{
    return (SOC_E_NONE);
}
/*Set PM in bypass mode. should be called in the aggregator code.*/
int pm8x50_pm_bypass_set(int unit, pm_info_t pm_info, int enable)
{

    return (SOC_E_NONE);
}

/*get port cores' phymod access*/
int pm8x50_pm_core_info_get(int unit, pm_info_t pm_info, int phyn,
                            portmod_pm_core_info_t* core_info)
{

    return (SOC_E_NONE);
}

/*Get PM phys.*/
int pm8x50_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{

    return (SOC_E_NONE);
}

/*Port remove in PM level*/
int pm8x50_port_detach(int unit, int port, pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

/*Port replace in PM level*/
int pm8x50_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{

    return (SOC_E_NONE);
}

/*Port enable*/
int pm8x50_port_enable_set(int unit, int port, pm_info_t pm_info,
                           int flags, int enable)
{
    int actual_flags = flags;
    int cdmac_flags = 0;
    uint32 is_bypassed = 0;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys = 0;
    SOC_INIT_FUNC_DEFS;

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) &&
       (!PORTMOD_PORT_ENABLE_RX_GET(flags))) {
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) &&
       (!PORTMOD_PORT_ENABLE_MAC_GET(flags))) {
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    /*
     * if MAC is set and having only either RX or TX set
     * is invalid combination
     */
    if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) &&
       (!PORTMOD_PORT_ENABLE_PHY_GET(actual_flags))) {

        if((!PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) ||
           (!PORTMOD_PORT_ENABLE_RX_GET(actual_flags))) {
           _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("MAC RX and TX can't be enabled separately")));
        }
    }

    if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        /* Only internal phy supported. */
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port,
                                     pm_info, &params, 1, &phy_access,
                                     &nof_phys, NULL));
    }


    _SOC_IF_ERR_EXIT(PM8x50_IS_BYPASSED_GET(unit, pm_info, &is_bypassed));

    if(enable){
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)) {
            _SOC_IF_ERR_EXIT(cdmac_enable_set(unit, port, cdmac_flags, 1));
        }

        if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
            if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT(
                    phymod_phy_tx_lane_control_set(&phy_access,
                                                   phymodTxSquelchOff));
            }
            if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT(
                    phymod_phy_rx_lane_control_set(&phy_access,
                                                   phymodRxSquelchOff));
            }
        } /* PORTMOD_PORT_ENABLE_PHY_GET */
    } else {/* disable */
        if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {

            if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT(
                    phymod_phy_tx_lane_control_set(&phy_access,
                                                   phymodTxSquelchOn));
            }

            if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT(
                    phymod_phy_rx_lane_control_set(&phy_access,
                                                   phymodRxSquelchOn));
            }
        }

        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))  && (!is_bypassed)) {
            _SOC_IF_ERR_EXIT(cdmac_enable_set(unit, port, cdmac_flags, 0));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_enable_get(int unit, int port, pm_info_t pm_info,
                           int flags, int* enable)
{
    int nof_phys = 0;
    int phy_enable = 1, mac_enable = 1;
    int mac_rx_enable = 0, mac_tx_enable = 0;
    uint32 is_bypassed = 0;
    int actual_flags = flags;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    phymod_phy_tx_lane_control_t tx_control = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_control = phymodRxSquelchOn;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) &&
       (!PORTMOD_PORT_ENABLE_RX_GET(flags))) {
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) &&
       (!PORTMOD_PORT_ENABLE_MAC_GET(flags))) {
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        /* Only internal phy supported. */
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        _SOC_IF_ERR_EXIT(pm8x50_port_phy_lane_access_get(unit, port,
                                     pm_info, &params, 1, &phy_access,
                                     &nof_phys, NULL));
    }

    _SOC_IF_ERR_EXIT(PM8x50_IS_BYPASSED_GET(unit, pm_info, &is_bypassed));

    if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(phymod_phy_tx_lane_control_get(&phy_access,
                                                        &tx_control));
        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_get(&phy_access,
                                                        &rx_control));

        /* TBD - set phy_enable to 1 for SIM */
        phy_enable = (SAL_BOOT_SIMULATION)? 1: 0;

        if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
            phy_enable |= (rx_control == phymodRxSquelchOn) ? 0 : 1;
        }
        if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
            phy_enable |= (tx_control == phymodTxSquelchOn) ? 0 : 1;
        }
    }

    if ((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)) {
        mac_enable = 0;

        if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT(cdmac_enable_get(unit, port,
                                              CDMAC_ENABLE_SET_FLAGS_RX_EN,
                                              &mac_rx_enable));
            mac_enable |= (mac_rx_enable)? 1: 0;
        }

        if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT(cdmac_enable_get(unit, port,
                                              CDMAC_ENABLE_SET_FLAGS_TX_EN,
                                              &mac_tx_enable));
            mac_enable |= (mac_tx_enable)? 1: 0;
        }
    }

    *enable = (mac_enable && phy_enable);

exit:
    SOC_FUNC_RETURN;
}

/*Reconfigure the vco rate for pm core.*/
int pm8x50_pm_vco_reconfig(int unit, int port, pm_info_t pm_info, int is_serdes_reset, int num_vco, const portmod_vco_type_t* vco)
{
    return (SOC_E_NONE);
}

/* set/get the speed config for the specified port.*/
int pm8x50_port_speed_config_set(int unit, int port, pm_info_t pm_info, const portmod_speed_config_t* speed_config)
{
    return (SOC_E_NONE);
}

int pm8x50_port_speed_config_get(int unit, int port, pm_info_t pm_info, portmod_speed_config_t* speed_config)
{
    return (SOC_E_NONE);
}

/*  set/get the interface, speed and encapsulation for the specified port.
 *  For the First port within core this function initialize the PM, MAC 
 *  and PHY before configure the port.
 */
int pm8x50_port_interface_config_set(int unit, int port, pm_info_t pm_info,
                                 const portmod_port_interface_config_t* config,
                                 int all_phy)
{

    return (SOC_E_NONE);
}
int pm8x50_port_interface_config_get(int unit, int port, pm_info_t pm_info,
                                     portmod_port_interface_config_t* config,
                                     int phy_init_flags)
{

    return (SOC_E_NONE);
}

/*Port cl72 set\get*/
int pm8x50_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{

    return (SOC_E_NONE);
}

/*Get port cl72 status*/
int pm8x50_port_cl72_status_get(int unit, int port, pm_info_t pm_info,
                                phymod_cl72_status_t* status)
{

    return (SOC_E_NONE);
}

/*Port speed set\get*/
int pm8x50_port_loopback_set(int unit, int port, pm_info_t pm_info,
                             portmod_loopback_mode_t loopback_type, int enable)
{
    /*int port_index;
    uint32 bitmap;*/

    SOC_INIT_FUNC_DEFS;

    /*_SOC_IF_ERR_EXIT(_pm8x50_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));*/

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(cdmac_loopback_set(unit, port, loopback_type, enable));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
                  _SOC_MSG("unsupported loopback type %d"), loopback_type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_loopback_get(int unit, int port, pm_info_t pm_info,
                             portmod_loopback_mode_t loopback_type,
                             int* enable)
{
    /*int port_index;
    uint32 bitmap;*/

    SOC_INIT_FUNC_DEFS;

    /*_SOC_IF_ERR_EXIT(_pm8x50_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));*/

    switch(loopback_type){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(cdmac_loopback_get(unit, port, loopback_type, enable));
            break;
        default:
            (*enable) = 0; /* not supported --> no loopback */
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port RX MAC ENABLE set\get*/
int pm8x50_port_rx_mac_enable_set(int unit, int port,
                                  pm_info_t pm_info, int enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_rx_mac_enable_get(int unit, int port,
                                  pm_info_t pm_info, int* enable)
{

    return (SOC_E_NONE);
}

/*Port TX MAC ENABLE set\get*/
int pm8x50_port_tx_mac_enable_set(int unit, int port,
                                  pm_info_t pm_info,
                                  int enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_tx_mac_enable_get(int unit, int port,
                                  pm_info_t pm_info,
                                  int* enable)
{

    return (SOC_E_NONE);
}

/*get port auto negotiation local ability*/
int pm8x50_port_ability_local_get(int unit, int port, pm_info_t pm_info,
                                  uint32 phy_flags,
                                  portmod_port_ability_t* ability)
{

    return (SOC_E_NONE);
}

/*Set\Get autoneg*/
int pm8x50_port_autoneg_set(int unit, int port, pm_info_t pm_info,
                            uint32 phy_flags,
                            const phymod_autoneg_control_t* an)
{

    return (SOC_E_NONE);
}
int pm8x50_port_autoneg_get(int unit, int port, pm_info_t pm_info,
                            uint32 phy_flags,
                            phymod_autoneg_control_t* an)
{

    return (SOC_E_NONE);
}

/*Get autoneg status*/
int pm8x50_port_autoneg_status_get(int unit, int port, pm_info_t pm_info,
                                   phymod_autoneg_status_t* an_status)
{

    return (SOC_E_NONE);
}

/*get link status*/
int pm8x50_port_link_get(int unit, int port, pm_info_t pm_info,
                         int flags, int* link)
{

    return (SOC_E_NONE);
}

/*get latch down link status (whether link was ever down since last clear)*/
int pm8x50_port_link_latch_down_get(int unit, int port,
                                    pm_info_t pm_info,
                                    uint32 flags, int* link)
{

    return (SOC_E_NONE);
}

/* link up event */
int pm8x50_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

/* link down event */
int pm8x50_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

/*PRBS configuration set/get*/
int pm8x50_port_prbs_config_set(int unit, int port, pm_info_t pm_info,
                                portmod_prbs_mode_t mode, int flags,
                                const phymod_prbs_t* config)
{

    return (SOC_E_NONE);
}
int pm8x50_port_prbs_config_get(int unit, int port, pm_info_t pm_info,
                                portmod_prbs_mode_t mode, int flags,
                                phymod_prbs_t* config)
{

    return (SOC_E_NONE);
}

/*PRBS enable set/get*/
int pm8x50_port_prbs_enable_set(int unit, int port, pm_info_t pm_info,
                                portmod_prbs_mode_t mode, int flags,
                                int enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_prbs_enable_get(int unit, int port, pm_info_t pm_info,
                                portmod_prbs_mode_t mode, int flags,
                                int* enable)
{

    return (SOC_E_NONE);
}

/*PRBS status get*/
int pm8x50_port_prbs_status_get(int unit, int port, pm_info_t pm_info,
                                portmod_prbs_mode_t mode, int flags,
                                phymod_prbs_status_t* status)
{

    return (SOC_E_NONE);
}

/*Number of lanes get*/
int pm8x50_port_nof_lanes_get(int unit, int port, pm_info_t pm_info,
                              int* nof_lanes)
{

    return (SOC_E_NONE);
}

/*Set port PHYs' firmware mode*/
int pm8x50_port_firmware_mode_set(int unit, int port, pm_info_t pm_info,
                                  phymod_firmware_mode_t fw_mode)
{

    return (SOC_E_NONE);
}
int pm8x50_port_firmware_mode_get(int unit, int port, pm_info_t pm_info,
                                  phymod_firmware_mode_t* fw_mode)
{

    return (SOC_E_NONE);
}

/*Filter packets smaller than the specified threshold*/
int pm8x50_port_runt_threshold_set(int unit, int port,
                                   pm_info_t pm_info,
                                   int value)
{

    return (SOC_E_NONE);
}
int pm8x50_port_runt_threshold_get(int unit, int port, pm_info_t pm_info,
                                   int* value)
{

    return (SOC_E_NONE);
}

/*Filter packets bigger than the specified value*/
int pm8x50_port_max_packet_size_set(int unit, int port, pm_info_t pm_info,
                                    int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_rx_max_size_set(unit, port , value));
exit:
    SOC_FUNC_RETURN;
}
int pm8x50_port_max_packet_size_get(int unit, int port, pm_info_t pm_info,
                                    int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_rx_max_size_get(unit, port , value));
exit:
    SOC_FUNC_RETURN;
}

/*
 * TX pad packets to the specified size.
 * values smaller than 17 means pad is disabled.
 */
int pm8x50_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_pad_size_set(unit, port , value));

exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    _SOC_IF_ERR_EXIT(cdmac_pad_size_get(unit, port , value));

exit:
    SOC_FUNC_RETURN;
}

/*set/get the MAC source address that will be sent in case of Pause/LLFC*/
int pm8x50_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info,
                              sal_mac_addr_t mac_sa)
{

    return (SOC_E_NONE);
}
int pm8x50_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info,
                              sal_mac_addr_t mac_sa)
{

    return (SOC_E_NONE);
}

/* set/get SA recognized for MAC control packets
 * in addition to the standard 0x0180C2000001
 */
int pm8x50_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info,
                              sal_mac_addr_t mac_sa)
{

    return (SOC_E_NONE);
}
int pm8x50_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info,
                              sal_mac_addr_t mac_sa)
{

    return (SOC_E_NONE);
}

/*set/get Average inter-packet gap*/
int pm8x50_port_tx_average_ipg_set(int unit, int port, pm_info_t pm_info,
                                   int value)
{

    return (SOC_E_NONE);
}
int pm8x50_port_tx_average_ipg_get(int unit, int port, pm_info_t pm_info,
                                   int* value)
{

    return (SOC_E_NONE);
}

/*local fault set/get*/
int pm8x50_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                                 const portmod_local_fault_control_t* control)
{

    return (SOC_E_NONE);
}
int pm8x50_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                                      portmod_local_fault_control_t* control)
{

    return (SOC_E_NONE);
}

/*remote fault set/get*/
int pm8x50_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                                 const portmod_remote_fault_control_t* control)
{

    return (SOC_E_NONE);
}
int pm8x50_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                                     portmod_remote_fault_control_t* control)
{

    return (SOC_E_NONE);
}

/*local fault steatus get*/
int pm8x50_port_local_fault_status_get(int unit, int port, pm_info_t pm_info,
                                       int* value)
{

    return (SOC_E_NONE);
}

/*remote fault status get*/
int pm8x50_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info,
                                        int* value)
{

    return (SOC_E_NONE);
}

/*local fault steatus clear*/
int pm8x50_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

/*remote fault status clear*/
int pm8x50_port_remote_fault_status_clear(int unit, int port,
                                          pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

int pm8x50_port_pause_control_set(int unit, int port, pm_info_t pm_info,
                                  const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pause_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pause_control_get(int unit, int port, pm_info_t pm_info,
                                  portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pause_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pfc_control_set(int unit, int port, pm_info_t pm_info,
                                const portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm8x50_port_pfc_control_get(int unit, int port, pm_info_t pm_info,
                                portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cdmac_pfc_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

/*set/get LLFC FC configuration*/
int pm8x50_port_llfc_control_set(int unit, int port, pm_info_t pm_info,
                                 const portmod_llfc_control_t* control)
{

    return (SOC_E_NONE);
}
int pm8x50_port_llfc_control_get(int unit, int port, pm_info_t pm_info,
                                 portmod_llfc_control_t* control)
{

    return (SOC_E_NONE);
}

/*Routine for MAC\PHY sync.*/
int pm8x50_port_update(int unit, int port, pm_info_t pm_info,
                       const portmod_port_update_control_t* update_control)
{

    return (SOC_E_NONE);
}

/*get port cores' phymod access*/
int pm8x50_port_core_access_get(int unit, int port, pm_info_t pm_info,
                                int phyn, int max_cores,
                                phymod_core_access_t* core_access_arr,
                                int* nof_cores, int* is_most_ext)
{

    return (SOC_E_NONE);
}

/*Get lane phymod access structure. can be used for per lane operations*/
int pm8x50_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
                                    const portmod_access_get_params_t* params,
                                    int max_phys,
                                    phymod_phy_access_t* phy_access,
                                    int* nof_phys, int* is_most_ext)
{
    int phyn = 0, rv;
    uint32  iphy_lane_mask = 0;
    int port_index=0;
    int serdes_lane=0;
    int i;
    uint32 pm_is_bypassed = 0;

    SOC_INIT_FUNC_DEFS;

    if(max_phys > MAX_NUM_CORES)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("max_phys parameter exceeded the "
                          "MAX value. max_phys=%d, max allowed %d."),
                          max_phys, MAX_NUM_CORES));
    }

    /* get port index and lane mask */
    _SOC_IF_ERR_EXIT(_pm8x50_port_index_get(unit, port, pm_info, &port_index,
                                            &iphy_lane_mask));

    rv = PM8x50_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    if(params->phyn >= PM_8x50_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
               PM_8x50_INFO(pm_info)->nof_phys[port_index] - 1, params->phyn));
    }

    for( i = 0 ; i < max_phys; i++) {
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    phyn = params->phyn;

    /* if phyn is -1, it is looking for outer most phy.
       assumption is that all lane has same phy depth. */
    if(phyn < 0) {
        phyn = PM_8x50_INFO(pm_info)->nof_phys[port_index] - 1;
    }


    if( phyn == 0 ) {
        *nof_phys = 1;
        /* internal core */
        sal_memcpy (&phy_access[0], &(PM_8x50_INFO(pm_info)->int_core_access),
                    sizeof(phymod_phy_access_t));
        phy_access[0].access.lane_mask = iphy_lane_mask;
        if (params->lane != -1) {
            serdes_lane = port_index + params->lane;
            phy_access[0].access.lane_mask &= (0x1 << serdes_lane );
            if (phy_access[0].access.lane_mask == 0) {
                *nof_phys = 0;
            }
        }
        /* only line is availabe for internal. */
        phy_access[0].port_loc = phymodPortLocLine;

        /* if it is warm boot, get probed information from wb db instead of re-probing. */
    }

    /*
     * assumption is that all logical port have same phy depths, that
     * will not true when simplex introduced , until then we can keep
     * this code.
     */
    if (is_most_ext) {
        if (phyn == PM_8x50_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/*Port duplex set\get*/
int pm8x50_port_duplex_set(int unit, int port, pm_info_t pm_info, int enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{

    return (SOC_E_NONE);
}

/*Port PHY Control register read*/
int pm8x50_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int flags,
                             int reg_addr, uint32* value)
{

    return (SOC_E_NONE);
}

/*Port PHY Control register write*/
int pm8x50_port_phy_reg_write(int unit, int port, pm_info_t pm_info,
                              int flags, int reg_addr, uint32 value)
{

    return (SOC_E_NONE);
}

/*Port Reset set\get*/
int pm8x50_port_reset_set(int unit, int port,
                          pm_info_t pm_info, int mode,
                          int opcode, int value)
{

    return (SOC_E_NONE);
}
int pm8x50_port_reset_get(int unit, int port,
                          pm_info_t pm_info, int mode,
                          int opcode, int* value)
{

    return (SOC_E_NONE);
}

/*Port remote Adv get*/
int pm8x50_port_adv_remote_get(int unit, int port,
                               pm_info_t pm_info,
                               int* value)
{

    return (SOC_E_NONE);
}

/*Drv Name Get*/
int pm8x50_port_drv_name_get(int unit, int port,
                             pm_info_t pm_info,
                             char* name, int len)
{

    return (SOC_E_NONE);
}

/*set/get port fec enable according to local/remote FEC ability*/
int pm8x50_port_fec_enable_set(int unit, int port, pm_info_t pm_info,
                               uint32_t enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_fec_enable_get(int unit, int port, pm_info_t pm_info,
                               uint32_t* enable)
{

    return (SOC_E_NONE);
}

/*get port auto negotiation local ability*/
int pm8x50_port_ability_advert_set(int unit, int port, pm_info_t pm_info,
                                   uint32 phy_flags,
                                   portmod_port_ability_t* ability)
{

    return (SOC_E_NONE);
}
int pm8x50_port_ability_advert_get(int unit, int port, pm_info_t pm_info,
                                   uint32 phy_flags,
                                   portmod_port_ability_t* ability)
{

    return (SOC_E_NONE);
}

/*Port ability remote Adv get*/
int pm8x50_port_ability_remote_get(int unit, int port, pm_info_t pm_info,
                                   uint32 phy_flags,
                                   portmod_port_ability_t* ability)
{

    return (SOC_E_NONE);
}

/*Port Mac Control Spacing Stretch*/
int pm8x50_port_frame_spacing_stretch_set(int unit, int port,
                                          pm_info_t pm_info, int spacing)
{

    return (SOC_E_NONE);
}
int pm8x50_port_frame_spacing_stretch_get(int unit, int port,
                                          pm_info_t pm_info,
                                          const int* spacing)
{

    return (SOC_E_NONE);
}

/*get port timestamps in fifo*/
int pm8x50_port_diag_fifo_status_get(int unit, int port, pm_info_t pm_info,
                                     const portmod_fifo_status_t* diag_info)
{

    return (SOC_E_NONE);
}

/*set/get pass control frames.*/
int pm8x50_port_rx_control_set(int unit, int port, pm_info_t pm_info,
                               const portmod_rx_control_t* rx_ctrl)
{

    return (SOC_E_NONE);
}
int pm8x50_port_rx_control_get(int unit, int port, pm_info_t pm_info,
                               portmod_rx_control_t* rx_ctrl)
{

    return (SOC_E_NONE);
}

/*set PFC config registers.*/
int pm8x50_port_pfc_config_set(int unit, int port, pm_info_t pm_info,
                               const portmod_pfc_config_t* pfc_cfg)
{

    return (SOC_E_NONE);
}
int pm8x50_port_pfc_config_get(int unit, int port, pm_info_t pm_info,
                               portmod_pfc_config_t* pfc_cfg)
{

    return (SOC_E_NONE);
}

/*set EEE Config.*/
int pm8x50_port_eee_set(int unit, int port, pm_info_t pm_info,
                        const portmod_eee_t* eee)
{

    return (SOC_E_UNAVAIL);
}
int pm8x50_port_eee_get(int unit, int port, pm_info_t pm_info,
                        portmod_eee_t* eee)
{

    return (SOC_E_UNAVAIL);
}

/*set EEE Config.*/
int pm8x50_port_eee_clock_set(int unit, int port, pm_info_t pm_info,
                              const portmod_eee_clock_t* eee_clk)
{

    return (SOC_E_NONE);
}
int pm8x50_port_eee_clock_get(int unit, int port, pm_info_t pm_info,
                              portmod_eee_clock_t* eee_clk)
{

    return (SOC_E_NONE);
}

/*set Vlan Inner/Outer tag.*/
int pm8x50_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,
                             const portmod_vlan_tag_t* vlan_tag)
{

    return (SOC_E_NONE);
}
int pm8x50_port_vlan_tag_get(int unit, int port, pm_info_t pm_info,
                             portmod_vlan_tag_t* vlan_tag)
{

    return (SOC_E_NONE);
}

/*set modid field.*/
int pm8x50_port_modid_set(int unit, int port, pm_info_t pm_info, int value)
{

    return (SOC_E_NONE);
}

/*set modid field.*/
int pm8x50_port_led_chain_config(int unit, int port, pm_info_t pm_info,
                                 int value)
{

    return (SOC_E_NONE);
}

/*set modid field.*/
int pm8x50_port_clear_rx_lss_status_set(int unit, int port, pm_info_t pm_info,
                                        int lcl_fault, int rmt_fault)
{

    return (SOC_E_NONE);
}
int pm8x50_port_clear_rx_lss_status_get(int unit, int port, pm_info_t pm_info,
                                        int* lcl_fault, int* rmt_fault)
{

    return (SOC_E_NONE);
}

/*Attaches an external phy lane to a specific port macro*/
int pm8x50_xphy_lane_attach_to_pm(int unit,
                         pm_info_t pm_info, int iphy, int phyn,
                         const portmod_xphy_lane_connection_t* lane_connection)
{

    return (SOC_E_NONE);
}

/*Attaches an external phy lane to a specific port macro*/
int pm8x50_xphy_lane_detach_from_pm(int unit,
                               pm_info_t pm_info, int iphy, int phyn,
                               portmod_xphy_lane_connection_t* lane_connection)
{

    return (SOC_E_NONE);
}

/*Toggle Lag Failover Status.*/
int pm8x50_port_lag_failover_status_toggle(int unit, int port,
                                           pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

/*Toggle Lag Failover loopback set / get.*/
int pm8x50_port_lag_failover_loopback_set(int unit, int port,
                                          pm_info_t pm_info,
                                          int value)
{

    return (SOC_E_NONE);
}
int pm8x50_port_lag_failover_loopback_get(int unit, int port,
                                          pm_info_t pm_info,
                                          int* value)
{

    return (SOC_E_NONE);
}

/*set modid field.*/
int pm8x50_port_mode_set(int unit, int port, pm_info_t pm_info,
                         const portmod_port_mode_info_t* mode)
{

    return (SOC_E_NONE);
}
int pm8x50_port_mode_get(int unit, int port, pm_info_t pm_info,
                         portmod_port_mode_info_t* mode)
{

    return (SOC_E_NONE);
}

/*set port encap.*/
int pm8x50_port_encap_set(int unit, int port, pm_info_t pm_info,
                          int flags, portmod_encap_t encap)
{

    return (SOC_E_NONE);
}
int pm8x50_port_encap_get(int unit, int port, pm_info_t pm_info,
                          int* flags, portmod_encap_t* encap)
{

    return (SOC_E_NONE);
}

/*set port register higig field.*/
int pm8x50_port_higig_mode_set(int unit, int port,
                               pm_info_t pm_info,
                               int mode)
{

    return (SOC_E_NONE);
}
int pm8x50_port_higig_mode_get(int unit, int port,
                               pm_info_t pm_info,
                               int* mode)
{

    return (SOC_E_NONE);
}

/*set port register higig field.*/
int pm8x50_port_higig2_mode_set(int unit, int port,
                                pm_info_t pm_info,
                                int mode)
{

    return (SOC_E_NONE);
}
int pm8x50_port_higig2_mode_get(int unit, int port,
                                pm_info_t pm_info,
                                int* mode)
{

    return (SOC_E_NONE);
}

/*set port register port type field.*/
int pm8x50_port_config_port_type_set(int unit, int port,
                                     pm_info_t pm_info,
                                     int type)
{

    return (SOC_E_NONE);
}
int pm8x50_port_config_port_type_get(int unit, int port,
                                     pm_info_t pm_info,
                                     int* type)
{

    return (SOC_E_NONE);
}

/*set/get hwfailover for trident.*/
int pm8x50_port_trunk_hwfailover_config_set(int unit, int port,
                                            pm_info_t pm_info,
                                            int hw_count)
{

    return (SOC_E_NONE);
}
int pm8x50_port_trunk_hwfailover_config_get(int unit, int port,
                                            pm_info_t pm_info,
                                            int* enable)
{

    return (SOC_E_NONE);
}

/*set/get hwfailover for trident.*/
int pm8x50_port_trunk_hwfailover_status_get(int unit, int port,
                                            pm_info_t pm_info,
                                            int* loopback)
{

    return (SOC_E_NONE);
}

/*set/get hwfailover for trident.*/
int pm8x50_port_diag_ctrl(int unit, int port, pm_info_t pm_info,
                          uint32 inst, int op_type, int op_cmd,
                          const void* arg)
{

    return (SOC_E_NONE);
}

/*Get/Set InterFrameGap Setting. */
int pm8x50_port_ifg_set(int unit, int port, pm_info_t pm_info, int speed,
                        soc_port_duplex_t duplex, int ifg, int* real_ifg)
{

    return (SOC_E_NONE);
}
int pm8x50_port_ifg_get(int unit, int port, pm_info_t pm_info, int speed,
                        soc_port_duplex_t duplex, int* ifg)
{

    return (SOC_E_NONE);
}

/*Get the reference clock value 156 or 125.*/
int pm8x50_port_ref_clk_get(int unit, int port,
                            pm_info_t pm_info,
                            int* ref_clk)
{

    return (SOC_E_NONE);
}

/*Disable lag failover.*/
int pm8x50_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

/*Disable lag failover.*/
int pm8x50_port_lag_remove_failover_lpbk_set(int unit, int port,
                                             pm_info_t pm_info, int val)
{

    return (SOC_E_NONE);
}
int pm8x50_port_lag_remove_failover_lpbk_get(int unit, int port,
                                             pm_info_t pm_info, int* val)
{

    return (SOC_E_NONE);
}

int pm8x50_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (cdmac_cntmaxsize_set(unit, port, val));
}

int pm8x50_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return (cdmac_cntmaxsize_get(unit, port, val));
}

/*Get Info needed to restore after drain cells.*/
int pm8x50_port_drain_cell_get(int unit, int port, pm_info_t pm_info,
                               portmod_drain_cells_t* drain_cells)
{

    return (SOC_E_NONE);
}

/*Restore informaation after drain cells.*/
int pm8x50_port_drain_cell_stop(int unit, int port, pm_info_t pm_info,
                                const portmod_drain_cells_t* drain_cells)
{

    return (SOC_E_NONE);
}

/*Restore informaation after drain cells.*/
int pm8x50_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_drain_cells_rx_enable(int unit, int port,
                                      pm_info_t pm_info,
                                      int rx_en)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_egress_queue_drain_rx_en(int unit, int port,
                                         pm_info_t pm_info,
                                         int rx_en)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_mac_ctrl_set(int unit, int port,
                             pm_info_t pm_info,
                             uint64 ctrl)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_txfifo_cell_cnt_get(int unit, int port,
                                    pm_info_t pm_info,
                                    uint32* cnt)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_egress_queue_drain_get(int unit, int port,
                                       pm_info_t pm_info,
                                       uint64* ctrl, int* rxen)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_mac_reset_set(int unit, int port, pm_info_t pm_info, int val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cdmac_soft_reset_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

/**/
int pm8x50_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{

    return (SOC_E_NONE);
}

/*Check if MAC needs to be reset.*/
int pm8x50_port_mac_reset_check(int unit, int port,
                                pm_info_t pm_info,
                                int enable, int* reset)
{
    return (cdmac_reset_check(unit, port, enable, reset));
}

/**/
int pm8x50_port_core_num_get(int unit, int port,
                             pm_info_t pm_info,
                             int* core_num)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_e2ecc_hdr_set(int unit, int port, pm_info_t pm_info,
                              const portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{

    return (SOC_E_NONE);
}
int pm8x50_port_e2ecc_hdr_get(int unit, int port, pm_info_t pm_info,
                              portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{

    return (SOC_E_NONE);
}

/**/
int pm8x50_port_e2e_enable_set(int unit, int port,
                               pm_info_t pm_info,
                               int enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_e2e_enable_get(int unit, int port,
                               pm_info_t pm_info,
                               int* enable)
{

    return (SOC_E_NONE);
}

/*get the speed for the specified port*/
int pm8x50_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{

    return (SOC_E_NONE);
}

/*TSC refere clock input and output set/get*/
int pm8x50_port_tsc_refclock_set(int unit, int port,
                                 pm_info_t pm_info,
                                 int ref_in, int ref_out)
{

    return (SOC_E_NONE);
}
int pm8x50_port_tsc_refclock_get(int unit, int port,
                                 pm_info_t pm_info,
                                 int* ref_in, int* ref_out)
{

    return (SOC_E_NONE);
}

/*Port discard set*/
int pm8x50_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{

    return (SOC_E_NONE);
}

/*Port soft reset set set*/
int pm8x50_port_soft_reset_set(int unit, int port, pm_info_t pm_info,
                               int idx, int val, int flags)
{

    return (SOC_E_NONE);
}

/*Port tx_en=0 and softreset mac*/
int pm8x50_port_tx_down(int unit, int port, pm_info_t pm_info)
{

    return (SOC_E_NONE);
}

/*reconfig pgw.*/
int pm8x50_port_pgw_reconfig(int unit, int port, pm_info_t pm_info,
                             const portmod_port_mode_info_t* pmode,
                             int phy_port, int flags)
{

    return (SOC_E_NONE);
}

/*Routine to notify internal phy of external phy link state.*/
int pm8x50_port_notify(int unit, int port, pm_info_t pm_info, int link)
{

    return (SOC_E_NONE);
}

/*"port control phy timesync config set/get"*/
int pm8x50_port_control_phy_timesync_set(int unit,
                               int port, pm_info_t pm_info,
                               portmod_port_control_phy_timesync_t config,
                               uint64 value)
{

    return (SOC_E_NONE);
}
int pm8x50_port_control_phy_timesync_get(int unit,
                              int port, pm_info_t pm_info,
                              portmod_port_control_phy_timesync_t config,
                              uint64* value)
{

    return (SOC_E_NONE);
}

/*"port timesync config set/get"*/
int pm8x50_port_timesync_config_set(int unit, int port, pm_info_t pm_info,
                              const portmod_phy_timesync_config_t* config)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_config_get(int unit, int port, pm_info_t pm_info,
                                    portmod_phy_timesync_config_t* config)
{

    return (SOC_E_NONE);
}

/*"port timesync enable set/get"*/
int pm8x50_port_timesync_enable_set(int unit, int port,
                                    pm_info_t pm_info,
                                    uint32 enable)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_enable_get(int unit, int port,
                                    pm_info_t pm_info,
                                     uint32* enable)
{

    return (SOC_E_NONE);
}

/*"port timesync nco addend  set/get"*/
int pm8x50_port_timesync_nco_addend_set(int unit, int port,
                                        pm_info_t pm_info,
                                        uint32 freq_step)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_nco_addend_get(int unit, int port,
                                        pm_info_t pm_info,
                                        uint32* freq_step)
{

    return (SOC_E_NONE);
}

/*"port timesync framesync info  set/get"*/
int pm8x50_port_timesync_framesync_mode_set(int unit,
                                int port, pm_info_t pm_info,
                                const portmod_timesync_framesync_t* framesync)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_framesync_mode_get(int unit,
                                     int port, pm_info_t pm_info,
                                     portmod_timesync_framesync_t* framesync)
{

    return (SOC_E_NONE);
}

/*"port timesync local time  set/get"*/
int pm8x50_port_timesync_local_time_set(int unit, int port,
                                        pm_info_t pm_info,
                                        uint64 local_time)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_local_time_get(int unit, int port,
                                        pm_info_t pm_info,
                                        uint64* local_time)
{

    return (SOC_E_NONE);
}

/*"port timesync framesync info  set/get"*/
int pm8x50_port_timesync_load_ctrl_set(int unit, int port, pm_info_t pm_info,
                                       uint32 load_once, uint32 load_always)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_load_ctrl_get(int unit, int port, pm_info_t pm_info,
                                       uint32* load_once, uint32* load_always)
{

    return (SOC_E_NONE);
}

/*"port timesync tx timestamp offset set/get"*/
int pm8x50_port_timesync_tx_timestamp_offset_set(int unit, int port,
                                                 pm_info_t pm_info,
                                                 uint32 ts_offset)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_tx_timestamp_offset_get(int unit, int port,
                                                 pm_info_t pm_info,
                                                 uint32* ts_offset)
{

    return (SOC_E_NONE);
}

/*"port timesync rx timestamp offset set/get"*/
int pm8x50_port_timesync_rx_timestamp_offset_set(int unit, int port,
                                                 pm_info_t pm_info,
                                                 uint32 ts_offset)
{

    return (SOC_E_NONE);
}
int pm8x50_port_timesync_rx_timestamp_offset_get(int unit, int port,
                                                 pm_info_t pm_info,
                                                 uint32* ts_offset)
{

    return (SOC_E_NONE);
}

/*set/get interrupt enable value. */
int pm8x50_port_interrupt_enable_set(int unit, int port,
                                     pm_info_t pm_info,
                                     int intr_type, uint32 val)
{

    return (SOC_E_NONE);
}
int pm8x50_port_interrupt_enable_get(int unit, int port,
                                     pm_info_t pm_info,
                                     int intr_type, uint32* val)
{

    return (SOC_E_NONE);
}

/*get interrupt status value. */
int pm8x50_port_interrupt_get(int unit, int port, pm_info_t pm_info, int intr_type, uint32* val)
{

    return (SOC_E_NONE);
}

/*get interrupt value array. */
int pm8x50_port_interrupts_get(int unit, int port, pm_info_t pm_info,
                               int arr_max_size, uint32* intr_arr,
                               uint32* size)
{

    return (SOC_E_NONE);
}

/* portmod check if external phy is legacy*/
int pm8x50_port_check_legacy_phy(int unit, int port,
                                 pm_info_t pm_info,
                                 int* legacy_phy)
{

    return (SOC_E_NONE);
}

/* portmod phy failover mode*/
int pm8x50_port_failover_mode_set(int unit, int port,
                                  pm_info_t pm_info,
                                  phymod_failover_mode_t failover)
{

    return (SOC_E_NONE);
}
int pm8x50_port_failover_mode_get(int unit, int port,
                                  pm_info_t pm_info,
                                  phymod_failover_mode_t* failover)
{

    return (SOC_E_NONE);
}

/* portmod port rsv mask set*/
int pm8x50_port_mac_rsv_mask_set(int unit, int port,
                                 pm_info_t pm_info,
                                 uint32 rsv_mask)
{

    return (SOC_E_NONE);
}

/* portmod port mib reset toggle*/
int pm8x50_port_mib_reset_toggle(int unit, int port,
                                 pm_info_t pm_info,
                                 int port_index)
{

    return (SOC_E_NONE);
}

/* portmod restore information after warmboot*/
int pm8x50_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info,
                        const portmod_port_interface_config_t* intf_config,
                        const portmod_port_init_config_t* init_config,
                        phymod_operation_mode_t phy_op_mode)
{

    return (SOC_E_NONE);
}

/* portmod port flow control config*/
int pm8x50_port_flow_control_set(int unit, int port,
                                 pm_info_t pm_info,
                                 int merge_mode_en,
                                 int parallel_fc_en)
{

    return (SOC_E_NONE);
}

/*Portmod state for any logical port dynamixc settings*/
int pm8x50_port_update_dynamic_state(int unit, int port,
                                     pm_info_t pm_info,
                                     uint32_t port_dynamic_state)
{

    return (SOC_E_NONE);
}

/*get phy operation mode. */
int pm8x50_port_phy_op_mode_get(int unit, int port, pm_info_t pm_info,
                                phymod_operation_mode_t* val)
{

    return (SOC_E_NONE);
}

/*Returns if the PortMacro associated with the port is initialized or not*/
int pm8x50_pm_is_initialized(int unit, pm_info_t pm_info, int pport,
                             int lport, int* is_initialized)
{
    int is_core_initialized = 0, rv;
     rv = PM8x50_IS_CORE_INITIALIZED_GET(unit, pm_info, &is_core_initialized);
     *is_initialized = is_core_initialized;

    return rv;
}
#endif /*_PM8X50_H_*/
